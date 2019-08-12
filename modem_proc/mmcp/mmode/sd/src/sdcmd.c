/*===========================================================================
          ONLY INTERNAL FUNCTIONS TO sd.c

GENERAL DESCRIPTION

SD 3.0 marks the move from SD (System Determination) as a library called from
the various components that use it to a task. This change stems from the more
highly multi-threaded environment in which MMODE now runs - multiple
simultaneous access to the SD database jeapordizes the integrity of that
database. Attempts were made to handle this with critical sections, but there
is just too much going on inside SD to safely take that approach.

Instead, SD is now its own task, and protects access to its database by
dispatching commands for each call of a frontend API. However, command
dispatch is an inherently asynchronous process. Changing SD's APIs so that
they were asynchronous was not an option, as this would have required major
rewrites of all SD clients. Instead, the APIs exported by SD still behave as
if they are synchronous, but instead of each API running to completion and
then returning when it is called, the APIs now queue a command and then block
the calling thread on a signal associated with that command until the SD task
has executed the command. Only when the command has completed execution does
the SD task set its associated signal, which unblocks the thread that called
the API, allowing it to return to the caller.

As a result of these changes, adding a frontend API to SD is now done
differently than in the past - you can no longer just write a function and
export the API. Instead, the actual API function needs to post a command to
the SD task and then block on a signal, waiting for command completion.

Things to watch out for:
========================
NEVER CALL A FRONTEND API FROM WITHIN THE FUNCTION THAT EXECUTES THAT API!!!

These functions are run on the SD task thread; calling a frontend API from
such a function will create a command, queue it to the SD command queue, and
then block the SD task thread waiting for the command execution to complete.
The SD task thread is now deadlocked, because it can't run to execute that
newly queued command. SD will ERR_FATAL if a command is queued from the SD
task thread, so if there is a unit test that exercises the code path you will
find out when you run SUTE.

http://qwiki.qualcomm.com/qct-multimode/SD_3.0


  Copyright (c) 2000 - 2015 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.


             EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdcmd.c#2 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/14   sm      Use common lte cs capbaility for client update
12/12/13   xs      In case LTE removed from mode pref sdss_get_ss_for_mode can't
                   be used to distinguish SV vs. non-SV
11/10/13   xs      Make VOLTE orig to SD backend
10/30/13   xs      plmn_allow handling for medium preferred sys
10/25/13   jvo     Replaced banned call to memcpy with assignment in
                   sdcmd_gw_opr_operator_capability_changed()
10/24/13   jvo     Added header for version tracking above
09/12/13   sm      check for feature mode before skipping hybr2 user pref 
09/06/13   xs      Reload EFS/NV during LPM->ONLINE for ALT_CDMA_BSR feature
08/28/13   jvo     Cleanup remaining Fusion pieces
07/19/13   jvo     Remove all MDM/MSM Fusion features
07/15/13   ss      FR1860 - T/G on DSDS
06/24/13   jh      Repopulate HYBR2 PREF_LST for sys_lost from Limited Regional
06/21/13   dk      added general description for the file, markers to separate 
                   functions, and copyright statement
06/21/13   gm      Selectively clear SDSR lists/systems and engine stack
06/19/13   gm      Send pref to SD-ALL stacks before ONLINE/PWRUP ind
04/19/13   gm      Process user pref in LPM/PWROFF
05/07/13   sm      Skip Script if HYBR2 in OPR State during stack sync up  

============================================================================*/ 

#include "sdcmd.h"
#include "sdefs.h"    /* SD EFS interface */
#include "cm_v.h"

#ifdef FEATURE_ECALL_IVS
#include "ecall_modem_apis.h"
#endif

#include <stringl/stringl.h>
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif

EXTERN  boolean     sdss_eng_stack_is_acq_type(

        sd_ss_e_type            ss,
            /* System selection stack.
            *//*lint -esym( 715, ss ) */ /* Symbol is not referenced.*/
        sdssscr_acq_e_type      acq_type
);

EXTERN void sdss_update_cdma_activity(

        sd_ss_e_type                   ss,
          /* System selection stack
          */
       
        boolean                        is_cdma_activity

);

#ifdef FEATURE_HDR

/*===========================================================================

FUNCTION sd_ss_update_chan_list

DESCRIPTION
  Store the OTA hashing channel information into channel list with following
  restrictions.

  If list is provided, store only the channel existing in the list.

  If acq_sys is provided, order the channel list according to the acq_sys.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
void                        sd_ss_chan_list_update(

        sdsr_e_type                 chan_list,
           /* The output list to store the channel list.
           */

        sd_mode_e_type              mode,
           /* The system mode of the chan list.
           */

        const sys_channel_type      *chan_ptr,
           /* Pointer to HDR hashing channel records.
           */

        word                        num_chan,
           /* The number of HDR hashing channel records specified in
           ** chan_ptr
           */

        sdsr_e_type                 curr_sys,
           /* Current system information used if channel ptr is null.
           */

        const sd_sid2_u_type        *sid_ptr,
             /* pointer to ID.
             */

        sdsr_e_type                 list,
           /* Store the channels exists in the list if provided.
           */

        sdsr_e_type                 acq_sys,
           /* Order channel list according to the acq system if provided.
           */

        sdsr_e_type                 srv_sys
           /* Order channel list according to the srv system if provided.
           */
)
{

  word            i;
  sdsr_ref_type   sr_ref;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( chan_list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( curr_sys, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( acq_sys, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( srv_sys, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clearing the list
  */
  sdsr_list_clr( chan_list );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If channel list is empty, copy the current system to the channle list.
  */
  if( chan_ptr == NULL || num_chan == 0 )
  {
    sdsr_list_cpy( chan_list, curr_sys );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update HDR chan list per input hashing channel records.
  */
  for( i = 0; i < num_chan; i++ )
  {
    SD_ASSERT( &chan_ptr[i] != NULL );

    if( &chan_ptr[i] == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Stored the channel list to the sdsr database and append to the channel
    ** list and if the input list is provided, store only those within the
    ** input list.
    */
    if( list == SDSR_MAX ||
        (sdsr_list_find_sys( list,
                             mode,
                             chan_ptr[i].band,
                             chan_ptr[i].chan_num ) != SDSR_POS_NULL) )
    {
      /* Store the channel list to SDSR table
      */
      sr_ref = sdsr_tbl_store10( mode,
                                 chan_ptr[i].band,
                                 chan_ptr[i].chan_num,
                                 sid_ptr );

      /* Append to the channel list.
      */
      sdsr_list_app_ref( chan_list, sr_ref );
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Order chan list according to the ACQ_SYS if it is provided.
  */
  if( acq_sys != SDSR_MAX )
  {
    sdsr_list_order( chan_list, acq_sys );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Order channel list according to band class of serving system.
  ** This is because if band class is different the RF condition is
  ** different.
  */
  if( srv_sys != SDSR_MAX )
  {
    sdsr_list_order2( chan_list, srv_sys, SDSR_ORDER_BAND );
    sdsr_list_order( chan_list, srv_sys );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_2( "chan_list cnt=%d mode=%d",
               sdsr_list_cnt( chan_list ),
               mode);

  return;

}/* sd_ss_update_chan_list */


/*===========================================================================

FUNCTION sd_ss_hdr_srv_sys_update

DESCRIPTION

DEPENDENCIES

RETURN VALUE  
 
SIDE EFFECTS 

===========================================================================*/
void                          sd_ss_hdr_srv_sys_update(

        sdsr_e_type                   sys,
            /* System per which to update the HDR serving system.
            ** Ignored if set to SDSR_MAX.
            */

        sdsr_e_type                   geo_list,
            /* List per which to update the HDR serving system's GEO.
            */

        sdsr_e_type                   pref_list,
            /* List per which to update the HDR serving system's more
            ** preferred list.
            */

        sdsr_e_type                   colloc_list
            /* List per which to update the serving system's collocated list.
            */
)
{
  SD_ASSERT( BETWEEN(sys, SDSR_1ST_SYS, SDSR_MAX) || sys == SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( geo_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE( pref_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_list, SDSR_1ST_SYS, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the serving system, its GEO, and its more preferred lists per the
  ** input lists.
  */
  if( sys != SDSR_MAX )
  {
    sdsr_list_cpy( SDSR_HDR_SRV_SYS, sys );
  }

  sdsr_list_cpy( SDSR_HDR_SRV_GEO_LST,  geo_list );
  sdsr_list_cpy( SDSR_HDR_SRV_PREF_LST, pref_list );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If needed, update the serving systems collocated list.
  */
  if( colloc_list != SDSR_MAX )
  {
    sdsr_list_cpy( SDSR_HDR_SRV_COLLOC_LST, colloc_list );
  }

} /* sd_ss_hdr_srv_sys_update */


#endif /* FEATURE_HDR */


#ifdef SD_USES_CAI
/*===========================================================================

FUNCTION sd_ss_return_cause_update

DESCRIPTION
  Update the return_cause indicator per the input SS-Event, redirection
  status, last and current systems attempted acquisition.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void                  sd_ss_return_cause_update(

        sd_ss_e_type                  ss,
            /* The ss for which the return cause is to be returned
            */

        sdss_evt_e_type               event,
            /* Event that is being processed.
            */

        boolean                       is_disabling_redir,
            /* Indicates whether redirection is being disabled.
            */

        const sdsr_s_type             *last_acq_sys_ptr
            /* Pointer to system record that was attempted acquisition last.
            */
)
{
  byte        return_cause = 0xff;

  #ifndef FEATURE_MMODE_REMOVE_1X
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX )
  SD_ASSERT( last_acq_sys_ptr != NULL )
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the return_cause indicator per the input SS-Event, and
  ** redirection status.
  */
  switch( event )
  {   /* enum constant 'sdss_evt_e_type' not used within defaulted switch */
    /* SS-Events that are a result of User Indications.
    */

    case SDSS_EVT_USER_PWR_UP:
    case SDSS_EVT_USER_OFFLINE_CDMA:
    case SDSS_EVT_USER_OFFLINE_AMPS:
    case SDSS_EVT_USER_PWR_SAVE_EXIT:
    case SDSS_EVT_USER_NAM_SEL:
    case SDSS_EVT_USER_PWR_DOWN:
    case SDSS_EVT_HYBR_2_USER_PWR_UP:
    case SDSS_EVT_HYBR_2_USER_OFFLINE_CDMA:
    case SDSS_EVT_HYBR_2_USER_PWR_SAVE_EXIT:
    case SDSS_EVT_HYBR_2_USER_NAM_SEL:
    case SDSS_EVT_HYBR_2_USER_PWR_DOWN:
    case SDSS_EVT_HYBR_3_USER_PWR_UP:
    case SDSS_EVT_HYBR_3_USER_OFFLINE_CDMA:
    case SDSS_EVT_HYBR_3_USER_PWR_SAVE_EXIT:
    case SDSS_EVT_HYBR_3_USER_NAM_SEL:
    case SDSS_EVT_HYBR_3_USER_PWR_DOWN:
      return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    case SDSS_EVT_USER_SS_PREF:
    case SDSS_EVT_HYBR_2_USER_SS_PREF:
    case SDSS_EVT_HYBR_3_USER_SS_PREF:
    case SDSS_EVT_INT_SRV_LOST:
    case SDSS_EVT_USER_GET_NET_GW:
    case SDSS_EVT_USER_TERM_GET_NET_GW:
    case SDSS_EVT_USER_PROT_DEACTIVATE:
    case SDSS_EVT_HYBR_2_USER_GET_NET_GW:
    case SDSS_EVT_HYBR_2_USER_TERM_GET_NET_GW:
    case SDSS_EVT_HYBR_2_USER_PROT_DEACTIVATE:
    case SDSS_EVT_HYBR_3_USER_GET_NET_GW:
    case SDSS_EVT_HYBR_3_USER_TERM_GET_NET_GW:
    case SDSS_EVT_HYBR_3_USER_PROT_DEACTIVATE:

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of CDMA Acquisition Indications.
    */

    case SDSS_EVT_CDMA_ACQ_FAILED_PILOT:
    case SDSS_EVT_CDMA_ACQ_FAILED_SYNC:
      if( is_disabling_redir ) {
        return_cause = CAI_SYSTEM_NOT_ACQ_RETURN;
      }
      break;

    case SDSS_EVT_CDMA_ACQ_PROT_MIS:
      if( is_disabling_redir ) {
        return_cause = CAI_PROTOCOL_MISMATCH_RETURN;
      }
      break;

    case SDSS_EVT_CDMA_ACQ_SCHM:
      if( is_disabling_redir )
      {
        /* If expected NID of last acquired system is not equal to
        ** SD_WILDCARD_NID or to obtained NID, set return_cause to
        ** CAI_WRONG_NID_RETURN.
        */
        if( last_acq_sys_ptr->sid_expected.prm.is95.nid != SD_WILDCARD_NID &&
            last_acq_sys_ptr->sid.is95.nid != last_acq_sys_ptr->sid_expected.prm.is95.nid)
        {
          return_cause = CAI_WRONG_NID_RETURN;
        }

        /* If expected SID of last acquired system is not equal to
        ** SD_WILDCARD_SID or to obtained SID, set return_cause to
        ** CAI_WRONG_SID_RETURN.
        */
        if( last_acq_sys_ptr->sid_expected.prm.is95.sid != SD_WILDCARD_SID &&
            last_acq_sys_ptr->sid.is95.sid != last_acq_sys_ptr->sid_expected.prm.is95.sid)
        {
          return_cause = CAI_WRONG_SID_RETURN;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_CDMA_ACQ_HO_FAIL:
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of CDMA Operation Indications.
    */

    case SDSS_EVT_CDMA_OPR_OH_SID:
    case SDSS_EVT_CDMA_OPR_OH_CHAN:
    case SDSS_EVT_CDMA_OPR_RESCAN:
      break;

    case SDSS_EVT_CDMA_OPR_REG_REJ:
      if( is_disabling_redir ) {
        return_cause = CAI_REG_REJ_RETURN;
      }
      break;

    case SDSS_EVT_CDMA_OPR_SYS_LOST:
    case SDSS_EVT_CDMA_OPR_REDIR:
    case SDSS_EVT_CDMA_OPR_ACPT_REJ:
    case SDSS_EVT_CDMA_OPR_NDSS_OFF:
    case SDSS_EVT_CDMA_OPR_NEW_SYS:
    case SDSS_EVT_CDMA_OPR_SYS_RESEL:
    case SDSS_EVT_CDMA_OPR_LOCK:
    case SDSS_EVT_CDMA_OPR_OPERATOR_CAPABILITY_CHANGED:
      return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    case SDSS_EVT_CDMA_OPR_UNLOCK:
    case SDSS_EVT_CDMA_OPR_ACC_TERM:
    case SDSS_EVT_CDMA_OPR_CAM_ERR:
    case SDSS_EVT_CDMA_OPR_CAM:
    case SDSS_EVT_CDMA_OPR_HDM:
    case SDSS_EVT_CDMA_OPR_ITSPM:
      break;

    case SDSS_EVT_CDMA_OPR_PROT_MIS:
      return_cause = CAI_PROTOCOL_MISMATCH_RETURN;
      break;

    case SDSS_EVT_CDMA_OPR_RELEASE:
      return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of AMPS Acquisition Indications.
    */

    case SDSS_EVT_AMPS_ACQ_FAILED_CCH:
      if( is_disabling_redir ) {
        return_cause = CAI_SYSTEM_NOT_ACQ_RETURN;
      }
      break;

    case SDSS_EVT_AMPS_ACQ_CCH_SID:
      if( is_disabling_redir )
      {
        /* If expected SID of last acquired system is not equal to
        ** SD_WILDCARD_SID or to obtained SID, set return_cause to
        ** CAI_WRONG_SID_RETURN.
        */
        if( last_acq_sys_ptr->sid_expected.prm.is95.sid != SD_WILDCARD_SID &&
            last_acq_sys_ptr->sid.is95.sid != last_acq_sys_ptr->sid_expected.prm.is95.sid)
        {
          return_cause = CAI_WRONG_SID_RETURN;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of AMPS Operation Indications.
    */

    case SDSS_EVT_AMPS_OPR_FAILED_PCH:
      if( is_disabling_redir ) {
        return_cause = CAI_SYSTEM_NOT_ACQ_RETURN;
      }
      break;

    case SDSS_EVT_AMPS_OPR_PCH_SID:
      if( is_disabling_redir )
      {
        /* If expected SID of last acquired system is not equal to
        ** SD_WILDCARD_SID or to obtained SID, set return_cause to
        ** CAI_WRONG_SID_RETURN.
        */
        if( last_acq_sys_ptr->sid_expected.prm.is95.sid != SD_WILDCARD_SID &&
            last_acq_sys_ptr->sid.is95.sid != last_acq_sys_ptr->sid_expected.prm.is95.sid)
        {
          return_cause = CAI_WRONG_SID_RETURN;
        }
      }
      break;

    case SDSS_EVT_AMPS_OPR_ACC_SID:
      break;

    case SDSS_EVT_AMPS_OPR_RESCAN:
    case SDSS_EVT_AMPS_OPR_SYS_LOST:
    case SDSS_EVT_AMPS_OPR_CDMA_AVAIL:
      return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    case SDSS_EVT_AMPS_OPR_ACC_TERM:
    case SDSS_EVT_AMPS_OPR_CAM:
      break;

    case SDSS_EVT_AMPS_OPR_OVERLOAD:
      return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of HDR User Indications.
    */
    case SDSS_EVT_HDR_USER_SS_PREF:
    case SDSS_EVT_HDR_USER_HYBR_START:
    case SDSS_EVT_HDR_USER_HYBR_END:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of HDR Acquisition Indications.
    */
    case SDSS_EVT_HDR_ACQ_FAILED_PILOT:
    case SDSS_EVT_HDR_ACQ_FAILED_SYNC:
    case SDSS_EVT_HDR_ACQ_PROT_MIS:
    case SDSS_EVT_HDR_ACQ_BAD_SYS:
    case SDSS_EVT_HDR_ACQ_SID:
      return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;


    /* SS-Events that are a result of 1X/HDR redirection/redirection abort
    ** Indications.
    */
    case SDSS_EVT_HDR_USER_REDIR_LTE:
    case SDSS_EVT_HDR_ACQ_REDIR_ABORT:
    case SDSS_EVT_USER_CDMA_REDIR_LTE:
    case SDSS_EVT_USER_CDMA_HO_LTE:
    case SDSS_EVT_CDMA_ACQ_REDIR_ABORT:
    case SDSS_EVT_USER_CDMA_REDIR_ACPT_REJ:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of HDR Operation Indications.
    */

    case SDSS_EVT_HDR_OPR_OH_SID:
    case SDSS_EVT_HDR_OPR_OH_CHAN:
    case SDSS_EVT_HDR_OPR_REDIR:
    case SDSS_EVT_HDR_OPR_SYS_LOST:
    case SDSS_EVT_HDR_OPR_RESEL_OK:
    case SDSS_EVT_HDR_OPR_ACC_TERM:
    case SDSS_EVT_HDR_OPR_SES_ABORT:
    case SDSS_EVT_HDR_OPR_SES_CLOSE:
    case SDSS_EVT_HDR_OPR_SES_NEGO_TIMEOUT:
    case SDSS_EVT_HDR_OPR_RELEASE:
    case SDSS_EVT_HDR_OPR_END_BCMCS:
      return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    case SDSS_EVT_HDR_OPR_PROT_MIS:
      return_cause = CAI_PROTOCOL_MISMATCH_RETURN;
      break;

    case SDSS_EVT_HDR_OPR_BAD_SYS:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
    /* SS-Events that are a result of GWL Acquisition Indications.
    */
    case SDSS_EVT_GWL_ACQ_PLMN_FAILED:
    case SDSS_EVT_GWL_ACQ_PLMN_SUCCESS:
    case SDSS_EVT_HYBR_2_GW_ACQ_PLMN_SUCCESS:
    case SDSS_EVT_HYBR_2_GW_ACQ_PLMN_FAILED:
    case SDSS_EVT_HYBR_3_GW_ACQ_PLMN_SUCCESS:
    case SDSS_EVT_HYBR_3_GW_ACQ_PLMN_FAILED:

    case SDSS_EVT_GWL_ACQ_PLMN_SUCCESS_LESS_PREF:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of GW Operation Indications.
    */

    case SDSS_EVT_GWL_OPR_SRV_INFO:
    case SDSS_EVT_GWL_OPR_SYS_LOST:
    case SDSS_EVT_HYBR_2_GW_OPR_SRV_INFO:
    case SDSS_EVT_HYBR_2_GW_OPR_SYS_LOST:
    case SDSS_EVT_HYBR_3_GW_OPR_SRV_INFO:
    case SDSS_EVT_HYBR_3_GW_OPR_SYS_LOST:
    case SDSS_EVT_GWL_OPR_ACC_DONE:
    case SDSS_EVT_HYBR_2_GW_OPR_ACC_DONE:
    case SDSS_EVT_HYBR_3_GW_OPR_ACC_DONE:
    case SDSS_EVT_GWL_OPR_OPERATOR_CAPABILITY_CHANGED:
      break;
    #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED || FEATURE_SD_LTE*/


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* SS-Events that are a result of Miscellaneous Indications.
    */

    case SDSS_EVT_MISC_TIMER_RESEL_OK:
    case SDSS_EVT_MISC_TIMER_RESEL_NO:
    case SDSS_EVT_MISC_MEAS_RPRT:
    case SDSS_EVT_MISC_ORIG_SUCCESS:
    case SDSS_EVT_MISC_IRAT_TO_GWL:
    case SDSS_EVT_MISC_GW_SIM_STATE_UPDATE:
    case SDSS_EVT_MISC_LTE_TO_HDR_BSR:
      break;

    case SDSS_EVT_MISC_PWR_SAVE_ENTER:
      return_cause = CAI_NORMAL_ACCESS_RETURN;
      break;

    case SDSS_EVT_MISC_FILLER:
    case SDSS_EVT_MISC_GW_NET_LIST:
    case SDSS_EVT_MISC_CNF:
      break;

    case SDSS_EVT_MISC_LOCAL_DEACTIVATE:
    case SDSS_EVT_MISC_LOCAL_ACTIVATE:
      break; /* Cat/Qin Pls review this */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are internally generated by the SS-Core.
    */
    case SDSS_EVT_INT_RESEL:
      SD_ERR_0( "ss_event");
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_MAX:
    default:
      SD_ERR_0( "ss_event");
      break;

  } /* switch */

  /* If valid return cause, update to sd_ss.
  */
  if( return_cause != 0xff )
  {
    sd_ss_ptr(ss)->return_cause = return_cause;
  }
  #else
  #error code not present
#endif

} /* sd_ss_return_cause_update */
#endif  /* defined(SD_USES_CAI) */


/*===========================================================================

FUNCTION sd_ss_acq_update_post

DESCRIPTION
  Does post acquisition attempt updates.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sd_ss_acq_update_post(

        sd_ss_e_type                  ss,
            /* The SS for which the acq is to be updated */

        sdsr_e_type                   list,
            /* List from which acquisition was attempted */
        int                           pos,
            /* List position of system that was attempted acquisition */

        sdss_acq_mode_e_type          acq_mode
            /* The acquisition mode that was attempted */
)
{
  sdsr_ref_type         sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to the system that was attempted acquisition.
  */
  sr_ref = sdsr_list_get_ref( list, pos );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the last full, limited systems that contain the same
  ** bands as that in the acq system.
  */
  sdsr_list_update_post_gwl_sys( ss, sr_ref, TRUE, FALSE, FALSE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If origination mode is emergency (origination or callback) and list is
  ** other than SCHED_LST (i.e., this acquisition attempt was not due to
  ** reacquisition schedule) or GWL LST (used during emerg redial on PS),
     update the EMERG_SYS per the system record
  ** that was attempted acquisition.
  */
  
  if( sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_EMERG) &&
     list != sdss_sr_list_map2(ss,SDSR_SCHED_LST) &&
	 ! (cmregprx_check_lte_priority_scan(ss) && ((list ==  sdss_sr_list_map2(ss,SDSR_GWL_LTD_LST)) ||
     (list ==  sdss_sr_list_map2(ss,SDSR_MMSS_GWL_LST))))
		 
	)
  {
    sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_EMERG_SYS), sr_ref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* According to the acquisition mode that was attempted, update
  ** the appropriate SDSR_ACQ_xxx_SYS system.
  */
  switch( acq_mode )
  {
    case SDSS_ACQ_MODE_FULL:
    case SDSS_ACQ_MODE_HO:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_FULL_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MINI:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_MINI_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MICRO:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_MICRO_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_FULL2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_FULL2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MINI2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_MINI2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MICRO2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_MICRO2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_DEEP:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_DEEP_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_DEEP2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_DEEP2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_SHALLOW:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_SHALLOW_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_SHALLOW2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_SHALLOW2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MICRO_ABSR:
      sdsr_list_put_ref( SDSR_ACQ_MICRO_ABSR_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_FULL_AOOS:
      sdsr_list_put_ref( SDSR_ACQ_FULL_AOOS_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_FULL_OSR:
      sdsr_list_put_ref( SDSR_ACQ_FULL_OSR_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_EOOS_FULL:
    case SDSS_ACQ_MODE_EOOS_FULL_RTN:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_EOOS_FULL_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_EOOS_FULL2:
    case SDSS_ACQ_MODE_EOOS_FULL_RTN2:
      sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_FULL2_SYS), sr_ref );
      break;

    case SDSS_ACQ_MODE_MAX:
    case SDSS_ACQ_MODE_PREV:
    default:
      SD_ERR_0( "acq_mode");
      break;

  } /* switch */

} /* sd_ss_acq_update_post */


/*===========================================================================

FUNCTION sd_si_pref_srv_acq

DESCRIPTION
  Notify the registered client (i.e., CM) of preferred service acquisition
  indicator changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_si_pref_srv_acq(
        sd_ss_e_type              ss
            /* System selection - MAIN or HDR */
)
{
  sd_si_info_s_type     *si_info_ptr = &sd_si_ptr(ss)->si_info;

  /* Get the current SS-State.
  */
  sdss_state_e_type     ss_state    = sdss_get_ss_state(ss);

  boolean               is_pref_srv_acq;
  boolean               is_stable_in_svc;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss_state, SDSS_STATE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out whether system determination is still attempting to acquire
  ** a more preferred service.
  */
  if( ss_state == SDSS_STATE_OPR ||
      sdss_is_resel_state(ss, SDSS_RESEL_STT_SRCH) )
  {
    is_pref_srv_acq = FALSE;
  }
  else
  {
    is_pref_srv_acq = TRUE;
  }

  if( ss_state == SDSS_STATE_OPR )
  {
    is_stable_in_svc = TRUE;
  }
  else
  {
    is_stable_in_svc = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* No need to inform the registered client of change if service is
  ** currently turned OFF or if preferred service acquisition indicator is
  ** not changed.
  */
  if( !sys_srv_status_is_srv( si_info_ptr->srv_status)  ||
      (si_info_ptr->is_pref_srv_acq == is_pref_srv_acq)
      #ifdef FEATURE_SD_LTE
      || (sdss_is_lte_connected_mode(ss) == TRUE)
      #endif
    #if defined(FEATURE_LTE_TO_1X)
      || ((sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG) ||
       (sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG)
      )
      #endif
    )
  {
    if(((sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG) ||
       (sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG)
      ) || (sdss_is_lte_connected_mode(ss) == TRUE))
    {
      SD_MSG_HIGH_2( "CSFB ORIG mode %d lte connected mode %d",\
                 sdss_get_orig_mode(ss), sdss_is_lte_connected_mode(ss) );
    }
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the preferred service acquisition indicator.
  */
  si_info_ptr->is_pref_srv_acq   = is_pref_srv_acq;
  si_info_ptr->is_stable_in_svc  = is_stable_in_svc;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (e.g., CM) of the service indicators
  ** change.
  */
  SD_MSG_HIGH_2( "Pref srv acq change=%d ss %d", is_pref_srv_acq, ss);
  sd_invoke_info_callback( ss );

  /* Reset the realign_hdr & realign_lte flag
  */
  si_info_ptr->realign_hdr  = FALSE;
  si_info_ptr->realign_lte = FALSE;

} /* sd_si_pref_srv_acq */


/*===========================================================================

FUNCTION sd_ss_acq_update_pre

DESCRIPTION
  Does pre acquisition attempt updates.The 3 fields are written together under
  critical section to ensure a valid set of values.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sd_ss_acq_update_pre(

        sd_ss_e_type                  ss,
            /* The SS for which the acq is to be updated */

        sdsr_e_type                   list,
            /* List from which acquisition is attempted */
        int                           pos,
            /* List position of system that is attempted acquisition */

        sdss_acq_mode_e_type          acq_mode
            /* The acquisition mode that is attempted */
)
{
  sd_ss_s_type          *ss_ptr               = NULL;

  sdsr_ref_type         sr_ref;
  #ifdef SD_USES_DB_SERVICES
  db_items_value_type   db_item;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to the system that is attempted acquisition.
  */
  sr_ref = sdsr_list_get_ref( list, pos );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SDSR_ACQ_SYS per the system record that is attempted
  ** acquisition.
  */
  sdsr_list_put_ref( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), sr_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_list_print(sdss_sr_list_map2(ss,SDSR_ACQ_SYS), TRUE);


  /* Update the acquisition list, position and mode.
  */
  SD_CRIT_SECT_ENTER();
  ss_ptr = sd_ss_ptr(ss);
  ss_ptr->acq_list  = list;
  ss_ptr->acq_pos   = pos;
  ss_ptr->acq_mode  = acq_mode;
  SD_CRIT_SECT_EXIT();

  SD_MSG_HIGH_3("SD_SS:FRFL: Updated Acq params list=%d, pos=%d, mode=%d",
                                                         list,pos,acq_mode);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert SD acquisition mode to DB acquisition mode */
  #ifdef SD_USES_DB_SERVICES
  if( ss == SD_SS_MAIN )
  {
    switch( acq_mode )
    {
      case SDSS_ACQ_MODE_FULL:
      case SDSS_ACQ_MODE_FULL2:
      case SDSS_ACQ_MODE_FULL_AOOS:
      case SDSS_ACQ_MODE_FULL_OSR:
      case SDSS_ACQ_MODE_HO:
      case SDSS_ACQ_MODE_EOOS_FULL:
      case SDSS_ACQ_MODE_EOOS_FULL2:
      case SDSS_ACQ_MODE_EOOS_FULL_RTN:
      case SDSS_ACQ_MODE_EOOS_FULL_RTN2:
        db_item.acq_mode = DB_ACQ_FULL;
        break;

      case SDSS_ACQ_MODE_MINI:
      case SDSS_ACQ_MODE_MINI2:
        db_item.acq_mode = DB_ACQ_MINI;
        break;

      case SDSS_ACQ_MODE_MICRO:
      case SDSS_ACQ_MODE_MICRO2:
      case SDSS_ACQ_MODE_MICRO_ABSR:
        db_item.acq_mode = DB_ACQ_MICRO;
        break;

      case SDSS_ACQ_MODE_DEEP:
      case SDSS_ACQ_MODE_DEEP2:
      case SDSS_ACQ_MODE_SHALLOW:
      case SDSS_ACQ_MODE_SHALLOW2:
        /* For HDR_Only mode, we have Deep scans, map these to mini */
        db_item.acq_mode = DB_ACQ_MINI;
        break;

      case SDSS_ACQ_MODE_MAX:
      case SDSS_ACQ_MODE_PREV:
      default:
        SD_ERR_0( "acq_mode");
        db_item.acq_mode = DB_ACQ_FULL;
        break;

    } /* switch */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Tell Search about the mode of the attempted acquisition by deposition
    ** the acquisition mode value in the DB.
    */
    db_put( DB_ACQ_MODE, &db_item );
  } /* if */
  #endif

} /* sd_ss_acq_update_pre */


/*===========================================================================

FUNCTION sd_ss_map_acq_mode_to_hdr

DESCRIPTION
  Map SD acquisition mode to HDR specific acquisition mode.

DEPENDENCIES
  None.

RETURN VALUE
  The specific HDR acquisition that is associated with the input SD
  acquisition mode.

  SD_BLKSYS_MAX if the input mode, band and channel are bad or unsupported.

SIDE EFFECTS
  None.

===========================================================================*/
static   sd_hdr_acq_mode_e_type    sd_ss_map_acq_mode_to_hdr(

        sdss_acq_mode_e_type      acq_mode
            /* SD acquisition mode to be mapped */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( acq_mode )
  {
    case SDSS_ACQ_MODE_HO:
    case SDSS_ACQ_MODE_FULL:
    case SDSS_ACQ_MODE_MINI:
    case SDSS_ACQ_MODE_FULL2:
    case SDSS_ACQ_MODE_MINI2:
    case SDSS_ACQ_MODE_FULL_AOOS:
    case SDSS_ACQ_MODE_FULL_OSR:
    case SDSS_ACQ_MODE_EOOS_FULL:
    case SDSS_ACQ_MODE_EOOS_FULL2:
    case SDSS_ACQ_MODE_EOOS_FULL_RTN:
    case SDSS_ACQ_MODE_EOOS_FULL_RTN2:
      return SD_HDR_ACQ_MODE_FULL;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_MICRO:
    case SDSS_ACQ_MODE_MICRO2:
    case SDSS_ACQ_MODE_MICRO_ABSR:
      return SD_HDR_ACQ_MODE_MICRO;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_DEEP:
    case SDSS_ACQ_MODE_DEEP2:
      return SD_HDR_ACQ_MODE_DEEP;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_SHALLOW:
    case SDSS_ACQ_MODE_SHALLOW2:
      return SD_HDR_ACQ_MODE_SHALLOW;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_ACQ_MODE_PREV:
    case SDSS_ACQ_MODE_MAX:
    default:
      return SD_HDR_ACQ_MODE_FULL;

  } /* switch */

} /* sd_ss_map_acq_mode_to_hdr */

#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || \
    defined(FEATURE_SD_LTE)
/*===========================================================================

FUNCTION sd_set_acq_gw_sys

DESCRIPTION
  Set system network selection type and manual selection mode for acquiring
  GW sytem based on plmn information.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void                            sd_set_acq_gw_sys(

        const sd_ss_e_type               ss,
            /* SS on which acq is happening */

        const sd_sid_plmn_s_type      *sid_exp_ptr,
        /* plmn for acquire GSM/WCDMA/LTE system */

        sdss_evt_e_type                 event,
            /* SS-Event to process */

        sd_ss_pref_reas_e_type          pref_reason,
          /* pref reason the update was done */

        sd_ss_act_acq_gwl_s_type         *act_ptr
        /* Pointer to act for acquiring GSM/WCDMA/LTE system */
)
{
  SD_ASSERT( sid_exp_ptr != NULL );
  SD_ASSERT( act_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( sid_exp_ptr->net_select_mode )
  {
    case SD_NET_SELECT_MODE_LIMITED:
      act_ptr->net_sel_type =
                      SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION;
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_NET_SELECT_MODE_AUTO:
      act_ptr->net_sel_type = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
      act_ptr->is_user_resel = FALSE;
      /* Set the resel flag to TRUE, if user initiated reselect.
      */
      if ( event == sdss_event_map(ss,SDSS_EVT_USER_SS_PREF) &&
           pref_reason == SD_SS_PREF_REAS_USER_RESEL
         )
      {
        act_ptr->is_user_resel = TRUE;
      }
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_NET_SELECT_MODE_HPLMN:
      act_ptr->net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL;
      act_ptr->selected_plmn_type = SD_MANUALLY_SELECTED_HPLMN;
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_NET_SELECT_MODE_PRL_SYS:
      act_ptr->net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL;
      act_ptr->selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;
      act_ptr->plmn = sid_exp_ptr->prm.gwl.plmn.plmn_id;
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_NET_SELECT_MODE_MANUAL_SYS:
      act_ptr->net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL;
      act_ptr->selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;
      act_ptr->plmn = sid_exp_ptr->prm.gwl.plmn.plmn_id;
      return;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_NET_SELECT_MODE_MAX:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    default:
      SD_ERR_1( "net_select_mode=%d",sid_exp_ptr->net_select_mode);
      break;

    } /* switch */

} /* sd_set_acq_gw_sys */
#endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED || FEATURE_SD_LTE */


#if defined FEATURE_SD_LTE || defined SD_GSM_SUPPORTED || defined SD_WCDMA_SUPPORTED

/*===========================================================================

FUNCTION sd_update_rat_pri_list

DESCRIPTION
  Copy the rat priority list stored in sdss core and ordered per the TOT
  For MMSS enabled operation, Reorder based on the Gwl priority list stored in the system record.
  For non MMSS enabled operation , retain the order in the TOT
  Update the bst_band_cap based on the bands in the system record
  Update the bst_rat_acq_required based on which RATs are present in the sys
  record and need to be scanned.
  For all remaining RATs that the UE supports but which are not part of the
  system record, set the bst_rat_acq_required to FALSE. .


DEPENDENCIES
  None

RETURN VALUE
  Updated rat priority list.

SIDE EFFECTS
  None.

===========================================================================*/
void                           sd_update_rat_pri_list(

        sys_rat_pri_list_info_s_type  *rat_pri_ptr,
            /* Pointer to the RAT priority list that needs to be updated */

        sdsr_s_type                   *sr_ptr,
            /* Pointer to the system to be acquired, based on which the RAT
            ** priority list needs to be updated */

        sd_ss_e_type                  ss,
        /* stack on which ACQ is occuring */

        sdss_evt_e_type           event
       /* Current event */
)
{
  sys_priority_list_info_s_type  *rat_info;
  sys_rat_pri_list_info_s_type    ue_cap;
  sd_ss_band_pref_e_type          band_pref   = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type        lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE;
  sys_sys_mode_e_type             mode;

  /* rat_pref is used to indicate what 3GPP rats need to be added into the
  ** rat priority list after including the ones in the system record
  */
  sd_ss_mode_pref_e_type         rat_pref     = SD_SS_MODE_PREF_GWL;
  sd_ss_mode_pref_e_type         temp_pref    = SD_SS_MODE_PREF_NONE;
  uint32                         i            = 0;
  uint32                         j            = 0;

  sd_ss_mode_pref_e_type         ss_mode_pref = SD_SS_MODE_PREF_NONE;
  boolean                        is_emerg_call_orig = sdss_is_emergency_call_orig(ss);
  sdss_acq_stt_e_type            acq_stt            = sdss_eng_stack_get_state(ss);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_5( "ss = %d system rec band: 0x%08x %08x, tds band: 0x%08x %08x ",
                  ss, QWORD_HIGH(sr_ptr->sys.band), QWORD_LOW(sr_ptr->sys.band),
    				QWORD_HIGH(sr_ptr->sys.tds_band), QWORD_LOW(sr_ptr->sys.tds_band));
  sd_print_lte_band_mask(sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band));


  /* get a local copy of the ue capability info from sdss core
  */

  sdss_get_rat_pri_list_info(ss, &ue_cap);

  sdss_get_mode_band_pref( ss,
                           &ss_mode_pref,
                           NULL,
                           NULL,
                           NULL );

  /* clear rat pri ptr before updating from acq record
  */
  memset(rat_pri_ptr,0,sizeof(sys_rat_pri_list_info_s_type));

  /* For MMSS Enabled operation, set the RATs in the system record as highest
  ** priority and list them in the order in which they occur in the
  ** acquisition record table
  */
  /* in case if feature ral_stored_n_efs is defined, and it is emergency mode origination
  we need to have rat_pri_list ordered by ETOT */
  if(sdss_is_mmss_operation(ss) && !(sdss_is_ral_stored_in_efs(ss) && sdss_is_orig_mode(ss,SD_SS_ORIG_MODE_VAL_EMERG)))
  {
    SD_MSG_HIGH_0( "is MMSS Operation");
    /* check if gwl acq record's priority list has valid values
    */
    if( sr_ptr->sid_expected.prm.gwl.num_rats > SYS_MAX_NUM_3GPP_RATS)
    {
      SD_ERR_0("uninitialized gwl_pri_lst");
      sr_ptr->sid_expected.prm.gwl.num_rats = SYS_MAX_NUM_3GPP_RATS;
      sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[0] = SD_MODE_GSM;
      sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[1] = SD_MODE_WCDMA;
      sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[2] = SD_MODE_LTE;
      /* not expected to ever go in here
      */
      SD_ASSERT(FALSE);
    }

    SD_MSG_HIGH_1(" sr_ptr->sid_expected.prm.gwl.num_rats = %d ",
                              sr_ptr->sid_expected.prm.gwl.num_rats);
    /* update from gwl acq record first
    */
    for(i = 0;i< sr_ptr->sid_expected.prm.gwl.num_rats;i++)
    {
      /* Find if each RAT in the system record is supported by
      ** the UE
      */
      j = sdss_get_rat_pri_sys_idx(ss,
                                  sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[i]);

      /* If the RAT is not supported, move to the next RAT in the sys record.
      */
      if(j == ue_cap.num_items)
      {
        SD_MSG_HIGH_1("RAT %d not supp - pri lst",
                   sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[i]);
        continue;
      }
      /* if we reach here, the RAT is supported per the phone's  mode/band
      ** settings and needs to be added to the rat priority list
      */
      mode = ue_cap.priority_list_info[j].acq_sys_mode;

      /* If UE is trying to perform initial attach on LTE during an emergency call
      ** restrict the scan to LTE only to avoid full service scans on GW in the 
      ** case where LTE scan fails 
      */ 
      if (acq_stt == SDSS_ACQ_STT_REACQ_FULL_SRV && mode != SD_MODE_LTE) 
      {
        SD_MSG_HIGH_0("LTE_911:skip GW full scan");
        continue;
      }

	  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined( FEATURE_SD_LTE )
      if ((mode != SYS_SYS_MODE_LTE) &&
           sdss_is_emer_orig_in_volte_only(ss) &&
          (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE))
      {
        SD_MSG_MED_0("E911_LTE_ONLY: GW cap not supp - emergency_orig_in lte only mcc");
        continue;
      }

	   /* For a specialized VOLTE only device,
	    if e911 call is made when in romaing , remove LTE as call should be tried over CS only*/
	  if((mode == SYS_SYS_MODE_LTE) &&
         sdss_get_e911_in_roam(ss) &&
         (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE)	   
		 )
      {
        SD_MSG_MED_0(" LTE not supp- originate in CS only during Roaming ");
        continue;
      }

      #endif

      /* get the bands corresponding to the mode
      */
      if( mode == SD_MODE_LTE )
      {
        lte_band_pref = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band);
        /* filter the bands based on the capability
        */
        lte_band_pref = SYS_LTE_BAND_MASK_COMMON(&lte_band_pref, &(ue_cap.priority_list_info[j].band_cap.lte_band_cap));
      }
      else
      {
        band_pref =  sr_ptr->sys.band;
      /* filter the bands based on the capability
      */
        band_pref = band_pref & ue_cap.priority_list_info[j].band_cap.chgwt_band_cap;
      }

      /* perform following checks to avoid situation where mode/band pref of
      ** phone supports GSM and WCDMA , but the system record has only wcdma
      ** bands. In such a scenario, the bst_rat_acq_reqd should be set to
      ** FALSE for GSM
      */
      if(mode == SYS_SYS_MODE_GSM)
      {
        band_pref = sdss_get_supp_gsm_band_pref(band_pref);
      }
      else if(mode == SYS_SYS_MODE_WCDMA)
      {
        band_pref = sdss_get_supp_wcdma_band_pref(band_pref);
      }
      /* filter out overlapping 2G 3g bands
      */
      if(mode != SD_MODE_LTE)
      {
        band_pref = sd_filter_out_overlap_band(band_pref);
      }
      /* if after the filtering , there are no bands, go to the next
      ** RAT in the system record.
      */
      if((mode != SD_MODE_LTE && band_pref == SD_SS_BAND_PREF_NONE)||
         (mode == SD_MODE_LTE && SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_band_pref))
        )
      {
        SD_MSG_HIGH_1("RAT %d not supp in rec", mode);
        continue;
      }

      /* If SIM is not ps valid, remove LTE from scans (unless UE is in an emergency
      ** call origination)
      */ 
      if( mode == SD_MODE_LTE && 
          (sdss_get_sim_state(ss)== SYS_SIM_STATE_PS_INVALID) &&
          !is_emerg_call_orig)
      {
        SD_MSG_HIGH_2("LTE not supp - sim state:%d or domain_pref:%d",sdss_get_sim_state(ss),
                      sdss_get_domain_pref(ss));
        continue;
      }

      /* Skip adding GW capabilities to RAT PRI LIST at the time of 1XCSFB call release */
      #if defined (FEATURE_LTE_TO_1X)
      if (!sdss_is_gw_supp_for_1xcsfb_call(ss, event, mode))
      {
        SD_MSG_HIGH_0("GW cap not supp - 1xCSFB call release event ");
        continue;
      }
      #endif
        
      /* if we reach here, there are bands in the system record that need to
      ** be searched as part of this RAT
      */
      rat_info = &(rat_pri_ptr->priority_list_info[rat_pri_ptr->num_items]);
      rat_info->acq_sys_mode = mode;
      if( mode == SD_MODE_LTE )
      {
        rat_info->band_cap.lte_band_cap        = ue_cap.priority_list_info[j].band_cap.lte_band_cap;
        rat_info->bst_band_cap.lte_band_cap    = lte_band_pref ;
      }
      else
      {
        rat_info->band_cap.chgwt_band_cap      = ue_cap.priority_list_info[j].band_cap.chgwt_band_cap;
        rat_info->bst_band_cap.chgwt_band_cap  = band_pref ;
      }

      SD_MSG_HIGH_1(" Updating mode = %d ", mode);
      /* Update the bst_band_cap with the bands that need to be searched by NAS
      ** It should be the bands that are present in the system record and
      ** filtered based on the UE capability
      */
      rat_info->bst_rat_acq_required = TRUE;
      rat_pri_ptr->num_items++;

      #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
      /* Do not acquire LTE for for 3GPP CSFB call when ever service aquisition is done
      ** during call setup (it could be because of redial/OOS/RLF)
      ** For 1XCSFB do not aquire LTE only if LTE is not part of mode preference
      */
      if(mode == SD_MODE_LTE &&
          ss   == SD_SS_MAIN  &&
          sdss_is_orig_mode( ss, SD_SS_ORIG_MODE_VAL_CSFB_ORIG ) )
      {
        if (sdss_is_orig_mode(SD_SS_MAIN, SD_SS_ORIG_MODE_VAL_PPCSFB_ORIG ))
        {
          SD_MSG_HIGH_0("LTE not supp -ppcsfb");
          rat_info->bst_rat_acq_required = FALSE;
          rat_info->bst_band_cap.lte_band_cap   = SYS_LTE_BAND_MASK_CONST_NONE;
        }
        else if(sdss_is_orig_mode(SD_SS_MAIN, SD_SS_ORIG_MODE_VAL_1XCSFB_ORIG ))
        {
          if(!(SD_IS_BIT_CONTAIN (ss_mode_pref ,SD_SS_MODE_PREF_LTE)))
          {
           SD_MSG_HIGH_0("LTE not supp -csfb");
           rat_info->bst_rat_acq_required = FALSE;
           rat_info->bst_band_cap.lte_band_cap  = SYS_LTE_BAND_MASK_CONST_NONE;
          }
        }

      }
      #endif
      if (mode == SD_MODE_LTE &&
          sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_ORIG))
      {
        if(!(SD_IS_BIT_CONTAIN (ss_mode_pref ,SD_SS_MODE_PREF_LTE)))
        {
          SD_MSG_HIGH_0("LTE not supp -call pref");
          rat_info->bst_rat_acq_required = FALSE;
          rat_info->bst_band_cap.lte_band_cap = SYS_LTE_BAND_MASK_CONST_NONE;
        }
      }

      if (mode == SD_MODE_TDS &&
          sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_ORIG))
      {
        if(!(SD_IS_BIT_CONTAIN (ss_mode_pref ,SD_SS_MODE_PREF_TDS)))
        {
          SD_MSG_HIGH_0("TDS is not supported - call pref");
          rat_info->bst_rat_acq_required = FALSE;
          rat_info->bst_band_cap.chgwt_band_cap  = SD_SS_BAND_PREF_NONE;
        }
      }	  

      /* Delete the RAT from the rat_pref, to indicate that it has
      ** already been intered into the rat priority list
      */
      temp_pref = sdss_map_mode_to_mode_pref(rat_info->acq_sys_mode);
      rat_pref = SD_MODE_DELETE(rat_pref, temp_pref);

    }// for num of rats in gwl sys rec

    /* update 3gpp2 info into the rat priority list from ue_cap
    ** as well as info for other 3GPP RATs which are not part of this system
    ** record
    */
    for(i = 0; i < ue_cap.num_items; i++)
    {

      j = rat_pri_ptr->num_items;
      rat_info = &(rat_pri_ptr->priority_list_info[j]);

      switch(ue_cap.priority_list_info[i].acq_sys_mode)
      {
        case SYS_SYS_MODE_HDR:
        case SYS_SYS_MODE_CDMA:
		  if ( sdss_is_emer_orig_in_volte_only(ss) &&
		       (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE))
		  {
		    SD_MSG_HIGH_0("E911_LTE_ONLY: 3gpp2 cap not supp - emergency_orig_in call mode volte only");
		    continue;
		  }
          rat_info->acq_sys_mode         =
                          ue_cap.priority_list_info[i].acq_sys_mode;
          rat_pri_ptr->priority_list_info[j].band_cap =
                              ue_cap.priority_list_info[i].band_cap;
          rat_info->bst_rat_acq_required = FALSE;
          rat_info->bst_band_cap.chgwt_band_cap  = SD_SS_BAND_PREF_NONE;
          rat_pri_ptr->num_items++;
          break;

        case SYS_SYS_MODE_GSM:
        case SYS_SYS_MODE_WCDMA:
        case SYS_SYS_MODE_LTE:
        case SYS_SYS_MODE_TDS:
          /* If this 3gpp mode is not present in rat_pref, it means
          ** that it was already inserted into the rat priority list
          */
          temp_pref = sdss_map_mode_to_mode_pref(
                          ue_cap.priority_list_info[i].acq_sys_mode);

          /* Skip adding GW capabilities to RAT PRI LIST at the time of 1XCSFB call release */
          #if defined (FEATURE_LTE_TO_1X)
          if (!sdss_is_gw_supp_for_1xcsfb_call(ss, event, ue_cap.priority_list_info[i].acq_sys_mode))
          {
            SD_MSG_HIGH_1("%d cap not supp - 1xCSFB call release event",
                                   ue_cap.priority_list_info[i].acq_sys_mode);
            continue;
          }
          #endif
          
          if(SD_MODE_CONTAIN(rat_pref,temp_pref))
          {
            /* If LTE is not supported as per SIM state, do not add it to
            ** the UE cap list
            */
            if(ue_cap.priority_list_info[i].acq_sys_mode == SD_MODE_LTE &&
               (sdss_get_sim_state(ss)== SYS_SIM_STATE_PS_INVALID) &&
               !is_emerg_call_orig )
            {
              SD_MSG_HIGH_2("LTE not supp in UE cap - sim state:%d or domain_pref:%d",
                            sdss_get_sim_state(ss),sdss_get_domain_pref(ss));
              continue;
            }

			#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined( FEATURE_SD_LTE )
		    if ((ue_cap.priority_list_info[i].acq_sys_mode != SYS_SYS_MODE_LTE) &&
		         sdss_is_emer_orig_in_volte_only(ss) &&
		        (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE))
		    {
		      SD_MSG_HIGH_0("E911_LTE_ONLY: GW cap not supp - emergency_orig_in lte only mcc");
		      continue;
		    }

		    /*For a specialized VOLTE only device,
	        if e911 call is made when in romaing , remove LTE as call should be tried over CS only*/
	         if((ue_cap.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE) &&
             sdss_get_e911_in_roam(ss) &&
             (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE)	   
		    )
            {
              SD_MSG_MED_0(" LTE not supp- originate in CS only during Roaming ");
              continue;           
			}

		    #endif
            /* If this 3gpp mode is present in rat_pref, it needs to
            ** be inserted into the rat priority list with bst_band_cap
            ** set to NONE and bst_rat_acq_required set to FALSE
            */
            SD_MSG_HIGH_1(" Appending  mode = %d ", ue_cap.priority_list_info[i].acq_sys_mode);
            rat_info->acq_sys_mode           =
                            ue_cap.priority_list_info[i].acq_sys_mode;
            if( ue_cap.priority_list_info[i].acq_sys_mode == SD_MODE_LTE )
            {
              rat_pri_ptr->priority_list_info[j].band_cap.lte_band_cap =
                                  ue_cap.priority_list_info[i].band_cap.lte_band_cap;
              rat_info->bst_band_cap.lte_band_cap = SYS_LTE_BAND_MASK_CONST_NONE;
            }
            else
            {
              rat_pri_ptr->priority_list_info[j].band_cap.chgwt_band_cap =
                                  ue_cap.priority_list_info[i].band_cap.chgwt_band_cap;
              rat_info->bst_band_cap.chgwt_band_cap  = SD_SS_BAND_PREF_NONE;
            }
            rat_info->bst_rat_acq_required = FALSE;
            rat_pri_ptr->num_items++;
          }
          break;

        case SYS_SYS_MODE_NONE:
        case SYS_SYS_MODE_NO_SRV:
        case SYS_SYS_MODE_AMPS:
        case SYS_SYS_MODE_GPS:
        case SYS_SYS_MODE_GW:
        case SYS_SYS_MODE_GWL:
        case SYS_SYS_MODE_MAX:
        default:
          break;
      }
    }
  } // is mmss operation
  else
  {
    /*
    ** For non-MMSS operation - update the entire RAT priority list based on
    ** the TOT order and with the bands from the system record entry.
    */
    if(sdss_is_ral_stored_in_efs(ss) && sdss_is_orig_mode(ss,SD_SS_ORIG_MODE_VAL_EMERG))
    {
      SD_MSG_HIGH_0("RAL: stored in efs is true and orig mode is emerg");
    }


    SD_MSG_HIGH_2( "MMSS Operation = %d, ue_cap.num_items = %d",sdss_is_mmss_operation(ss) ,ue_cap.num_items); 

    for(i = 0; i < ue_cap.num_items; i++)
    {

      j = rat_pri_ptr->num_items;
      rat_info = &(rat_pri_ptr->priority_list_info[j]);

      switch(ue_cap.priority_list_info[i].acq_sys_mode)
      {
        case SYS_SYS_MODE_HDR:
        case SYS_SYS_MODE_CDMA:
		  if ( sdss_is_emer_orig_in_volte_only(ss) &&
		       (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE))
		  {
		    SD_MSG_HIGH_0("E911_LTE_ONLY: 3gpp2 cap not supp - emergency_orig_in call mode volte only");
		    continue;
		  }
          rat_info->acq_sys_mode         =
                          ue_cap.priority_list_info[i].acq_sys_mode;
          rat_pri_ptr->priority_list_info[j].band_cap =
                              ue_cap.priority_list_info[i].band_cap;
          rat_info->bst_rat_acq_required = FALSE;
          rat_info->bst_band_cap.chgwt_band_cap   = SD_SS_BAND_PREF_NONE;
          rat_pri_ptr->num_items++;
          break;

        case SYS_SYS_MODE_GSM:
        case SYS_SYS_MODE_WCDMA:
        case SYS_SYS_MODE_LTE:
        case SYS_SYS_MODE_TDS:

          /* Skip adding GW capabilities to RAT PRI LIST at the time of 1XCSFB call release */
          #if defined (FEATURE_LTE_TO_1X)
          if (!sdss_is_gw_supp_for_1xcsfb_call(ss, event, ue_cap.priority_list_info[i].acq_sys_mode))
          {
            SD_MSG_HIGH_1("%d cap not supp - 1xCSFB call release event",
                                   ue_cap.priority_list_info[i].acq_sys_mode);
            continue;
          }
          #endif
           #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined( FEATURE_SD_LTE )
          if ((ue_cap.priority_list_info[i].acq_sys_mode != SYS_SYS_MODE_LTE) &&
               sdss_is_emer_orig_in_volte_only(ss) &&
              (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE))
          {
            SD_MSG_HIGH_0("E911_LTE_ONLY: GW cap not supp - emergency_orig_in lte only mcc");
            continue;
          }

		  /*For a specialized VOLTE only device,
	         if e911 call is made when in romaing , remove LTE as call should be tried over CS only*/
	      if((ue_cap.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE) &&
              sdss_get_e911_in_roam(ss) &&
              (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE)	   
		     )
          {
		    SD_MSG_MED_0(" LTE not supp- originate in CS only during Roaming ");
            continue;        
		  }

          #endif

          rat_info->acq_sys_mode           =
                          ue_cap.priority_list_info[i].acq_sys_mode;
          if( ue_cap.priority_list_info[i].acq_sys_mode == SD_MODE_LTE )
          {
            rat_pri_ptr->priority_list_info[j].band_cap.lte_band_cap =
                                ue_cap.priority_list_info[i].band_cap.lte_band_cap;
            sd_print_lte_band_mask(rat_pri_ptr->priority_list_info[j].band_cap.lte_band_cap);
          }
          else
          {
            rat_pri_ptr->priority_list_info[j].band_cap.chgwt_band_cap =
                                ue_cap.priority_list_info[i].band_cap.chgwt_band_cap;
            SD_MSG_HIGH_3( "rat_info->acq_sys_mode: %d, rat_pri_ptr->priority_list_info[j].band_cap: 0x%08x %08x",
                           rat_info->acq_sys_mode,
                           QWORD_HIGH(rat_pri_ptr->priority_list_info[j].band_cap.chgwt_band_cap),
                           QWORD_LOW(rat_pri_ptr->priority_list_info[j].band_cap.chgwt_band_cap));
          }
          /* get the bands corresponding to the mode from the sys record
          */
          if (rat_info->acq_sys_mode == SD_MODE_LTE)
          {
             lte_band_pref = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band);
             lte_band_pref = SYS_LTE_BAND_MASK_COMMON(&lte_band_pref, &(ue_cap.priority_list_info[j].band_cap.lte_band_cap));
          }
          else if (rat_info->acq_sys_mode == SD_MODE_TDS)
          {
             band_pref = sr_ptr->sys.tds_band;
          }
          else
          {
             band_pref = sr_ptr->sys.band;
          }

          /* filter the bands based on the capability
          */
          band_pref = band_pref & ue_cap.priority_list_info[i].band_cap.chgwt_band_cap;

          /* perform following checks to avoid situation where mode/band pref of
          ** phone supports GSM and WCDMA , but the system record has only wcdma
          ** bands. In such a scenario, the bst_rat_acq_reqd should be set to
          ** FALSE for GSM
          */
          if(rat_info->acq_sys_mode == SYS_SYS_MODE_GSM)
          {
            band_pref = sdss_get_supp_gsm_band_pref(band_pref);
          }
          else if(rat_info->acq_sys_mode == SYS_SYS_MODE_WCDMA)
          {
            band_pref = sdss_get_supp_wcdma_band_pref(band_pref);
          }
          /* filter out overlapping 2G 3g bands
          */
          if(rat_info->acq_sys_mode != SD_MODE_LTE &&
             rat_info->acq_sys_mode != SD_MODE_TDS)
          {
            band_pref = sd_filter_out_overlap_band(band_pref);
          }

             /* If the SIM state is PS invalid no need to scan LTE*/
              if(rat_info->acq_sys_mode == SD_MODE_LTE && 
                 (sdss_get_sim_state(ss)== SYS_SIM_STATE_PS_INVALID) &&
               (!is_emerg_call_orig || (is_emerg_call_orig && sdss_get_is_full_srv_req_during_emerg(ss))) )
            {
            SD_MSG_HIGH_2("LTE not supp in UE cap - sim state:%d or domain_pref:%d",
                          sdss_get_sim_state(ss),sdss_get_domain_pref(ss));
              continue;
            }

          if((rat_info->acq_sys_mode != SD_MODE_LTE && band_pref == SD_SS_BAND_PREF_NONE)||
             (rat_info->acq_sys_mode == SD_MODE_LTE && SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_band_pref))
            )
          {
            rat_info->bst_rat_acq_required = FALSE;
            SD_MSG_HIGH_1( "bst_rat_acq_required = %d", rat_info->bst_rat_acq_required);
          }
          /* If UE is trying to perform initial attach on LTE during an emergency call
          ** restrict the scan to LTE only to avoid full service scans on GW in the 
          ** case where LTE scan fails 
          */ 
          else if (acq_stt == SDSS_ACQ_STT_REACQ_FULL_SRV && 
                   rat_info->acq_sys_mode != SD_MODE_LTE) 
          {
            SD_MSG_HIGH_0("LTE_911:skip GWT full scan");
            rat_info->bst_rat_acq_required = FALSE;
            rat_info->bst_band_cap.chgwt_band_cap  = SD_SS_BAND_PREF_NONE;
          }
          /* if we are sending service request in case when NAS has send no service 
          with scan_status as PLMN found, and SDSS_FLAG_OVERRIDE_E911_ADD_INFO is set
          subsequent service request is to be sent with LTE only */
          else if( (sdss_is_ral_stored_in_efs(ss)) &&
                   (rat_info->acq_sys_mode != SD_MODE_LTE) &&
                   (sdss_is_flag_set(ss,SDSS_FLAG_OVERRIDE_E911_ADD_INFO))
                  )
          {
            SD_MSG_HIGH_0("LTE_911:skip GWT full scan, scan only LTE");
            rat_info->bst_rat_acq_required = FALSE;
            rat_info->bst_band_cap.chgwt_band_cap = SD_SS_BAND_PREF_NONE;
          }
          else
          {
            rat_info->bst_rat_acq_required = TRUE;
            #if defined (FEATURE_3GPP_CSFB) || defined (FEATURE_LTE_TO_1X)
            /* Do not acquire LTE for for 3GPP CSFB call when ever service aquisition is done
            ** during call setup (it could be because of redial/OOS/RLF)
            ** For 1XCSFB do not aquire LTE only if LTE is not part of mode preference
            */
            if( rat_info->acq_sys_mode == SD_MODE_LTE )
            {
              if( !sdss_is_lte_supp_for_csfb_call( ss ) )
              {
                rat_info->bst_rat_acq_required = FALSE;
                rat_info->bst_band_cap.lte_band_cap = SYS_LTE_BAND_MASK_CONST_NONE;
              }
            }

            #endif
            if ( rat_info->acq_sys_mode  == SD_MODE_LTE &&
                 sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_ORIG))
            {
              if(!(SD_IS_BIT_CONTAIN (ss_mode_pref ,SD_SS_MODE_PREF_LTE)))
              {
                SD_MSG_HIGH_0("LTE not supp -call pref");
                rat_info->bst_rat_acq_required = FALSE;
                rat_info->bst_band_cap.lte_band_cap = SYS_LTE_BAND_MASK_CONST_NONE;
              }
            }

            if ( rat_info->acq_sys_mode == SD_MODE_TDS &&
                 sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_ORIG))
            {
              if(!(SD_IS_BIT_CONTAIN (ss_mode_pref ,SD_SS_MODE_PREF_TDS)))
              {
                SD_MSG_HIGH_0("TDS is not supported - call pref");
                rat_info->bst_rat_acq_required = FALSE;
                rat_info->bst_band_cap.chgwt_band_cap = SD_SS_BAND_PREF_NONE;
              }
            }   
          }
          if ( rat_info->acq_sys_mode  == SD_MODE_LTE )
          {
            rat_info->bst_band_cap.lte_band_cap = lte_band_pref;
          }
          else
          {
            rat_info->bst_band_cap.chgwt_band_cap = band_pref;
          }
          rat_pri_ptr->num_items++;
          break;

        case SYS_SYS_MODE_NONE:
        case SYS_SYS_MODE_NO_SRV:
        case SYS_SYS_MODE_AMPS:
        case SYS_SYS_MODE_GPS:
        case SYS_SYS_MODE_GW:
        case SYS_SYS_MODE_GWL:
        case SYS_SYS_MODE_MAX:
        default:
          break;
      }// switch()
    } // for(ue_cap.num_items)
  }// else (is_mmss_op)

} /* sd_update_rat_pri_list*/


/*===========================================================================

FUNCTION sd_update_get_net_rat_pri_list

DESCRIPTION
  This function creates rat_pri_list for GET_NET_LIST command based on TOT order.


DEPENDENCIES
  None

RETURN VALUE
  Updated rat priority list.

SIDE EFFECTS
  None.

===========================================================================*/
static  void               sd_update_get_net_rat_pri_list_tot_order(
      sd_ss_e_type                              ss,
                       /* System selection stack
                       */
        sys_rat_pri_list_info_s_type       *rat_pri_ptr,
            /* Pointer to the RAT priority list that needs to be updated */

        const sd_ss_act_get_net_gw_s_type  *get_net_ptr
           /* Pointer to the GET_NET actions info */
)
{
  sys_priority_list_info_s_type  *rat_info;
  sd_ss_band_pref_e_type          band_pref = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type        lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE;
  sd_ss_mode_pref_e_type          temp_pref = get_net_ptr->gw_mode_pref;
  sys_sys_mode_e_type             get_sys_mode = SD_MODE_NONE;
  uint32                          i         = 0;
  sd_rat_acq_order_s_type  rat_acq_order = sdss_get_rat_acq_order(ss);

   /* clear rat pri ptr before updating from acq record
  */
  memset(rat_pri_ptr,0,sizeof(sys_rat_pri_list_info_s_type));

  for(i = 0;i< rat_acq_order.num_rat;i++)
  {
    if ( SD_MODE_CONTAIN( temp_pref, BM( rat_acq_order.acq_sys_mode[i] )))
    {
      if (rat_acq_order.acq_sys_mode[i] == SD_MODE_GSM )
      {
        get_sys_mode = SD_MODE_GSM;
        band_pref = get_net_ptr->gw_band_pref;
        temp_pref = SD_MODE_DELETE(temp_pref, SD_SS_MODE_PREF_GSM);
      }
      else if (rat_acq_order.acq_sys_mode[i] == SD_MODE_WCDMA)
      {
        get_sys_mode = SD_MODE_WCDMA;
        band_pref = get_net_ptr->gw_band_pref;
        temp_pref = SD_MODE_DELETE(temp_pref, SD_SS_MODE_PREF_WCDMA);
      }
      #if defined(FEATURE_SD_LTE)
      else if ( rat_acq_order.acq_sys_mode[i] == SD_MODE_LTE)
      {
        get_sys_mode = SD_MODE_LTE;
        lte_band_pref = get_net_ptr->lte_band_pref;
        temp_pref = SD_MODE_DELETE(temp_pref, SD_SS_MODE_PREF_LTE);
      }
      #endif
      else if ( rat_acq_order.acq_sys_mode[i] == SD_MODE_TDS)
      {
        get_sys_mode = SD_MODE_TDS;
        band_pref = get_net_ptr->tds_band_pref;
        temp_pref = SD_MODE_DELETE(temp_pref, SD_SS_MODE_PREF_TDS);
      }
      else
      {
        SD_MSG_HIGH_1("get net: inv mode %d",rat_acq_order.acq_sys_mode[i]);
        continue;
      }
    }
    if((band_pref == SD_SS_BAND_PREF_NONE && get_sys_mode != SD_MODE_LTE)
       ||
       (SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_band_pref) && get_sys_mode == SD_MODE_LTE)
      )
    {
      continue;
    }

    rat_info = &(rat_pri_ptr->priority_list_info[rat_pri_ptr->num_items]);
    rat_info->acq_sys_mode = get_sys_mode;
    if( get_sys_mode != SD_MODE_LTE )
    {
      rat_info->band_cap.chgwt_band_cap     = band_pref;
      rat_info->bst_band_cap.chgwt_band_cap = band_pref ;
    }
    else
    {
      rat_info->band_cap.lte_band_cap        = lte_band_pref;
      rat_info->bst_band_cap.lte_band_cap    = lte_band_pref;
    }
    rat_info->bst_rat_acq_required = TRUE;
    rat_pri_ptr->num_items++;
    band_pref = SD_SS_BAND_PREF_NONE;
    SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_pref);
  } // for

  SD_MSG_HIGH_2("get_net_rat_pri_list,gw_mode_pref=%d, num_items=%d",
               get_net_ptr->gw_mode_pref,rat_pri_ptr->num_items);

  for(i = 0; i < rat_pri_ptr->num_items; i++ )
  {
    if( rat_pri_ptr->priority_list_info[i].acq_sys_mode != SYS_SYS_MODE_LTE )
    {
      SD_MSG_HIGH_6("GET_NET:sys_mode = %d acq_reqd = %d band_cap = 0x%08x %08x bst_band_cap = 0x%08x %08x",
           rat_pri_ptr->priority_list_info[i].acq_sys_mode,
           rat_pri_ptr->priority_list_info[i].bst_rat_acq_required,
           QWORD_HIGH(rat_pri_ptr->priority_list_info[i].band_cap.chgwt_band_cap),
           QWORD_LOW(rat_pri_ptr->priority_list_info[i].band_cap.chgwt_band_cap),
           QWORD_HIGH(rat_pri_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap),
           QWORD_LOW(rat_pri_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap));

    }
    else
    {
      SD_MSG_HIGH_1("GET_NET:sys_mode = LTE, acq_reqd = %d",rat_pri_ptr->priority_list_info[i].bst_rat_acq_required);
      SD_MSG_HIGH_0("GET_NET: bst_band_cap");
      sd_print_lte_band_mask(rat_pri_ptr->priority_list_info[i].bst_band_cap.lte_band_cap);
      SD_MSG_HIGH_0("GET_NET: band_cap");
      sd_print_lte_band_mask(rat_pri_ptr->priority_list_info[i].band_cap.lte_band_cap);
    }
  }

} /* sd_update_get_net_rat_pri_list*/


/*===========================================================================

FUNCTION sd_update_get_net_rat_pri_list_manual_lst_order

DESCRIPTION
  This function creates rat_pri_list for GET_NET_LIST command based on the order of default
  bst entry.


DEPENDENCIES
  None

RETURN VALUE
  Updated rat priority list.

SIDE EFFECTS
  None.

===========================================================================*/
static  void       sd_update_get_net_rat_pri_list_manual_lst_order (

        sys_rat_pri_list_info_s_type       *rat_pri_ptr,
            /* Pointer to the RAT priority list that needs to be updated */

        const sd_ss_act_get_net_gw_s_type  *get_net_ptr
           /* Pointer to the GET_NET actions info */
)
{
  sys_priority_list_info_s_type *rat_info;
  sd_ss_band_pref_e_type        band_pref = SD_SS_BAND_PREF_NONE;
  sd_ss_mode_pref_e_type        temp_pref = get_net_ptr->gw_mode_pref;
  sys_sys_mode_e_type           get_sys_mode = SD_MODE_NONE;
  byte                          i         = 0;
  sys_sys_mode_e_type           mode;
  sdsr_s_type                   *list_ptr = sdsr_list_get_ptr(SDSR_MANUAL_LST, 0);
  sys_lte_band_mask_e_type      lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE;
  /* Make sure the list is not empty
   */
  if (list_ptr == NULL || !list_ptr->sid_expected.prm.gwl.num_rats)
  {
    sys_err_fatal_null_ptr_exception();
  }

  if (list_ptr->sid_expected.prm.gwl.num_rats > SYS_MAX_NUM_3GPP_RATS)
  {
    SD_ERR_1("sid_expected.prm.gwl.num_rats exceeds limit %d",
            SYS_MAX_NUM_3GPP_RATS);
  }

  /* clear rat pri ptr before updating from acq record
  */
  memset(rat_pri_ptr,0,sizeof(sys_rat_pri_list_info_s_type));

  /* Loop through num_rats
  */
  for(i = 0;i< list_ptr->sid_expected.prm.gwl.num_rats;i++)
  {
    mode = list_ptr->sid_expected.prm.gwl.gwl_pri_lst[i];

    if (SD_MODE_CONTAIN(temp_pref, sdss_map_mode_to_mode_pref(mode)))
    {
      switch (mode)
      {
        case SYS_SYS_MODE_GSM:
          get_sys_mode = SD_MODE_GSM;
          band_pref = get_net_ptr->gw_band_pref;
          break;

        case SYS_SYS_MODE_WCDMA:
          get_sys_mode = SD_MODE_WCDMA;
          band_pref = get_net_ptr->gw_band_pref;
          break;

        #if defined(FEATURE_SD_LTE)
        case SYS_SYS_MODE_LTE:
          get_sys_mode = SD_MODE_LTE;
          lte_band_pref = get_net_ptr->lte_band_pref;
          break;

        #endif
        case SYS_SYS_MODE_TDS:
          get_sys_mode = SD_MODE_TDS;
          band_pref = get_net_ptr->tds_band_pref;
          break;

        default:
          SD_MSG_HIGH_1("get net: inv mode %d", mode);
          break;
      }
    }

    if((band_pref == SD_SS_BAND_PREF_NONE && get_sys_mode != SD_MODE_LTE)
       ||
       (SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lte_band_pref) && get_sys_mode == SD_MODE_LTE)
       )
    {
      continue;
    }

    rat_info = &(rat_pri_ptr->priority_list_info[rat_pri_ptr->num_items]);
    rat_info->acq_sys_mode = get_sys_mode;
    if( get_sys_mode != SD_MODE_LTE )
    {
      rat_info->band_cap.chgwt_band_cap     = band_pref;
      rat_info->bst_band_cap.chgwt_band_cap = band_pref ;
    }
    else
    {
      rat_info->band_cap.lte_band_cap     = lte_band_pref;
      rat_info->bst_band_cap.lte_band_cap = lte_band_pref;
    }
    rat_info->bst_rat_acq_required = TRUE;
    rat_pri_ptr->num_items++;
    band_pref = SD_SS_BAND_PREF_NONE;
    SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_pref);
  } // for

  SD_MSG_HIGH_2("get_net_rat_pri_list,gw_mode_pref=%d, num_items=%d",
               get_net_ptr->gw_mode_pref,rat_pri_ptr->num_items);

  for(i = 0; i < rat_pri_ptr->num_items; i++ )
  {
    if( rat_pri_ptr->priority_list_info[i].acq_sys_mode != SYS_SYS_MODE_LTE )
    {
      SD_MSG_HIGH_6("GET_NET:sys_mode = %d acq_reqd = %d band_cap = 0x%08x %08x bst_band_cap = 0x%08x %08x",
         rat_pri_ptr->priority_list_info[i].acq_sys_mode,
         rat_pri_ptr->priority_list_info[i].bst_rat_acq_required,
           QWORD_HIGH(rat_pri_ptr->priority_list_info[i].band_cap.chgwt_band_cap),
           QWORD_LOW(rat_pri_ptr->priority_list_info[i].band_cap.chgwt_band_cap),
           QWORD_HIGH(rat_pri_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap),
           QWORD_LOW(rat_pri_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap));

    }
    else
    {
      SD_MSG_HIGH_1("GET_NET:sys_mode = LTE, acq_reqd = %d",rat_pri_ptr->priority_list_info[i].bst_rat_acq_required);
      SD_MSG_HIGH_0("GET_NET: bst_band_cap");
      sd_print_lte_band_mask(rat_pri_ptr->priority_list_info[i].bst_band_cap.lte_band_cap);
      SD_MSG_HIGH_0("GET_NET: band_cap");
      sd_print_lte_band_mask(rat_pri_ptr->priority_list_info[i].band_cap.lte_band_cap);
    }
  }


} /* sd_update_get_net_rat_pri_list*/

#endif // FEATURE_SD_LTE || SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED


/*===========================================================================

FUNCTION sd_misc_map_srv_req_in_camp_mode

DESCRIPTION
  This is the one location to adjust the outgoing SD action when UE is
  operating in CAMPED_ONLY mode.

  net_sel_mode : CAMPED_ONLY
  srv_req_type : normal/SYNC_UP

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void                      sd_misc_map_srv_req_in_camp_mode
(
       sd_ss_act_s_type          *rtrn_act_ptr
           /* Pointer to a buffer where to place returned SS-Action */
)
{
  /* If UE is not in CAMPED_ONLY mode, return */
  if( sdss_get_camp_mode() != SD_SS_PREF_CAMP_MODE_ON )
  {
    return;
  }

  if( rtrn_act_ptr == NULL )
  {
    SD_MSG_HIGH_0("SGLTE: rtrn_act_ptr is NULL. Nothing to adjust");
    return;
  }

  if( rtrn_act_ptr->act != SD_SS_ACT_ACQ_GWL )
  {
    SD_MSG_HIGH_0("SGLTE: rtrn_act_ptr is not ACQ_GWL");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* UE is in CAMPED_ONLY mode.
  ** Map the net select mode to CAMP_ONLY.
  */
  {
    sys_network_selection_mode_e_type temp_net_sel = rtrn_act_ptr->prm.acq_gwl.net_sel_type;
    switch( rtrn_act_ptr->prm.acq_gwl.net_sel_type )
    {
      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION:
      case SYS_NETWORK_SELECTION_MODE_LIMITED:
        temp_net_sel = SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY;
        break;

      case SYS_NETWORK_SELECTION_MODE_MANUAL:
      case SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION:
        temp_net_sel = SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY;
        break;

      case SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY:
      case SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY:
        break;

      case SYS_NETWORK_SELECTION_MODE_NONE:
      case SYS_NETWORK_SELECTION_MODE_MAX:
      default:
        SD_ERR_1("SGLTE: Invalid net_sel_mode %d",rtrn_act_ptr->prm.acq_gwl.net_sel_type);
        temp_net_sel = SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY;
        break;
    }

    SD_MSG_HIGH_2("SGLTE: Mapping net_sel_type %d -> %d",
                                       rtrn_act_ptr->prm.acq_gwl.net_sel_type,
                                                              temp_net_sel);

    rtrn_act_ptr->prm.acq_gwl.net_sel_type = temp_net_sel;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* UE is in CAMPED_ONLY mode.
  ** Service request type can only be NORMAL or Sync up.
  */
  if( rtrn_act_ptr->prm.acq_gwl.srv_req_type != SD_SS_SRV_REQ_TYPE_SYNC_UP &&
      rtrn_act_ptr->prm.acq_gwl.srv_req_type != SD_SS_SRV_REQ_TYPE_NORMAL
    )
  {
    rtrn_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_NORMAL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}


/*===========================================================================

FUNCTION sd_ss_event_rprt

DESCRIPTION
  Report SD event and the resulting SD action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_ss_event_rprt(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR */

        sdss_evt_e_type           event,
            /* SS-Event to process */

        const sd_ss_act_s_type    *act_ptr
            /* Pointer to a buffer with returned SS-Action */
)
{
  sd_ss_s_type              *ss_ptr         = sd_ss_ptr( ss );

  event_id_enum_type        event_id        = EVENT_SD_EVENT_ACTION;

  sdlog_event_s_type        event_buf;
  sdlog_event_s_type        *event_ptr      = &event_buf;
  unsigned char             event_buf_size  = sizeof( sdlog_event_s_type );

  dword                     uptime          = time_get_uptime_secs();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( act_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If reporting for HDR hybrid system selection, set event_id to
  ** EVENT_SD_EVENT_ACTION_HYBR.
  */
  if( ss == SD_SS_HYBR_1 ) {
    event_id = EVENT_SD_EVENT_ACTION_HYBR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the SS-event.
  */
  event_ptr->ss_event   = (word) event;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the SS-Action.
  */
  event_ptr->ss_act     = (byte) sd_log_map_ss_act( act_ptr->act );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the SD state.
  */
  event_ptr->sd_state   = (byte) sd_log_state_get( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy SS-Action's associated parameters.
  */
  switch( act_ptr->act )
  {
    case SD_SS_ACT_CONTINUE:
      if( ss_ptr->ss_timer > uptime ) {
        event_ptr->act_prm.cont.ss_timer = (word)(ss_ptr->ss_timer - uptime);
      }
      else {
        event_ptr->act_prm.cont.ss_timer = 0;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_CDMA:
      event_ptr->act_prm.acq_cdma.band    = (byte)act_ptr->prm.acq_cdma.band;
      event_ptr->act_prm.acq_cdma.blk_sys =
                                         (byte) act_ptr->prm.acq_cdma.blksys;
      event_ptr->act_prm.acq_cdma.chan    = act_ptr->prm.acq_cdma.chan;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_AMPS:
      event_ptr->act_prm.acq_amps.band  = (byte) act_ptr->prm.acq_amps.band;
      event_ptr->act_prm.acq_amps.sys   = (byte) act_ptr->prm.acq_amps.sys;
      event_ptr->act_prm.acq_amps.is_amps_2nd_cch =
                                       act_ptr->prm.acq_amps.is_amps_2nd_cch;
      event_ptr->act_prm.acq_amps.max_redirect_delay =
                             (byte) act_ptr->prm.acq_amps.max_redirect_delay;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_HDR:
      event_ptr->act_prm.acq_hdr.band     = (byte) act_ptr->prm.acq_hdr.band;
      event_ptr->act_prm.acq_hdr.acq_mode =
                                        (byte) act_ptr->prm.acq_hdr.acq_mode;
      event_ptr->act_prm.acq_hdr.chan     = act_ptr->prm.acq_hdr.chan;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_MEAS_DED:
    case SD_SS_ACT_MEAS_BACK:
    case SD_SS_ACT_PWR_SAVE:
    case SD_SS_ACT_ACCEPT:
    case SD_SS_ACT_REJECT:
    case SD_SS_ACT_RELEASE:
    case SD_SS_ACT_ACQ_GPS:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_GWL:
    #if( defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined FEATURE_SD_LTE )
       memscpy( event_ptr->act_prm.acq_gwl.plmn,
                sizeof(event_ptr->act_prm.acq_gwl.plmn),
                act_ptr->prm.acq_gwl.plmn.identity,
                sizeof(act_ptr->prm.acq_gwl.plmn.identity) );

      event_ptr->act_prm.acq_gwl.net_sel_mode =
                                     (byte) act_ptr->prm.acq_gwl.net_sel_type;
    #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED  || defined FEATURE_SD_LTE*/
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_GET_NET_GW:
    #if( defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) )
      event_ptr->act_prm.get_net_gwl.mode_pref =
                                 (byte) act_ptr->prm.get_net_gw.gw_mode_pref;
      event_ptr->act_prm.get_net_gwl.srv_domain_pref =
                              (byte) act_ptr->prm.get_net_gw.srv_domain_pref;

    #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED */
      break;

    case SD_SS_ACT_ACQ_GW:
    case SD_SS_ACT_MAX:
    default:
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For events that carry parameters, copy the parameters.
  */
  switch( event )
  {
    case SDSS_EVT_USER_SS_PREF:
    case SDSS_EVT_HYBR_2_USER_SS_PREF:
      {
        sd_ss_mode_pref_e_type ss_mode_pref      = SD_SS_MODE_PREF_NONE;
        sd_ss_band_pref_e_type ss_band_pref      = SD_SS_BAND_PREF_NONE;

        sdss_get_mode_band_pref( ss,
                                 &ss_mode_pref,
                                 &ss_band_pref,
                                 NULL,
                                 NULL );

        event_ptr->event_prm.ss_pref.orig_mode = (byte) sdss_get_orig_mode(ss);
        event_ptr->event_prm.ss_pref.mode_pref = (word) ss_mode_pref;
        event_ptr->event_prm.ss_pref.band_pref = (dword)ss_band_pref;
        event_ptr->event_prm.ss_pref.roam_pref = (byte) sdss_get_roam_pref(ss);
        event_ptr->event_prm.ss_pref.hybr_pref = (byte) sdss_get_curr_hybr_pref(ss);
      }
      break;

    case SDSS_EVT_CDMA_OPR_ACC_TERM:
      sdss_get_acc_prm(
            ss,
            &event_ptr->event_prm.acc_term.reason,
            &event_ptr->event_prm.acc_term.term );

      event_buf_size -= sizeof( event_ptr->event_prm.acc_term.reserved );
      break;

    case SDSS_EVT_HDR_OPR_ACC_TERM:
      sdss_get_acc_prm(
            ss,
            &event_ptr->event_prm.acc_term.reason,
            &event_ptr->event_prm.acc_term.term );

      event_buf_size -= sizeof( event_ptr->event_prm.acc_term.reserved );
      break;

    default:
      event_buf_size -= sizeof( event_ptr->event_prm );
      break;

  } /* switch( event ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Report the event.
  */
  event_report_payload( event_id, event_buf_size, event_ptr );

} /* sd_ss_event_rprt */


/*===========================================================================

FUNCTION sd_is_same_sid

DESCRIPTION
  Check whether the system IDs are equal.
  For WLAN, get the SSID from sid_expected.


DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system IDs are equal. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sd_is_same_sid(

        const sd_mode_e_type      mode,
            /* Mode of the sid_ptr.
            */

        const sd_sid_u_type       *sid_ptr,
            /* Pointer to a system Id.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
)
{
  #ifdef FEATURE_HDR
  unsigned int i;
  #endif /* FEATURE_HDR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sid_ptr != NULL );
  SD_ASSERT( sr2_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compare the two SIDs base on the system's mode.
  */
  switch( sr2_ptr->sys.mode )
  {
    case SD_MODE_AMPS:
    case SD_MODE_CDMA:

      if ( mode != sr2_ptr->sys.mode )
      {
        return FALSE;
      }

      if( sid_ptr->is95.sid != sr2_ptr->sid.is95.sid ||
          sid_ptr->is95.nid != sr2_ptr->sid.is95.nid )
      {
        return FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_HDR:

      #ifdef FEATURE_HDR
      if ( mode != sr2_ptr->sys.mode )
      {
        return FALSE;
      }

      for( i=0; i < ARR_SIZE(sr2_ptr->sid.is856.sector_id); i++ )
      {
        if( sid_ptr->is856.sector_id[i] != sr2_ptr->sid.is856.sector_id[i] )
        {
          return FALSE;
        }
      }
      #endif /* FEATURE_HDR */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
      if ( mode != SD_MODE_GSM &&
           mode != SD_MODE_WCDMA &&
           mode != SD_MODE_GW &&
           mode != SD_MODE_LTE &&
           mode != SD_MODE_TDS &&
           mode != SD_MODE_GWL
         )
      {
        return FALSE;
      }

      return sys_plmn_match( sid_ptr->plmn.plmn_id,
                             sr2_ptr->sid.plmn.plmn_id );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_MAX:
    default:
      return FALSE;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two SIDs match - return TRUE.
  */
  return TRUE;

} /* sd_is_same_sid */


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
/*===========================================================================

FUNCTION sd_is_same_gw_lac

DESCRIPTION
  Check whether the LAC of two GW system IDs of two system records are equal.


DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the LACs are equal. FALSE otherwise.
  This also return TRUE if we are comparing non-GW systems.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sd_is_same_gw_lac(

        const sd_mode_e_type      mode,
            /* Mode of the sid_ptr.
            */
        const sd_sid_u_type       *sid_ptr,
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr2_ptr.
            */
        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sid_ptr != 0 );
  SD_ASSERT( sr2_ptr != 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if both the modes are GW, where GW includes TD-SCDMA.
  */
  if( (SD_MODE_CONTAIN(sdss_map_mode_to_mode_pref(sr2_ptr->sys.mode),
                      SD_SS_MODE_PREF_GWL) &&
      SD_MODE_CONTAIN(sdss_map_mode_to_mode_pref(mode),
                      SD_SS_MODE_PREF_GWL)) ||
     (SD_MODE_CONTAIN(sdss_map_mode_to_mode_pref(sr2_ptr->sys.mode),
                      SD_SS_MODE_PREF_TDS) &&
      SD_MODE_CONTAIN(sdss_map_mode_to_mode_pref(mode),
                      SD_SS_MODE_PREF_TDS))
    )
  {
     if( sid_ptr->plmn.lac != sr2_ptr->sid.plmn.lac )
     {
       return FALSE;
     }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

}
#endif /*  SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED */


/*===========================================================================

FUNCTION sd_ss_idle_backstop_sys_update

DESCRIPTION
  Update the idle AMPS backstop system per the idle system.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sd_ss_idle_backstop_sys_update( void )
{
  sdsr_s_type       *idl_sys_ptr;
  sd_blksys_e_type  blksys;
  sdsr_e_type       amps_sys;
  sdsr_ref_type     amps_sys_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the idle AMPS backstop system.
  */
  sdsr_list_clr( SDSR_AMPS_BSIDL_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If AMPS backstop is turned OFF, return now.
  */
  if( ! sdprl_is_amps_backstop() )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here AMPS backstop is turned ON.
  */

  /* Point at idle system.
  */
  idl_sys_ptr = sdsr_list_get_ptr( SDSR_IDL_SYS, 0 );
  if(idl_sys_ptr == NULL)
  {
    SD_MSG_LOW_0("idl_sys_ptr NULL");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the cellular side of the idle system. If idle system is not
  ** associated with a cellular side, return now.
  */
  blksys = sd_map_chan_to_blksys( idl_sys_ptr->sys.mode,
          sdss_map_band_pref_to_band(idl_sys_ptr->sys.band),
                                     idl_sys_ptr->sys.chan );

  if( blksys == SD_BLKSYS_CELL_A ) {
    amps_sys = SDSR_AMPS_A_SYS;
  }
  else if( blksys == SD_BLKSYS_CELL_B ) {
    amps_sys = SDSR_AMPS_B_SYS;
  }
  else {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, AMPS backstop is turned on and the idle system is
  ** associated with a cellular side.
  */

  /* If idle system is a PRL listed system, and the idle GEO contains an
  ** AMPS system of the same cellular side as the idle system, set the idle
  ** AMPS backstop system to this AMPS system.
  */
  if( sdss_sr_is_prl(idl_sys_ptr, SDSS_PRL_VAL_LISTED) )
  {
    if( sdsr_list_is_contain(SDSR_IDL_GEO_LST,
                             SDSR_CONTAIN_ALL,
                             amps_sys ))
    {
      sdsr_list_cpy( SDSR_AMPS_BSIDL_SYS, amps_sys );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (the idle system is an available system), if the PRL contains an
  ** AMPS system of the same cellular side as the idle system, set the idle
  ** AMPS backstop system to this AMPS system.
  */
  else
  {
    amps_sys_ref = sdsr_list_get_ref( amps_sys, 0 );

    if( amps_sys_ref == sdsr_list_get_ref(SDSR_AMPS_1ST_SYS, 0) ||
        amps_sys_ref == sdsr_list_get_ref(SDSR_AMPS_2ND_SYS, 0) )
    {
      sdsr_list_cpy( SDSR_AMPS_BSIDL_SYS, amps_sys );
    }
  }

} /* sd_ss_idle_backstop_sys_update */


/*===========================================================================

FUNCTION sd_si_is_full_srv_on

DESCRIPTION
  Indicate whether the full service indicator is currently ON.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the full service is acquired on the given stack.
  FALSE otherwise

  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sd_si_is_full_srv_on(
        sd_ss_e_type              ss
            /* System selection - MAIN or HDR */
)
{

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if service status indicates full service
  */
  return sys_srv_status_is_full_srv( sd_si_ptr(ss)->si_info.srv_status );

} /* sd_si_is_full_srv_on */


/*=============================================================================*/
/* internal functions moved from sd.c which begin with _                       */
/*=============================================================================*/


/*===========================================================================

FUNCTION sd_ss_ptr

DESCRIPTION
  Return the pointer to SS component.


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_s_type*             sd_ss_ptr(
        sd_ss_e_type              ss
        /* System selection item */
)
{

  /* System Selection components.
  */
  static sd_ss_s_type   sd_ss;

  #if ( defined( FEATURE_HDR_HYBRID ))
  static sd_ss_s_type    sd_ss_hybr_1;
  #endif

  #if defined(FEATURE_MMODE_DUAL_SIM) || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
  static sd_ss_s_type    sd_ss_hybr2;
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  static sd_ss_s_type    sd_ss_hybr3;
  #endif

  static sd_ss_s_type*  sd_ss_arr[SD_SS_MAX] =
  {
    &sd_ss,

    #if ( defined( FEATURE_HDR_HYBRID ))
    &sd_ss_hybr_1,
    #else
    &sd_ss,
    #endif

    #if defined(FEATURE_MMODE_DUAL_SIM) || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
    &sd_ss_hybr2,
    #else
    &sd_ss,
    #endif

    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    &sd_ss_hybr3
    #else
    &sd_ss
    #endif

  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss < SD_SS_MAX );

  return sd_ss_arr[ss];

} /* sd_ss_ptr() */

/*===========================================================================

FUNCTION sd_si_ptr

DESCRIPTION

DEPENDENCIES

RETURN VALUE  

SIDE EFFECTS 

===========================================================================*/
sd_si_s_type*             sd_si_ptr(
        sd_ss_e_type              ss
        /* System selection - MAIN or HDR
           Return the pointer to Service indicator component */
)
{
  /* Service Indicators item.
  */
  static sd_si_s_type  sd_si[SD_SS_MAX];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ss < SD_SS_MAX );

  return &(sd_si[ss]);

} /* sd_si_ptr() */


/*===========================================================================

FUNCTION sd_is_opti_ss_pref_change

DESCRIPTION
   Indicate a ss pref change an optimized one if it is not a true preference
   change and the cause of the change is not require polling the scripts.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE   An optimized ss pref change.
  FALSE  Not an optimized ss pref change.

SIDE EFFECTS
  None.

===========================================================================*/
boolean                    sd_is_opti_ss_pref_change(

       sd_ss_e_type               ss,
            /* System selection enum type */

       sdss_pref_updated_e_type   pref_change,
           /* Indicate the ss pref change status */

       sd_ss_pref_reas_e_type     pref_reason,
           /* Reason for changing the SS-Preference */

       sd_ss_orig_mode_e_type     orig_mode,
           /* The orig mode, used to differentiate between DO and UMTS on the hybrid stack */

       sd_ss_pref_update_reason_e_type  pref_update_reason
            /* Prefernce change reason   */

)
{
  sdss_state_e_type          ss_state = sd_get_ss_state(ss);
  /* The current SS state */

  sd_ss_mode_pref_e_type      ss_mode_pref;

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  sys_lte_band_mask_e_type    lte_band_pref;
  
      /* Mode Preference of the current SS */

  /* Variables needed to compute if BSR on hybr 2 stack need to
  ** be restarted and whether it is BSR based on MAIN or HDR stack
  */
  sdsr_e_type                temp_lst;
  sdsr_e_type                temp2_lst;
  sdsr_e_type                pref_lst;
  sdsr_e_type                start_sys;
  boolean                    is_pref_lst_changed   = FALSE;
  boolean                    prev_bsr_state        = FALSE;
  sdsr_s_type                *hybr_1_srv_sys_ptr   = NULL;
  sdsr_s_type                *start_sys_ptr        = NULL;
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  sd_si_info_s_type         *si_info_ptr = &sd_si_ptr(ss)->si_info;
  boolean                    is_hybr2_in_opr       = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( pref_change, SDSS_PREF_UPDATE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( pref_reason, SD_SS_PREF_REAS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS pref reason is RESTORE/RESTORE USER PREF, return action CONTINUE without proceeding
  ** to the scripts. This is used to update the SD core with the UE
  ** preferences without generating any new action
  */
  if( pref_reason == SD_SS_PREF_REAS_RESTORE ||  pref_reason == SD_SS_PREF_REAS_RESTORE_USER_PREF)
  {
    return TRUE;
  }
  
   
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset is_full_srv_req_emerg while unforcing the emergency call preferences */ 
  if ((pref_reason != SD_SS_PREF_REAS_ORIG_START_CS) &&
      (pref_reason != SD_SS_PREF_REAS_REDIAL_NORM) &&
      (pref_reason != SD_SS_PREF_REAS_3GPP_PS_CALL_CONNECT) &&
      (orig_mode != SD_SS_ORIG_MODE_EMERG_ORIG) &&
      sdss_get_is_full_srv_req_during_emerg(ss))
  {
    SD_MSG_HIGH_1("Reset full srv req flag due to pref_reas %d",pref_reason);
    sdss_update_is_full_srv_req_during_emerg( ss, FALSE);
  }

   /* Reset emerg in roam flag while unforcing the emergency call preferences */
  if ((pref_reason != SD_SS_PREF_REAS_ORIG_START_CS) &&
      (pref_reason != SD_SS_PREF_REAS_REDIAL_NORM) &&
      sdss_get_e911_in_roam(ss))
  {
    SD_MSG_HIGH_0("Reset e911 roam flag ");
	sdss_set_e911_in_roam(ss,FALSE);
  }
  
   /* Reset emerg in roam flag while unforcing the emergency call preferences */
  if (sdss_is_emerg_in_roam_for_volte_specialization(ss) &&
      sdss_get_e911_in_roam(ss) == FALSE)
  {
    SD_MSG_HIGH_0("Set e911 roam flag ");
    sdss_set_e911_in_roam(ss,TRUE);
  }

  /* Below addresses when PM UE MODE changes from CSFB -> CSFB_PREF & vice-versa
  ** since no pref change , we can filter it here
  */
  if (pref_reason == SD_SS_PREF_REAS_USER &&
      pref_update_reason == SD_SS_PREF_UPDATE_REASON_SWITCH_UE_MODE &&
       (pref_change == SDSS_PREF_UPDATED_NONE ||
        pref_change == SDSS_PREF_UPDATED_PREF_REAS ) &&
       ss == SD_SS_MAIN)
  {
    SD_MSG_HIGH_0("ue_mode_change,pref_reas_user,no pref_chg");
    return TRUE;
  }

  if ((pref_reason == SD_SS_PREF_REAS_3GPP_PS_CALL_CONNECT) &&
      (sdss_get_ss_state(ss) != SDSS_STATE_OPR))
  {
    SD_MSG_HIGH_2("PS_CALL_CONNECT preferences and ss-state is not opr %d %d",pref_reason, sdss_get_ss_state(ss));
    return TRUE;
  }
  
  sdss_get_mode_band_pref( ss,
                           &ss_mode_pref,
                           NULL,
                           NULL,
                           NULL );

  /* If SS pref reason is REDIAL NORM and RLF recovery is in progress during
  ** an emergency call , return action CONTINUE without proceeding
  ** to the scripts.
  */
  if( pref_reason == SD_SS_PREF_REAS_REDIAL_NORM &&
      orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG &&
      sdss_get_is_RLF_during_emerg(ss) == TRUE )
  {
    /* If LTE is still in the mode preference for redial continue with RLF
    */
    if(!(SD_IS_BIT_CONTAIN (ss_mode_pref ,SD_SS_MODE_PREF_LTE)))
    {
      SD_MSG_HIGH_0("LTE removed. Reset RLF");
      sdss_reset_lte_connected_mode (SD_SS_MAIN);
      return FALSE;
    }
    else
    {
      SD_MSG_HIGH_0("RLF recovery in progress");
      return TRUE;
    }
  } //RLF during emerg silent redial

  #ifdef FEATURE_SD_LTE
  if ((si_info_ptr->mode == SD_MODE_LTE) &&
      (pref_reason == SD_SS_PREF_REAS_REDIAL_NORM) &&
      (orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG) &&
      (ss_state != SDSS_STATE_ACQ) &&
      !(SD_IS_BIT_CONTAIN (ss_mode_pref ,SD_SS_MODE_PREF_LTE)))
  {
    if (!sdss_is_cs_srv_domain_valid_for_voice(ss))
    {
      SD_MSG_HIGH_0("LTE can't provide voice servcies.Go to core");
      return FALSE;
    }
  }
  #endif
  
   /* if e911 orig is due to PS redial and 
          reacq is not attempted for full srv     */
   if(cmregprx_check_lte_priority_scan(ss) && 
     sdss_eng_stack_get_state(ss) != SDSS_ACQ_STT_REACQ_FULL_SRV)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If SS-Preferences (main) is not changed and the reason for the
  ** SS-Preference indication is other than USER or REDIAL_OPTI (optimized
  ** silent redial), and if pref_reas was not changed for originating a call
  ** and not in pwr_save. just return CONTINUE.
  */
  if(  orig_mode != SD_SS_ORIG_MODE_COLLOC              &&
       si_info_ptr->srv_status != SYS_SRV_STATUS_PWR_SAVE )
  {

    if(  ( pref_change == SDSS_PREF_UPDATED_NONE       ||
           pref_change == SDSS_PREF_UPDATED_PREF_REAS )
                                                             &&

        (   pref_reason == SD_SS_PREF_REAS_REDIAL_NORM  ||
            pref_reason == SD_SS_PREF_REAS_ORIG_END     ||
            pref_reason == SD_SS_PREF_REAS_HDR_CDMA_HO
        )
                                                           &&
          !( ( pref_reason == SD_SS_PREF_REAS_ORIG_START_CS ||
               pref_reason == SD_SS_PREF_REAS_ORIG_START_PS )
                                &&
             ( pref_change == SDSS_PREF_UPDATED_PREF_REAS ||
               ss_state    == SDSS_STATE_PWR_SAVE )
           )
      )
    {
      return TRUE;
    }

  }

  /* When UE has limited reginal service,SD should not try
  ** service aquisition in be case of multimode scan, PS data
  ** resume, origination end and GW PS call connected cases.
  */
  if (( pref_reason == SD_SS_PREF_REAS_ORIG_RESUME ||
        pref_reason == SD_SS_PREF_REAS_MULTIMODE_SCAN ||
        pref_reason == SD_SS_PREF_REAS_ORIG_END ||
        pref_reason == SD_SS_PREF_REAS_3GPP_PS_CALL_CONNECT) &&
        si_info_ptr->srv_status  == SYS_SRV_STATUS_LIMITED_REGIONAL  &&
        !sdss_is_target_cap_3gpp_only(ss) &&
      ( ss_state == SDSS_STATE_VRFY ||  ss_state == SDSS_STATE_OPR )
      )
  {
    SD_MSG_HIGH_2("UE in limited regional service, return CONTINUE, ss_state=%d, pref_reason=%d",
                     ss_state, pref_reason);

    if( pref_reason == SD_SS_PREF_REAS_ORIG_END &&
        sdss_is_pref_chgd(ss, SDSS_PREF_CHGD_ANY) )
    {
      sdss_set_pref_chg_in_ltd_reg(ss,TRUE);
    }

    return TRUE;
  }

  #if defined(FEATURE_LTE_TO_1X)
  /* Return CONTINUE if PS call preferences are unforced while UE is having 1XCSFB call and
   ** UE is in LTE limited regional service. pref reason will come as START_CS as top of the queue is voice call.  */
  if ((pref_reason == SD_SS_PREF_REAS_ORIG_START_CS)  &&
      (si_info_ptr->srv_status  == SYS_SRV_STATUS_LIMITED_REGIONAL)  &&
      ((orig_mode == SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG) ||
       (orig_mode == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG)) &&
      (si_info_ptr->mode  == SD_MODE_LTE)  &&
      (ss_state == SDSS_STATE_VRFY ||  ss_state == SDSS_STATE_OPR)
     )
  {
    SD_MSG_HIGH_0("PS call pref: UE in limited regional service, return CONTINUE");
    return TRUE;
  }
  #endif


  /* If there is
     A) no subscription
     B) UE has stable limited service
     C) UE is not in a call origination
     D) no conflict with system preferences,
     just continue
  */
  if (ss == SD_SS_MAIN &&
      !sdss_is_pref_reason(ss,(uint32)SDSSSCR_PREF_REAS_VAL_CALL_ORIG ) &&
      si_info_ptr->srv_status      == SYS_SRV_STATUS_LIMITED  &&
      ss_state                == SDSS_STATE_OPR &&
      sdss_get_subsc_type(ss) == SDSS_SUBSC_NONE &&
      !sdss_srv_sys_is_pref_conf(ss)
     )
  {
    SD_MSG_HIGH_0("no subsc, clear EMERG_SYS/EMERG_LST and continue ");
    sdsr_list_clr( SDSR_EMERG_SYS );
    sdsr_list_clr( SDSR_EMERG_LST );	
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  /* Get the pref info of the current ss
  */
  sdss_get_mode_band_pref( ss,
                           &ss_mode_pref,
                           NULL,
                           &lte_band_pref,
                           NULL );

  /* If we are forcing the C/C+G preferences to main because of Stack Sync-Up
  ** and main is already in Power Save mode just return CONTINUE
  */
  if( ss == SD_SS_MAIN &&
      pref_reason == SD_SS_PREF_REAS_STACK_SYNC_UP &&
      ss_state == SDSS_STATE_PWR_SAVE
    )
  {
    /*
    ** Kick MAIN if:
    ** 1. Adding back GWL and
    ** 2. Either HYBR in srv or
    ** 3. MAIN in HYBR BSR state for FEATURE_MMODE_MDM_HYBR_BSR
    **    (Fusion/MDM now removed)
    ** ELSE CONTINUE
    */
    if (!(( sd_si_is_srv_on( SD_SS_HYBR_1 ) ||
             sd_si_is_srv_on( SD_SS_HYBR_2 ) )
          && SD_MODE_CONTAIN(ss_mode_pref, SD_SS_MODE_PREF_GWL)
                 )
        &&
         (! sdss_is_resel_state(ss, SDSS_RESEL_STT_HYBR_BSR))
       )
    {
      return TRUE;
    }
  }

  /* If forcing the preferences to main which is not due to Stack Sync-Up
  ** and if stack sync-up is in progress then reset the stack sync-up flag
  ** as we might not return to the stack sync-up.
  */
  if( ss == SD_SS_MAIN &&
      pref_reason != SD_SS_PREF_REAS_STACK_SYNC_UP &&
      pref_reason != SD_SS_PREF_REAS_3GPP_PS_CALL_CONNECT)
  {
    sdss_set_flag(ss, SDSS_FLAG_SYNC_UP_IN_PROGRESS, FALSE);
  }

   /*Set force kick flag to FALSE is pref_reason is USER as it will brign HYBR-2
   to OOS/BSR timeline based on availability of 1X service.
     */
  if ((ss == SD_SS_HYBR_2)                          &&
      ((pref_reason == SD_SS_PREF_REAS_USER) ||
       (pref_reason == SD_SS_PREF_REAS_USER_RESEL)) &&
      sdss_is_flag_set(SD_SS_HYBR_2, SDSS_FLAG_IS_FORCE_KICK_HYBR2))
  {
    sdss_set_flag(SD_SS_HYBR_2, SDSS_FLAG_IS_FORCE_KICK_HYBR2, FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sdss_is_1x_sxlte())
  {
    /* If HYBR_2 gets a kick, update the hybr2 srv_pref_lst and decide if
    ** scripts need to be invoked
    */
    if( ss          == SD_SS_HYBR_2    )
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      /* emergency call related checks
      */
      if(sdss_is_orig_mode( ss, SD_SS_ORIG_MODE_VAL_EMERG))
      {
        /* If SD is in an emergency call, block all pref changes from CM other than
        ** call related ones or CM is not in AC EMERG state (Thermal/Mand PDN disabled emerg)
        */ 
        if(!sdss_is_pref_reason(ss,(uint32)SDSSSCR_PREF_REAS_VAL_CALL_ORIG) &&
           !sdss_is_pref_reason(ss,(uint32)SDSSSCR_PREF_REAS_VAL_ORIG_END)&&
           !(pref_update_reason == SD_SS_PREF_UPDATE_REASON_AC_EMERG))
        {
          SD_MSG_HIGH_0("LTE in emerg - block pref change ");
          return TRUE;
        }	
        /* filter out ECBM enter indication ,if SD HYBR2 is in opr state.
        ** if UE is camped on limited srv, this will avoid sending out an 
        ** additional service request 
        */ 
        else if (sdss_is_orig_mode( ss, SD_SS_ORIG_MODE_VAL_EMERG_CB) &&
                 sd_si_is_srv_on( ss ) )
        {
          SD_MSG_HIGH_0("ECBM:in srv");
          return TRUE;
        }
      }	
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      /* If MMSS is not enabled, do not do BSR based on 1x or HDR system.
      */
      if( !sdss_is_mmss_operation(ss) )
      {
        SD_MSG_HIGH_0("SCSVLTE:mmss not enabled");
        return FALSE;
      }

  	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      if ( pref_reason == SD_SS_PREF_REAS_STACK_SYNC_UP && 
           ss_state == SDSS_STATE_OPR &&
           sd_si_is_srv_on( SD_SS_HYBR_2 ))
  	{
          /* if already LTE in OPR State in Hyb2, skip scripts
          */
          SD_MSG_HIGH_0("SCSVLTE:LTE OPR State in HYB2,skip scripts");
          return TRUE;
  	}
      /* Ignore LTE call end provided its not for an emergency call
      ** When emergency call ends on HYBR2 it might be in power save The call end 
      ** should be a trigger to resume operation 
      */
  	if( (si_info_ptr->srv_status == SYS_SRV_STATUS_PWR_SAVE) &&
          (pref_reason == SD_SS_PREF_REAS_ORIG_END) &&
           !sdss_is_prev_orig_mode( ss, SD_SS_ORIG_MODE_VAL_EMERG)
         )
      {
        SD_MSG_HIGH_1("SCSVLTE:ignore CALL END, srv_status:%d",si_info_ptr->srv_status);
        return TRUE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      /* Check if service is acquired on MAIN and/or HDR.
      ** Depending on which stack has acquired service , it will be necessary to
      ** realign the HYBR2 search list accordingly
      */
    
     if ( sd_si_is_full_srv_on( SD_SS_MAIN ) && sd_si_is_srv_on( SD_SS_HYBR_1 ))
     {
       pref_lst = SDSR_HYBR_1_BACKUP_LST;
       sdsr_list_cpy(pref_lst, SDSR_SRV_PREF_LST);
       /* Check if HDR stack more pref list is greater than 0 else use more pref list computed based
       ** on main stack
       */

       if (sdsr_list_cnt(SDSR_MORE_PREF_LST_) > 0)
       {
          sdsr_list_intersect(pref_lst, SDSR_MORE_PREF_LST_);
       }
        start_sys = SDSR_SRV_SYS;
      }
      else if( sd_si_is_full_srv_on( SD_SS_MAIN )&&
         (sd_get_ss_state(SD_SS_MAIN) == SDSS_STATE_OPR 
          || (sdss_get_scan_scope_rule() & SYS_SCAN_SCOPE_RULE_ACQ_DB_NO_TRM
              && sd_si_ptr(SD_SS_MAIN)->si_info.mode == SD_MODE_CDMA 
              && sdss_is_resel_state(SD_SS_MAIN, SDSS_RESEL_STT_NONE) == FALSE)))
      {
        pref_lst = SDSR_SRV_PREF_LST;
        start_sys = SDSR_SRV_SYS;
      }
      /* Check if HDR is full srv or HDR deactivated due to GWL acq on Hybr2 */
      else if((sd_si_is_srv_on( SD_SS_HYBR_1 )&&
              (sd_get_ss_state(SD_SS_HYBR_1) == SDSS_STATE_OPR)) || 
               sdss_is_flag_set(SD_SS_HYBR_2, SDSS_FLAG_HDR_DEACT_BY_HYBR2_BSR))
      {
        pref_lst = SDSR_MORE_PREF_LST_;
        start_sys = SDSR_HDR_SRV_SYS;
      }
      else
      {
        /* During IRAT When MAIN and HDR is not in operation 
              ** continue with Hybr2 BSR and so that timer will be started
              ** till HDR activity is done.
              ** Else SD will enter OOS routine and issue LTE acq and deactivate HDR
              ** during IRAT
              */
        if(sdss_is_flag_set(ss,SDSS_FLAG_HYBR_HDR_IRAT ))
        {
          /* Since pref list is not filled use MMSS_GWL list to look for LTE
                  ** incase if 1x or HDR is not acquired when the timer expires.
                 */
          SD_MSG_HIGH_0("SCSVLTE:IRAT 1x/HDR not in OPR");
          pref_lst = SDSR_MMSS_GWL_LST;
          start_sys = SDSR_HDR_SRV_SYS;
        }
        else
        {
           
        /* If there is no 1x or HDR service , reset BSR flag and continue
        ** to the scripts to follow HYBR 2 oos timeline
        */
        sdss_set_flag(SD_SS_HYBR_2,SDSS_FLAG_HYBR2_BSR, FALSE);
        SD_MSG_HIGH_3("SCSVLTE:no srv hybr2 state %d main-mode %d main-is-not-in-resel %d",
          sd_ss_ptr(SD_SS_HYBR_2)->ss_state,
          sd_si_ptr(SD_SS_MAIN)->si_info.mode,
          sdss_is_resel_state(SD_SS_MAIN, SDSS_RESEL_STT_NONE));
        return FALSE;
        }
      }

      /* record HYBR2's current BSR state
      */
      prev_bsr_state = sdss_is_flag_set(SD_SS_HYBR_2,SDSS_FLAG_HYBR2_BSR);


	  /* We can clear the flag if pref_reason is USER & ss_mode_pref doesn't contain LTE or 
	  ** lte band pref is 0 or intersection of lte band pref & target LTE band cap is 0.
      */
	  if (pref_reason == SD_SS_PREF_REAS_USER && 
	  	  !sd_misc_is_target_supp_mode_band(ss_mode_pref, SD_SS_BAND_PREF_NONE, lte_band_pref, SD_SS_BAND_PREF_NONE))
	  {
		sdss_set_flag(SD_SS_HYBR_2,SDSS_FLAG_HYBR2_BSR, FALSE);
	  }
	  else
	  {
	    /* Set flag to indicate HYBR2 is in BSR state
          */
      sdss_set_flag(SD_SS_HYBR_2,SDSS_FLAG_HYBR2_BSR, TRUE);
	  }

      if(prev_bsr_state == FALSE &&
         sdss_is_flag_set(SD_SS_HYBR_2,SDSS_FLAG_HYBR2_BSR))
      {
        SD_MSG_HIGH_0("Starting HYBR_2 BSR, reset tele BSR timer for SD_SS_HYBR_2");
        sdss_lte_telescope_parm_reset(SD_SS_HYBR_2);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      /* update the hybr2 scan list.
      ** Delete all non compliant systems from the pref_lst and copy it to the
      ** backup list
      */
      sdsr_list_cpy(SDSR_HYBR_1_BACKUP_LST, pref_lst);
      sdsr_list_del_sys(SD_SS_HYBR_2,SDSR_HYBR_1_BACKUP_LST,SDSR_CAT_NON_COMPLY);
      pref_lst = SDSR_HYBR_1_BACKUP_LST;

     /* If MAIN/HDR stack is not in OPR state and stack syncup bsr is received and
         ** no IRAT flag is set just return without entering the script. Ideally
         ** this should not happen per design.
         */
      if ( (pref_reason == SD_SS_PREF_REAS_STACK_SYNC_UP_BSR) && 
           (sd_get_ss_state(SD_SS_MAIN) != SDSS_STATE_OPR) && 
           (sd_get_ss_state(SD_SS_HYBR_1) != SDSS_STATE_OPR) && 
           !sdss_is_flag_set(ss,SDSS_FLAG_HYBR_HDR_IRAT ) )
      { 
        SD_MSG_HIGH_0("SCSVLTE: Stack syncup BSR when 1x/HDR not in OPR and no IRAT");
        return TRUE;
      }

      /*If the number of more preferred systems on MAIN/HDR is different from the
      ** number of more pref list on HYBR 2 , set is_pref_lst_changed
      */
      if( sdsr_list_cnt( SDSR_HYBR_1_SRV_PREF_LST ) != sdsr_list_cnt( pref_lst ))
      {
        is_pref_lst_changed = TRUE;
      }
      /* If new pref list on HYBR2 is zero, allow to continue to scripts which
      ** will set the HYBR 2 state to PWR SAVE and prevent further ACQ attempts
      ** This is required if HYBR2 is in OOS timeline and UE acquires a 1x or DO
      ** which has no LTE systems in the same GEO.
      */
      else if( sdsr_list_cnt( pref_lst ) == 0)
      {
        is_pref_lst_changed = TRUE;
      }
      /* else compare contents of the 2 lists. If they are different
      **  set is_pref_lst_changed
      */
      else
      {
        temp_lst  =  sdss_sr_list_map2( ss, SDSR_TEMP_LST );
        temp2_lst =  sdss_sr_list_map2( ss, SDSR_TEMP2_LST );

        /* Copy the to, from lists into temp lists.
        */
        sdsr_list_cpy( temp_lst, pref_lst);
        sdsr_list_cpy( temp2_lst, SDSR_HYBR_1_SRV_PREF_LST);

        /* Sort the temp lists before comparing
        */
        sdsr_list_sort( temp_lst, SDSR_COMP_PRL_ACQ);
        sdsr_list_sort( temp2_lst, SDSR_COMP_PRL_ACQ);


        is_pref_lst_changed = !( sdsr_list_is_same( temp_lst,
                                                    temp2_lst,
                                                    SDSR_EQUAL_MODE )
                               &&

                               sdsr_list_is_same( temp_lst,
                                                  temp2_lst,
                                                  SDSR_EQUAL_BAND )
                               &&

                               sdsr_list_is_same( temp_lst,
                                                  temp2_lst,
                                                  SDSR_EQUAL_CHAN )
                              );
      }

      SD_MSG_HIGH_3("SCSVLTE:pref lst cnt %d %d, pref_chgd %d",
               sdsr_list_cnt( SDSR_HYBR_1_SRV_PREF_LST ),
               sdsr_list_cnt( pref_lst ),
               is_pref_lst_changed);


      /* If pref lst has changed , update the hybr2 stack with the new one
      ** and continue to the scripts , so that the new list will take effect
      */
      if( is_pref_lst_changed )
      {
        sdsr_list_cpy( SDSR_HYBR_1_SRV_PREF_LST, pref_lst );
        sdsr_list_cpy( SDSR_HYBR_1_START_PREF_LST, pref_lst );
        sdsr_list_cpy( SDSR_HYBR_1_START_SYS,start_sys);
      }

      /*If pref change is due to HYBR BSR to HDR end allow it to go through */
      if ( pref_change == SDSS_PREF_UPDATED_HYBR_BSR_TO_HDR_END &&
           pref_reason == SD_SS_PREF_REAS_STACK_SYNC_UP_BSR &&
           sdss_is_resel_state(ss, SDSS_RESEL_STT_MORE_PREF_HDR_SCAN))
      {
        SD_MSG_HIGH_0("LTE_TO_HDR: pref change Hybrid BSR END ");
        return FALSE;

      }
      if( pref_change != SDSS_PREF_UPDATED_HYBR_BSR_TO_HDR_END  &&
                pref_reason == SD_SS_PREF_REAS_STACK_SYNC_UP_BSR &&
                sdss_is_resel_state(ss, SDSS_RESEL_STT_MORE_PREF_HDR_SCAN ))
      {
        SD_MSG_HIGH_0("LTE_TO_HDR: Hybrid bsr in progress ignore stack syncup bsr ");
        return TRUE;
      }
        

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      /* If ALT_CDMA_BSR is on, do LTE Available File Front-End operation */
      if(sdss_is_c2k_alt_algr_operation())

      {
        sdss_lte_avail_e_type curr_lte_avail;
        sdss_lte_avail_e_type prev_lte_avail;

        sdss_get_lte_avail_1x(&prev_lte_avail, &curr_lte_avail);

        /* CDMA service is available */
        if(sd_si_is_srv_on( SD_SS_MAIN )&&
           (sd_si_ptr( SD_SS_MAIN )->si_info.mode == SYS_SYS_MODE_CDMA))
        {
          /* If the system is moving from LTENotAvail to NOT in LTENotAvail
          ** (either LTE Available or never scanned before), then even if the
          ** PREF_LST is not changed, trigger LTE scan immediately.
          ** ELSE, we skip this check and trigger LTE scan based on PREF_LST
          */
          sd_ss_mode_pref_e_type  ss_mode_pref        = SD_SS_MODE_PREF_NONE;
          if(curr_lte_avail!= SDSS_LTE_AVAIL_NOT_AVAIL &&
            (prev_lte_avail == SDSS_LTE_AVAIL_NOT_AVAIL))
          {
            SD_MSG_HIGH_0("SCSVLTE(FALSE):Moving from LteNotAvail to NOT in LteNotAvail");

            /* Before kick off 1x to LTE BSR, store the prev_lte_avail_1x to
            ** the current one so we don't trigger another BSR with same reason */
            sdss_update_lte_avail_1x(curr_lte_avail);

            return FALSE;
          }
          /* apply below logic only if LTE is present in mode_pref, else go to the core, and 
        ** shut down hybrid-2 stack */
          sdss_get_mode_band_pref( ss, 
                                 &ss_mode_pref, 
                                 NULL, 
                                 NULL,
                                 NULL);
          /* If the system is in LTENotAvail file, and the BSR is in progress,
          ** no need for immediate BSR but let the current BSR timer expires.
          ** This needs to be done before we check pref_reason, since we should
          ** CONTINUE regardless of pref reason
          */
          if(sd_misc_is_mode_pref(ss_mode_pref, SD_SS_MODE_PREF_LTE) &&
           curr_lte_avail == SDSS_LTE_AVAIL_NOT_AVAIL &&
           prev_bsr_state == TRUE)
          {
            /* If HYBR-2 is in power save, we should send acq-fail notification
            ** so that processing can start on HYBR-HDR stack.
            */
            if(sd_si_ptr( SD_SS_HYBR_2 )->si_info.srv_status == SYS_SRV_STATUS_PWR_SAVE)
            {
              sd_si_acq_fail( SD_SS_HYBR_2 );
            }
            SD_MSG_HIGH_0("SCSVLTE(TRUE):1x in LteNotAvail, RESEL in progress");
            return TRUE;
          }

        }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      /* if pref reas is BSR or
      ** pref reas = ORIG_END ( when SD is already in operation state on LTE with full srv)
      ** it is not necessary to go the scripts. check the remaining conditions
      ** to decide whether SD can return continue. For instance if LTE is more preferred than 1x 
      ** orig end should not cause SD to issue a new scan on LTE 
        */
      if( pref_reason != SD_SS_PREF_REAS_STACK_SYNC_UP_BSR &&
         !((pref_reason == SD_SS_PREF_REAS_ORIG_END) && 
            (ss_state == SDSS_STATE_OPR) &&
             sd_si_is_full_srv_on( SD_SS_HYBR_2 )
          )
        )
      {
        SD_MSG_HIGH_0("SCSVLTE:pref_reas not BSR/orig_end");
        return FALSE;
      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */

      if( sdss_is_flag_set(ss,SDSS_FLAG_HYBR_HDR_IRAT ))
      {
        /* if stack sync up BSR is due to LTE->DO resel, go to scripts */
        SD_MSG_HIGH_0("SCSVLTE:IRAT flag set");
        return FALSE;
      }

      if(sdss_is_pref_update_reas(ss, SD_SS_PREF_UPDATE_REASON_TRM_UNLOCK) &&
         sdss_get_acq_status_only(ss) == SYS_ACQ_STATUS_NO_RESOURCE)
      {
        SD_MSG_HIGH_1("Prev acq failed on %d due to no_resource, retry", ss);
        return FALSE;
      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      /* Check for hybr2 stack's state , whether it is in OPR state.
      ** for SVLTE_G, always return TRUE, irrepsective of hybr2 state, as we need to skip 
      **              this check for SVLTE+G.
      ** For other configs(SS SVLTE/SRLTE/SRLTE+G),
      **   If HYBR2 stack is in OPR state, mark is_hybr2_in_opr TRUE
      **   Else, mark is_hybr2_in_opr to FALSE.
      */
      if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) &&
         sdss_is_sfmode_svlte( (uint8)SD_SS_MAIN))
      {
        SD_MSG_HIGH_0("SCSVLTE:is_hybr2_in_opr: set to TRUE for SVLTE+G");
        is_hybr2_in_opr = TRUE;
      }
      else 
      {
         if(sdss_get_ss_state( SD_SS_HYBR_2 ) == SDSS_STATE_OPR )
         {
           SD_MSG_HIGH_0("SCSVLTE:is_hybr2_in_opr: set to TRUE");
           is_hybr2_in_opr = TRUE;
         }
         else
         {
           SD_MSG_HIGH_0("SCSVLTE:is_hybr2_in_opr: set to FALSE");
           is_hybr2_in_opr = FALSE;
         }
      }

      /* Check if full service is acquired on HYBR2 & it is preferred service
      ** as compared to current 1x or HDR service.
      ** Do not perform this check if LTE->DO irat indication is turned on.
      ** In this case, the LTE srv indication is still set to TRUE even though in
      ** reality LTE service is lost and lower layers deactivated.
      */
      hybr_1_srv_sys_ptr = sdsr_list_get_ptr( SDSR_HYBR_1_SRV_SYS, 0 );
      start_sys_ptr = sdsr_list_get_ptr( start_sys, 0 );

      if( hybr_1_srv_sys_ptr != NULL &&
          start_sys_ptr != NULL &&
          sd_si_is_full_srv_on( SD_SS_HYBR_2 ) &&
          is_hybr2_in_opr == TRUE &&
          sdss_sr_is_prl(hybr_1_srv_sys_ptr, SDSS_PRL_VAL_ANY_PREF)  &&
          !sdss_sr_is_pref_conf(SD_SS_HYBR_2, hybr_1_srv_sys_ptr, SDSS_PREF_CONF_ANY) &&
          sdprl_is_more_pref_upto(ss, hybr_1_srv_sys_ptr, start_sys_ptr, SDPRL_COMP_UPTO_GEO_POS)
        )                              
      {
        /*On preferred service already. No need to start BSR.
        */
        SD_MSG_HIGH_0("SCSVLTE:Best pref srv - cont");

        /* Clear SRV_PREF_LST when we declare best pref srv.
        ** While we are in LTE best pref srv, stack sync up can come multiple
        ** times. In that case, we don't want to keep the PREF_LST. When we lose
        ** LTE next time, PREF_LST will be recalculated based on serving 1x or HDR
        ** system */
        sdsr_list_clr(SDSR_HYBR_1_SRV_PREF_LST);
        return TRUE;
      }
        
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
      /* If pref lst has changed ,  continue to the scripts ,
      ** so that the new list will take effect
      */
      if( is_pref_lst_changed )
      {
        SD_MSG_HIGH_0("SCSVLTE:pref lst changed");
        return FALSE;
      }

      else if( prev_bsr_state == FALSE)
      {
        /* if HYBR2 is moving from OOS to BSR state */
        SD_MSG_HIGH_0("SCSVLTE:OOS to BSR");
        return FALSE;
      }
      else if(sdss_is_loc_bsr_change(ss))
      {
        SD_MSG_HIGH_0("SCSVLTE:loc bsr changed");
        sdss_set_loc_change(ss,FALSE);
        return FALSE;
      }
      /* If there is no change to the pref list and no change to BSR state,
      **  allow hybr 2 operation to continue as is,without going to the scripts
      */
      else
      {
        SD_MSG_HIGH_0("opti_ss_pref==TRUE. Send ACQ_FAIL");
        /* If no pref list has changed, and HYBR-2 is in power save
        ** we should send acq-fail notification so that processing can start
        ** on HYBR-HDR stack. In this case, ss == SD_SS_HYBR_2.
        */
        if (si_info_ptr->srv_status == SYS_SRV_STATUS_PWR_SAVE)
        {
          sd_si_acq_fail( SD_SS_HYBR_2);
        }
        return TRUE;
      }
    } // if(ss == SD_SS_HYBR_2 )
  } /* If (SVLTE ) */

  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Preferences(HDR) is not changed and the reason for the SS-Preference
  ** indication is REDIAL_NORM, just return CONTINUE. If AT is in
  ** power save, don't continue if the user changed the preferences or call
  ** is originating
  **
  ** If SS-Preferences(HDR) arent changed and the reason for the SS-Preference
  ** indication is ORIG_END, in SXLTE configuration ensure that UE switched to
  ** OOS algorithm immediately.
  */
  if( ( ss == SD_SS_HYBR_1 )
       && ( orig_mode == SD_SS_ORIG_MODE_COLLOC ) )
    {
      if( pref_change != SDSS_PREF_UPDATED_TRUE_PREF
          && pref_change != SDSS_PREF_UPDATED_SRV_DOMAIN
          && pref_change != SDSS_PREF_UPDATED_UE_CAPABILITY_CHG
          &&
          (( pref_reason == SD_SS_PREF_REAS_ORIG_END   &&
             si_info_ptr->srv_status != SYS_SRV_STATUS_SRV &&
             !sdss_is_1x_sxlte())
           ||
          ( pref_reason == SD_SS_PREF_REAS_REDIAL_NORM &&
             si_info_ptr->srv_status != SYS_SRV_STATUS_PWR_SAVE )
          )
        )
      {
        return TRUE;
      }
    }

  #ifdef FEATURE_HICPS_STACK_SYNC_UP

  /* If we are forcing the preferences to HDR because of Stack Sync-Up
  ** then filter the event If a call origination is happening on HDR.
  ** just return CONTINUE
  */
  if( ( ss == SD_SS_HYBR_1 ) &&
      (pref_reason == SD_SS_PREF_REAS_STACK_SYNC_UP &&
       sdss_is_pref_reason(ss,(uint32)SDSSSCR_PREF_REAS_VAL_CALL_ORIG))
    )
  {
      return TRUE;
  }

  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

}/* sd_is_opti_ss_pref_change */


/*===========================================================================

FUNCTION sd_si_ok_to_orig

DESCRIPTION
  Notify the registered client (i.e., CM) that it is OK to orig.

DEPENDENCIES
  Call after sd_si_update() and sd_si_update_data_suspend_state() have been
  called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                      sd_si_ok_to_orig(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR */

        boolean                   data_suspend
            /* Data suspend status  */
)
{
  sd_si_s_type                   *si_ptr     = sd_si_ptr( ss );
  sd_si_ok_to_orig_s_type        si_ok_to_orig;
  sd_si_info_s_type              *si_info_ptr = &si_ptr->si_info;
  sd_ss_mode_pref_e_type         mode_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send ok to orig if:
  ** 1.ss is hybr3 and camp mode is on 1st sub 
  ** 2.Current mode is not 3gpp(=CDMA or HDR)
  */
  
  if( sdss_get_camp_mode() == SD_SS_PREF_CAMP_MODE_ON && 
     !(sdss_is_1x_sxlte() && ss == SD_SS_HYBR_3) &&
      (si_info_ptr->mode      != SD_MODE_CDMA &&
       si_info_ptr->mode      != SD_MODE_HDR))
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdss_get_mode_band_pref(ss,&mode_pref,NULL,NULL,NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If UE is in EMERG_ORIG mode and has LTE service without emerg support,
  ** do not send ok to orig
  */
  if( sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_EMERG_ORIG &&
      si_info_ptr->mode           == SD_MODE_LTE &&
      !sdss_is_lte_ims_emerg_allowed()
    )
  {
    SD_MSG_HIGH_0("No ok_to_orig.LTE does'nt support emerg");
    return;
  }

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined( FEATURE_SD_LTE )
  if (sdss_is_emer_orig_in_volte_only(ss) && 
      (si_info_ptr->srv_status != SYS_SRV_STATUS_SRV) &&
      (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE))
  {
    SD_MSG_HIGH_0("No ok_to_orig as LTE FULL SRV is not possible");
    return;
  }

  /* In romaing send ok_to_orig is csfb is supported */
  if(sdss_is_emerg_in_roam_for_volte_specialization(ss) &&
	 (si_info_ptr->mode      == SD_MODE_LTE) &&
     (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE) && 
    !(si_info_ptr->srv_status        == SYS_SRV_STATUS_SRV &&
      si_info_ptr->cs_srv_status     == SYS_SRV_STATUS_SRV &&
      si_info_ptr->srv_domain        == SYS_SRV_DOMAIN_CS_PS &&
      si_info_ptr->lte_cs_capability == SYS_LTE_CS_CAPABILITY_NO_VALUE &&
      mode_pref & SD_SS_MODE_PREF_TDS_GSM_WCDMA)
    )
  {
    SD_MSG_HIGH_0("No ok_to_orig as LTE full/limited srv will ignored in roaming during e911 call ");
    return;
  }

  #endif
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If orig-mode is other than none, inform the registered client (i.e., CM)
  ** that it is OK to originate.
  */
  if( sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_ANY) )
  {
    if( si_ptr->ok_to_orig_cb_func != NULL )
    {
      SD_MSG_HIGH_1( "OK to orig ss=%d",ss);

      si_ok_to_orig.mode            = si_info_ptr->mode;
      si_ok_to_orig.srv_domain      = si_info_ptr->srv_domain;
      si_ok_to_orig.srv_status      = si_info_ptr->srv_status;
      si_ok_to_orig.ps_data_suspend = data_suspend;

      /* if optimized lte->ehprd handoff is in progress, even though HDR is not yet in operation
         ** state, if a hdr data call is originated (HDR has entered traffic), SD should send
         ** ok_to_orig with mode = hdr to CM.
         ** This is done so that the data call is sent over to HDR right away without waiting for
         ** HDR overhead indication in SD.
         */
      if (sdss_is_opt_ho_lte_do() && ss == SD_SS_HYBR_1)
      {
        si_ok_to_orig.mode            = SYS_SYS_MODE_HDR;
        si_ok_to_orig.srv_domain      = SYS_SRV_DOMAIN_PS_ONLY;
      }

      si_ptr->ok_to_orig_cb_func( ss, &si_ok_to_orig );
    }
  }

} /* sd_si_ok_to_orig */


/*===========================================================================

FUNCTION sd_update_manual_sys

DESCRIPTION
  Update the return_cause indicator per the input SS-Event, redirection
  status, last and current systems attempted acquisition.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                                sd_update_manual_sys(

        sdsr_e_type                         lst,
            /* List to be updated */

        const sys_manual_sys_info_s_type    *man_sys_info_ptr
            /* Pointer to the manual system */

)
{
  /* Point at manual system record.
  */
  sdsr_s_type            *man_sr_ptr;
  int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( lst, SDSR_MAX );
  SD_ASSERT( man_sys_info_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* User has changed the network selection preference and made it
  ** manual. Update the plmn information.
  */
  for( i=0; i < sdsr_list_cnt(lst); i++ )
  {
    man_sr_ptr = sdsr_list_get_ptr( lst, i );
    if (man_sr_ptr == NULL)
    {
      SD_MSG_LOW_0("man_sr_ptr NULL");
      continue;
    }

    /* Check if it GW system.
    */
    switch ( man_sr_ptr->sys.mode )
    {
      case SD_MODE_GSM:
      case SD_MODE_WCDMA:
      case SD_MODE_GW:
      case SD_MODE_LTE:
      case SD_MODE_GWL:
      case SD_MODE_TDS:
      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) ||  defined(FEATURE_SD_LTE)
        if ( man_sys_info_ptr->sys_id.id_type != SYS_SYS_ID_TYPE_UMTS )
        {
          break;
        }

        switch ( man_sys_info_ptr->prm.gw.sel_type )
        {
          case SYS_MANUAL_SEL_TYPE_HPLMN:
            man_sr_ptr->sid_expected.net_select_mode
                                      = SD_NET_SELECT_MODE_HPLMN;
            break;

          case SYS_MANUAL_SEL_TYPE_RPLMN:
            man_sr_ptr->sid_expected.net_select_mode
                                      = SD_NET_SELECT_MODE_MANUAL_SYS;
            sys_plmn_undefine_plmn_id
            (
              &man_sr_ptr->sid_expected.prm.gwl.plmn.plmn_id
            );
            break;

          case SYS_MANUAL_SEL_TYPE_USER_SPECIFIED:
            man_sr_ptr->sid_expected.net_select_mode
                                      = SD_NET_SELECT_MODE_MANUAL_SYS;
            man_sr_ptr->sid_expected.prm.gwl.plmn.plmn_id
                                      = man_sys_info_ptr->sys_id.id.plmn;
            break;

          case SYS_MANUAL_SEL_TYPE_NONE:
          case SYS_MANUAL_SEL_TYPE_MAX:
          default:
            sys_err_fatal_invalid_value_exception((int)man_sys_info_ptr->prm.gw.sel_type);

        } /* switch ( manual_sys_info_ptr->prm.gw.sel_type ) */
        man_sr_ptr->prl = (byte)SDSS_PRL_MOST_PREF;
        #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED */
        break;

      case SYS_SYS_MODE_NONE:
      case SYS_SYS_MODE_NO_SRV:
      case SYS_SYS_MODE_AMPS:
      case SYS_SYS_MODE_CDMA:
      case SYS_SYS_MODE_HDR:
      case SYS_SYS_MODE_GPS:
      case SYS_SYS_MODE_MAX:
      default:
        SD_MSG_HIGH_1("invalid manual mode %d", man_sr_ptr->sys.mode);
        break;

    } /* switch () */

  } /* for ( ... */
#if !(defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) )
SYS_ARG_NOT_USED(man_sys_info_ptr);
#endif
} /* sd_update_manual_sys */
/*===========================================================================

FUNCTION sd_ss_event_proc

DESCRIPTION
  Do SS-Event common processing. In other words, the processing that is
  common to all SS-Events is done in this function.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_event_proc(

        sd_ss_e_type              ss,
            /* The ss to use for the event */

        sdss_evt_e_type           event,
            /* SS-Event to process */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action */
)
{
  sdsr_s_type           *sr_ptr;
  sdss_iact_s_type      *iact_ptr             = NULL;
  #ifdef SD_USES_CAI
  boolean               is_disabling_redir;
  #endif /* SD_USES_CAI */
  sdsr_s_type           *last_acq_sys_ptr;
  sd_mode_e_type        last_mode             = SD_MODE_MAX;
  sd_blksys_e_type      last_blksys           = SD_BLKSYS_MAX;

  sd_ss_s_type          *ss_ptr               = sd_ss_ptr(ss);
  sd_ss_act_s_type      *ss_act_ptr           = NULL;
  sd_ss_act_e_type      act                   = SD_SS_ACT_MAX;
  dword                 uptime                = time_get_uptime_secs();
  uint32                i                     = 0;
  sdss_acq_stt_e_type   acq_stt               = SDSS_ACQ_STT_NONE;
  sd_si_s_type          *si_ptr               = sd_si_ptr( ss );
  sd_si_info_s_type     *si_info_ptr          = &si_ptr->si_info;
  uint32                idx                   = 0;

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined (FEATURE_SD_LTE)
  sd_ss_orig_mode_e_type  ss_orig_mode        = sdss_get_orig_mode(ss);
  sd_ss_pref_reas_e_type  ss_pref_reason      = sdss_get_pref_reason(ss);
  sd_ss_mode_pref_e_type  ss_mode_pref        = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type  ss_band_pref        = SD_SS_BAND_PREF_MAX;
  sys_lte_band_mask_e_type    ss_band_pref_lte;
  sd_ss_band_pref_e_type  ss_band_pref_tds    = SD_SS_BAND_PREF_MAX;
  sdlog_full_srv_req_emerg_s_type srv_req_event;
  sd_ss_srv_domain_pref_e_type lte_rpm_srv_domain_pref = SD_SS_SRV_DOMAIN_PREF_NONE;

  #endif

  #ifdef FEATURE_EOOS_UMTS
  sd_ss_gwl_scan_status_e_type gwl_scan_status;
  boolean use_timer = TRUE;
  #endif

  #ifdef FEATURE_AVOID_DUP_BAND_SCAN
  sdss_band_s_type band_scanned;
  sdss_band_s_type tmp_band;
  sdsr_s_type tmp_sr;
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );
  SD_ASSERT( ss_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize/reset GWL acq state in the internal action structure
  */
  iact_ptr = (sdss_iact_s_type*)modem_mem_alloc(sizeof(sdss_iact_s_type), MODEM_MEM_CLIENT_MMODE);
	
  if (iact_ptr != NULL)
  {
    memset(iact_ptr, 0, sizeof(sdss_iact_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
	
  iact_ptr->prm.acq.gwl_acq_type.acq_type = SDSS_GWL_ACQ_NONE;

  #if ( defined(FEATURE_HDR_HYBRID))
  /* If this is an HDR event and hybrid preference is CDMA+HDR, call on the
  ** HDR event processing function to process this event.
  */
  if( SDSS_EVT_IS_HDR(event) &&
      sd_misc_is_hybr_opr())
  {
    modem_mem_free(iact_ptr,MODEM_MEM_CLIENT_MMODE);
    return sd_ss_hdr_event_proc( event, rtrn_act_ptr );
  }
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Lock the ss component before processing it.
  */
  SD_SS_LOCK( ss_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print the SS-Event.
  */
  sdss_event_group_print( event );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the system that was attempted acquisition last and remember its
  ** mode and blksys.
  */
  last_acq_sys_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), 0 );

  if( last_acq_sys_ptr != NULL )
  {
    last_mode     = last_acq_sys_ptr->sys.mode;
    last_blksys   = sd_map_chan_to_blksys( last_acq_sys_ptr->sys.mode,
                     sdss_map_band_pref_to_band(last_acq_sys_ptr->sys.band),
                                              last_acq_sys_ptr->sys.chan );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef SD_USES_CAI
  /* Remember whether CDMA redirection is ON.
  */
  is_disabling_redir = sdss_is_redir( ss, SDSS_REDIR_VAL_CDMA_ANY );
  #endif /* SD_USES_CAI */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is an acquisition event, update the SDSR_ACQ_SYS as per this
  ** event.
  */
  if( SDSS_EVT_IS_XXX_ACQ(event) )
  {
    sdsr_s_type          *list_sr_ptr = NULL;
    sdsr_e_type           ss_acq_list = SDSR_MAX;
    int                   ss_acq_pos  = -1;
    sdss_acq_mode_e_type  ss_acq_mode = SDSS_ACQ_MODE_MAX;

    sd_ss_read_acq_prms( ss,
                         &ss_acq_list,
                         &ss_acq_pos,
                         &ss_acq_mode );

    list_sr_ptr = sdsr_list_get_ptr( ss_acq_list,
                                     ss_acq_pos );
    sdsr_list_print( SDSR_START_PREF_LST, FALSE );
	SD_ASSERT( list_sr_ptr != NULL );

    sr_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), 0 );
    SD_ASSERT( sr_ptr != NULL );

    /*lint -e774 */
    if( sr_ptr != NULL )
    {
      sr_ptr->event       = (word) event;
      sr_ptr->event_time  = uptime;
    }

    #ifdef FEATURE_EOOS_UMTS
    sdss_get_eoos_scan_gwl_param( ss, &use_timer, NULL, NULL, NULL, NULL,
                                    &gwl_scan_status );

    /* If the scan is aborted, don't update the acq_sys
    */
    if( !use_timer || gwl_scan_status != SD_SS_GWL_SCAN_STATUS_ABORTED ||
        ( ( list_sr_ptr != NULL && (list_sr_ptr->sys.mode != SD_MODE_GSM ) &&
           ( list_sr_ptr->sys.mode != SD_MODE_WCDMA ) &&
           ( list_sr_ptr->sys.mode != SD_MODE_GW )
          ) ||
          ( sr_ptr != NULL && (sr_ptr->sys.mode != SD_MODE_GSM ) &&
           ( sr_ptr->sys.mode != SD_MODE_WCDMA ) &&
           ( sr_ptr->sys.mode != SD_MODE_GW )
          )
        )
      )
    #endif
    {
      /* Do post acquisition attempt updates.
      */
      if( list_sr_ptr != NULL )
      {
        /* Do post acquisition attempt updates.
        */
        sd_ss_acq_update_post( ss,
                               ss_acq_list,
                               ss_acq_pos,
                               ss_acq_mode );
      }
      else if( sr_ptr != NULL )
      {
        /* Do post acquisition attempt updates.
        */
        sd_ss_acq_update_post( ss,
                               sdss_sr_list_map2(ss, SDSR_ACQ_SYS),
                               0,
                               ss_acq_mode );
      }
      sdsr_list_del_list(SDSR_PWR_SCAN_LST, SDSR_ACQ_SYS );
    } /* if */
    #ifdef FEATURE_EOOS_UMTS
    else
    {
      /* The acquisision was aborted, restore the new_acq_cnt
      */
      sr_ptr->new_acq_cnt=0;
    }
    #endif
    /*lint +e774 */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, if this is an operation event, update the SDSR_SRV_SYS and
  ** SDSR_IDL_SYS as per this event.
  */
  else if( SDSS_EVT_IS_XXX_OPR(event) )
  {
    sr_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_SRV_SYS), 0 );

    /*lint -e774 */
    if( sr_ptr != NULL ) {
      sr_ptr->event       = (word) event;
      sr_ptr->event_time  = uptime;
    }
    /*lint +e774 */
    sr_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_IDL_SYS), 0 );

    if( sr_ptr != NULL ) {
      sr_ptr->event       = (word) event;
      sr_ptr->event_time  = uptime;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remember the pervious SS-Action.
  */
  (void)sd_ss_act_get_true_curr_act_ss(ss,&(ss_ptr->prev_act));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_AVOID_DUP_BAND_SCAN

  /* No need to process the following if avoid_state is inactive or none
   */
  if ( !sdss_is_avoid_state_inactive_or_none(ss) )
  {
    sdss_proc_avoid_dup_scan_per_user_ss_pref(ss);

      /* Resume avoid_dup_band_scan if avoid_state is suspend.
      ** Here list the cases below in which avoid_state is suspend
      ** 1. REACQ/REACQ_GWL state
      ** 2. limited list scan if there are service found during full scan
      */
    sdss_avoid_dup_scan_resume(ss);

    if( last_acq_sys_ptr != NULL )
    {
      /* in case of acq plmn success,  alternate redial, and emergency call orig state only add
               specific acquired band in emerg avoid dup band info. Specific band is stored in 
               last_acq_sys_ptr->sid.gw.band during acq_plmn_success
               for acq fail, lower layers would have already scanned all the bands,
               so add all the bands in avoid list*/
      if(sdss_is_event_3gpp_acq_success(event) && sdss_is_emergency_call_orig(ss) &&
         sdss_is_e911_alternate_redial_algorithm(ss))
      {
        if(last_acq_sys_ptr->sid.gw.mode == SYS_SYS_MODE_GSM ||
           last_acq_sys_ptr->sid.gw.mode == SYS_SYS_MODE_WCDMA )
        {
          band_scanned.band = last_acq_sys_ptr->sid.gw.band.chgwt_band;
          band_scanned.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE);
          band_scanned.tds_band = SD_SS_BAND_PREF_NONE;
        }
        else if(last_acq_sys_ptr->sid.gw.mode == SYS_SYS_MODE_LTE )
        {
          band_scanned.band = SD_SS_BAND_PREF_NONE;
          band_scanned.lte_band = last_acq_sys_ptr->sid.gw.band.lte_band;
          band_scanned.tds_band = SD_SS_BAND_PREF_NONE;
        }
        else if(last_acq_sys_ptr->sid.gw.mode == SYS_SYS_MODE_TDS)
        {
          band_scanned.band = SD_SS_BAND_PREF_NONE;
          band_scanned.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE);
          band_scanned.tds_band = last_acq_sys_ptr->sid.gw.band.chgwt_band;
        }        
      }
      else
      {
        band_scanned.band = last_acq_sys_ptr->sys.band;
        band_scanned.lte_band = last_acq_sys_ptr->sys.lte_band;
        band_scanned.tds_band = last_acq_sys_ptr->sys.tds_band;
      }

      /* if event is "real" plmn_acq_fail
      ** (meaning that NO_PLMN_FOUND for the acquisition),
      ** then store/save the band to band_to_avoid bit mask.
      ** The band stored/saved will be filtered out later in BSR or OOS to avoid dup scan.
      ** Save the band only if avoid_state = active.
      */
      if ( sdss_is_event_3gpp_acq_fail(event) &&
           sdss_is_3gpp_service_status_no_service(ss) == TRUE)
      {
        if (sdss_update_band_mask( ss, SDSS_BAND_TYPE_AVOID, &band_scanned))
        {
          SD_MSG_LOW_0("AVOID_DUP_SCAN: band_to_avoid mask is updated");
        }
      }

      /* If plmn is found during the last acquisition, then save the band information
      ** to be used for limited scan.
      */
      if (sdss_get_is_plmn_found(ss) == TRUE)
      {
        if (sdss_update_band_mask( ss, SDSS_BAND_TYPE_PLMN_FOUND, &band_scanned ))
        {
          SD_MSG_LOW_0("AVOID_DUP_SCAN: plmn found band mask is updated");
        }
      }

      /* Update the flag is_prev_net_sel_type_limited based on the previous acq_gwl
      ** net sel type
      */
      sdss_update_is_prev_net_sel_type_limited(ss);
    }
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to process the SS-Event.
  */
  if( ss == SD_SS_MAIN ||
      ss == SD_SS_HYBR_2 ||
      ss == SD_SS_HYBR_3)
  {
    sdss_event_proc( ss, event, iact_ptr );
  }
  else
  {
    sys_err_fatal_invalid_value_exception((int)ss);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_AVOID_DUP_BAND_SCAN
  
  /* Reset 3gpp acq info for this acquisition after processing the event */
  sdss_update_3gpp_acq_info(ss,
                            SDSS_UPDATE_3GPP_ACQ_INFO_ALL,
                            FALSE,
                            SYS_SRV_STATUS_NONE
                            );
  
  #endif

  /* Reset DDS flag after processing the event */
  sdss_reset_is_dds_switchback_from_temp(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------
  ** Do SS-Action independent processing.
  ** ------------------------------------
  */

  /* Update the SS-State indicator and inform the registered client (i.e.,
  ** CM) of any preferred service acquisition indicator changes.
  */
  sd_set_ss_state( ss, iact_ptr->ss_state );
  sd_si_pref_srv_acq( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If timer expiration time has changed, set the timer per the new
  ** expiration time.
  */
  if( iact_ptr->ss_timer != ss_ptr->ss_timer )
  {
    /* If new expiration time is 0. Disable the timer by setting it to 0.
    */
    if( iact_ptr->ss_timer == 0 )
    {
      SD_MSG_HIGH_1( "SS-Timer=0, ss=%d",ss);
      ss_ptr->timer_func_ptr( 0 );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (new expiration time != 0), set the timer according to the new
    ** expiration time.
    */
    else
    {
      dword timer;

      if( iact_ptr->ss_timer > uptime ) {
        timer = (iact_ptr->ss_timer - uptime) * 1000;
      }
      else {
        timer = SD_SS_TIMER_INVALID;
      }

      SD_MSG_HIGH_5( "SS-Timer on ss%d:=%ds or %dms, uptime=%ds, exp=%ds",
                   ss,timer/1000, timer, uptime, iact_ptr->ss_timer );
      

      ss_ptr->timer_func_ptr( (int4) timer );
    }

    ss_ptr->ss_timer = iact_ptr->ss_timer;

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ss_act_ptr = (sd_ss_act_s_type*)modem_mem_alloc(sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  
  if (ss_act_ptr != NULL)
  {
    memset(ss_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);

  /* ----------------------------------
  ** Do SS-Action dependent processing.
  ** ----------------------------------
  */
  switch( iact_ptr->act )
  {
    case SDSS_IACT_CONTINUE:

      ss_act_ptr->act = SD_SS_ACT_CONTINUE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_ACQUIRE:

      /* Do pre acquisition attempt updates.
      */
      sd_ss_acq_update_pre( ss,
                            iact_ptr->prm.acq.list,
                            iact_ptr->prm.acq.pos,
                            iact_ptr->prm.acq.mode );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Get the system to acquire and set the returned SS-Action as per
      ** the mode of this system.
      */
      sr_ptr = sdsr_list_get_ptr( iact_ptr->prm.acq.list,
                                  iact_ptr->prm.acq.pos );
      if(sr_ptr == NULL)
      {
        sys_err_fatal_null_ptr_exception();
      }

      /* If sdsr table is full and a new entry has to be added, a least recently used
      ** system is overwritten. This decision is based on the lowest event time of a record 
      ** if there are multiple entries in sdsr_tbl with sys_rec_type, SDSR_MAX_LST.
      ** To avoid overwriting the current acq system in case any sdsr_tbl entry has to be 
      ** added after issuing acquisition and before acq response is received,
      ** update the event time of the system record while acquisition is issued.
      */
      sr_ptr->event		= (word) event;
      sr_ptr->event_time	= uptime;
  

      if( ! sdss_is_supp_mode_band(sr_ptr->sys.mode,
                                   sr_ptr->sys.band,
                                   sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band),
                                   sr_ptr->sys.tds_band) )
      {
          SD_ERR_FATAL_0( "Unsupported mode/band");
      }

      switch( sr_ptr->sys.mode )
      {
        case SD_MODE_CDMA:
          ss_act_ptr->act = SD_SS_ACT_ACQ_CDMA;
          ss_act_ptr->prm.acq_cdma.band    = sdss_map_band_pref_to_band(
                                                         sr_ptr->sys.band);
          ss_act_ptr->prm.acq_cdma.chan    = sr_ptr->sys.chan;
          ss_act_ptr->prm.acq_cdma.blksys  = sd_map_chan_to_blksys(
                             sr_ptr->sys.mode,
                             sdss_map_band_pref_to_band(sr_ptr->sys.band),
                             sr_ptr->sys.chan );

          /* Indicate whether MS switched mode or blksys.
          */
          if( last_mode   != SD_MODE_CDMA ||
              last_blksys != ss_act_ptr->prm.acq_cdma.blksys )
          {
            ss_act_ptr->prm.acq_cdma.is_new_mode_or_blksys = TRUE;
          }
          else
          {
            ss_act_ptr->prm.acq_cdma.is_new_mode_or_blksys = FALSE;
          }

          /* Update the handoff indicator
          */
          ss_act_ptr->prm.acq_cdma.is_handoff_acq =
                                            iact_ptr->prm.acq.is_handoff_acq;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_AMPS:
          ss_act_ptr->act = SD_SS_ACT_ACQ_AMPS;
          ss_act_ptr->prm.acq_amps.band = sdss_map_band_pref_to_band(
                                                           sr_ptr->sys.band);
          ss_act_ptr->prm.acq_amps.sys = (sd_cell_sys_e_type) sr_ptr->sys.chan;
          SD_ASSERT_ENUM_IS_INRANGE( ss_act_ptr->prm.acq_amps.sys,
                                     SD_CELL_SYS_MAX );

          ss_act_ptr->prm.acq_amps.is_amps_2nd_cch =
                                        iact_ptr->prm.acq.is_amps_2nd_cch;

          ss_act_ptr->prm.acq_amps.max_redirect_delay =
                                        iact_ptr->prm.acq.max_redirect_delay;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_HDR:
          ss_act_ptr->act = SD_SS_ACT_ACQ_HDR;
          ss_act_ptr->prm.acq_hdr.band = sdss_map_band_pref_to_band(
                                                    sr_ptr->sys.band);
          ss_act_ptr->prm.acq_hdr.chan = sr_ptr->sys.chan;
          ss_act_ptr->prm.acq_hdr.acq_mode = sd_ss_map_acq_mode_to_hdr(
                                                    iact_ptr->prm.acq.mode );
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_GPS:
          ss_act_ptr->act = SD_SS_ACT_ACQ_GPS;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_GSM:
        case SD_MODE_WCDMA:
        case SD_MODE_GW:
        case SD_MODE_LTE:
        case SD_MODE_GWL:
        case SD_MODE_TDS:

        #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined (FEATURE_SD_LTE)
          ss_act_ptr->act = SD_SS_ACT_ACQ_GWL;
          ss_act_ptr->prm.acq_gwl.acq_order_pref  = sdss_get_acq_order_pref(ss);
          ss_act_ptr->prm.acq_gwl.gwl_mode_pref   = SD_SS_MODE_PREF_NONE ;
          ss_act_ptr->prm.acq_gwl.is_orig_pending = FALSE;
          ss_act_ptr->prm.acq_gwl.simultaneous_mode =
                                    sdss_get_simultaneous_mode();
          ss_act_ptr->prm.acq_gwl.srv_req_type    = sdss_get_srv_req_type(ss);
          ss_act_ptr->prm.acq_gwl.voice_domain_pref = sdss_get_voice_domain_pref(ss);
          /* Fills the search scanning scope in the SD Action as per the user script */
          ss_act_ptr->prm.acq_gwl.scan_scope    = sdss_get_scan_scope_type(ss);

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          /* LTE RPM might adjust the srv_domain_pref to CS_ONLY
          ** if event timer is still running and EMM generic timer has reach maximum
          */
          lte_rpm_srv_domain_pref = cmrpm_lte_get_srv_domain_pref();
          if(lte_rpm_srv_domain_pref == SD_SS_SRV_DOMAIN_PREF_CS_ONLY)
          {
            ss_act_ptr->prm.acq_gwl.srv_domain_pref = lte_rpm_srv_domain_pref;
          }
          else
          {
            ss_act_ptr->prm.acq_gwl.srv_domain_pref = sdss_get_domain_pref(ss);
          }

          if (sdss_get_pref_update_reason(ss) == SDSS_PREF_UPDATED_UE_CAPABILITY_CHG)
          {
            ss_act_ptr->prm.acq_gwl.additional_info = BM(SYS_ADDTNL_INFO_UE_CAP_CHANGE);
            SD_MSG_HIGH_1( "Setting additional_flag %d", ss_act_ptr->prm.acq_gwl.additional_info);
            sdss_set_pref_update_reason(ss, SDSS_PREF_UPDATED_NONE);
          }
          else
          {
            ss_act_ptr->prm.acq_gwl.additional_info= 0;
          }
          
          /*Set force kick flag to FALSE for following cases.
                  1. 1x/HDR is acquired and stack syncup bsr preferences resulted in LTE acqusiition.
                  2. User changed the preferences while there is no 1X and HDR service which resulted in LTE acqusiition.
                  */
          if((ss == SD_SS_HYBR_2) &&
              sdss_is_flag_set(SD_SS_HYBR_2, SDSS_FLAG_IS_FORCE_KICK_HYBR2))
          {
            sdss_set_flag(SD_SS_HYBR_2, SDSS_FLAG_IS_FORCE_KICK_HYBR2, FALSE);
          }
          
          /* if is_ral_stored_in_efs is set and orig mode is emergency and override E911 info flag is not set
                       then send addiotional info as emerg_pending 
                       override E911 flag would be used for scenarios where Good LTE cell is not found and NAS would have send 
                       scan status as PLMN found. CM needs to send subsequent service request without additional info flag as SET*/
          if(sdss_is_ral_stored_in_efs(ss) && 
             (ss_orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG || ss_orig_mode == SD_SS_ORIG_MODE_EMERG_CB) &&
             (!sdss_is_flag_set(ss,SDSS_FLAG_OVERRIDE_E911_ADD_INFO))
             )
          {
            ss_act_ptr->prm.acq_gwl.additional_info |= BM(SYS_ADDTNL_INFO_EMERG_CALL);
          }

          if (sdss_is_1xcsfb_call_end(ss, event) &&
              sdss_get_1xcsfb_call_end_opt_status())
          {
            ss_act_ptr->prm.acq_gwl.additional_info |= BM(SYS_ADDTNL_INFO_1XCSFB_CALL_END);

            if (sdss_get_pref_update_reason(ss) == SDSS_PREF_UPDATED_1XCSFB_ORIG_END)
            {
              sdss_set_pref_update_reason(ss, SDSS_PREF_UPDATED_NONE);
            }
          }
          else
          {
            ss_act_ptr->prm.acq_gwl.additional_info &= (~BM(SYS_ADDTNL_INFO_1XCSFB_CALL_END)); 
          }
          
          /* 1.Detect the case of LTE connected mode and act upon it
          ** If UE is in an emergency call , do not act upon LTE connected
          ** mode and set srv req type to SUITABLE SEARCH
          ** exception is the case where RLF has to be performed during
          ** emergency call or callback mode. This is recognized via is_RLF_during_emerg flag.
          **
          ** 2. is_full_srv_req check is needed only to cover VOLTE to GW E911 trasition.
          ** If this check is not present, then CM will issue FULL GW servcie reqeust in
          ** LTE connected mode during VOLTE to GW transition.  
          */
          if( sdss_is_lte_connected_mode (ss) &&
              !( ( (ss_orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG ||
                    ss_orig_mode == SD_SS_ORIG_MODE_EMERG_CB) &&
                    ((sdss_get_is_RLF_during_emerg(ss) == FALSE ) &&
                     (sdss_get_is_full_srv_req_during_emerg(ss) == FALSE))
                  ) ||
                  ss_orig_mode == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG
               )
             )
          {
            /* Dudring LTE RLF, CM will have srv_status as SRV and 
            ** csfb_type will be caluated as PPCSFB. But SD will treat this
            ** reqeust as regualr emergecy call and issues FULL GW SERVCIE REQ
            */
            if (ss_orig_mode == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG)
            {
              if (sdss_get_is_full_srv_req_during_emerg(ss))
              {
                SD_MSG_HIGH_0( "Marking LTE suitable search due to PPCSFB emergency orig" );
                ss_act_ptr->prm.acq_gwl.srv_req_type =
                                      SD_SS_SRV_REQ_TYPE_LTE_SUITABLE_SEARCH;
              }
            }
            else
            {
              ss_act_ptr->prm.acq_gwl.srv_req_type =
                                      SD_SS_SRV_REQ_TYPE_LTE_SUITABLE_SEARCH;
            }
          }
          #if defined (FEATURE_WRLF_SYSTEM_SEL)
          if( sdss_is_wcdma_suitable_search (ss) &&
              ! ( ss_orig_mode == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG ||
                 ss_orig_mode == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG )
            )
          {
            if ( ss_orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG )
            {
              if (sdss_get_is_full_srv_req_during_emerg(ss))
              {
                SD_MSG_HIGH_0( "Marking W suitable search due to PPCSFB emergency orig" );
                ss_act_ptr->prm.acq_gwl.srv_req_type =
                                      SD_SS_SRV_REQ_TYPE_WCDMA_SUITABLE_SEARCH;
          }
            }
            else
            {
              ss_act_ptr->prm.acq_gwl.srv_req_type =
                                      SD_SS_SRV_REQ_TYPE_WCDMA_SUITABLE_SEARCH;
            }
          }
          #endif /* FEATURE_WRLF_SYSTEM_SEL */


          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Check if origination is pending.
          */
          if ( ss_pref_reason == SD_SS_PREF_REAS_ORIG_START_CS ||
               ss_pref_reason == SD_SS_PREF_REAS_ORIG_START_PS ||
               ss_pref_reason == SD_SS_PREF_REAS_REDIAL_NORM   ||
               ss_pref_reason == SD_SS_PREF_REAS_REDIAL_OPTI
             )
          {
            /* Origination is pending.
            */
            ss_act_ptr->prm.acq_gwl.is_orig_pending = TRUE;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Set manual selected plmn type and user reselect flag for
          ** origination mod.
          */
          ss_act_ptr->prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_MAX;
          ss_act_ptr->prm.acq_gwl.is_user_resel      = FALSE;
          ss_act_ptr->prm.acq_gwl.gw_band_pref       = SD_SS_BAND_PREF_NONE;
          ss_act_ptr->prm.acq_gwl.lte_band_pref      = SYS_LTE_BAND_MASK_CONST_NONE;
          ss_act_ptr->prm.acq_gwl.tds_band_pref      = SD_SS_BAND_PREF_NONE;

          sys_plmn_undefine_plmn_id(&ss_act_ptr->prm.acq_gwl.plmn);
          ss_act_ptr->prm.acq_gwl.csg_id  = SYS_CSG_ID_INVALID;
          ss_act_ptr->prm.acq_gwl.csg_rat = SYS_RAT_NONE;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Set the band preference according to the system's band.
          */

          #ifdef FEATURE_AVOID_DUP_BAND_SCAN
          {
            tmp_band.band = sr_ptr->sys.band;
            tmp_band.lte_band = sr_ptr->sys.lte_band;
            tmp_band.tds_band = sr_ptr->sys.tds_band;
  
          /* if alternate redial algo is TRUE, and RLF happened during emergency then do not filter out bands 
                      on the basis of avoid Dup Band */
          if(!(sdss_is_e911_alternate_redial_algorithm(ss) && 
               sdss_get_is_RLF_during_emerg(ss) == TRUE)
             )
          {
            /* Filters out 3gpp bands which have been scanned in this scan cycle.
                 ** bands are filtereded only if avoid_state = active.
            */
            sdss_filter_out_dup_band(ss, sr_ptr->sys.mode, &tmp_band);
  
            /* This should not happen */
              if(sdss_is_band_none_per_mode(sr_ptr->sys.mode, &tmp_band))
            {
                SD_MSG_HIGH_0("AVOID_DUP_SCAN: Error in ACQ_GWL, band none !!!");
  
                /* If this occurs, use the original record without filtering
                **  the band, SD would not be stuck.
              */
              ss_act_ptr->prm.acq_gwl.gw_band_pref  = sr_ptr->sys.band;
              ss_act_ptr->prm.acq_gwl.lte_band_pref = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band);
              ss_act_ptr->prm.acq_gwl.tds_band_pref = sr_ptr->sys.tds_band;
            }
            else
            {
              ss_act_ptr->prm.acq_gwl.gw_band_pref  = tmp_band.band;
              ss_act_ptr->prm.acq_gwl.lte_band_pref = sdsr_map_sys_rec_lte_band_to_lte_band_pref(tmp_band.lte_band);
              ss_act_ptr->prm.acq_gwl.tds_band_pref = tmp_band.tds_band;
            }
          }
          else
          {
            ss_act_ptr->prm.acq_gwl.gw_band_pref  = sr_ptr->sys.band;
            ss_act_ptr->prm.acq_gwl.lte_band_pref = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band);
            ss_act_ptr->prm.acq_gwl.tds_band_pref = sr_ptr->sys.tds_band;
          }
         }
         #else
          {
            ss_act_ptr->prm.acq_gwl.gw_band_pref  = sr_ptr->sys.band;
            ss_act_ptr->prm.acq_gwl.lte_band_pref = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band);
            ss_act_ptr->prm.acq_gwl.tds_band_pref = sr_ptr->sys.tds_band;
          }
         #endif

           #ifdef FEATURE_SD_LTE
		  /*  check if VOLTE call is connected and do exclusion of non VOLTE supported
		     LTE bands. */
		  if(sdss_get_volte_call_state(ss))    
          {
		     sdss_update_volte_supported_lte_bands(&ss_act_ptr->prm.acq_gwl.lte_band_pref,sdss_map_ss_to_as_id(ss) );
		  }
		
          #endif
		  
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Set the band preference according to the system's mode.
          */
          ss_act_ptr->prm.acq_gwl.gwl_mode_pref =
                               sdss_map_mode_to_mode_pref (sr_ptr->sys.mode);


          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Check if GW subscription is available */
          sdss_get_mode_band_pref( ss,
                                   &ss_mode_pref,
                                   &ss_band_pref,
                                   &ss_band_pref_lte,
                                   &ss_band_pref_tds );

          /* If GW subscription is not available or acq in gwl limited list,
          ** get band and mode based on true mode preference of phone
          */
          if (SD_BIT_COMMON(sdss_get_subsc_type(ss), SDSS_SUBSC_GW) == 0)
          {
            ss_mode_pref = sdss_get_true_mode_pref(ss);
            SD_MSG_HIGH_1("gw sub not avail, setting mode pref to true mode pref. ss_mode_pref %d",ss_mode_pref);
          }
          else if(sdsr_list_find( SDSR_GWL_LTD_LST, sr_ptr ) != SDSR_POS_NULL)
          {
            if(ss_mode_pref != sdss_get_true_mode_pref(ss))
            {
              ss_mode_pref = sdss_get_true_mode_pref(ss);
            }
            SD_MSG_HIGH_1("gwl limited list, setting mode pref to true mode pref. ss_mode_pref %d",ss_mode_pref);
          }

          sd_get_band_mode_mmode( &ss_act_ptr->prm.acq_gwl.gwl_mode_pref,
                                  &ss_act_ptr->prm.acq_gwl.gw_band_pref,
                                  &ss_act_ptr->prm.acq_gwl.lte_band_pref,
                                  &ss_act_ptr->prm.acq_gwl.tds_band_pref,
                                  ss_mode_pref,
                                  ss_band_pref,
                                  ss_band_pref_lte,
                                  ss_band_pref_tds
                                 );
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || \
              defined(FEATURE_SD_LTE)
          /* Set sys network selection type, manual select mode type and
          ** PLMN ID.
          */
          sd_set_acq_gw_sys( ss,
                             &sr_ptr->sid_expected,
                             event,
                             ss_pref_reason,
                             &ss_act_ptr->prm.acq_gwl );
          #endif

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          acq_stt = sdss_eng_stack_get_state(ss); 
          /* If origination mode is emergency origination or emergency callback mode
          ** force the network selection type to LIMITED
          ** Exception is the case where -
          ** 1) LTE RLF occured on a system which supported emergency and the
          **    last service request sent by CM was for full service
          ** 2) UE is attempting normal attach on an LTE system during emergency
          **    call ( SDSS_ACQ_STT_REACQ_FULL_SRV)
          */
          if( ss_orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG ||
              ss_orig_mode == SD_SS_ORIG_MODE_EMERG_CB )
          {
            if(sdss_is_ral_stored_in_efs(ss))
            {
              sdss_check_reset_emerg_rat_pri_list_info_no_emerg_orig(ss);
            }
            /* case 1: normal attach during 911 call*/
            if( acq_stt      == SDSS_ACQ_STT_REACQ_FULL_SRV )
            {
              /* If UE is trying to reacquire LTE full service during 911
              ** make sure that SD is using idle cap. Network sel mode has 
              ** already been set to either AUTO/MANUAL. do not overwrite to 
              ** LIMITED */ 
              sdss_update_rat_pri_list_with_idle_cap(ss);
              SD_ASSERT(ss_act_ptr->prm.acq_gwl.net_sel_type !=
                                          SYS_NETWORK_SELECTION_MODE_LIMITED);
              SD_MSG_HIGH_1("LTE_911:reacq net_sel %d ",ss_act_ptr->prm.acq_gwl.net_sel_type);

            }
            /* case 2: RLF during 911 */
            else if (sdss_get_is_RLF_during_emerg(ss) == TRUE ) 
            {
              /* If last srv status was not full service, do limited RLF */
              if (sd_si_is_full_srv_on(ss)) 
              {
                sdss_update_rat_pri_list_with_idle_cap(ss);
                SD_ASSERT(ss_act_ptr->prm.acq_gwl.net_sel_type !=
                                          SYS_NETWORK_SELECTION_MODE_LIMITED);
              }
              else
              {
                ss_act_ptr->prm.acq_gwl.net_sel_type =
                                          SYS_NETWORK_SELECTION_MODE_LIMITED;
                sdss_check_reset_emerg_rat_pri_list_info(ss);
              }
              SD_MSG_HIGH_1("LTE_911:RLF net_sel %d ",ss_act_ptr->prm.acq_gwl.net_sel_type);
            }
            /* all other cases */
            /* Do not issue Pure limited reqeust if FULL SRV REQ is needed */
            /* Avoid updating net_sel_mode if emergency orig is in volte only mode
                            and last known MCC belong to that whitelist */
            else if (!(sdss_get_is_full_srv_req_during_emerg(ss)
                        #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined( FEATURE_SD_LTE )
                        ||(sdss_is_emer_orig_in_volte_only(ss) &&
                           (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE))
                        #endif
                       )
                     )
            {
              ss_act_ptr->prm.acq_gwl.net_sel_type =
                                          SYS_NETWORK_SELECTION_MODE_LIMITED;
            }
			/* if there is limited routine acquisition in emergnecy orig in volte only mode, change it to automatic */
			else if(sdss_is_emer_orig_in_volte_only(ss) &&
                    (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE) &&
                    (ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION))
			{
			  SD_MSG_HIGH_0("Volte Only Emergency call net_sel changed from lmtd routine to automatic");
			  ss_act_ptr->prm.acq_gwl.net_sel_type = SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
			}
          }

          /* If origination mode is PPCSFB emerg orig and full srv req is not needed
          ** or SIM domain is not valid, force network sel type to limited.
          */
          if( ((ss_orig_mode == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG) &&
               (!sdss_get_is_full_srv_req_during_emerg(ss)))||
              !sdss_gw_sim_domain_valid (ss)
            )
          {
            ss_act_ptr->prm.acq_gwl.net_sel_type =
                                        SYS_NETWORK_SELECTION_MODE_LIMITED;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* If internal action dicates that the first plmn found should be
          ** returned , force the network selection type to LIMITED. This is
          ** done so that NAS returns with the first PLMN it finds without
          ** registering on it.
          */
          if( iact_ptr->prm.acq.gwl_acq_type.acq_type == SDSS_GWL_ACQ_FIRST_PLMN )
          {
            ss_act_ptr->prm.acq_gwl.additional_info |= BM(SYS_ADDTNL_INFO_GEO_DETERMINATION_PHASE);
            SD_MSG_HIGH_1( "Setting additional_flag for geo determination %d", ss_act_ptr->prm.acq_gwl.additional_info);
            ss_act_ptr->prm.acq_gwl.net_sel_type =
                                          SYS_NETWORK_SELECTION_MODE_LIMITED;
          }

          if ( sdss_get_sim_state(ss) == SYS_SIM_STATE_NOT_AVAILABLE )
          {
            ss_act_ptr->prm.acq_gwl.net_sel_type =
                                          SYS_NETWORK_SELECTION_MODE_LIMITED;
          }
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



          /* If origination mode is MANUAL, and the script is requesting
          ** Limited service, set net_sel_type to LIMITED_MANUAL_ACQUISITION
          */

          if( (sdss_is_orig_mode( ss, SD_SS_ORIG_MODE_VAL_MANUAL ))
               &&
              (sr_ptr->prl == (byte) SDSS_PRL_NEG     ||
               ss_act_ptr->prm.acq_gwl.net_sel_type   ==
                  SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION )
            )
          {
            int                   pos                   = 0;
            sdsr_s_type           *user_sys_ptr         = NULL;


            ss_act_ptr->prm.acq_gwl.net_sel_type       =
              SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION;

            /* Get PLMN id from USR_LIST. All entries in USR_LIST
            ** will have the manually selected PLMN id
            */
            pos          = 0;
            user_sys_ptr = sdsr_list_get_ptr( sdss_sr_list_map2( ss, SDSR_MANUAL_LST), pos );

            SD_ASSERT( user_sys_ptr != NULL );

            /*lint -e774 */
            if (user_sys_ptr != NULL)
            {

              SD_ASSERT (user_sys_ptr->sid_expected.net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS);
              SD_MSG_HIGH_0("Non-CSG: Using user_sys_ptr RPLMN");
              ss_act_ptr->prm.acq_gwl.plmn             =
                                user_sys_ptr->sid_expected.prm.gwl.plmn.plmn_id;
            }
            else
            {
              SD_ERR_0(" sys ptr to USER_LST is null");
            }

            ss_act_ptr->prm.acq_gwl.selected_plmn_type =
                                    SD_MANUALLY_SELECTED_SPECIFIED_PLMN;
          }
          /*lint +e774 */

          #ifdef FEATURE_SD_UPDATE_MRU_IN_MANUAL_MODE
          /* If origination mode is set to AUTO , and the script is requesting
          ** Manual service, set net_sel_type = AUTOMATIC, to avoid the UMTS/GSM
          ** stack to stuck on previously acquired full service on Manual PLMNs.
          */
          if( sdss_is_orig_mode( ss, SD_SS_ORIG_MODE_VAL_NONE ) &&
              (sr_ptr->sid_expected.net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS) &&
              ((ss_act_ptr->prm.acq_gwl.net_sel_type ==
                   SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION) ||
               (ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_MANUAL))
            )
          {
            ss_act_ptr->prm.acq_gwl.net_sel_type =
                                          SYS_NETWORK_SELECTION_MODE_AUTOMATIC;
            ss_act_ptr->prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_MAX;
          }
          #endif /* FEATURE_SD_UPDATE_MRU_IN_MANUAL_MODE  */

          #ifdef FEATURE_EOOS_UMTS
          if( ( ss_act_ptr->prm.acq_gwl.net_sel_type  ==
                                       SYS_NETWORK_SELECTION_MODE_AUTOMATIC )
                ||
               ( ss_act_ptr->prm.acq_gwl.net_sel_type ==
                                          SYS_NETWORK_SELECTION_MODE_MANUAL )
            )
          {
            ss_act_ptr->prm.acq_gwl.net_sel_type =
               sdss_compute_net_sel_mode( ss_act_ptr->prm.acq_gwl.net_sel_type, ss );
          }
          #endif /* FEATURE_EOOS */

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* update the RAT priority list sent to NAS based on the current
          ** system being acquired.
          */
          #ifdef FEATURE_AVOID_DUP_BAND_SCAN

          {
            /* Update that gw scan has been attempted
            ** If is_gw_scan_attempted = false,
            ** sd will not skip limited scan list/sys
            */
            if(ss_act_ptr->prm.acq_gwl.net_sel_type ==
                                    SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
               ss_act_ptr->prm.acq_gwl.net_sel_type == 
                                    SYS_NETWORK_SELECTION_MODE_MANUAL ||
               ss_act_ptr->prm.acq_gwl.net_sel_type ==
                                    SYS_NETWORK_SELECTION_MODE_LIMITED)
            {
              sdss_update_is_3gpp_scan_attempted(ss, TRUE);
            }

            /* Update rat_pri_list using the filtered band */
            tmp_sr = *sr_ptr;
            tmp_sr.sys.band = ss_act_ptr->prm.acq_gwl.gw_band_pref;
            tmp_sr.sys.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(ss_act_ptr->prm.acq_gwl.lte_band_pref);
            tmp_sr.sys.tds_band = ss_act_ptr->prm.acq_gwl.tds_band_pref;

            sd_update_rat_pri_list(&(ss_act_ptr->prm.acq_gwl.rat_pri_list_info),
                                    &tmp_sr, ss,event);
          }

          #else
          {
            SD_MSG_MED_0( "Call is with ss_ptr to update rat pri list");
            sd_update_rat_pri_list(&(ss_act_ptr->prm.acq_gwl.rat_pri_list_info),
                                    sr_ptr, ss,event);
          }

          #endif


          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          #if defined(FEATURE_EOOS_UMTS)
          sdss_update_eoos_scan_gwl_param(&ss_act_ptr->prm.acq_gwl);
          sd_display_eoos_params (&ss_act_ptr->prm.acq_gwl);
          #endif

          ss_act_ptr->prm.acq_gwl.acq_scan_type = SD_SS_ACQ_SCAN_TYPE_NORMAL;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Clear the Previous Colloc List when acquiring GWL on MAIN
          ** To make sure that realign_hdr is set if same CDMA is acq'd
          ** back on SS-MAIN. Thus kicking HDR which died because of GWL ACQ
          ** If GWL is acq'd on HYBR2, the acq_fail on hybr2 will kick HDR.
          */
          if( ss == SD_SS_MAIN )
          {
            sdsr_list_clr( SDSR_PREV_COLLOC_LST );
          }

          #ifdef FEATURE_MMSS_3_1
          if( sdss_is_mmss_operation(ss) &&
              !sdss_is_emergency_call_orig(ss)
            )
          {
            if(ss == SD_SS_MAIN)
            {
             /* Set srv_req_type to MMSS type if it is :
             ** srv_req_type is normal or system change
             ** net_sel_type is automatic
             ** current acq state is not REACQ and REACQ_GWL
             ** and non_mmss_type flag is not set.
             ** and UE is not in limited to full reselection
             **
             ** srv_type BSR should keep as is
             **
             */
             if(ss_act_ptr->prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_MMSS)
             {
               ss_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_NORMAL;
             }
             acq_stt = sdss_eng_stack_get_state(SD_SS_MAIN);
             if((ss_act_ptr->prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_NORMAL ||
                ss_act_ptr->prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_SYSTEM_CHANGE) &&
                 ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
                 acq_stt                         != SDSS_ACQ_STT_REACQ &&
                 acq_stt                         != SDSS_ACQ_STT_REACQ_GWL &&
                 acq_stt                         != SDSS_ACQ_STT_REACQ_FULL_SRV &&
                 !sdss_is_flag_set(ss, SDSS_FLAG_SRV_TYPE_NON_MMSS)&&
                 !sdss_is_cap_3gpp_only(ss) &&
                 !sdss_is_skip_mmss_srv_req(ss, ss_act_ptr)&&
                 !sdss_eng_stack_is_acq_type(ss, SDSSSCR_ACQ_ALLOWED)
                 )
             {
                /*Check MRU[0], If it is 3gpp mode change srv_type to MMSS_RPLMN. NAS will
                            ** search from RPLMN if EF_RPLMNSI=00 */
               if(sdss_is_mru_3gpp(ss))
               {
                 ss_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_MMSS_RPLMN;
                 
               }
               else
               {
                 ss_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_MMSS;
               }
               sdss_set_srv_req_type(ss,ss_act_ptr->prm.acq_gwl.srv_req_type);
             }
            }
            else if(ss == SD_SS_HYBR_2 
              #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
              || ss == SD_SS_HYBR_3 
              #endif
              )
            {
              if((ss_act_ptr->prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_SYSTEM_CHANGE ||
                  ss_act_ptr->prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_NORMAL) &&
                  ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
              {
                  sd_si_info_s_type *si_info_ptr = &sd_si_ptr(SD_SS_MAIN)->si_info;
                  sd_si_info_s_type *si_info_hybr_ptr = &sd_si_ptr(SD_SS_HYBR_1)->si_info;

                  /*If 1x or HDR is in service only send MMSS otherwise send MMSS_RPLMN */
                  if((si_info_ptr->mode == SYS_SYS_MODE_CDMA && si_info_ptr->srv_status == SYS_SRV_STATUS_SRV) ||
                     si_info_hybr_ptr->srv_status == SYS_SRV_STATUS_SRV)
                  {
                    ss_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_MMSS;
                  }
                  else
                  {
                    ss_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_MMSS_RPLMN;
                  }
                  sdss_set_srv_req_type(ss,ss_act_ptr->prm.acq_gwl.srv_req_type);
               }
              sdss_set_flag(ss, SDSS_FLAG_IS_IBSR_PWRUP_DONE,TRUE );
            }        
          }
          #endif

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
          /* If acq state is set to REACQ_FULL_SRV update the service req type
          ** to SRV_MODE_UPDATE
          */
          if(  acq_stt == SDSS_ACQ_STT_REACQ_FULL_SRV  )
          {
            SD_MSG_HIGH_0("LTE_911: SRV MODE UPDATE");
            ss_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_SRV_MODE_UPDATE;
          }
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

          /* If UE just transitioned from CAMP_ONLY to unrestricted mode 
          ** and it's not in a normal call origination, 
          ** set SGLTE specific service request type for NAS benefit.
          */
          if( sdss_get_camp_mode() == SD_SS_PREF_CAMP_MODE_OFF  &&
              *sdss_misc_sglte_move_to_unrestricted() == TRUE
            )
          {
            if(!sdss_is_sglte()&&
              (ss_act_ptr->prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_NORMAL        ||
               ss_act_ptr->prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_SYSTEM_CHANGE ||
               ss_act_ptr->prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_USER_SELECTION ) &&
               sdss_get_orig_mode(ss) != SD_SS_ORIG_MODE_NORMAL)
            {
              ss_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_CAMPED_PLMN;
            }
            *sdss_misc_sglte_move_to_unrestricted() = FALSE;
          }

          SD_MSG_HIGH_1("srv_req_type-%d",ss_act_ptr->prm.acq_gwl.srv_req_type);

          if (ss != SD_SS_HYBR_3)
          {
            sd_misc_map_srv_req_in_camp_mode( ss_act_ptr );
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
          /* Reset the srv req type
          */
          if( sdss_get_srv_req_type(ss) == SD_SS_SRV_REQ_TYPE_LAST_CAMPED_RAT)
          {
             sdss_set_srv_req_type(ss,SD_SS_SRV_REQ_TYPE_NORMAL);
          }

		  
          /* Send full service request event during emergency */
          if(sdss_get_is_full_srv_req_during_emerg(ss))
          {
            SD_MSG_HIGH_1("is_full_srv:%d",sdss_get_is_full_srv_req_during_emerg(ss));
            srv_req_event.as_id = sdss_map_ss_to_as_id(ss);
            event_report_payload(EVENT_FULL_SRV_EMC_CALL,
			   	 sizeof(sdlog_full_srv_req_emerg_s_type),
				 &srv_req_event );
          }

		   /* If rat priority list contains LTE, that means LTE scan would be attempted,
              so is_lte_scanned flag can be set to true */
          
           for(idx=0; idx< ss_act_ptr->prm.acq_gwl.rat_pri_list_info.num_items ;idx++)
           {
             if(ss_act_ptr->prm.acq_gwl.rat_pri_list_info.priority_list_info[idx].acq_sys_mode == SYS_SYS_MODE_LTE
                 && ss_act_ptr->prm.acq_gwl.rat_pri_list_info.priority_list_info[idx].bst_rat_acq_required == TRUE)
             {
               si_info_ptr->is_lte_scanned = TRUE;
               break;  
             }
          
           }

          #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED */
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_NONE:
        case SD_MODE_INACT:
        case SD_MODE_MAX:
        default:
          SD_ERR_0( "mode");
          break;
      }
      sdss_set_flag(ss, SDSS_FLAG_SRV_TYPE_NON_MMSS, FALSE);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_MEAS:

      /* First do the following:
      **
      ** 1. Increment the measurement ID.
      ** 2. Indicate that measurement request is not done being serviced.
      ** 3. Indicate whether the requested list of systems to measure is
      **    different than the previous requested list.
      ** 4. Copy the systems that are being measured into the SDSR_MEAS_LIST.
      */
      ss_ptr->meas_id++;
      ss_ptr->meas_is_done = FALSE;

      if( sdsr_list_is_contain(sdss_sr_list_map2(ss,SDSR_MEAS_LST),
                               SDSR_CONTAIN_SAME,
                               iact_ptr->prm.meas.list) )
      {
        ss_ptr->meas_list_is_diff = FALSE;
      }
      else
      {
        ss_ptr->meas_list_is_diff = TRUE;
      }
      sdsr_list_cpy( sdss_sr_list_map2(ss,SDSR_MEAS_LST), iact_ptr->prm.meas.list );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Set the returned SS-Action as per the measurement mode.
      */
      if( iact_ptr->prm.meas.mode == SDSS_MEAS_MODE_DED )
      {
        ss_act_ptr->act = SD_SS_ACT_MEAS_DED;
      }
      else
      {
        ss_act_ptr->act = SD_SS_ACT_MEAS_BACK;

      }
      ss_act_ptr->prm.meas.type  = iact_ptr->prm.meas.type;
      ss_act_ptr->prm.meas.id    = ss_ptr->meas_id;

	  /* Fills the search scanning scope in the SD Action as per the user script */
      ss_act_ptr->prm.meas.scan_scope    = sdss_get_scan_scope_type(ss);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_PWR_SAVE:

      ss_act_ptr->act = SD_SS_ACT_PWR_SAVE;
      ss_act_ptr->prm.pwr_save.reas = sdss_get_pwr_save_reason(ss);

      /* Reset avoid dup band scan state and band mask */
      #ifdef FEATURE_AVOID_DUP_BAND_SCAN
      
      sdss_proc_avoid_dup_scan_per_pwr_save(ss);
      
      #endif

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_ACCEPT:
      ss_act_ptr->act = SD_SS_ACT_ACCEPT;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_REJECT:
      ss_act_ptr->act = SD_SS_ACT_REJECT;
     break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_RELEASE:
      ss_act_ptr->act = SD_SS_ACT_RELEASE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_GET_NET_GWL:
    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined (FEATURE_SD_LTE)
    {
      /* Session info is stored in MAIN stack by mmocmmgsdi hence read from the same */
      mmgsdi_app_enum_type            ss_gwl_app_type = sdss_get_session_info(ss);
      ss_act_ptr->act = SD_SS_ACT_GET_NET_GW;
      ss_act_ptr->prm.get_net_gw.gw_mode_pref      = iact_ptr->prm.get_net.mode_pref;
      ss_act_ptr->prm.get_net_gw.gw_band_pref      = iact_ptr->prm.get_net.band_pref;
      ss_act_ptr->prm.get_net_gw.lte_band_pref     = sdsr_map_sys_rec_lte_band_to_lte_band_pref(iact_ptr->prm.get_net.lte_band_pref);
      ss_act_ptr->prm.get_net_gw.tds_band_pref     = iact_ptr->prm.get_net.tds_band_pref;
      ss_act_ptr->prm.get_net_gw.srv_domain_pref   = sdss_get_domain_pref(ss);
      ss_act_ptr->prm.get_net_gw.sdsr_ref          =(byte)iact_ptr->prm.get_net.sdsr_ref;
      ss_act_ptr->prm.get_net_gw.acq_order_pref    = iact_ptr->prm.get_net.acq_order_pref;
     
      if  ( (ss == SD_SS_MAIN) && (sdss_get_operator_name ( SYS_MODEM_AS_ID_1 ) == SD_OPERATOR_CT ) && 
                            !sdss_is_mmss_operation(SD_SS_MAIN) )
      {
        /*If inserted Non MMSS CT card, disable LTE bands */
        SD_MSG_HIGH_0("Non MMSS CT card inserted disabled LTE ");
        ss_act_ptr->prm.get_net_gw.lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE;
      }
      
      sdss_get_network_pref_read( ss,
                                  &ss_mode_pref,
                                  &ss_band_pref,
                                  &ss_band_pref_lte,
                                  &ss_band_pref_tds );

      /* Filter band modes based on target capability, band and mode
      ** preferences of get network request.*/
      sd_get_band_mode_mmode(&ss_act_ptr->prm.get_net_gw.gw_mode_pref,
                             &ss_act_ptr->prm.get_net_gw.gw_band_pref,
                             &ss_act_ptr->prm.get_net_gw.lte_band_pref,
                             &ss_act_ptr->prm.get_net_gw.tds_band_pref,
                             ss_mode_pref,
                             ss_band_pref,
                             ss_band_pref_lte,
                             ss_band_pref_tds);

      /* Filter band modes based on Manual Search BST and mode
      ** preferences of get network request.*/
       ss_act_ptr->prm.get_net_gw.mcc_mode_band_filter.num_items = 0;
       {
         sd_si_info_s_type *si_info_ptr = &sd_si_ptr(ss)->si_info;
         boolean has_mcc_match = FALSE;
         sd_ss_act_get_net_gw_s_type get_net_gw;
         get_net_gw.gw_mode_pref = SD_SS_MODE_PREF_NONE;

         if(sys_srv_status_is_srv(si_info_ptr->srv_status) && 
            !sys_plmn_id_is_undefined(si_info_ptr->sid.plmn.plmn_id))
         {
           sys_mcc_type mcc = 0; 
           sd_bst_s_type* bst_rec_ptr = NULL;
           
           sys_plmn_get_mcc(si_info_ptr->sid.plmn.plmn_id,&mcc);

           SD_MSG_HIGH_4("PLMN : %d %d %d.  Extracted MCC : %d ",si_info_ptr->sid.plmn.plmn_id.identity[0],
                                         si_info_ptr->sid.plmn.plmn_id.identity[1],
                                         si_info_ptr->sid.plmn.plmn_id.identity[2], mcc);
          
           
           has_mcc_match = sdss_bst_has_match(mcc,
                                          &ss_mode_pref,
                                          &ss_band_pref,
                                          &ss_band_pref_lte,
                                          &ss_band_pref_tds,
                                          &bst_rec_ptr);
           
           
         }//if(!sys_plmn_id_is_undefined(ss_ptr->rplmn))
         if(has_mcc_match == TRUE || 
            (has_mcc_match == FALSE && sdss_bst_get_default_prefs(&ss_mode_pref,
                                                                 &ss_band_pref,
                                                                 &ss_band_pref_lte,
                                                                 &ss_band_pref_tds))
             )
         {
           get_net_gw.gw_mode_pref = ss_mode_pref;
           get_net_gw.gw_band_pref = ss_band_pref;
           get_net_gw.lte_band_pref = ss_band_pref_lte;
           get_net_gw.tds_band_pref = ss_band_pref_tds;
		   
           SD_MSG_HIGH_1("GET_NET_GW : Fill MCC MODE BAND FILTER match %d",has_mcc_match);           
           
         } //if(has_mcc_match == TRUE)
       
           
	     if(get_net_gw.gw_mode_pref != SD_SS_MODE_PREF_NONE)
         {
           sdss_get_network_pref_read( ss,
                                   &ss_mode_pref,
                                   &ss_band_pref,
                                   &ss_band_pref_lte,
                                   &ss_band_pref_tds );
		   
	      /* Filter band modes based on target capability, band and mode
                 ** preferences of get network request.*/

          sd_get_band_mode_mmode(&get_net_gw.gw_mode_pref,
                                  &get_net_gw.gw_band_pref,
                                  &get_net_gw.lte_band_pref,
                                  &get_net_gw.tds_band_pref,
                                  ss_mode_pref,
                                  ss_band_pref,
                                  ss_band_pref_lte,
                                  ss_band_pref_tds);

          sdss_get_mode_band_pref( ss,
                               &ss_mode_pref,
                               NULL,
                               NULL,
                               NULL );

           /* Use the Manual list to order the RATs only if UE is in MANUAL
                 ** operation and 3GPP only operation */
           if ( sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_MANUAL) &&
               sdss_is_mmss_operation(ss)&&
              !sd_misc_is_mode_pref(ss_mode_pref, SD_SS_MODE_PREF_CDMA_HDR))
           {
             sd_update_get_net_rat_pri_list_manual_lst_order(
                                &(ss_act_ptr->prm.get_net_gw.mcc_mode_band_filter),
                                &get_net_gw);
           }
           else
           {
             sd_update_get_net_rat_pri_list_tot_order(
                                 ss,
                                &(ss_act_ptr->prm.get_net_gw.mcc_mode_band_filter),
                                &get_net_gw);
           }
	   
         }
         
       }

          sdss_get_mode_band_pref( ss,
                               &ss_mode_pref,
                               NULL,
                               NULL,
                               NULL );

      /* Use the Manual list to order the RATs only if UE is in MANUAL
      ** operation and 3GPP only operation */
      if ( sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_MANUAL) &&
           sdss_is_mmss_operation(ss)&&
           !sd_misc_is_mode_pref(ss_mode_pref, SD_SS_MODE_PREF_CDMA_HDR))
      {
        sd_update_get_net_rat_pri_list_manual_lst_order(
                                &(ss_act_ptr->prm.get_net_gw.rat_pri_list_info),
                                &(ss_act_ptr->prm.get_net_gw));
      }
      else
      {
        sd_update_get_net_rat_pri_list_tot_order(
                                 ss,
                                &(ss_act_ptr->prm.get_net_gw.rat_pri_list_info),
                                &(ss_act_ptr->prm.get_net_gw));
      }
    }
    #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED || defined (FEATURE_SD_LTE)*/
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_MAX:
    default:
      SD_ERR_0( "act");
      break;

  } /* switch( act ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Client supplied a buffer for the return SS-Action, copy the
  ** returned SS-Action into this buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = *ss_act_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the return_cause indicator, as necessary.
  */
  #ifdef SD_USES_CAI
  /* If redirection is ON, set is_disabling_redir to FALSE.
  */
  if( sdss_is_redir(ss, SDSS_REDIR_VAL_CDMA_ANY) )
  {
    is_disabling_redir = FALSE;
  }
  #endif /* SD_USES_CAI */
  /* If last system attempted acquisition exists, update the
  ** return_cause indicator.
  */
  #ifdef SD_USES_CAI
  if( last_acq_sys_ptr  != NULL )
  {
    sd_ss_return_cause_update( ss,
                               event,
                               is_disabling_redir,
                               last_acq_sys_ptr );
  }
  #endif /* defined(SD_USES_CAI) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy contents of act field to true current act data structure if act
  ** enum type is not continue.
  */
  sd_ss_act_set_true_curr_act_ss(ss,ss_act_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_2( "<<<< Returned Action=%d, ss=%d <<<<",ss_act_ptr->act,ss);
  sdss_act_print( ss_act_ptr );
  sdss_iact_print( iact_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_SD20_LOGGING
  /* Send SD log packet.
  */
  sd_ss_log_last_action( ss );
  #endif /* FEATURE_SD20_LOGGING */

  /* Send SD event packet.
  */
  sd_ss_event_rprt( ss, event, ss_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Unlock the MAIN ss component.
  */
  SD_SS_FREE( ss_ptr );
  SYS_ARG_NOT_USED(i);

  act = ss_act_ptr->act;
  modem_mem_free(iact_ptr,MODEM_MEM_CLIENT_MMODE);
  modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);

  return act;

} /* sd_ss_event_proc */


/*===========================================================================

FUNCTION sd_ss_read_acq_prms

DESCRIPTION
  Read the acq parameters in the sd_ss. The 3 fields are read together under
  critical section to ensure a valid set of values.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                          sd_ss_read_acq_prms(

        sd_ss_e_type                  ss,
            /* The SS for which the acq is to be updated */

        sdsr_e_type                   *list,
            /* List from which acquisition is attempted */
        int                           *pos,
            /* List position of system that is attempted acquisition */

        sdss_acq_mode_e_type          *acq_mode
            /* The acquisition mode that is attempted */
)
{
  sd_ss_s_type          *ss_ptr               = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( list != NULL );
  SD_ASSERT( pos != NULL );
  SD_ASSERT( acq_mode != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_CRIT_SECT_ENTER();
  ss_ptr    = sd_ss_ptr(ss);
  *list     = ss_ptr->acq_list;
  *pos      = ss_ptr->acq_pos;
  *acq_mode = ss_ptr->acq_mode;
  SD_CRIT_SECT_EXIT();


  SD_MSG_HIGH_3("SD_SS:FRFL: Read Acq params list=%d, pos=%d, mode=%d",
                                                      *list,*pos,*acq_mode);
}


/*===========================================================================

FUNCTION sd_ss_act_get_true_curr_act_ss

DESCRIPTION
  Return the true SS-Action that was most recently returned to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_act_get_true_curr_act_ss(

        sd_ss_e_type              ss,

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX )

  switch( ss )
  {
    case SD_SS_MAIN:
      return sd_ss_act_get_true_curr_act( rtrn_act_ptr );

    case SD_SS_HYBR_1:
      return sd_ss_hybr_1_act_get_true_curr_act( rtrn_act_ptr );

    case SD_SS_HYBR_2:
      return sd_ss_hybr_2_act_get_true_curr_act( rtrn_act_ptr ,ss);

    case SD_SS_HYBR_3:
      return sd_ss_hybr_2_act_get_true_curr_act( rtrn_act_ptr ,ss);

    default:
      SD_MSG_HIGH_1("SD_SS:FRFL:Invalid ss %d for act_get",ss);
      return SD_SS_ACT_MAX;
  }
}


/*===========================================================================

FUNCTION sd_get_ss_state

DESCRIPTION
  Retrieve the SS-State in the SD_SS struct. This API serves to protect the
  remaining SD_SS struct from inadvertant access by limiting the access to
  only ss_state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN sdss_state_e_type              sd_get_ss_state(
       sd_ss_e_type                   ss
)
{
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  return sd_ss_ptr(ss)->ss_state;
}


/*===========================================================================

FUNCTION sd_set_ss_state

DESCRIPTION
  Update the SS-State in the SD_SS struct. This API serves to protect the
  remaining SD_SS struct from inadvertant access by limiting the access to
  only ss_state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void                           sd_set_ss_state(
       sd_ss_e_type                   ss,
       sdss_state_e_type              ss_state
)
{
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss_state, SDSS_STATE_MAX );

  SD_MSG_HIGH_2("SD_SS:FRFL: Updating SS state from %d to %d",
                                       sd_ss_ptr(ss)->ss_state,ss_state);
  sd_ss_ptr(ss)->ss_state = ss_state;
}


/*===========================================================================
FUNCTION sd_get_band_mode_mmode

DESCRIPTION
  Filter out the band and mode not supported by the target capability.
  Filter out the band and mode that are not in part of system preference.
  Filter out any overlapped bands.
  Bands that do not have corresponding modes.
  Modes that do not have corresponding bands.
  Based off sd_get_band_mode() , extended to support LTE.

  MODE
DEPENDENCIES

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
void                      sd_get_band_mode_mmode(

        sd_ss_mode_pref_e_type    *mode_ptr,
            /* Pointer to the mode preference that needs to be filtered */

        sd_ss_band_pref_e_type    *band_ptr,
            /* 3GPP band preference that needs to be filtered */

        sys_lte_band_mask_e_type  *band_ptr_lte,
            /* 4G LTE band preference that needs to be filtered */

        sd_ss_band_pref_e_type    *band_ptr_tds,
            /* TD-SCDMA band preference that needs to be filtered */

        sd_ss_mode_pref_e_type    sys_mode_pref,
            /* Enumeration of system mode perference */

        sd_ss_band_pref_e_type    sys_band_pref,
        /* 3GPP Enumeration of system selection band and sub-band preferences */

        sys_lte_band_mask_e_type  sys_band_pref_lte,
        /* 4G LTE Enumeration of system selection band and sub-band preferences */

        sd_ss_band_pref_e_type    sys_band_pref_tds
        /* TD-SCDMA Enumeration of system selection band and sub-band preferences */
)
{

  sd_ss_mode_pref_e_type          mode_pref;
  sd_ss_band_pref_e_type          band_pref;
  sys_lte_band_mask_e_type        lte_band_pref;
  sd_ss_band_pref_e_type          tds_band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( mode_ptr     != NULL );
  SD_ASSERT( band_ptr     != NULL );
  SD_ASSERT( band_ptr_lte != NULL );
  SD_ASSERT( band_ptr_tds != NULL );


  mode_pref     = *mode_ptr;
  band_pref     = *band_ptr;
  lte_band_pref = *band_ptr_lte;
  tds_band_pref = *band_ptr_tds;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out modes that are not part of the mode preference.
  */
  mode_pref = SD_GET_COMMON_MODE( mode_pref, sys_mode_pref );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out bands that are not in the band preference.
  */
  band_pref     = SD_GET_COMMON_BAND( band_pref, sys_band_pref );
  lte_band_pref = SYS_LTE_BAND_MASK_COMMON (&lte_band_pref,&sys_band_pref_lte);
  tds_band_pref = SD_GET_COMMON_BAND (tds_band_pref,sys_band_pref_tds);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out:
  ** 1. Bands that do not have corresponding modes.
  ** 2. Modes that do not have corresponding bands.
  */
  mode_pref = sdss_get_supp_mode_pref_mmode(mode_pref, band_pref,lte_band_pref, tds_band_pref);
  band_pref = sdss_get_supp_mode_band_pref( mode_pref, band_pref );
  lte_band_pref = sdss_get_supp_mode_band_pref_lte( mode_pref,lte_band_pref );
  tds_band_pref = sdss_get_supp_mode_band_pref_tds( mode_pref,tds_band_pref );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Filter out overlapping bands.
  */
  band_pref = sd_filter_out_overlap_band( band_pref );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy the filtered mode and band back into the input parameters.
  */
  *mode_ptr = mode_pref;
  *band_ptr = band_pref;
  *band_ptr_lte = lte_band_pref;
  *band_ptr_tds = tds_band_pref;
} /* sd_get_band_mode_mmode */


#if defined FEATURE_SD_LTE || defined SD_GSM_SUPPORTED || defined SD_WCDMA_SUPPORTED
/*===========================================================================

FUNCTION sd_update_rat_pri_list_filter_meas_srch

DESCRIPTION
  This function filter out the RAT in the rat_pri_list which contains
  bst_rat_acq_required = 0;
  Motivation: As NAS perform background measurement, they will only look
  into the 'RAT', but not look into the bst_rat_acq_required fields.
  Hence, sending RAT {G, W, L} with corresponding bst_rat_acq_required fields
  to be {1, 1, 0} will causing NAS to scan on all RATs {G , W , L}.
  However, the subsequent periodic service request will contains the same
  rat_pri_list = {G, W, L} with {1, 1, 0}. With Service Request, NAS
  will consider both 'RATs' and 'bst_rat_acq_required fields. Hence, NAS
  will crash because the LTE's bst_rat_acq_required is set to zero while
  they possibly has found LTE service in MEAS background search.


DEPENDENCIES
  None

RETURN VALUE
  Updated rat priority list (out_rat_pri_ptr)

SIDE EFFECTS
  None.

===========================================================================*/
static  void                           sd_update_rat_pri_list_filter_meas_srch(

        sys_rat_pri_list_info_s_type  *in_rat_pri_ptr,
            /* Pointer to the RAT priority list that needs to be updated
            */

        sys_rat_pri_list_info_s_type  *out_rat_pri_ptr
)
{
  uint32 i = 0;

  memset(out_rat_pri_ptr,0,sizeof(sys_rat_pri_list_info_s_type));

  SD_MSG_HIGH_1(" ue_cap.num_items = %d ", in_rat_pri_ptr->num_items);

  for( i=0; i < in_rat_pri_ptr->num_items; i++)
  {
    if( in_rat_pri_ptr->priority_list_info[i].bst_rat_acq_required == TRUE &&
        (
         (in_rat_pri_ptr->priority_list_info[i].acq_sys_mode != SYS_SYS_MODE_LTE &&
         in_rat_pri_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap != SYS_BAND_MASK_EMPTY )
         ||
         (in_rat_pri_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE &&
         !SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&(in_rat_pri_ptr->priority_list_info[i].bst_band_cap.lte_band_cap)))
        )
      )
    {
      /* Copy the PRI_LIST_INFO over */
      int index = out_rat_pri_ptr->num_items;
      out_rat_pri_ptr->priority_list_info[index] =
        in_rat_pri_ptr->priority_list_info[i];

      out_rat_pri_ptr->num_items++;

      SD_MSG_HIGH_3("MEAS: rat_pri_list[%d]->mode=%d, cnt=%d",
                   index,
                   out_rat_pri_ptr->priority_list_info[index].acq_sys_mode,
                   out_rat_pri_ptr->num_items);
    }
  }
}
#endif // FEATURE_SD_LTE || SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED


/*===========================================================================

FUNCTION sd_ss_act_set_true_curr_act_ss

DESCRIPTION
  Return the true SS-Action that was most recently returned to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void          sd_ss_act_set_true_curr_act_ss(

        sd_ss_e_type              ss,

        const sd_ss_act_s_type   *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  if( rtrn_act_ptr != NULL )
  {
    /* Copy the most recent SS-Action from the supplied buffer */
    SD_CRIT_SECT_ENTER();
    sd_ss_ptr(ss)->act = *rtrn_act_ptr;
    if( rtrn_act_ptr->act != SD_SS_ACT_CONTINUE )
    {
      sd_ss_ptr(ss)->true_curr_act = *rtrn_act_ptr;
    }
    SD_CRIT_SECT_EXIT();
  }
}


/*===========================================================================

FUNCTION sd_ss_hybr_1_act_get_true_curr_act

DESCRIPTION
  Return to the HYBR_1 client the SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_hybr_1_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action */
)
{

  return sd_ss_act_get_true_curr_act( rtrn_act_ptr );

} /* sd_ss_hybr_1_act_get_true_curr_act */


/*===========================================================================

FUNCTION sd_ss_hybr_2_act_get_true_curr_act

DESCRIPTION
  Return the true SS-Action that was most recently returned to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_hybr_2_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr ,
            /* Pointer to a buffer where to place returned SS-Action */
        sd_ss_e_type                ss
)
{

  sd_ss_act_e_type act = SD_SS_ACT_MAX;
  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer */
  SD_CRIT_SECT_ENTER();
  act = sd_ss_ptr(ss)->act.act;
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = sd_ss_ptr(ss)->act;
  }
  SD_CRIT_SECT_EXIT();

  return act;

} /* sd_ss_hybr_2_act_get_true_curr_act */


/*===========================================================================

FUNCTION sd_ss_update_hybr2_pref_list

DESCRIPTION
  This function to update HYBR2 PREF_LIST based on 1x or DO serving system 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void sd_ss_update_hybr2_pref_list(void)
{
  sdsr_e_type            pref_lst = SDSR_MAX_LST;
  sdsr_e_type            start_sys = SDSR_MAX_LST;
  
  /* If PREF_LST is empty, recalculate the list based on the serving system
  ** before we trigger USER_SS_PRERF event
  */
  
  if(sdsr_list_cnt( SDSR_HYBR_1_SRV_PREF_LST ) == 0)
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  */
    /* Check if service is acquired on MAIN or HDR.
    ** Depending on which stack has acquired service , it will be necessary to
    ** realign the HYBR2 search list accordingly
    */
    if( sd_si_is_full_srv_on( SD_SS_MAIN ) )
    {
      pref_lst = SDSR_SRV_PREF_LST;
      start_sys = SDSR_SRV_SYS;
    }
    #ifdef FEATURE_HDR_HYBRID
    else if(sd_si_is_srv_on( SD_SS_HYBR_1 ) || 
            sdss_is_flag_set(SD_SS_HYBR_2, SDSS_FLAG_HDR_DEACT_BY_HYBR2_BSR))
    {
      pref_lst = SDSR_MORE_PREF_LST_;
      start_sys = SDSR_HDR_SRV_SYS;
    }
    #endif
    if(pref_lst!= SDSR_MAX_LST)
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      /* update the hybr2 scan list.
      ** Delete all non compliant systems from the pref_lst and copy it to the
      ** backup list
      */
      sdsr_list_cpy(SDSR_HYBR_1_BACKUP_LST, pref_lst);
      sdsr_list_del_sys(SD_SS_HYBR_2,SDSR_HYBR_1_BACKUP_LST,SDSR_CAT_NON_COMPLY);
      pref_lst = SDSR_HYBR_1_BACKUP_LST;
   
   
      /* Update PREF_LST, START_PREF_LST, START_SYS)
      */
      sdsr_list_cpy( SDSR_HYBR_1_SRV_PREF_LST, pref_lst );
      sdsr_list_cpy( SDSR_HYBR_1_START_PREF_LST, pref_lst );
      sdsr_list_cpy( SDSR_HYBR_1_START_SYS,start_sys);
   
      SD_MSG_HIGH_1("PREF_LST empty, repopulate, hybr1_pref_lst cnt=%d", 
                   sdsr_list_cnt(SDSR_HYBR_1_SRV_PREF_LST) );
                             
    }
  }

}

/*===========================================================================

FUNCTION sd_si_info_ptr_set

DESCRIPTION
  This function is used to update the current SI info. This is used to ensure
  integrity of the structure. This is the ONLY function allowed to write to
  si_info. It copies over the entire structure of which some fileds may have
  updated by calling function.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void                           sd_si_info_ptr_set(

       sd_ss_e_type                   ss,

       const sd_si_info_s_type*       info
)
{
  /* This is the only place where pointer to si_info struct is obtained and
  ** it will ONLY be used under critical section */
    sd_si_info_s_type* si_info_ptr = &(sd_si_ptr(ss)->si_info);


  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( info != NULL );

  SD_CRIT_SECT_ENTER();
  if( si_info_ptr->version_num == info->version_num )
  {
    *si_info_ptr = *info;
    si_info_ptr->version_num ++;
  }
  else
  {
    SD_ERR_2("SI_INFO:FRFL: Version %d trying to overwrite version %d",
                       info->version_num,si_info_ptr->version_num);
  }
  SD_CRIT_SECT_EXIT();
}


/*===========================================================================

FUNCTION sd_invoke_info_callback

DESCRIPTION
  If the info call back has been registered invoke it with the latest si_info.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void                           sd_invoke_info_callback(

       sd_ss_e_type                   ss
          /* System selection stack */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  if( sd_si_ptr( ss )->si_info_cb_func != NULL )
  {
    sd_si_ptr( ss )->si_info_cb_func( ss, &sd_si_ptr(ss)->si_info );
  }
  else
  {
    SD_MSG_MED_1("SD_SI:FRFL: No SI Info callback registered for ss %d!!",ss);
  }
}


/*===========================================================================

FUNCTION sd_log_map_ss_act

DESCRIPTION
  Map SD SS-Action to Logged SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  The logged SS-Action that is associated with the input SD SS-Action.
  SDLOG_SS_ACT_MAX if the input SD SS-Action is invalid.

SIDE EFFECTS
  None.

===========================================================================*/
sdlog_ss_act_e_type       sd_log_map_ss_act(
        sd_ss_act_e_type          ss_act
            /* SD SS-Action to be mapped */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( ss_act, SD_SS_ACT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ss_act )
  {
    case SD_SS_ACT_CONTINUE:    return SDLOG_SS_ACT_CONTINUE;
    case SD_SS_ACT_ACQ_CDMA:    return SDLOG_SS_ACT_ACQ_CDMA;
    case SD_SS_ACT_ACQ_AMPS:    return SDLOG_SS_ACT_ACQ_AMPS;
    case SD_SS_ACT_ACQ_HDR:     return SDLOG_SS_ACT_ACQ_HDR;
    case SD_SS_ACT_MEAS_DED:    return SDLOG_SS_ACT_MEAS_DED;
    case SD_SS_ACT_MEAS_BACK:   return SDLOG_SS_ACT_MEAS_BACK;
    case SD_SS_ACT_PWR_SAVE:    return SDLOG_SS_ACT_PWR_SAVE;
    case SD_SS_ACT_ACCEPT:      return SDLOG_SS_ACT_ACCEPT;
    case SD_SS_ACT_REJECT:      return SDLOG_SS_ACT_REJECT;
    case SD_SS_ACT_RELEASE:     return SDLOG_SS_ACT_RELEASE;
    case SD_SS_ACT_ACQ_GPS:     return SDLOG_SS_ACT_ACQ_GPS;
    case SD_SS_ACT_ACQ_GWL:     return SDLOG_SS_ACT_ACQ_GW;
    case SD_SS_ACT_GET_NET_GW:  return SDLOG_SS_ACT_GET_NET_GW;

    case SD_SS_ACT_MAX:         return SDLOG_SS_ACT_MAX;
    default:                    return SDLOG_SS_ACT_MAX;

  } /* switch */

} /* sd_log_map_ss_act */


/*===========================================================================

FUNCTION sd_log_state_get

DESCRIPTION
  Map SD SS-Action to Logged SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  The logged SS-Action that is associated with the input SD SS-Action.
  SDLOG_SS_ACT_MAX if the input SD SS-Action is invalid.

SIDE EFFECTS
  None.

===========================================================================*/
sd_log_state_e_type       sd_log_state_get(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR */
)
{
  sdss_state_e_type     ss_state    = sd_get_ss_state(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-State is in PWR_SAVE
  */
  if( ss_state == SDSS_STATE_PWR_SAVE )
  {
    return SD_LOG_STATE_PWR_SAVE;
  }

  /* If SS-State is not acquisition, SD is in operation state.
  */
  if( ss_state != SDSS_STATE_ACQ )
  {
    return SD_LOG_STATE_OPERATION;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, SD is in acquisition state. Return the acquisition
  ** substate.
  */
  switch( sdss_eng_stack_get_state(ss) )
  {
    case SDSS_ACQ_STT_START:      return SD_LOG_STATE_ACQUISITION;
    case SDSS_ACQ_STT_MORE_PREF:  return SD_LOG_STATE_ALT_ACQUISITION;

    case SDSS_ACQ_STT_REACQ:
    case SDSS_ACQ_STT_REACQ_GWL:
    case SDSS_ACQ_STT_REACQ_FULL_SRV:  return SD_LOG_STATE_REACQUISITION;

    case SDSS_ACQ_STT_DONE:
    case SDSS_ACQ_STT_SUSPEND:
    case SDSS_ACQ_STT_MAX:
    case SDSS_ACQ_STT_NONE:
    case SDSS_ACQ_STT_PWR_SAVE:
    case SDSS_ACQ_STT_PWR_SAVE_ENTER:
    case SDSS_ACQ_STT_SCAN_CONT:
    default:                      return SD_LOG_STATE_ACQUISITION;
  }

} /* sd_log_state_get */


/*===========================================================================

FUNCTION sd_ss_act_is_equal

DESCRIPTION
  Compare two SS-Actions and return TRUE if they are equal to one another.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the SS-Action that is pointed to by the act_a_ptr is equal to the
  SS-Action that is pointed to by the act_b_ptr. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean          sd_ss_act_is_equal(

        const sd_ss_act_s_type    *act_a_ptr,
            /* Pointer to an SS-Action to be compared */

        const sd_ss_act_s_type    *act_b_ptr,
            /* Pointer to an SS-Action to be compared */

        boolean                   meas_list_is_diff,
            /* Indicate whether the most recent list of systems to be
            ** measured is different than the previous list of systems to be
            ** measured */

        sd_ss_e_type              ss
)
{
  SD_ASSERT( act_a_ptr != NULL );
  SD_ASSERT( act_b_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the two SS-Actions are not the same return FALSE.
  */
  if( act_a_ptr->act != act_b_ptr->act )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two SS-Actions are identical, so compare the
  ** action's associated parameters (for actions that have parameters).
  */
  switch( act_a_ptr->act )
  {
    case SD_SS_ACT_ACQ_CDMA:
      if( act_a_ptr->prm.acq_cdma.band != act_b_ptr->prm.acq_cdma.band ||
          act_a_ptr->prm.acq_cdma.chan != act_b_ptr->prm.acq_cdma.chan )
      {
        return FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_AMPS:
      if( act_a_ptr->prm.acq_amps.band != act_b_ptr->prm.acq_amps.band ||
          act_a_ptr->prm.acq_amps.sys != act_b_ptr->prm.acq_amps.sys )
      {
        return FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_HDR:
      if( act_a_ptr->prm.acq_hdr.band != act_b_ptr->prm.acq_hdr.band ||
          act_a_ptr->prm.acq_hdr.chan != act_b_ptr->prm.acq_hdr.chan ||
          act_a_ptr->prm.acq_hdr.acq_mode != act_b_ptr->prm.acq_hdr.acq_mode)
      {
        return FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_MEAS_DED:
    case SD_SS_ACT_MEAS_BACK:
      /* If measurement type is different or list of channels to measure is
      ** different than SDSR_MEAS_LIST, return FALSE.
      */
      if( act_a_ptr->prm.meas.type != act_b_ptr->prm.meas.type ||
          meas_list_is_diff )
      {
        return FALSE;
      }
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_ACT_ACQ_GWL:
      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
      /* If user initiated, then always return false.
      ** If netwok selection type is automaitc, and do reselectioin,
      ** aways return false
      */

      if ( (   act_a_ptr->prm.acq_gwl.net_sel_type ==
                                 SYS_NETWORK_SELECTION_MODE_MANUAL
            && act_a_ptr->prm.acq_gwl.selected_plmn_type ==
                                 SD_MANUALLY_SELECTED_SPECIFIED_PLMN)   ||
           (   act_b_ptr->prm.acq_gwl.net_sel_type ==
                                 SYS_NETWORK_SELECTION_MODE_MANUAL
            && act_b_ptr->prm.acq_gwl.selected_plmn_type ==
                                 SD_MANUALLY_SELECTED_SPECIFIED_PLMN)   ||
           (   act_a_ptr->prm.acq_gwl.net_sel_type ==
                                 SYS_NETWORK_SELECTION_MODE_MANUAL_CSG
            && act_a_ptr->prm.acq_gwl.selected_plmn_type ==
                                 SD_MANUALLY_SELECTED_SPECIFIED_PLMN)   ||
           (   act_b_ptr->prm.acq_gwl.net_sel_type ==
                                 SYS_NETWORK_SELECTION_MODE_MANUAL_CSG
            && act_b_ptr->prm.acq_gwl.selected_plmn_type ==
                                 SD_MANUALLY_SELECTED_SPECIFIED_PLMN)   ||
           (   act_a_ptr->prm.acq_gwl.net_sel_type ==
                                 SYS_NETWORK_SELECTION_MODE_AUTOMATIC
            && act_a_ptr->prm.acq_gwl.is_user_resel )                    ||
           (   act_b_ptr->prm.acq_gwl.net_sel_type ==
                                 SYS_NETWORK_SELECTION_MODE_AUTOMATIC
            && act_b_ptr->prm.acq_gwl.is_user_resel)
         )
      {
        return FALSE;
      }

      /* Compare other Acquisition parameters.
      */

      if ( act_a_ptr->prm.acq_gwl.net_sel_type !=
                                act_b_ptr->prm.acq_gwl.net_sel_type    ||
           act_a_ptr->prm.acq_gwl.srv_domain_pref !=
                                act_b_ptr->prm.acq_gwl.srv_domain_pref ||
           act_a_ptr->prm.acq_gwl.is_orig_pending !=
                                act_b_ptr->prm.acq_gwl.is_orig_pending ||
           act_a_ptr->prm.acq_gwl.voice_domain_pref !=
                               act_b_ptr->prm.acq_gwl.voice_domain_pref  ||
           act_a_ptr->prm.acq_gwl.is_user_resel !=
                                act_b_ptr->prm.acq_gwl.is_user_resel   ||
           act_a_ptr->prm.acq_gwl.selected_plmn_type !=
                                act_b_ptr->prm.acq_gwl.selected_plmn_type ||
           act_a_ptr->prm.acq_gwl.srv_req_type !=
                                act_b_ptr->prm.acq_gwl.srv_req_type    ||
          (   ((act_a_ptr->prm.acq_gwl.net_sel_type ==
                                       SYS_NETWORK_SELECTION_MODE_MANUAL) ||
          (   act_a_ptr->prm.acq_gwl.net_sel_type ==
                                       SYS_NETWORK_SELECTION_MODE_MANUAL_CSG))
            && !sys_plmn_match (act_a_ptr->prm.acq_gwl.plmn,
                                act_b_ptr->prm.acq_gwl.plmn))
         )
      {
        return FALSE;
      }

      /* This code block is needed to cover the sceanrio of VOLTE to CSFB or regular e911
      ** transition. CM should immeadiately issue GW search to quickly place the call
      */
      if (sdss_get_is_full_srv_req_during_emerg( SD_SS_MAIN))
      {
        if( memcmp(&(act_a_ptr->prm.acq_gwl.rat_pri_list_info),
                   &(act_b_ptr->prm.acq_gwl.rat_pri_list_info),
                   sizeof(sys_rat_pri_list_info_s_type) ))
        {
          return FALSE;
        }
      }
      
      #ifdef FEATURE_MMSS_3_1
      if(sdss_is_mmss_operation(ss))
      {
        if( memcmp(&(act_a_ptr->prm.acq_gwl.rat_pri_list_info),
                   &(act_b_ptr->prm.acq_gwl.rat_pri_list_info),
                   sizeof(sys_rat_pri_list_info_s_type) ))
        {
          return FALSE;
        }
      }
      else
      #endif
      {
        if(act_a_ptr->prm.acq_gwl.gw_band_pref !=
                           act_b_ptr->prm.acq_gwl.gw_band_pref    ||
           act_a_ptr->prm.acq_gwl.acq_order_pref !=
                           act_b_ptr->prm.acq_gwl.acq_order_pref  ||
           act_a_ptr->prm.acq_gwl.gwl_mode_pref !=
                           act_b_ptr->prm.acq_gwl.gwl_mode_pref   ||
           !SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&act_a_ptr->prm.acq_gwl.lte_band_pref,
                                           &act_b_ptr->prm.acq_gwl.lte_band_pref)
                              ||
           act_a_ptr->prm.acq_gwl.tds_band_pref !=
                           act_b_ptr->prm.acq_gwl.tds_band_pref)
        {
          return FALSE;
        }
      }

      #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED || FEATURE_SD_LTE */
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SD_SS_ACT_CONTINUE:
    case SD_SS_ACT_PWR_SAVE:
    case SD_SS_ACT_ACCEPT:
    case SD_SS_ACT_ACQ_GW:
    case SD_SS_ACT_REJECT:
    case SD_SS_ACT_RELEASE:
    case SD_SS_ACT_ACQ_GPS:
    case SD_SS_ACT_GET_NET_GW:
    case SD_SS_ACT_MAX:
    default:
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the SS-Actions in question are identical - return TRUE.
  */
  return TRUE;

} /* sd_ss_act_is_equal */


/*===========================================================================

FUNCTION sd_si_srv_offline

DESCRIPTION
  Inform the System Indication component that the MS is going offline.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                         sd_si_srv_offline(

        sdss_opr_mode_e_type          opr_mode
            /* Offline mode (CDMA, AMPS, HDR, etc) */
)
{
  #ifdef SD_USES_DB_SERVICES
  db_items_value_type   db_item;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the DB_DIGITAL item according to the offline state.
  */
  #ifdef SD_USES_DB_SERVICES
  if( opr_mode == SDSS_OPR_MODE_OFFLINE_AMPS )
  {
    db_item.digital = FALSE;
  }
  else if ( opr_mode == SDSS_OPR_MODE_OFFLINE_CDMA )
  {
    db_item.digital = TRUE;
  }

  db_put( DB_DIGITAL, &db_item );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the service indicator to OFF.
  */
  sd_si_srv_off( SD_SS_MAIN, FALSE );
  #ifdef FEATURE_HDR_HYBRID
  sd_si_srv_off( SD_SS_HYBR_HDR, FALSE );
  #endif

  if(!sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
    sd_si_srv_off(SD_SS_HYBR_2, FALSE);
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_si_srv_off(SD_SS_HYBR_3, FALSE);
  #endif

} /* sd_si_srv_offline */
  /*lint +esym(715, opr_mode) */ /* param not referenced */


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)

/*===========================================================================

FUNCTION sd_si_net_list_rpt_cb

DESCRIPTION
  Notify the registered client (e.g., CM) the results of the get network
  request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_si_net_list_rpt_cb(

    sys_plmn_id_s_type            *rplmn_ptr,
       /* Pointer to a structure containing the last registered PLMN ID */

    sd_ss_e_type                           ss,

    sys_found_plmn_list_u_type *plmn_list_ptr,
        /* Pointer to a structure containing the list of available PLMNs */

    sys_plmn_list_status_e_type    plmn_list_status
        /* Gives the status of plmn list request (success/abort) */

)
{
  sd_si_s_type        *si_ptr       = sd_si_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( plmn_list_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the SD client of get network results.
  */
  if ( si_ptr->gw_net_list_cb_func != NULL )
  {
    SD_MSG_HIGH_0( "Get network list result posted");

    si_ptr->gw_net_list_cb_func( rplmn_ptr,
                                 ss,
                                 plmn_list_ptr,
                                 plmn_list_status
                               );

  }

} /*  sd_si_net_list_rpt_cb() */


/*===========================================================================

FUNCTION sd_si_update_sim_state

DESCRIPTION
  Update the SIM state if it was changed and report to the SD client the new
  SIM state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                      sd_si_update_sim_state(

        sys_sim_state_e_type      sim_state,
           /* New SIM state reported from REG.
           */

        sd_ss_e_type                ss
           /* ss to update
           */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );
  sys_sim_state_e_type   lte_rpm_sim_state = SYS_SIM_STATE_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( sim_state, SYS_SIM_STATE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* LTE RPM might adjust the SIM state to CS_PS_INVALID or PS_INVALID
  ** if event timer is still running and EMM generic timer has reach maximum
  */
  lte_rpm_sim_state = cmrpm_lte_update_sim_state();
  if ( (lte_rpm_sim_state == SYS_SIM_STATE_CS_PS_INVALID) ||
       (lte_rpm_sim_state == SYS_SIM_STATE_PS_INVALID) )
  {
    sim_state = lte_rpm_sim_state;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the sim_state was changed.
  */
  if ( sdss_get_sim_state(ss) != sim_state)
  {
     /* Sim state is changed, update it and
     */
     sdss_update_sim_state( sim_state, ss);

     /* Report to SD client if CB was registered.
     */
     if ( si_ptr->sim_state_cb_func != NULL)
     {
       si_ptr->sim_state_cb_func( sim_state, ss );
     }
  } /* if ( sdss_get_sim_state(ss) != sim_state )  */


} /* sd_si_update_sim_state() */


/*===========================================================================

FUNCTION sd_si_update_data_suspend_state

DESCRIPTION
  Update the data suspend flag if it was changed and report to the SD client
  the new data suspend state.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                      sd_si_update_data_suspend_state
(

        sd_ss_e_type              ss,
           /* The ss whose data suspend is to be updated */

        boolean                   data_suspend,
           /* New data suspend state reported from REG  */

        sys_srv_status_e_type     srv_status
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( si_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ss == SD_SS_HYBR_2 &&
      sdss_is_sglte() &&
      sdss_get_domain_pref(ss) == SD_SS_SRV_DOMAIN_PREF_CS_ONLY )
  {
    SD_MSG_HIGH_0("SC_SGLTE: Not sending data_suspend on HYBR_2 (CS-only)");
    return;
  }

  /* Force data_suspend flag if there is no full service and UE is not emergency attached
  */
  if( !( sys_srv_status_is_full_srv( srv_status ) ||
         data_suspend ||
         sdss_get_emerg_attached_state()) )
  {
    data_suspend = TRUE;

    SD_MSG_HIGH_1( "Force data_suspend flag, data_suspend = %d",
                  data_suspend);

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the data_suspend was changed.
  */
  if ( si_ptr->data_suspend != data_suspend )
  {
     /* Data suspend state is changed, update it and
     */
     si_ptr->data_suspend = data_suspend;

     /* Report to SD client if CB was registered.
     */
     if ( si_ptr->data_suspend_cb_func != NULL)
     {
       si_ptr->data_suspend_cb_func( data_suspend, ss );
     }
  } /* if ( si_ptr->data_suspend != data_suspend )  */


} /* sd_si_update_data_suspend_state() */

#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
/*===========================================================================
FUNCTION sd_misc_gwl_is_sys_lost_map_acq_fail

DESCRIPTION
  This is a utility API to be called ONLY from GWL service lost API. This API
  will check the system variables and return whether the event of sys_lost
  needs to be mapped to that og acq_plmn_failed.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE: Map this sys_lost to acquisition failure
  FALSE: Proceed with service lost processing.

SIDE EFFECT
  None
===========================================================================*/
EXTERN boolean                    sd_misc_gwl_is_sys_lost_map_acq_fail(
      sd_ss_e_type               ss
)
{
  if( ss < SD_SS_MAX )
  {
    sdss_state_e_type     ss_state   = sd_ss_ptr( ss )->ss_state;
    sys_srv_status_e_type srv_status = sd_si_ptr( ss )->si_info.srv_status;

    return( (ss_state == SDSS_STATE_ACQ || ss_state == SDSS_STATE_VRFY)
             &&
             srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL
             &&
             !sdss_is_target_cap_3gpp_only(ss)
             &&
             !sdss_get_pref_chg_in_ltd_reg(ss)
           );
  }
  return FALSE;
}


/*===========================================================================

FUNCTION sd_misc_gwl_map_sys_lost_to_acq_fail

DESCRIPTION
  Maps GW service lost to acq_plmn_failed so that the system selection scans
  can proceed without restarting.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type                  sd_misc_gwl_map_sys_lost_to_acq_fail
(
        sd_ss_e_type              ss,
            /* SD Stack on which to process */

        sys_sim_state_e_type      sim_state,
            /* SIM state */

        boolean                   data_suspend,
            /* Data suspend flag from REG */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action */

		sys_acq_status_e_type      acq_status
		    /* acq_status returned in srv_lost report  */
)
{
  sd_ss_ind_gw_acq_plmn_failed_cmd  cmd;
  SD_MSG_HIGH_0("LTD_REG -> NO SRV mapped to acq_plmn_failed");
  #ifndef FEATURE_JCDMA
  sd_si_srv_off( ss, FALSE );
  #endif

  cmd.rtrn_act_ptr = rtrn_act_ptr;
  cmd.ss = ss;
  cmd.sim_state = sim_state;
  cmd.data_suspend = data_suspend;
  cmd.scan_status = SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND;
  cmd.lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
  cmd.req_id = 0;
  cmd.extend_srv_info = SYS_EXTEND_SRV_INFO_NONE;
  cmd.csg_info_ptr = NULL;
  cmd.registered_plmn = NULL;
  cmd.acq_status = acq_status;

  return sdcmd_gw_acq_plmn_failed_ss_2(&cmd);
}


/*===========================================================================

FUNCTION sd_ss_gw_srv_sys_net_sel_update

DESCRIPTION
   Updates the  net_sel_mode field in srv_sys depending on service_status.
   Called when SD gets service_ind while in Limited service

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  Void

SIDE EFFECTS
  None.
===========================================================================*/
static void sd_ss_gw_srv_sys_net_sel_update (
    const sd_ss_e_type          ss,
    const sys_srv_status_e_type service_status
)
{
  sdsr_e_type          last_sys;
  sdsr_ref_type        last_sys_ref;

  /* Currently not handling Automatic -> Limited transition.
  ** Needs clarification from NAS
  */

  if (service_status == SYS_SRV_STATUS_SRV)
  {
    /* Update with last_full_sys or last_user_sys depending on
    ** current mode.
    */
    if ( sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_MANUAL) )
    {
      last_sys = sdss_sr_list_map2( ss, SDSR_GWL_LAST_USER_SYS);

    }
    else
    {
      last_sys = sdss_sr_list_map2( ss, SDSR_GWL_LAST_FULL_SYS);
    }

    last_sys_ref = sdsr_list_get_ref( last_sys, 0 );

    if(last_sys_ref == SDSR_REF_NULL)
    {
      SD_ERR_0("last_sys_ref NULL");
      return;
    }

    /* Update the Serving system.
    */
    sd_ss_srv_sys_update( ss,
                          last_sys,
                          SDSR_ACQ_GEO_LST,
                          SDSR_ACQ_PREF_LST,
                          SDSR_MAX );

    /* Update the Idle system.
    */
    sd_ss_idle_sys_update( ss,
                           last_sys,
                           SDSR_ACQ_GEO_LST,
                           SDSR_ACQ_PREF_LST );

  }
  return;
} /* sd_ss_gw_srv_sys_net_sel_update */


/*===========================================================================

FUNCTION sd_si_compare_plmn

DESCRIPTION
Compare PLMNs and return true if they are same.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean sd_misc_is_plmn_lac_match(sd_plmn_s_type plmn1,sd_plmn_s_type plmn2)
{
  if(plmn1.lac!=plmn2.lac)
  {
    return FALSE;
  }

  return sys_plmn_match(plmn1.plmn_id,plmn2.plmn_id);
}

#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) */


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
/*===========================================================================

FUNCTION sd_misc_adjust_service_status_in_camp_mode

DESCRIPTION
  This is the one location to adjust the incoming service status when UE is
  operating in CAMPED_ONLY mode. The adjustment is for internal SD
  processing only and not for reporting to registered clients.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static sys_srv_status_e_type     sd_misc_adjust_service_status_in_camp_mode(
       sys_srv_status_e_type     service_status,
       sys_reg_domain_e_type     reg_domain
)
{
  if(sdss_is_sglte())
  {
    /* With SGLTE: CAMP_ONLY mode is used when we're completely out of service
    ** So when as soon as we receives a PLMN. We just map to FULL_SERVICE and
    ** this triggers SD to return CONTINUE. Thus, we wait on further action.
    **
    ** At the same time, Policy Manager will receives new PLMN, and check if
    ** we should function in SGLTE or non-SGLTE. It will change the UE Mode.
    */
    SYS_ARG_NOT_USED(reg_domain);
    if( sdss_get_camp_mode() == SD_SS_PREF_CAMP_MODE_ON )
      return SYS_SRV_STATUS_SRV;
    else
      return service_status;
  }
  else
  {
    /* In the CAMPED_ONLY mode NAS will always send back limited service along
    ** with relevant reg_domain. If reg_domain contains PS translate
    ** the service indication to FULL service for internal SD computation.
    **
    ** The reason for considering only PS-Service is because:
    ** - QCS currently handling both CS+PS service
    ** - MSM is in CAMP_ONLY mode. (Only MSM uses Camp-Only)
    ** - When we receives service on MSM, if service supports PS:
    **    --> Move PS to MSM. This means we maps to "Full Service" and to
    **        let SD issued "CONTINUE" action. Thus, we wait for RIL to
    **        update PS on MSM.
    ** - But if service does not support PS:
    **    --> Stay in Limited Service. This will triggere aggressive limited
              reselection.
    **    --> No action will be taken on RIL side.
    */
    if( sdss_get_camp_mode() == SD_SS_PREF_CAMP_MODE_ON &&
        ( reg_domain == SYS_REG_DOMAIN_PS_SERVICE_ONLY ||
          reg_domain == SYS_REG_DOMAIN_CS_PS_SERVICE)
      )
    {
      return SYS_SRV_STATUS_SRV;
    }
    else
    {
      return service_status;
    }
  }
}
#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */


/*===========================================================================

FUNCTION sd_si_update

DESCRIPTION
  Set the service indicator to TRUE and update the system indicators per
  the input system record.

  If one or more of the service indicators has changed, inform the registered
  client (e.g., CM) of the new service indicators.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
  void                            sd_si_update(
             /*lint -esym(550, is_uz_check) */ /* para not accessed */
        sd_ss_e_type                    ss,
            /* System selection - MAIN or HDR */

        sd_si_update_e_type             update_criteria,
            /* Update criteria */

        sys_srv_status_e_type           srv_status,
            /* Current service status ( LIMITED, FULL service etc ) */

        sys_srv_domain_e_type           srv_domain,
            /* Registered service domain */

        sdsr_ref_type                   sr_ref,
            /* System record reference according to which to update the
            ** system indicators */

        boolean                         is_uz_check,
            /* Flag to indicate if User zone selected
            ** check needs to be done */

        const sd_mode_e_type            *mode_ptr,
           /* Pointer to mode - ignored if NULL */

        const sd_band_e_type            *band_ptr,
            /* Pointer to band - ignored if NULL */

        const sd_chan_type              *chan_ptr,
             /* Pointer to channel - ignored if NULL */

        const sd_si_mode_info_u_type    *mode_info_ptr,
            /* Pointer to mode info */

        sdsr_e_type                     colloc_lst,
            /* The collocated list */

        const sys_active_prot_e_type    *hdr_active_prot_ptr,
            /* The HDR active protocol revision */

        const sys_personality_e_type    *hdr_personality_ptr,
            /* The HDR active personality */

        const sdss_sys_addn_info_s_type  *curr_sys_info,
            /* Association tag of the currently acquired
            ** system, if any */

        sys_srv_status_e_type           cs_srv_status,
            /* Current CS service status */

        sys_lte_cs_capability_e_type    lte_cs_capability,
            /* LTE CS capability */

        sd_sib8_status                  sib8_available,
            /*sib8 status */

        boolean                         lte_ims_voice_avail,
            /* If IMS voice supported */

        sys_ims_lte_emerg_supp_e_type   lte_ims_emerg_avail,
            /* If IMS emergency call is supported over LTE */

        uint16                            req_id,
          /* Request Id to be populated back to client in case required for SS events */

        sys_extend_srv_info_e_type   extend_srv_info,
            /* Extended service info */

        sys_reg_domain_e_type           reg_domain,
          /* The REG domain value is used in CAMPED_ONLY mode by the
          ** CM client to determine service availability */

         /*lint -esym(715, csg_info_ptr) */ /* param not referenced */
        const sys_csg_info_s_type*      csg_info_ptr,
          /* CSG Information */

        /* Indicates if new srv needs to be informed to CM*/
        boolean                         is_silent

)
/*lint -esym(715, csg_info_ptr) */ /* param not referenced */
{
  sd_si_s_type          *si_ptr       = sd_si_ptr( ss );
  sd_si_info_s_type     *si_info_ptr  = &si_ptr->si_info;

  /* Get the current SS-State.
  */
  sdss_state_e_type     ss_state      = sdss_get_ss_state(ss);

  /* Point at system record per which to update the service indicators.
  */
  sdsr_s_type           *sr_ptr       = sdsr_tbl_get_ptr( sr_ref );

  boolean               is_pref_srv_acq;
  boolean               is_redir_or_ho;
  boolean               is_mcc_imsi_11_12_changed = FALSE;
  boolean               is_stable_in_svc;
  #ifdef FEATURE_HDR_HYBRID
  boolean               realign_hdr           = FALSE;
  boolean               is_colloc_changed  = FALSE;
  boolean               is_assn_tag_changed = FALSE;
  boolean               is_geo_changed = FALSE;
  boolean               is_geo_count_changed = FALSE;
  sdss_sys_addn_info_s_type  *prev_sys_info  = sdss_get_prev_sys_info(ss);
  sdsr_e_type           temp_lst;
  sdsr_e_type           temp2_lst;
  #endif /* FEATURE_HDR_HYBRID */

  #ifdef SD_USES_DB_SERVICES
  db_items_value_type   db_item;
  #endif

  sd_mode_e_type        mode;
  sd_band_e_type        band = SYS_BAND_CLASS_NONE;
  sd_band_e_type        band_lte = SYS_BAND_CLASS_NONE;
  sd_band_e_type        band_tds = SYS_BAND_CLASS_NONE;
  sd_chan_type          chan;
  sd_sid_u_type         prev_sid;
  sys_active_prot_e_type  hdr_active_prot;
  sys_personality_e_type  hdr_personality;
  uint8                   count = 0;

  boolean              realign_lte = FALSE;

  /* mcc_list_info holds the mcc list information per stack - applicable to CDMA/HDR.
  */
  sys_plmn_mcc_list    *mcc_list_info = sdss_get_mcc_list_info(ss);

  is_uz_check = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( sr_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( update_criteria, SD_SI_UPDATE_MAX );

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );
  SD_ASSERT_ENUM_IS_INRANGE( ss_state, SDSS_STATE_MAX );

  SD_ASSERT_ENUM_IS_INRANGE( sr_ptr->sys.mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sr_ptr->sys.band, SD_SS_BAND_PREF_MAX );
  SD_ASSERT( sr_ptr->roam < SYS_ROAM_STATUS_MAX );
  SD_ASSERT( srv_status <= SYS_SRV_STATUS_MAX );
  SD_ASSERT( srv_domain <= SYS_SRV_DOMAIN_MAX );
  SD_ASSERT( cs_srv_status <= SYS_SRV_STATUS_MAX );
  #if defined(FEATURE_LTE)
  SD_ASSERT( lte_cs_capability < SYS_LTE_CS_CAPABILITY_MAX );
  #endif
  SD_ASSERT( sib8_available < SD_SIB8_MAX);
  SD_ASSERT_ENUM_IS_INRANGE( reg_domain, SYS_REG_DOMAIN_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(is_uz_check);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* update the request Id supplied with this function so that it will be communicated
      back to client.
   */

  hdr_active_prot = si_info_ptr->hdr_active_prot;
  hdr_personality = si_info_ptr->hdr_personality;

  /* If the minimum service update criteria is not met, return now.
  */
  if( update_criteria < si_ptr->si_update_criteria )
  {
    return;
  }

  /* If we got here, the minimum service update criteria is met, so reset
  ** the service update criteria to successful acquisition.
  */
  si_ptr->si_update_criteria = SD_SI_UPDATE_ACQ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get acquired band and channel
  */
  mode = sr_ptr->sys.mode;
  if( mode != SD_MODE_LTE &&
      mode != SD_MODE_TDS)
  {
    band = sdss_map_band_pref_to_band(sr_ptr->sys.band);
  }

  if( mode == SD_MODE_LTE || mode == SD_MODE_GWL )
  {
    sys_lte_band_mask_e_type lte_band_pref_local = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band);
    band_lte = sdss_map_band_pref_to_band_lte(lte_band_pref_local);
  }

  if (mode == SD_MODE_TDS)
  {
    SD_ASSERT_ENUM_IS_INRANGE( sr_ptr->sys.tds_band, SD_SS_BAND_PREF_MAX );
    band_tds = sdss_map_band_pref_to_band_tds(sr_ptr->sys.tds_band);
  }

  /* Note that the mode_ptr points to the single RAT for which there might
  ** be a need to update the indicator.
  */
  if( mode_ptr != NULL &&
      *mode_ptr == SD_MODE_LTE
    )
  {
    band = band_lte;
  }

  if( mode_ptr != NULL &&
      *mode_ptr == SD_MODE_TDS
    )
  {
    band = band_tds;
  }

  chan = sr_ptr->sys.chan;

  /* Overwrite system's mode and/or band if input mode and/or band parameters
  ** are provided. Note that eventually, a single RAT and a single band will
  ** be passed to update the sd_si_info_s_type.
  */
  if( mode_ptr != NULL )
  {
    mode = *mode_ptr;
  }

  if( band_ptr != NULL )
  {
    band = *band_ptr;
  }

  if ( chan_ptr != NULL )
  {
    chan = *chan_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get HDR active protocol revision
  */
  if( hdr_active_prot_ptr != NULL )
  {
    hdr_active_prot = *hdr_active_prot_ptr;

    SD_ASSERT( hdr_active_prot == SYS_ACTIVE_PROT_NONE ||
               BETWEEN( hdr_active_prot,
                        SYS_ACTIVE_PROT_HDR_START,
                        SYS_ACTIVE_PROT_HDR_END ) );
  }

  /* Get HDR active personality
  */
  if( hdr_personality_ptr != NULL )
  {
    hdr_personality = *hdr_personality_ptr;

    SD_ASSERT( hdr_personality == SYS_PERSONALITY_NONE ||
               BETWEEN( hdr_personality,
                        SYS_PERSONALITY_HDR_START,
                        SYS_PERSONALITY_HDR_END ) );
  }
  if(mode == SD_MODE_HDR)
  {
    SD_MSG_HIGH_2("HDR rev %d personality %d",hdr_active_prot,hdr_personality);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform client if registration was done.
  */
  if( update_criteria == SD_SI_UPDATE_REG &&
      si_ptr->reg_done_cb_func != NULL )
  {
     SD_MSG_HIGH_1( "Registration done, mode=%d",
                  sr_ptr->sys.mode);

     si_ptr->reg_done_cb_func( ss, mode, srv_status, srv_domain );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out whether system determination is still attempting to acquire
  ** a more preferred service.
  */
  if( ss_state == SDSS_STATE_OPR ||
      sdss_is_resel_state(ss, SDSS_RESEL_STT_SRCH) )
  {
    is_pref_srv_acq = FALSE;
  }
  else
  {
    is_pref_srv_acq = TRUE;

    // Update BSR in progress status to TRUE while doing immediate BSR
    if(ss == SD_SS_MAIN && 
       mode == SD_MODE_CDMA &&
       !is_silent)
    {
      sdcmd_cdma_bsr_in_progress_cb(TRUE);
    }
  }

  /* Created a new indication to denote that SD is not performing any scans 
  ** at the moment. The conditions used to ser the is_pref_srv_acq are some
  ** what ambiguous. This new indication can eventually replace the
  ** is_pref_srv_acq indication altogether if there are no other conerns.
  */
  if( ss_state == SDSS_STATE_OPR )
  {
    is_stable_in_svc = TRUE;

    /* Once BSR is completed, update BSR in progress status to FALSE.
    */
    if(ss == SD_SS_MAIN && !is_silent)
    {
      sdcmd_cdma_bsr_in_progress_cb(FALSE);
    }
  }
  else
  {
    is_stable_in_svc = FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out whether redirection or handoff indication is ON.
  */
  if( sdss_is_redir( ss, SDSS_REDIR_VAL_NONE) &&
      sdss_is_handoff(ss, SDSS_HO_VAL_NONE) )
  {
    is_redir_or_ho = FALSE;
  }
  else
  {
    is_redir_or_ho = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set SI system for SID comparison.
  */
  prev_sid = si_info_ptr->sid;

  #ifdef FEATURE_HDR_HYBRID
  /* Check if the collocated list has changed */

  /* Get the association tag of the previous system acquired.
  */
  if(curr_sys_info != NULL)
  {
    /* Compare the Geo Offset of the previous system acquired.
    */
    if( prev_sys_info->geo_offset != curr_sys_info->geo_offset )
    {
      is_geo_changed = TRUE;
    }

    /* Compare the geo count with the previous system acquired.
    */
    if( prev_sys_info->geo_count != curr_sys_info->geo_count )
    {
       is_geo_count_changed = TRUE;
    }

    /* Compare the association tag of the previous system acquired.
    */
    if( prev_sys_info->assn_tag != curr_sys_info->assn_tag )
    {
       is_assn_tag_changed = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( colloc_lst != SDSR_MAX )
  {
    /*If the new system is collocated, and the previous system was not
    ** collocated, is_colloc is true
    */
    if( sdsr_list_cnt( colloc_lst ) > 0 &&
        sdsr_list_cnt( SDSR_PREV_COLLOC_LST ) == 0 )
    {
      is_colloc_changed = TRUE;
    }
    else
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
      /* When in hybrid operation, since both main stack and hdr stack use
      ** this function, it is essential that they use different lists to
      ** avoid overwriting the same list and causing erroneous behavior
      */

      temp_lst  =  sdss_sr_list_map2( ss, SDSR_TEMP_LST );
      temp2_lst =  sdss_sr_list_map2( ss, SDSR_TEMP2_LST );

      /* Copy the to, from lists into temp lists.
      */
      sdsr_list_cpy( temp_lst, SDSR_PREV_COLLOC_LST);
      sdsr_list_cpy( temp2_lst, colloc_lst);

      /* Sort the temp lists before comparing*/
      sdsr_list_order_per_prl_lst(temp_lst);
      sdsr_list_order_per_prl_lst(temp2_lst);


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      is_colloc_changed = !( sdsr_list_is_same( temp_lst,
                                                temp2_lst,
                                                         SDSR_EQUAL_MODE )
                             &&

                             sdsr_list_is_same( temp_lst,
                                                temp2_lst,
                                                       SDSR_EQUAL_BAND )
                             &&

                             sdsr_list_is_same( temp_lst,
                                                temp2_lst,
                                                       SDSR_EQUAL_CHAN )
                            );
    }

    /* When CDMA Service is acquired, update the collocated list used by HDR */
    if( mode == SD_MODE_CDMA )
    {
      sdss_update_hdr_realign_colloc_lst(TRUE);
      sdsr_list_cpy( SDSR_HDR_SRV_COLLOC_LST, colloc_lst );
      sdsr_list_cpy( SDSR_PREV_COLLOC_LST, colloc_lst );
      sdss_update_hdr_realign_colloc_lst(FALSE);
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if HDR instance needs to be realigned when the service status
  ** hasn't changed.
  */

  /* If we are moving to no service, no need to set re align hdr
  ** If the collocated list didn't change, then no need to realign HDR
  ** ...
  */
  if ( ( si_info_ptr->srv_status == srv_status ||
         ( sys_srv_status_is_srv(srv_status) &&
           ( is_assn_tag_changed || is_colloc_changed  ||
             is_geo_changed || is_geo_count_changed
           )
         )
       )
       &&
       (! sd_is_same_sid( si_info_ptr->mode, &prev_sid, sr_ptr)
          ||
          sd_map_band_to_band_pref(si_info_ptr->band) != sr_ptr->sys.band
          ||
          si_info_ptr->is_colloc != SDSR_PRL_ASSN_GET( sr_ptr->prl_assn,
                                                    SDSR_PRL_ASSN_TAG
                                                   )
       )
     )
  {
    realign_hdr     = TRUE;
  }
  else
  {
    realign_hdr     = FALSE;
  }

  /* If collocated HDR needs to be acquired, set the realign flag
  */

  if ( mode == SD_MODE_CDMA &&
       sys_srv_status_is_srv(srv_status) &&
       is_colloc_changed
      )
  {
    SD_MSG_HIGH_0("set realign");
    realign_hdr     = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the current system is GWL (or) HDR
  ** - Set the re-align flag to FALSE.
  ** - Clear SDSR_PREV_COLLOC_LST as GWL systems doesnot have HDR colloc systems.
  */
  if(mode == SD_MODE_GSM || mode == SD_MODE_WCDMA || mode == SD_MODE_GW ||
     mode == SD_MODE_LTE || mode == SD_MODE_GWL || mode == SD_MODE_HDR)
  {
    realign_hdr = FALSE;
  }

  #endif /* FEATURE_HDR_HYBRID */

  /* When ALT CDMA BSR is enabled, check sid/nid/bid change then set the
  ** realign flag to TRUE */
  if(sdss_is_c2k_alt_algr_operation() &&
     sdss_is_1x_sxlte())
  {
    if(mode == SD_MODE_CDMA
      &&  sys_srv_status_is_srv(srv_status)
      && (!sd_is_same_sid(si_info_ptr->mode, &prev_sid, sr_ptr) ||
         (mode_info_ptr!=NULL &&
         si_info_ptr->mode_info.cdma.base_id != mode_info_ptr->cdma.base_id))
      )
    {
      realign_lte = TRUE;
    }
  }

  if( si_info_ptr->mode == SD_MODE_AMPS || si_info_ptr->mode == SD_MODE_CDMA )
  {
    if( si_info_ptr->sid.is95.mcc != sr_ptr->sid.is95.mcc ||
        si_info_ptr->sid.is95.imsi_11_12 != sr_ptr->sid.is95.imsi_11_12 )
    {
      is_mcc_imsi_11_12_changed = TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* REG_DOMAIN is only valid when UE is in CAMPED_ONLY mode. Otherwise it
  ** should have the value NOT_APPLICABLE.
  */
  if( sdss_get_camp_mode() != SD_SS_PREF_CAMP_MODE_ON   &&
      reg_domain != SYS_REG_DOMAIN_NOT_APPLICABLE )
  {
    SD_MSG_HIGH_1("SGLTE: Invalid reg_domain %d changing to NOT_APPLICABLE",
                                                              reg_domain);
    reg_domain = SYS_REG_DOMAIN_NOT_APPLICABLE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If none of the service indicators has changed, just return.
  */
  if( si_info_ptr->srv_status           == srv_status &&
      si_info_ptr->cs_srv_status        == cs_srv_status &&
      #if defined(FEATURE_LTE)
      ((sdss_get_lte_cs_cap()      == lte_cs_capability) ||
	  !(mode == SD_MODE_LTE || mode == SD_MODE_GWL) )&&
      si_info_ptr->lte_ims_voice_avail  == lte_ims_voice_avail &&
      si_info_ptr->lte_ims_emerg_avail  == lte_ims_emerg_avail &&
      #endif
      #if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
      si_info_ptr->sib8_available    == sib8_available &&
      #endif
      si_info_ptr->extend_srv_info   == extend_srv_info &&
      si_info_ptr->is_pref_srv_acq   == is_pref_srv_acq &&
      si_info_ptr->is_stable_in_svc  == is_stable_in_svc &&
      si_info_ptr->is_redir_or_ho    == is_redir_or_ho &&
      si_info_ptr->mode              == mode &&
      si_info_ptr->band              == band &&
      si_info_ptr->chan              == chan &&
      is_mcc_imsi_11_12_changed   != TRUE &&
      si_info_ptr->prl_desig         == sr_ptr->prl &&
      si_info_ptr->hdr_active_prot   == hdr_active_prot &&
      si_info_ptr->hdr_personality   == hdr_personality &&
      si_info_ptr->is_sys_prl_match  == sdss_sr_is_prl( sr_ptr,
                                                     SDSS_PRL_VAL_ANY_PREF
                                                   ) &&
      si_info_ptr->is_sys_forbidden  == sdss_sr_is_prl( sr_ptr,
                                                     SDSS_PRL_VAL_FORBID
                                                   ) &&
      si_info_ptr->srv_domain        == srv_domain &&
      si_info_ptr->sys_srv_cap       == (sys_srv_capability_e_type)sr_ptr->srv_cap &&
      sd_is_same_sid( si_info_ptr->mode, &prev_sid, sr_ptr) &&
      #if  defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
      sd_is_same_gw_lac( si_info_ptr->mode, &prev_sid, sr_ptr ) &&
      #endif

      si_info_ptr->roam              == (sd_roam_e_type)sr_ptr->roam
                              &&
      ( mode_info_ptr == NULL ||
        mode != SD_MODE_CDMA ||
        ( si_info_ptr->mode_info.cdma.base_id   == mode_info_ptr->cdma.base_id &&
          si_info_ptr->mode_info.cdma.base_lat  == mode_info_ptr->cdma.base_lat &&
          si_info_ptr->mode_info.cdma.base_long == mode_info_ptr->cdma.base_long &&
          si_info_ptr->mode_info.cdma.height == mode_info_ptr->cdma.height &&
          si_info_ptr->mode_info.cdma.loc_unc_h == mode_info_ptr->cdma.loc_unc_h &&
          si_info_ptr->mode_info.cdma.loc_unc_v == mode_info_ptr->cdma.loc_unc_v
        )
      )
                              &&
      ( mode_info_ptr == NULL ||
       (mode != SD_MODE_GSM && mode != SD_MODE_WCDMA &&
        mode != SD_MODE_LTE && mode != SD_MODE_TDS)||
       ( si_info_ptr->mode_info.gwl.lac == mode_info_ptr->gwl.lac &&
         si_info_ptr->mode_info.gwl.tac == mode_info_ptr->gwl.tac &&
         si_info_ptr->mode_info.gwl.rac_or_mme_code == \
         mode_info_ptr->gwl.rac_or_mme_code
       )
      )
                              &&
	( mode_info_ptr == NULL ||
        mode != SD_MODE_HDR ||
       ( si_info_ptr->mode_info.hdr.latitude == mode_info_ptr->hdr.latitude &&
         si_info_ptr->mode_info.hdr.longitude == mode_info_ptr->hdr.longitude &&
         si_info_ptr->mode_info.hdr.hdr_system_type == mode_info_ptr->hdr.hdr_system_type &&
         si_info_ptr->mode_info.hdr.hdr_mcc == mode_info_ptr->hdr.hdr_mcc 
       )
      )                     
                              &&
      si_info_ptr->def_roam_ind      == prl_hdr_get_ptr()->def_roam_ind
                                                          &&
      si_info_ptr->reg_domain           == reg_domain
      #ifdef FEATURE_HDR_HYBRID
      /* Finally, if realign_hdr flag is set then dont ignore. Otherwise ignore.
      */
      && !realign_hdr
      #endif
      /* Check if realign_lte flag is set then don't ignore */
      && (!sdss_is_1x_sxlte() || !realign_lte) 
      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
                              &&
      /* Either the csg_info_ptr is null or if not null then check other values */
      ((NULL != csg_info_ptr                                               &&
      si_info_ptr->csg_info.csg_id == csg_info_ptr->csg_id                      &&
      si_info_ptr->csg_info.hnb_name.length == csg_info_ptr->hnb_name.length    &&
      (0 == strcmp((char*)si_info_ptr->csg_info.hnb_name.name, (char*)csg_info_ptr->hnb_name.name)))
                                                                           ||
      (NULL == csg_info_ptr))
      #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) */
    )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, one or more of the service indicators has changed.
  */

  /* Update SI_SYS per the input sr_ref.
  */
  sdsr_list_put_ref( si_ptr->si_sys, sr_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If Hybrid HDR is in operation, and we have to realign-hdr
  ** - The GEO, PREF info would be stale, if we decide to stay on
  **   the current HDR system and geo match is different
  ** - So, Set indication on HYBR1 stack to reload the GEO_LST and PREF_LST from
  **   the prl based on the HDR_SRV_SYS
  */
  if( sd_misc_is_hybr_opr() &&
      realign_hdr &&
      (sd_get_ss_state(SD_SS_HYBR_HDR) == SDSS_STATE_OPR)
    )
  {
    sdss_update_hdr_opr_realign_pref_lst(TRUE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*             U P D A T E   S E R V I C E     I N D I C A T I O N       */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the service indicators per the system record that is pointed by
  ** the sr_ptr parameter.
  */
  si_info_ptr->srv_status        = srv_status;
  si_info_ptr->is_pref_srv_acq   = is_pref_srv_acq;
  si_info_ptr->is_redir_or_ho    = is_redir_or_ho;
  si_info_ptr->is_stable_in_svc  = is_stable_in_svc;

  /* Set the CS service status
  */
  si_info_ptr->cs_srv_status     = cs_srv_status;

   /* Updating LTE CS Capability
   1)If lte_cs_cap sent from camped indication or 
	  plmn success from NAS, check for the global
	  lte cs cap and udpate it per stack

   2)if the received value is not NONE, update
	    the global and per stack 
  */
  if((mode == SD_MODE_LTE || mode == SD_MODE_GWL) )
  {
    if( lte_cs_capability == SYS_LTE_CS_CAPABILITY_NONE )
    {  
       si_info_ptr->lte_cs_capability = sdss_get_lte_cs_cap();
    }
    else 
    {
      sdss_set_lte_cs_cap( lte_cs_capability);
	  si_info_ptr->lte_cs_capability = sdss_get_lte_cs_cap();
    }
  }
  
  si_info_ptr->sib8_available    = sib8_available;
  si_info_ptr->lte_ims_voice_avail = lte_ims_voice_avail;
  si_info_ptr->lte_ims_emerg_avail = lte_ims_emerg_avail;
  si_info_ptr->extend_srv_info = extend_srv_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If mode is changed to CDMA from any other or mode is not CDMA.;
  ** initialize the mode specific info.
  */
  if ( mode == SD_MODE_CDMA &&
       si_info_ptr->mode != SD_MODE_CDMA
     )
  {
    si_info_ptr->mode_info.cdma.base_id    = 0;
    si_info_ptr->mode_info.cdma.base_lat   = 0;
    si_info_ptr->mode_info.cdma.base_long  = 0;
    si_info_ptr->mode_info.cdma.srch_win_n = 0;
    si_info_ptr->mode_info.cdma.reg_prd    = 0;
    si_info_ptr->mode_info.cdma.height     = 0;
    si_info_ptr->mode_info.cdma.loc_unc_h  = 0;
    si_info_ptr->mode_info.cdma.loc_unc_v  = 0;
  }

  
  /* If mode is changed to HDR from any other mode
  ** initialize the mode specific info.
  */
  if ( mode == SD_MODE_HDR &&
       si_info_ptr->mode != SD_MODE_HDR
     )
  {
    si_info_ptr->mode_info.hdr.latitude = 0;
    si_info_ptr->mode_info.hdr.longitude = 0;
    si_info_ptr->mode_info.hdr.hdr_system_type = 0;
    si_info_ptr->mode_info.hdr.hdr_mcc = 0;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* When system acquire, it only acquire a specific band.
  */
  si_info_ptr->mode  = mode;
  si_info_ptr->band  = band;
  si_info_ptr->chan  = chan;

  /* if its WLAN mode, get it from sid_expected else get it from sid.
  */
  switch (si_info_ptr->mode)
  {
    case SYS_SYS_MODE_CDMA:
    case SYS_SYS_MODE_AMPS:
      memscpy( &si_info_ptr->sid.is95,
               sizeof( si_info_ptr->sid.is95 ),
               &sr_ptr->sid.is95,
               sizeof( sr_ptr->sid.is95 ) );
      break;

    case SYS_SYS_MODE_HDR:
      memscpy( si_info_ptr->sid.is856.sector_id,
               sizeof( si_info_ptr->sid.is856.sector_id ),
               sr_ptr->sid.is856.sector_id,
               sizeof(sr_ptr->sid.is856.sector_id) );
      si_info_ptr->sid.is856.subnet_mask_len = sr_ptr->sid.is856.subnet_mask_len;
      break;

    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_GW:
    case SYS_SYS_MODE_LTE:
    case SYS_SYS_MODE_GWL:
    case SYS_SYS_MODE_TDS:
      si_info_ptr->sid.plmn = sr_ptr->sid.plmn;
      break;

    case SYS_SYS_MODE_GPS:
    case SYS_SYS_MODE_NONE:
    case SYS_SYS_MODE_NO_SRV:
    case SYS_SYS_MODE_MAX:
    default:
      break;

  } /* switch (si_info_ptr->mode) */


  si_info_ptr->roam              = (sd_roam_e_type)sr_ptr->roam;
  si_info_ptr->prl_desig         = sr_ptr->prl;
  si_info_ptr->is_sys_prl_match  = sdss_sr_is_prl( sr_ptr,
                                                SDSS_PRL_VAL_ANY_PREF
                                              );
  si_info_ptr->is_sys_forbidden  = sdss_sr_is_prl( sr_ptr,
                                                SDSS_PRL_VAL_FORBID
                                              );
  si_info_ptr->def_roam_ind      = prl_hdr_get_ptr()->def_roam_ind;

  si_info_ptr->srv_domain        = srv_domain;
  si_info_ptr->sys_srv_cap       = (sys_srv_capability_e_type)sr_ptr->srv_cap;
  si_info_ptr->reg_domain        = reg_domain;
  si_info_ptr->is_colloc         = SDSR_PRL_ASSN_GET( sr_ptr->prl_assn,
                                               SDSR_PRL_ASSN_TAG
                                             );

  si_info_ptr->is_pn_assn        = SDSR_PRL_ASSN_GET( sr_ptr->prl_assn,
                                               SDSR_PRL_ASSN_PN
                                             );

  si_info_ptr->is_data_assn      = SDSR_PRL_ASSN_GET( sr_ptr->prl_assn,
                                               SDSR_PRL_ASSN_DATA
                                             );

  if(si_info_ptr->mode == SD_MODE_CDMA || si_info_ptr->mode == SD_MODE_HDR)
  {
  
    if(si_info_ptr->roam != SYS_ROAM_STATUS_OFF &&
       sdprl_roam_ind_map_custom_home_to_home((sys_roam_status_e_type)si_info_ptr->roam) == SYS_ROAM_STATUS_OFF)
    {
      si_info_ptr->is_custom_home  = TRUE;
    }
    else
    {
      si_info_ptr->is_custom_home  = FALSE;	  
    }
  }
  else
  {
    si_info_ptr->is_custom_home  = FALSE;	    
  }

  if(curr_sys_info != NULL)
  {
    si_info_ptr->geo_sys_idx     = curr_sys_info->geo_sys_idx;
  }

  #ifdef FEATURE_HDR_HYBRID
  si_info_ptr->realign_hdr       = realign_hdr;
  #else
  si_info_ptr->realign_hdr       = FALSE;
  #endif

  if(sdss_is_1x_sxlte())
  {
  si_info_ptr->realign_lte = realign_lte;
  }
  else
  {
  si_info_ptr->realign_lte = FALSE;
  }

  if ( mode_info_ptr != NULL )
  {
    si_info_ptr->mode_info       = *mode_info_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset BCMCS service status if mode is a protocol other than HDR.  We
  ** don't want to turn off BCMCS service just because we lost HDR serivce,
  ** because if we do so, then the DS flows will be torn down unnecessarily.
  ** Simply pass through the BCMCS serivce status to DS via CM and let DS
  ** make all the decisions.
  */
  if ( si_info_ptr->mode != SD_MODE_HDR && si_info_ptr->mode != SD_MODE_INACT )
  {
    si_info_ptr->bcmcs_srv_status = SYS_BCMCS_SRV_STATUS_NO_SRV;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SS-Core srv_status/srv_domain.
  */
  sdss_update_srv_status( ss, srv_status, srv_domain );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If main system selection, Update DB items.
  */
  if( ss == SD_SS_MAIN )
  {
    #ifdef SD_USES_DB_SERVICES
    db_item.no_svc    = FALSE;
    db_put( DB_NO_SVC, &db_item );

    db_item.digital   = (si_info_ptr->mode == SD_MODE_AMPS) ? FALSE : TRUE;
    db_put( DB_DIGITAL, &db_item );

    db_item.roam      = (db_roam_type) si_info_ptr->roam;
    db_put( DB_ROAM, &db_item );
    #endif

    if( si_info_ptr->mode == SD_MODE_HDR )
    {
      /* Printout HDR Sector ID.
      */
      sdsr_hdr_sid_print( si_info_ptr->sid.is856.sector_id );
    }
    else
    {
      #ifdef SD_USES_DB_SERVICES
      db_item.sid       = si_info_ptr->sid.is95.sid;
      db_put( DB_SID, &db_item );

      db_item.nid       = si_info_ptr->sid.is95.nid;
      db_put( DB_NID, &db_item );
      #endif
    }

    /* Update the srv uptime if applicable
    */

    if(!sys_srv_status_is_srv(srv_status))
    {
      si_info_ptr->srv_acq_time = 0;

      if( si_info_ptr->srv_lost_time == 0 )
      {
        si_info_ptr->srv_lost_time = time_get_uptime_secs();
      }
    }
    else
    {
      if( si_info_ptr->srv_acq_time == 0 )
      {
        si_info_ptr->srv_acq_time = time_get_uptime_secs();
      }
      si_info_ptr->srv_lost_time = 0;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the HDR active protocol revision
  */
  if( ( hdr_active_prot > SYS_ACTIVE_PROT_HDR_START &&
        hdr_active_prot < SYS_ACTIVE_PROT_HDR_END ) ||
        (hdr_active_prot == SYS_ACTIVE_PROT_NONE ) )
  {
    si_info_ptr->hdr_active_prot = hdr_active_prot;
  }
  /* Update the HDR personality
  */
  if( ( hdr_personality  > SYS_PERSONALITY_HDR_START &&
        hdr_personality  < SYS_PERSONALITY_HDR_END ) ||
        (hdr_personality == SYS_PERSONALITY_NONE ) )
  {
    si_info_ptr->hdr_personality = hdr_personality;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update eHRPD Zone
  */

  sdss_set_eHRPD_zone(si_info_ptr, ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)

  /* Copy the CSG related data */
  if(NULL != csg_info_ptr)
  {
    si_info_ptr->csg_info = *csg_info_ptr;
  }
  else
  {
    memset(&si_info_ptr->csg_info.hnb_name, 0, sizeof(sys_home_nodeb_name_type));
    si_info_ptr->csg_info.csg_id = SYS_CSG_ID_INVALID;
  }

  #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //For failure cases - if req id is not zero, then only update or else don't
  if(0 != req_id)
  {
    si_info_ptr->sys_sel_pref_req_id = req_id;
  }
  else
  {
    //This will be set again in sd_si_info_ptr_set() below.
    si_info_ptr->sys_sel_pref_req_id = sd_si_ptr(ss)->si_info.sys_sel_pref_req_id;
  }

  /* Update si_info with CDMA/HDR plmn list */
  if((si_info_ptr->mode == SYS_SYS_MODE_CDMA || si_info_ptr->mode == SYS_SYS_MODE_HDR) &&
     mcc_list_info->count != 0 )
  {
    si_info_ptr->plmn_mcc_list.count = MIN(mcc_list_info->count,SD_PLMN_MCC_LIST_MAX);
    
    while(count < si_info_ptr->plmn_mcc_list.count)
    {
      si_info_ptr->plmn_mcc_list.mcc_list[count] = mcc_list_info->mcc_list[count];      
      count++;
    }    
  }
  else
  {
    /* Reset the mcc list for cdma/hdr stacks when the count is 0 and also for 3gpp stacks 
       - meaning we lost service, we go LPM and etc */    
    memset(&si_info_ptr->plmn_mcc_list, 0, sizeof(sys_plmn_mcc_list));
  }
  {
    sd_ss_mode_pref_e_type    user_mode_pref = SD_SS_MODE_PREF_NONE;
    sys_lte_band_mask_e_type  ss_lte_band_pref;

    sdss_get_user_mode_pref(ss,
		             &user_mode_pref,
                             NULL,
                             &ss_lte_band_pref,
                             NULL );
	
	/* 
	** During IRAT dont reset the lte_scanned flag so that
	** after acquiring 1x on MAIN, LTE acquisition is not started immediately
	** after IRAT is done lte_scanned flag will be reset
    */
    if( ss == SD_SS_MAIN 
        && (!sdss_is_supp_mode_band_pref_lte(user_mode_pref,ss_lte_band_pref))
		&& (!sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_HYBR_HDR_IRAT) ))
    {
      si_info_ptr->is_lte_scanned = FALSE;
      SD_MSG_HIGH_0("is_lte_scanned set to FALSE");
    }

    
  }  

  /* Inform the registered client (e.g., CM) of the new service indicators.
  */
  SD_MSG_HIGH_6( "sd_si_update | mode=%d,band=%d,roam=%d realign_hdr=%d,is_colloc=%d, prot=%d",
                sr_ptr->sys.mode, band, sr_ptr->roam, si_info_ptr->realign_hdr, si_info_ptr->is_colloc,
                si_info_ptr->hdr_active_prot );

  #if defined (FEATURE_LTE) 
  SD_MSG_HIGH_4( "sib8_available =%d,lte_cs_capability =%d, ims_voice_avail=%d, emerg %d",
                si_info_ptr->sib8_available, si_info_ptr->lte_cs_capability,
               si_info_ptr->lte_ims_voice_avail,si_info_ptr->lte_ims_emerg_avail);
  #endif

  SD_MSG_HIGH_6( "req_id=%u, sd_si_ptr->req_id=%u, extend srv info=%d, is_custom_home=%d, is_lte_scanned=%d, realign_lte=%d", 
                 req_id, sd_si_ptr(ss)->si_info.sys_sel_pref_req_id,
                 si_info_ptr->extend_srv_info, si_info_ptr->is_custom_home, 
                 si_info_ptr->is_lte_scanned, si_info_ptr->realign_lte);
  
  SD_MSG_HIGH_5( "CDMA base stn parms height=%d,loc_unc_h=%d,loc_unc_v=%d ; HDR base stn parms latitude=%d,longitude=%d",
                si_info_ptr->mode_info.cdma.height, si_info_ptr->mode_info.cdma.loc_unc_h,
                si_info_ptr->mode_info.cdma.loc_unc_v, si_info_ptr->mode_info.hdr.latitude, 
                si_info_ptr->mode_info.hdr.longitude);

  SD_MSG_HIGH_3( "Srv change,HDR base station parms latitude=%d,longitude=%d,extend_srv_info=%d",
                si_info_ptr->mode_info.hdr.latitude, si_info_ptr->mode_info.hdr.longitude,
                si_info_ptr->extend_srv_info);

  if(!is_silent)
  {
    sd_invoke_info_callback(ss);

    /* Reset the sys sel pref req id */
    si_info_ptr->sys_sel_pref_req_id = 0;
    si_ptr->si_info.sys_sel_pref_req_id = 0;

    
    if((si_info_ptr->extend_srv_info == SYS_EXTEND_SRV_INFO_REG_COMP_ACK_PENDING) &&
       (si_info_ptr->is_stable_in_svc == TRUE))
    {
      /* This info should only be used for the current SRV_IND/CNF. So reset the info, to avoid satrting un necessary delay timer */
      si_info_ptr->extend_srv_info = SYS_EXTEND_SRV_INFO_NONE;
    }
    
  }


  #ifndef FEATURE_HDR_HYBRID
  SYS_ARG_NOT_USED(colloc_lst);
  SYS_ARG_NOT_USED(curr_sys_info);
  #endif /* FEATURE_HDR_HYBRID */
} /* sd_si_update *//*lint +esym(550, is_uz_check) */ /* para not accessed */


/*===========================================================================

FUNCTION sd_ss_idle_sys_update

DESCRIPTION
  Update the idle system, its GEO list and PREF lists per the input lists
  as necessary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                          sd_ss_idle_sys_update(

        sd_ss_e_type                  ss,
            /* The ss for which the idle system is to be updated */

        sdsr_e_type                   sys,
            /* System per which to update the idle system.
            ** Ignored if set to SDSR_MAX */

        sdsr_e_type                   geo_list,
            /* List per which to update the GEO list  */

        sdsr_e_type                   pref_list
            /* List per which to update the PREF list */
)
{
  sdsr_ref_type     sr_ref;
  sdsr_s_type       *sys_ptr;
  boolean           is_update_idle_sys  = FALSE;
  boolean           is_gw_sys           = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( BETWEEN(sys, SDSR_1ST_SYS, SDSR_MAX) || sys == SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( geo_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE( pref_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sys != SDSR_MAX )
  {
    sys_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss, sys), 0 );
  }
  else
  {
    sys_ptr = NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Determine whether we need to update the idle system.
  */

  /* If handoff designation is none (i.e., MS is not engaged in a call or
  ** call setup activity), update the idle system.
  */
  if( sdss_is_handoff(ss,SDSS_HO_VAL_NONE) )
  {
    is_update_idle_sys = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else if handoff is CAM to CDMA paging, the system is allowed by the PRL,
  ** complies with the current SS-Preference, and explicitly listed in the
  ** PRL's acquisition, update the idle system.
  */
  else if( sdss_is_handoff(ss, SDSS_HO_VAL_CDMA_CAM_TO_CDMA_PAG) &&
           sys_ptr != NULL &&
           sdss_sr_is_prl(sys_ptr, SDSS_PRL_VAL_ALLOWED) &&
           sdss_sr_is_pref_conf(ss, sys_ptr, SDSS_PREF_CONF_NONE) &&
           sdsr_list_find(SDSR_PRL_LST, sys_ptr) != SDSR_POS_NULL )
  {
    is_update_idle_sys = TRUE;
  }

  /* If UE gets full srv on LTE during an emergency call but this is not the stack on which LTE is normally
  ** camped on ( outside of the emergency call) , do not update idl sys / mru 
  */ 
  if (sys_ptr != NULL &&
      sys_ptr->sys.mode == SD_MODE_GWL &&
      sys_ptr->sid.gw.mode == SD_MODE_LTE &&
      sdss_is_emergency_call_orig(ss) &&
      sdss_get_lte_idle_ss() != ss ) 
  {
    is_update_idle_sys = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If need to update the idle system, update the following system record
  ** lists:
  **
  ** 1. The SDSR_IDL_SYS.
  ** 2. The SDSR_IDL_GEO_LST.
  ** 3. The SDSR_IDL_REF_LST.
  ** 4. The SDSR_MRU_LST.
  */
  if( is_update_idle_sys && sys_ptr != NULL )
  {
    sys_srv_status_e_type ss_srv_status;

    sdsr_list_cpy( sdss_sr_list_map2(ss, SDSR_IDL_GEO_LST ),
                   sdss_sr_list_map2(ss, geo_list) );

    sdsr_list_cpy( sdss_sr_list_map2(ss, SDSR_IDL_PREF_LST),
                   sdss_sr_list_map2(ss, pref_list) );

    SD_MSG_HIGH_0("Updating SDSR_IDL_GEO_LST with the Acquired Network");
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If sys is not to be ignored, do the following:
    */
    if( sys != SDSR_MAX )
    {
      /* Update the idle system per sys.
      */
      sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_IDL_SYS ),
                     sdss_sr_list_map2( ss, sys ) );

      /* Maintain a copy of IDL system so that:
      ** 1. We use correct sys for HDR collocated in CDMA->GWL->HDR scenario
      ** 2. MSM CDMA srv ind doesn't overwrite GWL IDL sys for HYBR_BSR
      */
      if(sys_ptr->sys.mode == SYS_SYS_MODE_CDMA )
      {
        SD_MSG_MED_0("SDSR_CDMA_IDL_SYS is updated");
        sdsr_list_cpy( SDSR_CDMA_IDL_SYS , sdss_sr_list_map2( ss, sys ) );
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Insert system to beginning of MRU list (remove duplicates).
      */
      sr_ref = sdsr_list_get_ref( sdss_sr_list_map2( ss, sys ), 0 );
      SD_ASSERT( sr_ref != SDSR_REF_NULL )

      /* Update the last user system that contains the same bands as that in
      ** in the idle system.
      */
      sdsr_list_update_post_gwl_sys( ss, sr_ref, TRUE, TRUE, TRUE );

      /* Change the system to be logged if the system is a GW system.
      */
      if( sys_ptr->sys.mode == SD_MODE_GSM   ||
          sys_ptr->sys.mode == SD_MODE_WCDMA ||
          sys_ptr->sys.mode == SD_MODE_GW    ||
          sys_ptr->sys.mode == SD_MODE_LTE   ||
          sys_ptr->sys.mode == SD_MODE_TDS   ||
          sys_ptr->sys.mode == SD_MODE_GWL   )
      {
        sr_ref = sdsr_list_get_ref( sdss_sr_list_map2(ss, SDSR_GWL_LAST_FULL_SYS ), 0 );
        is_gw_sys = TRUE;
      }

      sdss_read_srv_status( ss, &ss_srv_status, NULL );

      /* If SD sent a service request with net_sel_mode- LIMITED_ROUTINE, received 
      ** full service first and limited service later, SD should send a new service
      ** request immediately. So updating the acq_sys as per the last_gwl_full_sys 
      ** while full service is received(Acq_sys net_sel_mode will be updated to Auto)
      */
      if( is_gw_sys &&  (ss_srv_status == SYS_SRV_STATUS_SRV) &&
          sr_ref != SDSR_REF_NULL )
      {
        sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_ACQ_SYS ),
                       sdss_sr_list_map2(ss, SDSR_GWL_LAST_FULL_SYS ) );
      }
      /* During emergency origination/callback, only insert system to
      ** beginning MRU if the system is found on the MRU, PRL or REDIR list.
      **
      ** Do not update MRU if origination mode is manul mode.
      ** Do not update MRU if acquired system is a GW system and doesn't
      ** provide full service.
      */
      if( (! sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_EMERG) ||
           sdsr_list_find_ref(sdss_sr_list_map2(ss, SDSR_MRU_LST ), sr_ref) != SDSR_POS_NULL ||
           sdsr_list_find_ref(SDSR_PRL_LST, sr_ref) != SDSR_POS_NULL ||
           sdsr_list_find_ref(sdss_sr_list_map2(ss, SDSR_REDIR_LST ), sr_ref) != SDSR_POS_NULL)
           #ifndef FEATURE_SD_UPDATE_MRU_IN_MANUAL_MODE
                           &&
           ! sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_MANUAL)
           #endif
                           &&
          ( !is_gw_sys ||
            ss_srv_status == SYS_SRV_STATUS_SRV )


        )
      {
        #ifdef FEATURE_JCDMA
        /* If JCDMA is defined, only insert to the MRU systems that are
        ** explicitly listed in the PRL.
        */
        if( sdsr_list_find_ref(SDSR_PRL_LST, sr_ref) != SDSR_POS_NULL ||
            sys_ptr->sys.mode == SD_MODE_GSM                          ||
            sys_ptr->sys.mode == SD_MODE_WCDMA                        ||
            sys_ptr->sys.mode == SD_MODE_GW                           ||
            sys_ptr->sys.mode == SD_MODE_LTE                          ||
            sys_ptr->sys.mode == SD_MODE_TDS                          ||
            sys_ptr->sys.mode == SD_MODE_GWL )
        #endif
        {
          SD_MSG_HIGH_0("Updating SDSR_MRU_LST with the Acquired Network");
          sdsr_list_insert_ref( sdss_sr_list_map2(ss, SDSR_MRU_LST ), 0, sr_ref, TRUE );
          sdsr_list_print(sdss_sr_list_map2(ss, SDSR_MRU_LST ),FALSE);
        }
      }

    } /* if( sys != SDSR_MAX ) */

  } /* if( not in handoff ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the idle AMPS backstop system.
  */
  sd_ss_idle_backstop_sys_update();

} /* sd_ss_idle_sys_update */


/*===========================================================================

FUNCTION sd_si_is_srv_on

DESCRIPTION
  Indicate whether the service indicator is currently ON.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the service indicator is currently ON. FALSE otherwise.

  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean                   sd_si_is_srv_on(

        sd_ss_e_type              ss
        /* System selection - MAIN or HDR */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if service status indicates NO_SRV.
  */
  return sys_srv_status_is_srv( sd_si_ptr(ss)->si_info.srv_status );

} /* sd_si_is_srv_on */


/*===========================================================================

FUNCTION sd_ss_srv_sys_update

DESCRIPTION
  Update the serving system, its GEO, and its more preferred lists per the
  input lists.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                          sd_ss_srv_sys_update(

        sd_ss_e_type                  ss,
            /* The ss for which the srv_sys is to be updated */

        sdsr_e_type                   sys,
            /* System per which to update the serving system
            ** Ignored if set to SDSR_MAX  */

        sdsr_e_type                   geo_list,
            /* List per which to update the serving system's GEO */

        sdsr_e_type                   pref_list,
            /* List per which to update the serving system's more preferred
            ** list */

        sdsr_e_type                   colloc_list
            /* List per which to update the serving system's collocated list */
)
{
  SD_ASSERT( BETWEEN(sys, SDSR_1ST_SYS, SDSR_MAX) || sys == SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( geo_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE( pref_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_list, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If needed, update the serving system.
  */
  if( sys != SDSR_MAX )
  {
    sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_SRV_SYS ),
                   sdss_sr_list_map2( ss, sys) );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the serving system's, GEO and more preferred lists.
  */
  sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_SRV_GEO_LST),
                 sdss_sr_list_map2( ss, geo_list ) );

  sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_SRV_PREF_LST),
                 sdss_sr_list_map2( ss, pref_list ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If needed, update the serving systems collocated list.
  */
  if( colloc_list != SDSR_MAX )
  {
    sdsr_list_cpy( sdss_sr_list_map2( ss, SDSR_SRV_COLLOC_LST),
                   sdss_sr_list_map2( ss, colloc_list) );
  }

} /* sd_ss_srv_sys_update */


/*===========================================================================

FUNCTION sd_si_update_if_srv_is_off

DESCRIPTION
  Set the service indicator to TRUE and update the system indicators per
  the input system record iff service is currently OFF.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                      sd_si_update_if_srv_is_off(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR
            */

        sd_si_update_e_type       update_criteria,
            /* Update criteria.
            */

        sys_srv_status_e_type     srv_status,
            /* Current service status ( LIMITED, FULL service etc ).
            */

        sys_srv_domain_e_type     srv_domain,
            /* Service domain.
            */


        sdsr_ref_type             sr_ref,
            /* System record reference according to which to update the
            ** system indicators.
            */

        const sd_mode_e_type      *mode_ptr,
           /* Pointer to mode - ignored if NULL.
           */

        const sd_band_e_type      *band_ptr,
            /* Pointer to band - ignored if NULL.
            */

        const sd_chan_type        *chan_ptr,
             /* Pointer to channel - ignored if NULL.
             */

        sdsr_e_type               colloc_list,
            /* collocated list.
            */

        const sys_active_prot_e_type *hdr_active_prot_ptr,
            /* HDR active protocol type.
            */

        const sys_personality_e_type    *hdr_personality_ptr,
            /* The HDR active personality
            */

        const sdss_sys_addn_info_s_type *curr_sys_info,
            /* Association tag of the currently acquired
            ** system, if any.
            */

        sys_srv_status_e_type     cs_srv_status,
            /* Current CS service status.
            */

        sd_sib8_status                   sib8_available,
        /** <sib8 status> */

        sys_extend_srv_info_e_type        extend_srv_info,
            /* Extended service info
            */

        sys_reg_domain_e_type           reg_domain,
            /* The REG domain value is used in CAMPED_ONLY mode by the
            ** CM client to determine service availability.
            */

        sd_si_mode_info_u_type    *mode_info_ptr
          /* Mode specific information
          */

)
{

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( update_criteria, SD_SI_UPDATE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If service is currently not full srv, call on sd_si_update() to update the
  ** service indicators.
  */
  if( !sd_si_is_full_srv_on( ss ) )
  {
    sd_si_update( ss,
                  update_criteria,
                  srv_status,
                  srv_domain,
                  sr_ref,
                  FALSE,
                  mode_ptr,
                  band_ptr,
                  chan_ptr,
                  mode_info_ptr,
                  colloc_list,
                  hdr_active_prot_ptr,
                  hdr_personality_ptr,
                  curr_sys_info,
                  cs_srv_status,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  sib8_available,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0,  /* Request Id not needed */
                  extend_srv_info,
                  reg_domain,
                  NULL,
                  FALSE
                );
  }

} /* sd_si_update_if_srv_is_off */


/*===========================================================================

FUNCTION sd_misc_map_sd_mode_to_sys_mode

DESCRIPTION
  Map SD mode to SYS mode

DEPENDENCIES
  None

RETURN VALUE
  Enumeration of system modes

SIDE EFFECTS
  None.

===========================================================================*/

sys_sys_mode_e_type       sd_misc_map_sd_mode_to_sys_mode(

        sd_mode_e_type            sd_mode
        /* SD mode */
)
{
  sys_sys_mode_e_type sys_mode = SYS_SYS_MODE_CDMA;
  sd_ss_mode_pref_e_type mode_capability = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type band_capability = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type band_capability_lte;
  sd_ss_band_pref_e_type band_capability_tds = SD_SS_BAND_PREF_NONE;


  switch ( sd_mode )
  {
    case SD_MODE_CDMA:
      sys_mode =  SYS_SYS_MODE_CDMA;
      break;

    case SD_MODE_AMPS:
      sys_mode =  SYS_SYS_MODE_AMPS;
      break;

    case SD_MODE_HDR:
      sys_mode =  SYS_SYS_MODE_HDR;
      break;

    case SD_MODE_GSM:
    case SD_MODE_GW:
      sys_mode =  SYS_SYS_MODE_GSM;
      break;

    case SD_MODE_WCDMA:
      sys_mode =  SYS_SYS_MODE_WCDMA;
      break;

    case SD_MODE_GWL:
      sdss_get_mode_band_capability_mmode(&mode_capability,
                                          &band_capability,
                                          &band_capability_lte,
                                          &band_capability_tds);
      if (mode_capability == SD_SS_MODE_PREF_LTE)
      {
        sys_mode = SYS_SYS_MODE_LTE;
      }
      else
      {
        sys_mode = SYS_SYS_MODE_GSM;
      }

      break;

    case SD_MODE_LTE:
      sys_mode = SYS_SYS_MODE_LTE;
      break;

    case SD_MODE_TDS:
      sys_mode = SYS_SYS_MODE_TDS;
      break;

    case SD_MODE_INACT:
      sys_mode = SYS_SYS_MODE_NO_SRV;
      break;

    case SD_MODE_NONE:
    case SD_MODE_GPS:
    case SD_MODE_MAX:
    default:
      SD_ERR_1( "Unsupported SD mode %d for mapping",sd_mode);
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_MED_1( "returned sys_mode=%d",sys_mode);
  return sys_mode;

}/* sd_misc_map_sd_mode_to_sys_mode */


/*===========================================================================

FUNCTION sd_ss_act_get_curr_act_ss

DESCRIPTION
  Return the true SS-Action that was most recently returned to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_act_get_curr_act_ss(

        sd_ss_e_type              ss,

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action */
)
{
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX )

  switch( ss )
  {
    case SD_SS_MAIN:
      return sd_ss_act_get( rtrn_act_ptr );

    case SD_SS_HYBR_1:
      return sd_ss_hybr_1_act_get( rtrn_act_ptr );

    case SD_SS_HYBR_2:
      return sd_ss_hybr_2_act_get( rtrn_act_ptr );

    case SD_SS_HYBR_3:
      return sd_ss_hybr_3_act_get( rtrn_act_ptr );

    default:
      SD_MSG_HIGH_1("SD_SS:FRFL:Invalid ss %d for act_get",ss);
      return SD_SS_ACT_MAX;
  }
}


#ifdef FEATURE_HDR_HYBRID
/*===========================================================================

FUNCTION sd_ss_hdr_event_proc

DESCRIPTION
  Do HDR SS-Event common processing when doing hybrid HDR operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_hdr_event_proc(


        sdss_evt_e_type           event,
            /* SS-Event to process.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  sdsr_s_type           *sr_ptr;
  sdss_iact_s_type      iact;

  sdss_iact_s_type      *iact_ptr             = &iact;
  sd_ss_s_type          *ss_ptr               = sd_ss_ptr(SD_SS_HYBR_HDR);
  sd_ss_act_s_type      ss_act;
  dword                 uptime                = time_get_uptime_secs();
  sdsr_e_type           ss_acq_list   = SDSR_MAX;
  int                   ss_acq_pos    = -1;
  sdss_acq_mode_e_type  ss_acq_mode   = SDSS_ACQ_MODE_MAX;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Lock the HDR ss component before processing it.
  */
  SD_SS_LOCK( ss_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( event, SDSS_EVT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print the SS-Event.
  */
  sdss_event_group_print( event );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is an acquisition event, update the SDSR_HDR_ACQ_SYS as per this
  ** event.
  */
  if( SDSS_EVT_IS_XXX_ACQ(event) )
  {
    sr_ptr = sdsr_list_get_ptr( SDSR_HDR_ACQ_SYS, 0 );
    SD_ASSERT( sr_ptr != NULL );

    /*lint -e774 */
    if( sr_ptr != NULL ) {
      sr_ptr->event       = (word) event;
      sr_ptr->event_time  = uptime;
    }
    /*lint +e774 */

    /* Do post HDR acquisition attempt updates.
    */

    sd_ss_read_acq_prms( SD_SS_HYBR_HDR,
                         &ss_acq_list,
                         &ss_acq_pos,
                         &ss_acq_mode );

    sd_ss_hdr_acq_update_post( ss_acq_list,
                               ss_acq_pos,
                               ss_acq_mode );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, if this is an operation event, update the SDSR_HDR_SRV_SYS and
  ** SDSR_HDR_IDL_SYS as per this event.
  */
  else if( SDSS_EVT_IS_XXX_OPR(event) )
  {
    sr_ptr = sdsr_list_get_ptr( SDSR_HDR_SRV_SYS, 0 );
    SD_ASSERT( sr_ptr != NULL );

    /*lint -e774 */
    if( sr_ptr != NULL ) {
      sr_ptr->event       = (word) event;
      sr_ptr->event_time  = uptime;
    }
    /*lint +e774 */
    sr_ptr = sdsr_list_get_ptr( SDSR_HDR_IDL_SYS, 0 );

    if( sr_ptr != NULL ) {
      sr_ptr->event       = (word) event;
      sr_ptr->event_time  = uptime;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remember the pervious SS-Action.
  */
  (void)sd_ss_hdr_act_get_true_curr_act(&(ss_ptr->prev_act));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to process the SS-Event.
  */
  sdss_hdr_event_proc( event, iact_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------
  ** Do SS-Action independent processing.
  ** ------------------------------------
  */

  (void)sd_ss_hdr_act_get_true_curr_act(&ss_act);

  /* Update the SS-State indicator and inform the registered client (i.e.,
  ** CM) of any preferred service acquisition indicator changes.
  */
  sd_set_ss_state( SD_SS_HYBR_HDR, iact_ptr->ss_state );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If timer expiration time has changed, set the timer per the new
  ** expiration time.
  */
  if( iact_ptr->ss_timer != ss_ptr->ss_timer )
  {
    /* If new expiration time is 0. Disable the timer by setting it to 0.
    */
    if( iact_ptr->ss_timer == 0 )
    {
      SD_MSG_HIGH_0( "SS-Timer:hdr=0");
      ss_ptr->timer_func_ptr( 0 );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (new expiration time != 0), set the timer according to the new
    ** expiration time.
    */
    else
    {
      dword timer;

      if( iact_ptr->ss_timer > uptime ) {
        timer = (iact_ptr->ss_timer - uptime) * 1000;
      }
      else {
        timer = SD_SS_TIMER_INVALID;
      }

      SD_MSG_HIGH_4( "SS-Timer:hdr=%ds or %dms, uptime=%ds, exp=%ds",
                   timer/1000, timer,uptime, iact_ptr->ss_timer );

      ss_ptr->timer_func_ptr( (int4) timer );
    }

    ss_ptr->ss_timer = iact_ptr->ss_timer;

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------
  ** Do SS-Action dependent processing.
  ** ----------------------------------
  */
  switch( iact_ptr->act )
  {
    case SDSS_IACT_CONTINUE:
      ss_act.act = SD_SS_ACT_CONTINUE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_ACQUIRE:

      /* Do pre acquisition attempt updates.
      */
      sd_ss_acq_update_pre( SD_SS_HYBR_HDR,
                            iact_ptr->prm.acq.list,
                            iact_ptr->prm.acq.pos,
                            iact_ptr->prm.acq.mode );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Get the system to acquire and set the returned SS-Action as per
      ** the mode of this system.
      */
      sr_ptr = sdsr_list_get_ptr( iact_ptr->prm.acq.list,
                                  iact_ptr->prm.acq.pos );
      if(sr_ptr == NULL)
      {
        sys_err_fatal_null_ptr_exception();
      }
      SD_ASSERT( sr_ptr != NULL );
      SD_ASSERT( sr_ptr->sys.mode == SD_MODE_HDR );

      /* If sdsr table is full and a new entry has to be added, a least recently used
      ** system is overwritten. This decision is based on the lowest event time of a record 
      ** if there are multiple entries in sdsr_tbl with sys_rec_type, SDSR_MAX_LST.
      ** To avoid overwriting the current acq system in case any sdsr_tbl entry has to be 
      ** added after issuing acquisition and before acq response is received,
      ** update the event time of the system record while acquisition is issued.
      */
      sr_ptr->event		 = (word) event;
      sr_ptr->event_time = uptime;

      if( ! sdss_is_supp_mode_band(sr_ptr->sys.mode,
                                   sr_ptr->sys.band,
                                   sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band),
                                   sr_ptr->sys.tds_band) )
      {
        SD_ERR_FATAL_0( "Unsupported mode/band");
      }

      ss_act.act = SD_SS_ACT_ACQ_HDR;
      ss_act.prm.acq_hdr.band =
        sdss_map_band_pref_to_band(sr_ptr->sys.band);
      ss_act.prm.acq_hdr.chan = sr_ptr->sys.chan;
      ss_act.prm.acq_hdr.acq_mode = sd_ss_map_acq_mode_to_hdr(
                                                    iact_ptr->prm.acq.mode );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_MEAS:

      /* First do the following:
      **
      ** 1. Increment the measurement ID.
      ** 2. Indicate that measurement request is not done being serviced.
      ** 3. Indicate whether the requested list of systems to measure is
      **    different than the previous requested list.
      ** 4. Copy the systems that are being measured into the SDSR_MEAS_LIST.
      */
      ss_ptr->meas_id++;
      ss_ptr->meas_is_done = FALSE;

      if( sdsr_list_is_contain(SDSR_HDR_MEAS_LST,
                               SDSR_CONTAIN_SAME,
                               iact_ptr->prm.meas.list) )
      {
        ss_ptr->meas_list_is_diff = FALSE;
      }
      else
      {
        ss_ptr->meas_list_is_diff = TRUE;
      }
      sdsr_list_cpy( SDSR_HDR_MEAS_LST, iact_ptr->prm.meas.list );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Set the returned SS-Action as per the measurement mode.
      */
      if( iact_ptr->prm.meas.mode == SDSS_MEAS_MODE_DED )
      {
        ss_act.act = SD_SS_ACT_MEAS_DED;

      }
      else
      {
        ss_act.act = SD_SS_ACT_MEAS_BACK;

      }
      ss_act.prm.meas.type  = iact_ptr->prm.meas.type;
      ss_act.prm.meas.id    = ss_ptr->meas_id;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_PWR_SAVE:
      ss_act.act = SD_SS_ACT_PWR_SAVE;

      /*  Populate the pwr save parameters, default is due to no_srv.
      **  check if there are modes available to acquire.
      */
      ss_act.prm.pwr_save.reas =  SD_SS_PWR_SAVE_REAS_NO_SRV;
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_IACT_ACCEPT:
    case SDSS_IACT_REJECT:
    case SDSS_IACT_RELEASE:
    case SDSS_IACT_GET_NET_GWL:
    case SDSS_IACT_MAX:
    default:
      SD_ERR_0( "act");
      break;

  } /* switch( act ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Client supplied a buffer for the return SS-Action, copy the
  ** returned SS-Action into this buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = ss_act;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy contents of act field to true current act data structure if act
  ** enum type is not continue.
  */
  sd_ss_act_set_true_curr_act_ss(SD_SS_HYBR_HDR, &ss_act);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_1( "<<<< Returned Action=%d, ss=1 <<<<",ss_act.act);
  sdss_act_print( &ss_act );
  sdss_iact_print( iact_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_SD20_LOGGING
    /* Send SD log packet.
    */
    sd_ss_log_last_action( SD_SS_HYBR_HDR );
  #endif /* FEATURE_SD20_LOGGING */

  /* Send SD event packet.
  */
  sd_ss_event_rprt( SD_SS_HYBR_HDR, event, &ss_act );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We only notify about the pref_srv_acq when the act is not CONTINUE
  ** or MEAS_BACK. As if the act is these two, it will be notified during
  ** sd_si_update();
  */
  if( event == SDSS_EVT_HDR_OPR_OH_SID ||
      event == SDSS_EVT_HDR_OPR_OH_CHAN ||
      event == SDSS_EVT_MISC_FILLER)
  {
    if(!( ss_act.act == SD_SS_ACT_CONTINUE ||
          ss_act.act == SD_SS_ACT_MEAS_BACK ))
    {
    
      SD_MSG_HIGH_2("SCSVLTE: Check pref_srv_acq, event=%d, act=%d",
                                               event, ss_act.act);
      sd_si_pref_srv_acq( SD_SS_HYBR_HDR );
    }
  }
  else
  {
    sd_si_pref_srv_acq( SD_SS_HYBR_HDR );
  }

  /* Unlock the HDR ss component.
  */
  SD_SS_FREE( ss_ptr );

  return ss_act.act;

} /* sd_ss_hdr_event_proc */

#endif /* FEATURE_HDR_HYBRID */


/*===========================================================================

FUNCTION sd_ss_timer_rest

DESCRIPTION
  Reset the SD timer

EPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sd_ss_timer_reset(
        sd_ss_e_type              ss
            /* System selection - MAIN or HDR */
)
{
  SD_ASSERT( ss < SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the timer value and disable the SS-timer.
  */
  sd_ss_ptr(ss)->ss_timer = 0;

  sd_ss_ptr(ss)->timer_func_ptr(0);

}/* sd_ss_timer_reset */


#if (defined (FEATURE_HDR_HYBRID))
/*===========================================================================

FUNCTION sd_ss_hdr_acq_update_post

DESCRIPTION
  Does post HDR acquisition attempt updates.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                          sd_ss_hdr_acq_update_post(

        sdsr_e_type                   list,
            /* List from which acquisition was attempted.
            */
        int                           pos,
            /* List position of system that was attempted acquisition.
            */

        sdss_acq_mode_e_type          acq_mode
            /* The acquisition mode that was attempted.
            */
)
{
  sdsr_ref_type         sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( acq_mode, SDSS_ACQ_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to the system that was attempted acquisition.
  */
  sr_ref = sdsr_list_get_ref( list, pos );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* According to the acquisition mode that was attempted, update
  ** the appropriate SDSR_HDR_ACQ_xxx_SYS system.
  */
  switch( acq_mode )
  {
    case SDSS_ACQ_MODE_FULL:
      sdsr_list_put_ref( SDSR_HDR_ACQ_FULL_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_MINI:
      sdsr_list_put_ref( SDSR_HDR_ACQ_MINI_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_MICRO:
      sdsr_list_put_ref( SDSR_HDR_ACQ_MICRO_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_FULL2:
      sdsr_list_put_ref( SDSR_HDR_ACQ_FULL2_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_MINI2:
      sdsr_list_put_ref( SDSR_HDR_ACQ_MINI2_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_MICRO2:
      sdsr_list_put_ref( SDSR_HDR_ACQ_MICRO2_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_DEEP:
      sdsr_list_put_ref( SDSR_HDR_ACQ_DEEP_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_DEEP2:
      sdsr_list_put_ref( SDSR_HDR_ACQ_DEEP2_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_SHALLOW:
      sdsr_list_put_ref( SDSR_HDR_ACQ_SHALLOW_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_SHALLOW2:
      sdsr_list_put_ref( SDSR_HDR_ACQ_SHALLOW2_SYS, sr_ref );
      break;

    case SDSS_ACQ_MODE_EOOS_FULL_RTN:
      sdsr_list_put_ref( SDSR_ACQ_EOOS_FULL_SYS, sr_ref );
      break;

    /* The the following ACQ_MODEs are specific for CDMA and  are not
    ** supported for HDR operation.
    **
    */
    case SDSS_ACQ_MODE_EOOS_FULL:
    case SDSS_ACQ_MODE_EOOS_FULL2:
    case SDSS_ACQ_MODE_EOOS_FULL_RTN2:
    case SDSS_ACQ_MODE_MICRO_ABSR:
    case SDSS_ACQ_MODE_FULL_AOOS:
    case SDSS_ACQ_MODE_PREV:
    case SDSS_ACQ_MODE_HO:
    case SDSS_ACQ_MODE_FULL_OSR:
    case SDSS_ACQ_MODE_MAX:
    default:
      SD_ERR_0( "acq_mode");
      break;

  } /* switch */
} /* sd_ss_hdr_acq_update_post */
#endif /* FEATURE_HDR_HYBRID */


/*===========================================================================

FUNCTION sd_ss_act_get_true_curr_act

DESCRIPTION
  Return the true SS-Action that was most recently returned to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{
  #ifdef FEATURE_HYBR_3GPP
  return sd_ss_hybr_2_act_get_true_curr_act(rtrn_act_ptr, SD_SS_HYBR_2);
  #elif defined(CFG_HYBR_GW_3)
  return sd_ss_hybr_2_act_get_true_curr_act(rtrn_act_ptr , SD_SS_HYBR_3);
  #else

  sd_ss_act_e_type act = SD_SS_ACT_MAX;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  SD_CRIT_SECT_ENTER();
  act = sd_ss_ptr(SD_SS_MAIN)->act.act;
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = sd_ss_ptr(SD_SS_MAIN)->act;
  }
  SD_CRIT_SECT_EXIT();
  return act;
  #endif

} /* sd_ss_act_get */


#ifdef FEATURE_GPSONE_MSBASED
/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_pref_opti_gps

DESCRIPTION
  Performs optimized GPS relate SS-Preference changes.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if doing optimized GPS relate SS-Preference change. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean                   sd_ss_pref_opti_gps(

        sd_ss_e_type              ss,
            /* Pointer to system selection.
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /* New mode preference.
            */

        sd_ss_orig_mode_e_type    orig_mode,
            /* New origination mode.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  sd_ss_s_type            *ss_ptr;
  sd_ss_act_s_type        ss_act;
  sd_ss_act_s_type        ss_true_curr_act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( orig_mode, SD_SS_ORIG_MODE_MAX );

  ss_ptr = sd_ss_ptr(ss);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If mode pref was changed to GPS and SS-State is not operation and last
  ** SS-Action is not continue, do optimized GPS mode preference change.
  */
  (void)sd_ss_act_get_true_curr_act_ss(ss,&ss_act);
  (void)sd_ss_act_get_curr_act_ss(ss,&ss_true_curr_act);

  if( mode_pref == SD_SS_MODE_PREF_GPS &&
      sd_get_ss_state(ss) != SDSS_STATE_OPR &&
      ss_act.act != SD_SS_ACT_CONTINUE )
  {
    /* Set the optimized GPS SS-Preference change flag.
    */
    ss_ptr->gps_is_opti_ss_pref = TRUE;

    /* Only set previous action if last action was not SD_SS_ACT_ACQ_GPS.
    */
    if( ss_act.act != SD_SS_ACT_ACQ_GPS ) {
      ss_ptr->prev_act = ss_act;
    }

    /* Set SS-Action to SD_SS_ACT_ACQ_GPS.
    */
    SD_MSG_LOW_1( "GPS acq start, prev_acq=%d", ss_ptr->prev_act.act);
    ss_act.act       = SD_SS_ACT_ACQ_GPS;
    ss_true_curr_act = ss_act;

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = ss_act.act;
    }

    sd_ss_act_set_true_curr_act_ss(ss,&ss_act);
    sdss_act_print( &ss_act );
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If previously we did an optimized GPS SS-Preference change and the new
  ** mode pref is not GPS and we are not trying to originate a call and
  ** previous SS-Action was not ACQ_GPS or CONTINUE, do optimized GPS mode
  ** preference change.
  */
  else if( ss_ptr->gps_is_opti_ss_pref &&
           mode_pref != SD_SS_MODE_PREF_GPS &&
           ss_ptr->prev_act.act != SD_SS_ACT_ACQ_GPS &&
           ss_ptr->prev_act.act != SD_SS_ACT_CONTINUE &&
           orig_mode == SD_SS_ORIG_MODE_NONE )
  {
    /* Reset the optimized GPS SS-Preference change flag.
    */
    ss_ptr->gps_is_opti_ss_pref = FALSE;

    /* Set SS-Action per previous action.
    */
    ss_act = ss_ptr->prev_act;
    ss_true_curr_act = ss_act;
    SD_MSG_LOW_1( "GPS acq end, prev_acq=%d", ss_ptr->prev_act.act);

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( rtrn_act_ptr != NULL )
    {
      rtrn_act_ptr->act = ss_act.act;
    }

    sdss_act_print( &ss_act );
    sd_ss_act_set_true_curr_act_ss(ss,&ss_act);

    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, reset the gps_is_opti_ss_pref flag and return FALSE to
  ** indicate that this is not an optimized GPS relate SS-Preference change.
  */
  ss_ptr->gps_is_opti_ss_pref = FALSE;
  return FALSE;

} /* sd_ss_pref_opti_gps */
#endif /* FEATURE_GPSONE_MSBASED */


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/**==========================================================================

@FUNCTION sd_si_update_cdma_time

@DESCRIPTION
  Updates CDMA -LTM offset and daylight savings info and leap seconds in
  sd_si_info object

@DEPENDENCIES
  None

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
void                      sd_si_update_cdma_time(

        sd_ss_e_type              ss,
        /**< System selection - MAIN */

        uint8                     ltm_offset,
        /**< Ltm offset on acquired CDMA system */

        sys_daylt_savings_e_type  daylt_savings,
        /**< Daylight savings on acquired CDMA system */

        byte                      leap_secs
        /**< Leap seconds */
)
{
  sd_si_info_s_type     *si_info_ptr = &sd_si_ptr(ss)->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ltm_offset != SYS_LTM_OFFSET_INVALID)
  {
    si_info_ptr->time_info.sys_mode = SYS_SYS_MODE_CDMA;
  }
  else
  {
    /* If ltm offset is invalid ,then indicate that the time info is invalid
    ** by setting the sys mode to NO SRV
    */
    si_info_ptr->time_info.sys_mode = SYS_SYS_MODE_NO_SRV;
  }

  si_info_ptr->time_info.time.cdma_time.ltm_offset     = ltm_offset;

  si_info_ptr->time_info.time.cdma_time.daylt_savings  = daylt_savings;

  si_info_ptr->time_info.time.cdma_time.lp_sec         = leap_secs;

  SD_MSG_MED_4("mode %d ltm %d daylt %d lp_sec %d", si_info_ptr->time_info.sys_mode,
               ltm_offset,daylt_savings, leap_secs);
 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sd_si_update_cdma_time */


void                      sd_ss_redir_rec_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_redir_rec_s_type *redir_rec_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec
            /* number of records in the redirection record list.
            */
)
{
  int                   i;
  sd_redir_rec_s_type   *rec_list_ptr = (sd_redir_rec_s_type *)redir_rec_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_rec_ptr != NULL || num_rec == 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the redirection records and store them into the system
  ** record data base.
  */

  for( i=0; i<num_rec; i++, rec_list_ptr++ )
  {
    SD_ASSERT( rec_list_ptr != NULL );

    if( rec_list_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }

    switch( rec_list_ptr->record_type )
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      #ifdef FEATURE_ACP
      case SD_REDIR_REC_AMPS:

         sd_ss_redir_rec_amps_store( sdsr_list,
                                     &rec_list_ptr->rec.amps_rec,
                                     rec_list_ptr->is_delete_tmsi);

      break;
      #endif
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_CDMA:


        sd_ss_redir_rec_cdma_store( sdsr_list,
                                    &rec_list_ptr->rec.cdma_rec,
                                    rec_list_ptr->is_delete_tmsi);

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_NDSS_OFF:
      case SD_REDIR_REC_TACS:
      case SD_REDIR_REC_JTACS:
      case SD_REDIR_REC_DS41:
      case SD_REDIR_REC_MAX:
      default:
        SD_ERR_1("Bad red rec type %d",  rec_list_ptr->record_type);
        break;
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;


}/* sd_ss_redir_rec_store */


/*===========================================================================

FUNCTION sd_ss_get_redir_rec_get_num_chan

DESCRIPTION
  Calculate the sum of the channels in the redirection records

DEPENDENCIES

RETURN VALUE
 The sum of the channels.

SIDE EFFECTS
  None.

===========================================================================*/
int                       sd_ss_redir_rec_get_num_chan(

        const sd_redir_rec_s_type *redir_rec_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec
            /* number of records in the redirection record list.
            */
)
{
  int i;
  int num_chan = 0;
  sd_redir_rec_s_type   *rec_list_ptr = (sd_redir_rec_s_type *)redir_rec_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_rec_ptr != NULL || num_rec == 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the redirection records and get the sum of the channels per
  ** redirection record.
  */

  for( i=0; i<num_rec; i++, rec_list_ptr++ )
  {
    SD_ASSERT( rec_list_ptr != NULL );

    if( rec_list_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }

    switch( rec_list_ptr->record_type )
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_AMPS:

         /* For AMPS, treat it as one channel per record.
         */
         num_chan += 1;

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_CDMA:

        num_chan += rec_list_ptr->rec.cdma_rec.num_chans;

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SD_REDIR_REC_NDSS_OFF:
      case SD_REDIR_REC_TACS:
      case SD_REDIR_REC_JTACS:
      case SD_REDIR_REC_DS41:
      case SD_REDIR_REC_MAX:
      default:
        SD_ERR_1("Bad red rec type %d",  rec_list_ptr->record_type);
        break;
    }

  }

  return num_chan;
}


#ifdef FEATURE_ACP
/*===========================================================================

FUNCTION sd_ss_get_redir_rec_amps_store

DESCRIPTION
  Step through the redirection message and store the redirection record that
  contains valide band/channl in the redirection list.
DEPENDENCIES

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
void                      sd_ss_redir_rec_amps_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_redir_rec_amps_s_type  *redir_rec_amps_ptr,
            /* Pointer to a analog redirection record to which the MS is
            ** being redirected.
            */

        boolean                   is_delete_tmsi
            /* Indicate if MS is required to delete the TMSI assigned.
            */

)
{
  sd_blksys_e_type    blksys;
  sdsr_ref_type       sr_ref;
  sd_cell_sys_e_type  first_sys       = SD_CELL_SYS_MAX;
  sd_cell_sys_e_type  second_sys      = SD_CELL_SYS_MAX;
  sd_cell_sys_e_type  redir_sys;
  sd_cell_sys_e_type  other_sys;
  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  int                 avail_entries   = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_rec_amps_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sdsr_list, SDSR_MAX );
  if(srv_sys_ptr != NULL)
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Decide whether redirecting system is an A or B system.
    */
    blksys = sd_map_chan_to_blksys( srv_sys_ptr->sys.mode,
         sdss_map_band_pref_to_band(srv_sys_ptr->sys.band),
                                     srv_sys_ptr->sys.chan );

    if( blksys == SD_BLKSYS_CELL_B )
    {
      redir_sys = SD_CELL_SYS_B;
      other_sys = SD_CELL_SYS_A;
    }
    else
    {
      redir_sys = SD_CELL_SYS_A;
      other_sys = SD_CELL_SYS_B;
    }
  }
  else
  {
    SD_ERR_0("srv_sys_ptr NULL");
    redir_sys = SD_CELL_SYS_A;
    other_sys = SD_CELL_SYS_B;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* According to the system ordering find out to which AMPS system(s) MS is
  ** being redirected to.
  */
  switch( redir_rec_amps_ptr->sys_ord )
  {
    case SD_SS_SYS_ORD_A_OR_B:

      /* Decide on the AMPS system according to the SID that is included with
      ** the redirection.
      */
      if( redir_rec_amps_ptr->expected_sid == SD_WILDCARD_SID )
      {
        if( sdss_is_band_pref( SD_SS_MAIN, SD_SS_BAND_PREF_CELL_A) )
        {
          first_sys   = SD_CELL_SYS_A;
        }
        else if( sdss_is_band_pref( SD_SS_MAIN, SD_SS_BAND_PREF_CELL_B) )
        {
          first_sys   = SD_CELL_SYS_B;
        }
        else
        {
          first_sys   = redir_sys;
          second_sys  = other_sys;
        }
      }
      else if( redir_rec_amps_ptr->expected_sid & 0x0001 )
      {
        first_sys   = SD_CELL_SYS_A;
      }
      else
      {
        first_sys   = SD_CELL_SYS_B;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_A_ONLY:
      first_sys   = SD_CELL_SYS_A;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_B_ONLY:
      first_sys   = SD_CELL_SYS_B;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_A_THEN_B:
      first_sys   = SD_CELL_SYS_A;
      second_sys  = SD_CELL_SYS_B;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_B_THEN_A:
      first_sys   = SD_CELL_SYS_B;
      second_sys  = SD_CELL_SYS_A;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_A_AND_B:

      /* Decide on the AMPS system according to the SID that is included with
      ** the redirection.
      */
      if( redir_rec_amps_ptr->expected_sid == SD_WILDCARD_SID )
      {
        if( sdss_is_band_pref( SD_SS_MAIN, SD_SS_BAND_PREF_CELL_A) )
        {
          first_sys   = SD_CELL_SYS_A;
          second_sys  = SD_CELL_SYS_B;
        }
        else if( sdss_is_band_pref( SD_SS_MAIN, SD_SS_BAND_PREF_CELL_B) )
        {
          first_sys   = SD_CELL_SYS_B;
          second_sys  = SD_CELL_SYS_A;
        }
        else
        {
          first_sys   = redir_sys;
          second_sys  = other_sys;
        }
      }
      else if( redir_rec_amps_ptr->expected_sid & 0x0001 )
      {
        first_sys   = SD_CELL_SYS_A;
        second_sys  = SD_CELL_SYS_B;
      }
      else
      {
        first_sys   = SD_CELL_SYS_B;
        second_sys  = SD_CELL_SYS_A;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_SYS_ORD_MAX:
    default:
      SD_ERR_0( "sys_ord");
      break;

  } /* switch */

  SD_ASSERT( first_sys != SD_CELL_SYS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out the number of system record table entries that are available
  ** for storing the redirection's channels.
  */
  avail_entries = sdsr_tbl_size() - sdsr_list_cnt(SDSR_PRL_LST);
  SD_ASSERT( avail_entries >= 2 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MS supports AMPS mode, store the redirection's channels in the system
  ** record table and update SDSR_REDIR_LST to reference the stored records.
  ** Else, indicate that this redirection is invalid.
  */
  if( sdss_is_supp_mode_band(SD_MODE_AMPS,
                             sd_map_band_to_band_pref(SD_BAND_CELL),
                             SYS_LTE_BAND_MASK_CONST_NONE,
                             SD_SS_BAND_PREF_NONE) )
  {
    if( first_sys != SD_CELL_SYS_MAX &&
        avail_entries > 0 )
    {
      sr_ref = sdsr_tbl_store4( SD_MODE_AMPS,
                                SD_BAND_CELL,
                                (word) first_sys,
                                redir_rec_amps_ptr->expected_sid,
                                SD_WILDCARD_NID,
                                is_delete_tmsi );

      sdsr_list_insert_ref( SDSR_REDIR_LST, SDSR_POS_NULL, sr_ref, FALSE );
    }

    if( second_sys != SD_CELL_SYS_MAX &&
        avail_entries > 1 )
    {
      sr_ref = sdsr_tbl_store4( SD_MODE_AMPS,
                                SD_BAND_CELL,
                                (word) second_sys,
                                redir_rec_amps_ptr->expected_sid,
                                SD_WILDCARD_NID,
                                is_delete_tmsi );

      sdsr_list_insert_ref( sdsr_list, SDSR_POS_NULL, sr_ref, FALSE );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

}/* sd_ss_redir_rec_amps_store */

#endif /* FEATURE_ACP */


/*===========================================================================

FUNCTION sd_ss_get_redir_rec_cdma_store

DESCRIPTION
  Step through the redirection message and store the redirection record that
  contains valide band/channl in the redirection list.
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void                      sd_ss_redir_rec_cdma_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_redir_rec_cdma_s_type  *redir_rec_cdma_ptr,
            /* Pointer to a analog redirection record to which the MS is
            ** being redirected.
            */
        boolean                   is_delete_tmsi
            /* Indicate if MS is required to delete the TMSI assigned.
            */

)
{
  int                            i;
  int                            avail_entries;
  int                            num_chans;
  sdsr_ref_type                  sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( redir_rec_cdma_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sdsr_list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out the number of system record table entries that are available
  ** for storing the redirection's channels.
  */
  num_chans = redir_rec_cdma_ptr->num_chans;
  avail_entries = sdsr_tbl_size() - sdsr_list_cnt(SDSR_PRL_LST);
  SD_ASSERT( avail_entries >= num_chans );
  num_chans = MIN( num_chans, avail_entries );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported redirection's channels in the system record table and
  ** update SDSR_REDIR_LST to reference the stored records.
  */
  for( i=0; i < num_chans; i++ )
  {
    SD_ASSERT( &redir_rec_cdma_ptr->chan[i] != NULL );

    SD_MSG_HIGH_2("IRAT: band=0x%lx, chan=%d", redir_rec_cdma_ptr->band_class,
                                             redir_rec_cdma_ptr->chan[i]);
    if( sdss_is_supp_sys(SD_MODE_CDMA,
                         sd_map_band_to_band_pref(redir_rec_cdma_ptr->band_class),
                         redir_rec_cdma_ptr->chan[i]) )
    {
      sr_ref = sdsr_tbl_store4( SD_MODE_CDMA,
                                redir_rec_cdma_ptr->band_class,
                                redir_rec_cdma_ptr->chan[i],
                                redir_rec_cdma_ptr->expected_sid,
                                redir_rec_cdma_ptr->expected_nid,
                                is_delete_tmsi );

      sdsr_list_insert_ref( sdsr_list, SDSR_POS_NULL, sr_ref, FALSE );

      SD_MSG_HIGH_0("IRAT: 1x sys inserted");

    }
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 return;

}/* sd_ss_redir_rec_cdma_store */


#if defined(FEATURE_LTE_TO_1X)
/*===========================================================================

FUNCTION sd_ss_ho_sys_param_store

DESCRIPTION
   store the ho system params.
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void                      sd_ss_ho_sys_param_store(

        sdsr_e_type               sdsr_list,
            /* Pointed to the sdsr list to store the redirection record.
            */

        const sd_ho_sys_param_s_type *ho_system_pram_ptr
            /* Pointer to a analog redirection record to which the MS is
            ** being redirected.
            */

)
{

  sdsr_ref_type                  sr_ref;
  //int                            avail_entries;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( ho_system_pram_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sdsr_list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Store supported handovered channels in the system record table and
  ** update SDSR_LTE_CDMA_HO_LST to reference the stored records.
  */


    SD_MSG_HIGH_2("LTE to CDMA HO: band=0x%lx, chan=%d", ho_system_pram_ptr->band,
                                             ho_system_pram_ptr->chan);
    if( sdss_is_supp_sys(SD_MODE_CDMA,
                         sd_map_band_to_band_pref(ho_system_pram_ptr->band),
                         ho_system_pram_ptr->chan) )
    {
      sr_ref = sdsr_tbl_store4( SD_MODE_CDMA,
                                ho_system_pram_ptr->band,
                                ho_system_pram_ptr->chan,
                                ho_system_pram_ptr->sid,
                                ho_system_pram_ptr->nid,
                                FALSE
                              );

      sdsr_list_insert_ref( sdsr_list, SDSR_POS_NULL, sr_ref, FALSE );

      SD_MSG_HIGH_0("LTE to CDMA HO: 1x sys inserted");

    }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 return;

}/* sd_ss_ho_sys_param_store */
#endif /* FEATURE_LTE_TO_1X */

#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */


#ifdef FEATURE_HDR

void                      sd_ss_idle_cand_sys_update
(
        sdsr_e_type               cand_sys,
            /* candidate system to be updated.
            */

        sd_ss_e_type              ss,
            /* System selection enum type
            */

        sdsr_e_type               chan_list,
            /* Input channel list
            */

        sdsr_e_type               acq_sys
            /* Input acquisition system
            */

)
{
  sdsr_ref_type                   chan_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cand_sys, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( chan_list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( acq_sys, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the candidate system.
  */
  if( acq_sys != SDSR_MAX )
  {
    /* Update cand_sys per acq_sys.
    */
    sdsr_list_cpy( cand_sys, acq_sys );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If channel list is empty, return now.
  */
  if( sdsr_list_cnt( chan_list ) == 0 )
  {
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a reference to the first channel system.
  */
  chan_ref = sdsr_list_get_ref( chan_list, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find whether the system in question matches any of the PRL system
  ** records.
  */
  if( sdprl_is_sys_match( ss, chan_ref ) )
  {
    /* Update the cand_sys.
    */
    sdsr_list_put_ref( cand_sys, chan_ref );

    SD_MSG_HIGH_0("Find new cand sys");

    return;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /* sd_ss_idle_cand_sys_update */


/*===========================================================================

FUNCTION sd_ss_hdr_idle_sys_update

DESCRIPTION
  Update the idle system, its GEO list and PREF lists per the input lists
  as necessary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                          sd_ss_hdr_idle_sys_update(

        sdsr_e_type                   sys,
            /* System per which to update the idle system.
            ** Ignored if set to SDSR_MAX.
            */

        sdsr_e_type                   geo_list,
            /* List per which to update the GEO list.
            */

        sdsr_e_type                   pref_list
            /* List per which to update the PREF list.
            */
)
{
  sdsr_ref_type     sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( BETWEEN(sys, SDSR_1ST_SYS, SDSR_MAX) || sys == SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( geo_list, SDSR_1ST_SYS);
  SD_ASSERT_ENUM_IS_INRANGE( pref_list, SDSR_1ST_SYS);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the following HDR idle related systems.
  */
  sdsr_list_cpy( SDSR_HDR_IDL_GEO_LST,  geo_list );
  sdsr_list_cpy( SDSR_HDR_IDL_PREF_LST, pref_list );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If sys is not to be ignored, do the following:
  */
  if( sys != SDSR_MAX )
  {
    /* Update the idle system per sys.
    */
    sdsr_list_cpy( SDSR_HDR_IDL_SYS, sys );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Insert system to beginning of MRU list (remove duplicates).
    */
    sr_ref = sdsr_list_get_ref( sys, 0 );
    SD_ASSERT( sr_ref != SDSR_REF_NULL )
    sdsr_list_insert_ref( SDSR_HDR_MRU_LST, 0, sr_ref, TRUE );
    sdsr_list_print(SDSR_HDR_MRU_LST,FALSE);

  } /* if( sys != SDSR_MAX ) */

} /* sd_ss_hdr_idle_sys_update */


/**==========================================================================

@FUNCTION sd_si_update_hdr_time

@DESCRIPTION
  Updates HDR -LTM offset and leap seconds in
  sd_si_info object. For HDR daylight savings is
  not supported.

@DEPENDENCIES
  None

@RETURNS
  None

@SIDE_EFFECT
  None

@x
===========================================================================*/
void                      sd_si_update_hdr_time(

        sd_ss_e_type              ss,
        /**< System selection - MAIN or HDR. */

        int16                     ltm_offset,
        /**< Ltm offset on acquired HDR system */

        uint8                     leap_secs
        /**< Leap seconds */
)
{
  sd_si_info_s_type     *si_info_ptr = &sd_si_ptr(ss)->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ltm_offset != SYS_LTM_OFFSET_INVALID)
  {
    si_info_ptr->time_info.sys_mode                 = SYS_SYS_MODE_HDR;
  }
  else
  {
    si_info_ptr->time_info.sys_mode                 = SYS_SYS_MODE_NO_SRV;
  }

  si_info_ptr->time_info.time.hdr_time.ltm_offset   = ltm_offset;

  si_info_ptr->time_info.time.hdr_time.lp_sec       = leap_secs;
  SD_MSG_MED_3("mode %d ltm %d lp_sec %d", si_info_ptr->time_info.sys_mode,
                                                ltm_offset,leap_secs);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sd_si_update_hdr_time */


/*===========================================================================

FUNCTION sd_si_sys_usable

DESCRIPTION
  Notify the registered client (i.e., CM) of system usable status changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                      sd_si_sys_usable(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_sys_usable_e_type      usable_status
            /* new usable status of the system.
            */
)
{
  sd_si_info_s_type     *si_info_ptr = &sd_si_ptr(ss)->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifdef FEATURE_JCDMA
  /* Always force hdr usable status as usable and ignoring what is reported
  ** from protocol in order to prevent IDM change event in CM, which is
  ** triggered by HDR system unusable report.
  */
  usable_status = SD_SYS_USABLE_OK;

  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* No need to inform the registered client of change if the usable status
  ** is not changed.
  */
  if( si_info_ptr->usable_status == usable_status )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the usable status in the service indicator.
  */
  si_info_ptr->usable_status = usable_status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (e.g., CM) of the service indicators
  ** change.
  */
  SD_MSG_HIGH_2( "Usable status change=%d ss %d", usable_status, ss);
  sd_invoke_info_callback(ss);
} /* sd_si_usable_status */


/*===========================================================================

FUNCTION sd_si_update_bcmcs_status

DESCRIPTION
  Update the BCMCS service status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                             sd_si_update_bcmcs_status(

        sd_ss_e_type                     ss,
           /* SD instance.
           */

        sys_bcmcs_srv_status_e_type      bcmcs_srv_status
           /* New BCMCS service status.
           */
)
{

  sd_si_info_s_type     *si_info_ptr = &sd_si_ptr(ss)->si_info;;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( bcmcs_srv_status, SYS_BCMCS_SRV_STATUS_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the bcmcs service status was changed.
  */
  if ( si_info_ptr->bcmcs_srv_status !=  bcmcs_srv_status )
  {
     /* Bcmcs service status is changed, update it and
     */
     si_info_ptr->bcmcs_srv_status = bcmcs_srv_status;

     /* Inform the registered client (e.g., CM) of the new service indicators.
     */
     SD_MSG_HIGH_2( "BCMCS Service is %d SS=%d", bcmcs_srv_status,ss);
     sd_invoke_info_callback(ss);
  } /* if ( )  */
} /* sd_si_update_bcmcs_status () */

#endif /* defined(FEATURE_HDR) */


/*===========================================================================

FUNCTION sd_ss_hdr_act_get_true_curr_act

DESCRIPTION
  Return to the HDR client the SS-Action that was most recently returned to
  the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_hdr_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  #ifndef FEATURE_HDR_HYBRID

  return sd_ss_act_get_true_curr_act( rtrn_act_ptr );

  #else /* FEATURE_HDR_HYBRID */

  sd_ss_s_type           *ss_ptr     = sd_ss_ptr(SD_SS_HYBR_HDR);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  if( rtrn_act_ptr != NULL )
  {
    sd_ss_act_e_type act = SD_SS_ACT_MAX;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If caller supplied a buffer, copy the most recent SS-Action into
    ** into the supplied buffer.
    */
    SD_CRIT_SECT_ENTER();
    act = sd_ss_ptr(SD_SS_HYBR_HDR)->act.act;

    *rtrn_act_ptr = sd_ss_ptr(SD_SS_HYBR_HDR)->act;

    SD_CRIT_SECT_EXIT();
    return act;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return most recent SS-Action.
  */
  return ss_ptr->act.act;

  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_hdr_act_get_true_curr_act */


/*===========================================================================

FUNCTION sd_si_update_hdr_activity

DESCRIPTION
  Notify the registered client (i.e., CM) that hdr activity is changed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                      sd_si_update_hdr_activity(

        boolean                   is_hdr_activity
        /* New acquisition throttling status. */

)
{
  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  sd_si_info_s_type         *si_info_ptr = &sd_si_ptr(SD_SS_MAIN)->si_info;

  if( si_info_ptr->is_hdr_activity != is_hdr_activity )
  {
    si_info_ptr->is_hdr_activity = is_hdr_activity;

    SD_MSG_HIGH_1("HICPS: Updating is_hdr_activity status %d to CM",
                                                    is_hdr_activity);

    sd_invoke_info_callback( SD_SS_MAIN );
  }
  #else
  SYS_ARG_NOT_USED( is_hdr_activity );
  #endif
}


#if defined (FEATURE_SD_LTE)
void                      sd_si_update_nas_addl_info(

     sd_ss_e_type                ss,

     sys_lte_cs_capability_e_type lte_cs_capability,
           /* New LTE CS capability
           */

     sys_extend_srv_info_e_type   ext_srv_info
 )
{
  sd_si_info_s_type         *si_info_ptr = &sd_si_ptr(ss)->si_info;

  SD_MSG_HIGH_2("SI_INFO:FRFL: Updating lte_cs_capability %d to %d",
                           si_info_ptr->lte_cs_capability,lte_cs_capability);
  SD_MSG_HIGH_2("SI_INFO:FRFL: Updating extend_srv_info %d to %d",
                   si_info_ptr->extend_srv_info, ext_srv_info);
  si_info_ptr->lte_cs_capability = lte_cs_capability;
  si_info_ptr->extend_srv_info = ext_srv_info;
  sdss_set_lte_cs_cap(lte_cs_capability);

}

#endif /* FEATURE_SD_LTE */


#ifdef FEATURE_EOOS_UMTS
/*===========================================================================

FUNCTION sd_display_eoos_params

DESCRIPTION
  Displays the EOOS parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void sd_display_eoos_params (const sd_ss_act_acq_gwl_s_type*  act_ptr)
{
    int i;
    int max_loop;
    /*lint -esym(550, gsm_scan) */ /* not accessed */
    /*lint -esym(550, wcdma_scan) */ /* not accessed */
    uint32 gsm_scan;
    uint32 wcdma_scan;

    SD_ASSERT(act_ptr != NULL);

    max_loop = (int)act_ptr->rat_pri_list_info.num_items;
    if(max_loop > SYS_MAX_ACQ_PREF_RAT_LIST_NUM)
    {
      max_loop = SYS_MAX_ACQ_PREF_RAT_LIST_NUM;
    }

    gsm_scan = ~0;
    wcdma_scan = ~0;

    /* For now extract only G and W */
    for( i=0; i < max_loop; i++ )
    {
        if( act_ptr->
            rat_pri_list_info.priority_list_info[i].acq_sys_mode ==
            SYS_SYS_MODE_GSM )
        {
            gsm_scan = act_ptr->
              rat_pri_list_info.priority_list_info[i].acq_sys_time_interval;
        }
        else if ( act_ptr->rat_pri_list_info.priority_list_info[i].acq_sys_mode ==
                  SYS_SYS_MODE_WCDMA )
        {
            wcdma_scan = act_ptr->
              rat_pri_list_info.priority_list_info[i].acq_sys_time_interval;
        }
    }

    SD_MSG_HIGH_3( "EOOS for GW: mode_pref: 0x%x use timer: 0x%x new acq: 0x%x",
        act_ptr->gwl_mode_pref,
        act_ptr->rat_pri_list_info.scan_type.use_timer,
        act_ptr->rat_pri_list_info.scan_type.new_scan);

    SD_MSG_HIGH_2( "EOOS for GW scan times: wcdma_scan: %ld [ms] gsm_scan: %ld [ms]",
        wcdma_scan, gsm_scan);
}
/*lint +esym(550, gsm_scan) */ /* not accessed */
/*lint +esym(550, wcdma_scan) */ /* not accessed */
#endif

/*===========================================================================

FUNCTION sd_si_info_ptr_get

DESCRIPTION
  This function is used to fetch the current SI info. This is used to ensure
  integrity of the structure. Every instance of the usage of the structure
  will always call this function and take a local copy of the contents of the
  si_info. Further processing will be done on the local copy.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void                           sd_si_info_ptr_get(

       sd_ss_e_type                   ss,

       sd_si_info_s_type*             info
)
{
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( info != NULL );

  SD_CRIT_SECT_ENTER();
  *info = sd_si_ptr(ss)->si_info;
  SD_CRIT_SECT_EXIT();
}


/*===========================================================================

FUNCTION sd_si_reg

DESCRIPTION
  Register callback functions with the Service Indicators component.

  SD calls on the Service Indicators callback function whenever one or more
  of the service indicators changes their value.

  SD calls on the Ok To Orig callback function whenever the Ok To Orig status
  changes from FALSE to TRUE while the origination mode is other than none.

  SD calls on the Emergency Callback Mode callback function whenever the
  Emergency Callback Mode status changes from FALSE to TRUE.

  NOTE! A new call to this function overrides all previously registered
  callback functions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                          sd_si_reg(

        sd_si_info_f_type             *si_info_cb_func,
            /* Pointer to a Service Indicators callback function. SD calls on
            ** this function whenever one or more of the service indicators
            ** changes their value.
            **
            ** The argument to this function is a const pointer to a
            ** structure containing the current value of the service
            ** indicators */

        sd_si_acq_fail_f_type         *acq_fail_cb_func,
            /* Pointer to an Acquisition Fail callback function. SD calls on
            ** this function whenever acquisition fails (i.e., SD have
            ** attempted full acquisitions on all systems with no success).
            */

        sd_si_ok_to_orig_f_type       *ok_to_orig_cb_func,
            /* Pointer to an Ok To Orig callback function. SD calls on this
            ** function whenever the Ok To Orig status changes from FALSE to
            ** TRUE while the origination mode is other than none */

        sd_si_emerg_cb_mode_f_type    *emerg_cb_mode_cb_func,
            /* Pointer to an Emergency Callback Mode callback function. SD
            ** calls on this function whenever the Emergency Callback Mode
            ** status changes from FALSE to TRUE */

        sd_si_autonam_change_f_type   *autonam_change_cb_func,
            /* Pointer to an auto-NAM callback function. SD calls on this
            ** function whenever auto-NAM change is appropriate */


        sd_si_gw_net_list_f_type      *gw_net_list_cb_func,
            /* Pointer to the GW network list call function. SD calls on
            ** this function once it receives the results of the get network
            ** list on GSM/WCDMA/LTE systems.
            */

        sd_si_sim_state_f_type        *sim_state_cb_func,
           /* Pointer to the sim state call back function. Sd calls on this
           ** function if the SIM state was changed.
           */
        sd_si_reg_done_f_type         *reg_done_cb_func,
           /* Pointer to the reg. done call back function. SD calls this when
           ** a reg. is done successfully.
           */
        sd_si_data_suspend_f_type     *data_suspend_cb_func,
           /* Pointer to the data suspend call back function. SD calls this when
           ** a REG reports change in data suspend flag.
           */

        sd_si_srv_lost_f_type         *srv_lost_cb_func,
           /**< Pointer to the service lost callback. Sd calls on this
           ** function once the service is lost
           */

        sd_si_prl_load_complete_f_type *si_prl_load_complete_cb_func,
           /**< Pointer to the prl load complete. Sd calls on this
           ** function once the prl is loaded.
           */

        sd_si_ss_stack_status_f_type   *ss_stack_status_cb_func,
           /**< Pointer to the stack status callback. SD calls on this
           ** function when the stack status is changed.
           */

        sd_si_generic_data_f_type      *sd_generic_cb_func,
           /**< Pointer to generic callback function. SD Calls to update
           ** CM with generic data depending on th type.
           */
        sd_si_full_srv_lost_f_type      *full_srv_lost_cb_func,
           /**< Pointer to full service lost callback function. SD Calls on this function 
                  once full service is lost
           */
        sd_si_cdma_bsr_stat_chgd_f_type *cdma_bsr_stat_chgd_cb_func,
           /**< Pointer to the BSR status changed function. SD calls on
            ** this function once the BSR status has changed */
			
		sd_si_cs_emerg_scan_fail_f_type    *cs_emerg_scan_fail_cb_func
		  /**SD report for sending cs scan fail during e911*/
)
{
  sd_si_s_type        *si_ptr     = sd_si_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Register the callback functions.
  */
  si_ptr->si_info_cb_func         = si_info_cb_func;
  si_ptr->acq_fail_cb_func        = acq_fail_cb_func;
  si_ptr->ok_to_orig_cb_func      = ok_to_orig_cb_func;
  si_ptr->emerg_cb_mode_cb_func   = emerg_cb_mode_cb_func;
  si_ptr->autonam_change_cb_func  = autonam_change_cb_func;
  si_ptr->gw_net_list_cb_func     = gw_net_list_cb_func;
  si_ptr->sim_state_cb_func       = sim_state_cb_func;
  si_ptr->reg_done_cb_func        = reg_done_cb_func;
  si_ptr->data_suspend_cb_func    = data_suspend_cb_func;
  si_ptr->srv_lost_cb_func        = srv_lost_cb_func;
  si_ptr->full_srv_lost_cb_func   = full_srv_lost_cb_func;
  si_ptr->si_prl_load_complete_cb_func = si_prl_load_complete_cb_func;
  si_ptr->ss_stack_status_cb_func = ss_stack_status_cb_func;
  si_ptr->sd_generic_cb_func      = sd_generic_cb_func;
  si_ptr->cdma_bsr_stat_chgd_cb_func = cdma_bsr_stat_chgd_cb_func;
  si_ptr->cs_emerg_scan_fail_cb_func = cs_emerg_scan_fail_cb_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  si_ptr = sd_si_ptr(SD_SS_HYBR_HDR);

  /* Register HDR hybrid the callback functions.
  */
  si_ptr->si_info_cb_func         = si_info_cb_func;
  si_ptr->acq_fail_cb_func        = acq_fail_cb_func;
  si_ptr->ok_to_orig_cb_func      = ok_to_orig_cb_func;
  si_ptr->emerg_cb_mode_cb_func   = emerg_cb_mode_cb_func;
  si_ptr->autonam_change_cb_func  = autonam_change_cb_func;
  si_ptr->gw_net_list_cb_func     = gw_net_list_cb_func;
  si_ptr->sim_state_cb_func       = sim_state_cb_func;
  si_ptr->reg_done_cb_func        = reg_done_cb_func;
  si_ptr->data_suspend_cb_func    = data_suspend_cb_func;
  si_ptr->srv_lost_cb_func        = srv_lost_cb_func;
  si_ptr->full_srv_lost_cb_func   = full_srv_lost_cb_func;
  si_ptr->si_prl_load_complete_cb_func = si_prl_load_complete_cb_func;
  si_ptr->ss_stack_status_cb_func = ss_stack_status_cb_func;
  si_ptr->sd_generic_cb_func      = sd_generic_cb_func;
  si_ptr->cs_emerg_scan_fail_cb_func = cs_emerg_scan_fail_cb_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  si_ptr = sd_si_ptr(SD_SS_HYBR_2);

  /* Register HDR hybrid the callback functions.
  */
  si_ptr->si_info_cb_func         = si_info_cb_func;
  si_ptr->acq_fail_cb_func        = acq_fail_cb_func;
  si_ptr->ok_to_orig_cb_func      = ok_to_orig_cb_func;
  si_ptr->emerg_cb_mode_cb_func   = emerg_cb_mode_cb_func;
  si_ptr->autonam_change_cb_func  = autonam_change_cb_func;
  si_ptr->gw_net_list_cb_func     = gw_net_list_cb_func;
  si_ptr->sim_state_cb_func       = sim_state_cb_func;
  si_ptr->reg_done_cb_func        = reg_done_cb_func;
  si_ptr->data_suspend_cb_func    = data_suspend_cb_func;
  si_ptr->srv_lost_cb_func        = srv_lost_cb_func;
  si_ptr->full_srv_lost_cb_func   = full_srv_lost_cb_func;
  si_ptr->ss_stack_status_cb_func = ss_stack_status_cb_func;
  si_ptr->sd_generic_cb_func      = sd_generic_cb_func;
  si_ptr->cs_emerg_scan_fail_cb_func = cs_emerg_scan_fail_cb_func;

  si_ptr = sd_si_ptr(SD_SS_HYBR_3);

  /* Register HDR hybrid the callback functions.
  */
  si_ptr->si_info_cb_func         = si_info_cb_func;
  si_ptr->acq_fail_cb_func        = acq_fail_cb_func;
  si_ptr->ok_to_orig_cb_func      = ok_to_orig_cb_func;
  si_ptr->emerg_cb_mode_cb_func   = emerg_cb_mode_cb_func;
  si_ptr->autonam_change_cb_func  = autonam_change_cb_func;
  si_ptr->gw_net_list_cb_func     = gw_net_list_cb_func;
  si_ptr->sim_state_cb_func       = sim_state_cb_func;
  si_ptr->reg_done_cb_func        = reg_done_cb_func;
  si_ptr->data_suspend_cb_func    = data_suspend_cb_func;
  si_ptr->srv_lost_cb_func        = srv_lost_cb_func;
  si_ptr->full_srv_lost_cb_func   = full_srv_lost_cb_func;
  si_ptr->ss_stack_status_cb_func = ss_stack_status_cb_func;
  si_ptr->sd_generic_cb_func      = sd_generic_cb_func;
  si_ptr->cs_emerg_scan_fail_cb_func = cs_emerg_scan_fail_cb_func;


} /* sd_si_reg */

/*===========================================================================

FUNCTION sd_si_srv_off

DESCRIPTION
  Inform the System Indication component that the service indication should
  be turned off.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_srv_off(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */
        boolean                   is_not_true_srv_loss
        /* Indicates whether it is internal or actual service loss */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );
  sd_si_info_s_type     *si_info_ptr = &si_ptr->si_info;
  sdss_opr_mode_e_type  oprt_mode   = sdss_get_opr_mode(ss);

  #ifdef FEATURE_SD_LTE
  sys_extend_srv_info_e_type ext_srv_info = SYS_EXTEND_SRV_INFO_NONE;
  sys_lte_cs_capability_e_type lte_cs_capability =  SYS_LTE_CS_CAPABILITY_MAX;
  #endif
  #ifdef SD_USES_DB_SERVICES
  db_items_value_type   db_item;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If main system selection, update DB items.
  */
  if( ss == SD_SS_MAIN )
  {
    #ifdef SD_USES_DB_SERVICES
    db_item.no_svc    = TRUE;
    db_put( DB_NO_SVC, &db_item );

    db_item.roam      = DB_ROAM_OFF;
    db_put( DB_ROAM, &db_item );
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If service is already OFF, just return.
  */
  if( (oprt_mode != SDSS_OPR_MODE_PWR_DOWN) &&
      (si_info_ptr->srv_status == SYS_SRV_STATUS_NO_SRV ))
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset PLMN info upon power-down.
  */
  if(oprt_mode == SDSS_OPR_MODE_PWR_DOWN)
  {
    sys_plmn_undefine_plmn_id(&si_info_ptr->sid.plmn.plmn_id);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, service is not already OFF
  */

  /* Clear the SI_SYS.
  */
  sdsr_list_clr( si_ptr->si_sys );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the service status to NO_SRV.
  */
  if( TRUE == is_not_true_srv_loss)
  {
    si_info_ptr->srv_status    = SYS_SRV_STATUS_NO_SRV_INTERNAL;
  }
  else
  {
    si_info_ptr->srv_status    = SYS_SRV_STATUS_NO_SRV;
  }
  si_info_ptr->srv_acq_time  = 0;

  si_info_ptr->is_colloc     = FALSE;
  si_info_ptr->is_pn_assn    = FALSE;
  si_info_ptr->is_data_assn  = FALSE;

  if( si_info_ptr->srv_lost_time == 0 )
  {
    si_info_ptr->srv_lost_time = time_get_uptime_secs();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SS-Core srv_status.
  */
  sdss_update_srv_status( ss, SYS_SRV_STATUS_NO_SRV, SYS_SRV_DOMAIN_NO_SRV );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update eHRPD zone variable when we have service lost on HDR
  */
  if (ss == SD_SS_HYBR_HDR )
  {
    sdss_set_eHRPD_zone( si_info_ptr, ss );
  }

  /* Update LTE CS capability.This is needed, if limited LTE CS capability is
  ** updated by NAS along with limited service (CM_SERVICE_CNF)
  */
  #ifdef FEATURE_SD_LTE
  sdss_get_nas_addl_info(ss, &lte_cs_capability, &ext_srv_info);
  if (lte_cs_capability != SYS_LTE_CS_CAPABILITY_NONE)
  {
    si_info_ptr->lte_cs_capability = lte_cs_capability;
  }
   if (ext_srv_info != SYS_EXTEND_SRV_INFO_NONE)
  {
    si_info_ptr->extend_srv_info = ext_srv_info;
  }
  else if (si_info_ptr->extend_srv_info == SYS_EXTEND_SRV_INFO_REG_COMP_ACK_PENDING)
  {
    si_info_ptr->extend_srv_info = ext_srv_info;
  }

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* clear CDMA / HDR time info
  */
  si_info_ptr->time_info.sys_mode    = SYS_SYS_MODE_NO_SRV;

  if( sdss_get_camp_mode() != SD_SS_PREF_CAMP_MODE_ON )
  {
    si_info_ptr->reg_domain = SYS_REG_DOMAIN_NOT_APPLICABLE;
  }
  else
  {
    si_info_ptr->reg_domain = SYS_REG_DOMAIN_NO_SERVICE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Initialize CDMA/HDR mcc list */
  memset(&si_info_ptr->plmn_mcc_list, 0, sizeof(sys_plmn_mcc_list));

  if(ss == SD_SS_MAIN )
  {
    si_ptr->si_info.mode_info.cdma.base_id = 0;
  }

  /* Inform the registered client (e.g., CM) of the new service indicators.
  */
  SD_MSG_HIGH_2( "Service is OFF, SS=%d, srv_status =%d",ss,si_info_ptr->srv_status);

  sd_invoke_info_callback(ss);

} /* sd_si_srv_off */

/*===========================================================================

FUNCTION sd_si_resel_hybr_bsr_state_update

DESCRIPTION
  Notify the registered client (i.e., CM) of change in resel state. Currently
  only transitions from/to HYBR_BSR are indicated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void                      sd_si_resel_hybr_bsr_state_update(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR */

        boolean                   is_hybr_bsr
            /* If the new resel state is HYBR_BSR */
)
{
  sd_si_info_s_type     *si_info_ptr = &sd_si_ptr(ss)->si_info;;


  /* Indicate to CM only if HYBR_BSR state is toggled since last indication */
  if ( si_info_ptr->is_hybr_bsr_acq != is_hybr_bsr )
  {
    si_info_ptr->is_hybr_bsr_acq = is_hybr_bsr;


    SD_MSG_HIGH_1( "SCSVLTE/FSN: IS HYBR_BSR toggled,new flag: %d", is_hybr_bsr);
    sd_invoke_info_callback(ss);
  }
} /* sd_si_resel_hybr_bsr_state_update */


/*===========================================================================

FUNCTION sd_si_srv_off_until

DESCRIPTION
  Inform the System Indication component that the service indication should
  be turned off until the specified service update criteria is met.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_srv_off_until(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sd_si_update_e_type       update_criteria
            /* Service update criteria - indicate the minimum criteria for
            ** updating the service indicators.
            */
)
{
  sd_si_s_type          *si_ptr     = sd_si_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the service update criteria.
  */
  si_ptr->si_update_criteria = update_criteria;

  /* Set the service indicator to OFF.
  */
  sd_si_srv_off( ss, FALSE );

} /* sd_si_srv_off_until */


/*===========================================================================

FUNCTION sd_si_pwr_save

DESCRIPTION
  Inform the System Indication component that the service indication
  is pwr_save.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_srv_pwr_save(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type          *si_ptr     = NULL;
  sd_si_info_s_type     *si_info_ptr = &sd_si_ptr(ss)->si_info;;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  si_ptr = sd_si_ptr( ss );

  /* If power save status is shown while UE is in Limited service, it will lead to UI displaying
     * "Searching" status . So do not send power status while UE is camped on Limited service
     * and RLF recvery is going on */
  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  if((si_info_ptr->srv_status == SYS_SRV_STATUS_LIMITED) &&
     (sdss_is_wcdma_connected_mode(ss) == TRUE))
  {
    SD_MSG_HIGH_0( "WRLF: PWR SAVE ignored as SD is in WRLF recovery & in limited service");
    return;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* update the SD core
  */
  sdss_update_srv_status( ss, SYS_SRV_STATUS_PWR_SAVE, SYS_SRV_DOMAIN_NO_SRV );

  #ifdef FEATURE_MMODE_DUAL_SIM
  if( sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM)
      && !sdss_is_sxlte()
	&& si_info_ptr->srv_status == SYS_SRV_STATUS_LIMITED 
    && sdss_is_other_ss_in_srv(ss) != SD_SS_MAX)
  {
    SD_MSG_MED_1( "DSDS: PWR SAVE ignored, other sub in srv:%d", sdss_is_other_ss_in_srv(ss));
    return ;
  }
  #endif
  
  /* If service is already PWR_SAVE, just return.
  */
  if( si_info_ptr->srv_status == SYS_SRV_STATUS_PWR_SAVE )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, service is not already OFF
  */

  /* Clear the SI_SYS.
  */
  sdsr_list_clr( si_ptr->si_sys );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the fields.
  */
  si_info_ptr->is_colloc     = FALSE;
  si_info_ptr->is_pn_assn    = FALSE;
  si_info_ptr->is_data_assn  = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the srv_status to pwr_save.
  */
  si_info_ptr->srv_status = SYS_SRV_STATUS_PWR_SAVE;
  /* Set the domain prefernce to NO SERVICE */
  si_info_ptr->srv_domain = SYS_SRV_DOMAIN_NO_SRV;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* clear time info flag */
  si_info_ptr->time_info.sys_mode     = SYS_SYS_MODE_NO_SRV;

  if( sdss_get_camp_mode() != SD_SS_PREF_CAMP_MODE_ON )
  {
    si_info_ptr->reg_domain = SYS_REG_DOMAIN_NOT_APPLICABLE;
  }
  else
  {
    si_info_ptr->reg_domain = SYS_REG_DOMAIN_NO_SERVICE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_SD_LTE
  /* Update LTE IMS capability */
  si_info_ptr->lte_ims_emerg_avail = SYS_IMS_LTE_EMERG_SUPP_UNKNOWN;
  si_info_ptr->lte_ims_voice_avail = FALSE;
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* When HDR is going to power save afte LTE to DO IRAT failed due to no HDR 
   ** service possible, reset the IRAT flag */
  if(ss == SD_SS_HYBR_1 &&
     sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_HYBR_HDR_IRAT))
  {
    SD_MSG_MED_0("IRAT:reset flag in HDR pwr save");
    sdss_set_flag(SD_SS_MAIN, SDSS_FLAG_HYBR_HDR_IRAT, FALSE);
  }

  /* Inform the registered client (e.g., CM) of the new service indicators.
  */
  SD_MSG_HIGH_2( "Service is PWR_SAVE, SS=%d, srv_domain=%d",
                                                ss, si_info_ptr->srv_domain);
  sd_invoke_info_callback(ss);

} /* sd_si_srv_pwr_save*/


/*===========================================================================

FUNCTION sd_si_acq_fail

DESCRIPTION
  Notify the registered client (i.e., CM) that acquisition over all systems
  has failed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_acq_fail(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type                *si_ptr     = sd_si_ptr( ss );
  sd_ss_mode_pref_e_type       mode_pref = SD_SS_MODE_PREF_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Operator specific requirement */

  if ( (ss == SD_SS_MAIN) && sdss_get_operator_name ( SYS_MODEM_AS_ID_1 ) == SD_OPERATOR_CT )
  {
    sdss_set_operator_capability(ss,SD_SS_MODE_PREF_ANY);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( si_ptr->acq_fail_cb_func != NULL )
  {
    sdss_get_mode_band_pref(ss,&mode_pref,NULL,NULL,NULL);
    
    si_ptr->acq_fail_cb_func( ss, mode_pref);
  }

} /* sd_si_acq_fail */

/*===========================================================================

FUNCTION sd_si_cdma_lock_status

DESCRIPTION
  Notify the registered client (i.e., CM) about the CDMA lock status change

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void sd_si_cdma_lock_status_rpt(boolean is_cmda_locked)
{
  sd_si_s_type        *si_ptr     = sd_si_ptr(SD_SS_MAIN);
  sd_rpt_generic_s_type  cdma_lock_rpt;

  if(si_ptr == NULL)
  {
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cdma_lock_rpt.type = SD_GENERIC_CB_CDMA_LOCK_STATUS;
  cdma_lock_rpt.data.cdma_lock_rpt.is_cmda_locked = is_cmda_locked;
  if(si_ptr->sd_generic_cb_func != NULL)
  {
    /* Call to the sd_generic callback */
    SD_MSG_HIGH_1( "cdma lock status reporting to cm %d",is_cmda_locked);
    si_ptr->sd_generic_cb_func(&cdma_lock_rpt);
  }
} /* sd_si_cdma_lock_status_rpt */

/*===========================================================================

FUNCTION sd_si_hybr_bsr_to_hdr

DESCRIPTION
  Notify the CM to start or end lte to hdr hybrid bsr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_hybr_bsr_to_hdr(
        sd_ss_e_type              ss,
        sd_hybr_bsr_to_hdr_s_type  hybr_bsr_to_hdr_status )

{
  sd_si_s_type        *si_ptr     = sd_si_ptr(ss);
  sd_rpt_generic_s_type  hybr_bsr_to_hdr_rpt;

  if(si_ptr == NULL)
  {
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hybr_bsr_to_hdr_rpt.type = SD_GENERIC_CB_HYBR_BSR_TO_HDR;
  hybr_bsr_to_hdr_rpt.data.hybr_bsr_rpt.hybr_bsr_to_hdr_status = hybr_bsr_to_hdr_status;
  if(si_ptr->sd_generic_cb_func != NULL)
  {
    /* Call to the sd_generic callback */
    /* HYBR_BSR_TO_HDR indication should be sent only when MORE PRE HDR LST is valid */	
    if(sdsr_list_cnt(SDSR_MORE_PREF_HDR_LST) > 0)
    {
      SD_MSG_HIGH_2( "LTE_TO_HDR: hybr_bsr_to_hdr_status %d ss %d",hybr_bsr_to_hdr_status,ss);
      si_ptr->sd_generic_cb_func(&hybr_bsr_to_hdr_rpt);
    }
  }

} /* sd_si_hybr_bsr_to_do */

/*===========================================================================

FUNCTION sd_si_kick_hybr2

DESCRIPTION
  Notify the CM to kick hybr2 stack.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_kick_hybr2(
        sd_ss_e_type              ss
)

{
  sd_si_s_type        *si_ptr     = sd_si_ptr(ss);
  sd_rpt_generic_s_type  kick_hybr2_rpt;

  if(si_ptr == NULL)
  {
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  kick_hybr2_rpt.type = SD_GENERIC_CB_KICK_HYBR2;
  if(si_ptr->sd_generic_cb_func != NULL)
  {
    si_ptr->sd_generic_cb_func(&kick_hybr2_rpt);
  }

} /* sd_si_kick_hybr2 */
/*===========================================================================

FUNCTION sd_si_opt_sr_hdr_acq_fail

DESCRIPTION
  Notify the registered client (i.e., CM) that acquisition over current colloc systems
  in redial list has failed once.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_opt_sr_hdr_acq_fail()
{
  sd_si_s_type        *si_ptr     = sd_si_ptr(SD_SS_HYBR_1);
  sd_rpt_generic_s_type  hdr_acq_fair_rpt;

  if(si_ptr == NULL)
  {
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hdr_acq_fair_rpt.type = SD_GENERIC_CB_HDR_OPT_FAIL;
  if(si_ptr->sd_generic_cb_func != NULL)
  {
    /* Call to the sd_generic callback */
  SD_MSG_HIGH_0( "opt sr hdr acq fail");
    si_ptr->sd_generic_cb_func(&hdr_acq_fair_rpt);
  }

} /* sd_si_opt_sr_hdr_acq_fail */


/*===========================================================================

FUNCTION sd_si_srv_lost_cb

DESCRIPTION
  Notify the registered client (i.e., CM) that service is lost

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_srv_lost_cb(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  sd_si_s_type        *si_ptr     = sd_si_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( si_ptr->srv_lost_cb_func != NULL )
  {
    SD_MSG_HIGH_1( "SRV Lost ss=%d",ss);
    si_ptr->srv_lost_cb_func( ss );
  }

} /* sd_si_srv_lost_cb */

/*===========================================================================

FUNCTION sd_si_full_srv_lost_cb

DESCRIPTION
  Notify the registered client (i.e., CM) that full service is lost

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_full_srv_lost_cb(
  sd_ss_e_type              ss
      /* System selection - MAIN or HDR.
      */

)
{

  sd_si_s_type        *si_ptr     = sd_si_ptr( ss );
  if (si_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( si_ptr->full_srv_lost_cb_func != NULL )
  {
    si_ptr->full_srv_lost_cb_func( ss );
  }


} /* sd_si_full_srv_lost_cb */

/*===========================================================================

FUNCTION sdcmd_cdma_bsr_in_progress_cb

DESCRIPTION
  Notify the registered client (i.e., CM) that BSR status has changed

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sdcmd_cdma_bsr_in_progress_cb(boolean bsr_status)
{
  sd_si_s_type          *si_ptr     = NULL;
  static boolean     sd_bsr_status  = FALSE;
  si_ptr = sd_si_ptr( SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(( si_ptr->cdma_bsr_stat_chgd_cb_func != NULL ) &&
      sd_bsr_status != bsr_status)
  {
    SD_MSG_HIGH_1( "BSR status start/end:  %d",bsr_status);
    sd_bsr_status = bsr_status;
    si_ptr->cdma_bsr_stat_chgd_cb_func( bsr_status);
  }


}


/*===========================================================================

FUNCTION sd_si_autonam_change

DESCRIPTION
  Notify the registered client (e.g., CM) that auto-NAM change is
  appropriate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_autonam_change(

        sd_nam_e_type             nam
        /* The NAM that is proposed by the auto-NAM functionality.
        */
)
{
   sd_si_s_type        *si_ptr     = sd_si_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (e.g., CM) that auto-NAM change is
  ** appropriate.
  */
  if( si_ptr->autonam_change_cb_func != NULL )
  {
    SD_MSG_HIGH_1( "Auto-NAM change nam=%d",nam);
    si_ptr->autonam_change_cb_func( nam );
  }

} /* sd_si_autonam_change */

/*===========================================================================

FUNCTION sdss_report_mcc_to_cm

DESCRIPTION
  Report the MCC of the current system to CM.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void sdss_report_mcc_to_cm(
   sd_ss_e_type       ss,
   /* Storing the current stack which service is reported from */

   uint32             mcc,
   /* Storing the MCC related the found service */

   sys_plmn_id_s_type plmn,
   /* Storing the PLMN related to the found service */

   sd_mode_e_type     mode,
   /* Stores the mode related to found service */

   sys_srv_status_e_type  srv_status
   /* srv status for corresponding stack */
)
{
  sd_si_s_type *   si_ptr = sd_si_ptr(SD_SS_MAIN);
  sd_rpt_generic_s_type  mcc_rpt;

  /*-------------------------------------------------------------*/

  if(si_ptr == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------*/

  /* Set the report type and data */
  SD_MSG_HIGH_4("SD->CM: MMSOA: MCC =%d to CM, ss=%d, mode=0x%x srv_status=%d", mcc, ss, mode, srv_status);
  mcc_rpt.type = SD_GENERIC_CB_TYPE_MCC;
  mcc_rpt.data.mcc_rpt.ss = ss;
  mcc_rpt.data.mcc_rpt.mcc = mcc;
  mcc_rpt.data.mcc_rpt.plmn_id = plmn;
  mcc_rpt.data.mcc_rpt.mode = mode;
  mcc_rpt.data.mcc_rpt.srv_status = srv_status;

  /* Report to CM */

  if(si_ptr->sd_generic_cb_func != NULL)
  {

    /* Call to the sd_generic callback */
    si_ptr->sd_generic_cb_func(&mcc_rpt);
  }

  return;
}


#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH

/**===========================================================================

@FUNCTION sd_ss_set_sv_oprt_enable_switch

@DESCRIPTION
  Set the sdss sv_oprt_enabled flag to indicates if we enabled the feature

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  void

@SIDE EFFECTS
  None.

===========================================================================*/
void           sd_ss_set_sv_oprt_enable_switch(

 boolean       is_enabled
 /* Indicate if SV OPR dynamic switch is enabled */

)
{
  /* Set SD internal information */
  sdss_set_sv_dynamic_switch_enabled( is_enabled );
}

#endif


/*===========================================================================

FUNCTION sd_si_get_srv_status

DESCRIPTION
  Get srv status which was reported to client
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_srv_status_e_type                      sd_si_get_srv_status(
  sd_ss_e_type              ss
      /* System selection - MAIN or HDR.
      */

)
{

  return sd_si_ptr( ss )->si_info.srv_status;


} /* sd_si_full_srv_lost_cb */

/*===========================================================================

FUNCTION sd_ss_meas_is_done

DESCRIPTION
  Indicate whether most recent measurement request is done being serviced by
  Search.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if most recent measurement request is done being serviced by Search.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean          sd_ss_meas_is_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
)
{
  sd_ss_s_type           *ss_ptr     = sd_ss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ss_ptr->meas_is_done;

} /* sd_ss_meas_is_done */


/*===========================================================================

FUNCTION sd_ss_is_get_net_done

DESCRIPTION
  Indicate whether network list was received for the last get networks
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean          sd_ss_is_get_net_done(

        sd_ss_e_type     ss
          /* System selection - MAIN or HDR.
          */
)
{
  sd_ss_s_type           *ss_ptr     = sd_ss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ss_ptr->is_get_net_done;

} /* sd_ss_is_get_net_done */


/*===========================================================================

FUNCTION sd_ss_update_get_net_done

DESCRIPTION
  Update the get network done flag.
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if network list was received
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void             sd_ss_update_get_net_done(

        sd_ss_e_type     ss,
          /* System selection - MAIN or HDR */
        boolean          flag

)
{
  sd_ss_s_type           *ss_ptr     = sd_ss_ptr( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ss_ptr->is_get_net_done = flag;

} /* sd_ss_update_get_net_done */


/*===========================================================================

FUNCTION sd_ss_get_timer

DESCRIPTION
  Get the SD timer

EPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  dword                      sd_ss_get_timer(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
)
{
  SD_ASSERT( ss < SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the timer value and disable the SS-timer.
  */
  return (sd_ss_ptr(ss)->ss_timer);

}/* sd_ss_get_timer */


/*===========================================================================

FUNCTION sd_ss_hybr_2_act_get

DESCRIPTION
  Return the SS-Action that was most recently returned to the SS-Client
  other than CONTINUE.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_hybr_2_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  sd_ss_act_e_type act = SD_SS_ACT_MAX;

  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  SD_CRIT_SECT_ENTER();
  act = sd_ss_ptr(SD_SS_HYBR_2)->true_curr_act.act;
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = sd_ss_ptr(SD_SS_HYBR_2)->true_curr_act;
  }
  SD_CRIT_SECT_EXIT();
  return act;
} /* sd_ss_hybr_2_act_get */


/*===========================================================================

FUNCTION sd_ss_hybr_3_act_get

DESCRIPTION
  Return the SS-Action that was most recently returned to the SS-Client
  other than CONTINUE.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sd_ss_hybr_3_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
)
{

  sd_ss_act_e_type act = SD_SS_ACT_MAX;

  /* If caller supplied a buffer, copy the most recent SS-Action into
  ** into the supplied buffer.
  */
  SD_CRIT_SECT_ENTER();
  act = sd_ss_ptr(SD_SS_HYBR_3)->true_curr_act.act;
  if( rtrn_act_ptr != NULL )
  {
    *rtrn_act_ptr = sd_ss_ptr(SD_SS_HYBR_3)->true_curr_act;
  }
  SD_CRIT_SECT_EXIT();
  return act;
} /* sd_ss_hybr_3_act_get */

/*===========================================================================

FUNCTION sd_decode_3gpp2_mcc

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS 

===========================================================================*/
EXTERN void sd_decode_3gpp2_mcc(word *decoded_mcc, const word encoded_mcc)
{
  word d1, d2, d3, buf = encoded_mcc + 111;

  d3 = buf % 10;
  buf = ( d3 == 0 ) ? (buf-10)/10 : buf/10;

  d2 = buf % 10;
  buf = ( d2 == 0 ) ? (buf-10)/10 : buf/10;

  d1 = ( buf == 10 ) ? 0 : buf;

  *decoded_mcc = d1*100 + d2*10 + d3;
}

EXTERN void sd_decode_3gpp2_mnc(word *decoded_mnc, const byte encoded_mnc)
{
  word d11, d12, buf = encoded_mnc + 11;
    
  
  d12 = buf % 10;
  buf = ( d12 == 0 ) ? (buf-10)/10 : buf/10;

  d11 = ( buf == 10 ) ? 0 : buf;
   
  *decoded_mnc = d11*10 + d12;
}

/*===========================================================================

FUNCTION sd_filter_out_overlap_band

DESCRIPTION
  Get band preference complies to MS radio access capability.Clear PGSM band
  pref if EGSM band pref is present. Clear both EGSM and PGSM band pref if
  RGSM band pref is present.

  SEMANTIC RULE : Among the three Access Type Technologies GSM 900-P,
  GSM 900-E and GSM 900-R only one shall be  present.

  Refer to 3GPP TS 05.05 Frequency in MHz
             Mobile Tx     Base Station Tx
  PGSM       890-915       935-960
  EGSM       880-915       925-960
  RGSM       876-915       921-960

DEPENDENCIES
  None

RETURN VALUE
  band preference enum type

SIDE EFFECTS
  None.

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
EXTERN  sd_ss_band_pref_e_type    sd_filter_out_overlap_band(

        sd_ss_band_pref_e_type    band_pref
       /* Enumeration of system selection band and sub-band preferences.*/
)
{
  sd_ss_band_pref_e_type          return_band_pref = band_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 #ifdef FEATURE_RGSM_CAPABLE
  if( SD_BAND_CONTAIN( band_pref, SD_SS_BAND_PREF_GSM_RGSM_900) )
  {
    return_band_pref = SD_BAND_DELETE( return_band_pref,
                                       SD_SS_BAND_PREF_GSM_EGSM_900
                                     );
    return_band_pref = SD_BAND_DELETE( return_band_pref,
                                       SD_SS_BAND_PREF_GSM_PGSM_900
                                     );

  }
  else
 #else
    return_band_pref = SD_BAND_DELETE( return_band_pref,
                                       SD_SS_BAND_PREF_GSM_RGSM_900
                                     );
 #endif
  if( SD_BAND_CONTAIN( band_pref, SD_SS_BAND_PREF_GSM_EGSM_900) )
  {
    return_band_pref = SD_BAND_DELETE( return_band_pref,
                                       SD_SS_BAND_PREF_GSM_PGSM_900
                                     );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return return_band_pref;

} /* sd_filter_out_overlap_band */


#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION sd_si_emerg_cb_mode

DESCRIPTION
  Notify the registered client (e.g., CM) that SD entered  or exited
  emergency callback mode of operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sd_si_emerg_cb_mode(

        sd_emerg_cb_mode_e_type          emerg_cb_mode
            /* Callback mode - enter or exit
            */
)
{
  sd_si_s_type        *si_ptr     = sd_si_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (e.g., CM) that SD entered emergency
  ** callback mode of operation.
  */
  if( si_ptr->emerg_cb_mode_cb_func != NULL )
  {
    SD_MSG_HIGH_1( "Emergency cb mode %d",emerg_cb_mode);
    si_ptr->emerg_cb_mode_cb_func(emerg_cb_mode);
  }

} /* sd_si_emerg_cb_mode */


/*===========================================================================

FUNCTION sd_misc_get_curr_acq_sys_mode_ss

DESCRIPTION
   Returns the mode of system over which service is currently provided. If
   no service is currently provided, return the mode of the system that
   is being attempted acquisition.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
// TODO: Verify that this does not need to be executed as a command.
EXTERN  sys_sys_mode_e_type            sd_misc_get_curr_acq_sys_mode_ss
(
  sd_ss_e_type ss
)
{
  sd_si_info_s_type     *si_info_ptr = &sd_si_ptr(ss)->si_info;
  sdsr_s_type           *sr_ptr;
  sys_sys_mode_e_type    sys_mode     = SYS_SYS_MODE_CDMA;
  sd_mode_e_type         acq_sys_mode = SD_MODE_CDMA;

  sd_ss_mode_pref_e_type mode_capability = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type band_capability = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type band_capability_lte;
  sd_ss_band_pref_e_type band_capability_tds = SD_SS_BAND_PREF_NONE;
  sd_ss_mode_pref_e_type ss_true_mode_pref = SD_SS_MODE_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If service is currently being indicated, return the mode of this
  ** service, Else, return the mode of the ACQ_SYS.
  */
  if( sys_srv_status_is_srv( si_info_ptr->srv_status ) )
  {
    acq_sys_mode = si_info_ptr->mode;
  }
  else
  {
    sr_ptr = sdsr_list_get_ptr( sdss_sr_list_map2( ss, SDSR_ACQ_SYS), 0 );
    if ( sr_ptr != NULL )
    {
      acq_sys_mode = sr_ptr->sys.mode;
    }
    else
    {
      /* Not attempting acquisitions, use the mode_pref settings.
      */
      ss_true_mode_pref = sdss_get_true_mode_pref(ss);

      #if defined(FEATURE_ACP)
      if ( SD_MODE_CONTAIN( ss_true_mode_pref, BM( SD_MODE_AMPS )) )
      {
        acq_sys_mode = SD_MODE_AMPS;
      }
      #endif

      #if defined(SD_GSM_SUPPORTED)
      if ( SD_MODE_CONTAIN( ss_true_mode_pref, BM( (int)SD_MODE_GSM )) )
      {
        acq_sys_mode = SD_MODE_GSM;
      }
      #endif

      #if defined(SD_WCDMA_SUPPORTED)
      if ( SD_MODE_CONTAIN( ss_true_mode_pref, BM( (int)SD_MODE_WCDMA )) )
      {
        acq_sys_mode = SD_MODE_WCDMA;
      }
      #endif

      #if defined(FEATURE_HDR)
      if ( SD_MODE_CONTAIN( ss_true_mode_pref, BM( SD_MODE_HDR )) )
      {
       acq_sys_mode = SD_MODE_HDR;
      }
      #endif

      #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
      if ( SD_MODE_CONTAIN( ss_true_mode_pref, BM( SD_MODE_CDMA )) )
      {
       acq_sys_mode = SD_MODE_CDMA;
      }
      #endif

      #if defined(FEATURE_SD_LTE)
      if ( SD_MODE_CONTAIN(ss_true_mode_pref, BM( SD_MODE_LTE)))
      {
        acq_sys_mode = SD_MODE_LTE;
      }
      #endif

      #if defined(FEATURE_TDSCDMA)
      if ( SD_MODE_CONTAIN(ss_true_mode_pref, BM( SD_MODE_TDS)))
      {
        acq_sys_mode = SD_MODE_TDS;
      }
      #endif
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Map SD mode to SYS mode.
  */
  switch ( acq_sys_mode )
  {
    case SD_MODE_CDMA:
      sys_mode =  SYS_SYS_MODE_CDMA;
      break;

    case SD_MODE_AMPS:
      sys_mode =  SYS_SYS_MODE_AMPS;
      break;

    case SD_MODE_HDR:
      sys_mode =  SYS_SYS_MODE_HDR;
      break;

    case SD_MODE_GSM:
    case SD_MODE_GW:
      sys_mode =  SYS_SYS_MODE_GSM;
      break;

    case SD_MODE_WCDMA:
      sys_mode =  SYS_SYS_MODE_WCDMA;
      break;

    case SD_MODE_GWL:
      sdss_get_mode_band_capability_mmode(&mode_capability,
                                          &band_capability,
                                          &band_capability_lte,
                                          &band_capability_tds);
      if (mode_capability == SD_SS_MODE_PREF_LTE)
      {
        sys_mode = SYS_SYS_MODE_LTE;
      }
      else
      {
        sys_mode = SYS_SYS_MODE_GSM;
      }

      break;

    case SD_MODE_LTE:
      sys_mode = SYS_SYS_MODE_LTE;
      break;

    case SD_MODE_TDS:
      sys_mode = SYS_SYS_MODE_TDS;
      break;

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_MAX:
    default:
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sys_mode;

} /* sd_misc_get_curr_acq_sys_mode_ss */


/*=============================================================================*/
/*  internal functions moved from sd.c which began with _                                                                */
/*=============================================================================*/


/*===========================================================================

FUNCTION sdcmd_user_ss_pref7

DESCRIPTION

DEPENDENCIES

===========================================================================*/
sd_ss_act_e_type        sdcmd_user_ss_pref7(
  sd_ss_ind_user_ss_pref_cmd  *cmd
)
{
  sd_ss_s_type        *ss_ptr   = sd_ss_ptr(SD_SS_MAIN);
  sd_si_s_type        *si_ptr   = sd_si_ptr(SD_SS_MAIN);
  sd_ss_act_e_type    act = SD_SS_ACT_MAX;
  sd_ss_act_s_type    *prev_act_ptr = NULL;
  sd_ss_act_s_type    *curr_act_ptr = NULL;
  sdss_pref_updated_e_type is_pref_change;
  sdss_state_e_type   ss_state = sd_get_ss_state(SD_SS_MAIN);
  sd_ss_mode_pref_e_type sdss_mode_pref = SD_SS_MODE_PREF_NONE;
  sys_rat_pri_list_info_s_type  ss_main_rat_pri_list_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->pref_reason, SD_SS_PREF_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->orig_mode, SD_SS_ORIG_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->user_mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->band_pref_tds, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->prl_pref,  SD_SS_PRL_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->hybr_pref, SD_SS_HYBR_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->otasp_band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->otasp_blksys, SD_BLKSYS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->camp_mode, SD_SS_PREF_CAMP_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_HIGH_6( "main:pref_reason=%d, orig_mode=%d, mode_pref=0x%x,roam_pref=0x%x, hybr_pref=%d voice_domain_pref=%d",
                cmd->pref_reason, cmd->orig_mode, cmd->mode_pref,
                cmd->roam_pref, cmd->hybr_pref,cmd->voice_domain_pref );

  SD_MSG_HIGH_4("band_pref:(MSB)(LSB) cgw=0x%08x 0x%08x, TDS = 0x%08x 0x%08x",
               QWORD_HIGH(cmd->band_pref),
               QWORD_LOW(cmd->band_pref),
               QWORD_HIGH(cmd->band_pref_tds),
               QWORD_LOW(cmd->band_pref_tds));

  sd_print_lte_band_mask(cmd->band_pref_lte);
  SD_MSG_HIGH_5( "prl_pref 0x%x req Id %d camp_mode: %d, CSG_ID=%d, CSG_RAT=%d",
                cmd->prl_pref,
                cmd->req_id,
                cmd->camp_mode, 
                cmd->csg_id,
                cmd->csg_rat);

  if (cmd->rat_acq_order_ptr != NULL)
  {
    sdss_print_rat_acq_order(cmd->rat_acq_order_ptr);
  }

  /*Reset Scan Scope*/
  sdss_set_scan_scope_type(SD_SS_MAIN, SYS_SCAN_SCOPE_FULL_BAND);

  if (cmd->hybr_pref != SD_SS_HYBR_PREF_CDMA__LTE__HDR
      && sdsr_list_cnt (SDSR_MORE_PREF_HDR_LST))
  {
    sdsr_list_clr(SDSR_MORE_PREF_HDR_LST);
    SD_MSG_HIGH_0("Clearing the MORE_PREF_HDR_LIST as UE is in NON_SV config");
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  prev_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (prev_act_ptr != NULL)
  {
    memset(prev_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*  Update orig_mode if mode pref does not contain 3GPP RATs */
  cmd->orig_mode = sdss_compute_orig_mode(cmd->orig_mode, \
    sdss_get_supp_mode_pref_mmode(cmd->mode_pref,cmd->band_pref, cmd->band_pref_lte, cmd->band_pref_tds), \
    NULL);

  (void)sd_ss_act_get_true_curr_act(prev_act_ptr);

  #ifdef FEATURE_GPSONE_MSBASED
  /* If this is an optimized GPS related SS-Preference change, return without
  ** further processing.
  */
  if( sd_ss_pref_opti_gps( SD_SS_MAIN,
                           cmd->mode_pref,
                           cmd->orig_mode,
                           cmd->rtrn_act_ptr) )
  {
    modem_mem_free(prev_act_ptr, MODEM_MEM_CLIENT_MMODE);
    return sd_ss_act_get_true_curr_act(NULL);
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the hybr preference is toggled from none to on, set the start hybr
  ** hdr flag to TRUE.
  */
  if(  sdss_is_hybr_pref( SD_SS_MAIN, SD_SS_HYBR_PREF_NONE ) &&
       cmd->hybr_pref != SD_SS_HYBR_PREF_NONE
    )
  {
    SD_MSG_HIGH_0("Toggling on hybr pref");
    sd_ss_ptr(SD_SS_HYBR_HDR)->is_start_instance = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* RLF recovery is in progress and CS call is originated
  ** If last LTE system supported emergency continue with RLF.
  ** If not exit it and acquire srv on CS RATs
  */
  #ifdef FEATURE_LTE
  if ((((cmd->pref_reason == SD_SS_PREF_REAS_ORIG_START_CS) &&
      (cmd->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG )) ||
	  (!SD_MODE_CONTAIN( cmd->mode_pref, SD_SS_MODE_PREF_LTE))) &&
       sdss_is_lte_connected_mode (SD_SS_MAIN))
  {
    /* Emerg call is originated, reset the LTE connected state if last LTE
    ** system did not support emergency or mode pref for emergency call does not contain LTE
    */
    if( sdss_is_lte_ims_emerg_allowed() &&
        SD_MODE_CONTAIN(SD_SS_MODE_PREF_LTE, cmd->mode_pref) )
    {
      sdss_update_is_RLF_during_emerg( SD_SS_MAIN,TRUE);
      SD_MSG_HIGH_0(" continue with RLF recovery");
    }
    else
    {
      SD_MSG_HIGH_0("Emerg call orig-reset LTE conn mode or no LTE in mode pref ");
      sdss_update_is_RLF_during_emerg( SD_SS_MAIN, FALSE);
      sdss_check_reset_emerg_rat_pri_list_info(SD_SS_MAIN);
    }
  }

  if(!sdss_is_orig_mode0( cmd->orig_mode, SD_SS_ORIG_MODE_VAL_EMERG)
    && sdss_is_lte_connected_mode (SD_SS_MAIN)
    && (sdss_get_sim_state(SD_SS_MAIN)== SYS_SIM_STATE_PS_INVALID
        || sdss_get_sim_state(SD_SS_MAIN)== SYS_SIM_STATE_CS_PS_INVALID)
    )
  {
    SD_MSG_HIGH_1(" Resetting connected mode lte on %d", SD_SS_MAIN);
    sdss_reset_lte_connected_mode(SD_SS_MAIN);
  }
  
  /* If emergency call ends, reset RLF in emergency flag */
  if ( cmd->orig_mode != SD_SS_ORIG_MODE_EMERG_ORIG  &&
       sdss_get_is_RLF_during_emerg(SD_SS_MAIN) == TRUE )
  {
    sdss_update_is_RLF_during_emerg( SD_SS_MAIN,FALSE);
  }
  #endif /* FEATURE_LTE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdss_set_csg_data(SD_SS_MAIN, cmd->csg_id, cmd->csg_rat);

  /* Call on the SS-Core to update its SS-Preference setting.
  */
  is_pref_change = sdss_pref_update( cmd->pref_reason,
                                     cmd->orig_mode,
                                     cmd->mode_pref,
                                     cmd->band_pref,
                                     cmd->band_pref_lte,
                                     cmd->band_pref_tds,
                                     cmd->prl_pref,
                                     cmd->roam_pref,
                                     cmd->hybr_pref,
                                     cmd->otasp_band,
                                     cmd->otasp_blksys,
                                     cmd->domain_pref,
                                     cmd->acq_order_pref,
                                     cmd->pref_update_reason,
                                     cmd->user_mode_pref,
                                     cmd->rat_acq_order_ptr,
                                     cmd->camp_mode,
                                     cmd->voice_domain_pref
                                   );

   SD_MSG_HIGH_3("sd_ss_ind_user_ss_pref7 | setting csg data csg_id = %d, csg_rat = %d is_pref_change %d", 
                       cmd->csg_id, cmd->csg_rat,is_pref_change);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* We want to retain is_lte_scanned flag for one CSFB to SRLTE switch cycle only.
     bcoz scope of this flag is only when UE already scanned LTE in CSFB mode and 
     could not find LTE service, thus UE shall not scan LTE immediately(Silent BSR)
     in SRLTE sub-mode, Thus we want to reset is_lte_scanned flag as soon as SRLTE
     switch happens, because UE will move to SRLTE to CSFB mode only when LTE full
     service with voice option is available, thus there shall be no restriction to 
     scan LTE back in CSFB/SRLTE sub mode */

   /* check if LTE is allowed by the current preferences of the UE
   */
    sdss_get_rat_pri_list_info(SD_SS_MAIN, &ss_main_rat_pri_list_info);

    if( (sd_si_ptr(SD_SS_MAIN)->si_info.is_lte_scanned == TRUE) &&
         (sdss_get_rat_pri_sys_idx(SD_SS_MAIN,SD_MODE_LTE) ==
                                        ss_main_rat_pri_list_info.num_items))
    {
      /* LTE is not part of Main stack RAT Priority list, bst_flag check not required
         as it is always set, when acq_mode is present */
      sd_si_ptr(SD_SS_MAIN)->si_info.is_lte_scanned = FALSE;
      SD_MSG_HIGH_0("USER_SS_PREF: LTE not part of RPL, set Main is_lte_scanned to FALSE");
    }  

  /* Check if prl preference changed.
  */
  if ( (is_pref_change == SDSS_PREF_UPDATED_TRUE_PREF   ||
        is_pref_change == SDSS_PREF_UPDATED_SRV_DOMAIN) &&
       sdss_is_prl_pref_chgd( SD_SS_MAIN )
     )
  {
    /* Update the PRL_AVAIL status on the appropriate SYS.
    */
    if( ss_state == SDSS_STATE_OPR )
    {
      sdss_update_avail_sys_status( SD_SS_MAIN, SDSR_SRV_SYS );
    }
    else
    {
      sdss_update_avail_sys_status( SD_SS_MAIN, SDSR_ACQ_SYS );
    }

  }

  /* Reset CONNECTED mode status for emergency call orig,
  ** if FULL SRV is not needed during emergency orig.
  ** connecte dmode status will be reset once FULL SRV ACQ is completed.
  */
  sdss_get_mode_band_pref(SD_SS_MAIN,
                         &sdss_mode_pref,
                         NULL,
                         NULL,
                         NULL);
  if (!sdss_get_is_full_srv_req_during_emerg(SD_SS_MAIN) &&
      (((cmd->pref_reason == SD_SS_PREF_REAS_ORIG_START_CS) &&
      (cmd->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG )) ||
      (!SD_MODE_CONTAIN(sdss_mode_pref, SD_SS_MODE_PREF_LTE))) &&
      !sdss_get_is_RLF_during_emerg(SD_SS_MAIN) &&
      sdss_is_lte_connected_mode (SD_SS_MAIN) )
  {
    sdss_reset_lte_connected_mode(SD_SS_MAIN);
  }

  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  if (!sdss_get_is_full_srv_req_during_emerg(SD_SS_MAIN) &&
      (((cmd->pref_reason == SD_SS_PREF_REAS_ORIG_START_CS) &&
      (cmd->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG)) ||
      (!SD_MODE_CONTAIN( cmd->mode_pref, SD_SS_MODE_PREF_TDS_WCDMA))) &&
       sdss_is_wcdma_connected_mode (SD_SS_MAIN))
  {

    /* Emergency call is originated, reset the WCDMA connected state
    */
    sdss_reset_wcdma_connected_mode (SD_SS_MAIN);
  }
  #endif /* FEATURE_WRLF_SYSTEM_SEL */
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Preferences is a optimized ss pref change. just return CONTINUE.
  */
  if( sd_is_opti_ss_pref_change( SD_SS_MAIN,
                                 is_pref_change,
                                 cmd->pref_reason,
                                 cmd->orig_mode,
                                 cmd->pref_update_reason))
  {



    SD_MSG_HIGH_1("SS-Pref didn't change - CONTINUE ss_state=%d",ss_state);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-State is operation, inform the registered client (i.e., CM) that
    ** it is OK to originate as necessary.
    */
    if(ss_state == SDSS_STATE_OPR
#ifdef FEATURE_ECALL_IVS
  ||
      ((ss_state == SDSS_STATE_VRFY) && (ecall_get_ecall_operating_mode() ==  ECALL_MODE_ECALL_ONLY ) )
#endif
      )
    {
      sd_si_ok_to_orig( SD_SS_MAIN, si_ptr->data_suspend );
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* If IRAT flag is set and pref reason is STACK SYNC UP, clear it.
    */
    if( cmd->pref_reason == SD_SS_PREF_REAS_STACK_SYNC_UP &&
        sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_HYBR_HDR_IRAT))
    {
      SD_MSG_HIGH_0("IRAT:reset flag in ss pref - cont");
      sdss_set_flag(SD_SS_MAIN, SDSS_FLAG_HYBR_HDR_IRAT, FALSE);
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    modem_mem_free(prev_act_ptr, MODEM_MEM_CLIENT_MMODE);

    return SD_SS_ACT_CONTINUE;
  }

  
  if( cmd->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG && 
      sdss_is_flag_set(SD_SS_MAIN,SDSS_FLAG_OVERRIDE_E911_ADD_INFO) )
  {
      sdss_set_flag(SD_SS_MAIN,SDSS_FLAG_OVERRIDE_E911_ADD_INFO, FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, at least one of the SS-Preferences was changed or
  ** the SS-State is operation and origination mode is other than none.
  */

  /* If user wants to originate an OTASP call (i.e., orig_mode == OTASP),
  ** clear the OTASP system record list and append to it all the systems that
  ** are associated with the provisioning band-class and PCS frequency block/
  ** Cellular system.
  */
  if( cmd->pref_reason != SD_SS_PREF_REAS_AVOID_SYS &&
      cmd->orig_mode == SD_SS_ORIG_MODE_OTASP )
  {
    sdsr_list_clr( SDSR_OTASP_LST );
    (void) sdprl_blksys_app( SDSR_OTASP_LST, cmd->otasp_band, cmd->otasp_blksys );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if( cmd->pref_reason != SD_SS_PREF_REAS_AVOID_SYS &&
           ( cmd->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG  ||
             cmd->orig_mode == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG ||
             cmd->orig_mode == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG ) &&
           sdsr_list_cnt(SDSR_ALL_PREF_LST) == 0 )
  {
    sdprl_all_pref_sys_app( SDSR_ALL_PREF_LST );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if ( cmd->pref_reason == SD_SS_PREF_REAS_USER_RESEL &&
            sdss_is_orig_mode0( cmd->orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL) )
  {

    sd_update_manual_sys( SDSR_MANUAL_LST, cmd->manual_sys_info_ptr );

  } /* else if ( pref_reas == ... ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if ( cmd->pref_reason == SD_SS_PREF_REAS_AVOID_SYS)
  {

    /* Clear Avoid System List
     */
    if (cmd->avoid_type == SD_SS_CLR_AVOID_SYS_LIST)
    {
      /* Clear the system avoid list */
      sdss_sr_avoid_list_clear(SD_SS_MAIN, SDSR_TBL_LST);

      sdprl_clr_avoid_system_list();
      modem_mem_free(prev_act_ptr, MODEM_MEM_CLIENT_MMODE);
      return SD_SS_ACT_CONTINUE;
    }

    /* Avoid SID/WILDCARD_NID
    */
    else
    {
      if ( !sdprl_add_lock_sid_nid( sdsr_list_get_ptr(SDSR_IDL_SYS, 0 ), TRUE ))
      {
        SD_MSG_HIGH_0("SID/NID not locked, table full");
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Avoid band/chan for 60 seconds.
      */
        cmd->avoid_time = (dword)60;
	  
      sdss_sr_list_avoid2( SD_SS_MAIN, SDSR_IDL_SYS, cmd->avoid_time );
    }
  }

  #ifdef FEATURE_AVOID_DUP_BAND_SCAN
  /* pass emergency dup band to avoid info from hybrid-2 to main stack */
  if( (cmd->pref_reason == SD_SS_PREF_REAS_ORIG_START_CS) &&
       (cmd->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG) && 
       (sdss_is_e911_alternate_redial_algorithm(SD_SS_MAIN))
     )
  {
    sdss_copy_emerg_avoid_dup_band_info(SD_SS_MAIN, SD_SS_HYBR_2);
    /* reset hybrid-2 avoid dup band info as well. 
      ** this is required to make sure that before when preferences are being forced 
      ** for second call, before first call preferences are unforced, we are not copying the
      ** hybrid 2 avoid bands again*/
    sdss_reset_emerg_band_to_avoid(SD_SS_HYBR_2);
    
  }
  #endif /* FEATURE_AVOID_DUP_BAND_SCAN */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Applicable when UE needs to scan for 1x systems while registered on VOLTE. VOLTE to 1x
  ** fallback cases - silent redial, Wireless Priority Service calls(WPS - native 1x calls) 
  ** If 1XSIB8 feature NV is enabled and if the preference forced to SD is 
  ** during a call and if the service on main stack is LTE and if the 
  ** mode preference forced to SD doesn't have LTE,
  ** set the flag, 1XSIB8 scan flag to TRUE and reset the current loop number to 0.
  ** Otherwise, set the flag to FALSE
  */
  if(  sdss_is_1x_sib8_scan_opt_enabled() &&
       SD_IS_BIT_CONTAIN( BM(cmd->pref_reason),(uint32)SDSSSCR_PREF_REAS_VAL_CALL_ORIG) &&  
       sd_si_is_srv_on(SD_SS_MAIN) &&
       si_ptr->si_info.mode == SD_MODE_LTE && 
       (!SD_MODE_CONTAIN( cmd->mode_pref, SD_SS_MODE_PREF_LTE))   	   
    )
  {
    sdss_set_1xsib8_scan_allowed_flag(SD_SS_MAIN, TRUE);
    sdss_set_1xsib8_scan_loop_num(SD_SS_MAIN, 0);
  }
  else
  {
    sdss_set_1xsib8_scan_allowed_flag(SD_SS_MAIN, FALSE);
  }

  if( cmd->pref_update_reason == SD_SS_PREF_UPDATE_REASON_SWITCH_CSFB &&
  	 ( sdss_eng_stack_get_state(SD_SS_MAIN) == SDSS_ACQ_STT_REACQ_GWL))
  {
    sdss_set_flag(SD_SS_MAIN, SDSS_FLAG_SRV_TYPE_NON_MMSS, TRUE);
  }

  /* Process the SS-Event that is associated with this SS-Indication.
  ** and if no other event is processed already
  */
  if(act == SD_SS_ACT_MAX)
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_SS_PREF, cmd->rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE |
                    SD_SS_ACT_ACQ_GPS  ));

  /* Save the request id pertaining to sys sel pref as provided by client if the action is ACQ_GWL
  */
  if((SD_SS_ACT_ACQ_GWL == act || SD_SS_ACT_PWR_SAVE == act || SD_SS_ACT_CONTINUE == act) &&
    ((SD_SS_PREF_REAS_USER == cmd->pref_reason) || (SD_SS_PREF_REAS_USER_RESEL == cmd->pref_reason)))
  {
    ss_ptr->act.prm.acq_gwl.sys_sel_pref_req_id = cmd->req_id;
    ss_ptr->true_curr_act.prm.acq_gwl.sys_sel_pref_req_id = cmd->req_id;
    if(NULL != cmd->rtrn_act_ptr)
    {
      cmd->rtrn_act_ptr->prm.acq_gwl.sys_sel_pref_req_id = cmd->req_id;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdss_set_csg_non_rplmn_selection_type(SD_SS_MAIN, FALSE);

  curr_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (curr_act_ptr != NULL)
  {
    memset(curr_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  (void)sd_ss_act_get_true_curr_act(curr_act_ptr);

  if(SYS_CSG_ID_INVALID != cmd->csg_id)
  {

    /* CSG Selection is desired */
    if(NULL != cmd->manual_sys_info_ptr                                               &&
      TRUE == sys_plmn_id_is_valid(cmd->manual_sys_info_ptr->sys_id.id.plmn)          &&
      TRUE != sys_plmn_match(ss_ptr->rplmn, cmd->manual_sys_info_ptr->sys_id.id.plmn) &&
      act == SD_SS_ACT_ACQ_GWL)
    {
      /* This is case of non-RPLMN based CSG Selection
       * Hence, change the mode to manual_csg.
       */
      SD_MSG_HIGH_0( "non-RPLMN based CSG Selection");
      sd_rex_enter_crit_sect();

      if(NULL != cmd->rtrn_act_ptr)
      {
        cmd->rtrn_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
        cmd->rtrn_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
        cmd->rtrn_act_ptr->prm.acq_gwl.net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
        *(&(cmd->rtrn_act_ptr->prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
        cmd->rtrn_act_ptr->prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;
      }

      curr_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
      curr_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
      curr_act_ptr->prm.acq_gwl.net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
      curr_act_ptr->prm.acq_gwl.plmn = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
      curr_act_ptr->prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;

      sdss_set_csg_non_rplmn_selection_type (SD_SS_MAIN, TRUE);

      if(ss_ptr->true_curr_act.act == SD_SS_ACT_ACQ_GWL)
      {
        ss_ptr->true_curr_act.prm.acq_gwl.csg_id  = cmd->csg_id;
        ss_ptr->true_curr_act.prm.acq_gwl.csg_rat = cmd->csg_rat;
        ss_ptr->true_curr_act.prm.acq_gwl.net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
        *(&(ss_ptr->true_curr_act.prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
        ss_ptr->true_curr_act.prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;

        if(cmd->pref_reason == SD_SS_PREF_REAS_USER && cmd->orig_mode == SD_SS_ORIG_MODE_NONE)
        {  
           /* Set Non RPLMN CSG related Info */
           sdss_set_non_rplmn_csg_data(SD_SS_MAIN,
                                       cmd->csg_id,
                                       cmd->csg_rat, 
                                       SYS_NETWORK_SELECTION_MODE_MANUAL_CSG,
                                       cmd->manual_sys_info_ptr->sys_id.id.plmn,
                                       SD_MANUALLY_SELECTED_SPECIFIED_PLMN
                                       );
        }
      }

      sd_rex_leave_crit_sect();
    }
  else
  {
    /* RPLMN based case */
    SD_MSG_HIGH_0( "RPLMN based CSG Selection");

    sd_rex_enter_crit_sect();
    if(ss_ptr->true_curr_act.act == SD_SS_ACT_ACQ_GWL)
      {
        ss_ptr->true_curr_act.prm.acq_gwl.csg_id  = cmd->csg_id;
        ss_ptr->true_curr_act.prm.acq_gwl.csg_rat = cmd->csg_rat;
        ss_ptr->true_curr_act.prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN;
        *(&(ss_ptr->true_curr_act.prm.acq_gwl.plmn)) = *(&(ss_ptr->rplmn));

        if(cmd->pref_reason == SD_SS_PREF_REAS_USER && cmd->orig_mode == SD_SS_ORIG_MODE_NONE)
        {  

           /* Setting RPLMN related CSG info*/
           sdss_set_rplmn_csg_data(SD_SS_MAIN,
                                   cmd->csg_id, 
                                   cmd->csg_rat, 
                                   ss_ptr->rplmn, 
                                   SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN);
        }
      }

    if(NULL != cmd->rtrn_act_ptr)
    {
      cmd->rtrn_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
      cmd->rtrn_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
      cmd->rtrn_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN;
      *(&(cmd->rtrn_act_ptr->prm.acq_gwl.plmn)) = *(&(ss_ptr->rplmn));
    }

    curr_act_ptr->prm.acq_gwl.csg_id       = cmd->csg_id;
    curr_act_ptr->prm.acq_gwl.csg_rat      = cmd->csg_rat;
    curr_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN;
    *(&(curr_act_ptr->prm.acq_gwl.plmn)) = *(&(ss_ptr->rplmn));

    sd_rex_leave_crit_sect();
    }
  }
  /* Following else block is not related to CSG feature. On the other hand
     its for another feature called preferred PLMN+RAT selection */
  else if( cmd->csg_rat != SYS_RAT_NONE &&
           cmd->manual_sys_info_ptr != NULL &&
           sys_plmn_id_is_valid(cmd->manual_sys_info_ptr->sys_id.id.plmn) &&
           act == SD_SS_ACT_ACQ_GWL )
  {
    /* If preferred RAT info is passed, then attach it with ACQ_GWL
       Since this is one time only, don't need to remember in SD core state */
    if(cmd->rtrn_act_ptr != NULL)
    {
      cmd->rtrn_act_ptr->prm.acq_gwl.srv_req_type =
                                            SD_SS_SRV_REQ_TYPE_USER_PLMN_RAT;
      cmd->rtrn_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
      *(&(cmd->rtrn_act_ptr->prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
    }
    if(ss_ptr->true_curr_act.act == SD_SS_ACT_ACQ_GWL)
    {
      ss_ptr->true_curr_act.prm.acq_gwl.srv_req_type =
                                            SD_SS_SRV_REQ_TYPE_USER_PLMN_RAT;
      ss_ptr->true_curr_act.prm.acq_gwl.csg_rat = cmd->csg_rat;
      *(&(ss_ptr->true_curr_act.prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
    }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  /* Clear the action after forcing ss-pref */
  sdss_set_sv_opr_act( SDSS_SV_OPR_ACT_NONE );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If we have exited ECBM (prev_orig_mode=EMERG_CB and new_orig_mode!=EMERG_CB)
  ** and 1xcsfb_ecbm flag is set, reset the flag 
  */
  if(sdss_is_prev_orig_mode(SD_SS_MAIN, SD_SS_ORIG_MODE_VAL_EMERG_CB) &&
     !sdss_is_orig_mode(SD_SS_MAIN, SD_SS_ORIG_MODE_VAL_EMERG_CB) &&
     sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_1XCSFB_ECBM )
    )
  {
    sdss_set_flag(SD_SS_MAIN, SDSS_FLAG_1XCSFB_ECBM, FALSE );
    SD_MSG_HIGH_0("Reset 1XCSFB_ECBM flag as we exited ECBM ");
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  /* If action is equal to previous returned action, just continue - no need
  ** to instruct the SS-Client of the same action twice.
  */

  if( sd_ss_act_is_equal(curr_act_ptr, prev_act_ptr, ss_ptr->meas_list_is_diff, SD_SS_MAIN))
  {
    SD_MSG_HIGH_0( "Same action - CONTINUE");

    /* If SS-Action is MEAS_XXX, set the measurement ID per the pervious
    ** SS-Action's measurement ID.
    */
    if( curr_act_ptr->act == SD_SS_ACT_MEAS_BACK ||
        curr_act_ptr->act == SD_SS_ACT_MEAS_DED )
    {
      ss_ptr->meas_id = prev_act_ptr->prm.meas.id;
    }

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    act = SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-State is operation and SS-Action is CONTINUE or background
  ** measurement, inform the registered client (i.e., CM) that it is OK to
  ** originate as necessary.
  */
  if(( (ss_state == SDSS_STATE_OPR) &&
      (act == SD_SS_ACT_CONTINUE || act == SD_SS_ACT_MEAS_BACK) )
#ifdef FEATURE_ECALL_IVS
      ||
      ((ss_state == SDSS_STATE_VRFY) && (ecall_get_ecall_operating_mode() ==  ECALL_MODE_ECALL_ONLY)  && (act == SD_SS_ACT_CONTINUE) )
#endif
      )
  {
    sd_si_ok_to_orig( SD_SS_MAIN, si_ptr->data_suspend );
    #ifdef FEATURE_AVOID_DUP_BAND_SCAN
    /* in case of emergency call origination
    and alternate redial enabled, if action is continue, 
    then copy the srv system LTE band in emerg dup band info.
    this will be required to copy LTE bands already attempted upon 
    hybrid-2 stack to be avoided, if emergency call is shifted to 
    main stack after hard failure on hybrid-2 stack */
    if(act == SD_SS_ACT_CONTINUE && sdss_is_emergency_call_orig(SD_SS_MAIN) && 
      sdss_is_e911_alternate_redial_algorithm(SD_SS_MAIN))
    {
      sdss_add_last_acq_sys_band_to_emerg_dup_band_info(SD_SS_MAIN);
    }
    #endif /* FEATURE_AVOID_DUP_BAND_SCAN */
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If IRAT flag is set and pref reason is STACK SYNC UP, clear it.
  */
  if( cmd->pref_reason == SD_SS_PREF_REAS_STACK_SYNC_UP &&
      sdss_is_flag_set(SD_SS_MAIN, SDSS_FLAG_HYBR_HDR_IRAT))
  {
    SD_MSG_HIGH_0("IRAT:reset flag in ss pref");
    sdss_set_flag(SD_SS_MAIN, SDSS_FLAG_HYBR_HDR_IRAT, FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  modem_mem_free(prev_act_ptr, MODEM_MEM_CLIENT_MMODE);
  modem_mem_free(curr_act_ptr, MODEM_MEM_CLIENT_MMODE);

  return act;
} /* sd_ss_ind_user_ss_pref7 */


/*===========================================================================

FUNCTION sdcmd_meas_list_get3

DESCRIPTION

===========================================================================*/
boolean                 sdcmd_meas_list_get3(
  sd_ss_meas_list_cmd  *cmd
)
{

  sdsr_s_type            *sr_ptr = NULL;
    /* Pointer to sdsr_tbl entry */

  sys_lte_band_mask_e_type band_ptr_lte;
    /* Added to start using sd_misc_get_band_mode_mmode() instead of
       sd_misc_get_band_mode() which doesn't support LTE band pref */

  sd_ss_band_pref_e_type band_ptr_tds = SD_SS_BAND_PREF_ANY;
  /* Dummy variable */

  #if defined FEATURE_SD_LTE || defined FEATURE_GSM || defined FEATURE_WCDMA
  sys_rat_pri_list_info_s_type *temp_rat_pri_list_ptr;
    /* Store the a temporary rat_pri_list, which could contains all the
    ** RATs based on the supported mode / band / channels.
    ** However, for MEAS list, we remove all the RATs which does not expected
    ** to be scanned (having bst_rat_acq_required = 0). That will be our
    ** return value for *rat_pri_list_ptr.
    */
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_ss_mode_pref_e_type ss_mode_pref      = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type ss_band_pref      = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type ss_lte_band_pref;
  sd_ss_band_pref_e_type ss_tds_band_pref  = SD_SS_BAND_PREF_NONE;

  /* Fetch the preferences from SD_SS.
  */
  sdss_get_mode_band_pref( SD_SS_MAIN,
                           &ss_mode_pref,
                           &ss_band_pref,
                           &ss_lte_band_pref,
                           &ss_tds_band_pref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!(INRANGE(cmd->pos, 0, sdsr_list_cnt(SDSR_MEAS_LST))))
  {
    SD_ERR_1("Out of Range: pos %d",cmd->pos);
    return FALSE;
  }
  SD_ASSERT( cmd->rtrn_mode_ptr != NULL );
  SD_ASSERT( cmd->rtrn_band_ptr != NULL );
  SD_ASSERT( cmd->rtrn_chan_ptr != NULL );
  SD_ASSERT( cmd->rat_pri_list_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement list position resolved to an existing system, copy the
  ** system's mode, band and channel to the returned buffer and return TRUE.
  ** Else, return FALSE.
  */
  sr_ptr = sdsr_list_get_ptr( SDSR_MEAS_LST, cmd->pos );

  if( sr_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy mode, band and channel
  */
  *cmd->rtrn_mode_ptr = sdss_map_mode_to_mode_pref (sr_ptr->sys.mode);
  *cmd->rtrn_band_ptr = sr_ptr->sys.band;
  *cmd->rtrn_chan_ptr = sr_ptr->sys.chan;
  band_ptr_lte = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band);

  /* Filtering mode and band for only 3GPP systems
  */
  if (sd_is_3gpp_mode_pref(*cmd->rtrn_mode_ptr))
  {
    /* Filters out mode and band to comply with target and system preference
    */
    sd_misc_get_band_mode_mmode( cmd->rtrn_mode_ptr,
                                 cmd->rtrn_band_ptr,
                                 &band_ptr_lte,
                                 &band_ptr_tds,
                                 ss_mode_pref,
                                 ss_band_pref,
                                 ss_lte_band_pref,
                                 ss_tds_band_pref);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined FEATURE_SD_LTE || defined FEATURE_GSM || defined FEATURE_WCDMA

  temp_rat_pri_list_ptr = (sys_rat_pri_list_info_s_type *)modem_mem_alloc
                      (sizeof(sys_rat_pri_list_info_s_type), MODEM_MEM_CLIENT_MMODE);

  if ( temp_rat_pri_list_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* Update the RAT priority list sent to NAS based on the current system on which
  ** we are doing the mearsurement
  */
  sd_update_rat_pri_list( temp_rat_pri_list_ptr, sr_ptr, SD_SS_MAIN, SDSS_EVT_MISC_FILLER);

  /* Filter all the RAT that have bst_rat_acq_required = 0
  */
  sd_update_rat_pri_list_filter_meas_srch(temp_rat_pri_list_ptr, cmd->rat_pri_list_ptr);

  modem_mem_free(temp_rat_pri_list_ptr, MODEM_MEM_CLIENT_MMODE);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sdcmd_meas_list_get3 */


/*===========================================================================

FUNCTION sdcmd_hybr_2_meas_list_get2

DESCRIPTION

===========================================================================*/
boolean                   sdcmd_hybr_2_meas_list_get2(
  sd_ss_meas_list_cmd *cmd
)
{

  sdsr_s_type            *sr_ptr = NULL;
    /* Pointer to sdsr_tbl entry */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_ss_mode_pref_e_type ss_mode_pref      = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type ss_band_pref      = SD_SS_BAND_PREF_NONE;

  /* Fetch the preferences from SD_SS.
  */
  sdss_get_mode_band_pref( SD_SS_HYBR_2,
                           &ss_mode_pref,
                           &ss_band_pref,
                           NULL,
                           NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!(INRANGE(cmd->pos, 0, sdsr_list_cnt(sdss_sr_list_map2(SD_SS_HYBR_2,SDSR_MEAS_LST)))))
  {
    SD_ERR_1("Out of Range: pos %d",cmd->pos);
    return FALSE;
  }
  SD_ASSERT( cmd->rtrn_mode_ptr != NULL );
  SD_ASSERT( cmd->rtrn_band_ptr != NULL );
  SD_ASSERT( cmd->rtrn_chan_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement list position resolved to an existing system, copy the
  ** system's mode, band and channel to the returned buffer and return TRUE.
  ** Else, return FALSE.
  */
  sr_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(SD_SS_HYBR_2,SDSR_MEAS_LST), cmd->pos );

  if( sr_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy mode, band and channel
  */
  *cmd->rtrn_mode_ptr = sdss_map_mode_to_mode_pref (sr_ptr->sys.mode);
  *cmd->rtrn_band_ptr = sr_ptr->sys.band;
  *cmd->rtrn_chan_ptr = sr_ptr->sys.chan;

  #if defined FEATURE_SD_LTE || defined FEATURE_GSM || defined FEATURE_WCDMA

  /* Update the RAT priority list sent to NAS based on the current system on which
  ** we are doing the mearsurement
  */
  sd_update_rat_pri_list( cmd->rat_pri_list_ptr, sr_ptr, SD_SS_HYBR_2, SDSS_EVT_MISC_FILLER);
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filtering mode and band for only GW systems
  */
  if ( (*cmd->rtrn_mode_ptr == SD_SS_MODE_PREF_GW) ||
       (*cmd->rtrn_mode_ptr == SD_SS_MODE_PREF_GSM) ||
       (*cmd->rtrn_mode_ptr == SD_SS_MODE_PREF_WCDMA)
     )
  {
    /* Filters out mode and band to comply with target and system preference
    */
    sd_misc_get_band_mode( cmd->rtrn_mode_ptr,
                           cmd->rtrn_band_ptr,
                           ss_mode_pref,
                           ss_band_pref);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sd_ss_hybr_2_meas_list_get2 */

boolean                   sdcmd_hybr_3_meas_list_get(
  sd_ss_meas_list_cmd *cmd
)
{

  sdsr_s_type            *sr_ptr = NULL;
    /* Pointer to sdsr_tbl entry */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_ss_mode_pref_e_type ss_mode_pref      = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type ss_band_pref      = SD_SS_BAND_PREF_NONE;

  /* Fetch the preferences from SD_SS.
  */
  sdss_get_mode_band_pref( SD_SS_HYBR_3,
                           &ss_mode_pref,
                           &ss_band_pref,
                           NULL,
                           NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!( INRANGE(cmd->pos, 0, sdsr_list_cnt(sdss_sr_list_map2(SD_SS_HYBR_3,SDSR_MEAS_LST))) ))
  {
    SD_ERR_1("Out of Range: pos %d",cmd->pos);
    return FALSE;
  }
  SD_ASSERT( cmd->rtrn_mode_ptr != NULL );
  SD_ASSERT( cmd->rtrn_band_ptr != NULL );
  SD_ASSERT( cmd->rtrn_chan_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement list position resolved to an existing system, copy the
  ** system's mode, band and channel to the returned buffer and return TRUE.
  ** Else, return FALSE.
  */
  sr_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(SD_SS_HYBR_3,SDSR_MEAS_LST), cmd->pos );

  if( sr_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy mode, band and channel
  */
  *cmd->rtrn_mode_ptr = sdss_map_mode_to_mode_pref (sr_ptr->sys.mode);
  *cmd->rtrn_band_ptr = sr_ptr->sys.band;
  *cmd->rtrn_chan_ptr = sr_ptr->sys.chan;

  #if defined FEATURE_SD_LTE || defined FEATURE_GSM || defined FEATURE_WCDMA

  /* Update the RAT priority list sent to NAS based on the current system on which
  ** we are doing the mearsurement
  */
  sd_update_rat_pri_list( cmd->rat_pri_list_ptr, sr_ptr, SD_SS_HYBR_3,SDSS_EVT_MISC_FILLER );
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filtering mode and band for only GW systems
  */
  if ( (*cmd->rtrn_mode_ptr == SD_SS_MODE_PREF_GW) ||
       (*cmd->rtrn_mode_ptr == SD_SS_MODE_PREF_GSM) ||
       (*cmd->rtrn_mode_ptr == SD_SS_MODE_PREF_WCDMA)
     )
  {
    /* Filters out mode and band to comply with target and system preference
    */
    sd_misc_get_band_mode( cmd->rtrn_mode_ptr,
                           cmd->rtrn_band_ptr,
                           ss_mode_pref,
                           ss_band_pref);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sd_ss_hybr_3_meas_list_get */

/*===========================================================================

FUNCTION sdcmd_meas_list_rprt

DESCRIPTION

===========================================================================*/
boolean                   sdcmd_meas_list_rprt(
  sd_ss_meas_list_rprt_cmd  *cmd
)
{
  sdsr_s_type            *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!( INRANGE(cmd->pos, 0, sdsr_list_cnt(SDSR_MEAS_LST)) ))
  {
    SD_ERR_1("Out of Range: pos %d",cmd->pos);
    return FALSE;
  }
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement list position resolved to an existing system record,
  ** copy the reported measurement value into the meas_val field of the
  ** system record and return TRUE. Else, return FALSE.
  */
  sr_ptr = sdsr_list_get_ptr( SDSR_MEAS_LST, cmd->pos );


  if( sr_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Measurement report for GW systems contains number of PLMNs found
  ** as part of background service search. This number can be either 0
  ** or positive and should not be made negative.
  ** Measurement value is negative only for CDMA.
  */
  if ( (sr_ptr->sys.mode == SD_MODE_CDMA)
       &&
       (cmd->meas_val > 0)
     )
  {
    cmd->meas_val = -cmd->meas_val;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_3( "Meas report=%d, pos=%d, chan=%d",
                 cmd->meas_val, cmd->pos, sr_ptr->sys.chan );

  sr_ptr->meas_val = cmd->meas_val;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sd_ss_meas_list_rprt */


/*===========================================================================

FUNCTION sdcmd_hybr_2_meas_list_rprt

DESCRIPTION

===========================================================================*/
boolean                   sdcmd_hybr_2_meas_list_rprt(
  sd_ss_meas_list_rprt_cmd  *cmd
)
{
  sdsr_s_type            *sr_ptr;
  sdsr_e_type             meas_lst = sdss_sr_list_map2(SD_SS_HYBR_2, SDSR_MEAS_LST);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!( INRANGE(cmd->pos, 0, sdsr_list_cnt(meas_lst)) ))
  {
    SD_ERR_1("Out of Range: pos %d",cmd->pos);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement list position resolved to an existing system record,
  ** copy the reported measurement value into the meas_val field of the
  ** system record and return TRUE. Else, return FALSE.
  */
  sr_ptr = sdsr_list_get_ptr( meas_lst, cmd->pos );


  if( sr_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Measurement report for GW systems contains number of PLMNs found
  ** as part of background service search. This number can be either 0
  ** or positive and should not be made negative.
  ** Measurement value is negative only for CDMA.
  */
  if ( (sr_ptr->sys.mode == SD_MODE_CDMA)
       &&
       (cmd->meas_val > 0)
     )
  {
    cmd->meas_val = -cmd->meas_val;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_3( "Meas report=%d, pos=%d, chan=%d",
                 cmd->meas_val, cmd->pos, sr_ptr->sys.chan );

  sr_ptr->meas_val = cmd->meas_val;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sd_ss_hybr_2_meas_list_rprt */

boolean                   sdcmd_hybr_3_meas_list_rprt(
  sd_ss_meas_list_rprt_cmd  *cmd
)
{
  sdsr_s_type            *sr_ptr;
  sdsr_e_type             meas_lst = sdss_sr_list_map2(SD_SS_HYBR_3, SDSR_MEAS_LST);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!( INRANGE(cmd->pos, 0, sdsr_list_cnt(meas_lst)) ))
  {
    SD_ERR_1("Out of Range: pos %d",cmd->pos);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement list position resolved to an existing system record,
  ** copy the reported measurement value into the meas_val field of the
  ** system record and return TRUE. Else, return FALSE.
  */
  sr_ptr = sdsr_list_get_ptr( meas_lst, cmd->pos );


  if( sr_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Measurement report for GW systems contains number of PLMNs found
  ** as part of background service search. This number can be either 0
  ** or positive and should not be made negative.
  ** Measurement value is negative only for CDMA.
  */
  if ( (sr_ptr->sys.mode == SD_MODE_CDMA)
       &&
       (cmd->meas_val > 0)
     )
  {
    cmd->meas_val = -cmd->meas_val;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_3( "Meas report=%d, pos=%d, chan=%d",
                 cmd->meas_val, cmd->pos, sr_ptr->sys.chan );

  sr_ptr->meas_val = cmd->meas_val;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sd_ss_hybr_2_meas_list_rprt */

/*===========================================================================

FUNCTION sdcmd_user_pwr_up

DESCRIPTION

DEPENDENCIES

===========================================================================*/
sd_ss_act_e_type sdcmd_user_pwr_up(
  sd_ss_ind_user_pwr_up_cmd *cmd
)
{
  #ifdef FEATURE_HYBR_3GPP
  return sdcmd_hybr_2_user_pwr_up(cmd);
  #elif defined(CFG_HYBR_GW_3)
  return sdcmd_hybr_3_user_pwr_up(cmd);
  #else
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If locked and needs to unlock, do it now
  */
  if( cmd->is_cdma_unlock )
  {
    if(sdss_is_cdma_locked(SD_SS_MAIN))
    {
      /* The CDMA lock status is required to propagated to CM client/AP. 
          When modem enters online, SD reset the state. It need to report to CM. 
          All the other case 1X will sends CM_LOCK_F or CM_UNLOCK_F */      
      sd_si_cdma_lock_status_rpt(FALSE);
    }
    sdss_cdma_lock_update( SD_SS_MAIN, FALSE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( SD_SS_MAIN,SDSS_OPR_MODE_ONLINE );
  /* MRU_LST was loaded from RAM memory during NAM sel.
  Now since there is no NAM_sel at LPM->ONLINE and possibly RAM memory can be
  updated in LPM. So loading afresh from RAM if it was updated during LPM */
  if(sdprl_is_mru_updated_in_offline())
  {
    sdprl_copy_active_nam_mru_to_lists();
  }
  sdsr_list_print(SDSR_MRU_LST,FALSE);

  if(sdss_is_ral_stored_in_efs(SD_SS_MAIN))
  {
    /* add records from EFS RAL to SR table */
    sdprl_ral_cpy(SDSR_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_MAIN));
    sdsr_list_print(SDSR_ACQED_LST,FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_JCDMA
  /* If MRU is not empty, update the service indicators per MRU[0]. Else
  ** update the service indicators per PRL[0]
  ** Only if
  */
  if( sdsr_list_cnt(SDSR_MRU_LST) > 0 )
  {
    if(sdsr_list_find_ref(SDSR_GWL_LST, sdsr_list_get_ref(SDSR_MRU_LST,0)) == SDSR_POS_NULL)
    {
      sd_si_update_if_srv_is_off( SD_SS_MAIN,
                                  SD_SI_UPDATE_ACQ,
                                  SYS_SRV_STATUS_SRV,
                                  SYS_SRV_DOMAIN_CS_PS,
                                  sdsr_list_get_ref(SDSR_MRU_LST, 0),
                                  NULL,
                                  NULL,
                                  NULL,
                                  SDSR_MAX,
                                  NULL,
                                  NULL,
                                  NULL,
                                  SYS_SRV_STATUS_SRV,
                                  SD_SIB8_NO_CHANGE,
                                  SYS_EXTEND_SRV_INFO_NONE,
                                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                                  NULL
                                  );
    }
  }
  else
  {
    if(sdsr_list_find_ref(SDSR_GWL_LST, sdsr_list_get_ref(SDSR_PRL_LST,0)) == SDSR_POS_NULL)
    {
      sd_si_update_if_srv_is_off( SD_SS_MAIN,
                                  SD_SI_UPDATE_ACQ,
                                  SYS_SRV_STATUS_SRV,
                                  SYS_SRV_DOMAIN_CS_PS,
                                  sdsr_list_get_ref(SDSR_PRL_LST, 0),
                                  NULL,
                                  NULL,
                                  NULL,
                                  SDSR_MAX,
                                  NULL,
                                  NULL,
                                  NULL,
                                  SYS_SRV_STATUS_SRV,
                                  SD_SIB8_NO_CHANGE,
                                  SYS_EXTEND_SRV_INFO_NONE,
                                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                                  NULL
                                  );
    }
  }
  #endif /* FEATURE_JCDMA */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* At power up: reset the CSG DATA.
   * Currently there is no requirement for power up CSG Selection.
   */
  sdss_set_csg_data(SD_SS_MAIN, SYS_CSG_ID_INVALID, SYS_RAT_NONE);

  /* Reset is_in_srv_lost_state upon pwr_up
  */
  sdss_reset_srv_lost_state(SD_SS_MAIN);

  sdss_lte_telescope_parm_reset(SD_SS_MAIN);
  sdss_lte_telescope_parm_reset(SD_SS_HYBR_2);
  sdss_1x_telescope_parm_reset();

  /* reload EFS/NV since nam_sel will not be done during LPM->ONLINE */
  if(sdss_is_c2k_alt_algr_operation())
  {
    /* For ALT_CDMA_BSR feature,
    ** Need to re-read sdssscr configuration items upon going online
    ** BSR timers could be updated through OTA
    */
    sdssscr_efs_items_init();
    sdssscr_nv_items_init();
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_PWR_UP, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
  #endif //CFG_HYBR_GW

} /* sd_ss_ind_user_pwr_up */


/*===========================================================================

FUNCTION sdcmd_hybr_2_user_pwr_up

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type
    sdcmd_hybr_2_user_pwr_up( sd_ss_ind_user_pwr_up_cmd *cmd)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
    sys_err_fatal_invalid_value_exception((int)SYS_OVERALL_FEATURE_MODE_NORMAL);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( SD_SS_HYBR_2,SDSS_OPR_MODE_ONLINE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* At power up: reset the CSG DATA.
   * Currently there is no requirement for power up CSG Selection.
   */
  sdss_set_csg_data(SD_SS_HYBR_2, SYS_CSG_ID_INVALID, SYS_RAT_NONE);

  /* Reset is_in_srv_lost_state upon pwr_up
  */
  sdss_reset_srv_lost_state(SD_SS_HYBR_2);
  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) &&
    sdss_is_ral_stored_in_efs(SD_SS_HYBR_2))
  {
    sdprl_ral_cpy(SDSR_HYBR_1_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_HYBR_2));
    sdsr_list_print(SDSR_HYBR_1_ACQED_LST,FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_2,  SDSS_EVT_HYBR_2_USER_PWR_UP, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
} /* sd_ss_ind_hybr_2_user_pwr_up */

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_ss_act_e_type
      sdcmd_hybr_3_user_pwr_up( sd_ss_ind_user_pwr_up_cmd *cmd)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
    sys_err_fatal_invalid_value_exception((int)SYS_OVERALL_FEATURE_MODE_NORMAL);
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( SD_SS_HYBR_3,SDSS_OPR_MODE_ONLINE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* At power up: reset the CSG DATA.
   * Currently there is no requirement for power up CSG Selection.
   */
  
  sdss_set_csg_data(SD_SS_HYBR_3, SYS_CSG_ID_INVALID, SYS_RAT_NONE);

  /* Reset is_in_srv_lost_state upon pwr_up
  */
  sdss_reset_srv_lost_state(SD_SS_HYBR_3);
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(sdss_is_sxlte() && sdss_is_ral_stored_in_efs(SD_SS_HYBR_3))
  {
    sdprl_ral_cpy(SDSR_HYBR_3_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_HYBR_3));
    sdsr_list_print(SDSR_HYBR_3_ACQED_LST,FALSE);
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_3,  SDSS_EVT_HYBR_3_USER_PWR_UP, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
} /* sd_ss_ind_hybr_3_user_pwr_up */
#endif /* defined FEATURE_MMODE_TRIPLE_SIM */


/*===========================================================================

FUNCTION sdcmd_user_offline_cdma

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_user_offline_cdma(
  sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update( SD_SS_MAIN,SDSS_OPR_MODE_OFFLINE_CDMA );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the MS is going offline-
  ** CDMA.
  */
  sd_si_srv_offline( SDSS_OPR_MODE_OFFLINE_CDMA );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save PRL-Component related information to NV.
  */
  sdprl_save();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset is_in_srv_lost_state upon offline
  */
  sdss_reset_srv_lost_state(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_OFFLINE_CDMA, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_offline_cdma */


/*===========================================================================

FUNCTION sdcmd_hybr_2_user_prot_deactivate

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hybr_2_user_prot_deactivate(
  sd_ss_ind_user_prot_deactivate_cmd  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
    sys_err_fatal_invalid_value_exception((int)SYS_OVERALL_FEATURE_MODE_NORMAL);
  }

  /* Inform the System Indication component that the SS-Main protocol is
  ** being deactivated.
  */
  sd_si_srv_off( SD_SS_HYBR_2, FALSE );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_2,  SDSS_EVT_HYBR_2_USER_PROT_DEACTIVATE, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmd->adtnl_info == SD_SS_PROT_DEACT_INFO_SUBS_CHANGE_TRANS)
   {
   sd_ss_s_type    *ss_ptr     = sd_ss_ptr(SD_SS_HYBR_2);

   if(cmd->rtrn_act_ptr)
   {
     cmd->rtrn_act_ptr->prm.pwr_save.reas = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
   }
   ss_ptr->true_curr_act.prm.pwr_save.reas = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
   ss_ptr->act.prm.pwr_save.reas       = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
   }

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_PWR_SAVE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_prot_deactivate */

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_ss_act_e_type          sdcmd_hybr_3_user_prot_deactivate(
  sd_ss_ind_user_prot_deactivate_cmd  *cmd
)
{
  sd_ss_act_e_type    act;

  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
    sys_err_fatal_invalid_value_exception((int)SYS_OVERALL_FEATURE_MODE_NORMAL);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the SS-Main protocol is
  ** being deactivated.
  */
  sd_si_srv_off( SD_SS_HYBR_3, FALSE );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_3,  SDSS_EVT_HYBR_3_USER_PROT_DEACTIVATE, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if(cmd->adtnl_info == SD_SS_PROT_DEACT_INFO_SUBS_CHANGE_TRANS)
   {
	 sd_ss_s_type		 *ss_ptr		 = sd_ss_ptr(SD_SS_HYBR_3);
  
	 if(cmd->rtrn_act_ptr)
	 {
	   cmd->rtrn_act_ptr->prm.pwr_save.reas = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
	 }
	 ss_ptr->true_curr_act.prm.pwr_save.reas = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
	 ss_ptr->act.prm.pwr_save.reas			 = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
   }
  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_PWR_SAVE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sdcmd_hybr_3_user_prot_deactivate */
#endif /* defined(FEATURE_MMODE_TRIPLE_SIM) */


/*===========================================================================

FUNCTION sdcmd_user_prot_deactivate

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_user_prot_deactivate(
  sd_ss_ind_user_prot_deactivate_cmd  *cmd
)
{
  #ifdef FEATURE_HYBR_3GPP
  return sdcmd_hybr_2_user_prot_deactivate(cmd);
  #elif defined(CFG_HYBR_GW_3)
  return sdcmd_hybr_3_user_prot_deactivate(cmd);
  #else
  sd_ss_act_e_type    act;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the SS-Main protocol is
  ** being deactivated.
  */
  sd_si_srv_off( SD_SS_MAIN, FALSE );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_PROT_DEACTIVATE, cmd->rtrn_act_ptr );

  if(cmd->adtnl_info == SD_SS_PROT_DEACT_INFO_SUBS_CHANGE_TRANS)
  {
    sd_ss_s_type        *ss_ptr         = sd_ss_ptr(SD_SS_MAIN);

    if(cmd->rtrn_act_ptr)
    {
      cmd->rtrn_act_ptr->prm.pwr_save.reas = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
    }
    ss_ptr->true_curr_act.prm.pwr_save.reas = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
    ss_ptr->act.prm.pwr_save.reas           = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_PWR_SAVE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
  #endif

} /* sd_ss_ind_user_prot_deactivate */


/*===========================================================================

FUNCTION sdcmd_hybr_2_user_pwr_save_exit

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hybr_2_user_pwr_save_exit(
  sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
    sys_err_fatal_invalid_value_exception((int)SYS_OVERALL_FEATURE_MODE_NORMAL);
  }

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_2,  SDSS_EVT_HYBR_2_USER_PWR_SAVE_EXIT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_GWL |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sdcmd_hybr_2_user_pwr_save_exit */

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
sd_ss_act_e_type          sdcmd_hybr_3_user_pwr_save_exit(
  sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
    sys_err_fatal_invalid_value_exception((int)SYS_OVERALL_FEATURE_MODE_NORMAL);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_3,  SDSS_EVT_HYBR_3_USER_PWR_SAVE_EXIT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_GWL |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sdcmd_hybr_3_user_pwr_save_exit */
#endif /*defined(FEATURE_MMODE_TRIPLE_SIM) */


/*===========================================================================

FUNCTION sdcmd_user_pwr_save_exit

DESCRIPTION

DEPENDENCIES

===========================================================================*/
sd_ss_act_e_type          sdcmd_user_pwr_save_exit(
  sd_cmd_t  *cmd
)
{
  #ifdef FEATURE_HYBR_3GPP
  return sdcmd_hybr_2_user_pwr_save_exit(cmd);
  #elif defined(CFG_HYBR_GW_3)
  return sdcmd_hybr_3_user_pwr_save_exit(cmd);
  #else
  sd_ss_act_e_type    act;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_PWR_SAVE_EXIT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
  #endif

} /* sd_ss_ind_user_pwr_save_exit */


/*===========================================================================

FUNCTION sdcmd_hybr_2_user_ss_pref5

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type    sdcmd_hybr_2_user_ss_pref5( sd_ss_ind_user_ss_pref_cmd  *cmd)
{

  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr         = sd_ss_ptr(SD_SS_HYBR_2);
  sd_ss_act_s_type    *prev_act_ptr   = NULL;
  sd_ss_act_s_type    *curr_act_ptr   = NULL;
  sd_si_s_type        *si_ptr         = sd_si_ptr(SD_SS_HYBR_2);
  sdss_pref_updated_e_type is_pref_change;
  sdss_state_e_type   ss_state = sd_get_ss_state(SD_SS_HYBR_2);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->pref_reason, SD_SS_PREF_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->orig_mode, SD_SS_ORIG_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->prl_pref,  SD_SS_PRL_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->hybr_pref, SD_SS_HYBR_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->otasp_band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->otasp_blksys, SD_BLKSYS_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_HIGH_6( "hybr2:pref_reason=%d, orig_mode=%d, mode_pref=0x%x,roam_pref=0x%x, hybr_pref=%d voice_domain_pref=%d",
                cmd->pref_reason, cmd->orig_mode, cmd->mode_pref,cmd->roam_pref, cmd->hybr_pref,cmd->voice_domain_pref);
  SD_MSG_HIGH_4( "band_pref:(MSB)(LSB) cgw=0x%08x 0x%08x, TDS = 0x%08x 0x%08x",
               QWORD_HIGH(cmd->band_pref),
               QWORD_LOW(cmd->band_pref),
               QWORD_HIGH(cmd->band_pref_tds),
               QWORD_LOW(cmd->band_pref_tds));

  sd_print_lte_band_mask(cmd->band_pref_lte);
                
  SD_MSG_HIGH_3( "domain_pref=0x%x, acq_order_pref=0x%x  prl_pref 0x%x",
                cmd->domain_pref, cmd->acq_order_pref, cmd->prl_pref );
  SD_MSG_HIGH_4( "hybr2:sys_sel_pref_req_id = %u, CSG ID = %d, CSG RAT =%d,user_mode_pref: %d",
                cmd->req_id, cmd->csg_id, cmd->csg_rat,cmd->user_mode_pref);

  if (cmd->rat_acq_order_ptr != NULL)
  {
    sdss_print_rat_acq_order(cmd->rat_acq_order_ptr);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
    sys_err_fatal_invalid_value_exception((int)SYS_OVERALL_FEATURE_MODE_NORMAL);
  }
  prev_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (prev_act_ptr != NULL)
  {
    memset(prev_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*Reset Scan Scope*/
  sdss_set_scan_scope_type(SD_SS_HYBR_2, SYS_SCAN_SCOPE_FULL_BAND);

  (void)sd_ss_hybr_2_act_get_true_curr_act(prev_act_ptr, SD_SS_HYBR_2);

  #ifdef FEATURE_GPSONE_MSBASED
  /* If this is an optimized GPS related SS-Preference change, return without
  ** further processing.
  */
  if( sd_ss_pref_opti_gps( SD_SS_HYBR_2,
                           cmd->mode_pref,
                           cmd->orig_mode,
                           cmd->rtrn_act_ptr) )
  {
    modem_mem_free(prev_act_ptr,MODEM_MEM_CLIENT_MMODE);
    return sd_ss_hybr_2_act_get_true_curr_act(NULL , SD_SS_HYBR_2);
  }
  #endif

  /* Only when in SVLTE feature mode, e911 state of HYBR2 is checked for 
     Skip */
  if(sdss_is_1x_sxlte())
  {
    if(sdss_is_emergency_call_orig(SD_SS_HYBR_2) &&
        !SD_IS_BIT_CONTAIN( BM(cmd->pref_reason),(uint32)SDSSSCR_PREF_REAS_VAL_CALL_ORIG) &&
        !SD_IS_BIT_CONTAIN( BM(cmd->pref_reason),(uint32)SDSSSCR_PREF_REAS_VAL_ORIG_END) &&
	    !SD_IS_BIT_CONTAIN( BM(cmd->pref_reason),(uint32)SDSSSCR_PREF_REAS_VAL_USER ))
    {
      SD_MSG_HIGH_0( " HYBR2 LTE in emerg - block if reason is not User/Call Orig/ Orig End");
      if( cmd->rtrn_act_ptr != NULL )
      {
        cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
      }
      modem_mem_free(prev_act_ptr,MODEM_MEM_CLIENT_MMODE);

      return SD_SS_ACT_CONTINUE;
	}

  }

  /* RLF recovery is in progress and CS call is originated
  ** If last LTE system supported emergency continue with RLF. 
  ** If not exit it and acquire srv on CS RATs
  */
  #ifdef FEATURE_LTE
  if ((cmd->pref_reason == SD_SS_PREF_REAS_ORIG_START_CS) &&
      (cmd->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG ) &&
       sdss_is_lte_connected_mode (SD_SS_HYBR_2))
  {
    /* Emerg call is originated, reset the LTE connected state if last LTE
    ** system did not support emergency or mode pref for emergency call does not contain LTE
    */  
    if( sdss_is_lte_ims_emerg_allowed() &&
        SD_MODE_CONTAIN(SD_SS_MODE_PREF_LTE, cmd->mode_pref) )
    {
      sdss_update_is_RLF_during_emerg( SD_SS_HYBR_2,TRUE);
      SD_MSG_HIGH_0(" continue with RLF recovery");
    }
    else
    {
      SD_MSG_HIGH_0("Emerg call orig-reset LTE conn mode");
      sdss_reset_lte_connected_mode (SD_SS_HYBR_2);
    }
  }

  if(!sdss_is_orig_mode0( cmd->orig_mode, SD_SS_ORIG_MODE_VAL_EMERG)
    && sdss_is_lte_connected_mode (SD_SS_HYBR_2)
    && (sdss_get_sim_state(SD_SS_HYBR_2)== SYS_SIM_STATE_PS_INVALID
        || sdss_get_sim_state(SD_SS_HYBR_2)== SYS_SIM_STATE_CS_PS_INVALID)
    )
  {
    SD_MSG_HIGH_1(" Resetting connected mode lte on %d", SD_SS_HYBR_2);
    sdss_reset_lte_connected_mode(SD_SS_HYBR_2);
  }
  
  /* If emergency call ends, reset RLF in emergency flag */
  if ( cmd->orig_mode != SD_SS_ORIG_MODE_EMERG_ORIG  &&
       sdss_get_is_RLF_during_emerg(SD_SS_HYBR_2) == TRUE )
  {
    sdss_update_is_RLF_during_emerg( SD_SS_HYBR_2,FALSE);
  }
  #endif /* FEATURE_LTE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdss_set_csg_data(SD_SS_HYBR_2, cmd->csg_id, cmd->csg_rat);

  /* Call on the SS-Core to update its SS-Preference setting.
  */
  is_pref_change = sdss_hybr_2_pref_update( cmd->pref_reason,
                                     cmd->orig_mode,
                                     cmd->mode_pref,
                                     cmd->band_pref,
                                     cmd->band_pref_lte,
                                     cmd->band_pref_tds,
                                     cmd->prl_pref,
                                     cmd->roam_pref,
                                     cmd->hybr_pref,
                                     cmd->otasp_band,
                                     cmd->otasp_blksys,
                                     cmd->domain_pref,
                                     cmd->acq_order_pref,
                                     cmd->pref_update_reason,
                                     cmd->user_mode_pref,
                                     cmd->voice_domain_pref,
                                     cmd->rat_acq_order_ptr
                                   );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if prl preference changed.
  */
  if ( (is_pref_change == SDSS_PREF_UPDATED_TRUE_PREF   ||
        is_pref_change == SDSS_PREF_UPDATED_SRV_DOMAIN) &&
       sdss_is_prl_pref_chgd( SD_SS_HYBR_2 )
     )
  {
    /* Update the PRL_AVAIL status on the appropriate SYS.
    */
    if( ss_state == SDSS_STATE_OPR )
    {
      sdss_update_avail_sys_status( SD_SS_HYBR_2, SDSR_HYBR_1_SRV_SYS );
    }
    else
    {
      sdss_update_avail_sys_status( SD_SS_HYBR_2, SDSR_HYBR_1_ACQ_SYS );
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Preferences is a optimized ss pref change. just return CONTINUE.
  */
  if( sd_is_opti_ss_pref_change( SD_SS_HYBR_2,
                                 is_pref_change,
                                 cmd->pref_reason,
                                 cmd->orig_mode,
                                 cmd->pref_update_reason ))
  {

    SD_MSG_HIGH_0( "SS-Pref didn't change - CONTINUE");

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-State is operation, inform the registered client (i.e., CM) that
    ** it is OK to originate as necessary.
    */
    if(ss_state == SDSS_STATE_OPR
#ifdef FEATURE_ECALL_IVS
  ||
      ((ss_state == SDSS_STATE_VRFY) && 
      (ecall_get_ecall_operating_mode_per_subs(sdss_map_ss_to_as_id(SD_SS_HYBR_2)) ==  ECALL_MODE_ECALL_ONLY ) )
#endif
      )
    {
      sd_si_ok_to_orig( SD_SS_HYBR_2, si_ptr->data_suspend );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }
    modem_mem_free(prev_act_ptr,MODEM_MEM_CLIENT_MMODE);

    return SD_SS_ACT_CONTINUE;
  }


  if( cmd->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG && 
      sdss_is_flag_set(SD_SS_HYBR_2,SDSS_FLAG_OVERRIDE_E911_ADD_INFO) )
  {
    sdss_set_flag(SD_SS_HYBR_2,SDSS_FLAG_OVERRIDE_E911_ADD_INFO, FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if ( cmd->pref_reason == SD_SS_PREF_REAS_USER_RESEL &&
            sdss_is_orig_mode0( cmd->orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL) )
  {

    sd_update_manual_sys( SDSR_HYBR_1_MANUAL_LST, cmd->manual_sys_info_ptr );

  } /* else if ( pref_reas == ... ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if ( cmd->pref_reason == SD_SS_PREF_REAS_AVOID_SYS)
  {
    /* Avoid SID/WILDCARD_NID
    */
    if ( !sdprl_add_lock_sid_nid( sdsr_list_get_ptr(SDSR_HYBR_1_IDL_SYS, 0 ), TRUE ))
    {
      SD_MSG_HIGH_0("SID/NID not locked, table full");
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Avoid band/chan for 60 seconds.
    */
    cmd->avoid_time = (dword)60;
    sdss_sr_list_avoid2( SD_SS_MAIN, SDSR_IDL_SYS, cmd->avoid_time );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_2,  SDSS_EVT_HYBR_2_USER_SS_PREF, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE |
                    SD_SS_ACT_ACQ_GPS  |
                    SD_SS_ACT_ACQ_GWL  ));


  /* Save the request id pertaining to sys sel pref as provided by client if the action is ACQ_GWL
  */
  if ((SD_SS_ACT_ACQ_GWL == act || SD_SS_ACT_PWR_SAVE == act || SD_SS_ACT_CONTINUE == act)
    && ((SD_SS_PREF_REAS_USER == cmd->pref_reason) || (SD_SS_PREF_REAS_USER_RESEL == cmd->pref_reason)))
  {
    ss_ptr->act.prm.acq_gwl.sys_sel_pref_req_id = cmd->req_id;
    ss_ptr->true_curr_act.prm.acq_gwl.sys_sel_pref_req_id = cmd->req_id;
    if(NULL != cmd->rtrn_act_ptr)
    {
      cmd->rtrn_act_ptr->prm.acq_gwl.sys_sel_pref_req_id = cmd->req_id;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdss_set_csg_non_rplmn_selection_type(SD_SS_HYBR_2, FALSE);

 curr_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                     (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
 if (curr_act_ptr != NULL)
 {
   memset(curr_act_ptr, 0, sizeof(sd_ss_act_s_type));
 }
 else
 {
   sys_err_fatal_null_ptr_exception();
 }

 (void)sd_ss_hybr_2_act_get_true_curr_act(curr_act_ptr, SD_SS_HYBR_2);

  if(SYS_CSG_ID_INVALID != cmd->csg_id)
  {
    /* CSG Selection is desired */
    if(NULL != cmd->manual_sys_info_ptr                                               &&
      TRUE == sys_plmn_id_is_valid(cmd->manual_sys_info_ptr->sys_id.id.plmn)          &&
      TRUE != sys_plmn_match(ss_ptr->rplmn, cmd->manual_sys_info_ptr->sys_id.id.plmn) &&
      act  == SD_SS_ACT_ACQ_GWL)
    {
      /* This is case of non-RPLMN based CSG Selection
       * Hence, change the mode to manual_csg.
       */
      sd_rex_enter_crit_sect();

      if(NULL != cmd->rtrn_act_ptr)
      {
  cmd->rtrn_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
        cmd->rtrn_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
        cmd->rtrn_act_ptr->prm.acq_gwl.net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
        *(&(cmd->rtrn_act_ptr->prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
        cmd->rtrn_act_ptr->prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;
      }

      curr_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
      curr_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
      curr_act_ptr->prm.acq_gwl.net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
      *(&(curr_act_ptr->prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
      curr_act_ptr->prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;

      sdss_set_csg_non_rplmn_selection_type (SD_SS_HYBR_2, TRUE);

      if(ss_ptr->true_curr_act.act == SD_SS_ACT_ACQ_GWL)
      {
        ss_ptr->true_curr_act.prm.acq_gwl.csg_id  = cmd->csg_id;
        ss_ptr->true_curr_act.prm.acq_gwl.csg_rat = cmd->csg_rat;
        ss_ptr->true_curr_act.prm.acq_gwl.net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
        *(&(ss_ptr->true_curr_act.prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
        ss_ptr->true_curr_act.prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;

        if(cmd->pref_reason == SD_SS_PREF_REAS_USER && cmd->orig_mode == SD_SS_ORIG_MODE_NONE)
        {  

          /* Set Non RPLMN CSG related Info */
          sdss_set_non_rplmn_csg_data(SD_SS_HYBR_2,
                                      cmd->csg_id,
                                      cmd->csg_rat, 
                                      SYS_NETWORK_SELECTION_MODE_MANUAL_CSG,
                                      cmd->manual_sys_info_ptr->sys_id.id.plmn,
                                      SD_MANUALLY_SELECTED_SPECIFIED_PLMN
                                      );
        }      
      }

      sd_rex_leave_crit_sect();
    }
    else
    {
      /* RPLMN based case */
      sd_rex_enter_crit_sect();
      if(ss_ptr->true_curr_act.act == SD_SS_ACT_ACQ_GWL)
      {
        ss_ptr->true_curr_act.prm.acq_gwl.csg_id  = cmd->csg_id;
        ss_ptr->true_curr_act.prm.acq_gwl.csg_rat = cmd->csg_rat;
        ss_ptr->true_curr_act.prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN;
        *(&(ss_ptr->true_curr_act.prm.acq_gwl.plmn)) = *(&(ss_ptr->rplmn));

        if(cmd->pref_reason == SD_SS_PREF_REAS_USER && cmd->orig_mode == SD_SS_ORIG_MODE_NONE)
        {
          /* Setting RPLMN related CSG info*/
          sdss_set_rplmn_csg_data(SD_SS_HYBR_2,
                                  cmd->csg_id, 
                                  cmd->csg_rat, 
                                  ss_ptr->rplmn, 
                                  SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN);
        }
      }

    if(NULL != cmd->rtrn_act_ptr)
    {
      cmd->rtrn_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
      cmd->rtrn_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
      cmd->rtrn_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN;
      *(&(cmd->rtrn_act_ptr->prm.acq_gwl.plmn)) = *(&(ss_ptr->rplmn));
    }

    curr_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
    curr_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
    curr_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN;
    *(&(curr_act_ptr->prm.acq_gwl.plmn)) = *(&(ss_ptr->rplmn));

    sd_rex_leave_crit_sect();
    }
  }
   /* Following else block is not related to CSG feature. On the other hand
     its for another feature called preferred PLMN+RAT selection */
  else if( cmd->csg_rat != SYS_RAT_NONE &&
           cmd->manual_sys_info_ptr != NULL &&
           sys_plmn_id_is_valid(cmd->manual_sys_info_ptr->sys_id.id.plmn) &&
           act == SD_SS_ACT_ACQ_GWL )
  {
    SD_MSG_MED_1("preferred plmn+rat selection for hybr2 csg_rat:%d",cmd->csg_rat);
    /* If preferred RAT info is passed, then attach it with ACQ_GWL
       Since this is one time only, don't need to remember in SD core state */
    if(cmd->rtrn_act_ptr != NULL)
    {
      cmd->rtrn_act_ptr->prm.acq_gwl.srv_req_type =
                                            SD_SS_SRV_REQ_TYPE_USER_PLMN_RAT;
      cmd->rtrn_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
      *(&(cmd->rtrn_act_ptr->prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
    }
    if(ss_ptr->true_curr_act.act == SD_SS_ACT_ACQ_GWL)
    {
      ss_ptr->true_curr_act.prm.acq_gwl.srv_req_type =
                                            SD_SS_SRV_REQ_TYPE_USER_PLMN_RAT;
      ss_ptr->true_curr_act.prm.acq_gwl.csg_rat = cmd->csg_rat;
      *(&(ss_ptr->true_curr_act.prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
    }
  }
    

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  /* Clear the action after forcing ss-pref */
  sdss_set_sv_opr_act( SDSS_SV_OPR_ACT_NONE );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If action is equal to previous returned action, just continue - no need
  ** to instruct the SS-Client of the same action twice.
  */

  if( sd_ss_act_is_equal(curr_act_ptr, prev_act_ptr, ss_ptr->meas_list_is_diff, SD_SS_HYBR_2))
  {
    SD_MSG_HIGH_0( "Same action - CONTINUE");

    /* If SS-Action is MEAS_XXX, set the measurement ID per the pervious
    ** SS-Action's measurement ID.
    */
    if( curr_act_ptr->act == SD_SS_ACT_MEAS_BACK ||
        curr_act_ptr->act == SD_SS_ACT_MEAS_DED )
    {
      ss_ptr->meas_id = prev_act_ptr->prm.meas.id;
    }

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    act = SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-State is operation and SS-Action is CONTINUE or background
  ** measurement, inform the registered client (i.e., CM) that it is OK to
  ** originate as necessary.
  */
  if(( (ss_state == SDSS_STATE_OPR) &&
      (act == SD_SS_ACT_CONTINUE || act == SD_SS_ACT_MEAS_BACK) )
#ifdef FEATURE_ECALL_IVS
      ||
      ((ss_state == SDSS_STATE_VRFY) && 
      (ecall_get_ecall_operating_mode_per_subs(sdss_map_ss_to_as_id(SD_SS_HYBR_2)) ==  ECALL_MODE_ECALL_ONLY) && 
      (act == SD_SS_ACT_CONTINUE) )
#endif
      )
  {
    sd_si_ok_to_orig( SD_SS_HYBR_2, si_ptr->data_suspend );
    #ifdef FEATURE_AVOID_DUP_BAND_SCAN
    /* in case of emergency call origination
    and alternate redial enabled, if action is continue, 
    then copy the srv system LTE band in emerg dup band info.
    this will be required to copy LTE bands already attempted upon 
    hybrid-2 stack to be avoided, if emergency call is shifted to 
    main stack after hard failure on hybrid-2 stack */
    if(act == SD_SS_ACT_CONTINUE && sdss_is_emergency_call_orig(SD_SS_HYBR_2) && 
      sdss_is_e911_alternate_redial_algorithm(SD_SS_HYBR_2))
    {
      sdss_add_last_acq_sys_band_to_emerg_dup_band_info(SD_SS_HYBR_2);
    }
    #endif /* FEATURE_AVOID_DUP_BAND_SCAN */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  modem_mem_free(prev_act_ptr,MODEM_MEM_CLIENT_MMODE);
  modem_mem_free(curr_act_ptr,MODEM_MEM_CLIENT_MMODE);
  return act;

} /* sd_ss_ind_hybr_2_user_ss_pref5 */



/*===========================================================================

FUNCTION sdcmd_hybr_3_user_ss_pref

DESCRIPTION

===========================================================================*/
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
sd_ss_act_e_type    sdcmd_hybr_3_user_ss_pref( sd_ss_ind_user_ss_pref_cmd  *cmd)
{

  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr         = sd_ss_ptr(SD_SS_HYBR_3);
  sd_ss_act_s_type    *prev_act_ptr   = NULL;
  sd_ss_act_s_type    *curr_act_ptr   = NULL;
  sd_si_s_type        *si_ptr         = sd_si_ptr(SD_SS_HYBR_3);
  sdss_pref_updated_e_type is_pref_change;
  sdss_state_e_type   ss_state = sd_get_ss_state(SD_SS_HYBR_3);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->pref_reason, SD_SS_PREF_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->orig_mode, SD_SS_ORIG_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->prl_pref,  SD_SS_PRL_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->hybr_pref, SD_SS_HYBR_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->otasp_band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->otasp_blksys, SD_BLKSYS_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_HIGH_6( "hybr3:pref_reason=%d, orig_mode=%d, mode_pref=0x%x,roam_pref=0x%x, hybr_pref=%d,user_mode_pref: %d",
                cmd->pref_reason, cmd->orig_mode, cmd->mode_pref,cmd->roam_pref, cmd->hybr_pref,cmd->user_mode_pref );
  SD_MSG_HIGH_4( "band_pref:(MSB)(LSB) cgw=0x%08x 0x%08x, TDS = 0x%08x 0x%08x",
               QWORD_HIGH(cmd->band_pref),
               QWORD_LOW(cmd->band_pref),
               QWORD_HIGH(cmd->band_pref_tds),
               QWORD_LOW(cmd->band_pref_tds));
  sd_print_lte_band_mask(cmd->band_pref_lte);
  SD_MSG_HIGH_6( "domain_pref=0x%x, acq_order_pref=0x%x  prl_pref 0x%x,hybr3:sys_sel_pref_req_id = %u, CSG ID = %d, CSG RAT =%d",
                cmd->domain_pref, cmd->acq_order_pref, cmd->prl_pref,cmd->req_id, cmd->csg_id, cmd->csg_rat );

  if (cmd->rat_acq_order_ptr != NULL)
  {
    sdss_print_rat_acq_order(cmd->rat_acq_order_ptr);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
    sys_err_fatal_invalid_value_exception((int)SYS_OVERALL_FEATURE_MODE_NORMAL);
  }

  prev_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (prev_act_ptr != NULL)
  {
    memset(prev_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*Reset Scan Scope*/
  sdss_set_scan_scope_type(SD_SS_HYBR_3, SYS_SCAN_SCOPE_FULL_BAND);

  (void)sd_ss_hybr_2_act_get_true_curr_act(prev_act_ptr, SD_SS_HYBR_3);
  

  #ifdef FEATURE_GPSONE_MSBASED
  /* If this is an optimized GPS related SS-Preference change, return without
  ** further processing.
  */
  if( sd_ss_pref_opti_gps( SD_SS_HYBR_3,
                           cmd->mode_pref,
                           cmd->orig_mode,
                           cmd->rtrn_act_ptr) )
  {
    modem_mem_free(prev_act_ptr,MODEM_MEM_CLIENT_MMODE);
    return sd_ss_hybr_2_act_get_true_curr_act(NULL , SD_SS_HYBR_3);
  }
  #endif
#if 0 //remove this svlte checking on hybr_3 
  if(sdss_is_emergency_call_orig(SD_SS_HYBR_3) &&
     !SD_IS_BIT_CONTAIN( BM(cmd->pref_reason),(uint32)SDSSSCR_PREF_REAS_VAL_CALL_ORIG) &&
     !SD_IS_BIT_CONTAIN( BM(cmd->pref_reason),(uint32)SDSSSCR_PREF_REAS_VAL_ORIG_END))
  {
    SD_MSG_HIGH_0( "LTE in emerg - block stack_syncup_bsr");
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }
    modem_mem_free(prev_act_ptr,MODEM_MEM_CLIENT_MMODE);

    return SD_SS_ACT_CONTINUE;
  }
#endif
  /* RLF recovery is in progress and CS call is originated
  ** If last LTE system supported emergency continue with RLF. 
  ** If not exit it and acquire srv on CS RATs
  */
  #ifdef FEATURE_LTE
  if ((cmd->pref_reason == SD_SS_PREF_REAS_ORIG_START_CS) &&
      (cmd->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG ) &&
       sdss_is_lte_connected_mode (SD_SS_HYBR_3))
  {
    /* Emerg call is originated, reset the LTE connected state if last LTE
    ** system did not support emergency or mode pref for emergency call does not contain LTE
    */  
    if( sdss_is_lte_ims_emerg_allowed() &&
        SD_MODE_CONTAIN(SD_SS_MODE_PREF_LTE, cmd->mode_pref) )
    {
      sdss_update_is_RLF_during_emerg( SD_SS_HYBR_3,TRUE);
      SD_MSG_HIGH_0(" continue with RLF recovery");
    }
    else
    {
      SD_MSG_HIGH_0("Emerg call orig-reset LTE conn mode");
      sdss_reset_lte_connected_mode (SD_SS_HYBR_3);
    }
  }

  if(!sdss_is_orig_mode0( cmd->orig_mode, SD_SS_ORIG_MODE_VAL_EMERG)
    && sdss_is_lte_connected_mode (SD_SS_HYBR_3)
    && (sdss_get_sim_state(SD_SS_HYBR_3)== SYS_SIM_STATE_PS_INVALID
        || sdss_get_sim_state(SD_SS_HYBR_3)== SYS_SIM_STATE_CS_PS_INVALID)
    )
  {
    SD_MSG_HIGH_1(" Resetting connected mode lte on %d", SD_SS_HYBR_3);
    sdss_reset_lte_connected_mode(SD_SS_HYBR_3);
  }
  
  /* If emergency call ends, reset RLF in emergency flag */
  if ( cmd->orig_mode != SD_SS_ORIG_MODE_EMERG_ORIG  &&
       sdss_get_is_RLF_during_emerg(SD_SS_HYBR_3) == TRUE )
  {
    sdss_update_is_RLF_during_emerg( SD_SS_HYBR_3,FALSE);
  }
  #endif /* FEATURE_LTE */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its SS-Preference setting.
  */
  is_pref_change = sdss_hybr_3_pref_update( cmd->pref_reason,
                                     cmd->orig_mode,
                                     cmd->mode_pref,
                                     cmd->band_pref,
                                     cmd->band_pref_lte,
                                     cmd->band_pref_tds,
                                     cmd->prl_pref,
                                     cmd->roam_pref,
                                     cmd->hybr_pref,
                                     cmd->otasp_band,
                                     cmd->otasp_blksys,
                                     cmd->domain_pref,
                                     cmd->acq_order_pref,
                                     cmd->pref_update_reason,
                                     cmd->user_mode_pref,
                                     cmd->voice_domain_pref,
                                     cmd->rat_acq_order_ptr
                                   );

  SD_MSG_HIGH_2("sd_ss_ind_hybr_3_user_ss_pref5 | setting csg data csg_id = %d, csg_rat = %d", cmd->csg_id, cmd->csg_rat);
  sdss_set_csg_data(SD_SS_HYBR_3, cmd->csg_id, cmd->csg_rat);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if prl preference changed.
  */
  if ( (is_pref_change == SDSS_PREF_UPDATED_TRUE_PREF   ||
        is_pref_change == SDSS_PREF_UPDATED_SRV_DOMAIN) &&
       sdss_is_prl_pref_chgd( SD_SS_HYBR_3 )
     )
  {
    /* Update the PRL_AVAIL status on the appropriate SYS.
    */
    if( ss_state == SDSS_STATE_OPR )
    {
      sdss_update_avail_sys_status( SD_SS_HYBR_3, SDSR_HYBR_3_SRV_SYS );
    }
    else
    {
      sdss_update_avail_sys_status( SD_SS_HYBR_3, SDSR_HYBR_3_ACQ_SYS );
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Preferences is a optimized ss pref change. just return CONTINUE.
  */
  if( sd_is_opti_ss_pref_change( SD_SS_HYBR_3,
                                 is_pref_change,
                                 cmd->pref_reason,
                                 cmd->orig_mode,
                                 cmd->pref_update_reason ))
  {

    SD_MSG_HIGH_0( "SS-Pref didn't change - CONTINUE");

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-State is operation, inform the registered client (i.e., CM) that
    ** it is OK to originate as necessary.
    */
    if(ss_state == SDSS_STATE_OPR
#ifdef FEATURE_ECALL_IVS
  ||
      ((ss_state == SDSS_STATE_VRFY) && 
      (ecall_get_ecall_operating_mode_per_subs(sdss_map_ss_to_as_id(SD_SS_HYBR_3)) ==  ECALL_MODE_ECALL_ONLY ) )
#endif
      )
    {
      sd_si_ok_to_orig( SD_SS_HYBR_3, si_ptr->data_suspend );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }
    modem_mem_free(prev_act_ptr,MODEM_MEM_CLIENT_MMODE);

    return SD_SS_ACT_CONTINUE;
  }


  if( cmd->orig_mode == SD_SS_ORIG_MODE_EMERG_ORIG && 
      sdss_is_flag_set(SD_SS_HYBR_3,SDSS_FLAG_OVERRIDE_E911_ADD_INFO) )
  {
    sdss_set_flag(SD_SS_HYBR_3,SDSS_FLAG_OVERRIDE_E911_ADD_INFO, FALSE);
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if ( cmd->pref_reason == SD_SS_PREF_REAS_USER_RESEL &&
            sdss_is_orig_mode0( cmd->orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL) )
  {

    sd_update_manual_sys( SDSR_HYBR_3_MANUAL_LST, cmd->manual_sys_info_ptr );

  } /* else if ( pref_reas == ... ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  else if ( cmd->pref_reason == SD_SS_PREF_REAS_AVOID_SYS)
  {
    /* Avoid SID/WILDCARD_NID
    */
    if ( !sdprl_add_lock_sid_nid( sdsr_list_get_ptr(SDSR_HYBR_3_IDL_SYS, 0 ), TRUE ))
    {
      SD_MSG_HIGH_0("SID/NID not locked, table full");
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Avoid band/chan for 60 seconds.
    */
    cmd->avoid_time = (dword)60;
    sdss_sr_list_avoid2( SD_SS_MAIN, SDSR_IDL_SYS, cmd->avoid_time );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_3,  SDSS_EVT_HYBR_3_USER_SS_PREF, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE |
                    SD_SS_ACT_ACQ_GPS  |
                    SD_SS_ACT_ACQ_GWL  ));


  /* Save the request id pertaining to sys sel pref as provided by client if the action is ACQ_GWL
  */
  if ((SD_SS_ACT_ACQ_GWL == act || SD_SS_ACT_PWR_SAVE == act || SD_SS_ACT_CONTINUE == act)
    && ((SD_SS_PREF_REAS_USER == cmd->pref_reason) || (SD_SS_PREF_REAS_USER_RESEL == cmd->pref_reason)))
  {
    ss_ptr->act.prm.acq_gwl.sys_sel_pref_req_id = cmd->req_id;
    ss_ptr->true_curr_act.prm.acq_gwl.sys_sel_pref_req_id = cmd->req_id;
    if(NULL != cmd->rtrn_act_ptr)
    {
      cmd->rtrn_act_ptr->prm.acq_gwl.sys_sel_pref_req_id = cmd->req_id;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdss_set_csg_non_rplmn_selection_type(SD_SS_HYBR_3, FALSE);

  curr_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (curr_act_ptr != NULL)
  {
    memset(curr_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

 (void)sd_ss_hybr_2_act_get_true_curr_act(curr_act_ptr, SD_SS_HYBR_3);

  if(SYS_CSG_ID_INVALID != cmd->csg_id)
  {
    /* CSG Selection is desired */
    if(NULL != cmd->manual_sys_info_ptr                                               &&
      TRUE == sys_plmn_id_is_valid(cmd->manual_sys_info_ptr->sys_id.id.plmn)          &&
      TRUE != sys_plmn_match(ss_ptr->rplmn, cmd->manual_sys_info_ptr->sys_id.id.plmn) &&
      act  == SD_SS_ACT_ACQ_GWL)
    {
      /* This is case of non-RPLMN based CSG Selection
       * Hence, change the mode to manual_csg.
       */
      sd_rex_enter_crit_sect();

      if(NULL != cmd->rtrn_act_ptr)
      {
  cmd->rtrn_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
        cmd->rtrn_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
        cmd->rtrn_act_ptr->prm.acq_gwl.net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
        *(&(cmd->rtrn_act_ptr->prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
        cmd->rtrn_act_ptr->prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;
      }

      curr_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
      curr_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
      curr_act_ptr->prm.acq_gwl.net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
      *(&(curr_act_ptr->prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
      curr_act_ptr->prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;

      sdss_set_csg_non_rplmn_selection_type (SD_SS_HYBR_3, TRUE);

      if(ss_ptr->true_curr_act.act == SD_SS_ACT_ACQ_GWL)
      {
  ss_ptr->true_curr_act.prm.acq_gwl.csg_id  = cmd->csg_id;
        ss_ptr->true_curr_act.prm.acq_gwl.csg_rat = cmd->csg_rat;
        ss_ptr->true_curr_act.prm.acq_gwl.net_sel_type = SYS_NETWORK_SELECTION_MODE_MANUAL_CSG;
        *(&(ss_ptr->true_curr_act.prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
        ss_ptr->true_curr_act.prm.acq_gwl.selected_plmn_type = SD_MANUALLY_SELECTED_SPECIFIED_PLMN;

        if(cmd->pref_reason == SD_SS_PREF_REAS_USER && cmd->orig_mode == SD_SS_ORIG_MODE_NONE)
        {  
           /* Set Non RPLMN CSG related Info */
           sdss_set_non_rplmn_csg_data(SD_SS_HYBR_3,
                                       cmd->csg_id,
                                       cmd->csg_rat, 
                                       SYS_NETWORK_SELECTION_MODE_MANUAL_CSG,
                                       cmd->manual_sys_info_ptr->sys_id.id.plmn,
                                       SD_MANUALLY_SELECTED_SPECIFIED_PLMN
                                       );
        }
      }

      sd_rex_leave_crit_sect();
    }
    else
    {
      /* RPLMN based case */
      sd_rex_enter_crit_sect();
      if(ss_ptr->true_curr_act.act == SD_SS_ACT_ACQ_GWL)
      {
        ss_ptr->true_curr_act.prm.acq_gwl.csg_id  = cmd->csg_id;
        ss_ptr->true_curr_act.prm.acq_gwl.csg_rat = cmd->csg_rat;
        ss_ptr->true_curr_act.prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN;
        *(&(ss_ptr->true_curr_act.prm.acq_gwl.plmn)) = *(&(ss_ptr->rplmn));

        if(cmd->pref_reason == SD_SS_PREF_REAS_USER && cmd->orig_mode == SD_SS_ORIG_MODE_NONE)
        {
          /* Setting RPLMN related CSG info*/
          sdss_set_rplmn_csg_data(SD_SS_HYBR_3,
                                  cmd->csg_id, 
                                  cmd->csg_rat, 
                                  ss_ptr->rplmn, 
                                  SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN);
        }
      }

    if(NULL != cmd->rtrn_act_ptr)
    {
      cmd->rtrn_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
      cmd->rtrn_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
      cmd->rtrn_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN;
      *(&(cmd->rtrn_act_ptr->prm.acq_gwl.plmn)) = *(&(ss_ptr->rplmn));
    }

    curr_act_ptr->prm.acq_gwl.csg_id  = cmd->csg_id;
    curr_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
    curr_act_ptr->prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN;
    *(&(curr_act_ptr->prm.acq_gwl.plmn)) = *(&(ss_ptr->rplmn));

    sd_rex_leave_crit_sect();
    }
  }

   /* Following else block is not related to CSG feature. On the other hand
     its for another feature called preferred PLMN+RAT selection */
  else if( cmd->csg_rat != SYS_RAT_NONE &&
           cmd->manual_sys_info_ptr != NULL &&
           sys_plmn_id_is_valid(cmd->manual_sys_info_ptr->sys_id.id.plmn) &&
           act == SD_SS_ACT_ACQ_GWL )
  {
    SD_MSG_MED_1("preferred plmn+rat selection for hybr3 csg_rat:%d",cmd->csg_rat);
    /* If preferred RAT info is passed, then attach it with ACQ_GWL
       Since this is one time only, don't need to remember in SD core state */
    if(cmd->rtrn_act_ptr != NULL)
    {
      cmd->rtrn_act_ptr->prm.acq_gwl.srv_req_type =
                                            SD_SS_SRV_REQ_TYPE_USER_PLMN_RAT;
      cmd->rtrn_act_ptr->prm.acq_gwl.csg_rat = cmd->csg_rat;
      *(&(cmd->rtrn_act_ptr->prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
    }
    if(ss_ptr->true_curr_act.act == SD_SS_ACT_ACQ_GWL)
    {
      ss_ptr->true_curr_act.prm.acq_gwl.srv_req_type =
                                            SD_SS_SRV_REQ_TYPE_USER_PLMN_RAT;
      ss_ptr->true_curr_act.prm.acq_gwl.csg_rat = cmd->csg_rat;
      *(&(ss_ptr->true_curr_act.prm.acq_gwl.plmn)) = *(&(cmd->manual_sys_info_ptr->sys_id.id.plmn));
    }
  }
    

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  /* Clear the action after forcing ss-pref */
  sdss_set_sv_opr_act( SDSS_SV_OPR_ACT_NONE );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If action is equal to previous returned action, just continue - no need
  ** to instruct the SS-Client of the same action twice.
  */

  if( sd_ss_act_is_equal(curr_act_ptr, prev_act_ptr, ss_ptr->meas_list_is_diff, SD_SS_HYBR_3))
  {
    SD_MSG_LOW_0( "Same action - CONTINUE");

    /* If SS-Action is MEAS_XXX, set the measurement ID per the pervious
    ** SS-Action's measurement ID.
    */
    if( curr_act_ptr->act == SD_SS_ACT_MEAS_BACK ||
        curr_act_ptr->act == SD_SS_ACT_MEAS_DED )
    {
      ss_ptr->meas_id = prev_act_ptr->prm.meas.id;
    }

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    act = SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-State is operation and SS-Action is CONTINUE or background
  ** measurement, inform the registered client (i.e., CM) that it is OK to
  ** originate as necessary.
  */
  if(( (ss_state == SDSS_STATE_OPR) &&
      (act == SD_SS_ACT_CONTINUE || act == SD_SS_ACT_MEAS_BACK) )
#ifdef FEATURE_ECALL_IVS
      ||
      ((ss_state == SDSS_STATE_VRFY) && 
      (ecall_get_ecall_operating_mode_per_subs(sdss_map_ss_to_as_id(SD_SS_HYBR_3)) ==  ECALL_MODE_ECALL_ONLY) && 
      (act == SD_SS_ACT_CONTINUE) )
#endif
      )
  {
    sd_si_ok_to_orig( SD_SS_HYBR_3, si_ptr->data_suspend );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  modem_mem_free(prev_act_ptr,MODEM_MEM_CLIENT_MMODE);
  modem_mem_free(curr_act_ptr,MODEM_MEM_CLIENT_MMODE);
  return act;

} /* sd_ss_ind_hybr_2_user_ss_pref5 */
#endif 


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
/*===========================================================================

FUNCTION sdcmd_user_get_net_gw_ss

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_user_get_net_gw_ss(
  sd_ss_ind_user_get_net_gw_ss_cmd  *cmd
)
{
  sd_ss_act_e_type      act                   = SD_SS_ACT_MAX;
  sd_ss_e_type          ss = cmd->ss;
  sd_ss_act_s_type      *ss_act_ptr           = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  boolean               is_get_net_allowed = FALSE ;
  sd_si_info_s_type    *si_info_ptr = &sd_si_ptr(ss)->si_info;
  sys_plmn_id_s_type rplmn_ptr;
  sys_found_plmn_list_u_type  *plmn_list_ptr = NULL;
  sys_plmn_list_status_e_type   plmn_list_status;

  
  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  dword                 wcdma_conn_mode_uptime = 0;
  boolean               wcdma_in_conn_mode = FALSE;
  
  sdss_read_wcdma_connected_mode(ss, &wcdma_conn_mode_uptime, &wcdma_in_conn_mode);
  #endif
  rplmn_ptr = si_info_ptr->sid.plmn.plmn_id;

  /*
     If SD is not in connected mode

      or 

      If SD is in connected mode and NV is set(manual search in wrlf)
      if manual search supported NV is set" allow manual search after suitable search period is over.
  */
  if (!sdss_is_in_connected_mode (ss))
  {
     is_get_net_allowed = TRUE ;
   }
   else
   {
     if (sdss_is_manual_search_supported_in_wrlf(ss))
     {    
       if ((sdss_is_lte_connected_mode(ss) == FALSE)
       #if defined ( FEATURE_WRLF_SYSTEM_SEL)
                && (wcdma_conn_mode_uptime == 0)
       #endif
          )
       {
         is_get_net_allowed = TRUE ;
       }
     }
   }

  if (is_get_net_allowed)
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Update the mode and band preference for get network list request.*/

    sdss_get_network_pref_update( ss, cmd->mode_pref, cmd->band_pref, cmd->band_pref_lte, cmd->band_pref_tds, cmd->list_type);

   
    /* Update network list type */
    sdss_update_get_network_type(ss, cmd->list_type);
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Process the SS-Event that is associated with this SS-Indication.
    ** GET NET is supported in AUTO mode. When LTE is in service 
    ** GET NET will be on Hybrid 2 stack for SVLTE
    */
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if(ss == SD_SS_HYBR_3)
    {
      act = sd_ss_event_proc( SD_SS_HYBR_3,  SDSS_EVT_HYBR_3_USER_GET_NET_GW, cmd->rtrn_act_ptr );
    }
    else
    #endif

    if(ss == SD_SS_HYBR_2 && (sdss_is_1x_sxlte() ||
                              (sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) && 
                               !sdss_is_sglte())))
    {
      act = sd_ss_event_proc( SD_SS_HYBR_2,  SDSS_EVT_HYBR_2_USER_GET_NET_GW, cmd->rtrn_act_ptr );
    }
    else
    {
      act = sd_ss_event_proc( ss, SDSS_EVT_USER_GET_NET_GW, cmd->rtrn_act_ptr );
    }

    /* If no system matches with mode and band pref GET NET is not possible and moves to 
      ** next construct. Declare GET NET request is done to CM.
      ** This is to handle rare condition and generally CM should handle and 
      ** reject such requests
      */
    if((act != SD_SS_ACT_GET_NET_GW) && sd_ss_is_get_net_done(ss))
    {
      plmn_list_status = SYS_PLMN_LIST_AS_ABORT;

      /* Report Empty List */
      plmn_list_ptr = (sys_found_plmn_list_u_type  *)modem_mem_alloc
                          (sizeof(sys_found_plmn_list_u_type ), MODEM_MEM_CLIENT_MMODE);
      if (plmn_list_ptr != NULL)
      {
        memset(plmn_list_ptr, 0, sizeof(sys_found_plmn_list_u_type ));
        sd_si_net_list_rpt_cb( &rplmn_ptr, ss, plmn_list_ptr, plmn_list_status );
        
      }
      SD_MSG_HIGH_0("GET NET not handled when no valid system record found");
      /* Free the memory allocated for PLMN List */
      modem_mem_free(plmn_list_ptr,MODEM_MEM_CLIENT_MMODE);
     }
  }
  else
  {
    /* Reject Manual PLMN serach req in RLF recovery */
    plmn_list_status = SYS_PLMN_LIST_REJ_IN_RLF;

    /* Report Empty List */
    plmn_list_ptr = (sys_found_plmn_list_u_type  *)modem_mem_alloc
                        (sizeof(sys_found_plmn_list_u_type ), MODEM_MEM_CLIENT_MMODE);
    if (plmn_list_ptr != NULL)
    {
      memset(plmn_list_ptr, 0, sizeof(sys_found_plmn_list_u_type ));
      sd_si_net_list_rpt_cb( &rplmn_ptr, ss, plmn_list_ptr, plmn_list_status );
      act = SD_SS_ACT_CONTINUE;
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }

    /* Free the memory allocated for PLMN List */
    modem_mem_free(plmn_list_ptr,MODEM_MEM_CLIENT_MMODE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & ( SD_SS_ACT_CONTINUE |
                     SD_SS_ACT_GET_NET_GW |
                     SD_SS_ACT_ACQ_GWL));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The band and mode preference is updated in system core.
  */
  /* Update the mode preference to be used for get networks list in the
  ** sd action.
  */
  ss_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (ss_act_ptr != NULL)
  {
    memset(ss_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  if ( act == SD_SS_ACT_GET_NET_GW )
  {
    (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);
    /* Print out true band and mode information. */
    SD_MSG_HIGH_3( "GET_NET_GW, true mode_pref=0x%x, band_pref=0x%llx ss=%d",
                 ss_act_ptr->prm.get_net_gw.gw_mode_pref,
                 ss_act_ptr->prm.get_net_gw.gw_band_pref,
                 ss);

  } /* if ( act == SD_SS_ACT_GET_NET_GW) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the network list type */
  sd_rex_enter_crit_sect();
  sd_ss_ptr(ss)->act.prm.get_net_gw.network_list_type           = cmd->list_type;
  sd_ss_ptr(ss)->true_curr_act.prm.get_net_gw.network_list_type = cmd->list_type;
  if(NULL != cmd->rtrn_act_ptr)
  {
    cmd->rtrn_act_ptr->prm.get_net_gw.network_list_type         = cmd->list_type;
  }

  sd_rex_leave_crit_sect();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);

  return act;

} /* sd_ss_ind_user_get_net_gw() */
#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */



#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/*===========================================================================
  
FUNCTION sdcmd_cdma_acq_failed_pilot

DESCRIPTION
 
===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_acq_failed_pilot(
      sd_cmd_t                *cmd
            /* Pointer to a buffer where to place returned SS-Action */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_ACQ_FAILED_PILOT, cmd->rtrn_act_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_acq_failed_pilot */


/*===========================================================================

FUNCTION sdcmd_cdma_acq_failed_sync

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS 

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_acq_failed_sync(
        sd_ss_act_s_type          *rtrn_act_ptr
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_ACQ_FAILED_SYNC, rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_acq_failed_sync */


/*===========================================================================

FUNCTION sdcmd_cdma_acq_failed_sync2

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_acq_failed_sync2(
      sd_cmd_t  *cmd
)
{
  return sdcmd_cdma_acq_failed_sync(cmd->rtrn_act_ptr);
}


/*===========================================================================

FUNCTION sdcmd_cdma_acq_prot_mis

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_acq_prot_mis(
        sd_cmd_t                  *cmd
            /* Pointer to a buffer where to place returned SS-Action */
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_ACQ_PROT_MIS, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_acq_prot_mis */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_prot_mis

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_prot_mis(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_PROT_MIS, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_prot_mis */


/*===========================================================================

FUNCTION sdcmd_cdma_acq_schm2

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_acq_schm2(
        sd_ss_ind_cdma_acq_schm_cmd *cmd
)
{
  sd_ss_act_e_type      act;

  /* Point at acquired system.
  */
  sdsr_ref_type       acq_sys_ref   = sdsr_list_get_ref( SDSR_ACQ_SYS, 0 );
  sdsr_s_type         *acq_sys_ptr  = sdsr_list_get_ptr( SDSR_ACQ_SYS, 0 );

  /* Point at candidate system.
  */
  sdsr_s_type         *cand_sys_ptr = sdsr_list_get_ptr( SDSR_CAND_SYS, 0 );

  /* Remember whether the service indicator is ON.
  */
  boolean             is_srv_on     = sd_si_is_srv_on( SD_SS_MAIN );

  sdss_sys_addn_info_s_type curr_sys_info;

  word                geo_sys_idx   = 0xFFFF;
  
  sys_plmn_mcc_list   *mcc_list_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_sys_ref != SDSR_REF_NULL );
  if(acq_sys_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  SD_ASSERT(BETWEEN(cmd->daylt_savings, SYS_DAYLT_SAVINGS_NONE, SYS_DAYLT_SAVINGS_MAX));

  SD_MSG_HIGH_4( "SRV_3GPP2: cdma_acq_schm, sid=%d, nid=%d ltm_off=%d, daylt_savings=%d", cmd->sid, cmd->nid, cmd->ltm_off, cmd->daylt_savings);



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the additional sys info structure */
  sdss_addn_sys_info_init(&curr_sys_info);

  /* Update the SID/NID of the acquired system per the SCHM's SID/NID.
  */
  acq_sys_ptr->sid.is95.sid = cmd->sid;
  acq_sys_ptr->sid.is95.nid = cmd->nid;

  /* If this function is being called from sync, update the true SID/NID
  ** fields.
  */
  if( cmd->is_from_sync )
  {
    acq_sys_ptr->sid.is95.true_sid = cmd->sid;
    acq_sys_ptr->sid.is95.true_nid = cmd->nid;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the
  ** acquired system.
  */
  if(sdss_is_mmss_operation(SD_SS_MAIN))
  	 sdprl_free_gwl_more_pref_list(SYS_SYS_MODE_CDMA);
  
  /* Initialize the plmn info list for 1x */
  mcc_list_ptr = modem_mem_alloc(sizeof(sys_plmn_mcc_list), MODEM_MEM_CLIENT_MMODE);

  if(mcc_list_ptr)
  {
    memset(mcc_list_ptr,0, sizeof(sys_plmn_mcc_list));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  
  geo_sys_idx = sdprl_get_info3(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_ACQ_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_ACQ_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_ACQ_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_ACQ_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_ACQ_COLLOC_LST, /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL,   /* Get info regardless of PRL match */
                  FALSE,               /* Indicate if srv sys is changed */
                  mcc_list_ptr);       /* 1x mcc list */  
		   
  sdss_update_mcc_list_info(SD_SS_MAIN,mcc_list_ptr);

  modem_mem_free(mcc_list_ptr, MODEM_MEM_CLIENT_MMODE);
		   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Insert the acquired system at the beginning of the SDSR_ACQED_LST and
  ** remove any duplicates.
  */
  sdsr_list_insert_ref( SDSR_ACQED_LST, 0, acq_sys_ref, TRUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the following conditions are TRUE, return CONTINUE in order to get
  ** the MCC/IMSI_11_12 from the paging channel before deciding whether to
  ** stay on the acquired system:
  **
  ** 1. There is no explicit PRL match for the SID/NID,
  ** 2. This function is being called from sync.
  ** 3. The PRL contains MCC/IMSI_11_12 system records.
  ** 4. This acquisition is not for a specific SID.
  */
  if( acq_sys_ptr->match_lvl < (byte) SDPRL_MATCH_WILDCARD_NID &&
      cmd->is_from_sync &&
      prl_hdr_get_ptr()->is_mcc_sys_rec &&
      !sdss_is_acq_specific_sid(SD_SS_MAIN) )
  {
    if( acq_sys_ptr->match_lvl < (byte) SDPRL_MATCH_WILDCARD_SID ) {
      SD_MSG_LOW_0( "No SID match - trying MCC/IMSI_11_12");
    }
    else {
      SD_MSG_LOW_0( "Wildcard SID match - trying MCC/IMSI_11_12");
    }

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL ) {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }



    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do auto-NAM processing.
  */
  sdprl_autonam_proc( acq_sys_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the acquired system does not conflict with the current SS-Preference
  ** and it is designated as an available in the PRL, append the acquired
  ** system to the AVAIL_LST list.
  */
  if( sdss_sr_is_pref_conf(SD_SS_MAIN, acq_sys_ptr, SDSS_PREF_CONF_NONE) &&
      sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_AVAIL) )
  {
    sdsr_list_app( SDSR_AVAIL_LST, SDSR_ACQ_SYS );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If we are in LTE connected mode, reset all the related fields*/
  if (sdss_is_lte_connected_mode (SD_SS_MAIN))
  {
     /* Since UE has camped, it is no longer in LTE connected state
     */
      sdss_reset_lte_connected_mode (SD_SS_MAIN);
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  /* Reset the Stack sync up flag.*/
  sdss_set_flag(SD_SS_MAIN, SDSS_FLAG_SYNC_UP_IN_PROGRESS, FALSE);
  #endif
  sdss_reset_sys_lost();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_ACQ_SCHM, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE (i.e., we have selected a new serving system),
  ** do the following updates.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {

    /* Update cdma time in si_info */
    sd_si_update_cdma_time (SD_SS_MAIN, cmd->ltm_off, cmd->daylt_savings, cmd->leap_secs);

    #ifdef FEATURE_JCDMA

    /* Keep the expected SID/NID of acq system if it is  wildcard and
    ** redirection is on.
    **
    ** Acq system could be the redirected system or idle system ( call
    ** release would trigger an attempt on the idle system )
    **
    */
    if( ! ( sdss_is_redir( SD_SS_MAIN, SDSS_REDIR_VAL_CDMA_ANY )
            && acq_sys_ptr->sid_expected.prm.is95.sid == PRL_WILDCARD_SID ))

    #endif /* FEATURE_JCDMA */
    {

      /* Update the expected SID/NID of the acquired system per the acquired
      ** SID/NID.
      */
      acq_sys_ptr->sid_expected.prm.is95.sid = cmd->sid;
      acq_sys_ptr->sid_expected.prm.is95.nid = cmd->nid;
    }

    /* Get the additional system info of the acquired system.
    */
    sdprl_get_addn_sys_record_info(acq_sys_ptr,
                                   &(curr_sys_info.assn_tag),
                                   &(curr_sys_info.geo_offset),
                                   &(curr_sys_info.geo_count));

    /* Store the geo index of the current system
    */
    curr_sys_info.geo_sys_idx = geo_sys_idx;

    /* Update the serving system per the acquired system.
    */
    sd_ss_srv_sys_update( SD_SS_MAIN,
                          SDSR_ACQ_SYS,
                          SDSR_ACQ_GEO_LST,
                          SDSR_ACQ_PREF_LST,
                          SDSR_ACQ_COLLOC_LST );

    /* Update the system indicators per the acquired system.
    */
    if ( sdss_get_operator_name ( SYS_MODEM_AS_ID_1 ) == SD_OPERATOR_CT &&
	 !sdss_is_mmss_operation(SD_SS_MAIN))
    {
    
      /* If service is currently not full srv, call on sd_si_update() to update the
         ** service indicators.
         */
      if( !sd_si_is_full_srv_on( SD_SS_MAIN ) )
      {
        sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_ACQ,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  acq_sys_ref,
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_ACQ_COLLOC_LST,
                  NULL,
                  NULL,
                  &curr_sys_info,
                  SYS_SRV_STATUS_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0,  /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  TRUE
                );
      }
    }
    else
    {
    sd_si_update_if_srv_is_off( SD_SS_MAIN,
                                SD_SI_UPDATE_ACQ,
                                SYS_SRV_STATUS_SRV,
                                SYS_SRV_DOMAIN_CS_PS,
                                acq_sys_ref,
                                NULL,
                                NULL,
                                NULL,
                                SDSR_ACQ_COLLOC_LST,
                                NULL,
                                NULL,
                                &curr_sys_info,
                                SYS_SRV_STATUS_SRV,
                                SD_SIB8_NO_CHANGE,
                                SYS_EXTEND_SRV_INFO_NONE,
                                SYS_REG_DOMAIN_NOT_APPLICABLE,
                                NULL
                                );

    }

    if( !is_srv_on )
    {
      /* Update the additional system info in sd state with that of
      ** the acquired system.
      */
      sdss_set_prev_sys_info(SD_SS_MAIN,
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }


    /* Update the candidate system per the acquired system.
    */
    sdsr_list_put_ref( SDSR_CAND_SYS, acq_sys_ref );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (we have not yet selected a serving system), if the acquired system
  ** is allowed by the PRL and complies with the current SS-Preference do the
  ** following updates:
  */
  else if(sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED) &&
          sdss_sr_is_pref_conf(SD_SS_MAIN, acq_sys_ptr, SDSS_PREF_CONF_NONE))
  {
    /* If the candidate system is NULL or the acquired system is more
    ** preferred than the candidate system, update the candidate system per
    ** the acquired system and reorder the available list according to the
    ** new candidate system.
    */
    if( cand_sys_ptr == NULL ||
        sdprl_is_more_pref(SD_SS_MAIN, acq_sys_ptr, cand_sys_ptr) )
    {
      sdsr_list_put_ref( SDSR_CAND_SYS, acq_sys_ref );
      sdsr_list_order( SDSR_AVAIL_LST, SDSR_CAND_SYS );
    }

    /* If before processing the SS-Event the service indicator was OFF,
    ** update the system indicators per the acquired system.
    */
    if( ! is_srv_on )
    {
      /* Get the additional system info of the acquired system.
      */
      sdprl_get_addn_sys_record_info(acq_sys_ptr,
                                     &(curr_sys_info.assn_tag),
                                     &(curr_sys_info.geo_offset),
                                     &(curr_sys_info.geo_count));

      /* Update time info in si_info */
      sd_si_update_cdma_time (SD_SS_MAIN, cmd->ltm_off, cmd->daylt_savings, cmd->leap_secs);

      if ( sdss_get_operator_name ( SYS_MODEM_AS_ID_1 ) == SD_OPERATOR_CT &&
	 !sdss_is_mmss_operation(SD_SS_MAIN))
      {
        sd_si_update( SD_SS_MAIN,
                    SD_SI_UPDATE_ACQ,
                    SYS_SRV_STATUS_SRV,
                    SYS_SRV_DOMAIN_CS_PS,
                    acq_sys_ref,
                    FALSE,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    SDSR_ACQ_COLLOC_LST,
                    NULL,
                    NULL,
                    &curr_sys_info,
                    SYS_SRV_STATUS_SRV,
                    SYS_LTE_CS_CAPABILITY_NONE,
                    SD_SIB8_NO_CHANGE,
                    FALSE,
                    SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                    0, /* Request Id not needed */
                    SYS_EXTEND_SRV_INFO_NONE,
                    SYS_REG_DOMAIN_NOT_APPLICABLE,
                    NULL,
                    TRUE);
      }

      else
      {
      sd_si_update( SD_SS_MAIN,
                    SD_SI_UPDATE_ACQ,
                    SYS_SRV_STATUS_SRV,
                    SYS_SRV_DOMAIN_CS_PS,
                    acq_sys_ref,
                    FALSE,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    SDSR_ACQ_COLLOC_LST,
                    NULL,
                    NULL,
                    &curr_sys_info,
                    SYS_SRV_STATUS_SRV,
                    SYS_LTE_CS_CAPABILITY_NONE,
                    SD_SIB8_NO_CHANGE,
                    FALSE,
                    SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                    0, /* Request Id not needed */
                    SYS_EXTEND_SRV_INFO_NONE,
                    SYS_REG_DOMAIN_NOT_APPLICABLE,
                    NULL,
                    FALSE);
      }
      sdss_set_prev_sys_info(SD_SS_MAIN,
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }
  }



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_acq_schm2 */


/*===========================================================================

FUNCTION sdcmd_cdma_acq_schm

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_acq_schm(
        sd_ss_ind_cdma_acq_schm_cmd *cmd
)
{
  return sdcmd_cdma_acq_schm2(cmd);
}


/*===========================================================================

FUNCTION sdcmd_cdma_acq_ho_fail

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_acq_ho_fail(
      sd_cmd_t                  *cmd
          /* Pointer to a buffer where to place returned SS-Action */
)
{

  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_ACQ_HO_FAIL, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
}


/*===========================================================================

FUNCTION sdcmd_cdma_acq_redir_abort

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type sdcmd_cdma_acq_redir_abort(
        sd_cmd_t        *cmd
           /**< Pointer to a buffer where to place returned SS-Action */
)
{
  sd_ss_act_e_type  act;
  sd_ss_e_type      ss        = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_CDMA_ACQ_REDIR_ABORT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA) );

  return act;

}


/*===========================================================================

FUNCTION sdcmd_cdma_opr_oh_info

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_oh_info(
      sd_ss_ind_cdma_opr_oh_info_cmd  *cmd
)
{
  sd_ss_act_e_type      act;
  boolean               chan_is_changed   = FALSE;
  boolean               band_is_changed   = FALSE;
  boolean               sid_is_changed    = FALSE;
  boolean               mcc_mnc_is_changed    = FALSE;
  boolean               is_prl_match      = FALSE;
  sdss_state_e_type     prev_ss_state;
  word                  sid_expected;
  word                  nid_expected;
  sdprl_extract_e_type  extract           = SDPRL_EXTRACT_ALL;
  sdsr_ref_type         srv_sys_ref;
  sdsr_s_type           *srv_sys_ptr;
  sdsr_s_type           *ho_sys_ptr;
  sd_si_mode_info_u_type  mode_info;
  sd_si_s_type          *si_ptr           = sd_si_ptr(SD_SS_MAIN);
  sdsr_s_type           *acq_sys_ptr  = sdsr_list_get_ptr( SDSR_ACQ_SYS, 0 );
  const prl_hdr_s_type  *prl_hdr_ptr  = prl_hdr_get_ptr();
  sdss_sys_addn_info_s_type curr_sys_info;
  word                  geo_sys_idx   = 0xFFFF;
  sdss_state_e_type   ss_state = sd_get_ss_state(SD_SS_MAIN);
  boolean               bid_is_changed    = FALSE;
  sdss_lte_avail_sys_s_type  sys; 
  boolean                    is_pref_lst_changed   = FALSE;
  sdss_lte_avail_e_type        prev_lte_avail_1x;
  sdss_lte_avail_e_type        curr_lte_avail_1x;
  sd_ss_mode_pref_e_type ss_mode_pref      = SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type ss_band_pref      = SD_SS_BAND_PREF_NONE;
  sys_plmn_mcc_list    *mcc_list_ptr = NULL;



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->band, SD_BAND_MAX );
  #ifdef SD_DEBUG
  #error code not present
#endif
  SD_ASSERT_ENUM_IS_INRANGE( cmd->ovhd_status, SD_SS_OVHD_STATUS_MAX );
  SD_ASSERT( acq_sys_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->reg_status, SYS_REG_STATUS_MAX );

  SD_MSG_HIGH_3( "SRV_3GPP2: CDMA oh, sid=%d, nid=%d, chan=%d", cmd->sid, cmd->nid, cmd->chan );
  SD_MSG_HIGH_6( "band=%d, mcc=%d, imsi_11_12=%d ovhd_status %d reg_status =%d, bid =%d", cmd->band, cmd->mcc, cmd->imsi_11_12,
  	cmd->ovhd_status, cmd->reg_status, cmd->base_id );
  
  if( acq_sys_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_HIGH_2("is_mmss_operation on main:%d. before reset :acq_sys_ptr->is_mape: %d ",sdss_is_mmss_operation(SD_SS_MAIN), acq_sys_ptr->is_mape);
  
  acq_sys_ptr->is_mape= FALSE;

  /* Initialize the additional sys info structure */
  sdss_addn_sys_info_init(&curr_sys_info);
  curr_sys_info.geo_sys_idx = si_ptr->si_info.geo_sys_idx;

  /* If SS-State is acquisition (i.e. we were holding on processing sync in
  ** order to get the MCC/IMSI_11_12 from the paging channel), call on
  ** sd_ss_ind_cdma_acq_schm2 first.
  */
  if( ss_state == SDSS_STATE_ACQ )
  {
    /* If MCC/IMSI_11_12 are provided, update the MCC and IMSI_11_12 fields
    ** of the ACQ_SYS, as well as encode the MCC/IMSI_11_12 into the SID/NID
    ** fields.
    */
    //if( mcc != SD_MCC_NONE )
    if( ( cmd->mcc > SD_MCC_NONE  && cmd->mcc < SD_MCC_MAX ) &&
        ( cmd->imsi_11_12 > SD_IMSI_11_12_NONE && cmd->imsi_11_12 < SD_IMSI_11_12_MAX ))
    {
      acq_sys_ptr->sid.is95.mcc         = cmd->mcc;
      acq_sys_ptr->sid.is95.imsi_11_12  = cmd->imsi_11_12;
      sdsr_sid_encode_mcc_imsi_11_12( acq_sys_ptr );
    }
   /* If we are in LTE connected mode, reset all teh related fields*/
    if (sdss_is_lte_connected_mode (SD_SS_MAIN))
    {
     /* Since UE has camped, it is no longer in LTE connected state
     */
      sdss_reset_lte_connected_mode (SD_SS_MAIN);
    }

    /* Process SCHM.
    */
    {
      sd_ss_ind_cdma_acq_schm_cmd schm_cmd;

      schm_cmd.sid = acq_sys_ptr->sid.is95.sid;
      schm_cmd.nid = acq_sys_ptr->sid.is95.nid;
      schm_cmd.is_from_sync = FALSE;
      schm_cmd.ltm_off = SYS_LTM_OFFSET_INVALID;
      schm_cmd.daylt_savings = SYS_DAYLT_SAVINGS_NO_INFO;
      schm_cmd.leap_secs = SYS_LEAP_SECONDS_NO_INFO;
      schm_cmd.rtrn_act_ptr = cmd->rtrn_act_ptr;

      act = sdcmd_cdma_acq_schm(&schm_cmd);
    }

    /* If there is no explicit PRL match for the MCC/IMSI_11_12, restore the
    ** true SID/NID values.
    */
    if( acq_sys_ptr->match_lvl < (byte) SDPRL_MATCH_WILDCARD_NID )
    {
      SD_MSG_LOW_0( "No PRL match for MCC/IMSI_11_12");
      sdsr_sid_restore_true( acq_sys_ptr );
    }

    /* If SS-Action is other than CONTINUE return now.
    */
    if( act != SD_SS_ACT_CONTINUE )
    {
      return act;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_EOOS
   #error code not present
#endif
  /* If we are in LTE connected mode, reset all teh related fields*/
  if (sdss_is_lte_connected_mode (SD_SS_MAIN))
  {
     /* Since UE has camped, it is no longer in LTE connected state
     */
      sdss_reset_lte_connected_mode (SD_SS_MAIN);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at serving and idle systems.
  */
  srv_sys_ref   = sdsr_list_get_ref( SDSR_SRV_SYS, 0 );
  srv_sys_ptr   = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  ho_sys_ptr    = sdsr_list_get_ptr( SDSR_HO_SYS, 0 );

  SD_ASSERT( srv_sys_ref != SDSR_REF_NULL );
  if(srv_sys_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out whether band/channel has changed.
  */
  if( srv_sys_ptr->sys.band !=  sd_map_band_to_band_pref(cmd->band) )
  {
    band_is_changed = TRUE;
    chan_is_changed = TRUE;
  }
  else if( srv_sys_ptr->sys.chan != cmd->chan  )
  {
    chan_is_changed = TRUE;
  }

  /* Find out whether the SID/NID has changed.
  */
  if( srv_sys_ptr->sid.is95.true_sid   != cmd->sid ||
      srv_sys_ptr->sid.is95.true_nid   != cmd->nid  )
  {
    sid_is_changed = TRUE;
  }

  /* Find out whether the MCC/IMSI_11_12 has changed.
  */
  if( srv_sys_ptr->sid.is95.mcc        != cmd->mcc ||
      srv_sys_ptr->sid.is95.imsi_11_12 != cmd->imsi_11_12 )
  {
    if ( prl_hdr_ptr->is_mcc_sys_rec == TRUE )
    {
      sid_is_changed = TRUE;
    }

    /* Update MCC/IMSI_11_12, regardless of is_mcc_sys_rec */
    srv_sys_ptr->sid.is95.mcc        = cmd->mcc;
    srv_sys_ptr->sid.is95.imsi_11_12 = cmd->imsi_11_12;
    mcc_mnc_is_changed = TRUE;
  }

  /* - If bid changed remember it. in event proc it will checked to decide to scan LTE or not
  ** - update lte_avail_1x flag. If this 1x is not in LTE_AVAIL and LTE_NOT_AVAIL, reset to none
  **   If it is already in one of the 2 files, update flag.
  */
  if(sdss_is_c2k_alt_algr_operation()) 
  {
    if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL) ||
      (sdss_get_ss_for_mode(SD_MODE_LTE) == SD_SS_MAIN))
    {
      if( si_ptr->si_info.mode_info.cdma.base_id != cmd->base_id )
      {
        bid_is_changed = TRUE;
        sdss_set_bid_change(TRUE);  
      }
      else
      {
        sdss_set_bid_change(FALSE); 
      }

      /* update 1x in LTE_NOT_AVAIL with bid info which added in schm immediate bsr
      ** since schm has no bid info. delay adding bid when get ovhd
      **  
      ** During UHDM and ECAM 1xCSFB call, base_id will be not available from base station
      ** so 1XCP will send bid 0 which is invalid  
      */    
      if(cmd->base_id != 0)
      {
        sys.sid = cmd->sid;
        sys.nid = cmd->nid;
        sys.bid = cmd->base_id;
        sdss_lte_avail_file_bid_update(&sys);
        
        /* update lte_avail_1x flag */
        if(sid_is_changed || bid_is_changed || 
           sdss_get_orig_mode(SD_SS_MAIN) == SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG ||
           sdss_get_orig_mode(SD_SS_MAIN) == SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG)
        {
          sdss_lte_avail_file_update(SDSS_EVT_CDMA_OPR_OH_SID, &sys, SD_SS_MAIN);
        }
      }

      /* Check if pref list change */
      sdsr_list_cpy(SDSR_TEMP_LST, SDSR_SRV_PREF_LST);
      if( ! sid_is_changed &&
        ! band_is_changed )
      {
        extract = SDPRL_EXTRACT_MATCH_ONLY;
      }
	  
      if(sdss_is_mmss_operation(SD_SS_MAIN))
  	    sdprl_free_gwl_more_pref_list(SYS_SYS_MODE_CDMA);

      /* Initialize the plmn info list for 1x */
      mcc_list_ptr = modem_mem_alloc(sizeof(sys_plmn_mcc_list), MODEM_MEM_CLIENT_MMODE);

      if(mcc_list_ptr)
      {
         memset(mcc_list_ptr,0, sizeof(sys_plmn_mcc_list));
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }	  
      geo_sys_idx = sdprl_get_info3(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_SRV_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_SRV_COLLOC_LST, /* Put collocated sys in this list */
                  extract,             /* extract level information */
                  FALSE,               /* Indicate if srv sys is changed */
                  mcc_list_ptr);       /* 1x mcc list */     
      sdss_update_mcc_list_info(SD_SS_MAIN,mcc_list_ptr);

      modem_mem_free(mcc_list_ptr, MODEM_MEM_CLIENT_MMODE);

      /* check if more pref LTe sys changed. e.g. when 1x ho happens in same GEO, the more pref LTE
      ** won't change. skip immediate LTE BSR in this case if no sid/nid change
      */
      sdsr_list_cpy(SDSR_TEMP2_LST, SDSR_SRV_PREF_LST);
      sdsr_list_del_sys(SD_SS_MAIN, SDSR_TEMP_LST, SDSR_CAT_NON_LTE);
      sdsr_list_del_sys(SD_SS_MAIN, SDSR_TEMP2_LST, SDSR_CAT_NON_LTE);
      sdsr_list_sort( SDSR_TEMP_LST, SDSR_COMP_PRL_ACQ);
      sdsr_list_sort( SDSR_TEMP2_LST, SDSR_COMP_PRL_ACQ);
      is_pref_lst_changed = !( sdsr_list_is_same( SDSR_TEMP_LST,
                                                  SDSR_TEMP2_LST,
                                                  SDSR_EQUAL_MODE )
                             &&

                             sdsr_list_is_same( SDSR_TEMP_LST,
                                                SDSR_TEMP2_LST,
                                                SDSR_EQUAL_BAND )
                             &&

                             sdsr_list_is_same( SDSR_TEMP_LST,
                                                SDSR_TEMP2_LST,
                                                SDSR_EQUAL_CHAN )
                            );

      sdss_set_lte_pref_change(is_pref_lst_changed);

      sdss_get_lte_avail_1x(&prev_lte_avail_1x, &curr_lte_avail_1x);
      /* set pref list change when it is moving from LTE_NOT_AVAIL to LTE_AVAIL/Unknown
      */
      if((prev_lte_avail_1x == SDSS_LTE_AVAIL_NOT_AVAIL) &&
        (curr_lte_avail_1x != SDSS_LTE_AVAIL_NOT_AVAIL))
      {
        sdss_set_lte_pref_change(TRUE);
      }

      if( chan_is_changed ||sid_is_changed || mcc_mnc_is_changed)
      {
        SD_MSG_HIGH_0("reset 1x tele bsr due to sid/chan/mcc change");
        sdss_1x_telescope_parm_reset();
      }
      if(is_pref_lst_changed)
      {        
        /*reset lte tele bsr only if there is geo change/ lte avail change too*/
        if(geo_sys_idx != curr_sys_info.geo_sys_idx ||
           curr_lte_avail_1x != prev_lte_avail_1x)
        {
          sdss_lte_telescope_parm_reset(SD_SS_MAIN);
        }
      }
    }
    else if(sdss_is_1x_sxlte() || 
          (sdss_get_ss_for_mode(SD_MODE_LTE) == SD_SS_HYBR_2))
    {
      if( si_ptr->si_info.mode_info.cdma.base_id != cmd->base_id )
      {
        bid_is_changed = TRUE;
      }
    }

    

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If serving system's information has changed, do the following updates:
  */
  SD_MSG_MED_3("is_sid_chg %d, is_chan_chg %d, is_bid_chg %d",sid_is_changed, chan_is_changed, bid_is_changed);
  if( chan_is_changed ||
      sid_is_changed || mcc_mnc_is_changed)
  {
    /* Remember the SID/NID of the previous serving system.
    */
    sid_expected = srv_sys_ptr->sid.is95.sid;
    nid_expected = srv_sys_ptr->sid.is95.nid;

    /* Reset evt cnt on sid nid change.
    */
    sdss_event_cnt_update( cmd->ss, SDSS_EVT_CNT_UPDT_REAS_SID_CHANGED );

    /* If band is changed but SID/NID is not changed, set sid_expected to
    ** SD_NEW_SID to indicate that this is a new system. We need to do this
    ** because the SID/NID is not unique across different bands.
    */
    if( band_is_changed &&
        ! sid_is_changed )
    {
      sid_expected = SD_NEW_SID;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Store the new information in the system record table and set the
    ** SRV_SYS to reference the stored record.
    */
    srv_sys_ref = sdsr_tbl_store5( SD_MODE_CDMA,
                                   cmd->band,
                                   cmd->chan,
                                   cmd->sid,
                                   cmd->nid,
                                   srv_sys_ptr->prl,
                                   srv_sys_ptr->roam,
                                   srv_sys_ptr->prl_assn );

    sdsr_list_put_ref( SDSR_SRV_SYS, srv_sys_ref );
    srv_sys_ptr = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
    if(srv_sys_ptr == NULL)
    {
      sys_err_fatal_null_ptr_exception();
    }
    /* Update the MCC/IMSI_11_12.
    */
    srv_sys_ptr->sid.is95.mcc         = cmd->mcc;
    srv_sys_ptr->sid.is95.imsi_11_12  = cmd->imsi_11_12;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_JCDMA

    if( sdss_is_redir( SD_SS_MAIN, SDSS_REDIR_VAL_CDMA_ANY )
        && ( acq_sys_ptr->sid_expected.prm.is95.sid == PRL_WILDCARD_SID ))
    {
      /* Set the expected SID of the new serving system to wildcard if the
      ** expected SID of the acq system is wildcard and redirection is on
      */
      srv_sys_ptr->sid_expected.prm.is95.sid = PRL_WILDCARD_SID;
    }
    else

    #endif /* FEATURE_JCDMA */
    {
      /* Update the expected SID/NID of the new
      ** serving system per the SID/NID
      ** of the previous serving system.
      */
      srv_sys_ptr->sid_expected.prm.is95.sid = sid_expected;
      srv_sys_ptr->sid_expected.prm.is95.nid = nid_expected;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Get the PRL designation, GEO list and more preferred list of the
    ** new serving system.
    **
    ** Note that if SID/NID is not changed and band is not changes, we only
    ** update the PRL related information if the new serving system has an
    ** explicit PRL match - else we continue to use the current PRL
    ** information. This is done so because the PRL might not list systems
    ** (i.e., SID/NID, mode, band and channel combinations) to which MS can
    ** get through channel hashing or idle handoff.
    */
    if( ! sid_is_changed &&
        ! band_is_changed )
    {
      extract = SDPRL_EXTRACT_MATCH_ONLY;
    }

    if(sdss_is_mmss_operation(SD_SS_MAIN))
  	 sdprl_free_gwl_more_pref_list(SYS_SYS_MODE_CDMA);

    /* Initialize the plmn info list for 1x */
      mcc_list_ptr = modem_mem_alloc(sizeof(sys_plmn_mcc_list), MODEM_MEM_CLIENT_MMODE);


    if(mcc_list_ptr)
    {
       memset(mcc_list_ptr,0, sizeof(sys_plmn_mcc_list));
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }	
    geo_sys_idx =
              sdprl_get_info3(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_SRV_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_SRV_COLLOC_LST, /* Put collocated sys in this list */
                  extract,             /* Get info if diff SID or PRL match*/
                  FALSE,               /* Indicate if srv sys is changed */
                  mcc_list_ptr);       /* 1x mcc list */        
    
    sdss_update_mcc_list_info(SD_SS_MAIN,mcc_list_ptr);
   
    if(mcc_list_ptr)
    {
      modem_mem_free(mcc_list_ptr, MODEM_MEM_CLIENT_MMODE);
    } 
    SD_MSG_HIGH_1("sdcmd_cdma_opr_oh_info | geo_sys_idx1: %d", geo_sys_idx);
    if( geo_sys_idx == 0xFFFF )
    {
      is_prl_match = FALSE;
    }
    else
    {
      is_prl_match = TRUE;
    }
  
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the following conditions are TRUE, try to match the MCC/IMSI_11_12:
    **
    ** 1. There is no explicit PRL match for the SID/NID,
    ** 2. The PRL contains MCC_IMSI_11_12 system records
    ** 3. MCC/IMSI_11_12 are provided
    ** 4. Acquisition is not for a specific SID.
    */
    //if( !is_prl_match &&
    if( (!is_prl_match || srv_sys_ptr->match_lvl < (byte) SDPRL_MATCH_WILDCARD_NID) &&
        prl_hdr_ptr->is_mcc_sys_rec &&
        cmd->mcc < SD_MCC_MAX &&
        cmd->imsi_11_12 < SD_IMSI_11_12_MAX &&
        !sdss_is_acq_specific_sid(SD_SS_MAIN) )
    {

      /* Encode the MCC/IMSI_11_12 into the SID/NID.
      */
      sdsr_sid_encode_mcc_imsi_11_12( srv_sys_ptr );

      /* Look for a PRL match.
      */
      if(sdss_is_mmss_operation(SD_SS_MAIN))
  	    sdprl_free_gwl_more_pref_list(SYS_SYS_MODE_CDMA);

      /* Initialize the plmn info list for 1x */
      mcc_list_ptr = modem_mem_alloc(sizeof(sys_plmn_mcc_list), MODEM_MEM_CLIENT_MMODE);

      if(mcc_list_ptr)
      {
         memset(mcc_list_ptr,0, sizeof(sys_plmn_mcc_list));
      }
      else
      {
        SD_ERR_FATAL_0("Can't allocate the memory for MCC list");
      }	
      geo_sys_idx =
              sdprl_get_info3(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_SRV_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_SRV_COLLOC_LST, /* Put collocated sys in this list */
                  extract,             /* Get info if diff SID or PRL match*/
                  FALSE,               /* Indicate if srv sys is changed */
                  mcc_list_ptr);       /* 1x mcc list */            

      sdss_update_mcc_list_info(SD_SS_MAIN,mcc_list_ptr);
      if(mcc_list_ptr)
      {
        modem_mem_free(mcc_list_ptr, MODEM_MEM_CLIENT_MMODE);
      }

      SD_MSG_HIGH_1("sdcmd_cdma_opr_oh_info | geo_sys_idx2: %d", geo_sys_idx);
      if( geo_sys_idx == 0xFFFF )
      {
        is_prl_match = FALSE;
      }
      else
      {
        is_prl_match = TRUE;
      }

      /* If there is no explicit PRL match for the MCC/IMSI_11_12, restore
      ** the true SID/NID values.
      */
      if( !is_prl_match ||
          srv_sys_ptr->match_lvl < (byte) SDPRL_MATCH_WILDCARD_NID )
      {
        sdsr_sid_restore_true( srv_sys_ptr );
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Insert the serving system at the beginning of the SDSR_ACQED_LST and
    ** remove any duplicates.
    */
    sdsr_list_insert_ref( SDSR_ACQED_LST, 0, srv_sys_ref, TRUE );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Do auto-NAM processing.
    */
    sdprl_autonam_proc( srv_sys_ptr );

  } /* if( Channel or SID is changed ) */
  else
  {
    geo_sys_idx = si_ptr->si_info.geo_sys_idx;
    SD_MSG_HIGH_1("sdcmd_cdma_opr_oh_info | geo_sys_idx3: %d", geo_sys_idx);

  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remember the current SS-State.
  */
  prev_ss_state = ss_state;

  /* Update the registration state.
  */
  sdss_cdma_reg_status_update( SD_SS_MAIN, cmd->reg_status, srv_sys_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if BSR timer uptime needs to be reset
  */
  sdss_check_bsr_timer(SD_SS_MAIN);

  /* CDMA ovhd info is received, reset BSR flag to FALSE
  */
  sdcmd_cdma_bsr_in_progress_cb(FALSE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication as
  ** follows:
  **
  **   1. If SS-State is not yet operation, SID/NID has changed or band has
  **      changed, process SDSS_EVT_CDMA_OPR_OH_SID.
  **   2. Else, if channel has changed, process SDSS_EVT_CDMA_OPR_OH_CHAN.
  **   3. Else, process SDSS_EVT_MISC_FILLER.
  */
  if( ss_state != SDSS_STATE_OPR ||
      sid_is_changed ||
      band_is_changed ||      
      (bid_is_changed && 
      sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE) == SD_SS_MAIN &&
      sdss_get_lte_pref_change()))
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_OH_SID, cmd->rtrn_act_ptr );
  }
  else if( chan_is_changed )
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_OH_CHAN, cmd->rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_FILLER, cmd->rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is continue or background mode measurement, do the
  ** following updates.
  ** If the overhead is incomplete, CP will not be idle even though
  ** the action is CONTINUE.
  */
  if( ( act == SD_SS_ACT_CONTINUE &&
        cmd->ovhd_status == SD_SS_OVHD_STATUS_COMPLETE ) ||
      act == SD_SS_ACT_MEAS_BACK )
  {
    #ifdef FEATURE_JCDMA

    /* Set the expected SID of the new serving system to wildcard if the
    ** expected SID of the acq system is wildcard and redirection is on.
    */
    if( sdss_is_redir( SD_SS_MAIN, SDSS_REDIR_VAL_CDMA_ANY )
        && ( acq_sys_ptr->sid_expected.prm.is95.sid == PRL_WILDCARD_SID ))
    {
      srv_sys_ptr->sid_expected.prm.is95.sid = PRL_WILDCARD_SID;
    }
    else

    #endif /* FEATURE_JCDMA */
    {
      /* Update the expected SID/NID of the serving system per the Overhead
      ** Info's SID/NID.
      */
      srv_sys_ptr->sid_expected.prm.is95.sid = cmd->sid;
      srv_sys_ptr->sid_expected.prm.is95.nid = cmd->nid;
    }


    /* Clear SDSR_SIB8_1X_NBR_LST if SD decides to stay on CDMA system */

    sdsr_list_clr( SDSR_SIB8_1X_NBR_LST);

    /* For Multimode devices, if device acquired 1x from TOT/PRL scans during Limited to FULL
     reselection phase, reset the resel_mode now */
    sdss_reset_resel_mode_if_full(SD_SS_MAIN);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If previous SS-State was not operation, update the idle system, its
    ** GEO and its PREF lists per the initial acquisition system.
    */
    if( prev_ss_state != SDSS_STATE_OPR )
    {
      sd_ss_idle_sys_update( SD_SS_MAIN,
                             SDSR_ACQ_SYS,
                             SDSR_ACQ_GEO_LST,
                             SDSR_ACQ_PREF_LST );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (previous SS-State is operation) if handoff designation is
    ** CDMA CAM to CDMA paging, update the idle system per the handoff
    ** system.
    */
    else if( sdss_is_handoff(SD_SS_MAIN, SDSS_HO_VAL_CDMA_CAM_TO_CDMA_PAG) &&
             ho_sys_ptr != NULL )
    {
      ho_sys_ptr->prl   = srv_sys_ptr->prl;
      ho_sys_ptr->roam  = srv_sys_ptr->roam;

      sd_ss_idle_sys_update( SD_SS_MAIN,
                             SDSR_HO_SYS,
                             SDSR_IDL_GEO_LST,
                             SDSR_IDL_PREF_LST );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SID/NID is changed, band is changed or channel is changed and the
    ** new serving system is a PRL system that is explicitly listed in the
    ** PRL's acquisition table, update the IDL_SYS, IDL_GEO and IDL_PREF
    ** lists per the SRV_SYS, SRV_GEO and SRV_PREF lists.
    */
    if( sid_is_changed
              ||
        band_is_changed
              ||
        (chan_is_changed &&
         is_prl_match &&
         sdsr_list_find(SDSR_PRL_LST, srv_sys_ptr) != SDSR_POS_NULL) )
    {
      sd_ss_idle_sys_update( SD_SS_MAIN,
                             SDSR_SRV_SYS,
                             SDSR_SRV_GEO_LST,
                             SDSR_SRV_PREF_LST );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else if channel is changed, only update the IDL_GEO and IDL_PREF lists
    ** per the SRV_GEO and SRV_PREF lists.
    */
    else if( chan_is_changed )
    {
      sd_ss_idle_sys_update( SD_SS_MAIN,
                             SDSR_MAX,
                             SDSR_SRV_GEO_LST,
                             SDSR_SRV_PREF_LST );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If there is no pending GSRDM, update the service indicators per the
    ** new serving system now. Else leave the service indicators unchanged -
    ** they will be updated upon processing the GSRDM.
    */
    if( ! cmd->is_pending_gsrdm )
    {
      /* Populate the mode specific info.
      */
      mode_info.cdma.base_id = cmd->base_id;
      mode_info.cdma.base_lat = cmd->base_lat;
      mode_info.cdma.base_long = cmd->base_long;
      mode_info.cdma.srch_win_n = cmd->srch_win_n;
      mode_info.cdma.reg_prd    = cmd->reg_prd;
      mode_info.cdma.height    = cmd->height;
      mode_info.cdma.loc_unc_h = cmd->loc_unc_h;
      mode_info.cdma.loc_unc_v = cmd->loc_unc_v;


      /* Get the additional system info of the acquired system.
      */
      sdprl_get_addn_sys_record_info(acq_sys_ptr,
                                     &(curr_sys_info.assn_tag),
                                     &(curr_sys_info.geo_offset),
                                     &(curr_sys_info.geo_count));

      /* Store the geo index of the current system
      */
      curr_sys_info.geo_sys_idx = geo_sys_idx;

      sd_si_update( SD_SS_MAIN,
                    SD_SI_UPDATE_OH,
                    SYS_SRV_STATUS_SRV,
                    SYS_SRV_DOMAIN_CS_PS,
                    srv_sys_ref,
                    TRUE,
                    NULL,
                    NULL,
                    NULL,
                    &mode_info,
                    SDSR_SRV_COLLOC_LST,
                    NULL,
                    NULL,
                    &curr_sys_info,
                    SYS_SRV_STATUS_SRV,
                    SYS_LTE_CS_CAPABILITY_NONE,
                    SD_SIB8_NO_CHANGE,
                    FALSE,
                    SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                    0, /* Request Id not needed */
                    SYS_EXTEND_SRV_INFO_NONE,
                    SYS_REG_DOMAIN_NOT_APPLICABLE,
                    NULL,
                    FALSE);

      /* Update the additional system info in sd state with that of
      ** the acquired system.
      */
      sdss_set_prev_sys_info(SD_SS_MAIN,
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Inform the registered client (i.e., CM) that it is OK to originate as
    ** necessary.
    */
    sd_si_ok_to_orig( SD_SS_MAIN, si_ptr->data_suspend );
   /* For SVLTE if more prefered HDR list is not zero update the list
   ** to have colloc systems with acquired 1x.
   */
   if(sdss_is_1x_sxlte() && 
      (sdss_get_ss_for_mode(SD_MODE_LTE)== SD_SS_HYBR_2) && 
      (sdsr_list_cnt(SDSR_MORE_PREF_HDR_LST) > 0))
   {
     sdsr_list_intersect(SDSR_MORE_PREF_HDR_LST, SDSR_SRV_COLLOC_LST);
   }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Reset any throttle counters if running.
    ** necessary.
    */
    sdss_throttle_reset_cntrs( SD_SS_MAIN );

    /* ALT_CDMA_BSR feature is on, do LTEAvailFile operation */
    if(sdss_is_c2k_alt_algr_operation()&&
      sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE) == SD_SS_HYBR_2 &&
      sdss_is_1x_sxlte())
    {
      /* Check if sid/nid/bid is changed
      ** Note. "sid_is_changed" include both sid and nid change
      */
      if(prev_ss_state!= SDSS_STATE_OPR ||
         sid_is_changed || bid_is_changed)
      {
        sys.sid = si_ptr->si_info.sid.is95.sid;
        sys.nid = si_ptr->si_info.sid.is95.nid;
        sys.bid = si_ptr->si_info.mode_info.cdma.base_id;

        sdss_lte_avail_file_update(SDSS_EVT_CDMA_OPR_OH_SID, &sys, SD_SS_MAIN);
      }

      sdss_get_lte_avail_1x(&prev_lte_avail_1x, &curr_lte_avail_1x);

      if((prev_lte_avail_1x == SDSS_LTE_AVAIL_NOT_AVAIL) &&
        (curr_lte_avail_1x != SDSS_LTE_AVAIL_NOT_AVAIL))
      {
        SD_MSG_HIGH_2("lte_avail_1x changed, prev = %d, curr = %d, reset tele BSR timer for SD_SS_HYBR_2",
                      prev_lte_avail_1x, curr_lte_avail_1x);
        sdss_lte_telescope_parm_reset(SD_SS_HYBR_2);
      }
    }

  } /* act == CONTINUE || MEAS_BACK */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_oh_info5 */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_rescan

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_rescan(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_RESCAN, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
} /* sd_ss_ind_cdma_opr_rescan */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_reg_rej

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_reg_rej(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_REG_REJ, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE  |
                    SD_SS_ACT_ACQ_CDMA  |
                    SD_SS_ACT_ACQ_AMPS  |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL    |
                    SD_SS_ACT_MEAS_DED  |
                    SD_SS_ACT_MEAS_BACK |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
} /* sd_ss_ind_cdma_opr_reg_rej */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_sys_lost2

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_sys_lost2(
      sd_ss_ind_cdma_opr_sys_lost_cmd *cmd
)
{
  sd_ss_act_e_type    act;
  sdss_state_e_type     ss_state          = sd_get_ss_state(SD_SS_MAIN);
  const prl_hdr_s_type  *prl_hdr_ptr      = prl_hdr_get_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ///* Update the bad points count.
  //*/
  //sd_ss_bad_point_update();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdss_reset_splmn_info(SYS_SYS_MODE_CDMA);


  /* If SS-State is acquisition and we have a PRL with MCC/IMSI_11_12 system
  ** records, process this as the following.
  **
  ** If the system lost is advise unlock rf, treat if as acq_failed_unlock_rf
  ** otherwise treat it as sync acquisition failure.
  */
  if( ss_state == SDSS_STATE_ACQ &&
      prl_hdr_ptr->is_mcc_sys_rec )
  {

    if( cmd->sys_lost == SD_SS_SYS_LOST_ADVISE_UNLOCK_RF )
    {
      SD_MSG_HIGH_0( "sys_lost => acq_failed_unlock_rf");
        /* Note that sd_ss_ind_cdma_acq_failed_unlock_rf does not
        ** issue a command, so we can call it directly.
        */
      return sd_ss_ind_cdma_acq_failed_unlock_rf( cmd->rtrn_act_ptr );
    }


    SD_MSG_HIGH_0( "sys_lost => acq_failed");
    return sdcmd_cdma_acq_failed_sync( cmd->rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->sys_lost, SD_SS_SYS_LOST_MAX );

  SD_MSG_HIGH_1( "sys_lost=%d", cmd->sys_lost);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the system lost reason.
  */
  sdss_sys_lost_update( SD_SS_MAIN, cmd->sys_lost );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_SYS_LOST, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(sd_si_is_srv_on( SD_SS_MAIN ))
  {
    sd_si_ptr(SD_SS_MAIN)->si_info.mode_info.cdma.base_id = 0;
    SD_MSG_MED_0("clear bid");
  }

  return act;

} /* sd_ss_ind_cdma_opr_sys_lost */


sd_ss_act_e_type		  sdcmd_cdma_operator_capability_changed(
		  sd_ss_ind_cdma_opr_sys_lost_cmd *cmd
	)
{
	sd_ss_act_e_type	act;
	  
	/* If SS-State is acquisition and we have a PRL with MCC/IMSI_11_12 system
	** records, process this as the following.
	**
	** If the system lost is advise unlock rf, treat if as acq_failed_unlock_rf
	** otherwise treat it as sync acquisition failure.
	*/

  
	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
	SD_ASSERT_ENUM_IS_INRANGE( cmd->sys_lost, SD_SS_SYS_LOST_MAX );
  
	SD_MSG_HIGH_1( "sys_lost=%d", cmd->sys_lost);
  
	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
	/* Update the SS-Core with the system lost reason.
	*/
	sdss_sys_lost_update( SD_SS_MAIN, cmd->sys_lost );
  
	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
	/* Process the SS-Event that is associated with this SS-Indication.
	*/
	act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_OPERATOR_CAPABILITY_CHANGED, cmd->rtrn_act_ptr );
  
	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
	/* Make sure the returned value is one of the allowed SS-Actions for this
	** SS-Indication.
	*/
	SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
					  SD_SS_ACT_ACQ_AMPS |
					  SD_SS_ACT_ACQ_HDR  |
					  SD_SS_ACT_ACQ_GWL   |
					  SD_SS_ACT_MEAS_DED |
					  SD_SS_ACT_PWR_SAVE) );
  
	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
	return act;
  
  } /* sdcmd_cdma_operator_capability_changed */

/*===========================================================================
FUNCTION sdcmd_cdma_opr_redir

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_redir(
      sd_ss_ind_cdma_opr_redir_cmd  *cmd
)
{
  sd_ss_act_e_type    act;

  boolean             redir_is_valid = TRUE;
  int                 num_chan =0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( cmd->redir_type == SD_SS_REDIR_NONE       ||
             cmd->redir_type == SD_SS_REDIR_CDMA_SRDM  ||
             cmd->redir_type == SD_SS_REDIR_CDMA_GSRDM ||
             cmd->redir_type == SD_SS_REDIR_CDMA_NDSS  );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported redirection's bands and channels in the system record
  ** table and update SDSR_REDIR_LST to reference the stored records.
  */
  sdsr_list_clr( SDSR_REDIR_LST );

  sd_ss_redir_rec_store( SDSR_REDIR_LST,
                         cmd->rec_list_ptr,
                         cmd->num_rec);

  /* Get total number of channels in the redirection records.
  */
  num_chan = sd_ss_redir_rec_get_num_chan( cmd->rec_list_ptr, cmd->num_rec );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If none of the redirection channels is valid for this target & num_chan
  ** is zero, indicate that this is an invalid redirection.
  */
  if( num_chan > 0 &&
      sdsr_list_cnt(SDSR_REDIR_LST) == 0 )
  {
    redir_is_valid = FALSE;
  }

  SD_MSG_HIGH_2("CDMA redir recs=%d, is_valid=%d", num_chan, redir_is_valid);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the redirection type.
  */
  sdss_redir_update2( SD_SS_MAIN,          /* MAIN system selection */
                      cmd->redir_type,     /* Redirection type */
                      redir_is_valid,      /* Valid redirection indicator */
                      cmd->is_rif,         /* RETURN_IF_FAIL indicator */
                      cmd->rec_list_ptr    /* Redirection list pointer */
                    );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SDSR_REDIR_SYS per the redirecting system.
  */
  //sdsr_list_cpy( SDSR_REDIR_SYS, SDSR_SRV_SYS );
  sdsr_list_cpy( SDSR_REDIR_SYS, SDSR_ACQ_SYS );

  if(sdss_is_c2k_alt_algr_operation() &&
     (sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE) == SD_SS_MAIN))
  {
    SD_MSG_HIGH_0("reset tele bsr due to redir");
    sdss_lte_telescope_parm_reset(SD_SS_MAIN);
    sdss_1x_telescope_parm_reset();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_REDIR, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACCEPT |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

}/* sd_ss_ind_cdma_opr_redir */


#if defined(FEATURE_LTE_TO_1X)
/*===========================================================================

FUNCTION sdcmd_cdma_opr_ho_lte

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_ho_lte(
      sd_ss_ind_cdma_opr_ho_lte_cmd *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( cmd->ho_type == SD_SS_HO_LTE_TO_CDMA );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported redirection's bands and channels in the system record
  ** table and update SDSR_REDIR_LST to reference the stored records.
  */
  sdsr_list_clr( SDSR_LTE_CDMA_HO_LST );

  sd_ss_ho_sys_param_store(SDSR_LTE_CDMA_HO_LST,
                         cmd->ho_sys_param);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the SS-Core with the ho type.
  */
  sdss_ho_update( SD_SS_MAIN, cmd->ho_type);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_CDMA_HO_LTE, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

}/* sd_ss_ind_cdma_opr_ho_lte */
#endif /* defined(FEATURE_LTE_TO_1X) */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_acpt_rej

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_acpt_rej(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( sdss_is_redir( SD_SS_MAIN, SDSS_REDIR_VAL_CDMA_LTE ) )
  {
    /* Process the SS-Event that is associated with this SS-Indication.
    ** If this is due to LTE-1x IRAT, this can happen either when SD is in
    ** ACQ state or OPR state (on LTE). Hence, this event needs to be handled
    ** without linking it to ACQ/OPR state.
    */
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_CDMA_REDIR_ACPT_REJ, cmd->rtrn_act_ptr );
  }
  else
  {
  /* Process the SS-Event that is associated with this SS-Indication.
  */
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_ACPT_REJ, cmd->rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE, update the service indicators, per the serving
  ** system. This is necessary because we do not update the service
  ** indicators in the receiving the sd_ss_ind_cdma_opr_oh_info() if there is
  ** a pending redirection.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_OH,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0, /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  FALSE
                );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_acpt_rej */


/*===========================================================================

FUNCTION sdcmd_update_cdma_sib8_nbrs

DESCRIPTION

===========================================================================*/
void       sdcmd_update_cdma_sib8_nbrs(
      sd_ss_ind_update_cdma_sib8_nbrs_cmd *cmd
)
{
  uint32 index                       = 0;
  uint32 num_nbrs                    = 0;
  sd_redir_rec_s_type *sib8_recs_ptr = NULL;

  SD_ASSERT(cmd->sib8_nbr_cell_info_ptr != NULL);

  num_nbrs = (uint32)(cmd->sib8_nbr_cell_info_ptr->num_nbrs_present);

  sdsr_list_clr( SDSR_SIB8_1X_NBR_LST );

  if ((num_nbrs == 0) || (num_nbrs > SD_MAX_CDMA_SIB8_NBR_CELLS))
  {
    SD_MSG_HIGH_0("Number of SIB-8 NBR records are ZERO so exiting now");
    return;
  }

  sib8_recs_ptr = modem_mem_alloc((sizeof(sd_redir_rec_s_type) * num_nbrs),
                                   MODEM_MEM_CLIENT_MMODE);

  if (sib8_recs_ptr != NULL)
  {

    for (index = 0; index < num_nbrs; index++)
    {
       (sib8_recs_ptr + index)->record_type    = SD_REDIR_REC_CDMA;
       (sib8_recs_ptr + index)->is_delete_tmsi = FALSE;

       (sib8_recs_ptr + index)->rec.cdma_rec.expected_nid = SD_WILDCARD_NID;
       (sib8_recs_ptr + index)->rec.cdma_rec.expected_sid = SD_WILDCARD_SID;
       (sib8_recs_ptr + index)->rec.cdma_rec.num_chans    = 1;


       (sib8_recs_ptr + index)->rec.cdma_rec.band_class
                                          = cmd->sib8_nbr_cell_info_ptr->sib8_nbrs_list[index].band_class;
       SD_MSG_MED_1("SIB8 INFO UPDATE: Band class = %d",
                                            cmd->sib8_nbr_cell_info_ptr->sib8_nbrs_list[index].band_class);

       (sib8_recs_ptr + index)->rec.cdma_rec.chan[0]
                                          = cmd->sib8_nbr_cell_info_ptr->sib8_nbrs_list[index].channel;
       SD_MSG_MED_1("SIB8 INFO UPDATE: Channel num = %d",
                                            cmd->sib8_nbr_cell_info_ptr->sib8_nbrs_list[index].channel);

    }
    /* Store supported redirection's bands and channels in the system record
     ** table and update SDSR_SIB8_NBR_LST to reference the stored records.
     */
    sd_ss_redir_rec_store( SDSR_SIB8_1X_NBR_LST,
                           sib8_recs_ptr,
                           cmd->sib8_nbr_cell_info_ptr->num_nbrs_present);

    SD_MSG_HIGH_1("SIB8 INFO UPDATE: num rec=%d", num_nbrs);

    modem_mem_free(sib8_recs_ptr, MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

}


/*===========================================================================

FUNCTION sdcmd_cdma_opr_new_sys

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_new_sys(
      sd_ss_ind_cdma_opr_new_sys_cmd  *cmd
)
{
  sd_ss_act_e_type    act;
  sdsr_ref_type       new_sys_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->band, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported new system information in the system record table and
  ** set the SDSR_NEW_SYS to reference the stored record.
  */
  sdsr_list_clr( SDSR_NEW_SYS );
  if( sdss_is_supp_sys(SD_MODE_CDMA, sd_map_band_to_band_pref(cmd->band), cmd->chan) )
  {
    new_sys_ref = sdsr_tbl_store2( SD_MODE_CDMA,
                                   sd_map_band_to_band_pref(cmd->band),
                                   cmd->chan );

    sdsr_list_put_ref( SDSR_NEW_SYS, new_sys_ref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sdss_is_c2k_alt_algr_operation() &&
     (sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE) == SD_SS_MAIN))
  {
    SD_MSG_HIGH_0("reset tele bsr due to redir");
    sdss_lte_telescope_parm_reset(SD_SS_MAIN);
    sdss_1x_telescope_parm_reset();
  }

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_NEW_SYS, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_new_sys */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_sys_resel

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_sys_resel(
      sd_ss_ind_cdma_opr_sys_resel_cmd  *cmd
)
{
  sd_ss_act_e_type    act;
  int                 i;
  sdsr_ref_type       sr_ref;
  int                 avail_entries;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( cmd->sys_num >= 0 );
  SD_ASSERT( !(cmd->sys_ptr == NULL && cmd->sys_num != 0) );

  if( cmd->sys_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out the number of system record table entries that are available
  ** for storing the reselection systems.
  */
  avail_entries = sdsr_tbl_size() - sdsr_list_cnt(SDSR_PRL_LST);
  SD_ASSERT( avail_entries >= cmd->sys_num );
  cmd->sys_num = MIN( cmd->sys_num, avail_entries );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported system reselection's channels in the system record table
  ** and update SDSR_SYS_RESEL_LST to reference the stored records.
  */
  sdsr_list_clr( SDSR_SYS_RESEL_LST );
  for( i=0; i < cmd->sys_num; i++ )
  {
    SD_ASSERT( cmd->sys_ptr != NULL );
    if( sdss_is_supp_sys(cmd->sys_ptr->mode,
                         cmd->sys_ptr->band,
                         cmd->sys_ptr->chan) )
    {
      sr_ref = sdsr_tbl_store2( cmd->sys_ptr->mode,
                                cmd->sys_ptr->band,
                                cmd->sys_ptr->chan );

      sdsr_list_insert_ref( SDSR_SYS_RESEL_LST,
                            SDSR_POS_NULL,
                            sr_ref,
                            TRUE );
    }
    cmd->sys_ptr++;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_SYS_RESEL, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_sys_resel */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_lock

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_lock(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SDSR_LOCK_SYS as per the SDSR_SRV_SYS.
  */
  sdsr_list_cpy( SDSR_LOCK_SYS, SDSR_SRV_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the CDMA locked status.
  */
  sdss_cdma_lock_update( SD_SS_MAIN, TRUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_LOCK, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_lock */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_unlock

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_unlock(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the CDMA locked status.
  */
  sdss_cdma_lock_update( SD_SS_MAIN, FALSE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_UNLOCK, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_unlock */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_acc_term

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_acc_term(
      sd_ss_ind_cdma_opr_acc_term_cmd *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->acc_reas, SD_SS_ACC_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->acc_term, SD_SS_ACC_TERM_MAX );

  SD_MSG_HIGH_2( "acc_term=%d, cmd->acc_reas=%d", cmd->acc_term, cmd->acc_reas);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the access and termination reasons.
  */
  sdss_acc_update( SD_SS_MAIN, cmd->acc_reas, cmd->acc_term );

  /* If the reason is ACC_SUCCESS, update the event count.
  */
  if( cmd->acc_term == SD_SS_ACC_TERM_ACC_SUCCESS )
  {
     sdss_event_cnt_update( cmd->ss, SDSS_EVT_CNT_UPDT_REAS_ACC_SUCCESS );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If access is successful, update the service indicators due to implicit
  ** registration.
  */
  if(( cmd->acc_term == SD_SS_ACC_TERM_NORMAL )||
     ( cmd->acc_term == SD_SS_ACC_TERM_ACC_SUCCESS ))
  {
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0, /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_ACC_TERM, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_acc_term */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_cam_err

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_cam_err(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_CAM_ERR, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_cam_err */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_cam_cdma

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_cam_cdma(
      sd_ss_ind_cdma_opr_cam_cdma_cmd *cmd
)
{
  sd_ss_act_e_type    act;
  sdsr_ref_type       ho_sys_ref;

  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  if(srv_sys_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( cmd->ho_type == SD_SS_HO_CDMA_CAM_TO_CDMA_PAG ||
             cmd->ho_type == SD_SS_HO_CDMA_CAM_TO_CDMA_TRF )
  SD_ASSERT_ENUM_IS_INRANGE( cmd->band, SD_BAND_MAX );
  #ifdef SD_DEBUG
  #error code not present
#endif

  SD_ASSERT( srv_sys_ptr != NULL );

  SD_MSG_HIGH_3("OPR_CAM: chan = %d, sid =%d, band = %d", cmd->chan, srv_sys_ptr->sid.is95.sid, cmd->band);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the handoff type.
  */
  sdss_ho_update( SD_SS_MAIN, cmd->ho_type );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the handoff system per the channel assignment and the current
  ** serving system.
  */
  ho_sys_ref = sdsr_tbl_store6( SD_MODE_CDMA,        /* mode */
                                cmd->band,                 /* band */
                                cmd->chan,                 /* CDMA channel */
                                srv_sys_ptr->sid.is95.sid, /* SID */
                                srv_sys_ptr->sid.is95.nid, /* NID */
                                srv_sys_ptr->sid.is95.sid, /* Expected SID */
                                srv_sys_ptr->sid.is95.nid, /* Expected NID */
                                srv_sys_ptr->sid.is95.mcc, /* MCC */
                                srv_sys_ptr->sid.is95.imsi_11_12, /*IMSI_11*/
                                srv_sys_ptr->prl,    /* PRL designation */
                                srv_sys_ptr->roam,   /* Roam designation*/
                                srv_sys_ptr->prl_assn );/* Assn designation*/

  sdsr_list_put_ref( SDSR_HO_SYS, ho_sys_ref );

  sdsr_list_print( SDSR_HYBR_1_SRV_SYS, TRUE );
  sdsr_list_print( SDSR_HO_SYS, TRUE ); 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_CAM, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE and CAM is not to CDMA paging, do the
  ** following:
  */
  if( act == SD_SS_ACT_CONTINUE &&
      ! sdss_is_handoff(SD_SS_MAIN, SDSS_HO_VAL_CDMA_CAM_TO_CDMA_PAG) )
  {
    /* Update serving system per the handoff system.
    */
    sdsr_list_cpy( SDSR_SRV_SYS, SDSR_HO_SYS );

    /* Update the service indicators per the new serving system.
    */
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0, /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_cam_cdma */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_cam_amps

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_cam_amps(
      sd_ss_ind_cdma_opr_cam_amps_cmd *cmd
)
{
  sd_ss_act_e_type    act;
  sdsr_ref_type       ho_sys_ref;

  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  if(srv_sys_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( cmd->ho_type == SD_SS_HO_CDMA_CAM_TO_AMPS_SYS ||
             cmd->ho_type == SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE );
  #ifdef SD_DEBUG
  #error code not present
#endif

  SD_ASSERT( srv_sys_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the handoff type.
  */
  sdss_ho_update( SD_SS_MAIN, cmd->ho_type );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MS is assigned to AMPS voice channel, get the cellular system that
  ** corresponds to this voice channel.
  */
  if( cmd->ho_type == SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE )
  {
    cmd->cell_sys = sdprl_map_amps_vc_to_sys(cmd->voice_chan);
  }

  SD_ASSERT_ENUM_IS_INRANGE( cmd->cell_sys, SD_CELL_SYS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the handoff system per the channel assignment and the current
  ** serving system.
  */
  ho_sys_ref = sdsr_tbl_store6( SD_MODE_AMPS,        /* mode */
                                SD_BAND_CELL,        /* band */
                 (sd_chan_type) cmd->cell_sys,       /* AMPS system */
                                cmd->sid,            /* SID */
                                SD_WILDCARD_NID,     /* NID */
                                cmd->sid,            /* Expected SID */
                                SD_WILDCARD_NID,     /* Expected NID */
                                srv_sys_ptr->sid.is95.mcc, /* MCC */
                                srv_sys_ptr->sid.is95.imsi_11_12, /*IMSI_11*/
                                srv_sys_ptr->prl,    /* PRL designation */
                                srv_sys_ptr->roam,   /* Roam designation*/
                                srv_sys_ptr->prl_assn );/* Assn designation*/

  sdsr_list_put_ref( SDSR_HO_SYS, ho_sys_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the
  ** handoff system.
  */
  (void) sdprl_get_info(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_HO_SYS,         /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_HO_GEO_LST,     /* Put GEO systems into this list */
                  SDSR_HO_PREF_LST,    /* Put PREF systems into this list */
                  SDSR_MAX,            /* Do not extract collocated sys */
                  SDPRL_EXTRACT_ALL ); /* Get info regardless of PRL match */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_CAM, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE or ACQ_AMPS, do the following:
  */
  if( act == SD_SS_ACT_CONTINUE ||
      act == SD_SS_ACT_ACQ_AMPS )
  {
    /* Update the serving system per the handoff system.
    */
    sd_ss_srv_sys_update( SD_SS_MAIN,
                          SDSR_HO_SYS,
                          SDSR_HO_GEO_LST,
                          SDSR_HO_PREF_LST,
                          SDSR_MAX );

    /* Update the service indicators per the new serving system.
    */
    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0, /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  FALSE);

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_cam_amps */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_hdm_cdma

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_hdm_cdma(
      sd_ss_ind_cdma_opr_hdm_cdma_cmd *cmd
)
{
  sd_ss_act_e_type    act;
  sdsr_ref_type       ho_sys_ref;

  /* Point at serving system.
  */
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->band, SD_BAND_MAX );
  #ifdef SD_DEBUG
  #error code not present
#endif

  if(srv_sys_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the handoff type.
  */
  sdss_ho_update( SD_SS_MAIN, SD_SS_HO_CDMA_HDM_TO_CDMA_TRF );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the handoff system per the handoff direction and the current
  ** serving system.
  */
  ho_sys_ref = sdsr_tbl_store6(
                           SD_MODE_CDMA,              /* Mode */
                           cmd->band,                 /* Band */
                           cmd->chan,                 /* CDMA channel */
                           srv_sys_ptr->sid.is95.sid, /* SID */
                           srv_sys_ptr->sid.is95.nid, /* NID */
                           srv_sys_ptr->sid_expected.prm.is95.sid, /* Expected SID */
                           srv_sys_ptr->sid_expected.prm.is95.nid, /* Expected NID */
                           srv_sys_ptr->sid.is95.mcc, /* MCC */
                           srv_sys_ptr->sid.is95.imsi_11_12, /* IMSI_11_12 */
                           srv_sys_ptr->prl,          /* PRL designation */
                           srv_sys_ptr->roam,         /* Roam designation*/
                           srv_sys_ptr->prl_assn );   /* Assn designation */

  sdsr_list_put_ref( SDSR_HO_SYS, ho_sys_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_HDM, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE, update the serving system per the handoff
  ** system and the system indicators per the new serving system.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    sdsr_list_cpy( SDSR_SRV_SYS, SDSR_HO_SYS );

    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  sdsr_list_get_ref(SDSR_SRV_SYS, 0),
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0, /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_hdm_cdma */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_itspm

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_itspm(
      sd_ss_ind_cdma_opr_itspm_cmd  *cmd
)
{
  sd_ss_act_e_type    act;
  sdsr_s_type         *srv_sys_ptr  = sdsr_list_get_ptr( SDSR_SRV_SYS, 0 );
  sdsr_ref_type       srv_sys_ref   = sdsr_list_get_ref( SDSR_SRV_SYS, 0 );
  sdsr_s_type         *acq_sys_ptr  = sdsr_list_get_ptr( SDSR_ACQ_SYS, 0 );

  SYS_ARG_NOT_USED( acq_sys_ptr );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( srv_sys_ref != SDSR_REF_NULL );
  SD_ASSERT( acq_sys_ptr != NULL );

  if(srv_sys_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SID/NID did not change, return CONTINUE.
  */
  if( srv_sys_ptr->sid.is95.sid == cmd->sid &&
      srv_sys_ptr->sid.is95.nid == cmd->nid )
  {
    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID/NID of the current serving system per the ITSPM.
  */
  srv_sys_ptr->sid.is95.sid = cmd->sid;
  srv_sys_ptr->sid.is95.nid = cmd->nid;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the new
  ** serving system.
  */
  if(sdss_is_mmss_operation(SD_SS_MAIN))
  	 sdprl_free_gwl_more_pref_list(SYS_SYS_MODE_CDMA);
  
  (void) sdprl_get_info(
                  SD_SS_MAIN,          /* MAIN system selection */
                  SDSR_SRV_SYS,        /* Get PRL info for this list */
                  0,                   /* and list position */
                  SDSR_SRV_GEO_LST,    /* Put GEO systems into this list */
                  SDSR_SRV_PREF_LST,   /* Put PREF systems into this list */
                  SDSR_SRV_COLLOC_LST, /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info regardless of PRL match */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_ITSPM, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_RELEASE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE (i.e., we have selected a new serving system),
  ** update the expected SID/NID of the serving system per the acquired
  ** SID/NID and the system indicators per the new serving system.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    #ifdef FEATURE_JCDMA

    /* Set the expected SID of the new serving system to wildcard if the
    ** expected SID of the acq system is wildcard and redirection is on.
    */
    if( sdss_is_redir( SD_SS_MAIN, SDSS_REDIR_VAL_CDMA_ANY )
        && ( acq_sys_ptr->sid_expected.prm.is95.sid == PRL_WILDCARD_SID ))
    {
      srv_sys_ptr->sid_expected.prm.is95.sid = PRL_WILDCARD_SID;
    }
    else

    #endif /* FEATURE_JCDMA */
    {
      srv_sys_ptr->sid_expected.prm.is95.sid = cmd->sid;
      srv_sys_ptr->sid_expected.prm.is95.nid = cmd->nid;
    }

    sd_si_update( SD_SS_MAIN,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_CS_PS,
                  srv_sys_ref,
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_SRV_COLLOC_LST,
                  NULL,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0, /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_itspm */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_release

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_release(
  sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_RELEASE, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_release */

/*===========================================================================

FUNCTION sdcmd_cdma_opr_start_activity

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_start_activity(
        sd_ss_ind_cdma_opr_activity_cmd *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;

  /* Set SS to MAIN as 1x will always be on MAIN stack
  */
  sd_ss_e_type        ss    = SD_SS_MAIN;

  sd_ss_s_type        *ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->cdma_act_mask, SD_SS_CDMA_ACT_MASK_MAX );
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the activity flag to TRUE and inform CM about it */
  
  sdss_update_cdma_activity( ss, TRUE );

  /* Is there a need to inform CM about the on-going CDMA
  ** activity with the SI Callback..?? */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the activity to the activity mask.
  */
  ss_ptr = sd_ss_ptr( ss );

  ss_ptr->cdma_act_mask = ( sd_ss_cdma_act_mask_e_type )SD_BIT_ADD(
                                ss_ptr->cdma_act_mask, cmd->cdma_act_mask );

  SD_MSG_HIGH_3( "Cdma_activity set to TRUE. Start 1x activity 0x%x mask 0x%x, Mask from 1x = 0x%x",
                 cmd->cdma_act_mask, ss_ptr->cdma_act_mask, cmd->cdma_act_mask);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  if ( cmd->rtrn_act_ptr != NULL )
  {
    cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
  }

  SD_ASSERT( act & SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
} /* sd_ss_ind_cdma_opr_start_activity */

/*===========================================================================

FUNCTION sdcmd_cdma_opr_stop_activity

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_cdma_opr_stop_activity(
        sd_ss_ind_cdma_opr_activity_cmd *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;

  sdss_state_e_type   ss_state;

  /* Assume no hybrid operation.
  */
  sd_ss_e_type        ss        = SD_SS_MAIN;
  sd_ss_s_type        *ss_ptr   = sd_ss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->cdma_act_mask, SD_SS_CDMA_ACT_MASK_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out the activities that have not been started.
  */
  cmd->cdma_act_mask = ( sd_ss_cdma_act_mask_e_type )SD_BIT_COMMON(
                                     ss_ptr->cdma_act_mask, cmd->cdma_act_mask );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove the activity from the CDMA activity mask.
  */
 

  ss_ptr->cdma_act_mask = ( sd_ss_cdma_act_mask_e_type )SD_BIT_DELETE(
                                        ss_ptr->cdma_act_mask, cmd->cdma_act_mask );

 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ss_ptr->cdma_act_mask == SD_SS_CDMA_ACT_MASK_NONE )
  {
    /* Set cdma activity flag to FALSE */
    sdss_update_cdma_activity( SD_SS_MAIN, FALSE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the system selection state.
  */
  ss_state  = sd_get_ss_state(ss);

  SD_MSG_HIGH_3( "Stop activity 0x%x, mask 0x%x, ss_state %d",
                 cmd->cdma_act_mask, ss_ptr->cdma_act_mask, ss_state );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if rtrn_act_ptr is not NULL and populate return action.
  */
  if ( cmd->rtrn_act_ptr != NULL )
  {
    cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
  }

  SD_ASSERT( act & SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_stop_activity */

#ifdef FEATURE_MMODE_LTE_RESEL
/*===========================================================================

FUNCTION sdcmd_misc_irat_to_gwl

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_misc_irat_to_gwl(
      sd_ss_ind_misc_irat_to_gwl_cmd  *cmd
)
{
  sd_ss_act_e_type    act            = SD_SS_ACT_CONTINUE;
  sd_ss_e_type        ss             = cmd->ss;
  boolean             redir_is_valid = TRUE;
  sdsr_e_type         redir_list     = sdss_sr_list_map2(ss, SDSR_REDIR_LST );
  sys_rat_pri_list_info_s_type    ss_rat_pri_list_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cmd->is_resel_allowed == FALSE )
  {
    SD_ERR_1("RESEL: is_resel_allowed %d", cmd->is_resel_allowed);
    return act;
  }

  /* check if LTE is allowed by the current preferences of the UE
  */
  if( sdss_get_ss_for_mode(SD_MODE_LTE) != ss)
  {
    SD_ERR_1("RESEL: LTE not supported in mode pref for ss %d", ss);
    return act;
  }

  /* check if LTE is allowed by the current preferences of the UE
  */
  sdss_get_rat_pri_list_info(SD_SS_MAIN, &ss_rat_pri_list_info);

  if( ss == SD_SS_MAIN &&
      sdss_get_rat_pri_sys_idx( ss, SD_MODE_LTE ) == ss_rat_pri_list_info.num_items )
  {
    SD_ERR_0("RESEL: LTE not in rat_pri_list");
    return act;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_list_clr( redir_list );

  /* Add GWL system with all supported GWL bands into the REDIR list
  */
  sdprl_add_gwl_sys( redir_list);

  /* Update gwl_ltd and manual lst
  */
  sdsr_list_update_gw_sys( ss,
                           sdsr_list_get_ref( redir_list, 0),
                            FALSE,
                            FALSE,
                            FALSE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If none of the redirection channels is valid for this target & num_chan
  ** is zero, indicate that this is an invalid redirection.
  */
  if( sdsr_list_cnt(redir_list) == 0 )
  {
    redir_is_valid = FALSE;
  }
  SD_MSG_MED_1("RESEL: is_valid=%d",  redir_is_valid);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if 0
  /* Update the SS-Core with the redirection type.
  ** If this code is enabled, support should be added to clear this
  ** after timer expires or UE moves to 1x/HDR service
  **
  */
  sdss_redir_update2( SD_SS_MAIN,                /* MAIN system selection */
                      SD_SS_REDIR_IRAT_TO_LTE,        /* Redirection type */
                      redir_is_valid,      /* Valid redirection indicator */
                      FALSE,               /* RETURN_IF_FAIL set to False */
                      NULL                    /* Redirection list pointer */
                    );
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SDSR_REDIR_SYS per the redirecting system.
  */
  sdsr_list_cpy( sdss_sr_list_map2(ss, SDSR_REDIR_SYS ),
                 sdss_sr_list_map2(ss, SDSR_ACQ_SYS ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset telescopic BSR parameters on receiving IRAT indication 
  */
  if(sdss_is_c2k_alt_algr_operation() &&
     (sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE) == SD_SS_MAIN))
  {
    SD_MSG_HIGH_0("reset tele bsr due to IRAT");
    sdss_lte_telescope_parm_reset(SD_SS_MAIN);
    sdss_1x_telescope_parm_reset();
  }
  
  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss,
                          SDSS_EVT_MISC_IRAT_TO_GWL,
                          cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(redir_is_valid);

  return act;
}

#endif /* FEATURE_MMODE_LTE_RESEL */


/*===========================================================================

FUNCTION sdcmd_user_cdma_irat_lte

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_user_cdma_irat_lte(
        sd_ss_ind_user_cdma_irat_lte_cmd  *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_REJECT;
  boolean             redir_is_valid = TRUE;
  int                 num_chan, i =0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( cmd->redir_type == SD_SS_REDIR_CDMA_LTE_REDIR || cmd->redir_type == SD_SS_REDIR_CDMA_LTE_RESEL);

  if( cmd->rec_list_ptr == NULL )
  {
    SD_ERR_0("IRAT: rec list ptr NULL");
    return act;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported redirection's bands and channels in the system record
  ** table and update SDSR_REDIR_LST to reference the stored records.
  */
  sdsr_list_clr( SDSR_REDIR_LST );

  sd_ss_redir_rec_store( SDSR_REDIR_LST,
                         cmd->rec_list_ptr,
                         cmd->num_rec);

  

  /* Get total number of channels in the redirection records.
  */
  num_chan = sd_ss_redir_rec_get_num_chan( cmd->rec_list_ptr, cmd->num_rec );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If none of the redirection channels is valid for this target & num_chan
  ** is zero, indicate that this is an invalid redirection.
  */
  if( num_chan > 0 &&
      sdsr_list_cnt(SDSR_REDIR_LST) == 0 )
  {
    redir_is_valid = FALSE;
  }

  /* If all the systems in redir list is avoided then reject LTE to 1x reselection from n/w
   */
  if (cmd->redir_type == SD_SS_REDIR_CDMA_LTE_RESEL)
  {
    for( i=0; i < sdsr_list_cnt(SDSR_REDIR_LST); i++ )
    {
      if (sdss_sr_is_avoid(cmd->ss, sdsr_list_get_ptr( SDSR_REDIR_LST, i )))
      {
        redir_is_valid = FALSE;
      }
      else
      {
        redir_is_valid = TRUE;
        break;
      }
    }
    if(redir_is_valid == FALSE)
    {
      SD_MSG_HIGH_0("Resel is rejected as all systems in redir list are under avoidance");
    }

  }

  SD_MSG_HIGH_3("IRAT: num rec=%d num chan=%d, is_valid=%d", cmd->num_rec, num_chan, redir_is_valid);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the redirection type.
  */
  sdss_redir_update2( SD_SS_MAIN,          /* MAIN system selection */
                      cmd->redir_type,     /* Redirection type */
                      redir_is_valid,      /* Valid redirection indicator */
                      FALSE,               /* RETURN_IF_FAIL set to False */
                      cmd->rec_list_ptr    /* Redirection list pointer */
                    );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SDSR_REDIR_SYS per the redirecting system.
  */
  sdsr_list_cpy( SDSR_REDIR_SYS, SDSR_ACQ_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_CDMA_REDIR_LTE,
                          cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACCEPT |
                    SD_SS_ACT_REJECT) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;


}/* sd_ss_ind_user_cdma_irat_lte */

#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */


/*===========================================================================

FUNCTION sdcmd_misc_stack_local_deactivate

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_misc_stack_local_deactivate(
        sd_ss_ind_misc_service_transfer_cmd  *cmd
  )
{
  sd_ss_act_e_type    act;
  sd_ss_e_type        ss = cmd->ss;
  sd_ss_s_type        *ss_ptr         = sd_ss_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ss_ptr == NULL || !sdss_is_sglte() )
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the ss-stack has been
  ** deactivated and service is OFF.
  */
  sd_si_srv_off( ss, FALSE );


  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_MISC_LOCAL_DEACTIVATE, cmd->rtrn_act_ptr );

  /* Set the power-save reason */
  if(cmd->rtrn_act_ptr)
  {
    cmd->rtrn_act_ptr->prm.pwr_save.reas = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
  }
  ss_ptr->true_curr_act.prm.pwr_save.reas = SD_SS_PWR_SAVE_REAS_PROT_DEACT;
  ss_ptr->act.prm.pwr_save.reas           = SD_SS_PWR_SAVE_REAS_PROT_DEACT;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_PWR_SAVE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
}


/*===========================================================================

FUNCTION sdcmd_misc_stack_local_activate

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_misc_stack_local_activate(
        sd_ss_ind_misc_service_transfer_cmd  *cmd
  )
{
  sd_ss_e_type        ss = cmd->ss;
  sd_ss_s_type        *ss_ptr         = sd_ss_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ss_ptr == NULL || !sdss_is_sglte())
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  (void)sd_ss_event_proc( ss, SDSS_EVT_MISC_LOCAL_ACTIVATE, cmd->rtrn_act_ptr );

  /* Externally return act continue so no service request is sent to NAS */
  return SD_SS_ACT_CONTINUE;

}

#ifdef FEATURE_HDR
/*===========================================================================

FUNCTION sdcmd_hdr_user_ss_pref3

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_user_ss_pref3(
  sd_ss_ind_user_ss_pref_cmd  *cmd
)
{

  #ifndef FEATURE_HDR_HYBRID

  return  sdcmd_user_ss_pref7(cmd);

  #else /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr         = sd_ss_ptr( SD_SS_HYBR_HDR );
  sd_si_s_type        *si_ptr         = sd_si_ptr( SD_SS_HYBR_HDR );
  sd_ss_act_s_type    *prev_act_ptr   = NULL;
  sd_ss_act_s_type    *curr_act_ptr   = NULL;
  sdss_evt_e_type     event           = SDSS_EVT_HDR_USER_SS_PREF;
  sdss_pref_updated_e_type is_pref_change;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->pref_reason, SD_SS_PREF_REAS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->orig_mode, SD_SS_ORIG_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->mode_pref, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->roam_pref, SD_SS_ROAM_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->hybr_pref, SD_SS_HYBR_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->otasp_band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->otasp_blksys, SD_BLKSYS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If hybrid pref is none, process this as a regular SS-Pref change.
  */
  if( cmd->hybr_pref == SD_SS_HYBR_PREF_NONE &&
      sdss_is_hybr_pref( SD_SS_HYBR_HDR, SD_SS_HYBR_PREF_NONE ) )
  {
    return  sdcmd_user_ss_pref7(cmd);
  }
  prev_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (prev_act_ptr != NULL)
  {
    memset(prev_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  (void)sd_ss_hdr_act_get_true_curr_act(prev_act_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_6( "hdr:pref_reason=%d, orig_mode=%d, mode_pref=0x%x domain_pref=0x%x, acq_order_pref=0x%x prl_pref 0x%x",
                cmd->pref_reason, cmd->orig_mode, cmd->mode_pref,
                cmd->domain_pref, cmd->acq_order_pref, cmd->prl_pref );
  SD_MSG_HIGH_4( "band_pref (MSB) 0x%08x, (LSB) 0x%08x. roam_pref=0x%x, hybr_pref=%d",
               QWORD_HIGH(cmd->band_pref),
               QWORD_LOW(cmd->band_pref),
               cmd->roam_pref, 
               cmd->hybr_pref);
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If switching from non-hybrid to hybrid operation:
  **
  ** 1. set the event to be processed to SDSS_EVT_HDR_USER_HYBR_START.
  ** 2. Copy SDSR_IDL_GEO_LST into SDSR_HDR_IDL_GEO_LST
  ** 3. Call on the system record to switch the SDSR_HDR_XXX lists to
  **    reference the appropriate lists in according to the new hybrid
  **    preference.
  */
  if( ss_ptr->is_start_instance &&
      sdss_get_opr_mode(SD_SS_MAIN) == SDSS_OPR_MODE_ONLINE )
  {
    SD_MSG_HIGH_0("Start Hybrid HDR");
	
    /* Initiate LTE to HDR BSR with pref reason STACK_SYNC_UP_BSR */
    if ( cmd->pref_reason != SD_SS_PREF_REAS_STACK_SYNC_UP_BSR )
    {
      event = SDSS_EVT_HDR_USER_HYBR_START;
    }
    sdsr_list_cpy( SDSR_HDR_IDL_GEO_LST, SDSR_IDL_GEO_LST );
    if ( !sdss_is_hdr_realign_colloc_lst()
       )
    {
      SD_MSG_HIGH_1("Copy Colloc List - lst cnt=%d",
      sdsr_list_cnt(SDSR_SRV_COLLOC_LST));
      sdsr_list_cpy( SDSR_SRV_COLLOC_LST_, SDSR_SRV_COLLOC_LST );
    }
    else
    {
      SD_MSG_HIGH_0("Colloc List = 0 or 1x realign in progress");
    }
    sdsr_lists_hybr_pref_switch( cmd->hybr_pref );
    ss_ptr->is_start_instance = FALSE;
  }

  /* Else if switching from hybrid to non-hybrid operation:
  ** 1. Set the event to be processed to SDSS_EVT_HDR_USER_HYBR_END.
  ** 2. Call on the system record to switch the SDSR_HDR_XXX lists to
  **    reference the appropriate lists in according to the new hybrid
  **    preference.
  */
  else if( cmd->hybr_pref == SD_SS_HYBR_PREF_NONE )
  {
    /* send stop hybr hdr event if hybr pref on main is ON
    ** or hybr hdr stack is not in power save state
    */
    if(sd_misc_is_hybr_opr() ||
      sd_get_ss_state(SD_SS_HYBR_1) != SDSS_STATE_PWR_SAVE)
    {
      SD_MSG_HIGH_0("Stop Hybrid HDR");
      event = SDSS_EVT_HDR_USER_HYBR_END;
    }
    /* list swap to main is necessary when hybr pref is OFF */
    sdsr_lists_hybr_pref_switch( cmd->hybr_pref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(SD_IS_BIT_CONTAIN( BM(cmd->pref_reason),(uint32)SDSSSCR_PREF_REAS_VAL_CALL_ORIG) &&
     sdsr_list_cnt(SDSR_HDR_REDIAL_LST))
  {
    sdsr_list_clr(SDSR_HDR_REDIAL_LST);
    SD_MSG_HIGH_0("Clearing the redial list during call origination");
  }

  /* Call on the SS-Core to update its SS-Preference setting.
  */
  is_pref_change = sdss_hdr_pref_update( cmd->pref_reason,
                                         cmd->orig_mode,
                                         cmd->mode_pref,
                                         cmd->band_pref,
                                         cmd->prl_pref,
                                         cmd->roam_pref,
                                         cmd->hybr_pref,
                                         cmd->domain_pref,
                                         cmd->acq_order_pref
                                       );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is not SDSS_EVT_HDR_USER_HYBR_START and
  ** SS-Preferences is a optimized ss pref change. just return CONTINUE.
  */
  if( event != SDSS_EVT_HDR_USER_HYBR_START &&
      sd_is_opti_ss_pref_change( SD_SS_HYBR_HDR,
                                 is_pref_change,
                                 cmd->pref_reason,
                                 cmd->orig_mode,
                                 cmd->pref_update_reason)
    )
  {
    SD_MSG_HIGH_0( "SS-Pref didn't change - CONTINUE");

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-State is operation, inform the registered client (i.e., CM) that
    ** it is OK to originate as necessary.
    */
    if( sd_get_ss_state(SD_SS_HYBR_HDR) == SDSS_STATE_OPR )
    {
      sd_si_ok_to_orig( SD_SS_HYBR_HDR, si_ptr->data_suspend );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }
    modem_mem_free(prev_act_ptr,MODEM_MEM_CLIENT_MMODE);

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If Hybrid HDR was in operation state when 1x srv changed and we have to realign-hdr
  ** - So, Reload the GEO_LST and PREF_LST from the prl based on
  **   the HDR_SRV_SYS
  */
  if(sdss_is_hdr_opr_realign_pref_lst())
  {
    //clear the flag
    sdss_update_hdr_opr_realign_pref_lst(FALSE);

    //update the lists
    if( cmd->hybr_pref != SD_SS_HYBR_PREF_NONE &&
        (sd_get_ss_state(SD_SS_HYBR_HDR) == SDSS_STATE_OPR)
       )
    {
      /* Get the PRL designation, GEO list and more preferred list of the
      ** current serving system.
      */
      sdsr_s_type * srv_sys_ptr;
      sdsr_s_type * idl_sys_ptr;
      srv_sys_ptr = sdsr_list_get_ptr( SDSR_HDR_SRV_SYS, 0 );
      idl_sys_ptr = sdsr_list_get_ptr( SDSR_HDR_IDL_SYS, 0 );
      if(srv_sys_ptr && idl_sys_ptr)
      {
        if(sdss_is_mmss_operation(SD_SS_MAIN))
          sdprl_free_gwl_more_pref_list(SYS_SYS_MODE_HDR);
		
        (void)sdprl_get_info2(
                    SD_SS_HYBR_HDR,       /* system selection - MAIN or HDR */
                    SDSR_HDR_IDL_SYS,     /* Get PRL info for this list */
                    0,                    /* and list position */
                    SDSR_HDR_SRV_SYS,     /* Get band chan info for this list */
                    0,                    /* and list position */
                    SDSR_HDR_SRV_GEO_LST, /* Put GEO sys into this list */
                    SDSR_HDR_SRV_PREF_LST,/* Put PREF sys into this list */
                    SDSR_MAX,             /* Put collocated sys in this list */
                    SDPRL_EXTRACT_ALL,    /* Get info if diff SID or PRL match */
                    FALSE);               /* Update the srv system as channel is
                                          ** changed
                                          */
      }
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, at least one of the SS-Preferences was changed or
  ** the SS-State is operation and origination mode is other than none.
  */

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_hdr_event_proc( event, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If action is equal to previous returned action, just continue - no need
  ** to instruct the SS-Client of the same action twice.
  */
  curr_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (curr_act_ptr != NULL)
  {
    memset(curr_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  (void)sd_ss_hdr_act_get_true_curr_act(curr_act_ptr);

  if( sd_ss_act_is_equal(curr_act_ptr, prev_act_ptr, ss_ptr->meas_list_is_diff, SD_SS_MAIN))
  {
    SD_MSG_LOW_0( "Same action - CONTINUE");

    /* If SS-Action is MEAS_XXX, set the measurement ID per the pervious
    ** SS-Action's measurement ID.
    */
    if( curr_act_ptr->act == SD_SS_ACT_MEAS_BACK ||
        curr_act_ptr->act == SD_SS_ACT_MEAS_DED )
    {
      ss_ptr->meas_id = prev_act_ptr->prm.meas.id;
    }

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    act = SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-State is operation and SS-Action is CONTINUE or background
  ** measurement, inform the registered client (i.e., CM) that it is OK to
  ** originate as necessary.
  ** Or if optimized ho is in progress and SS-Action is CONTINUE, and pref
  ** reason is call orig, inform CM ok to orig.
  */
  if( ((sd_get_ss_state(SD_SS_HYBR_HDR) == SDSS_STATE_OPR) &&
      (act == SD_SS_ACT_CONTINUE || act == SD_SS_ACT_MEAS_BACK)) ||
      (sdss_is_opt_ho_lte_do() &&
       act == SD_SS_ACT_CONTINUE &&
       sdss_is_pref_reason(SD_SS_HYBR_HDR,(uint32)SDSSSCR_PREF_REAS_VAL_CALL_ORIG)))
  {
    sd_si_ok_to_orig( SD_SS_HYBR_HDR, si_ptr->data_suspend );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  modem_mem_free(prev_act_ptr,MODEM_MEM_CLIENT_MMODE);
  modem_mem_free(curr_act_ptr,MODEM_MEM_CLIENT_MMODE);

  return act;

  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_ind_hdr_user_ss_pref2 */


#endif /* FEATURE_HDR */


#ifdef FEATURE_HDR
/*===========================================================================

FUNCTION sdcmd_hdr_user_prot_deactivate

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_user_prot_deactivate(
      sd_ss_ind_user_prot_deactivate_cmd  *cmd
)
{
  sd_ss_act_e_type    act;
  #ifndef FEATURE_HDR_HYBRID
  return sdcmd_user_prot_deactivate(cmd);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If currently not doing CDMA+HDR hybrid operation, call on
  ** sd_ss_ind_user_prot_deactivate() to process this event.
  */
  if( !sd_misc_is_hybr_opr() )
  {
    return sdcmd_user_prot_deactivate(cmd);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the SS-Main protocol is
  ** being deactivated.
  */
  sd_si_srv_off( SD_SS_HYBR_HDR, FALSE);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_hdr_event_proc( SDSS_EVT_USER_PROT_DEACTIVATE, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_PWR_SAVE  );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_user_prot_deactivate */

#endif /* FEATURE_HDR */


/*===========================================================================

FUNCTION sdcmd_misc_meas_rprt

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_misc_meas_rprt(
  sd_ss_ind_misc_meas_rprt_cmd  *cmd
 )
{
  #ifdef FEATURE_HYBR_3GPP
  return sdcmd_hybr_2_misc_meas_rprt(cmd);
  #elif defined(CFG_HYBR_GW_3)
  return sdcmd_hybr_3_misc_meas_rprt(cmd);
  #else
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_MAIN);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement ID does not match the ID of the measurement request,
  ** instruct SS-Client to CONTINUE.
  */
  if( cmd->meas_id != ss_ptr->meas_id )
  {
    SD_MSG_HIGH_2( "meas_id=%d mismatch - req_id=%d",
                 cmd->meas_id,ss_ptr->meas_id);

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the measurement ID matches the ID of the measurement
  ** request.
  */

  /* Indicate that measurement is done being serviced.
  */
  ss_ptr->meas_is_done = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If reselection is not allowed, instruct SS-Client to CONTINUE.
  */
  if( ! cmd->resel_is_allowed )
  {
    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the measurement ID matches the ID of the measurement
  ** request and reselection is allowed, so process the SS-Event that is
  ** associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_MEAS_RPRT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
  #endif //CFG_HYBR_GW

} /* sd_ss_ind_misc_meas_rprt */


#ifdef FEATURE_HDR
/*===========================================================================

FUNCTION sdcmd_hdr_user_redir_lte

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type sdcmd_hdr_user_redir_lte(
     sd_ss_ind_hdr_user_redir_lte_cmd  *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;
  int                 i;
  sd_sys_s_type       sys;
  sd_ss_e_type        ss = sd_misc_get_ss_for_mode(SYS_SYS_MODE_HDR);
  sd_ss_e_type        lte_ss = sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE);
  sd_ss_s_type        *ss_ptr         = sd_ss_ptr( SD_SS_HYBR_HDR );
  sdsr_ref_type       sr_ref;
  int                 avail_entries;
  sdsr_s_type         *tmp_sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd->redir_type   != SD_SS_REDIR_HDR_LTE &&
       cmd->redir_type   != SD_SS_REDIR_HDR_LTE_RESEL &&
       cmd->redir_type   != SD_SS_REDIR_HDR_LTE_RESEL_OPT_HO) ||
       cmd->sys_get_func == NULL ||
       ss           == SD_SS_MAX
    )
    {
      /* Ignore this redirection
      */
      SD_MSG_MED_2("IRAT: type %d ss %d",cmd->redir_type,ss);

      /* If SS-Client supplied a buffer for the return SS-Action, set the
      ** returned SS-Action in this buffer to PWR_SAVE.
      */
      if( cmd->rtrn_act_ptr != NULL )
      {
        cmd->rtrn_act_ptr->act = SD_SS_ACT_PWR_SAVE;
      }

      SD_MSG_MED_0( "Redir ignored - received in non-hybrid state");

      return SD_SS_ACT_PWR_SAVE;
    }

  /* Hybrid switch should happens follow redirection, so we need to clear
  ** the flag that trigger hybrid starting "is_start_instance"
  */
  if( ss_ptr->is_start_instance )
  {
    SD_MSG_HIGH_0("Clear Hybrid Flag Follow Redirection and switch hybr pref");

    sdsr_list_cpy( SDSR_HDR_IDL_GEO_LST, SDSR_IDL_GEO_LST );

    sdsr_lists_hybr_pref_switch( sdss_get_curr_hybr_pref(SD_SS_MAIN) );

    ss_ptr->is_start_instance = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out the number of system record table entries that are available
  ** for storing the redirection's channels.
  */
  avail_entries = sdsr_tbl_size() - sdsr_list_cnt(SDSR_PRL_LST);

  SD_MSG_MED_3("IRAT: ss=%d, avail_entries %d, redir_type=%d", ss, avail_entries, cmd->redir_type);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported redirection's channels in the system record table and
  ** update SDSR_REDIR_LST to reference the stored records.
  */
  sdsr_list_clr( SDSR_HDR_REDIR_LST );


  for( i=0; cmd->sys_get_func(cmd->data_ptr, i, &sys); i++ )
  {
    SD_ASSERT_ENUM_IS_INRANGE( sys.mode, SD_MODE_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( (sd_band_e_type) sys.band, SD_BAND_MAX );
    SD_ASSERT( i < avail_entries );
    SD_MSG_HIGH_3("IRAT: mode %d band 0x%lx chan %d",
                sys.mode, sys.band, sys.chan);

    /* If MS supports this system and we still have space, store the system
    ** in the system record table.
    */
    if( sdss_is_supp_sys(
                   sys.mode,
                   sd_map_band_to_band_pref((sd_band_e_type)sys.band),
                   sys.chan) && i < avail_entries )
    {
      sr_ref = sdsr_tbl_store2(
                      sys.mode,
                      sd_map_band_to_band_pref((sd_band_e_type)sys.band),
                      sys.chan );

      /* Set the expected SID to wildcard SID. */
      tmp_sr_ptr = sdsr_tbl_get_ptr(sr_ref);
      if( tmp_sr_ptr == NULL)
      {
        SD_MSG_LOW_0("tmp_sr_ptr NULL");
        continue;
      }
      tmp_sr_ptr->sid_expected.prm.is856.subnet_mask_len = 0;

      sdsr_list_insert_ref( SDSR_HDR_REDIR_LST,
                            SDSR_POS_NULL,
                            sr_ref,
                            FALSE );

      SD_MSG_HIGH_0("IRAT: sys inserted");

    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the redirection type.
  */
  sdss_redir_update(
          ss,                           /* System selection - MAIN or HDR */
          cmd->redir_type,              /* Set redirection type to HDR */
          TRUE,                         /* Set is_valid to TRUE */
          SD_WILDCARD_SID,              /* Set expected SID to wildcard-SID*/
          SD_WILDCARD_NID,              /* Set expected NID to wildcard-NID*/
          FALSE,                        /* Set is_rif to FALSE */
          FALSE,                        /* Set is_ignore_cdma to FALSE */
          SD_SS_MAX_REDIR_DELAY_NONE ); /* Set max_redirect_delay to none */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is opt HO LTE->ehrpd, set the flag is_opt_ho_lte_do to true.
   */
  if (cmd->redir_type == SD_SS_REDIR_HDR_LTE_RESEL_OPT_HO)
  {
    sdss_update_opt_ho_lte_do(TRUE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SDSR_REDIR_SYS per the redirecting system.
  */
  sdsr_list_cpy( SDSR_HDR_REDIR_SYS, SDSR_HDR_ACQ_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_USER_REDIR_LTE, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_HDR   |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* if redirection is happening to hybrid HDR, update the IRAT flag
  */
  if( act == SD_SS_ACT_ACQ_HDR &&
      ss  == SD_SS_HYBR_HDR    &&
      lte_ss != SD_SS_MAX )
  {
    SD_MSG_HIGH_1("IRAT:set flag ss %d",lte_ss);
    sdss_set_flag(lte_ss, SDSS_FLAG_HYBR_HDR_IRAT, TRUE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

}/* sd_ss_ind_hdr_user_redir_lte */


/*===========================================================================

FUNCTION sdcmd_hybr_1_user_pwr_save_exit

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hybr_1_user_pwr_save_exit(
      sd_cmd_t  *cmd
)
{

  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_hdr_event_proc( SDSS_EVT_HDR_USER_PWR_SAVE_EXIT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_HDR |
                    SD_SS_ACT_PWR_SAVE ) );
  return act;

} /* sd_ss_ind_hdr_user_pwr_save_exit() */


/*===========================================================================

FUNCTION sdcmd_hdr_misc_meas_rprt

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_misc_meas_rprt(
      sd_ss_ind_misc_meas_rprt_cmd  *cmd
)
{
  #ifndef FEATURE_HDR_HYBRID

  return sd_ss_ind_misc_meas_rprt(cmd);

  #else /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_HYBR_HDR);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If currently not doing CDMA+HDR hybrid operation, call on
  ** sd_ss_ind_misc_meas_rprt().
  */
  if( !sd_misc_is_hybr_opr() )
  {
    return sdcmd_misc_meas_rprt(cmd);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If measurement ID does not match the ID of the measurement request,
  ** instruct SS-Client to CONTINUE.
  */
  if( cmd->meas_id != ss_ptr->meas_id )
  {
    SD_MSG_HIGH_2( "meas_id=%d mismatch - req_id=%d",
                 cmd->meas_id,ss_ptr->meas_id);

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the measurement ID matches the ID of the measurement
  ** request.
  */

  /* Indicate that measurement is done being serviced.
  */
  ss_ptr->meas_is_done = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If reselection is not allowed, instruct SS-Client to CONTINUE.
  */
  if( ! cmd->resel_is_allowed )
  {
    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the measurement ID matches the ID of the measurement
  ** request and reselection is allowed, so process the SS-Event that is
  ** associated with this SS-Indication.
  */
  act = sd_ss_hdr_event_proc( SDSS_EVT_MISC_MEAS_RPRT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

  #endif /* FEATURE_HDR_HYBRID */

} /* sd_ss_ind_hdr_misc_meas_rprt */


/*===========================================================================

FUNCTION sdcmd_hdr_acq_failed_pilot

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_acq_failed_pilot(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  sd_ss_e_type        ss = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_ACQ_FAILED_PILOT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_acq_failed_pilot */


/*===========================================================================

FUNCTION sdcmd_hdr_acq_failed_sync

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS 

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_acq_failed_sync(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  sd_ss_e_type        ss = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_ACQ_FAILED_SYNC, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_acq_failed_sync */


/*===========================================================================

FUNCTION sdcmd_hdr_acq_prot_mis

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_acq_prot_mis(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  sd_ss_e_type        ss = SD_SS_MAIN;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_ACQ_PROT_MIS, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_acq_prot_mis */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_prot_mis

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_prot_mis(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  sd_ss_e_type        ss = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_PROT_MIS, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_opr_prot_mis */


/*===========================================================================

FUNCTION sdcmd_hdr_acq_sid2

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_acq_sid2(
         sd_ss_ind_hdr_acq_sid_cmd *cmd
)
{
  sd_ss_act_e_type  act;
  boolean           is_srv_on;

  /* Point at acquired system.
  */
  sdsr_ref_type             acq_sys_ref   =
                                    sdsr_list_get_ref( SDSR_HDR_ACQ_SYS, 0 );
  sdsr_s_type               *acq_sys_ptr  =
                                    sdsr_list_get_ptr( SDSR_HDR_ACQ_SYS, 0 );

  /* Point at candidate system.
  */
  sdsr_s_type               *cand_sys_ptr =
                                    sdsr_list_get_ptr( SDSR_HDR_CAND_SYS, 0);

  sdsr_s_type               *cdma_srv_sys_ptr =
                                    sdsr_list_get_ptr( SDSR_CDMA_IDL_SYS, 0 );

  /* Assume no hybrid operation.
  */
  sd_ss_e_type      ss        = SD_SS_MAIN;

  sys_active_prot_e_type hdr_active_prot = SYS_ACTIVE_PROT_NONE;

  sys_personality_e_type hdr_personality = SYS_PERSONALITY_NONE;

  sdss_sys_addn_info_s_type curr_sys_info;

  word                      geo_sys_idx   = 0xFFFF;

  sd_ss_e_type              lte_ss = sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE);

  sd_si_s_type              *si_ptr = NULL;

  sys_plmn_mcc_list         *hdr_mcc_list_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_sys_ref != SDSR_REF_NULL );
  SD_ASSERT( acq_sys_ptr != NULL );
  SD_ASSERT( cmd->sid_ptr != NULL );

  if( acq_sys_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  SD_MSG_HIGH_1("SRV_3GPP2: hdr_acq_sid, mcc=0x%x", cmd->mcc);
  sdsr_hdr_sid_print( cmd->sid_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the additional sys info structure */
  sdss_addn_sys_info_init(&curr_sys_info);

  /* Initialize the plmn info list for DO */
  hdr_mcc_list_ptr = modem_mem_alloc
                    (sizeof(sys_plmn_mcc_list), MODEM_MEM_CLIENT_MMODE);
  if(hdr_mcc_list_ptr)
  {
    memset(hdr_mcc_list_ptr,0, sizeof(sys_plmn_mcc_list));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

#ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss = SD_SS_HYBR_HDR;
  }
#endif /* FEATURE_HDR_HYBRID */

  si_ptr       = sd_si_ptr( ss );

  /*  We want to keep previous personality when sd_ss_ind_hdr_acq_sid() is called.
   *  Personality is only updated when client calls sd_ss_ind_hdr_opr_oh_info5().
   */
  hdr_personality = si_ptr->si_info.hdr_personality;

  hdr_active_prot = si_ptr->si_info.hdr_active_prot;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_EOOS
   #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remember whether the service indicator is ON.
  */
  is_srv_on = sd_si_is_srv_on( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID of the acquired system per the SCHM's SID.
  */
  memscpy( acq_sys_ptr->sid.is856.sector_id,
           sizeof(acq_sys_ptr->sid.is856.sector_id),
           cmd->sid_ptr,
           sizeof(acq_sys_ptr->sid.is856.sector_id) );

  acq_sys_ptr->sid.is856.subnet_mask_len = cmd->subnet_mask_len;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the channel list according to the hashing channel list.
  */
  sd_ss_chan_list_update( SDSR_HDR_ACQ_CHAN_LST,
                          SD_MODE_HDR,
                          cmd->chan_ptr,
                          cmd->num_chan,
                          SDSR_HDR_ACQ_SYS,
                          NULL,
                          SDSR_MAX,
                          SDSR_MAX,
                          SDSR_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the
  ** acquired system.
  */
  geo_sys_idx =
    sdprl_get_info3(
              ss,                     /* system selection - MAIN or HDR */
              SDSR_HDR_ACQ_SYS,       /* Get PRL info for this list */
              0,                      /* and list position */
              SDSR_HDR_ACQ_SYS,       /* Get system info for SRV list */
              0,                      /* and list position */
              SDSR_HDR_ACQ_GEO_LST,   /* Put GEO systems into this list */
              SDSR_HDR_ACQ_PREF_LST,  /* Put PREF systems into this list */
              SDSR_HDR_ACQ_COLLOC_LST,/* Put collocated sys in this list */
              SDPRL_EXTRACT_ALL,      /* Get info regardless of PRL match */
              FALSE,                  /* Indicate if srv sys is changed */
              hdr_mcc_list_ptr);      /* HDR plmn mcc list */

  sdss_update_mcc_list_info(ss,hdr_mcc_list_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Insert the acquired system at the beginning of the SDSR_ACQED_LST and
  ** remove any duplicates.
  */
  sdsr_list_insert_ref( sdss_sr_list_map2(ss,SDSR_ACQED_LST), 0, acq_sys_ref, TRUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the acquired system does not conflict with the current SS-Preference
  ** and it is designated as an available in the PRL, append the acquired
  ** system to the AVAIL_LST list.
  */
  if( sdss_sr_is_pref_conf(ss, acq_sys_ptr, SDSS_PREF_CONF_NONE) &&
      sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_AVAIL) )
  {
    sdsr_list_app( SDSR_HDR_AVAIL_LST, SDSR_HDR_ACQ_SYS );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_ACQ_SID, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE (i.e., we have selected a new serving system),
  ** do the following updates.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {

    /* Update the expected SID/NID of the acquired system per the acquired
    ** SID/NID.
    */
    memscpy( acq_sys_ptr->sid_expected.prm.is856.sector_id,
             sizeof(acq_sys_ptr->sid_expected.prm.is856.sector_id),
             cmd->sid_ptr,
             sizeof(acq_sys_ptr->sid_expected.prm.is856.sector_id) );

    acq_sys_ptr->sid_expected.prm.is856.subnet_mask_len = cmd->subnet_mask_len;

    /* Get the additional system info of the acquired system.
    */
    sdprl_get_addn_sys_record_info(acq_sys_ptr,
                                   &(curr_sys_info.assn_tag),
                                   &(curr_sys_info.geo_offset),
                                   &(curr_sys_info.geo_count));

    /* Store the geo index of the current system
    */
    curr_sys_info.geo_sys_idx = geo_sys_idx;

    /* Update the system indicators per the acquired system.
    */
    sd_si_update_if_srv_is_off( ss,
                                SD_SI_UPDATE_ACQ,
                                SYS_SRV_STATUS_SRV,
                                SYS_SRV_DOMAIN_PS_ONLY,
                                acq_sys_ref,
                                NULL,
                                NULL,
                                NULL,
                                SDSR_HDR_ACQ_COLLOC_LST,
                                &hdr_active_prot,
                                &hdr_personality,
                                &curr_sys_info,
                                SYS_SRV_STATUS_NO_SRV,
                                SD_SIB8_NO_CHANGE,
                                SYS_EXTEND_SRV_INFO_NONE,
                                SYS_REG_DOMAIN_NOT_APPLICABLE,
                                NULL
                                );

    if( !is_srv_on )
    {
      /* Update the additional system info in sd state with that of
      ** the acquired system.
      */
      sdss_set_prev_sys_info(ss,
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }

    /* Update the candidate system per the acquired system.
    */
    sdsr_list_put_ref( SDSR_HDR_CAND_SYS, acq_sys_ref );

    /* Update the serving system per the acquired system.
    */
    sd_ss_hdr_srv_sys_update( SDSR_HDR_ACQ_SYS,
                              SDSR_HDR_ACQ_GEO_LST,
                              SDSR_HDR_ACQ_PREF_LST,
                              SDSR_MAX ); //Dont update colloc lst

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If device enters KDDI network and if SDSSSCR_EFS_ITEM_LOC_BASE_BSR is
        ** enabled use longer bsr otherwise use default
        */
    if( lte_ss != SD_SS_MAX )
    {
      if((sdss_is_flag_set(lte_ss, SDSS_FLAG_MCC_MATCH_FOUND)) && (sd_efs_is_loc_bsr_enabled()))
      {
        if(!sdss_is_flag_set(lte_ss,SDSS_FLAG_LOC_BASE_BSR))
        {
          sdss_set_flag(lte_ss,SDSS_FLAG_LOC_BASE_BSR,TRUE);
          SD_MSG_MED_0(" SDSS_FLAG_LOC_BASE_BSR  set to TRUE  ");
        }
      }
      else
      {
        if(sdss_is_flag_set(lte_ss,SDSS_FLAG_LOC_BASE_BSR))
        {
          sdss_set_flag(lte_ss,SDSS_FLAG_LOC_BASE_BSR,FALSE);
          SD_MSG_MED_0(" SDSS_FLAG_LOC_BASE_BSR  set to FALSE  ");
        }
      }
    }

    
    /* Reset HDR deact by Hybr2 BSR flag to FALSE 
    */
    sdss_set_flag(SD_SS_HYBR_2,SDSS_FLAG_HDR_DEACT_BY_HYBR2_BSR, FALSE);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Reset the event counters if the system SID changed.
    */
    if( !sdss_event_is_same_is856_sid( ss, cmd->sid_ptr, cmd->subnet_mask_len) )
    {
      sdss_event_cnt_update( ss, SDSS_EVT_CNT_UPDT_REAS_SID_CHANGED );

      sdss_event_is856_sid_update( ss, cmd->sid_ptr );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Update the idle cand system per acq_sys
    */
    sd_ss_idle_cand_sys_update( SDSR_HDR_CAND_IDL_SYS,
                                ss,
                                SDSR_MAX,
                                SDSR_HDR_ACQ_SYS);

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (we have not yet selected a serving system), if the acquired system
  ** is allowed by the PRL and complies with the current SS-Preference do the
  ** following updates:
  */
  /* for acq_type COLLOC_BSET_PREF, incase if a non collocated ststem is found,
   ** SD core  rejects the system and gives next action ACQ_HDR/PWR_SAVE.
   ** in that case do not update candidate system and not send srv_ind to CM
  */
  else if( sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED) &&
           sdss_sr_is_pref_conf(ss, acq_sys_ptr, SDSS_PREF_CONF_NONE) &&
           ((sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_COLLOC &&
           sdss_is_sr_associated(acq_sys_ptr, cdma_srv_sys_ptr)) ||
           sdss_get_orig_mode(ss) != SD_SS_ORIG_MODE_COLLOC) )
  {
    /* If the candidate system is NULL or the acquired system is more
    ** preferred than the candidate system, update the candidate system per
    ** the acquired system and reorder the available list according to the
    ** new candidate system.
    */
    if( cand_sys_ptr == NULL ||
        sdprl_is_more_pref(ss, acq_sys_ptr, cand_sys_ptr) )
    {
      sdsr_list_put_ref( SDSR_HDR_CAND_SYS, acq_sys_ref );
      sdsr_list_order( SDSR_HDR_AVAIL_LST, SDSR_HDR_CAND_SYS );
    }

    /* If before processing the SS-Event the service indicator was OFF,
    ** update the system indicators per the acquired system.
    */
    if( ! is_srv_on )
    {
      /* Get the additional system info of the acquired system.
      */
      sdprl_get_addn_sys_record_info(acq_sys_ptr,
                                     &(curr_sys_info.assn_tag),
                                     &(curr_sys_info.geo_offset),
                                     &(curr_sys_info.geo_count));

      sd_si_update( ss,
                    SD_SI_UPDATE_ACQ,
                    SYS_SRV_STATUS_SRV,
                    SYS_SRV_DOMAIN_PS_ONLY,
                    acq_sys_ref,
                    FALSE,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    SDSR_HDR_ACQ_COLLOC_LST,
                    &hdr_active_prot,
                    &hdr_personality,
                    &curr_sys_info,
                    SYS_SRV_STATUS_NO_SRV,
                    SYS_LTE_CS_CAPABILITY_NONE,
                    SD_SIB8_NO_CHANGE,
                    FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                    0,
                    SYS_EXTEND_SRV_INFO_NONE,
                    SYS_REG_DOMAIN_NOT_APPLICABLE,
                    NULL,
                    FALSE);

      /* Update the additional system info in sd state with that of
      ** the acquired system.
      */
      sdss_set_prev_sys_info(ss,
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }
  }

  if(hdr_mcc_list_ptr)
  {
    modem_mem_free(hdr_mcc_list_ptr, MODEM_MEM_CLIENT_MMODE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
}


/*===========================================================================

FUNCTION sdcmd_hdr_acq_sid3

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_acq_sid3(
      sd_ss_ind_hdr_acq_sid_cmd *cmd
)
{
  sd_ss_act_e_type  act;
  boolean           is_srv_on;

  /* Point at acquired system.
  */
  sdsr_ref_type             acq_sys_ref   =
                                    sdsr_list_get_ref( SDSR_HDR_ACQ_SYS, 0 );
  sdsr_s_type               *acq_sys_ptr  =
                                    sdsr_list_get_ptr( SDSR_HDR_ACQ_SYS, 0 );

  /* Point at candidate system.
  */
  sdsr_s_type               *cand_sys_ptr =
                                    sdsr_list_get_ptr( SDSR_HDR_CAND_SYS, 0);

  sdsr_s_type               *cdma_srv_sys_ptr =
                                    sdsr_list_get_ptr( SDSR_CDMA_IDL_SYS, 0 );

  /* Assume no hybrid operation.
  */
  sd_ss_e_type      ss        = SD_SS_MAIN;

  sys_active_prot_e_type hdr_active_prot = SYS_ACTIVE_PROT_NONE;

  sys_personality_e_type hdr_personality = SYS_PERSONALITY_NONE;

  sdss_sys_addn_info_s_type curr_sys_info;

  word                      geo_sys_idx   = 0xFFFF;
  sd_ss_e_type              lte_ss = sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE);

  sd_si_s_type              *si_ptr = NULL;
  sys_plmn_mcc_list         *hdr_mcc_list_ptr = NULL;
  sd_si_mode_info_u_type     mode_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( acq_sys_ref != SDSR_REF_NULL );
  SD_ASSERT( acq_sys_ptr != NULL );
  SD_ASSERT( cmd->sid_ptr != NULL );

  SD_MSG_HIGH_1("SRV_3GPP2: hdr_acq_sid, mcc=0x%x", cmd->mcc);
  sdsr_hdr_sid_print( cmd->sid_ptr );

  if( acq_sys_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the additional sys info structure */
  sdss_addn_sys_info_init(&curr_sys_info);
  
  /* Initialize the plmn info list for DO */
  hdr_mcc_list_ptr = modem_mem_alloc
                    (sizeof(sys_plmn_mcc_list), MODEM_MEM_CLIENT_MMODE);
  if(hdr_mcc_list_ptr)
  {
    memset(hdr_mcc_list_ptr,0, sizeof(sys_plmn_mcc_list));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss = SD_SS_HYBR_HDR;
  }
  #endif /* FEATURE_HDR_HYBRID */

  si_ptr       = sd_si_ptr( ss );

  /*  We want to keep previous personality when sd_ss_ind_hdr_acq_sid() is called.
   *  Personality is only updated when client calls sd_ss_ind_hdr_opr_oh_info5().
   */
  hdr_personality = si_ptr->si_info.hdr_personality;
  hdr_active_prot = si_ptr->si_info.hdr_active_prot;
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_EOOS
   #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remember whether the service indicator is ON.
  */
  is_srv_on = sd_si_is_srv_on( ss );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID of the acquired system per the SCHM's SID.
  */
  memscpy( acq_sys_ptr->sid.is856.sector_id,
           sizeof(acq_sys_ptr->sid.is856.sector_id),
           cmd->sid_ptr,
           sizeof(acq_sys_ptr->sid.is856.sector_id) );

  acq_sys_ptr->sid.is856.subnet_mask_len = cmd->subnet_mask_len;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the channel list according to the hashing channel list.
  */
  sd_ss_chan_list_update( SDSR_HDR_ACQ_CHAN_LST,
                          SD_MODE_HDR,
                          cmd->chan_ptr,
                          cmd->num_chan,
                          SDSR_HDR_ACQ_SYS,
                          NULL,
                          SDSR_MAX,
                          SDSR_MAX,
                          SDSR_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the
  ** acquired system.
  */
  geo_sys_idx =
    sdprl_get_info3(
              ss,                     /* system selection - MAIN or HDR */
              SDSR_HDR_ACQ_SYS,       /* Get PRL info for this list */
              0,                      /* and list position */
              SDSR_HDR_ACQ_SYS,       /* Get system info for SRV list */
              0,                      /* and list position */
              SDSR_HDR_ACQ_GEO_LST,   /* Put GEO systems into this list */
              SDSR_HDR_ACQ_PREF_LST,  /* Put PREF systems into this list */
              SDSR_HDR_ACQ_COLLOC_LST,/* Put collocated sys in this list */
              SDPRL_EXTRACT_ALL,      /* Get info regardless of PRL match */
              FALSE,                  /* Indicate if srv sys is changed */
              hdr_mcc_list_ptr);      /* HDR plmn mcc list */

  sdss_update_mcc_list_info(ss,hdr_mcc_list_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Insert the acquired system at the beginning of the SDSR_ACQED_LST and
  ** remove any duplicates.
  */
  sdsr_list_insert_ref( sdss_sr_list_map2(ss,SDSR_ACQED_LST), 0, acq_sys_ref, TRUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the acquired system does not conflict with the current SS-Preference
  ** and it is designated as an available in the PRL, append the acquired
  ** system to the AVAIL_LST list.
  */
  if( sdss_sr_is_pref_conf(ss, acq_sys_ptr, SDSS_PREF_CONF_NONE) &&
      sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_AVAIL) )
  {
    sdsr_list_app( SDSR_HDR_AVAIL_LST, SDSR_HDR_ACQ_SYS );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_ACQ_SID, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE (i.e., we have selected a new serving system),
  ** do the following updates.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    /* If device enters KDDI network and if SDSSSCR_EFS_ITEM_LOC_BASE_BSR is
    ** enabled use longer bsr otherwise use default
    */
    if(lte_ss != SD_SS_MAX)
    {
      if((cmd->mcc == KDDI_MCC_VAL) && (sd_efs_is_loc_bsr_enabled()))
      {
        if(!sdss_is_flag_set(lte_ss,SDSS_FLAG_LOC_BASE_BSR))
        {
          sdss_set_flag(lte_ss,SDSS_FLAG_LOC_BASE_BSR,TRUE);
        }
      }
      else
      {
        if(sdss_is_flag_set(lte_ss,SDSS_FLAG_LOC_BASE_BSR))
        {
          sdss_set_flag(lte_ss,SDSS_FLAG_LOC_BASE_BSR,FALSE);
        }
      }
    }

    
    /* Reset HDR deact by Hybr2 BSR flag to FALSE 
    */
    sdss_set_flag(SD_SS_HYBR_2,SDSS_FLAG_HDR_DEACT_BY_HYBR2_BSR, FALSE);


    /* Update the expected SID/NID of the acquired system per the acquired
    ** SID/NID.
    */
    memscpy( acq_sys_ptr->sid_expected.prm.is856.sector_id,
             sizeof(acq_sys_ptr->sid_expected.prm.is856.sector_id),
             cmd->sid_ptr,
             sizeof(acq_sys_ptr->sid_expected.prm.is856.sector_id) );

    acq_sys_ptr->sid_expected.prm.is856.subnet_mask_len = cmd->subnet_mask_len;

    /* Get the additional system info of the acquired system.
    */
    sdprl_get_addn_sys_record_info(acq_sys_ptr,
                                   &(curr_sys_info.assn_tag),
                                   &(curr_sys_info.geo_offset),
                                   &(curr_sys_info.geo_count));

    /* Store the geo index of the current system
    */
    curr_sys_info.geo_sys_idx = geo_sys_idx;

    mode_info.hdr.hdr_mcc = cmd->mcc;

    /* Update the system indicators per the acquired system.
    */
    sd_si_update_if_srv_is_off( ss,
                                SD_SI_UPDATE_ACQ,
                                SYS_SRV_STATUS_SRV,
                                SYS_SRV_DOMAIN_PS_ONLY,
                                acq_sys_ref,
                                NULL,
                                NULL,
                                NULL,
                                SDSR_HDR_ACQ_COLLOC_LST,
                                &hdr_active_prot,
                                &hdr_personality,
                                &curr_sys_info,
                                SYS_SRV_STATUS_NO_SRV,
                                SD_SIB8_NO_CHANGE,
                                SYS_EXTEND_SRV_INFO_NONE,
                                SYS_REG_DOMAIN_NOT_APPLICABLE,
                                &mode_info);

    if( !is_srv_on )
    {
      /* Update the additional system info in sd state with that of
      ** the acquired system.
      */
      sdss_set_prev_sys_info(ss,
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }

    /* Update the candidate system per the acquired system.
    */
    sdsr_list_put_ref( SDSR_HDR_CAND_SYS, acq_sys_ref );

    /* Update the serving system per the acquired system.
    */
    sd_ss_hdr_srv_sys_update( SDSR_HDR_ACQ_SYS,
                              SDSR_HDR_ACQ_GEO_LST,
                              SDSR_HDR_ACQ_PREF_LST,
                              SDSR_MAX ); //Dont update colloc lst

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Reset the event counters if the system SID changed.
    */
    if( !sdss_event_is_same_is856_sid( ss, cmd->sid_ptr, cmd->subnet_mask_len) )
    {
      sdss_event_cnt_update( ss, SDSS_EVT_CNT_UPDT_REAS_SID_CHANGED );

      sdss_event_is856_sid_update( ss, cmd->sid_ptr );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Update the idle cand system per acq_sys
    */
    sd_ss_idle_cand_sys_update( SDSR_HDR_CAND_IDL_SYS,
                                ss,
                                SDSR_MAX,
                                SDSR_HDR_ACQ_SYS);

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (we have not yet selected a serving system), if the acquired system
  ** is allowed by the PRL and complies with the current SS-Preference do the
  ** following updates:
  */
  /* for acq_type COLLOC_BSET_PREF, incase if a non collocated ststem is found,
   ** SD core  rejects the system and gives next action ACQ_HDR/PWR_SAVE.
   ** in that case do not update candidate system and not send srv_ind to CM
  */
  else if( sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED) &&
           sdss_sr_is_pref_conf(ss, acq_sys_ptr, SDSS_PREF_CONF_NONE) &&
           ((sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_COLLOC &&
           sdss_is_sr_associated(acq_sys_ptr, cdma_srv_sys_ptr)) ||
           sdss_get_orig_mode(ss) != SD_SS_ORIG_MODE_COLLOC) )
  {
    /* If the candidate system is NULL or the acquired system is more
    ** preferred than the candidate system, update the candidate system per
    ** the acquired system and reorder the available list according to the
    ** new candidate system.
    */
    if( cand_sys_ptr == NULL ||
        sdprl_is_more_pref(ss, acq_sys_ptr, cand_sys_ptr) )
    {
      sdsr_list_put_ref( SDSR_HDR_CAND_SYS, acq_sys_ref );
      sdsr_list_order( SDSR_HDR_AVAIL_LST, SDSR_HDR_CAND_SYS );
    }

    /* If before processing the SS-Event the service indicator was OFF,
    ** update the system indicators per the acquired system.
    */
    if( ! is_srv_on )
    {
      /* Get the additional system info of the acquired system.
      */
      sdprl_get_addn_sys_record_info(acq_sys_ptr,
                                     &(curr_sys_info.assn_tag),
                                     &(curr_sys_info.geo_offset),
                                     &(curr_sys_info.geo_count));

      sd_si_update( ss,
                    SD_SI_UPDATE_ACQ,
                    SYS_SRV_STATUS_SRV,
                    SYS_SRV_DOMAIN_PS_ONLY,
                    acq_sys_ref,
                    FALSE,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    SDSR_HDR_ACQ_COLLOC_LST,
                    &hdr_active_prot,
                    &hdr_personality,
                    &curr_sys_info,
                    SYS_SRV_STATUS_NO_SRV,
                    SYS_LTE_CS_CAPABILITY_NONE,
                    SD_SIB8_NO_CHANGE,
                    FALSE,
                    SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                    0,
                    SYS_EXTEND_SRV_INFO_NONE,
                    SYS_REG_DOMAIN_NOT_APPLICABLE,
                    NULL,
                    FALSE);

      /* Update the additional system info in sd state with that of
      ** the acquired system.
      */
      sdss_set_prev_sys_info(ss,
                             curr_sys_info.assn_tag,
                             curr_sys_info.geo_offset,
                             curr_sys_info.geo_count);
    }
  }

  if(hdr_mcc_list_ptr)
  {
    modem_mem_free(hdr_mcc_list_ptr, MODEM_MEM_CLIENT_MMODE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_acq_sid3 *//*lint -esym(715,subnet_mask_len) */


/*===========================================================================

FUNCTION sdcmd_hdr_acq_redir_abort

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type sdcmd_hdr_acq_redir_abort(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type  act;
  sd_ss_e_type      ss = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sd_misc_is_hybr_opr() )
  {
    ss = SD_SS_HYBR_1;
  }

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_ACQ_REDIR_ABORT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_HDR|SD_SS_ACT_PWR_SAVE) );

  return act;

}/* sd_ss_ind_hdr_acq_redir_abort */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_oh_info

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_oh_info(
      sd_ss_ind_hdr_opr_oh_info_cmd *cmd
)
{
  sd_ss_act_e_type      act;
  boolean               chan_is_changed   = FALSE;
  boolean               sid_is_changed    = FALSE;
  sdss_state_e_type     prev_ss_state;
  sdss_state_e_type     ss_state;
  sdprl_extract_e_type  extract           = SDPRL_EXTRACT_ALL;

  sdsr_ref_type         srv_sys_ref;
  sdsr_s_type           *srv_sys_ptr;
  sdsr_s_type           *acq_sys_ptr;

  byte                  expected_sid[16];
  byte                  expected_subnet_mask_len;

  /* Assume no hybrid operation.
  */
  sd_ss_e_type          ss                = SD_SS_MAIN;
  sd_si_s_type          *si_ptr           = sd_si_ptr( SD_SS_MAIN );

  sdss_sys_addn_info_s_type
                        curr_sys_info;
  word                  geo_sys_idx   = 0xFFFF;
  sd_si_info_s_type    *si_info_ptr = &sd_si_ptr(SD_SS_HYBR_1)->si_info;
  sd_si_mode_info_u_type  mode_info;


  sd_ss_e_type              lte_ss = sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE);
  sys_plmn_mcc_list         *hdr_mcc_list_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->band, SD_BAND_MAX );
  #ifdef SD_DEBUG
  #error code not present
#endif
  SD_ASSERT( cmd->subnet_mask_len <= 128 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If optimized ho LTE->ehrpd flag is set and hdr has received overhead information, reset
   ** the flag.
   */
  if (sdss_is_opt_ho_lte_do())
  {
    sdss_update_opt_ho_lte_do(FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  curr_sys_info.assn_tag    = SDSS_ASSN_TAG_NONE;
  curr_sys_info.geo_offset  = SDSS_GEO_OFFSET_NONE;
  curr_sys_info.geo_count   = SDSS_GEO_COUNT_NONE;
  curr_sys_info.geo_sys_idx = si_info_ptr->geo_sys_idx;

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss        = SD_SS_HYBR_HDR;
    si_ptr    = sd_si_ptr( SD_SS_HYBR_HDR );
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the system selection state.
  */
  ss_state  = sd_get_ss_state(SD_SS_HYBR_HDR);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the plmn info for DO */
  hdr_mcc_list_ptr = modem_mem_alloc
                    (sizeof(sys_plmn_mcc_list), MODEM_MEM_CLIENT_MMODE);
  
  if(hdr_mcc_list_ptr)
  {
    /* set the defalut value of hdr_mcc_list with the info that we already have 
    if sid/chan is changed PRL will get re-parsed again and will update */
    *hdr_mcc_list_ptr = *(&(si_ptr->si_info.plmn_mcc_list));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the SS-State is Verify or Operation - continue with
  ** overhead info processing.
  */

  /* Point at serving and acquisition systems.
  */
  srv_sys_ref   = sdsr_list_get_ref( SDSR_HDR_SRV_SYS, 0 );
  srv_sys_ptr   = sdsr_list_get_ptr( SDSR_HDR_SRV_SYS, 0 );

  acq_sys_ptr   = sdsr_list_get_ptr( SDSR_HDR_ACQ_SYS, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( srv_sys_ref != SDSR_REF_NULL );
  SD_ASSERT( acq_sys_ptr != NULL );
  SD_ASSERT( cmd->sid_ptr != NULL );

  if( acq_sys_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }


  SD_MSG_HIGH_3( "SRV_3GPP2: HDR oh, band=%d, chan=%d mask_len=%d",
             cmd->band, cmd->chan, cmd->subnet_mask_len );
  sdsr_hdr_sid_print( cmd->sid_ptr );
  if(srv_sys_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out whether band/channel has changed.
  */
  if( srv_sys_ptr->sys.band != sd_map_band_to_band_pref(cmd->band) ||
      srv_sys_ptr->sys.chan != cmd->chan  )
  {
    chan_is_changed = TRUE;
  }

  /* Find out whether the SID has changed.
  */
  if( ! sdsr_is_same_is856_sid( srv_sys_ptr, cmd->sid_ptr, cmd->subnet_mask_len) )
  {
    sid_is_changed = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SID or channel has changed, do the following updates:
  */
  if( sid_is_changed || chan_is_changed )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Update the SID of the acquisition system (originally acquired system)
    ** per the reported SID.
    */
    memscpy( acq_sys_ptr->sid.is856.sector_id,
             sizeof(acq_sys_ptr->sid.is856.sector_id),
             cmd->sid_ptr,
             sizeof(acq_sys_ptr->sid.is856.sector_id));

    acq_sys_ptr->sid.is856.subnet_mask_len = cmd->subnet_mask_len;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Save the expected SID and subnet_mask_len of the serving system.
    */
    memscpy( expected_sid,
             sizeof(expected_sid),
             srv_sys_ptr->sid_expected.prm.is856.sector_id,
             sizeof( srv_sys_ptr->sid_expected.prm.is856.sector_id ) );

    expected_subnet_mask_len =
              srv_sys_ptr->sid_expected.prm.is856.subnet_mask_len;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Store the information of the new serving system in the system record
    ** table including the mode, band ,chan, SID and index of the
    ** acquisition system.
    */
    srv_sys_ref = sdsr_tbl_store2( SD_MODE_HDR,
                                   sd_map_band_to_band_pref(cmd->band),
                                   cmd->chan );

    sdsr_list_put_ref( SDSR_HDR_SRV_SYS, srv_sys_ref );
    srv_sys_ptr  = sdsr_list_get_ptr( SDSR_HDR_SRV_SYS, 0 );
    if(srv_sys_ptr == NULL)
    {
      sys_err_fatal_null_ptr_exception();
    }
    /* Set the SID to the one received in the OH info.
    */
    memscpy( srv_sys_ptr->sid.is856.sector_id,
             sizeof(srv_sys_ptr->sid.is856.sector_id),
             cmd->sid_ptr,
             sizeof(srv_sys_ptr->sid.is856.sector_id));

    srv_sys_ptr->sid.is856.subnet_mask_len = cmd->subnet_mask_len;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Set the expected SID of the serving system with the expected SID
    ** (which is same as the SID) of the previous serving system.
    */
    memscpy( srv_sys_ptr->sid_expected.prm.is856.sector_id,
             sizeof(srv_sys_ptr->sid_expected.prm.is856.sector_id),
             expected_sid,
             sizeof( expected_sid ) );

    srv_sys_ptr->sid_expected.prm.is856.subnet_mask_len =
            expected_subnet_mask_len;

    srv_sys_ptr->idx_expected = acq_sys_ptr->idx_expected;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Insert the serving system at the beginning of the SDSR_ACQED_LST and
    ** remove any duplicates.
    */
    sdsr_list_insert_ref( SDSR_ACQED_LST, 0, srv_sys_ref, TRUE );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Update the channel list.
    */
    sd_ss_chan_list_update( SDSR_HDR_OH_CHAN_LST,
                            SD_MODE_HDR,
                            cmd->chan_ptr,
                            cmd->num_chan,
                            SDSR_HDR_SRV_SYS,
                            &srv_sys_ptr->sid,
                            SDSR_PRL_LST,
                            SDSR_HDR_ACQ_SYS,
                            SDSR_HDR_SRV_SYS);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Update the idle cand system
    */
    sd_ss_idle_cand_sys_update( SDSR_HDR_CAND_IDL_SYS,
                                ss,
                                SDSR_HDR_OH_CHAN_LST,
                                SDSR_HDR_ACQ_SYS);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Get the PRL designation, GEO list and more preferred list of the
    ** new serving system.
    */
    if(sdss_is_mmss_operation(SD_SS_MAIN))
  	    sdprl_free_gwl_more_pref_list(SYS_SYS_MODE_HDR);
	
    geo_sys_idx =
      sdprl_get_info3(
                ss,                   /* system selection - MAIN or HDR */
                SDSR_HDR_CAND_IDL_SYS,     /* Get PRL info for this list */
                0,                    /* and list position */
                SDSR_HDR_SRV_SYS,     /* Get band chan info for this list */
                0,                    /* and list position */
                SDSR_HDR_SRV_GEO_LST, /* Put GEO sys into this list */
                SDSR_HDR_SRV_PREF_LST,/* Put PREF sys into this list */
                SDSR_HDR_ACQ_COLLOC_LST,
                                      /* Put collocated sys in ACQ list, since
                                         SRV list is based on CDMA sys and is
                                         used in OOS etc.
                                      */
                extract,              /* Get info if diff SID or PRL match */
                chan_is_changed,      /* Update the srv system as channel is
                                      ** changed
                                      */
                hdr_mcc_list_ptr);    /* PLMN MCC list for acquired HDR SYS */                      

    sdss_update_mcc_list_info(ss,hdr_mcc_list_ptr);

    /* Store the geo index of the current system
    */
    curr_sys_info.geo_sys_idx = geo_sys_idx;

  } /* if( SID or channel is changed ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !(sd_misc_is_hybr_opr()) )
  {
     /* REDIR_OFF - Set the redirection indication to disabled.
        */
        sdss_redir_update(
            ss,
            SD_SS_REDIR_NONE,             /* Set redirection to none */
            FALSE,                        /* Set valid redirection to FALSE*/
            SD_WILDCARD_SID,              /* Set expected SID to wildcard */
            SD_WILDCARD_NID,              /* Set expected NID to wildcard */
            FALSE,                        /* Set RETURN_IF_FAIL to FALSE */
            FALSE,                        /* Set IGNORE_CDMA to FALSE */
            SD_SS_MAX_REDIR_DELAY_NONE ); /* Set max redirect delay to none*/

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remember the current SS-State.
  */
  prev_ss_state = ss_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if BSR timer uptime needs to be reset
  */
  sdss_check_bsr_timer(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication as
  ** follows:
  **
  **   1. If SS-State is not yet operation or SID/NID has changed, process
  **      SDSS_EVT_HDR_OPR_OH_SID.
  **   2. Else, if channel has changed, process SDSS_EVT_HDR_OPR_OH_CHAN.
  **   3. Else, process SDSS_EVT_MISC_FILLER.
  */
  if( ss_state != SDSS_STATE_OPR ||
      sid_is_changed )
  {
    act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_OH_SID, cmd->rtrn_act_ptr );
  }
  else if( chan_is_changed )
  {
    act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_OH_CHAN, cmd->rtrn_act_ptr );
  }
  else
  {
    /* If currently doing CDMA+HDR hybrid operation, call on
    ** sd_ss_hdr_event_proc(). Else call on sd_ss_event_proc().
    */
    if( sd_misc_is_hybr_opr() )
    {
      act = sd_ss_hdr_event_proc( SDSS_EVT_MISC_FILLER, cmd->rtrn_act_ptr );
    }
    else
    {
      act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_FILLER, cmd->rtrn_act_ptr );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE  |
                    SD_SS_ACT_ACQ_CDMA  |
                    SD_SS_ACT_ACQ_AMPS  |
                    SD_SS_ACT_ACQ_HDR   |
                    SD_SS_ACT_ACQ_GWL    |
                    SD_SS_ACT_MEAS_DED  |
                    SD_SS_ACT_MEAS_BACK |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is continue or background mode measurement, do the
  ** following updates.
  */
  if( act == SD_SS_ACT_CONTINUE || act == SD_SS_ACT_MEAS_BACK )
  {
    /* If device enters KDDI network and if SDSSSCR_EFS_ITEM_LOC_BASE_BSR is
    ** enabled use longer bsr otherwise use default
    */
    if( lte_ss != SD_SS_MAX )
    {
      if((sdss_is_flag_set(lte_ss, SDSS_FLAG_MCC_MATCH_FOUND)) && (sd_efs_is_loc_bsr_enabled()))
      {
        if(!sdss_is_flag_set(lte_ss,SDSS_FLAG_LOC_BASE_BSR))
        {
          sdss_set_flag(lte_ss,SDSS_FLAG_LOC_BASE_BSR,TRUE);
          SD_MSG_MED_0(" SDSS_FLAG_LOC_BASE_BSR  set to TRUE  ");
        }
      }
      else
      {
        if(sdss_is_flag_set(lte_ss,SDSS_FLAG_LOC_BASE_BSR))
        {
          /* Do not reset in this case since LOC_BASE_BSR flag 
          ** could have been updated during acq_sid from MCC sent from
          ** HDR */
        }
      }
    }
    /* Update the expected SID of the serving system per the OH info's SID.
    */
    memscpy( srv_sys_ptr->sid_expected.prm.is856.sector_id,
             sizeof(srv_sys_ptr->sid_expected.prm.is856.sector_id),
             cmd->sid_ptr,
             sizeof(srv_sys_ptr->sid_expected.prm.is856.sector_id) );

    srv_sys_ptr->sid_expected.prm.is856.subnet_mask_len = cmd->subnet_mask_len;

    /* If previous SS-State was not operation, update the idle system, its
    ** GEO and its PREF lists per the initial acquisition system.
    */
    if( prev_ss_state != SDSS_STATE_OPR )
    {
      sd_ss_hdr_idle_sys_update( SDSR_HDR_ACQ_SYS,
                                 SDSR_HDR_ACQ_GEO_LST,
                                 SDSR_HDR_ACQ_PREF_LST );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If SID/NID is changed or channel is changed, update the IDL_SYS,
    ** IDL_GEO and IDL_PREF lists per the candidate idle system,
    ** SRV_GEO and SRV_PREF lists.
    */
    if( sid_is_changed ||
        chan_is_changed)
    {
      sd_ss_hdr_idle_sys_update( SDSR_HDR_CAND_IDL_SYS,
                                 SDSR_HDR_SRV_GEO_LST,
                                 SDSR_HDR_SRV_PREF_LST );
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Get the additional system info of the acquired system.
    */
    sdprl_get_addn_sys_record_info(srv_sys_ptr, &(curr_sys_info.assn_tag),
                                           &(curr_sys_info.geo_offset),
                                           &(curr_sys_info.geo_count));

    /* Update time info
    */
    sd_si_update_hdr_time (ss, cmd->local_time_offset, cmd->leap_secs);

    /* Update the system indicators per the new serving system.
    */

   mode_info.hdr.hdr_mcc = cmd->hdr_mcc;
   mode_info.hdr.latitude = cmd->latitude;
   mode_info.hdr.longitude = cmd->longitude;
   mode_info.hdr.hdr_system_type = cmd->system_type; 
	
    sd_si_update( ss,
                  SD_SI_UPDATE_OH,
                  SYS_SRV_STATUS_SRV,
                  SYS_SRV_DOMAIN_PS_ONLY,
                  srv_sys_ref,
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  &mode_info,
                  SDSR_HDR_SRV_COLLOC_LST,
                  &cmd->hdr_active_prot,
                  &cmd->hdr_personality,
                  &curr_sys_info,
                  SYS_SRV_STATUS_NO_SRV,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  0, /* Request Id not needed */
                  SYS_EXTEND_SRV_INFO_NONE,
                  SYS_REG_DOMAIN_NOT_APPLICABLE,
                  NULL,
                  FALSE);

    /* Update the additional system info in sd state with that of
    ** the acquired system.
    */
      sdss_set_prev_sys_info(ss,  curr_sys_info.assn_tag,
                                  curr_sys_info.geo_offset,
                                  curr_sys_info.geo_count);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If HDR call processing is currently able to process originations,
    ** Inform the registered client (i.e., CM) that is OK to originate as
    ** necessary.
    */
    if( cmd->is_ok_to_orig )
    {
      sd_si_ok_to_orig( ss, si_ptr->data_suspend );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Reset the event counters if the system SID changed.
    */
    if( ! sdss_event_is_same_is856_sid( ss, cmd->sid_ptr, cmd->subnet_mask_len) )
    {
      sdss_event_cnt_update(ss, SDSS_EVT_CNT_UPDT_REAS_SID_CHANGED);

      sdss_event_is856_sid_update( ss, cmd->sid_ptr );
    }
    /* If Hybr2 is more preferred DO scan, send hybr bsr end success to CM 
    */

    if (sdss_is_resel_state(SD_SS_HYBR_2, SDSS_RESEL_STT_MORE_PREF_HDR_SCAN))
    {
      sd_si_hybr_bsr_to_hdr(ss, SD_HYBR_BSR_TO_HDR_END_SUCCESS); 
    }

  }

  if(hdr_mcc_list_ptr)
  { 
    /* Free the mcc list for HDR */
    modem_mem_free(hdr_mcc_list_ptr, MODEM_MEM_CLIENT_MMODE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_opr_oh_info5 */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_sys_usable

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_sys_usable(
      sd_ss_ind_hdr_opr_sys_usable_cmd  *cmd
)
{
  sd_ss_act_e_type      act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->usable_status, SD_SYS_USABLE_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ******************* NO EVENT TO SS CURRENTLY ******************
  */
  act = SD_SS_ACT_CONTINUE;

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  //SD_ASSERT( act & SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Inform clients of the usable status if we continue to remain on the
  ** system ( usable or not ) and currently applicable only when we are in
  ** hybrid operation.
  */
  if( sd_misc_is_hybr_opr() )
  {
    sd_si_sys_usable( SD_SS_HYBR_HDR,
                      cmd->usable_status
                    );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Client supplied a buffer for the return SS-Action, set the
  ** returned SS-Action in this buffer to CONTINUE.
  */
  if( cmd->rtrn_act_ptr != NULL )
  {
    cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
  }

  return act;

} /* sd_ss_ind_hdr_opr_sys_usable */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_redir

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_redir(
      sd_ss_ind_hdr_opr_redir_cmd *cmd
)
{
  sd_ss_act_e_type    act;
  int                 i;
  sd_sys_s_type       sys;
  sdsr_ref_type       sr_ref;
  int                 avail_entries;
  sdss_state_e_type   ss_state;
  sdsr_s_type         *tmp_sr_ptr;

  /* Assume no hybrid operation.
  */
  sd_ss_e_type        ss                = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( cmd->redir_type == SD_SS_REDIR_HDR_QC ||
             cmd->redir_type == SD_SS_REDIR_HDR_DIRECTED );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_HIGH_2( "HDR redir, redir_type=%d, *sys_get_func=0x%x",
             cmd->redir_type, cmd->sys_get_func);

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss      = SD_SS_HYBR_HDR;
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the system selection state.
  */
  ss_state  = sd_get_ss_state(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the SS-State is acquisition (i.e., SD has not yet decided to stay on
  ** this system), do the following processing.
  */
  if( ss_state == SDSS_STATE_ACQ )
  {
    /* If redirection type is QC Redirection, treat this indication as pilot
    ** acquisition failure.
    */
    if( cmd->redir_type == SD_SS_REDIR_HDR_QC )
    {
      return sdcmd_hdr_acq_failed_pilot((sd_cmd_t *) cmd);
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else (this is a directed redirection), ignore the redirection and
    ** return CONTINUE.
    */

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    SD_MSG_LOW_0( "Redir ignored - received in ACQ state");

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find out the number of system record table entries that are available
  ** for storing the redirection's channels.
  */
  avail_entries = sdsr_tbl_size() - sdsr_list_cnt(SDSR_PRL_LST);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store supported redirection's channels in the system record table and
  ** update SDSR_REDIR_LST to reference the stored records.
  */
  sdsr_list_clr( SDSR_HDR_REDIR_LST );

  if( cmd->sys_get_func != NULL )
  {
    for( i=0; cmd->sys_get_func(cmd->data_ptr, i, &sys); i++ )
    {
      SD_ASSERT_ENUM_IS_INRANGE( sys.mode, SD_MODE_MAX );
      SD_ASSERT_ENUM_IS_INRANGE( (sd_band_e_type) sys.band, SD_BAND_MAX );
      SD_ASSERT( i < avail_entries );

      /* If MS supports this system and we still have space, store the system
      ** in the system record table.
      */
      if( sdss_is_supp_sys(
                     sys.mode,
                     sd_map_band_to_band_pref((sd_band_e_type)sys.band),
                     sys.chan) && i < avail_entries )
      {
        sr_ref = sdsr_tbl_store2(
                        sys.mode,
                        sd_map_band_to_band_pref((sd_band_e_type)sys.band),
                        sys.chan );

        /* Set the expected SID to wildcard SID. */
        tmp_sr_ptr = sdsr_tbl_get_ptr(sr_ref);
        if( tmp_sr_ptr == NULL)
        {
          SD_MSG_LOW_0("tmp_sr_ptr NULL");
          continue;
        }
        tmp_sr_ptr->sid_expected.prm.is856.subnet_mask_len = 0;

        SD_MSG_HIGH_3( "HDR redir: insert, mode=%d, band=%d, chan=%d",
                     sys.mode,sys.band,sys.chan);

        sdsr_list_insert_ref( SDSR_HDR_REDIR_LST,
                              SDSR_POS_NULL,
                              sr_ref,
                              FALSE );
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the redirection type.
  */
  sdss_redir_update(
          ss,                           /* System selection - MAIN or HDR */
          cmd->redir_type,              /* Set redirection type to HDR */
          TRUE,                         /* Set is_valid to TRUE */
          SD_WILDCARD_SID,              /* Set expected SID to wildcard-SID*/
          SD_WILDCARD_NID,              /* Set expected NID to wildcard-NID*/
          FALSE,                        /* Set is_rif to FALSE */
          FALSE,                        /* Set is_ignore_cdma to FALSE */
          SD_SS_MAX_REDIR_DELAY_NONE ); /* Set max_redirect_delay to none */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update SDSR_REDIR_SYS per the redirecting system.
  */
  //sdsr_list_cpy( SDSR_HDR_REDIR_SYS, SDSR_HDR_SRV_SYS );
  sdsr_list_cpy( SDSR_HDR_REDIR_SYS, SDSR_HDR_ACQ_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_REDIR, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE  |
                    SD_SS_ACT_ACQ_CDMA  |
                    SD_SS_ACT_ACQ_AMPS  |
                    SD_SS_ACT_ACQ_HDR   |
                    SD_SS_ACT_ACQ_GWL    |
                    SD_SS_ACT_MEAS_DED  |
                    SD_SS_ACT_MEAS_BACK |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_opr_redir */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_sys_lost2

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_sys_lost2(
      sd_ss_ind_hdr_opr_sys_lost_cmd  *cmd
)
{
  sd_ss_act_e_type    act;
  sdss_state_e_type   ss_state;

  /* Assume no hybrid operation.
  */
  sd_ss_e_type        ss    = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->sys_lost, SD_SS_SYS_LOST_MAX );

  SD_MSG_HIGH_1( "sys_lost=%d", cmd->sys_lost);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr())
  {
    ss = SD_SS_HYBR_HDR;
  }
  #endif /* FEATURE_HDR_HYBRID */


  sdss_reset_splmn_info(SYS_SYS_MODE_HDR);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the system selection state.
  */
  ss_state  = sd_get_ss_state(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the SS-State is acquisition, treat this indication as sync
  ** acquisition failure.
  */
  if( ss_state  == SDSS_STATE_ACQ )
  {
    return sdcmd_hdr_acq_failed_sync((sd_cmd_t *) cmd);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the system lost reason.
  */
  sdss_sys_lost_update( ss, cmd->sys_lost );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update fade related parameters of the HDR idle system.
  */
  sdsr_list_fade_update( SDSR_HDR_IDL_SYS, cmd->sys_lost );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_SYS_LOST, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( act == SD_SS_ACT_CONTINUE )
  {
    act = sd_ss_hdr_act_get( cmd->rtrn_act_ptr );

    SD_MSG_HIGH_1( "Get true action= %d ", act);

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_opr_sys_lost */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_sys_lost3

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_sys_lost3(
        sd_ss_ind_hdr_opr_sys_lost_cmd  *cmd
)
{

  /* Since this API is called by MMOC when GWL is activated, this will
  ** always be for hybrid HDR.
  */
  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {

    /* When HDR sys-lost due to GWL being activated, we need to update
    ** the sys-lost reason also.
    ** In a normal sys-lost case due to HDR-CP encounter service-lost,
    ** sd_si_srv_off( ) is triggerred after we updated sys_lost reason.
    */
    sdss_sys_lost_update( SD_SS_HYBR_HDR, cmd->sys_lost );

    /* Inform the System Indication component that the SS-Hybr protocol is
    ** being deactivated.
    ** HDR SRV lost (while doing BSR) due to LTE scan, so will treat this as
    ** internal service lost and hold this srv_lost for 4 secs.
    */
    SD_MSG_HIGH_1( "sys_lost =%d. treating this as internal service lost",
                  cmd->sys_lost);
    sd_si_srv_off( SD_SS_HYBR_HDR, TRUE );
  }
  else
  {
    SD_ERR_0("incorrect usage of srv_lost3");
  }
  #endif /* FEATURE_HDR_HYBRID */


  return sdcmd_hdr_opr_sys_lost2(cmd);

} /* sd_ss_ind_hdr_opr_sys_lost3 */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_resel_ok

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_resel_ok(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;
  sd_ss_e_type        ss = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_RESEL_OK, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_opr_resel_ok */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_acc_term

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_acc_term(
      sd_ss_ind_hdr_opr_acc_term_cmd  *cmd
)
{
  sd_ss_act_e_type    act;

  /* Assume no hybrid operation.
  */
  sd_ss_e_type        ss     = SD_SS_MAIN;

  sd_si_s_type       *si_ptr = sd_si_ptr( SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->acc_term, SD_SS_ACC_TERM_MAX );

  SD_MSG_HIGH_1( "acc_term=%d", cmd->acc_term);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss = SD_SS_HYBR_HDR;
    si_ptr = sd_si_ptr( SD_SS_HYBR_HDR );
  }
  #endif /* FEATURE_HDR_HYBRID */

  if( cmd->acc_term != SD_SS_ACC_TERM_NORMAL )
  {
    sdss_reset_splmn_info(SYS_SYS_MODE_HDR);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the access termination reason.
  */
  sdss_acc_update( ss, SD_SS_ACC_REAS_OTHER, cmd->acc_term );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the access termination timeout, update the event count.
  */
  if( cmd->acc_term == SD_SS_ACC_TERM_TIMEOUT )
  {
    sdss_event_cnt_update( ss, SDSS_EVT_CNT_UPDT_REAS_ACC_TIMEOUT );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the connection deny reason general, update the event count.
  */
  if( cmd->acc_term == SD_SS_ACC_TERM_CON_DEN )
  {
    sdss_event_cnt_update( ss, SDSS_EVT_CNT_UPDT_REAS_CON_DENY );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the connection deny reason network busy, update the event count.
  */
  if( cmd->acc_term == SD_SS_ACC_TERM_NET_BUSY )
  {
    sdss_event_cnt_update( ss, SDSS_EVT_CNT_UPDT_REAS_NET_BUSY );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the reason is max access probes, update the event count.
  */
  if( cmd->acc_term == SD_SS_ACC_TERM_MAX_ACC )
  {
    sdss_event_cnt_update( ss, SDSS_EVT_CNT_UPDT_REAS_MAP );
  }


  /* The SD_SS_ACC_TERM_NORM is used to reset acc_failure counter
  ** There is race condition where this acc_term could be received while
  ** SD is in accquisition state.
  ** Return SD_SS_ACT_CONTINUE immediately.
  */
  if( cmd->acc_term == SD_SS_ACC_TERM_NORMAL )
  {
    sdss_event_cnt_update( ss, SDSS_EVT_CNT_UPDT_REAS_ACC_NORMAL );

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_ACC_TERM, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  if( cmd->acc_term == SD_SS_ACC_TERM_MAX_ACC  )
  {
    SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                      SD_SS_ACT_ACQ_AMPS |
                      SD_SS_ACT_ACQ_HDR  |
                      SD_SS_ACT_ACQ_GWL   |
                      SD_SS_ACT_MEAS_DED |
                      SD_SS_ACT_PWR_SAVE ) );
  }
  else
  {
    SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                      SD_SS_ACT_ACQ_CDMA |
                      SD_SS_ACT_ACQ_AMPS |
                      SD_SS_ACT_ACQ_HDR  |
                      SD_SS_ACT_ACQ_GWL   |
                      SD_SS_ACT_MEAS_DED |
                      SD_SS_ACT_MEAS_BACK|
                      SD_SS_ACT_PWR_SAVE ) );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is continue or background mode measurement, inform the
  ** registered client (i.e., CM) that is OK to originate as necessary.
  */
  if( act == SD_SS_ACT_CONTINUE ||
      act == SD_SS_ACT_MEAS_BACK )
  {
    sd_si_ok_to_orig( ss, si_ptr->data_suspend );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_opr_acc_term */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_ses_abort

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_ses_abort(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /* Assume no hybrid operation.
  */
  sd_ss_e_type         ss       = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss     = SD_SS_HYBR_HDR;
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the SS-State is acquisition, treat this indication as sync
  ** acquisition failure.
  */
  if( sd_get_ss_state(ss) == SDSS_STATE_ACQ )
  {
    return sdcmd_hdr_acq_failed_sync(cmd);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_SES_ABORT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_opr_ses_abort */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_ses_nego_timeout

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_ses_nego_timeout(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /* Assume no hybrid operation.
  */
  sd_ss_e_type         ss        = SD_SS_MAIN;
  ///sd_ss_s_type        *ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss = SD_SS_HYBR_HDR;
  }
  #endif /* FEATURE_HDR_HYBRID */

  //ss_ptr = sd_ss_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update counter irrespective of the SS state.
  */
  sdss_event_cnt_update(ss, SDSS_EVT_CNT_UPDT_REAS_SES_NEGO_TIMEOUT);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the SS-State is acquisition, treat this indication as sync
  ** acquisition failure.
  */
  //if( ss_ptr->ss_state == SDSS_STATE_ACQ )
  //{
  //  return sd_ss_ind_hdr_acq_failed_sync( rtrn_act_ptr );
  //}

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_SES_NEGO_TIMEOUT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_opr_ses_nego_timeout */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_ses_opened

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_ses_opened(
    sd_cmd_t  *cmd
)
{
  /* Assume no hybrid operation.
  */
  sd_ss_e_type         ss        = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss = SD_SS_HYBR_HDR;
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If checking for SS state in the future, then update counter
  ** irrespective of the SS state. */
  sdss_event_cnt_update(ss, SDSS_EVT_CNT_UPDT_REAS_SES_OPENED);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  if ( cmd->rtrn_act_ptr != NULL )
  {
    cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
  }

  return SD_SS_ACT_CONTINUE;

} /* sd_ss_ind_hdr_opr_ses_opened */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_ses_close

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_ses_close(
      sd_ss_ind_hdr_opr_ses_close_cmd *cmd
)
{
  sd_ss_act_e_type    act;

  /* Assume no hybrid operation.
  */
  sd_ss_e_type        ss    = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->ses_close, SD_SS_SES_CLOSE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss = SD_SS_HYBR_HDR;
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SS-Core with the HDR session close reason.
  */
  sdss_hdr_ses_close_update( ss, cmd->ses_close );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_SES_CLOSE, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_opr_ses_close */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_bad_sys

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_bad_sys(
      sd_ss_ind_hdr_opr_bad_sys_cmd *cmd
)
{

  sd_ss_act_e_type    act;

  /* Assumed no hybrid operation.
  */
  sd_ss_e_type         ss       = SD_SS_MAIN;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->reas, SD_SS_BAD_SYS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss        = SD_SS_HYBR_HDR;
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  if( sd_get_ss_state(ss) == SDSS_STATE_ACQ )
  {
    act = sd_ss_event_proc( ss, SDSS_EVT_HDR_ACQ_BAD_SYS, cmd->rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_BAD_SYS, cmd->rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_hdr_opr_bad_sys */


/*===========================================================================

FUNCTION sdcmd_hdr_opr_start_activity

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_start_activity(
      sd_ss_ind_hdr_opr_activity_cmd *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;

  /* Assume no hybrid operation.
  */
  sd_ss_e_type        ss    = SD_SS_MAIN;

  sd_ss_s_type        *ss_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->hdr_act_mask, SD_SS_HDR_ACT_MASK_MAX );
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If in hybrid mode, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
    ss = SD_SS_HYBR_HDR;

    /* Set hdr activity flag to true */
    sdss_update_hdr_activity(SD_SS_MAIN, TRUE);

    /* Update cm that hdr activity is started */
    sd_si_update_hdr_activity(TRUE);
  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the activity to the activity mask.
  */
  ss_ptr = sd_ss_ptr( ss );

  ss_ptr->hdr_act_mask = (sd_ss_hdr_act_mask_e_type)SD_BIT_ADD(
                                ss_ptr->hdr_act_mask, cmd->hdr_act_mask );

  SD_MSG_HIGH_2("Start activity 0x%x mask 0x%x",
              cmd->hdr_act_mask, ss_ptr->hdr_act_mask);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_IS_BIT_CONTAIN( cmd->hdr_act_mask, SD_SS_HDR_ACT_MASK_BCMCS ) )
  {
    /* Update the HDR activity start uptime.
    */
    sdss_hdr_bcmcs_start_uptime_update( ss );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* either mask (session related or call related), we will update the traffic*/
  if( SD_IS_BIT_CONTAIN( cmd->hdr_act_mask, SD_SS_HDR_ACT_MASK_TRAFFIC ) ||
      SD_IS_BIT_CONTAIN( cmd->hdr_act_mask, SD_SS_HDR_ACT_MASK_SESSION ))
  {
    /* Update event count if HDR enters traffic.
    */
    sdss_event_cnt_update(ss, SDSS_EVT_CNT_UPDT_REAS_HDR_ENTER_TRAFFIC);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  if ( cmd->rtrn_act_ptr != NULL )
  {
    cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
  }

  SD_ASSERT( act & SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

}/*sd_ss_ind_hdr_opr_start_activity*/


/*===========================================================================

FUNCTION sdcmd_hdr_opr_stop_activity

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hdr_opr_stop_activity(
      sd_ss_ind_hdr_opr_activity_cmd  *cmd
)
{

  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;

  sdss_state_e_type   ss_state;

  /* Assume no hybrid operation.
  */
  sd_ss_e_type        ss                = SD_SS_MAIN;
  sd_ss_s_type        *ss_ptr   = sd_ss_ptr(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->hdr_act_mask, SD_SS_HDR_ACT_MASK_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr())
  {
    ss = SD_SS_HYBR_HDR;
    ss_ptr = sd_ss_ptr(SD_SS_HYBR_HDR);
  }
  #endif /* FEATURE_HDR_HYBRID */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Filter out the activities that have not been started.
  */
  cmd->hdr_act_mask = (sd_ss_hdr_act_mask_e_type)SD_BIT_COMMON(
                                     ss_ptr->hdr_act_mask, cmd->hdr_act_mask);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove the activity from the activity mask.
  */
  ss_ptr->hdr_act_mask = (sd_ss_hdr_act_mask_e_type)SD_BIT_DELETE(
                                        ss_ptr->hdr_act_mask, cmd->hdr_act_mask );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_IS_BIT_CONTAIN( cmd->hdr_act_mask, SD_SS_HDR_ACT_MASK_BCMCS ) )
  {
    /* Update the HDR activity start uptime.
    */
    sdss_hdr_bcmcs_stop_uptime_update( ss );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_HDR_HYBRID
  if( ss_ptr->hdr_act_mask == SD_SS_HDR_ACT_MASK_NONE )
  {
    /* Set hdr activity flag to true */
    sdss_update_hdr_activity(SD_SS_MAIN, FALSE);
  

    /* Update cm that hdr activity is started */
    sd_si_update_hdr_activity(FALSE);
  }

  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the system selection state.
  */
  ss_state  = sd_get_ss_state(ss);

  SD_MSG_HIGH_3("Stop activity 0x%x, mask 0x%x, ss_state %d",
              cmd->hdr_act_mask, ss_ptr->hdr_act_mask, ss_state);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if the session is on when we end HDR call, do nothing
  ** Else, shorten the timer
  */
  if(  ss_state != SDSS_STATE_ACQ &&
       SD_IS_BIT_CONTAIN( cmd->hdr_act_mask, SD_SS_HDR_ACT_MASK_TRAFFIC ) )
  {
    act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_RELEASE, cmd->rtrn_act_ptr );
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the HDR activity is to end BCMCS flow monitoring.
  */
  if(  ss_state != SDSS_STATE_ACQ &&
       SD_IS_BIT_CONTAIN( cmd->hdr_act_mask, SD_SS_HDR_ACT_MASK_BCMCS ))
  {
    act = sd_ss_event_proc( ss, SDSS_EVT_HDR_OPR_END_BCMCS, cmd->rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if rtrn_act_ptr is not NULL and populate return action.
  */
  if ( cmd->rtrn_act_ptr != NULL )
  {
    cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
  }

  SD_ASSERT( act & SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

}/*sd_ss_ind_hdr_opr_stop_activity*/
#endif /* FEATURE_HDR */

static boolean sdcmd_is_bsr_allowedby_splmn(sd_ss_e_type ss)
{
   if(!sdss_is_splmn_support_enabled())
   {
    SD_MSG_HIGH_0("C2k_resel_splmn: Feature not enabled");
   	return TRUE;
   }

   SD_MSG_MED_2("C2k_resel_splmn: sdcmd_is_bsr_allowedby_splmn ss :%d LTE is on ss :%d",ss, sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE));
   
   sdsr_list_print(sdss_sr_list_map2(ss,SDSR_SRV_PREF_LST),FALSE);
   sdsr_list_print(SDSR_SPLMN_MORE_PREF_LST,FALSE);

   if( ss == SD_SS_MAIN && 
   	   sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE) == SD_SS_MAIN &&
   	   sdsr_list_is_same (SDSR_SRV_PREF_LST,SDSR_SPLMN_MORE_PREF_LST,SDSR_EQUAL_MODE))
   {
     return FALSE;
   }
   else if(ss == SD_SS_HYBR_2 &&
   	       sdss_is_1x_sxlte() &&  
   	       sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE) == SD_SS_HYBR_2 &&
   	       sdsr_list_is_same(SDSR_HYBR_1_SRV_PREF_LST,SDSR_SPLMN_MORE_PREF_LST,SDSR_EQUAL_MODE))
   {
     return FALSE;
   }
   return TRUE;
}
/*===========================================================================

FUNCTION sdcmd_misc_timer

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_misc_timer(
  sd_ss_ind_misc_timer_cmd  *cmd
)
{
  #ifdef FEATURE_HYBR_3GPP
  return sdcmd_hybr_2_misc_timer(cmd);
  #elif defined(CFG_HYBR_GW_3)
  return sdcmd_hybr_3_misc_timer(cmd);
  #else
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_MAIN);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate that the SS-Timer is expired by setting it to 0.
  */
  ss_ptr->ss_timer = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication
  ** (depending whether reselection is allowed).
  */
  if( cmd->resel_is_allowed )
  {
    if( !sdcmd_is_bsr_allowedby_splmn(SD_SS_MAIN))
    {
      SD_MSG_HIGH_0("C2k_resel_splmn: BSR not allowed by resel to SPLMN feature");
      return SD_SS_ACT_CONTINUE;
    }
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_TIMER_RESEL_OK, cmd->rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_TIMER_RESEL_NO, cmd->rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  if( cmd->resel_is_allowed )
  {
    SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                      SD_SS_ACT_ACQ_CDMA |
                      SD_SS_ACT_ACQ_AMPS |
                      SD_SS_ACT_ACQ_HDR  |
                      SD_SS_ACT_ACQ_GWL   |
                      SD_SS_ACT_MEAS_DED |
                      SD_SS_ACT_MEAS_BACK|
                      SD_SS_ACT_PWR_SAVE ) );
  }
  else
  {
    SD_ASSERT( act == SD_SS_ACT_CONTINUE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
  #endif

} /* sd_ss_ind_misc_timer */


/*===========================================================================

FUNCTION sdcmd_hybr_2_misc_timer

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hybr_2_misc_timer(
  sd_ss_ind_misc_timer_cmd *cmd
)
{
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_HYBR_2);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
    sys_err_fatal_invalid_value_exception((int)SYS_OVERALL_FEATURE_MODE_NORMAL);
  }

  /* Indicate that the SS-Timer is expired by setting it to 0.
  */
  ss_ptr->ss_timer = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication
  ** (depending whether reselection is allowed).
  */
  if( cmd->resel_is_allowed )
  {
    if(!sdcmd_is_bsr_allowedby_splmn(SD_SS_HYBR_2))
    {
      SD_MSG_HIGH_0("C2k_resel_splmn: BSR not allowed by resel to SPLMN feature");
      return SD_SS_ACT_CONTINUE;
    }
	
    act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_MISC_TIMER_RESEL_OK, cmd->rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_MISC_TIMER_RESEL_NO, cmd->rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  if( cmd->resel_is_allowed )
  {
    SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                      SD_SS_ACT_ACQ_CDMA |
                      SD_SS_ACT_ACQ_AMPS |
                      SD_SS_ACT_ACQ_HDR  |
                      SD_SS_ACT_ACQ_GW   |
                      SD_SS_ACT_MEAS_DED |
                      SD_SS_ACT_MEAS_BACK|
                      SD_SS_ACT_PWR_SAVE|
                      SD_SS_ACT_ACQ_GWL) );
  }
  else
  {
    SD_ASSERT( act == SD_SS_ACT_CONTINUE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_misc_timer */

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sd_ss_act_e_type          sdcmd_hybr_3_misc_timer(
  sd_ss_ind_misc_timer_cmd *cmd
)
{
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_HYBR_3);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate that the SS-Timer is expired by setting it to 0.
  */
  ss_ptr->ss_timer = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication
  ** (depending whether reselection is allowed).
  */
  if( cmd->resel_is_allowed )
  {
    act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_MISC_TIMER_RESEL_OK, cmd->rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_MISC_TIMER_RESEL_NO, cmd->rtrn_act_ptr );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  if( cmd->resel_is_allowed )
  {
    SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                      SD_SS_ACT_ACQ_CDMA |
                      SD_SS_ACT_ACQ_AMPS |
                      SD_SS_ACT_ACQ_HDR  |
                      SD_SS_ACT_ACQ_GW   |
                      SD_SS_ACT_MEAS_DED |
                      SD_SS_ACT_MEAS_BACK|
                      SD_SS_ACT_PWR_SAVE|
                      SD_SS_ACT_ACQ_GWL) );
  }
  else
  {
    SD_ASSERT( act == SD_SS_ACT_CONTINUE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sdcmd_hybr_3_misc_timer */
#endif /* defined(FEATURE_MMODE_TRIPLE_SIM) */


/*===========================================================================

FUNCTION sdcmd_hybr_1_misc_timer

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hybr_1_misc_timer(
  sd_ss_ind_misc_timer_cmd *cmd
)
{
  #if (!defined(FEATURE_HDR_HYBRID))
  return sdcmd_misc_timer(cmd);
  #else

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_HYBR_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If currently not doing CDMA+HDR hybrid operation, return CONTINUE.
  */
  if( !sd_misc_is_hybr_opr())
  {
    if ( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }
    return SD_SS_ACT_CONTINUE;
  }

  /* Indicate that the SS-Timer is expired by setting it to 0.
  */
  ss_ptr->ss_timer = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sanity check
  */
  if(  sd_misc_is_hybr_opr()   &&
      ( SD_IS_BIT_CONTAIN( ss_ptr->hdr_act_mask, SD_SS_HDR_ACT_MASK_BCMCS )
                            ||
        SD_IS_BIT_CONTAIN( ss_ptr->hdr_act_mask, SD_SS_HDR_ACT_MASK_TRAFFIC ))
                            &&
      cmd->resel_is_allowed )
  {
    SD_ERR_2( "No Resel in HDR act!!! resel %d, act 0x%x",
                  cmd->resel_is_allowed, ss_ptr->hdr_act_mask);

  }

  /* Process the SS-Event that is associated with this SS-Indication
  ** (depending whether reselection is allowed).
  */
  if( cmd->resel_is_allowed )
  {
    act = sd_ss_hdr_event_proc( SDSS_EVT_MISC_TIMER_RESEL_OK, cmd->rtrn_act_ptr );
  }
  else
  {
    act = sd_ss_hdr_event_proc( SDSS_EVT_MISC_TIMER_RESEL_NO, cmd->rtrn_act_ptr );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  if( cmd->resel_is_allowed )
  {
    SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                      SD_SS_ACT_ACQ_CDMA |
                      SD_SS_ACT_ACQ_AMPS |
                      SD_SS_ACT_ACQ_HDR  |
                      SD_SS_ACT_ACQ_GWL   |
                      SD_SS_ACT_MEAS_DED |
                      SD_SS_ACT_MEAS_BACK|
                      SD_SS_ACT_PWR_SAVE ) );
  }
  else
  {
    SD_ASSERT( act == SD_SS_ACT_CONTINUE );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
  #endif
} /* sd_ss_ind_hybr_1_misc_timer */


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
/*===========================================================================

FUNCTION sdcmd_gw_acq_plmn_failed_ss_2

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_gw_acq_plmn_failed_ss_2(
      sd_ss_ind_gw_acq_plmn_failed_cmd  *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;
  sd_ss_e_type        ss = cmd->ss;
  sd_si_info_s_type   *si_info_ptr = &sd_si_ptr(ss)->si_info;

  sd_ss_s_type* ss_ptr = sd_ss_ptr(ss);

  /* Point at acquired system.*/
  sdsr_ref_type       acq_sys_ref   = sdsr_list_get_ref( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), 0 );

  #ifdef FEATURE_EOOS_UMTS
  sd_ss_gwl_scan_status_e_type    gwl_scan_status;
  #endif

  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  sd_ss_act_s_type       *ss_act_ptr = NULL;
  #endif

  #ifdef FEATURE_AVOID_DUP_BAND_SCAN
  boolean is_plmn_found;
  #endif

  sys_srv_status_e_type srv_status = SYS_SRV_STATUS_LIMITED;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_2("sd_ss_ind_gw_acq_plmn_failed_ss_2 | req_id = %u, scan_status = %d", 
                 cmd->req_id, cmd->scan_status);

  SD_ASSERT_ENUM_IS_INRANGE( cmd->sim_state, SYS_SIM_STATE_MAX );

  SD_ASSERT(NULL != ss_ptr);
  /* We should reset the request ID from last  sd action. 
    */
  ss_ptr->act.prm.acq_gwl.sys_sel_pref_req_id = 0;
  ss_ptr->true_curr_act.prm.acq_gwl.sys_sel_pref_req_id = 0;

  /*Once the FULL SRV REQ is issued, reset the flag to continue with
  ** pure limited acqusiition
  */
  if (sdss_get_is_full_srv_req_during_emerg(ss) &&
      !sdss_get_is_RLF_during_emerg(ss))
  {
    SD_MSG_HIGH_0("sd_ss_ind_gw_acq_plmn_failed_ss_2 - reset full srv req flag");
    
    sdss_check_reset_emerg_rat_pri_list_info(ss);
    sdss_update_is_full_srv_req_during_emerg(ss,FALSE);

    #if defined( FEATURE_LTE )
    if (sdss_is_lte_connected_mode(SD_SS_MAIN))
    {
      sdss_reset_lte_connected_mode(SD_SS_MAIN);
    }
    #endif

    #if defined ( FEATURE_WRLF_SYSTEM_SEL)
    if (sdss_is_wcdma_connected_mode(SD_SS_MAIN))
    {
      sdss_reset_wcdma_connected_mode(SD_SS_MAIN);
    }
    #endif
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Save the rplmn in global */
  if(NULL != cmd->registered_plmn)
  {
    SD_MSG_HIGH_3("sd_ss_ind_gw_acq_plmn_failed_ss_2 | plmn id [0]=%x, [1]=%x, [2]=%x", cmd->registered_plmn->identity[0],cmd->registered_plmn->identity[1],cmd->registered_plmn->identity[2]);
    *(&(ss_ptr->rplmn)) = *(cmd->registered_plmn);
  }
  else
  {
    /* undefine the PLMN id */
    SD_MSG_HIGH_0("sd_ss_ind_gw_acq_plmn_failed_ss_2 | undefining rplmn");
    sys_plmn_undefine_plmn_id(&ss_ptr->rplmn);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* After Limited Service request, scan_status reported by NAS as PLMN Found
     shall be ignored, as it may trigger Limited Routine Acquisition  by SD.
     and may affect the HPLMN search at NAS, also LIMITED_ROUTINE_ACQ after
     Limited_Service is not expected behavior */

  if((sd_misc_get_act_net_sel(ss) == SYS_NETWORK_SELECTION_MODE_LIMITED) && 
     (cmd->scan_status == SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND) &&
     (sdss_is_mmss_operation(ss) == TRUE))
  {
    SD_MSG_HIGH_1("UE requested Limited Service so ignoring NAS scan_status = %d",
                                              cmd->scan_status);
    cmd->scan_status = SD_SS_GWL_SCAN_STATUS_COMPLETE_NO_PLMN;
  }

  /* If this is the first time a plmn is found, then
   ** update is_3gpp_service_found to true
   */
  #ifdef FEATURE_AVOID_DUP_BAND_SCAN

    is_plmn_found = (cmd->scan_status ==
                                  SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND);
    sdss_update_3gpp_acq_info(ss, SDSS_UPDATE_3GPP_ACQ_INFO_IS_PLMN_FOUND,
                              is_plmn_found, SYS_SRV_STATUS_NONE);

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SD is in operation state, map to sd_ss_ind_gw_opr_srv_lost.
  */
  if( sd_get_ss_state(ss) == SDSS_STATE_OPR )
  {
    sd_ss_ind_gw_opr_srv_lost_cmd srv_lost_cmd;

    srv_lost_cmd.ss = ss;
    srv_lost_cmd.rtrn_act_ptr = cmd->rtrn_act_ptr;
    srv_lost_cmd.sim_state = cmd->sim_state;
    srv_lost_cmd.data_suspend = cmd->data_suspend;
    srv_lost_cmd.suitable_search_time = 0;
    srv_lost_cmd.registered_plmn = NULL;
    srv_lost_cmd.acq_status = SYS_ACQ_STATUS_SUCCESS;
    return sdcmd_gw_opr_srv_lost_ss_3(&srv_lost_cmd);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_EOOS_UMTS
  SD_MSG_HIGH_4("EOOS GW: SS: %d acq_plmn_failed sim: %d ds: %d scan_status: %d",
                                      ss, cmd->sim_state, cmd->data_suspend, cmd->scan_status );
  
  #else
  SD_MSG_HIGH_3("acq_plmn_failed %d %d %d", cmd->sim_state, cmd->data_suspend, ss );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  /* Update the sim state.
  */

  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  ss_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (ss_act_ptr != NULL)
  {
    memset(ss_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);
  if(sd_get_ss_state(ss) != SDSS_STATE_OPR &&
     si_info_ptr->srv_status == SYS_SRV_STATUS_LIMITED &&
     sdss_is_wcdma_connected_mode (ss) &&
    (ss_act_ptr->prm.acq_gwl.net_sel_type ==
      SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION
                        ||
      ss_act_ptr->prm.acq_gwl.net_sel_type ==
      SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {

     /* Limited service aquisition failed - set the service-indication to OFF
     */
    SD_MSG_HIGH_0("WRLF:Previous limited service is lost.\
                  Sending NO service ind now" );
    sd_si_srv_off( ss, FALSE );
  }
  #endif /* FEATURE_WRLF_SYSTEM_SEL */

  /* update SIM state
  */
  if(sdss_is_1x_sxlte() && (ss!= SD_SS_HYBR_3))
  {
    sd_si_update_sim_state( cmd->sim_state, SD_SS_MAIN );
    sd_si_update_sim_state( cmd->sim_state, SD_SS_HYBR_2 );
  }
  else
  {
    sd_si_update_sim_state( cmd->sim_state, ss );
  }

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update sdss's srv_status too.
  */
  sdss_update_srv_status( ss,
                          SYS_SRV_STATUS_NO_SRV,
                          SYS_SRV_DOMAIN_NO_SRV
                        );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //if we are here, update the req_id in si_info so that next si update will carry it.
  sd_si_ptr(ss)->si_info.sys_sel_pref_req_id = cmd->req_id;

  sdss_update_acq_status(ss, cmd->acq_status);

  /* Update the GWL scan status
  */
  sdss_update_gwl_scan_status( ss, cmd->scan_status );

   #ifdef FEATURE_EOOS_UMTS

  /* Read the scan status updated by the sdss object, this might not be the
  ** same as the scan_status passed by the lower layers. Don't use the
  ** scan_status passed directly by the lower layers
  */

  sdss_get_eoos_scan_gwl_param( SD_SS_MAIN, NULL, NULL, NULL, NULL, NULL,
                                  &gwl_scan_status );

  /* If PLMN indicates there is limited service, update the si
  */
  if( gwl_scan_status == SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND )
  {

    sdsr_ref_type       acq_sys_ref   = sdsr_list_get_ref( sdss_sr_list_map2( ss, SDSR_ACQ_SYS), 0 );

    SD_ASSERT( acq_sys_ref != SDSR_REF_NULL );

    SD_MSG_HIGH_1("sd_ss_ind_gw_acq_plmn_failed_ss_2| calling sd_si_update_1| req_id=%u", cmd->req_id);

    sd_si_update( ss,
                  SD_SI_UPDATE_REG,
                  SYS_SRV_STATUS_LIMITED,
                  SYS_SRV_DOMAIN_NO_SRV,
                  acq_sys_ref,
                  FALSE,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  SYS_SRV_STATUS_LIMITED,
                  SYS_LTE_CS_CAPABILITY_NONE,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  cmd->req_id,
                  cmd->extend_srv_info,
                  si_info_ptr->reg_domain,
                  cmd->csg_info_ptr,
                  FALSE
                );
  }
  #endif
  if(cmd->req_id > 0)
  {
	  /* update the service status baseed scan status. 
	  **  this check is added to inform NO_SERVICE incase of no PLMN found.
	  */
	  if(cmd->scan_status == SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND)
	  {

		  srv_status = 	SYS_SRV_STATUS_LIMITED;
	  }
	  else
	  {   

		  srv_status  = SYS_SRV_STATUS_NO_SRV;
	  }
	  
	  SD_MSG_HIGH_2("sd_ss_ind_gw_acq_plmn_failed_ss_2| calling sd_si_update_1| req_id=%u , srv_status = %d", cmd->req_id,srv_status);
	  sd_si_update( ss,
		  SD_SI_UPDATE_REG,
		  srv_status,
		  SYS_SRV_DOMAIN_NO_SRV,
		  acq_sys_ref,
		  FALSE,
		  &si_info_ptr->mode,
		  NULL,
		  NULL,
		  NULL,
		  SDSR_MAX,
		  NULL,
		  NULL,
		  NULL,
		  srv_status,
		  cmd->lte_cs_capability,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  cmd->req_id,
                  cmd->extend_srv_info,
                  si_info_ptr->reg_domain,
                  cmd->csg_info_ptr,
                  FALSE
                );
  }
  #ifdef FEATURE_SD_LTE
  
  else
  /* Update LTE CS capability  if we are in LTE*/
  if (((cmd->lte_cs_capability != SYS_LTE_CS_CAPABILITY_NONE) ||
        (cmd->extend_srv_info != SYS_EXTEND_SRV_INFO_NONE))&&
       (si_info_ptr->mode == SD_MODE_LTE)&&
        sys_srv_status_is_srv(si_info_ptr->srv_status))
  {

    sd_si_update( ss,
                  SD_SI_UPDATE_REG,
                  si_info_ptr->srv_status,
                  si_info_ptr->srv_domain,
                  acq_sys_ref,
                  FALSE,
                  &si_info_ptr->mode,
                  NULL,
                  NULL,
                  NULL,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  si_info_ptr->cs_srv_status,
                  cmd->lte_cs_capability,
                  SD_SIB8_NO_CHANGE,
                  FALSE,
                  SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                  cmd->req_id,
                  cmd->extend_srv_info,
                  si_info_ptr->reg_domain,
                  cmd->csg_info_ptr,
                  FALSE
                );
   }
   #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch( ss )
  {
    case SD_SS_MAIN:
    case SD_SS_HYBR_1:
      /* Process the SS-Event that is associated with this SS-Indication.
      */
      act = sd_ss_event_proc(ss, SDSS_EVT_GWL_ACQ_PLMN_FAILED, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_2:
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_HYBR_2_GW_ACQ_PLMN_FAILED, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_3:
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_HYBR_3_GW_ACQ_PLMN_FAILED, cmd->rtrn_act_ptr );
      #endif
      break;

    case SD_SS_MAX:
    default:
      SD_ERR_1(" Invalid ss %d",ss);
      act = SD_SS_ACT_PWR_SAVE;
      break;
  }


#ifdef FEATURE_WRLF_SYSTEM_SEL
  /* If the last SERVICE_REQ was sent for W in Manual CSG mode and RLF occured,
        update SD for sending SERVICE_REQ on same CSG till the suitable search timer
        expires */
  sdss_restore_csg_info_to_ss_on_wrlf( ss, ss_ptr->rplmn, &ss_ptr->true_curr_act, cmd->rtrn_act_ptr); 
#endif



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  /* Update the data suspend state */
  sd_si_update_data_suspend_state( ss, cmd->data_suspend, SYS_SRV_STATUS_NO_SRV );
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If act is ACQ_GWL, it means we are still trying GWL acquisition
  ** until it's completely finishes, do not declare acquisition fail for
  ** LTENotAvailFile */
   if( (ss == SD_SS_HYBR_2 && act == SD_SS_ACT_PWR_SAVE) ||
       ss == SD_SS_MAIN)
   {
     /* ALT_CDMA_BSR feature is on, do LTEAvailFile operation
     ** Note: Consider only complete no plmn as LTE acquisition fail. If NAS
     ** reports limited plmn found, don't declare it as LTE acq fail.
     */
     if(sdss_is_c2k_alt_algr_operation() &&
        cmd->scan_status == SD_SS_GWL_SCAN_STATUS_COMPLETE_NO_PLMN )
     {
       sdss_lte_avail_sys_s_type sys;
       sd_si_s_type              *si_main_ptr = sd_si_ptr( SD_SS_MAIN );

       if(si_main_ptr &&
          si_main_ptr->si_info.mode == SYS_SYS_MODE_CDMA &&
          sd_si_is_full_srv_on( SD_SS_MAIN ))
       {
         /* Only when we have 1x service, we need to update the lte available file
             ** upon ACQ_GWL_FAIL
             */
         sys.sid = si_main_ptr->si_info.sid.is95.sid;
         sys.nid = si_main_ptr->si_info.sid.is95.nid;
         sys.bid = si_main_ptr->si_info.mode_info.cdma.base_id;

         sdss_lte_avail_file_update(SDSS_EVT_GWL_ACQ_PLMN_FAILED, &sys, ss);
       }
       else
       {
         SD_MSG_MED_0("Invalid si_ptr");
       }
     }

   }
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
#if defined ( FEATURE_WRLF_SYSTEM_SEL)
   modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);
#endif
#endif

  return act;

} /* sd_ss_ind_gw_acq_plmn_failed_ss_2() */

  sd_ss_act_e_type          sdcmd_gw_opr_operator_capability_changed(
        sd_ss_ind_gw_acq_plmn_camped_cmd  *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;
  sd_ss_e_type        ss = cmd->ss;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_HIGH_3("sdcmd_gw_opr_operator_capability_changed | plmn id [0]=%x, [1]=%x, [2]=%x", 
                cmd->plmn.identity[0],cmd->plmn.identity[1],cmd->plmn.identity[2]);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch( ss )
  {
    case SD_SS_MAIN:
      /* Process the SS-Event that is associated with this SS-Indication.
      */
      act = sd_ss_event_proc(ss, SDSS_EVT_GWL_OPR_OPERATOR_CAPABILITY_CHANGED, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_1:
    case SD_SS_HYBR_2:
    case SD_SS_MAX:
    default:
      SD_ERR_1(" Invalid ss %d",ss);
      act = SD_SS_ACT_PWR_SAVE;
      break;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE |
                    SD_SS_ACT_CONTINUE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  return act;

} /* sdcmd_gw_opr_operator_capability_changed() */


/*===========================================================================

FUNCTION sdcmd_reset_rlf_conn_mode_status

DESCRIPTION
Reset

DEPENDENCIES
sd_init() must have already been called to initialize SD.


===========================================================================*/

void sdcmd_reset_rlf_conn_mode_status
(
  sd_cmd_t  *cmd
)
{
  sd_ss_e_type  ss = cmd->ss;

  if(sdss_is_lte_connected_mode(ss))
  {
    sdss_reset_lte_connected_mode(ss);
    SD_MSG_HIGH_0("UE ending LTE connected mode");
  }

  #ifdef FEATURE_WRLF_SYSTEM_SEL

  if(sdss_is_wcdma_connected_mode(ss))
  {
    sdss_reset_wcdma_connected_mode(ss);
    SD_MSG_HIGH_0("UE ending WCDMA connected mode");
  }

  #endif
}

/*===========================================================================

FUNCTION sdcmd_gw_opr_srv_lost_ss_3

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_gw_opr_srv_lost_ss_3(
      sd_ss_ind_gw_opr_srv_lost_cmd *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;
  sd_ss_e_type        ss = cmd->ss;
  sd_si_info_s_type   *si_info_ptr = &sd_si_ptr(ss)->si_info;
  sd_ss_s_type* ss_ptr = sd_ss_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->sim_state, SYS_SIM_STATE_MAX );

  if(NULL != cmd->registered_plmn)
  {
    SD_MSG_HIGH_3("sd_ss_ind_gw_opr_srv_lost_ss_3 | Manual |plmn id [0]=%x, [1]=%x, [2]=%x", cmd->registered_plmn->identity[0],cmd->registered_plmn->identity[1],cmd->registered_plmn->identity[2]);
    *(&(ss_ptr->rplmn)) = *(cmd->registered_plmn);
  }
  else
  {
    /* undefine the PLMN id */
    SD_MSG_HIGH_0("sd_ss_ind_gw_opr_srv_lost_ss_3 | undefining rplmn");
    sys_plmn_undefine_plmn_id(&ss_ptr->rplmn);
  }

#if defined ( FEATURE_WRLF_SYSTEM_SEL)
  SD_MSG_HIGH_3("WRLF:gw srv lost sim state %d data suspend %d suitable_search_time %d",\
               cmd->sim_state, cmd->data_suspend, cmd->suitable_search_time );
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  /* Update the sim state.
  */
  if(sdss_is_1x_sxlte() && (ss!= SD_SS_HYBR_3))
  {
    sd_si_update_sim_state( cmd->sim_state, SD_SS_MAIN );
    sd_si_update_sim_state( cmd->sim_state, SD_SS_HYBR_2 );
  }
  else
  {
    sd_si_update_sim_state( cmd->sim_state, ss );
  }

  /* Update the data suspend state.
  */
  sd_si_update_data_suspend_state( cmd->ss, cmd->data_suspend, SYS_SRV_STATUS_NO_SRV );
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update sdss's srv_status too.
  */
  sdss_update_srv_status( ss,
                          SYS_SRV_STATUS_NO_SRV,
                          SYS_SRV_DOMAIN_NO_SRV
                        );

  /*Update acq_status*/
  sdss_update_acq_status(ss, cmd->acq_status);

  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
   /* set the connected mode flag and suitable search time */
  if (!sdss_get_is_full_srv_req_during_emerg(ss))
  {
    sdss_set_wcdma_connected_mode(cmd->suitable_search_time, ss);
  }
  else
  { /* Do not set W connected mode flag for CSFB E911 orig or a VOLTE call that experianced
      ** HARD FAILURE.
      */
    if ((sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG) ||
         (sdss_get_ss_state(ss) == SDSS_STATE_ACQ))
    {
      SD_MSG_HIGH_0("sdcmd_gw_opr_srv_lost3 - do not set WCDMA connected mode");
    }
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If ue is in an emergency call origination and the current RAT priority list
  ** does not match the emerg capability, update the current RAT priority list
  */
  if (!sdss_get_is_full_srv_req_during_emerg(ss) &&
      !sdss_is_emer_orig_in_volte_only(ss))
  {
    sdss_check_reset_emerg_rat_pri_list_info(ss);
  }
 
  /* 3GPPCSFB call originations, NAS takes care of FULL SRV acquisition.
  ** So,reset the full_srv_req falg to proceed with pur limited acqusiition.
  */
  if ((sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG) && 
      sdss_get_is_full_srv_req_during_emerg(ss))
  {
    SD_MSG_HIGH_0("sdcmd_gw_opr_srv_lost_ss_3 - reset full srv req flag due to PPCSFB orig");
    sdss_update_is_full_srv_req_during_emerg( ss, FALSE);
    sdss_check_reset_emerg_rat_pri_list_info(ss);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sd_misc_gwl_is_sys_lost_map_acq_fail(ss) )
  {
    if(ss == SD_SS_HYBR_2 &&
       sdss_is_1x_sxlte())
    {
      /* Update PREF_LIST if this is for HYBR2 stack */
      sd_ss_update_hybr2_pref_list();
    }
    
    return sd_misc_gwl_map_sys_lost_to_acq_fail( ss,
                                                 cmd->sim_state,
                                                 cmd->data_suspend,
                                                 cmd->rtrn_act_ptr,
                                                 cmd->acq_status);
  }
  sdss_set_pref_chg_in_ltd_reg(ss,FALSE);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ss )
  {
    case SD_SS_MAIN:
    case SD_SS_HYBR_1:

      /* Process the SS-Event that is associated with this SS-Indication.
      */
      act = sd_ss_event_proc( ss, SDSS_EVT_GWL_OPR_SYS_LOST, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_2:
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      /*Check for LTE service due to NO TRM RESOURCE*/

      if( sdss_is_sfmode_1x_srlte(SYS_MODEM_AS_ID_1) && 
          (cmd->acq_status == SYS_ACQ_STATUS_NO_RESOURCE) && sdss_is_flag_set(SD_SS_HYBR_2, SDSS_FLAG_HYBR2_BSR))
      {
        /* Update PREF_LIST if this is for HYBR2 stack */
        sd_ss_update_hybr2_pref_list();
		
        act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_HYBR_2_USER_SS_PREF, cmd->rtrn_act_ptr );  
      }
      else
      {
        act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_HYBR_2_GW_OPR_SYS_LOST, cmd->rtrn_act_ptr );
      }
      break;

    case SD_SS_HYBR_3:
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_HYBR_3_GW_OPR_SYS_LOST, cmd->rtrn_act_ptr );
      #endif
      break;

    case SD_SS_MAX:
    default:
      SD_ERR_1(" wrong ss %d",ss);
      act = SD_SS_ACT_ACQ_GWL;
      break;
  }

#ifdef FEATURE_WRLF_SYSTEM_SEL
  /* If the last SERVICE_REQ was sent for W in Manual CSG mode and RLF occured,
        update SD for sending SERVICE_REQ on same CSG till the suitable search timer
        expires */
  sdss_restore_csg_info_to_ss_on_wrlf( ss, ss_ptr->rplmn, &ss_ptr->true_curr_act, cmd->rtrn_act_ptr); 
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */

  SD_ASSERT( act & (SD_SS_ACT_PWR_SAVE |
                    SD_SS_ACT_ACQ_GWL) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset LAC and RAC.
  */
  si_info_ptr->mode_info.gwl.lac             = 0;
  si_info_ptr->mode_info.gwl.rac_or_mme_code = 0;

  return act;

} /*  sdcmd_gw_opr_srv_lost_ss_3() */


#if defined ( FEATURE_WRLF_SYSTEM_SEL)
/*===========================================================================

FUNCTION sdcmd_gw_opr_srv_lost3

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_gw_opr_srv_lost3(
      sd_ss_ind_gw_opr_srv_lost_cmd *cmd
)
  {
  sd_ss_act_e_type    act;
  sd_ss_e_type        ss = SD_SS_MAIN;
  sd_si_info_s_type   *si_info_ptr = &sd_si_ptr(ss)->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->sim_state, SYS_SIM_STATE_MAX );

  SD_MSG_HIGH_3("WRLF:gw srv lost sim state %d data suspend %d suitable_search_time %d",\
               cmd->sim_state, cmd->data_suspend, cmd->suitable_search_time );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  /* Update the sim state.
  */
  sd_si_update_sim_state( cmd->sim_state, ss );

  /* Update the data suspend state.
  */
  sd_si_update_data_suspend_state( ss, cmd->data_suspend, SYS_SRV_STATUS_NO_SRV );

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update sdss's srv_status too.
  */
  sdss_update_srv_status( ss,
                          SYS_SRV_STATUS_NO_SRV,
                          SYS_SRV_DOMAIN_NO_SRV
                        );

  /* set the connected mode flag and suitable search time */
  if (!sdss_get_is_full_srv_req_during_emerg(ss))
  {
    sdss_set_wcdma_connected_mode(cmd->suitable_search_time, ss);
  }
  else
  { /* Do not set W connected mode flag for CSFB E911 orig or a VOLTE call that experianced
      ** HARD FAILURE.
      */
    if ((sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG) ||
         (sdss_get_ss_state(ss) == SDSS_STATE_ACQ))
    {
      SD_MSG_HIGH_0("sdcmd_gw_opr_srv_lost3 - do not set WCDMA connected mode");
    }
  }

  if (!sdss_get_is_full_srv_req_during_emerg(ss) &&
      !sdss_is_emer_orig_in_volte_only(ss))
  {
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If ue is in an emergency call origination and the current RAT priority list
  ** does not match the emerg capability, update the current RAT priority list
  */
  sdss_check_reset_emerg_rat_pri_list_info(ss);
  }

  /* For PPCSFB origination, NAS takes care of FULL SRV acquisition. So reset the is_full_srv_req 
   ** flag to FALSE
   */
  if ((sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG) && 
      sdss_get_is_full_srv_req_during_emerg(ss))
  {
    SD_MSG_HIGH_0("sdcmd_gw_opr_srv_lost3 - reset full srv req flag due to PPCSFB orig");
    sdss_update_is_full_srv_req_during_emerg( ss, FALSE);
    sdss_check_reset_emerg_rat_pri_list_info(ss);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sd_misc_gwl_is_sys_lost_map_acq_fail(ss) )
  {
    if(ss == SD_SS_HYBR_2 &&
       sdss_is_1x_sxlte())
    {
      /* Update PREF_LIST if this is for HYBR2 stack */
      sd_ss_update_hybr2_pref_list();
    }
    
    return sd_misc_gwl_map_sys_lost_to_acq_fail( ss,
                                                 cmd->sim_state,
                                                 cmd->data_suspend,
                                                 cmd->rtrn_act_ptr,
                                                 cmd->acq_status);
  }
  sdss_set_pref_chg_in_ltd_reg(ss,FALSE);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss,
                          sdss_event_map(ss,SDSS_EVT_GWL_OPR_SYS_LOST),
                          cmd->rtrn_act_ptr );

  SD_ASSERT( act & SD_SS_ACT_ACQ_GWL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Reset LAC and RAC.
  */
  si_info_ptr->mode_info.gwl.lac             = 0;
  si_info_ptr->mode_info.gwl.rac_or_mme_code = 0;

  return act;

} /*  sd_ss_ind_gw_opr_srv_lost3() */


/*===========================================================================

FUNCTION sdcmd_wcdma_suitable_search_end

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS 

===========================================================================*/
sd_ss_act_e_type          sdcmd_wcdma_suitable_search_end(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;
  sd_ss_s_type        *ss_ptr     = sd_ss_ptr(cmd->ss);

  /*
  ** If the UE is in WCDMA Connected mode, stop the suitable search time.
  */

  SD_MSG_HIGH_0("WRLF:Received sutable seach end ind");

  if((sdss_is_wcdma_connected_mode (cmd->ss)))
  {
     /* reset the suitable search time*/
    sdss_reset_wcdma_suitable_search_time(cmd->ss);

    /* Resetting the CSG Id and CSG Rat, when suitable search period ends */
    sdss_set_csg_data(cmd->ss, SYS_CSG_ID_INVALID, SYS_RAT_NONE);


     SD_MSG_HIGH_0("WRLF:Ending suitable search");

    /* Here, it's intentional to give SD_SS_MAIN as a parameter. The requirement
    ** for Suitable Search End is that SD should immediately act and send
    ** new service request in case of MMSS/SVLTE world, but in 3GPP only world,
    ** SD should remains CONTINUE and NAS will do all the neccessary scans.
    ** Once done, NAS will inform REGPRX with SERVICE_CNF.
    **
    ** Hence, if 3GPP-only is detected on Main, we can safely return CONTINUE without
    ** trigger further action.
    */
    if (!sdss_is_cap_3gpp_only(SD_SS_MAIN))
    {
      if (ss_ptr->true_curr_act.act == SD_SS_ACT_ACQ_GWL  &&
          ss_ptr->true_curr_act.prm.acq_gwl.srv_req_type ==
                     SD_SS_SRV_REQ_TYPE_WCDMA_SUITABLE_SEARCH)
      {
        SD_MSG_HIGH_1("WRLF:Treating suitable search end based on ss_state: %d", ss_ptr->ss_state);      
        if( ss_ptr->ss_state == SDSS_STATE_OPR)
        {
          act = sd_ss_event_proc(cmd->ss,
                           sdss_event_map(cmd->ss,SDSS_EVT_GWL_OPR_SYS_LOST),
                           cmd->rtrn_act_ptr );

        }
        else if (ss_ptr->ss_state == SDSS_STATE_ACQ)
        {
          act = sd_ss_event_proc(cmd->ss,
                            sdss_event_map(cmd->ss,SDSS_EVT_GWL_ACQ_PLMN_FAILED),
                            cmd->rtrn_act_ptr );
        }
      }
    }
  }
  if(cmd->rtrn_act_ptr != NULL)
  {
     cmd->rtrn_act_ptr->act = act;
  }

  return act;
}


/*===========================================================================

FUNCTION sdcmd_wcdma_conn_mode_end

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS 

===========================================================================*/
sd_ss_act_e_type          sdcmd_wcdma_conn_mode_end(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;
  sd_ss_e_type      ss = cmd->ss;

  /*
  ** If the UE is in WCDMA Connected mode, come out to idle mode.
  */

  SD_MSG_HIGH_0("WRLF:Received connected mode end ind");


  if((sdss_is_wcdma_connected_mode (ss)))
  {
     sdss_reset_wcdma_connected_mode(ss);
     SD_MSG_HIGH_0("WRLF:Ending connected mode search procedure");

  }

  if(cmd->rtrn_act_ptr != NULL)
  {
     cmd->rtrn_act_ptr->act = act;
  }

  return act;
}

#endif /* FEATURE_WRLF_SYSTEM_SEL */


/*===========================================================================

FUNCTION sdcmd_lte_opr_srv_lost_1

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_lte_opr_srv_lost_1(
        sd_ss_ind_lte_opr_srv_lost_cmd  *cmd
)
{
  sd_ss_act_e_type    act;
  sd_ss_e_type        ss = cmd->ss;
  sd_ss_s_type        *ss_ptr     = sd_ss_ptr(ss);
  sd_si_info_s_type   *si_info_ptr = &sd_si_ptr(ss)->si_info;
  boolean             is_emerg_call_orig = sdss_is_emergency_call_orig(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->sim_state, SYS_SIM_STATE_MAX );

  SD_MSG_HIGH_4("LTE srv RLF ss=%d sim_state=%d data_suspend=%d acq_status=%d",
              ss, cmd->sim_state, cmd->data_suspend, cmd->acq_status );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save the rplmn in global */
  if(NULL != cmd->registered_plmn)
  {
    *(&(ss_ptr->rplmn)) = *(cmd->registered_plmn);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined (FEATURE_SD_LTE)
  /* Update the sim state.
  */
  if(sdss_is_1x_sxlte() && (ss!= SD_SS_HYBR_3))
  {
    sd_si_update_sim_state( cmd->sim_state, SD_SS_MAIN );
    sd_si_update_sim_state( cmd->sim_state, SD_SS_HYBR_2 );
  }
  else
  {
    sd_si_update_sim_state( cmd->sim_state, ss );
  }

  /* Update the data suspend state.
  */
  sd_si_update_data_suspend_state( ss, cmd->data_suspend, SYS_SRV_STATUS_NO_SRV );
#endif

  /* Reset acq_status */
  sdss_update_acq_status(ss, cmd->acq_status);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update sdss's srv_status too.
  */
  sdss_update_srv_status( ss,
                          SYS_SRV_STATUS_NO_SRV,
                          SYS_SRV_DOMAIN_NO_SRV
                        );

  /*Reset Pref Update Reason*/
  if( sdss_is_pref_update_reas(ss, SD_SS_PREF_UPDATE_REASON_TRM_UNLOCK) && 
      cmd->acq_status == SYS_ACQ_STATUS_NO_RESOURCE)
  {
    sdss_set_pref_update_reason_ss(ss,SD_SS_PREF_UPDATE_REASON_NONE);
  }

  /* For PPCSFB cases, NAS takes care of complete GWL search.
  ** So no need of FULL SRV acqusiition. For VOLTE call if RLF happens,
  ** reset the is_full_srv_req falg upon RLF to proceeed with E911 design.
  */
  if (sdss_get_is_full_srv_req_during_emerg(ss))
  {
    SD_MSG_HIGH_0("sdcmd_lte_opr_srv_lost_1 - reset full srv req flag due to PPCSFB orig");
    sdss_check_reset_emerg_rat_pri_list_info(ss);
    sdss_update_is_full_srv_req_during_emerg(ss, FALSE);
  }

  /* CSFB call in progress, No  need to do the the RLF recovery
  ** As we can aquire service on G/W directly and place the call
  */
  if( sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_PPCSFB_ORIG) )
  {
    SD_MSG_HIGH_0("PPCSFB call skip RLF");
  }
  /* If SIM is Ps invalid and we are not in an emergency call orig, No need to do the the RLF recovery
  */
  else if( !is_emerg_call_orig && 
           (cmd->sim_state == SYS_SIM_STATE_PS_INVALID || cmd->sim_state == SYS_SIM_STATE_CS_PS_INVALID)
          )
  {
    SD_MSG_HIGH_1("skip RLF, sim=%d", cmd->sim_state);
  }
  /* If UE is in an emergency call and was acquired on LTE service that supported emergency
  ** do RLF recovery 
  */
  else if (is_emerg_call_orig) 
  {
    sdss_set_lte_connected_mode(ss,cmd->connected_time);
    sdss_update_is_RLF_during_emerg( ss,TRUE);
    SD_MSG_HIGH_0("RLF during emerg call");

    /* If last srv status was not full service, use expanded capability list */
    if (sd_si_is_full_srv_on(ss)) 
    {
      sdss_update_rat_pri_list_with_idle_cap(ss);
    }
    else
    {
      sdss_check_reset_emerg_rat_pri_list_info(ss);
    }
  }
  else
  {
    sdss_set_lte_connected_mode(ss,cmd->connected_time);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If not in RLF during emergency call, allow the service lost to be mapped to
  ** acq failure if necessary */
  if( sd_misc_gwl_is_sys_lost_map_acq_fail(ss) &&
     !sdss_get_is_RLF_during_emerg(ss))
  {
    if(ss == SD_SS_HYBR_2 &&
       sdss_is_1x_sxlte())
    {
      /* Update PREF_LIST if this is for HYBR2 stack */
      sd_ss_update_hybr2_pref_list();
    }
    
    return sd_misc_gwl_map_sys_lost_to_acq_fail( ss,
                                                 cmd->sim_state,
                                                 cmd->data_suspend,
                                                 cmd->rtrn_act_ptr,
                                                 cmd->acq_status);
  }
  sdss_set_pref_chg_in_ltd_reg(ss,FALSE);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ss_ptr->ss_state != SDSS_STATE_OPR)
  {
    SD_MSG_HIGH_1("LTE Srv Lost in state -%d", ss_ptr->ss_state);
    /* If SD is in acquisition state, don't declare OPR_SYS_LOST, but trigger
    ** USER_SS_PREF, it will eventually go to INT_SRV_LOST.
    ** This it to handle LTE RLF before we get full acquisition.
    */
    act = sd_ss_event_proc(ss,
                           sdss_event_map(ss,SDSS_EVT_USER_SS_PREF),
                           cmd->rtrn_act_ptr );

    return act;
  }

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss,
                          sdss_event_map(ss, SDSS_EVT_GWL_OPR_SYS_LOST),
                          cmd->rtrn_act_ptr );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset LAC and RAC.
  */
  si_info_ptr->mode_info.gwl.lac             = 0;
  si_info_ptr->mode_info.gwl.rac_or_mme_code = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /*  sd_ss_ind_lte_opr_srv_lost() */


/*===========================================================================

FUNCTION sdcmd_gw_opr_srv_info_ss_1

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  
SIDE EFFECTS 

===========================================================================*/
sd_ss_act_e_type          sdcmd_gw_opr_srv_info_ss_1(
        sd_ss_ind_gw_srv_info_cmd *cmd
)
{
  sd_ss_act_e_type         act = SD_SS_ACT_CONTINUE;
  sd_ss_e_type             ss = cmd->ss;
  sd_band_e_type           band;
  boolean                  is_band_mismatch = FALSE;
  sdsr_ref_type            sr_ref           = SDSR_REF_NULL;
  sys_srv_status_e_type    service_status_rcvd = cmd->service_status;
  sdsr_e_type              colloc_lst = SDSR_MAX;

  /* Point at current serving system.
  */
  sdsr_ref_type       srv_sys_ref     = sdsr_list_get_ref( sdss_sr_list_map2( ss, SDSR_SRV_SYS), 0 );
  sdsr_s_type         *srv_sys_ptr    = sdsr_list_get_ptr( sdss_sr_list_map2( ss, SDSR_SRV_SYS), 0 );
  sdsr_s_type         *acq_sys_ptr    = NULL;
  sd_si_mode_info_u_type  *mode_info_ptr = NULL;
  sd_ss_act_s_type        *ss_act_ptr = NULL;
  sys_srv_status_e_type     ss_srv_status = SYS_SRV_STATUS_NO_SRV;
  sd_ss_s_type              *ss_ptr   = sd_ss_ptr(ss);


  if(cmd->mode == SD_MODE_LTE)
  {
       band = sdss_map_band_pref_to_band_lte(cmd->band_pref.lte_band);
  }
  else if(cmd->mode == SD_MODE_TDS)
  {
       band = sdss_map_band_pref_to_band_tds(cmd->band_pref.chgwt_band );
  }
  else
  {
       band = sdss_map_band_pref_to_band(cmd->band_pref.chgwt_band );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->sim_state, SYS_SIM_STATE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->plmn_srv_cap, SYS_SRV_CAPABILITY_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->service_status, SYS_SRV_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->roaming_ind, SYS_ROAM_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->srv_domain, SYS_SRV_DOMAIN_MAX );
  SD_ASSERT( cmd->mode == SD_MODE_GSM || cmd->mode == SD_MODE_WCDMA ||
             cmd->mode == SD_MODE_LTE || cmd->mode == SD_MODE_TDS);
  SD_ASSERT_ENUM_IS_INRANGE( cmd->cs_service_status, SYS_SRV_STATUS_MAX );

  #if defined(FEATURE_LTE)
  SD_ASSERT( cmd->lte_cs_capability < SYS_LTE_CS_CAPABILITY_MAX );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_HIGH_5("SRV_3GPP: GWL oh,mode=%d,ss=%d, status=%d, sim_state=%d, band=%d",
              cmd->mode, ss, cmd->service_status,cmd->sim_state, band );
  SD_MSG_HIGH_6("srv info %d %d %d. cs_service_status=%d, srv_domain=%d, cmd->extend_srv_info=%d",
              cmd->is_plmn_forbidden,
              cmd->roaming_ind,
              cmd->data_suspend, cmd->cs_service_status,cmd->srv_domain,
              cmd->extend_srv_info);
  
  #if defined(FEATURE_LTE)
  
  SD_MSG_HIGH_4("lte_cs_capability=%d, reg_domain=%d ims_voice=%d, ims_emerg=%d",
  				cmd->lte_cs_capability, cmd->reg_domain, cmd->lte_ims_voice_avail, cmd->lte_ims_emerg_avail);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Save the rplmn in global */
  if(NULL != cmd->registered_plmn)
  {
    *(&(ss_ptr->rplmn)) = *(cmd->registered_plmn);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(srv_sys_ptr == NULL)
  {
    SD_ERR_0("srv_sys_ptr NULL");
    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check if UE has moved to a band that is not present in current SRV_SYS
  */
  switch(cmd->mode)
  {
    case SD_MODE_LTE:
       {
         sys_lte_band_mask_e_type srv_sys_lte_band = sdsr_map_sys_rec_lte_band_to_lte_band_pref(srv_sys_ptr->sys.lte_band);
         if( !SYS_LTE_BAND_MASK_CHK_IF_OVERLAP(&cmd->band_pref.lte_band, &srv_sys_lte_band) )
      {
        is_band_mismatch = TRUE;
      }
       }
      break;
    case SD_MODE_TDS:
      if( !SD_BAND_CONTAIN(cmd->band_pref.chgwt_band , srv_sys_ptr->sys.tds_band) )
      {
        is_band_mismatch = TRUE;
      }
      break;
    default:
      if( !SD_BAND_CONTAIN(cmd->band_pref.chgwt_band , srv_sys_ptr->sys.band) )
      {
        is_band_mismatch = TRUE;
      }
      break;

  }//  switch(mode)

  if( is_band_mismatch )
  {
    SD_MSG_HIGH_0("MMSS:band change in oh info");
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  /* Update the sim state.
  */
  if(sdss_is_1x_sxlte() && (ss!= SD_SS_HYBR_3))
  {
    sd_si_update_sim_state( cmd->sim_state, SD_SS_MAIN );
    sd_si_update_sim_state( cmd->sim_state, SD_SS_HYBR_2 );
  }
  else
  {
    sd_si_update_sim_state( cmd->sim_state, ss );
  }

  #endif

  sdss_read_srv_status( ss, &ss_srv_status, NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd->extend_srv_info == SYS_EXTEND_SRV_INFO_T3402_STARTED) &&
      (cmd->service_status != SYS_SRV_STATUS_LIMITED))
  {
    cmd->extend_srv_info = SYS_EXTEND_SRV_INFO_NONE;
  }
  
  /* Update LTE CS CAPABILITY in SDSS */
   sdss_update_nas_addl_info(ss, cmd->lte_cs_capability, cmd->extend_srv_info);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If we acquired limited service and we did not asked for limited service
  ** or in manual mode we got the limited service on a forbidden PLMN which
  ** is not user selected PLMN, process this as acquisition failure.
  */
  ss_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (ss_act_ptr != NULL)
  {
    memset(ss_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);
  acq_sys_ptr     = sdsr_list_get_ptr( sdss_sr_list_map2( ss, SDSR_ACQ_SYS), 0 );

  if( ( (!cmd->is_plmn_forbidden) ||
        ( acq_sys_ptr != NULL &&
         !sys_plmn_match ( acq_sys_ptr->sid.plmn.plmn_id, cmd->plmn ) &&
          ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_MANUAL
         )
      )
                      &&
      cmd->service_status == SYS_SRV_STATUS_LIMITED
                        &&
      ss_act_ptr->prm.acq_gwl.net_sel_type !=
      SYS_NETWORK_SELECTION_MODE_LIMITED
                        &&
      ss_act_ptr->prm.acq_gwl.net_sel_type !=
      SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION
                        &&
      ss_act_ptr->prm.acq_gwl.net_sel_type !=
      SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY
                        &&
      ss_act_ptr->prm.acq_gwl.net_sel_type !=
      SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY
                        &&
      ss_act_ptr->prm.acq_gwl.net_sel_type !=
      SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION
      &&
      (ss_srv_status == SYS_SRV_STATUS_LIMITED_REGIONAL &&
       !sdss_is_target_cap_3gpp_only(ss))
     )
  {

    modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);

    /* If preferences have changed, map to Service loss so OOS can restart. */
    if( sdss_get_pref_chg_in_ltd_reg(ss) )
    {
      sd_ss_ind_gw_opr_srv_lost_cmd srv_lost_cmd;

      SD_MSG_HIGH_1("Ltd_Reg: LTD_REG -> LTD mapped to sys_lost, ss_state=%d",
                             sd_ss_ptr(ss)->ss_state);
      srv_lost_cmd.ss                   = ss;
      srv_lost_cmd.rtrn_act_ptr         = cmd->rtrn_act_ptr;
      srv_lost_cmd.sim_state            = cmd->sim_state;
      srv_lost_cmd.data_suspend         = cmd->data_suspend;
      srv_lost_cmd.suitable_search_time = 0;
      srv_lost_cmd.registered_plmn      = NULL;
      srv_lost_cmd.acq_status           = SYS_ACQ_STATUS_SUCCESS;

      sdss_set_pref_chg_in_ltd_reg(ss,FALSE);
      return sdcmd_gw_opr_srv_lost_ss_3(&srv_lost_cmd);
    }
    else
    {
    sd_ss_ind_gw_acq_plmn_failed_cmd  failcmd;

    SD_MSG_HIGH_1("Ltd_Reg: LTD_REG -> LTD mapped to acq_plmn_failed, ss_state=%d",
                           sd_ss_ptr(ss)->ss_state);

    failcmd.ss = cmd->ss;
    failcmd.sim_state = cmd->sim_state;
    failcmd.data_suspend = cmd->data_suspend;
    failcmd.scan_status = SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND;
    failcmd.rtrn_act_ptr = cmd->rtrn_act_ptr;
    failcmd.lte_cs_capability = cmd->lte_cs_capability;
    failcmd.extend_srv_info = cmd->extend_srv_info;
    failcmd.csg_info_ptr = cmd->csg_info_ptr;
    failcmd.registered_plmn = cmd->registered_plmn;
	failcmd.acq_status = SYS_ACQ_STATUS_SUCCESS;

    /* srv pref list is updated when limited regional service is acquired.
    ** For ltd_reg->limited transition, we need to repropulate the list.
    */
    #ifdef FEATURE_MMODE_SC_SVLTE
    if (ss == SD_SS_HYBR_2 &&
        (sd_ss_ptr(ss)->ss_state == SDSS_STATE_ACQ || 
         sd_ss_ptr(ss)->ss_state == SDSS_STATE_VRFY))
    {
      sd_ss_update_hybr2_pref_list();
    }
    #endif
    return sdcmd_gw_acq_plmn_failed_ss_2(&failcmd);
  }
  }

  /* If Full service acquired, clear the flag which indicates the change in 
  ** preferences during limited regional service. Full service indication is
  ** not mapped to anything else and will ensure that the new preferences are
  ** considered during processing.
  */
  if( cmd->service_status == SYS_SRV_STATUS_SRV )
  {
    sdss_set_pref_chg_in_ltd_reg(ss,FALSE);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( is_band_mismatch == TRUE &&
      ss == SD_SS_MAIN &&
      sdss_is_mmss_operation(SD_SS_MAIN)  )
  {
    sd_sid_plmn_s_type   sid_exp;
    SD_MSG_HIGH_1("MMSS: curr srv sys ref %d",srv_sys_ref);

    /* Store the new system in the data base as automatic plmn selection
    ** record and copy it to the BACKUP_LST
    */
    memset(&sid_exp,0, sizeof(sid_exp));
    sid_exp.net_select_mode = SD_NET_SELECT_MODE_AUTO;
    sid_exp.prm.gwl.plmn.lac = PRL_WILDCARD_LAC;
    sys_plmn_undefine_plmn_id(&(sid_exp.prm.gwl.plmn.plmn_id));
    sid_exp.prm.gwl.num_rats = 1;
    sid_exp.prm.gwl.gwl_pri_lst[0] = cmd->mode;

    sdsr_list_clr( SDSR_BACKUP_LST );
    sdsr_list_store_and_app3( SDSR_BACKUP_LST,
                              SDSR_MAX,
                              SDSR_MAX,
                              SD_MODE_GWL,
                              (cmd->mode == SD_MODE_LTE || cmd->mode == SD_MODE_TDS) ? 0 : cmd->band_pref.chgwt_band ,
                              (cmd->mode == SD_MODE_LTE ) ? sdsr_map_lte_band_pref_to_sys_rec_lte_band(cmd->band_pref.lte_band)
                                                          : sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                              (cmd->mode == SD_MODE_TDS ) ? cmd->band_pref.chgwt_band  : 0,
                              0,
                              &sid_exp,
                              SD_NO_CHANGE_SYS_IDX );
    srv_sys_ptr     = sdsr_list_get_ptr( SDSR_BACKUP_LST, 0 );

    if( srv_sys_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }

    /* Update with acquired system mode and PLMN for GW sr to perform correct mathcing.
    */
    srv_sys_ptr->sid.gw.mode = cmd->mode;
    srv_sys_ptr->sid.plmn.plmn_id  = cmd->plmn;
    srv_sys_ptr->srv_cap = cmd->plmn_srv_cap;
    if( SD_MODE_LTE == cmd->mode )
    {
      srv_sys_ptr->sid.gw.band.lte_band  = sdsr_map_lte_band_pref_to_sys_rec_lte_band(cmd->band_pref.lte_band);
    }
    else
    {
      srv_sys_ptr->sid.gw.band.chgwt_band = cmd->band_pref.chgwt_band;
    }

    /* Find the entry in the PRL that matches with the acquired system.
    ** If there is a match, the system will be stored in SDSR_GW_CAND_SYS
    ** within sdprl_get_info_base()
    */
    sdsr_list_clr( SDSR_GW_CAND_SYS );
    (void)sdprl_get_info_base( SD_SS_MAIN,
                               SDSR_BACKUP_LST,
                               0,
                               SDSR_BACKUP_LST,
                               0,
                               SDSR_MAX,
                               SDSR_MAX,
                               SDSR_MAX,
                               SDPRL_EXTRACT_MATCH_ONLY,
                               NULL);

    /* update SRV sys per the GW_CAND_SYS if it is not NULL.
    */
    sr_ref = sdsr_list_get_ref( sdss_sr_list_map2( ss, SDSR_GW_CAND_SYS ), 0 );

    /* Update the last user system that contains the same bands as that in
    ** in the idle system.
    */
    SD_MSG_HIGH_1("MMSS:opr_srv_info: cand sys %d",sr_ref);    
    if(sr_ref != SDSR_REF_NULL )
    {
      sdsr_list_put_ref( sdss_sr_list_map2( ss, SDSR_SRV_SYS), sr_ref );
      sdsr_list_update_post_gwl_sys( ss, sr_ref, TRUE, TRUE, TRUE );
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the net_sel_mode in srv_sys based on the service_status
  */
  sd_ss_gw_srv_sys_net_sel_update(ss, cmd->service_status);

  /* Refresh the serving system pointer now that it might have been updated.
  */
  srv_sys_ref     = sdsr_list_get_ref( sdss_sr_list_map2( ss, SDSR_SRV_SYS), 0 );
  srv_sys_ptr     = sdsr_list_get_ptr( sdss_sr_list_map2( ss, SDSR_SRV_SYS), 0 );

  if( srv_sys_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* Update with acquired system mode for GW sr to perform correct mathcing.
  */
  srv_sys_ptr->sid.gw.mode = cmd->mode;
  if( SD_MODE_LTE == cmd->mode )
  {
    srv_sys_ptr->sid.gw.band.lte_band  = sdsr_map_lte_band_pref_to_sys_rec_lte_band(cmd->band_pref.lte_band);
  }
  else
  {
    srv_sys_ptr->sid.gw.band.chgwt_band = cmd->band_pref.chgwt_band;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the
  ** serving syste, GSM/WCDMA/LTE system.
  */
  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) &&
  	 (ss == SD_SS_HYBR_3 || !(sdss_is_1x_sxlte() || sdss_is_sglte()) ))
  {
    colloc_lst = 
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      (ss == SD_SS_HYBR_3) ? SDSR_HYBR_3_SRV_COLLOC_LST:
      #endif
      ((ss == SD_SS_HYBR_2) ? 
                              SDSR_HYBR_1_SRV_COLLOC_LST:SDSR_SRV_COLLOC_LST);
  }
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)|| defined(FEATURE_SD_LTE)
  
  if(sdss_is_mmss_operation(ss))
  	 sdprl_free_gwl_more_pref_list(SYS_SYS_MODE_CDMA);
  
    (void) sdprl_gw_get_info(
                    ss,                  /* MAIN system selection */
                    sdss_sr_list_map2( ss, SDSR_SRV_SYS),        /* Get PRL info for this list */
                    0,                   /* and list position */
                    sdss_sr_list_map2( ss, SDSR_SRV_GEO_LST),    /* Put GEO systems into this list */
                    sdss_sr_list_map2( ss, SDSR_SRV_PREF_LST),   /* Put PREF systems into this list */
                    colloc_lst,
                    SDPRL_EXTRACT_ALL,   /* Get info regardless of PRL match */
                    cmd->plmn,                /* PLMN id of acqed sys */
                    cmd->lac_tac_rac.lac,
                    cmd->plmn_srv_cap,        /* Service capability of acqed sys */
                    cmd->service_status,      /* Service status */
                    cmd->is_plmn_forbidden,   /* Is acqed system forbidden? */
                    cmd->roaming_ind);         /* Roaming indication for acqed sys */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If UE in CAMPED_ONLY mode, check the reg_domain and adjust the
  ** service status for internal SD evaluation. The service indication will
  ** reflect the NON-ADJUSTED value for service status.
  */
  cmd->service_status = sd_misc_adjust_service_status_in_camp_mode( service_status_rcvd, cmd->reg_domain );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update sdss's srv_status too.
  */
  sdss_update_srv_status( ss, cmd->service_status, cmd->srv_domain );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if BSR timer uptime needs to be reset
  */
  
  if(!(ss_srv_status == SYS_SRV_STATUS_LIMITED
    && service_status_rcvd == SYS_SRV_STATUS_LIMITED
    && sdss_is_resel_mode_full(ss))
    )
  {
    sdss_check_bsr_timer(ss);
  }

  /* If we receive overhead info when csg app search is in progress return continue
    */
    
   (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);
   if((ss_act_ptr->act == SD_SS_ACT_ACQ_GWL) && 
     (ss_act_ptr->prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_CSG_SELECTION_APP) &&
     (sdss_get_ss_state(ss) == SDSS_STATE_ACQ))
   {
     modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);
     return SD_SS_ACT_CONTINUE;
   }

  /* update manual bst if needed*/
  if(cmd->is_plmn_forbidden == FALSE)
  {
    sdss_man_srch_bst_node_s_type success_info;
    success_info.band_pref = cmd->band_pref;
    success_info.mode = cmd->mode;
    success_info.plmn = cmd->plmn;
    sdss_check_update_manual_search_bst(&success_info);
  }

  /* IF RAL band based granularity is required then add new record in sdsr table 
  and add that reference in acqed_lst, rather than adding all the bands from acq sys to
  RAL */  
  
  /* Insert the acquired system at the beginning of the SDSR_ACQED_LST and
  ** remove any duplicates.
  */
  /* for intra band transfers and redirections/reselections intra 3GPP
  Need to update RAL */
  
  if(sdss_is_ral_stored_in_efs(ss))
  {
    sd_sid_plmn_s_type   sid_exp;
    sdsr_ref_type       ral_sys_ref;
    memset(&sid_exp,0, sizeof(sid_exp));

    /* fill sid_expected parameter */
    sid_exp.net_select_mode = SD_NET_SELECT_MODE_AUTO; 
    sys_plmn_undefine_plmn_id(&(sid_exp.prm.gwl.plmn.plmn_id));
    sid_exp.prm.gwl.plmn.lac = PRL_WILDCARD_LAC; 
    sid_exp.prm.gwl.num_rats = 1;
    sid_exp.prm.gwl.gwl_pri_lst[0] = cmd->mode;

    /* add record in SR table */
    ral_sys_ref = sdsr_tbl_store11( SD_MODE_GWL,
                                    (cmd->mode == SD_MODE_LTE || cmd->mode == SD_MODE_TDS) ? 0 : cmd->band_pref.chgwt_band,
                                    (cmd->mode == SD_MODE_LTE ) ? sdsr_map_lte_band_pref_to_sys_rec_lte_band(cmd->band_pref.lte_band) :
                                                                  sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                                    (cmd->mode == SD_MODE_TDS ) ? cmd->band_pref.chgwt_band : 0,
                                    0, 
                                    &sid_exp, 
                                    SD_NO_CHANGE_SYS_IDX );
    if(ral_sys_ref == SDSR_REF_NULL)
    {
      SD_MSG_HIGH_0("RAL: not able to add single band entry into sdsr_table"); 
    }
    else
    {
      sdsr_s_type* ral_sr_ptr = NULL;
      SD_MSG_HIGH_1("RAL: added ral_sys_ref %d in sdsr_table",ral_sys_ref);
      ral_sr_ptr = sdsr_tbl_get_ptr( ral_sys_ref );
      if(ral_sr_ptr != NULL)
      {
        ral_sr_ptr->sid.gw.mode = cmd->mode;
        if( SD_MODE_LTE == cmd->mode )
        {
          ral_sr_ptr->sid.gw.band.lte_band  = sdsr_map_lte_band_pref_to_sys_rec_lte_band(cmd->band_pref.lte_band);
        }
        else
        {
          ral_sr_ptr->sid.gw.band.chgwt_band = cmd->band_pref.chgwt_band;
        }
      }
      else
      {
        SD_MSG_HIGH_0("RAL: error in getting sr_ptr");
      }
      sdsr_list_insert_ref( sdss_sr_list_map2( ss, SDSR_ACQED_LST ), 0, ral_sys_ref, TRUE );
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ss )
  {
    case SD_SS_MAIN:
    case SD_SS_HYBR_1:

      /* Process the SS-Event that is associated with this SS-Indication.
      */
      act = sd_ss_event_proc( ss, SDSS_EVT_GWL_OPR_SRV_INFO, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_2:
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_HYBR_2_GW_OPR_SRV_INFO, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_3:
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_HYBR_3_GW_OPR_SRV_INFO, cmd->rtrn_act_ptr );
      #endif
      break;

        case SD_SS_MAX:
    default:
      SD_ERR_1(" wrong ss %d",ss);
      act = SD_SS_ACT_CONTINUE;
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_GWL) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE (i.e., we have selected a new serving system),
  ** do the following updates.
  */
  mode_info_ptr = (sd_si_mode_info_u_type *)modem_mem_alloc
                      (sizeof(sd_si_mode_info_u_type), MODEM_MEM_CLIENT_MMODE);
  if (mode_info_ptr != NULL)
  {
    memset(mode_info_ptr, 0, sizeof(sd_si_mode_info_u_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  if( act == SD_SS_ACT_CONTINUE || (act == SD_SS_ACT_ACQ_GWL && service_status_rcvd == SYS_SRV_STATUS_LIMITED) )
  {
    if( cmd->mode == SD_MODE_GSM || cmd->mode == SD_MODE_WCDMA)
    {
      mode_info_ptr->gwl.lac = cmd->lac_tac_rac.lac;
    }
    #ifdef FEATURE_SD_LTE
    else
    {
      mode_info_ptr->gwl.tac = cmd->lac_tac_rac.tac;
    }
    #endif

    mode_info_ptr->gwl.rac_or_mme_code = cmd->lac_tac_rac.rac_or_mme_code;

    /* Update the system indicators per the acquired system.
    */

    srv_sys_ref = sdsr_list_get_ref( sdss_sr_list_map2( ss, SDSR_SRV_SYS), 0 );


    sd_si_update( ss,
                  SD_SI_UPDATE_REG,
                  service_status_rcvd,
                  cmd->srv_domain,
                  srv_sys_ref,
                  FALSE,
                  &cmd->mode,
                  &band,
                  NULL,
                  mode_info_ptr,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  cmd->cs_service_status,
                  cmd->lte_cs_capability,
                  cmd->sib8_available,
                  cmd->lte_ims_voice_avail,
                  cmd->lte_ims_emerg_avail? SYS_IMS_LTE_EMERG_SUPP_AVAILABLE : SYS_IMS_LTE_EMERG_SUPP_NOT_AVAILABLE,
                  0, /* Request Id not needed */
                  cmd->extend_srv_info,
                  cmd->reg_domain,
                  cmd->csg_info_ptr,
                  FALSE
                );

    sdss_update_srv_status( ss, cmd->service_status, cmd->srv_domain );

    if(act == SD_SS_ACT_CONTINUE)
    {

       (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);
        if ( cmd->service_status == SYS_SRV_STATUS_SRV &&
             ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_MANUAL )
        {
          /* Point at manual system record */
          sdsr_s_type         *user_sys_ptr  = NULL;

          /* Fetch the user system record */
          user_sys_ptr   = sdsr_list_get_ptr( sdss_sr_list_map2( ss, SDSR_MANUAL_LST), 0 );

          if ( user_sys_ptr != NULL &&
               (memcmp (&(user_sys_ptr->sid_expected.prm.gwl.plmn.plmn_id),
                        &cmd->plmn,
                        sizeof(cmd->plmn)) != 0 ))
          {
            user_sys_ptr->sid_expected.prm.gwl.plmn.plmn_id = cmd->plmn;
            SD_MSG_HIGH_3 ("sid_expected updated with new plmn %d %d %d",
                          cmd->plmn.identity[0], cmd->plmn.identity[1], cmd->plmn.identity[2]);

          }

          if( NULL != cmd->csg_info_ptr )
          {
            SD_MSG_HIGH_1("sd_ss_ind_gw_opr_srv_info_ss | csg_id = %u", cmd->csg_info_ptr->csg_id);
          }
        }

        sd_ss_idle_sys_update( ss,
                               sdss_sr_list_map2( ss, SDSR_SRV_SYS),
                               sdss_sr_list_map2( ss, SDSR_SRV_GEO_LST),
                               sdss_sr_list_map2( ss, SDSR_SRV_PREF_LST) );

        #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
        /* Update the data suspend state.
        */
        sd_si_update_data_suspend_state( ss, cmd->data_suspend, cmd->service_status );
        #endif


        /* Inform the registered client (i.e., CM) that it is OK to originate as
        ** necessary. If UE is in camp_only mode, no calls can be originated.
        */
        if( sdss_get_camp_mode() != SD_SS_PREF_CAMP_MODE_ON )
        {
        sd_si_ok_to_orig( ss, sd_si_ptr(ss)->data_suspend );
        }
      }

  } /* if ( act == SD_SS_ACT_CONTINUE ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update sdss's srv_status with the non adjusted value so that CM and SD
  ** remain in sync.
  */
  if( cmd->service_status != service_status_rcvd )
  {
    sdss_update_srv_status( ss, service_status_rcvd, cmd->srv_domain );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  /* Update the data suspend state.
  */
  sd_si_update_data_suspend_state( ss, cmd->data_suspend, cmd->service_status );
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);
  modem_mem_free(mode_info_ptr,MODEM_MEM_CLIENT_MMODE);

  return act;

} /* sd_ss_ind_gw_opr_srv_info2() */


/*===========================================================================

FUNCTION sdcmd_gw_opr_acc_done_ss

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_gw_opr_acc_done_ss(
        sd_cmd_t *cmd
)
{

  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT_ENUM_IS_INRANGE(cmd->ss, SD_SS_MAX);

  /* Handle GWL call release only when SD's state is OPR and mode is 3GPP */
  if(sdss_get_ss_state(cmd->ss) != SDSS_STATE_OPR ||
     (sd_si_ptr(cmd->ss)->si_info.mode == SYS_SYS_MODE_CDMA ||
      sd_si_ptr(cmd->ss)->si_info.mode == SYS_SYS_MODE_HDR)
    )
  {
    SD_MSG_HIGH_2("Wrong state %d or mode %d to handle GWL opr acc done",
      sdss_get_ss_state(cmd->ss),sd_si_ptr(cmd->ss)->si_info.mode);
    return act;
  }

  switch( cmd->ss )
  {
    case SD_SS_MAIN:
    case SD_SS_HYBR_1:

      /* Process the SS-Event that is associated with this SS-Indication.
      */
      act = sd_ss_event_proc( cmd->ss, SDSS_EVT_GWL_OPR_ACC_DONE, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_2:
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_HYBR_2_GW_OPR_ACC_DONE, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_3:
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_HYBR_3_GW_OPR_ACC_DONE, cmd->rtrn_act_ptr );
      #endif
      break;

    case SD_SS_MAX:
    default:
      SD_ERR_1(" wrong ss %d",cmd->ss);
      act = SD_SS_ACT_CONTINUE;
      break;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
} /* sd_ss_ind_gw_opr_acc_done() */


/*===========================================================================

FUNCTION sdcmd_gw_ps_detach

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type    sdcmd_gw_ps_detach (
        sd_cmd_t  *cmd
)
{
  sd_ss_e_type        ss = cmd->ss;
  sd_si_info_s_type  *si_info_ptr = &sd_si_ptr(ss)->si_info;

  sys_srv_domain_e_type  new_domain;
  sys_srv_status_e_type  new_srv_status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  sd_ss_act_s_type    ss_act;

  new_domain = si_info_ptr->srv_domain;
  new_srv_status = si_info_ptr->srv_status;

  if( (si_info_ptr->mode != SYS_SYS_MODE_CDMA ) && (si_info_ptr->mode != SYS_SYS_MODE_HDR ))
  {
  switch(si_info_ptr->srv_domain)
  {
    case SYS_SRV_DOMAIN_CS_PS:
         new_domain = SYS_SRV_DOMAIN_CS_ONLY;
         break;

    case SYS_SRV_DOMAIN_PS_ONLY:
         new_domain = SYS_SRV_DOMAIN_NO_SRV;
		 new_srv_status = SYS_SRV_STATUS_NO_SRV;
         break;

    default:
         SD_MSG_HIGH_1("PS detach ind but no PS in srv domain: %d", si_info_ptr->srv_domain);
         break;
  }
  }
  #if defined (FEATURE_SD_LTE)
  sd_si_update_nas_addl_info( SD_SS_MAIN, SYS_LTE_CS_CAPABILITY_NONE, SYS_EXTEND_SRV_INFO_NONE);
  sdss_reset_nas_addl_info(SD_SS_MAIN);
  #endif

  if(new_domain != si_info_ptr->srv_domain)
  {
    /* Inform SDSS of new srv_domain */
    sdss_update_srv_status( ss, new_srv_status, new_domain );

    si_info_ptr->srv_domain = new_domain;
	si_info_ptr->srv_status = new_srv_status;

    /* Inform CM of new domain */
    sd_invoke_info_callback(ss);
  }

  /* Reset the last SD action since new preference will follow and SD must initiate action even if it is same as current action of ACQ */
  (void)sd_ss_act_get_true_curr_act_ss(ss,&ss_act);
  ss_act.act = SD_SS_ACT_CONTINUE;
  sd_ss_act_set_true_curr_act_ss(ss,&ss_act);

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmd->rtrn_act_ptr != NULL)
  {
    cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
  }

  return SD_SS_ACT_CONTINUE;

}


/*===========================================================================

FUNCTION sdcmd_gw_acq_plmn_success_ss_2

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_gw_acq_plmn_success_ss_2 (
      sd_ss_ind_gw_srv_info_cmd *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;
  sd_band_e_type      band;
  sd_ss_e_type        ss = cmd->ss;
  sd_ss_band_pref_e_type band_srched;
  sys_lte_band_mask_e_type band_srched_lte;
  boolean             is_band_srched = FALSE;
  sd_si_mode_info_u_type mode_info;
  sys_srv_status_e_type  service_status_rcvd = cmd->service_status;
  boolean                is_band_mismatch = FALSE;
  sys_sim_state_e_type  prev_sim_state;

  /* Point at acquired system.
  */
  sdsr_ref_type       acq_sys_ref   = sdsr_list_get_ref( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), 0 );
  sdsr_s_type         *acq_sys_ptr  = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), 0 );
  sdsr_s_type         *cand_sys_ptr = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_CAND_SYS), 0 );

  boolean             is_srv_on = sd_si_is_srv_on( ss );
  sd_ss_act_s_type    *ss_act_ptr = NULL;
  #if defined ( FEATURE_WRLF_SYSTEM_SEL) || defined(FEATURE_MMODE_DUAL_SIM)
  sd_ss_act_s_type    *ss_true_curr_act_ptr = NULL;
  #endif

  sd_ss_s_type* ss_ptr=sd_ss_ptr(ss);
  sys_rat_pri_list_info_s_type    ss_rat_pri_list_info;

  sdlog_bsr_end_info_s_type   end_info;
  boolean            send_bsr_success = FALSE;
  boolean            is_lmtd_to_no_srv = FALSE;
  sdsr_ref_type       ral_sys_ref;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( cmd->sim_state, SYS_SIM_STATE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->plmn_srv_cap, SYS_SRV_CAPABILITY_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->service_status, SYS_SRV_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->roaming_ind, SYS_ROAM_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->srv_domain, SYS_SRV_DOMAIN_MAX );
  SD_ASSERT( cmd->mode == SD_MODE_GSM || cmd->mode == SD_MODE_WCDMA \
      || cmd->mode == SD_MODE_LTE || cmd->mode == SD_MODE_TDS);
  SD_ASSERT_ENUM_IS_INRANGE( cmd->cs_service_status, SYS_SRV_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( cmd->reg_domain, SYS_REG_DOMAIN_MAX);


  #if defined(FEATURE_LTE)
  SD_ASSERT( cmd->lte_cs_capability < SYS_LTE_CS_CAPABILITY_MAX );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_4("SRV_3GPP: GWL acq success,mode=%d,ss=%d, status=%d, srv_domain=%d",
              cmd->mode,
              ss,
              cmd->service_status,
              cmd->srv_domain
              );
  SD_MSG_HIGH_6("srv info: is_plmn_forbidden=%d, roaming_ind=%d, data_suspend=%d, extend_srv_info=%d, acq_sys_ref=%d, sim_state=%d",
             cmd->is_plmn_forbidden,
             cmd->roaming_ind,
             cmd->data_suspend,cmd->extend_srv_info,acq_sys_ref,cmd->sim_state);
  

  if(NULL != cmd->csg_info_ptr && cmd->csg_info_ptr->csg_id != SYS_CSG_ID_INVALID)
  {
    SD_MSG_HIGH_1("CSG INFO: csg_id = %u", cmd->csg_info_ptr->csg_id);
  }

  #if defined(FEATURE_LTE)
  SD_MSG_HIGH_4("lte_cs_capability=%d, reg_domain=%d ims_avail=%d,emerg_avail=%d",cmd->lte_cs_capability, cmd->reg_domain, cmd->lte_ims_voice_avail, cmd->lte_ims_emerg_avail);
  
  #endif

  /* Send LTE BSR end success */
   if(ss == SD_SS_HYBR_2 && 
      cmd->mode == SD_MODE_LTE &&
      sdss_is_resel_state(ss, SDSS_RESEL_STT_SRCH))
   {
     send_bsr_success = TRUE;
   }
   else
   {
     send_bsr_success = FALSE;
   }

  
  /* On acquiring GWL system on MAIN stack, reset CDMA BSR flag to FALSE
  */
  if(ss == SD_SS_MAIN)
  {   
    sdcmd_cdma_bsr_in_progress_cb(FALSE);
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_EOOS_UMTS
  /*  Reset the EOoS field since we are in service */
  sdss_reset_eoos_timer(ss);
  #endif

  (void)memset(&mode_info, 0, sizeof(mode_info));

  SD_ASSERT( acq_sys_ref != SDSR_REF_NULL );
  SD_ASSERT( acq_sys_ptr != NULL );

  if(acq_sys_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  if(cmd->mode == SD_MODE_LTE)
  {
    band = sdss_map_band_pref_to_band_lte(cmd->band_pref.lte_band);
    band_srched_lte = sdsr_map_sys_rec_lte_band_to_lte_band_pref(acq_sys_ptr->sys.lte_band);
    is_band_srched = SYS_LTE_BAND_MASK_CHK_IF_OVERLAP(&cmd->band_pref.lte_band,&band_srched_lte);

  }
  else if (cmd->mode == SD_MODE_TDS)
  {
    band = sdss_map_band_pref_to_band_tds(cmd->band_pref.chgwt_band );
    band_srched = acq_sys_ptr->sys.tds_band;
    is_band_srched = SD_BAND_CONTAIN(cmd->band_pref.chgwt_band ,acq_sys_ptr->sys.tds_band );
  }
  else
  {
    band = sdss_map_band_pref_to_band(cmd->band_pref.chgwt_band );
    band_srched = acq_sys_ptr->sys.band;
    is_band_srched = SD_BAND_CONTAIN(cmd->band_pref.chgwt_band ,acq_sys_ptr->sys.band );
  }

  /* reset   SDSS_FLAG_OVERRIDE_E911_ADD_INFO  in case of acq plmn success
  for acq plmn fail, script ssscr_called_gwl_no_srv_plmn_found_lmtd_sys_scan will take care of it*/
  sdss_set_flag(ss,SDSS_FLAG_OVERRIDE_E911_ADD_INFO, FALSE);

  /* Check if UE has moved to a band that is not present in current ACQ_SYS
  */
  if(!is_band_srched)
  {
    SD_MSG_HIGH_1("Band mismatch: acq_sys mode= %d ",acq_sys_ptr->sys.mode);
    if( ss == SD_SS_MAIN && sdss_is_mmss_operation(ss) )
    {
      is_band_mismatch = TRUE;
    }
    else
    {
    SD_ASSERT(FALSE);
  }
  }

  SD_ASSERT( ss_ptr != NULL );

  if( cmd->service_status == SYS_SRV_STATUS_SRV )
  {
    sdss_set_pref_chg_in_ltd_reg(ss,FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* We should reset the request ID from last  sd action. 
   */
  ss_ptr->act.prm.acq_gwl.sys_sel_pref_req_id = 0;
  ss_ptr->true_curr_act.prm.acq_gwl.sys_sel_pref_req_id = 0;
 
  /* Once FULL servcie acqusiition is completed, proceed with 
  ** pure limited scans.
  */
  if (sdss_get_is_full_srv_req_during_emerg(ss))
  {
    SD_MSG_HIGH_0("sdcmd_gw_acq_plmn_success_ss_2 - reset full srv req flag due to full srv");
    sdss_check_reset_emerg_rat_pri_list_info(ss);
    sdss_update_is_full_srv_req_during_emerg(ss,FALSE);
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Save the rplmn in global */
  if(NULL != cmd->registered_plmn)
  {
    *(&(ss_ptr->rplmn)) = *(cmd->registered_plmn);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_AVOID_DUP_BAND_SCAN
  
  /* Update is_plmn_found to true for the acquisition
  */
  sdss_update_3gpp_acq_info(ss, SDSS_UPDATE_3GPP_ACQ_INFO_ALL, TRUE,
                              cmd->service_status);
  
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sdss_is_lte_connected_mode (ss))
  {
    /* Since UE has camped, it is no longer in LTE connected state
     */
    sdss_reset_lte_connected_mode (ss);
  }

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  /* Reset the Stack sync up flag.*/
  sdss_set_flag(ss, SDSS_FLAG_SYNC_UP_IN_PROGRESS, FALSE);
  #endif

  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  if (sdss_is_wcdma_connected_mode (ss) && (cmd->service_status == SYS_SRV_STATUS_SRV) )
  {
    /* Since UE aquired full service, it is no longer in WCDMA connected state.
    */
    SD_MSG_HIGH_0("WRLF: Ending OOSC procedure as FULL SRV is aquired");
    sdss_reset_wcdma_connected_mode (ss);
  }
  
  /* Set CSG_ID to INVALID if CSG is different or Cell is Macro */
  sdss_clear_csg_info_on_ss(ss, cmd->csg_info_ptr);

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ss == SD_SS_HYBR_2 || ss == SD_SS_MAIN)
  {
    /* ALT_CDMA_BSR feature is on and LTE is full srv,
    ** do LTEAvailFile operation */
    if(sdss_is_c2k_alt_algr_operation() &&
      cmd->mode == SD_MODE_LTE && cmd->service_status == SYS_SRV_STATUS_SRV)
    {
      sdss_lte_avail_sys_s_type sys;
      sd_si_s_type              *si_main_ptr = sd_si_ptr( SD_SS_MAIN );

      if(si_main_ptr &&
         si_main_ptr->si_info.mode == SYS_SYS_MODE_CDMA &&
         sd_si_is_full_srv_on( SD_SS_MAIN ))
      {
        /* Only when we have 1x service, we need to update the lte available file
        ** upon ACQ_GWL SUCCESS
        */
        sys.sid = si_main_ptr->si_info.sid.is95.sid;
        sys.nid = si_main_ptr->si_info.sid.is95.nid;
        sys.bid = si_main_ptr->si_info.mode_info.cdma.base_id;

        if(sys.bid != SDSS_BID_VAL_NONE)
        {
          sdss_lte_avail_file_update(SDSS_EVT_GWL_ACQ_PLMN_SUCCESS, &sys, ss);
        }
      }
      else
      {
        SD_MSG_MED_0("Invalid si_ptr");
      }
    }
  }

   /* If SD is in operation state, map to sd_ss_ind_gw_opr_srv_info
   */
  if( sd_get_ss_state(ss) == SDSS_STATE_OPR )
  {
    sd_ss_ind_gw_srv_info_cmd infocmd;

    infocmd.plmn = cmd->plmn;
    infocmd.lac_tac_rac = cmd->lac_tac_rac;
    infocmd.band_pref = cmd->band_pref;
    infocmd.chan = cmd->chan;
    infocmd.plmn_srv_cap = cmd->plmn_srv_cap;
    infocmd.is_plmn_forbidden = cmd->is_plmn_forbidden;
    infocmd.sim_state = cmd->sim_state;
    infocmd.service_status = cmd->service_status;
    infocmd.roaming_ind = cmd->roaming_ind;
    infocmd.srv_domain = cmd->srv_domain;
    infocmd.mode = cmd->mode;
    infocmd.data_suspend = cmd->data_suspend;
    infocmd.cs_service_status = cmd->cs_service_status;
    infocmd.ss = ss;
    infocmd.lte_cs_capability = cmd->lte_cs_capability;
    infocmd.sib8_available = cmd->sib8_available;
    infocmd.lte_ims_voice_avail = cmd->lte_ims_voice_avail;
    infocmd.lte_ims_emerg_avail = cmd->lte_ims_emerg_avail;
    infocmd.extend_srv_info = cmd->extend_srv_info;
    infocmd.reg_domain = cmd->reg_domain;
    infocmd.rtrn_act_ptr = cmd->rtrn_act_ptr;
    infocmd.csg_info_ptr = cmd->csg_info_ptr;
    infocmd.registered_plmn = cmd->registered_plmn;

    return sdcmd_gw_opr_srv_info_ss_1(&infocmd);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  
  /* Get the sim state before updating it*/
  prev_sim_state = sdss_get_sim_state(ss);

  /* update SIM state
  */
  if(sdss_is_1x_sxlte() && (ss!= SD_SS_HYBR_3))
  {
    sd_si_update_sim_state( cmd->sim_state, SD_SS_MAIN );
    sd_si_update_sim_state( cmd->sim_state, SD_SS_HYBR_2 );
  }
  else
  {
    sd_si_update_sim_state( cmd->sim_state, ss );
  }

  #endif //gwl

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If UE in CAMPED_ONLY mode, check the reg_domain and adjust the
  ** service status for internal SD evaluation. The service indication will
  ** reflect the NON-ADJUSTED value for service status.
  */
  cmd->service_status = sd_misc_adjust_service_status_in_camp_mode( service_status_rcvd, cmd->reg_domain );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //if we are here, update the req_id in si_info so that next si update will carry it.
  sd_si_ptr(ss)->si_info.sys_sel_pref_req_id = cmd->req_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update sdss's srv_status too.
  */
  sdss_update_srv_status( ss, cmd->service_status, cmd->srv_domain );

  /*Reset acq_status, as we found PLMN*/
  sdss_update_acq_status(ss,SYS_ACQ_STATUS_SUCCESS);

  if ((cmd->extend_srv_info == SYS_EXTEND_SRV_INFO_T3402_STARTED) &&
      (cmd->service_status != SYS_SRV_STATUS_LIMITED))
  {
    cmd->extend_srv_info = SYS_EXTEND_SRV_INFO_NONE;
  }
  

   sdss_update_nas_addl_info(ss, cmd->lte_cs_capability, cmd->extend_srv_info);

  /* Clear the power scan list
  */
  sdsr_list_clr( sdss_sr_list_map2(ss,SDSR_PWR_SCAN_LST) );

  
   /* If the previous sim state is CS/PS invalid and the new sim state is SIM_STATE_AVAILABLE,
   ** issue new service request for CS_PS registration.
   ** If LTE RPM adjust the SD SIM state to PS_INVALID, we do not need to issue new srv request.
    */
   if ( (prev_sim_state == SYS_SIM_STATE_PS_INVALID ||
      prev_sim_state == SYS_SIM_STATE_CS_INVALID) &&
         (cmd->sim_state == SYS_SIM_STATE_AVAILABLE) &&
         (cmrpm_lte_update_sim_state() != SYS_SIM_STATE_PS_INVALID) )
    {
	 
      SD_MSG_HIGH_0("Sim state change CS/PS INVALID to AVAILABLE, trigger sim state update event");
	  
      return sd_ss_event_proc( cmd->ss,
                              SDSS_EVT_MISC_GW_SIM_STATE_UPDATE,
                              cmd->rtrn_act_ptr );
	
   }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check conditions to map to acq fail
  */
  {
    /* This local variable is used to track whether UE is currently in
    ** emergency call origination phase
    */
    boolean  is_emerg_call_orig = FALSE;

    /* This local variable is used to track if this acquisition success needs
    ** to be mapped to an acq_fail
    */
    boolean  is_map_to_acq_fail = FALSE;

    if( sdss_get_orig_mode(ss)  == SD_SS_ORIG_MODE_EMERG_ORIG &&
        sdss_is_pref_reason( ss, SDSSSCR_PREF_REAS_VAL_CALL_ORIG))
    {
      is_emerg_call_orig = TRUE;
    }
	
	
    /* If SD acquired service on LTE on a cell that does not support IMS
    ** emergency treat this as no service. emergency barring checking is removed since
    ** LTE rrc will take care not sending barred cell when emc_pending is set.
    */
    if( cmd->mode                == SD_MODE_LTE &&
        is_emerg_call_orig       == TRUE &&
        ( cmd->lte_ims_emerg_avail   != SYS_IMS_LTE_EMERG_SUPP_AVAILABLE )
      )
    {
      SD_MSG_HIGH_0("LTE- no emerg srv - map to acq_fail");
      is_map_to_acq_fail = TRUE;
    }

    /* Map Limited service to Acq fail for
       if emerg orig mode is set to Volte Only is set and UE is in emergency call origination*/
    if(sdss_is_emer_orig_in_volte_only(ss) &&\
       cmd->service_status == SYS_SRV_STATUS_LIMITED &&
      ((cmd->sim_state != SYS_SIM_STATE_PS_INVALID) &&
       (cmd->sim_state != SYS_SIM_STATE_CS_PS_INVALID))) 
    {
      is_map_to_acq_fail = TRUE;
      sdss_update_rat_pri_list_with_idle_cap(ss);
	  //Set the roaming flag to false
	  sdss_set_e911_in_roam(ss, FALSE);
      SD_MSG_HIGH_0("MAp to acq fail as call should go on LTE FULL SRV ONLY");
    }

	 /* Map Full srv to acq_fail when LTE FULL/LIMITED service  is received during Roaming
	   if emerg call mode is set to NORMAL */
    if(sdss_is_emerg_in_roam_for_volte_specialization(ss) &&\
	   cmd->mode                == SD_MODE_LTE &&
       ((cmd->sim_state != SYS_SIM_STATE_PS_INVALID) &&
       (cmd->sim_state != SYS_SIM_STATE_CS_PS_INVALID)))
    {
      is_map_to_acq_fail = TRUE;
	  sdss_set_e911_in_roam(ss,TRUE);
      SD_MSG_HIGH_0("Map to acq fail as e911 call should go on CS only when in Roaming");
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If we acquired limited service and we did not asked for limited service
    ** or in manual mode we got the limited service on a forbidden PLMN which
    ** is not user selected PLMN, process this as acquisition failure.
    */
    ss_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                        (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
    if (ss_act_ptr != NULL)
    {
      memset(ss_act_ptr, 0, sizeof(sd_ss_act_s_type));
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);
    if( !is_emerg_call_orig )
    {
      if(((!cmd->is_plmn_forbidden) || (!sys_plmn_match ( acq_sys_ptr->sid.plmn.plmn_id, cmd->plmn ) &&
        ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_MANUAL))
                         &&
         cmd->service_status == SYS_SRV_STATUS_LIMITED
                           &&
         ss_act_ptr->prm.acq_gwl.net_sel_type !=
         SYS_NETWORK_SELECTION_MODE_LIMITED
                           &&
         ss_act_ptr->prm.acq_gwl.net_sel_type !=
         SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION
                           &&
         ss_act_ptr->prm.acq_gwl.net_sel_type !=
         SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY
                           &&
         ss_act_ptr->prm.acq_gwl.net_sel_type !=
         SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY
                           &&
         ss_act_ptr->prm.acq_gwl.net_sel_type !=
         SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION )
      {
        is_map_to_acq_fail = TRUE;
      }
    }//if( !is_emerg_call_orig )
    if (is_map_to_acq_fail)
    {
      sd_ss_ind_gw_acq_plmn_failed_cmd  failcmd;

      failcmd.ss = ss;
      failcmd.rtrn_act_ptr = cmd->rtrn_act_ptr,
      failcmd.sim_state = cmd->sim_state;
      failcmd.data_suspend = cmd->data_suspend;
      failcmd.scan_status = SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND;
      failcmd.lte_cs_capability = cmd->lte_cs_capability;
      failcmd.extend_srv_info = cmd->extend_srv_info;
      failcmd.req_id = cmd->req_id;
      failcmd.csg_info_ptr = NULL;
      failcmd.registered_plmn = NULL;
	  failcmd.acq_status = SYS_ACQ_STATUS_SUCCESS;
      modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);
      return sdcmd_gw_acq_plmn_failed_ss_2(&failcmd);
    }
  }
  /* If NAS reports limited service for non RPLMN in service confirm/indication while in MANUAL_CSG mode 
  ** then mark it as acq fail so that net sel mode will be revert to previous network selection mode same 
  ** acq fail case.
  */
  if ( cmd->csg_info_ptr != NULL && 
       cmd->service_status == SYS_SRV_STATUS_LIMITED && 
       cmd->csg_info_ptr->csg_id != SYS_CSG_ID_INVALID && 
       sdss_get_non_rplmn_csg_selection_type(ss) == TRUE
     ) 
    {
      sd_ss_ind_gw_acq_plmn_failed_cmd  failcmd;

      failcmd.ss = ss;
      failcmd.rtrn_act_ptr = cmd->rtrn_act_ptr,
      failcmd.sim_state = cmd->sim_state;
      failcmd.data_suspend = cmd->data_suspend;
      failcmd.scan_status = SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND;
      failcmd.lte_cs_capability = cmd->lte_cs_capability;
      failcmd.extend_srv_info = cmd->extend_srv_info;
      failcmd.req_id = cmd->req_id;
      failcmd.csg_info_ptr = cmd->csg_info_ptr;
      failcmd.registered_plmn = cmd->registered_plmn;
	  failcmd.acq_status = SYS_ACQ_STATUS_SUCCESS;
      modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);
      return sdcmd_gw_acq_plmn_failed_ss_2(&failcmd);
    }


  /* If 1x service received in between service request (NAS found less preferred LTE system, SD will give 
  ** action as OK to camp on less preferred as there is no 1x system) and service cnf, which may lead UE 
  ** to stay on less preferred LTE system. So when SD gets LTE service cnf, always compare it with 1x sys 
  ** on main stack. If it is less preferred sys then map it to plmn_acq_fail.
  */

  if(cmd->mode == SD_MODE_LTE && ss == SD_SS_HYBR_2 &&
     cmd->service_status == SYS_SRV_STATUS_SRV &&
     sd_si_ptr(SD_SS_MAIN)->si_info.mode == SD_MODE_CDMA &&
     sd_si_ptr(SD_SS_MAIN)->si_info.srv_status == SYS_SRV_STATUS_SRV
     )

  {
      sd_ss_ind_gw_acq_plmn_failed_cmd  failcmd;
      sd_is_plmn_rat_allowed_cmd  is_plmn_rat_cmd;
      boolean is_plmn_rat_allowed = FALSE;

      is_plmn_rat_cmd.plmn_id = cmd->plmn;
      is_plmn_rat_cmd.mode = SD_MODE_LTE;
      is_plmn_rat_cmd.ss = SD_SS_HYBR_2;

      /* If csg search in auto mode enabled and BSR is not allowed when camped on
           ** CSG cell do not check for more preferred systems. Allow to camp on less preferred
           ** cell only when user has selected the csg cell
           */
      if ( (NULL != cmd->csg_info_ptr) &&
        (ss_ptr->true_curr_act.prm.acq_gwl.csg_id == cmd->csg_info_ptr->csg_id) &&
        (ss_ptr->true_curr_act.prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN || 
        ss_ptr->true_curr_act.prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_CSG_SELECTION_APP) &&
        sdefs_csg_auto_mode_bsr_disable()== TRUE )
      {
        SD_MSG_HIGH_0("User CSG cell selection in auto mode, bsr is disabled");
        is_plmn_rat_allowed = TRUE;
      }
      else if( cmd->service_status == SYS_SRV_STATUS_LIMITED &&
               (ss_ptr->true_curr_act.prm.acq_gwl.net_sel_type ==
                SYS_NETWORK_SELECTION_MODE_LIMITED
                           ||
                ss_ptr->true_curr_act.prm.acq_gwl.net_sel_type ==
                SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION
                           ||
                ss_ptr->true_curr_act.prm.acq_gwl.net_sel_type ==
                SYS_NETWORK_SELECTION_MODE_AUTOMATIC_CAMP_ONLY
                           ||
                ss_ptr->true_curr_act.prm.acq_gwl.net_sel_type ==
                SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY
                           ||
                ss_ptr->true_curr_act.prm.acq_gwl.net_sel_type ==
                SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION ))
      {
        SD_MSG_HIGH_0("Requested for Limited service, so plmn_allow set to TRUE");
        is_plmn_rat_allowed = TRUE;        
      }
      else
      {
       /* Check for more preferred systems available in GEO. If rat plmn is not allowed
            ** look for preferred systems else continue with the given plmn
            */
        is_plmn_rat_allowed = sdcmd_is_plmn_rat_allowed(&is_plmn_rat_cmd);
      }
      if(!is_plmn_rat_allowed)
      {
        SD_MSG_HIGH_0("plmn is not allowed - Mapped to FAIL");
        /* Simulate acq fail on GWLT */
        failcmd.ss = ss;
        failcmd.rtrn_act_ptr = cmd->rtrn_act_ptr,
        failcmd.sim_state = cmd->sim_state;
        failcmd.data_suspend = cmd->data_suspend;
        failcmd.scan_status = SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND;
        failcmd.lte_cs_capability = cmd->lte_cs_capability;
        failcmd.extend_srv_info = cmd->extend_srv_info;
        failcmd.req_id = cmd->req_id;
        failcmd.csg_info_ptr = cmd->csg_info_ptr;
        failcmd.registered_plmn = cmd->registered_plmn;
		failcmd.acq_status = SYS_ACQ_STATUS_SUCCESS;
        modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);

        return sdcmd_gw_acq_plmn_failed_ss_2(&failcmd);
      }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_MMODE_SC_SVLTE
  {
    boolean   is_limited_srv_allowed = TRUE;

  /* for SVLTE-2, if we're in GW-limited, we shall disable GW-Limited camping
  ** so that reselection can happens from GW-Limited to HDR. This will be
  ** removed once we have completed GW-limited --> HDR BSR algorithm.
  ** Only in case of emergency and TOT scan GW-limited is allowed.
  */

    if(sdss_is_1x_sxlte())
    {
      /* For SC-SVLTE, if we have 1X full service we will not allow HYBR2 to
      ** to camp on LTE limited service
      */

    /* 
    ** if limited service request is received due to 
    ** acq type reg_best_pref, then skip the logic of is_limited_srv_allowed
    ** this is because in case of reg_best_pref, anyways limited service will be obtained
    ** but in case if acq_stt is reacq_gwl then SD is trying to acquire full service on 
    ** the system for which NAS has send limited servic eearlier. So in second case 
    ** if their is limited service reported in state reacq_gwl, then do not skip this 
    ** limited service not allowed logic.
    */
      if( ss == SD_SS_HYBR_2 &&
          sd_si_ptr(SD_SS_MAIN)->si_info.mode == SD_MODE_CDMA &&
          sd_si_ptr(SD_SS_MAIN)->si_info.srv_status == SYS_SRV_STATUS_SRV
          #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
          && sdss_allow_ltd_acq_fail_mapping(ss)
          #endif          
      )
      {
        is_limited_srv_allowed = FALSE;
      }
    }
    if( cmd->service_status == SYS_SRV_STATUS_LIMITED &&
        !is_limited_srv_allowed )
    {
      sd_ss_ind_gw_acq_plmn_failed_cmd  failcmd;

      failcmd.ss = ss;
      failcmd.rtrn_act_ptr = cmd->rtrn_act_ptr,
      failcmd.sim_state = cmd->sim_state;
      failcmd.data_suspend = cmd->data_suspend;
      failcmd.scan_status = SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND;
      failcmd.lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
      failcmd.req_id = cmd->req_id;
      failcmd.extend_srv_info = cmd->extend_srv_info;
      failcmd.csg_info_ptr = cmd->csg_info_ptr;
      failcmd.registered_plmn = cmd->registered_plmn;
	  failcmd.acq_status = SYS_ACQ_STATUS_SUCCESS;

      SD_MSG_HIGH_0("SVLTE: Acquired 3GPP ltd service - Mapped to FAIL");
      /* mapped to service failed */
      modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);
      return sdcmd_gw_acq_plmn_failed_ss_2(&failcmd);
    }
  }
  #endif // FEATURE_MMODE_SC_SVLTE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* In global mode if NAS reports service on a RPLMN for a csg service request treat that as acq failure
  */

  (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);
  if ( (cmd->csg_info_ptr != NULL) &&
       (cmd->csg_info_ptr->csg_id == SYS_CSG_ID_INVALID) &&
       (ss_act_ptr->prm.acq_gwl.srv_req_type == SD_SS_SRV_REQ_TYPE_CSG_SELECTION_APP) &&
       !sdss_is_target_cap_3gpp_only(ss)
     )
    {
      sd_ss_ind_gw_acq_plmn_failed_cmd  failcmd;
      SD_MSG_HIGH_0("Global mode CSG: Expecting csg cell for csg selection app -- Mapped to fail");
      failcmd.ss = ss;
      failcmd.rtrn_act_ptr = cmd->rtrn_act_ptr,
      failcmd.sim_state = cmd->sim_state;
      failcmd.data_suspend = cmd->data_suspend;
      failcmd.scan_status = SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND;
      failcmd.lte_cs_capability = cmd->lte_cs_capability;
      failcmd.extend_srv_info = cmd->extend_srv_info;
      failcmd.req_id = cmd->req_id;
      failcmd.csg_info_ptr = cmd->csg_info_ptr;
      failcmd.registered_plmn = cmd->registered_plmn;
	  failcmd.acq_status = SYS_ACQ_STATUS_SUCCESS;
      modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);
      return sdcmd_gw_acq_plmn_failed_ss_2(&failcmd);
    }
  #ifdef FEATURE_EOOS_UMTS
  /* Reset the use timer
  */
  sdss_reset_gw_acq_para( ss );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Limited service aquisition in connected mode is only for showing to UI
  ** So SD core has to treat the limited service as NO service itself.*/

  {
   sd_ss_e_type other_ss = SD_SS_MAIN;
   
  #ifdef FEATURE_WRLF_SYSTEM_SEL
  if(cmd->service_status == SYS_SRV_STATUS_LIMITED &&
     sdss_is_wcdma_connected_mode(ss))
  {
    SD_MSG_HIGH_0("WRLF:Simulate Limited service as NO service");
    is_lmtd_to_no_srv = TRUE;
  }
  #endif

  /* Limited service mapped to no service if either is true
  ** 1. in SXLTE, main stack is 3GPP only and other sub in srv
  ** 2. not in SXLTE, other sub in srv
  */
  #ifdef FEATURE_MMODE_DUAL_SIM
    if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) &&
    	 cmd->service_status == SYS_SRV_STATUS_LIMITED &&
       !(sdss_is_sfmode_1x_sxlte(sdss_map_ss_to_as_id(cmd->ss)) &&
       cmd->ss == SD_SS_HYBR_2) &&
       !(sdss_is_sfmode_1x_sxlte(sdss_map_ss_to_as_id(cmd->ss)) && 
         !sdss_is_cap_3gpp_only(cmd->ss)) &&  
     (((other_ss = sdss_is_other_ss_in_srv(cmd->ss))!= SD_SS_MAX) &&
        (sdss_get_orig_mode(ss)  == SD_SS_ORIG_MODE_NONE ||
           sdss_get_orig_mode(ss)  == SD_SS_ORIG_MODE_MANUAL_IDLE)))
    {
      is_lmtd_to_no_srv = TRUE;
    }
  #endif

  #if defined FEATURE_WRLF_SYSTEM_SEL || defined FEATURE_MMODE_DUAL_SIM
  ss_true_curr_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (ss_true_curr_act_ptr != NULL)
  {
    memset(ss_true_curr_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  if(is_lmtd_to_no_srv)
  {

    sd_ss_ind_gw_acq_plmn_failed_cmd  failcmd; 
  
    failcmd.ss = ss;
      failcmd.rtrn_act_ptr      = cmd->rtrn_act_ptr;
    failcmd.sim_state = cmd->sim_state;
    failcmd.data_suspend = cmd->data_suspend;
    failcmd.scan_status = SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND;
    failcmd.lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
    failcmd.req_id = cmd->req_id;
    failcmd.extend_srv_info = SYS_EXTEND_SRV_INFO_NONE;
    failcmd.csg_info_ptr = NULL;
    failcmd.registered_plmn = NULL;
	failcmd.acq_status = SYS_ACQ_STATUS_SUCCESS;


    if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM))
    {
      #ifndef FEATURE_MMODE_SXLTE_G
      SD_MSG_HIGH_0("DSDS:Simulate Limited service as NO service");
      if(other_ss < SD_SS_MAX && other_ss >= SD_SS_MAIN)
      {
        SD_MSG_HIGH_3 ("other_ss1 is in srv other_ss %d, srv status %d, orig mode on ss %d",
        other_ss, sd_si_ptr(other_ss)->si_info.srv_status, sdss_get_orig_mode(ss));   
      }
      #endif
    }
    act = sdcmd_gw_acq_plmn_failed_ss_2(&failcmd);

    if (act == SD_SS_ACT_ACQ_GWL)
    {
      (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);
      (void)sd_ss_act_get_curr_act_ss(ss,ss_true_curr_act_ptr);

      /* Update the srev request type accordingly */
      ss_true_curr_act_ptr->prm.acq_gwl.srv_req_type = sdss_get_srv_req_type(ss);
      ss_act_ptr->prm.acq_gwl.srv_req_type = sdss_get_srv_req_type(ss);

      sd_ss_act_set_true_curr_act_ss(ss,ss_act_ptr);
    }

    SD_MSG_HIGH_0("Ltd_to_no_srv:Indicating limited service only to show on UI");

    if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM))
    {
      acq_sys_ptr->sid.plmn.plmn_id = cmd->plmn;
    }
    /* Update the system indicators per the acquired system.
    */
    sd_si_update( ss,
                  SD_SI_UPDATE_REG,
                   service_status_rcvd,
                  cmd->srv_domain,
                  acq_sys_ref,
                  FALSE,
                  &cmd->mode,
                  &band,
                  NULL,
                  &mode_info,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  cmd->cs_service_status,
                  cmd->lte_cs_capability,
                  cmd->sib8_available,
                  cmd->lte_ims_voice_avail,
                  cmd->lte_ims_emerg_avail? SYS_IMS_LTE_EMERG_SUPP_AVAILABLE : SYS_IMS_LTE_EMERG_SUPP_NOT_AVAILABLE,
                  cmd->req_id,
                  cmd->extend_srv_info,
                  cmd->reg_domain,
                  cmd->csg_info_ptr,
                  FALSE
                );
    modem_mem_free(ss_true_curr_act_ptr,MODEM_MEM_CLIENT_MMODE);
    modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);  
    return act;
  }
  #endif
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( is_band_mismatch == TRUE &&
      ss == SD_SS_MAIN &&
      sdss_is_mmss_operation(SD_SS_MAIN)  )
  {
    sd_sid_plmn_s_type   sid_exp;
    sdsr_ref_type        sr_ref           = SDSR_REF_NULL;

    /* Store the new system in the data base as automatic plmn selection
    ** record and copy it to the BACKUP_LST
    */
    memset(&sid_exp,0, sizeof(sid_exp));
    sid_exp.net_select_mode        = SD_NET_SELECT_MODE_AUTO;
    sid_exp.prm.gwl.plmn.lac       = PRL_WILDCARD_LAC;
    sys_plmn_undefine_plmn_id(&(sid_exp.prm.gwl.plmn.plmn_id));
    sid_exp.prm.gwl.num_rats       = 1;
    sid_exp.prm.gwl.gwl_pri_lst[0] = cmd->mode;

    sdsr_list_clr( SDSR_BACKUP_LST );
    sdsr_list_store_and_app3( SDSR_BACKUP_LST,
                              SDSR_MAX,
                              SDSR_MAX,
                              SD_MODE_GWL,
                              (cmd->mode == SD_MODE_LTE || cmd->mode == SD_MODE_TDS) ? 0 : cmd->band_pref.chgwt_band ,
                              (cmd->mode == SD_MODE_LTE ) ? sdsr_map_lte_band_pref_to_sys_rec_lte_band(cmd->band_pref.lte_band) :
                                                            sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                              (cmd->mode == SD_MODE_TDS ) ? cmd->band_pref.chgwt_band  : 0,
                              0,
                              &sid_exp,
                              SD_NO_CHANGE_SYS_IDX );
    acq_sys_ptr     = sdsr_list_get_ptr( SDSR_BACKUP_LST, 0 );

    if( acq_sys_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }

    /* Update with acquired system mode and PLMN for GW sr to perform correct mathcing.
    */
    acq_sys_ptr->sid.gw.mode       = cmd->mode;
    acq_sys_ptr->sid.plmn.plmn_id  = cmd->plmn;
    acq_sys_ptr->srv_cap           = cmd->plmn_srv_cap;

    /* Find the entry in the PRL that matches with the acquired system.
    ** If there is a match, the system will be stored in SDSR_GW_CAND_SYS
    ** within sdprl_get_info_base()
    */
    //sdsr_list_clr( SDSR_GW_CAND_SYS );
    (void)sdprl_get_info_base( SD_SS_MAIN,
                               SDSR_BACKUP_LST,
                               0,
                               SDSR_BACKUP_LST,
                               0,
                               SDSR_MAX,
                               SDSR_MAX,
                               SDSR_MAX,
                               SDPRL_EXTRACT_MATCH_ONLY,
                               NULL);

    /* update SRV sys per the GW_CAND_SYS if it is not NULL.
    */
    sr_ref = sdsr_list_get_ref( sdss_sr_list_map2( ss, SDSR_GW_CAND_SYS ), 0 );

    /* Update the last user system that contains the same bands as that in
    ** in the idle system.
    */
    SD_MSG_HIGH_1("MMSS:cand_sys %d",sr_ref);
    if(sr_ref != SDSR_REF_NULL )
    {
      sdsr_list_put_ref( sdss_sr_list_map2( ss, SDSR_ACQ_SYS), sr_ref );
      sdsr_list_update_post_gwl_sys( ss, sr_ref, TRUE, TRUE, TRUE );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update with acquired system mode and band for GW sr to perform correct matching.
  ** The band is used currently for LTE emergency call processing to decide whether
  ** a full service request needs to be issued.
  */
  if( (acq_sys_ptr->sys.mode == SD_MODE_GW) || (acq_sys_ptr->sys.mode == SD_MODE_GWL) )
  {
    acq_sys_ptr->sid.gw.mode = cmd->mode;
    /* Update acquired band to be used for comparision*/
    if( cmd->mode == SD_MODE_LTE  )
    {
    acq_sys_ptr->sid.gw.band.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(cmd->band_pref.lte_band);
    }
    else
    {
      acq_sys_ptr->sid.gw.band.chgwt_band = band ;
    }
  }

  /* If the acquired system's mode is not match that in sr, it will error
  ** fatal except for GW mode.
  */

  if( ((acq_sys_ptr->sys.mode != SD_MODE_GW) && (acq_sys_ptr->sys.mode != SD_MODE_GWL)) &&
      (acq_sys_ptr->sys.mode != cmd->mode ) )
  {
    sys_err_fatal_invalid_value_exception((int)acq_sys_ptr->sys.mode);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the PRL designation, GEO list and more preferred list of the
  ** acquired GSM/WCDMA/LTE system.
  */
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)

  if(sdss_is_mmss_operation(ss))
  	 sdprl_free_gwl_more_pref_list(SYS_SYS_MODE_CDMA);
  
  (void) sdprl_gw_get_info(
                    ss,                  /* MAIN system selection */
                    sdss_sr_list_map2( ss, SDSR_ACQ_SYS),        /* Get PRL info for this list */
                    0,                   /* and list position */
                    sdss_sr_list_map2( ss, SDSR_ACQ_GEO_LST),    /* Put GEO systems into this list */
                    sdss_sr_list_map2( ss, SDSR_ACQ_PREF_LST),   /* Put PREF systems into this list */
                    SDSR_MAX,            /* Do not extract collocated sys */
                    SDPRL_EXTRACT_ALL,   /* Get info regardless of PRL match */
                    cmd->plmn,           /* PLMN id of acqed sys */
                    cmd->lac_tac_rac.lac,/* LAC of acqed sys */
                    cmd->plmn_srv_cap,   /* Service capability of acqed sys */
                    cmd->service_status, /* Service status */
                    cmd->is_plmn_forbidden,   /* Is acqed system forbidden? */
                    cmd->roaming_ind);   /* Roaming indication for acqed sys */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* update manual bst if needed*/
  if(cmd->is_plmn_forbidden == FALSE)
  {
    sdss_man_srch_bst_node_s_type success_info;
    success_info.band_pref = cmd->band_pref;
    success_info.mode = cmd->mode;
    success_info.plmn = cmd->plmn;
    sdss_check_update_manual_search_bst(&success_info);
  }

   /* IF RAL band based granularity is required then add new record in sdsr table 
  and add that reference in acqed_lst, rather than adding all the bands from acq sys to
  RAL */  
  
  /* Insert the acquired system at the beginning of the SDSR_ACQED_LST and
  ** remove any duplicates.
  */
  if(sdss_is_ral_stored_in_efs(ss))
  {
    sd_sid_plmn_s_type   sid_exp;
    memset(&sid_exp,0, sizeof(sid_exp));

    /* fill sid_expected parameter */
    sid_exp.net_select_mode = SD_NET_SELECT_MODE_AUTO;
    sys_plmn_undefine_plmn_id(&(sid_exp.prm.gwl.plmn.plmn_id));
    sid_exp.prm.gwl.plmn.lac = PRL_WILDCARD_LAC; 
    sid_exp.prm.gwl.num_rats = 1;
    sid_exp.prm.gwl.gwl_pri_lst[0] = cmd->mode;

    /* add record in SR table */
    ral_sys_ref = sdsr_tbl_store11( SD_MODE_GWL,
                                    (cmd->mode == SD_MODE_LTE || cmd->mode == SD_MODE_TDS) ? 0 : cmd->band_pref.chgwt_band ,
                              (cmd->mode == SD_MODE_LTE ) ? sdsr_map_lte_band_pref_to_sys_rec_lte_band(cmd->band_pref.lte_band) :
                                                            sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                                    (cmd->mode == SD_MODE_TDS ) ? cmd->band_pref.chgwt_band  : 0,
                                    0, 
                                    &sid_exp, 
                                    SD_NO_CHANGE_SYS_IDX );
    SD_MSG_HIGH_1("RAL: Add ral_sys_ref %d to sdsr_table",ral_sys_ref);	
    if(ral_sys_ref == SDSR_REF_NULL)
    {
      /* fail safe mechanism */
      ral_sys_ref = acq_sys_ref;
    }
    else
    {
      sdsr_s_type* ral_sr_ptr = NULL;
      ral_sr_ptr = sdsr_tbl_get_ptr( ral_sys_ref );
      if(ral_sr_ptr != NULL)
      {
        ral_sr_ptr->sid.gw.mode = cmd->mode;
        /* to store sd_band_e_type rather than MASK here */
        if( cmd->mode == SD_MODE_LTE  )
        {
          ral_sr_ptr->sid.gw.band.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(cmd->band_pref.lte_band);
        }
        else
        {
          ral_sr_ptr->sid.gw.band.chgwt_band = band;
        }
      }
      else
      {
        SD_MSG_HIGH_0("RAL: error in getting sr_ptr for");
      }
    }
  }
  else
  {
    ral_sys_ref = acq_sys_ref;
  }
  /* Insert the acquired system at the beginning of the SDSR_ACQED_LST and
  ** remove any duplicates.
  */
  sdsr_list_insert_ref( sdss_sr_list_map2( ss, SDSR_ACQED_LST ), 0, ral_sys_ref, TRUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the acquired system does not conflict with the current SS-Preference
  ** and it is designated as an available in the PRL, append the acquired
  ** system to the AVAIL_LST list.
  */

  if( sdss_sr_is_pref_conf(ss, acq_sys_ptr, SDSS_PREF_CONF_NONE) &&
      sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_AVAIL) )
  {
    sdsr_list_app( sdss_sr_list_map2( ss, SDSR_AVAIL_LST ), sdss_sr_list_map2(ss,SDSR_ACQ_SYS) );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ss )
  {
    case SD_SS_MAIN:
      /* In case if SD previously allowed camping on system which NAS queried during BSR service request , state will be moved
	to DONE ,update only SS state  */
     if(sdss_eng_stack_get_state(ss)!= SDSS_ACQ_STT_DONE)
     {	 
       act = sd_ss_event_proc(ss, SDSS_EVT_GWL_ACQ_PLMN_SUCCESS, cmd->rtrn_act_ptr );	
     }
     else
     {
       sdss_set_ss_state(ss,SDSS_STATE_OPR);
     }
     break;
     
    case SD_SS_HYBR_1:
      /* Process the SS-Event that is associated with this SS-Indication.
      */
      act = sd_ss_event_proc(ss, SDSS_EVT_GWL_ACQ_PLMN_SUCCESS, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_2:
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_HYBR_2_GW_ACQ_PLMN_SUCCESS, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_3:
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_HYBR_3_GW_ACQ_PLMN_SUCCESS, cmd->rtrn_act_ptr );
      #endif
      break;

    case SD_SS_MAX:
    default:
      SD_ERR_1(" Invalid ss %d",ss);
      act = SD_SS_ACT_CONTINUE;
      break;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_HDR
                    ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Fill in the mode_info with the LAC/TAC/RAC info.
  */
  if( cmd->mode == SD_MODE_GSM || cmd->mode == SD_MODE_WCDMA)
  {
    mode_info.gwl.lac = cmd->lac_tac_rac.lac;
  }
  #ifdef FEATURE_SD_LTE
  else
  {
    mode_info.gwl.tac = cmd->lac_tac_rac.tac;
  }
  #endif

  mode_info.gwl.rac_or_mme_code = cmd->lac_tac_rac.rac_or_mme_code;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Action is CONTINUE (i.e., we have selected a new serving system),
  ** do the following updates.
  */
  if( act == SD_SS_ACT_CONTINUE )
  {
    /* 
           If UE is acquiring either G/W/L, reset the TELE BSR params and let SD follow the 
           legacy c2k 2 stage algorithm for more pref 1X.
           
            SD follows below timelines for different cases.
            
           1.If LTE FULL SRV is acquired on HYBR_2,
             a) No need to run any BSR timer for LTE as FULL SRV is acquired and NAS PLMN selection takes
                care of moving to high priority PLMN.
             
           2.If LTE/GSM/WCDMA FULL SRV is acquired on MAIN,
             a) NAS takes care of looking for more pref G/W/L systems. 
             b) If there are more pref 1X systems, SD will follow legacy 2 stage c2k BSR algorithm.

           3.If 1x is acquired on MAIN and if UE is in SRLTE mode
             a) If there are more pref 1X sytems, SD will follow legacy 2 stage BSR algorithm for 1X/GW.

           4.If 1X is acquired on MAIN when UE is in CSFB sub mode of operation.
             a) If there are more pref 1X & LTE systesms, SD will follow LTE -TELE BSR timeline for more pref LTE
                Also starts 1X 2 stage c2k algorith for more pref 1X.
             b) If there are no more pref LTE systems and has only more pref 1X, SD will follow legacy 2 stage BSR algorithm.
         */

      /* Reset LTE tele BSR parameters when acquired service*/
      sdss_lte_telescope_parm_reset(ss);
       /* Reset 1X tele BSR parameters */
      sdss_1x_telescope_parm_reset();
	
    /* reset emerg in roam flag for volte specialization feature */
    sdss_set_e911_in_roam(ss,FALSE);

    /* Send LTE BSR end success */
    if(send_bsr_success)
    {
      end_info.bsr_status = 1; /* Success */
      event_report_payload(EVENT_SD_HYBR2_BSR_END,
                     sizeof(sdlog_bsr_end_info_s_type),
                     &end_info );
    }
    
    (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);
    if ( cmd->service_status == SYS_SRV_STATUS_SRV &&
         ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_MANUAL )
    {
      /* Point at manual system record */
      sdsr_s_type         *user_sys_ptr  = NULL;

      /* Fetch the user system record */
      user_sys_ptr   = sdsr_list_get_ptr( sdss_sr_list_map2( ss, SDSR_MANUAL_LST), 0 );

      if( user_sys_ptr != NULL &&
          ( memcmp (&(user_sys_ptr->sid_expected.prm.gwl.plmn.plmn_id),
                    &cmd->plmn,
                    sizeof (cmd->plmn)) != 0))
      {
        if(sdss_get_non_rplmn_csg_selection_type(ss) == FALSE )
        {
          user_sys_ptr->sid_expected.prm.gwl.plmn.plmn_id = cmd->plmn;
          SD_MSG_HIGH_3 ("sid_expected updated with new plmn %d %d %d",
                        cmd->plmn.identity[0], cmd->plmn.identity[1], cmd->plmn.identity[2]);
        }
      }
    }
    
    /* Clear SDSR_SIB8_1X_NBR_LST if SD decides to stay on G/W/T system */

    if((cmd->mode == SD_MODE_GSM || cmd->mode == SD_MODE_WCDMA) &&
	ss == SD_SS_MAIN )
    {
      sdsr_list_clr(SDSR_SIB8_1X_NBR_LST);
    }

    /* Update the system indicators per the acquired system.
    */
    sd_si_update( ss,
                  SD_SI_UPDATE_REG,
                  service_status_rcvd,
                  cmd->srv_domain,
                  acq_sys_ref,
                  FALSE,
                  &cmd->mode,
                  &band,
                  NULL,
                  &mode_info,
                  SDSR_MAX,
                  NULL,
                  NULL,
                  NULL,
                  cmd->cs_service_status,
                  cmd->lte_cs_capability,
                  cmd->sib8_available,
                  cmd->lte_ims_voice_avail,
                  cmd->lte_ims_emerg_avail? SYS_IMS_LTE_EMERG_SUPP_AVAILABLE : SYS_IMS_LTE_EMERG_SUPP_NOT_AVAILABLE,
                  cmd->req_id,
                  cmd->extend_srv_info,
                  cmd->reg_domain,
                  cmd->csg_info_ptr,
                  FALSE
                );

    if( cmd->service_status == SYS_SRV_STATUS_SRV )
    {
       /* For Multimode devices, if device acquired 1x from TOT/PRL scans during Limited to FULL
          reselection phase, reset the resel_mode now */	
       sdss_reset_resel_mode_if_full(ss);
    }
    /* Update sdss's srv_status too.
    */
    sdss_update_srv_status( ss, cmd->service_status, cmd->srv_domain );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Update the candidate system per the acquired system.
    */
    sdsr_list_put_ref( sdss_sr_list_map2( ss, SDSR_CAND_SYS ), acq_sys_ref );

    /* Update the Serving system.
    */
    sd_ss_srv_sys_update( ss,
                          sdss_sr_list_map2( ss, SDSR_ACQ_SYS),
                          sdss_sr_list_map2( ss, SDSR_ACQ_GEO_LST),
                          sdss_sr_list_map2( ss, SDSR_ACQ_PREF_LST),
                          SDSR_MAX );

    /* Update the idle system per the acquired system.
    */
    sd_ss_idle_sys_update( ss,
                           sdss_sr_list_map2( ss, SDSR_ACQ_SYS),
                           sdss_sr_list_map2( ss, SDSR_ACQ_GEO_LST),
                           sdss_sr_list_map2( ss, SDSR_ACQ_PREF_LST) );

    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
    /* Update the data suspend state.
    */
    sd_si_update_data_suspend_state( ss, cmd->data_suspend, cmd->service_status );
    #endif

    /* Inform the registered client (i.e., CM) that it is OK to originate as
    ** necessary. If UE is in camp_only mode, no calls can be originated.
    */
    if( sdss_get_camp_mode() != SD_SS_PREF_CAMP_MODE_ON )
    {
      sd_si_ok_to_orig( ss, sd_si_ptr(ss)->data_suspend );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check if BSR timer uptime needs to be reset
    */
    sdss_check_bsr_timer(ss);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  	

     sdss_get_rat_pri_list_info(ss, &ss_rat_pri_list_info);
    /*Update more_pref_do list if HDR is mode pref */
    if( sdss_is_1x_sxlte() && (ss == SD_SS_HYBR_2) &&
    sdss_get_rat_pri_sys_idx( ss, SD_MODE_HDR ) != ss_rat_pri_list_info.num_items )
    {
      sdsr_generate_more_pref_hdr_lst(ss, SDSR_SRV_PREF_LST);
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    switch( ss )
    {
      case SD_SS_MAIN:
      case SD_SS_HYBR_1:
        /* Process the SS-Event that is associated with this SS-Indication.
        */
        act = sd_ss_event_proc( ss, SDSS_EVT_GWL_OPR_SRV_INFO, cmd->rtrn_act_ptr );
        break;
      case SD_SS_HYBR_2:
        /* Process the SS Event that is associated with thsi SS-Indication.
        */
        act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_HYBR_2_GW_OPR_SRV_INFO, cmd->rtrn_act_ptr );
        break;

      case SD_SS_HYBR_3:
        #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
        /* Process the SS Event that is associated with thsi SS-Indication.
        */
        act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_HYBR_3_GW_OPR_SRV_INFO, cmd->rtrn_act_ptr );
        #endif
        break;

      case SD_SS_MAX:
      default:

        SD_ERR_1(" wrong ss %d",ss);
        act = SD_SS_ACT_CONTINUE;
        break;
    }

    /* Make sure the returned value is one of the allowed SS-Actions for this
    ** SS-Indication.
    */
    SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                      SD_SS_ACT_ACQ_GWL  |
                      SD_SS_ACT_ACQ_CDMA |
                      SD_SS_ACT_ACQ_HDR  | 
                      SD_SS_ACT_PWR_SAVE 
                      ) );


   /* Reset HDR deact by Hybr2 BSR flag to FALSE  */
   if ((ss == SD_SS_HYBR_2) && (act == SD_SS_ACT_CONTINUE))
   {
     sdss_set_flag(SD_SS_HYBR_2,SDSS_FLAG_HDR_DEACT_BY_HYBR2_BSR, FALSE);
   }

   /* Make sure to send SRV_IND to CM after entering OPR state */
    if ((sd_si_ptr(ss)->si_info.is_stable_in_svc == FALSE) &&
        (sdss_get_ss_state(ss) == SDSS_STATE_OPR))
    {
      SD_MSG_HIGH_0 ("Triggering SRV_IND due to change in is_stable_in_srv");


      /* If UE goes to OPR state on MAIN, reset the kick HYBR_2 flag */
      if ((ss == SD_SS_MAIN) &&
           sdss_is_flag_set(SD_SS_HYBR_2, SDSS_FLAG_IS_FORCE_KICK_HYBR2))
      {
        sdss_set_flag(SD_SS_HYBR_2, SDSS_FLAG_IS_FORCE_KICK_HYBR2, FALSE);
      }
      
      /* Update the system indicators per the acquired system.*/
      sd_si_update( ss,
                    SD_SI_UPDATE_REG,
                    service_status_rcvd,
                    cmd->srv_domain,
                    acq_sys_ref,
                    FALSE,
                    &cmd->mode,
                    &band,
                    NULL,
                    &mode_info,
                    SDSR_MAX,
                    NULL,
                    NULL,
                    NULL,
                    cmd->cs_service_status,
                    cmd->lte_cs_capability,
                    cmd->sib8_available,
                    cmd->lte_ims_voice_avail,
                    cmd->lte_ims_emerg_avail ? SYS_IMS_LTE_EMERG_SUPP_AVAILABLE : SYS_IMS_LTE_EMERG_SUPP_NOT_AVAILABLE,
                    cmd->req_id,
                    cmd->extend_srv_info,
                    cmd->reg_domain,
                    NULL,
                    FALSE
                  );
    }
    	 
  } /* if ( act == SD_SS_ACT_CONTINUE ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else (we have not yet selected a serving system), if the acquired system
  ** is allowed by the PRL and complies with the current SS-Preference do the
  ** following updates:
  */
  else if(sdss_sr_is_prl(acq_sys_ptr, SDSS_PRL_VAL_ALLOWED) &&
          sdss_sr_is_pref_conf(ss, acq_sys_ptr, SDSS_PREF_CONF_NONE))
  {
    /* If the candidate system is NULL or the acquired system is more
    ** preferred than the candidate system, update the candidate system per
    ** the acquired system and reorder the available list according to the
    ** new candidate system.
    */
    if( cand_sys_ptr == NULL ||
        sdprl_is_more_pref(ss, acq_sys_ptr, cand_sys_ptr) )
    {
      sdsr_list_put_ref( sdss_sr_list_map2( ss, SDSR_CAND_SYS ), acq_sys_ref );
      sdsr_list_order( sdss_sr_list_map2( ss, SDSR_AVAIL_LST), sdss_sr_list_map2( ss, SDSR_CAND_SYS) );
    }

    /* If before processing the SS-Event the service indicator was OFF,
    ** update the system indicators per the acquired system.
    */
    if( ! is_srv_on )
    {
      sd_si_update( ss,
                    SD_SI_UPDATE_REG,
                    service_status_rcvd,
                    cmd->srv_domain,
                    acq_sys_ref,
                    FALSE,
                    &cmd->mode,
                    &band,
                    NULL,
                    &mode_info,
                    SDSR_MAX,
                    NULL,
                    NULL,
                    NULL,
                    cmd->cs_service_status,
                    cmd->lte_cs_capability,
                    cmd->sib8_available,
                    cmd->lte_ims_voice_avail,
                    cmd->lte_ims_emerg_avail? SYS_IMS_LTE_EMERG_SUPP_AVAILABLE : SYS_IMS_LTE_EMERG_SUPP_NOT_AVAILABLE,
                    cmd->req_id,
                    cmd->extend_srv_info,
                    cmd->reg_domain,
                    cmd->csg_info_ptr,
                    FALSE
                   );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update sdss's srv_status with the non adjusted value so that CM and SD
  ** remain in sync.
  */
  if( cmd->service_status != service_status_rcvd )
  {
    sdss_update_srv_status( ss, service_status_rcvd, cmd->srv_domain );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  /* Update the data suspend state.
  */
  sd_si_update_data_suspend_state( ss, cmd->data_suspend, cmd->service_status );
#endif

#if defined FEATURE_WRLF_SYSTEM_SEL || defined FEATURE_MMODE_DUAL_SIM
  modem_mem_free(ss_true_curr_act_ptr,MODEM_MEM_CLIENT_MMODE);
#endif
  
  modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);

  return act;

} /* sd_ss_ind_gw_acq_plmn_success_ss_1() */


/*===========================================================================

FUNCTION sdcmd_gw_acq_plmn_camped_ss

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_gw_acq_plmn_camped_ss (
  sd_ss_ind_gw_acq_plmn_camped_cmd  *cmd
)
{
  sd_ss_e_type  ss = cmd->ss;

  /* Point at acquired system.
  */
  sdsr_ref_type       acq_sys_ref   = sdsr_list_get_ref( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), 0 );
  sdsr_s_type         *acq_sys_ptr  = sdsr_list_get_ptr( sdss_sr_list_map2(ss,SDSR_ACQ_SYS), 0 );
  sd_si_s_type        *si_ptr       = sd_si_ptr( ss );
  sys_srv_status_e_type  srv_status = SYS_SRV_STATUS_LIMITED;
  sd_plmn_s_type      acq_sys;
  sd_band_e_type      band ;

  sd_si_info_s_type   *si_info_ptr = &sd_si_ptr(ss)->si_info;


  if (cmd->mode == SD_MODE_LTE)
  {
       band = sdss_map_band_pref_to_band_lte(cmd->band_pref.lte_band);
  }
  else if (cmd->mode == SD_MODE_TDS)
  {
       band = sdss_map_band_pref_to_band_tds(cmd->band_pref.chgwt_band);
  }
  else
  {
       band = sdss_map_band_pref_to_band(cmd->band_pref.chgwt_band);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  acq_sys.plmn_id = cmd->plmn;
  acq_sys.lac = cmd->lac_id;
  SD_MSG_HIGH_4( "GW acq_plmn_camped mode %d band 0x%x ss %d acq_sys_ref=%d", cmd->mode, band, ss, acq_sys_ref);
  SD_MSG_HIGH_3("acq_sys.plmn_id:[0x%x] [0x%x] [0x%x] ",
              acq_sys.plmn_id.identity[0],
              acq_sys.plmn_id.identity[1],
              acq_sys.plmn_id.identity[2]);


  SD_ASSERT( cmd->mode == SD_MODE_GSM || cmd->mode == SD_MODE_WCDMA ||
     cmd->mode == SD_MODE_LTE || cmd->mode == SD_MODE_TDS);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_EOOS_UMTS
   /*  Reset the EOoS field since we are in service */
  sdss_reset_eoos_timer(ss);
  #endif

  if(acq_sys_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update is_plmn_found to true for the acquisition.
   */
  #ifdef FEATURE_AVOID_DUP_BAND_SCAN

  
  sdss_update_3gpp_acq_info(ss, SDSS_UPDATE_3GPP_ACQ_INFO_IS_PLMN_FOUND,
                              TRUE, SYS_SRV_STATUS_NONE);
  
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update with acquired system mode for GW sr to perform correct mathcing.
  */
  if( (acq_sys_ptr->sys.mode == SD_MODE_GW) ||
      (acq_sys_ptr->sys.mode == SD_MODE_GWL) ||
      (acq_sys_ptr->sys.mode == SD_MODE_TDS)
     )
  {
    acq_sys_ptr->sid.gw.mode = cmd->mode;
  }

  /* If the acquired system's mode is not match that in sr, it will error
  ** fatal except for GW mode.
  */

  if( acq_sys_ptr->sys.mode != SD_MODE_GW  &&
      acq_sys_ptr->sys.mode != SD_MODE_GWL &&
      acq_sys_ptr->sys.mode != SD_MODE_TDS &&
      acq_sys_ptr->sys.mode != cmd->mode  )
  {
    sys_err_fatal_invalid_value_exception((int)acq_sys_ptr->sys.mode);
  }
  if (sdss_is_lte_connected_mode (ss))
  {
    /* Since UE has camped, it is no longer in LTE connected state
    */
    sdss_reset_lte_connected_mode (ss);
  }

  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  /* Update sdss's srv_status too.
  */
  sdss_update_srv_status( ss,
                          SYS_SRV_STATUS_LIMITED,
                          SYS_SRV_DOMAIN_CAMPED
                        );

  /*Reset acq_status, as we found PLMN*/
  sdss_update_acq_status(ss,SYS_ACQ_STATUS_SUCCESS);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the power scan list
  */
  sdsr_list_clr( sdss_sr_list_map2(ss,SDSR_PWR_SCAN_LST) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do LTE avail file processing for non_svlte */
  if(ss == SD_SS_MAIN &&
    sdss_is_c2k_alt_algr_operation() && 
      cmd->mode == SD_MODE_LTE &&
      si_info_ptr->mode == SYS_SYS_MODE_CDMA &&
         sd_si_is_full_srv_on( SD_SS_MAIN ))
  {
      sdss_lte_avail_sys_s_type sys;

      /* Only when we have 1x service, we need to update the lte available file
      ** 
      */
      sys.sid = si_info_ptr->sid.is95.sid;
      sys.nid = si_info_ptr->sid.is95.nid;
      sys.bid = si_info_ptr->mode_info.cdma.base_id;
      
      if(sys.bid != SDSS_BID_VAL_NONE)
      {
        sdss_lte_avail_file_update(SDSS_EVT_GWL_ACQ_PLMN_SUCCESS, &sys, ss);
      }
  }

  /* MS camped on a cell but did not do Location Update yet, so just inform
   ** the SI-Client of service only if there is no service and
   ** send ok_to_orig as necessary.
   */
  if ( sys_srv_status_is_full_srv( si_ptr->si_info.srv_status )&&
                                  (si_ptr->si_info.mode != SD_MODE_CDMA))
  {
    /* Leave srv_status same and update the srv_domain_camped.
       */
    srv_status = si_info_ptr->srv_status;
    si_info_ptr->srv_domain = SYS_SRV_DOMAIN_CAMPED;

  }
  else
  {
     /* Default fields plmn, plmn_forbidden, roam and srv_capability.
        */
     acq_sys_ptr->prl =  (byte) SDSS_PRL_MOST_PREF;
     acq_sys_ptr->roam = SYS_ROAM_STATUS_UNKNOWN;
     if(acq_sys_ptr->sid.gw.mode == SD_MODE_LTE)
     {
       acq_sys_ptr->srv_cap = SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;
     }
     else
     {
       acq_sys_ptr->srv_cap = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
     }

     /* Storing the PLMN and LAC ID Passed to this function parameters. */
     acq_sys_ptr->sid.plmn.plmn_id = cmd->plmn;
     acq_sys_ptr->sid.plmn.lac     = cmd->lac_id;
  }
  
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined( FEATURE_SD_LTE )
  if (sdss_is_emer_orig_in_volte_only(ss) &&
      (sdss_get_sim_state(ss) == SYS_SIM_STATE_AVAILABLE) &&
      ((cmd->mode == SD_MODE_GSM) || (cmd->mode == SD_MODE_WCDMA)))
  {
    SD_MSG_HIGH_0("E911_LTE_ONLY: Donot update G/W service during emergency orig");
    return SD_SS_ACT_CONTINUE;
  }
  #endif  

  if(!((sd_misc_is_plmn_lac_match(si_info_ptr->sid.plmn,acq_sys)) &&
           sys_srv_status_is_full_srv( si_info_ptr->srv_status )))
  {
    /* Send CS service status as Limited regional for Camped ind
    */
    sd_si_update_if_srv_is_off( ss,
                                SD_SI_UPDATE_ACQ,
                                srv_status,
                                SYS_SRV_DOMAIN_CAMPED,
                                acq_sys_ref,
                                &cmd->mode,
                                &band,
                                NULL,
                                SDSR_MAX,
                                NULL,
                                NULL,
                                NULL,
                                SYS_SRV_STATUS_LIMITED_REGIONAL,
                                SD_SIB8_NO_CHANGE,
                                SYS_EXTEND_SRV_INFO_NONE,
                                si_info_ptr->reg_domain,
                                NULL
                               );

     /* Send service status change if RAT changes
       */
    if ( cmd->mode != si_info_ptr->mode )
    {
      sd_si_update( ss,
                    SD_SI_UPDATE_ACQ,
                    srv_status,
                    SYS_SRV_DOMAIN_CAMPED,
                    acq_sys_ref,
                    FALSE,
                    &cmd->mode,
                    &band,
                    NULL,
                    NULL,
                    SDSR_MAX,
                    NULL,
                    NULL,
                    NULL,
                    SYS_SRV_STATUS_LIMITED_REGIONAL,
                    SYS_LTE_CS_CAPABILITY_NONE,
                    SD_SIB8_NO_CHANGE,
                    FALSE,
                    SYS_IMS_LTE_EMERG_SUPP_UNKNOWN,
                    0, /* Request Id not needed */
                    SYS_EXTEND_SRV_INFO_NONE,
                    si_info_ptr->reg_domain,
                    NULL,
                    FALSE);
    }
  }

  if( cmd->mode != SYS_SYS_MODE_LTE )
  {
    /* Send ok to orig only if the mode is not LTE
    ** For LTE - wait till SRV_CNF to send origination indication
    */
    sd_si_ok_to_orig( ss, si_ptr->data_suspend );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SS-Client supplied a buffer for the return SS-Action, set the
  ** returned SS-Action in this buffer to CONTINUE.
  */
  if( cmd->rtrn_act_ptr != NULL )
  {
    cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  return SD_SS_ACT_CONTINUE;

} /* sd_ss_ind_gw_acq_plmn_camped */


/*===========================================================================

FUNCTION sdcmd_lte_suitable_search_end

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_lte_suitable_search_end(
  sd_cmd_t  *cmd
)
{

  sd_ss_act_e_type    act         = SD_SS_ACT_CONTINUE;
  sd_ss_act_s_type    *ss_act_ptr     = NULL;
  sd_ss_act_s_type    *ss_true_curr_act_ptr = NULL;
  sd_ss_e_type        ss = cmd->ss;

  /*
  ** If the UE is in LTE Connected mode, force the end to that state
  */
  ss_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (ss_act_ptr != NULL)
  {
    memset(ss_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  
  ss_true_curr_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (ss_true_curr_act_ptr != NULL)
  {
    memset(ss_true_curr_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  if((sdss_is_lte_connected_mode (ss)))
  {
    sys_acq_status_e_type acq_status = sdss_get_acq_status_only(ss);
    
    sdss_reset_lte_connected_mode(ss);
    SD_MSG_HIGH_2("UE ending LTE connected mode, acq_status=%d scanscope %d", 
                  acq_status,sdss_get_scan_scope_rule());

    /*Update Service Loss to Clients*/
    if (acq_status != SYS_ACQ_STATUS_NO_RESOURCE)
    {
      sd_ss_ind_misc_srv_lost(ss);
    }

    /* 3GPP only sub so return continue directly */
    if ((ss == SD_SS_HYBR_3 && sdss_is_sxlte()) ||
        (!sdss_is_sxlte() && 
           ss == SD_SS_HYBR_2 && 
           sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM)
         )
    )
    {
      SD_MSG_HIGH_1("RLF on ss %d, return continue", ss);
    }

    /* Here, it's intentional to give SD_SS_MAIN as a parameter. The requirement
    ** for Suitable Search End is that SD should immediately act and send
    ** new service request in case of MMSS/SVLTE world, but in 3GPP only world,
    ** SD should remains CONTINUE and NAS will do all the neccessary scans.
    ** Once done, NAS will inform REGPRX with SERVICE_CNF.
    **
    ** Hence, if 3GPP-only is detected on Main, we can safely return CONTINUE without
    ** trigger further action.
       When VOLTE call on going, avoid sending new service request
       as this might trigger giving away TRM to sub2 in MSIM configuration
    */
    else if (!sdss_is_cap_3gpp_only(SD_SS_MAIN) && !( sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) &&
                                                 sdss_is_sfmode_svlte( (uint8)SD_SS_MAIN) &&
                                                 (acq_status == SYS_ACQ_STATUS_NO_RESOURCE)) && 
		                                  !(cmsds_is_gwt_scan_allowed(SD_SS_MAIN) && 
						 (sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE) == SD_SS_MAIN)))
    {
      (void)sd_ss_act_get_true_curr_act_ss(ss,ss_act_ptr);
      (void)sd_ss_act_get_curr_act_ss(ss,ss_true_curr_act_ptr);
      /* Update the srv request type accordingly
      */
      ss_true_curr_act_ptr->prm.acq_gwl.srv_req_type = sdss_get_srv_req_type(ss);
      ss_act_ptr->prm.acq_gwl.srv_req_type = sdss_get_srv_req_type(ss);
     
      if(ss == SD_SS_HYBR_2 &&
         sdss_is_1x_sxlte())
      {
        /* Update PREF_LIST if this is for HYBR2 stack */
        sd_ss_update_hybr2_pref_list();
      }

      sd_ss_act_set_true_curr_act_ss(ss,ss_act_ptr);
      /*sd_ss_act_set_curr_act_ss(ss,&ss_true_curr_act);
      ** Process the SS-Event that is associated with this SS-Indication.
      */
      act = sd_ss_event_proc( ss,
                              sdss_event_map(ss,SDSS_EVT_USER_SS_PREF),
                              cmd->rtrn_act_ptr );
    }
  }

  if(cmd->rtrn_act_ptr != NULL)
  {
      cmd->rtrn_act_ptr->act = act;
  }
  modem_mem_free(ss_act_ptr,MODEM_MEM_CLIENT_MMODE);
  modem_mem_free(ss_true_curr_act_ptr,MODEM_MEM_CLIENT_MMODE);

  return act;
}

/*===========================================================================

FUNCTION sdcmd_update_sys_sel_pref_req_id

DESCRIPTION

===========================================================================*/
void    sdcmd_update_sys_sel_pref_req_id (
    sd_ss_e_type               ss,
    /* Stack id
      */
    uint16                     req_id
    /* Request Id
      */
)
{
  sd_si_info_s_type     *si_info_ptr = &sd_si_ptr(ss)->si_info;

  /* Update sys_sel_pref_req_id from command */
  SD_CRIT_SECT_ENTER();
  si_info_ptr->sys_sel_pref_req_id = req_id;
  SD_CRIT_SECT_EXIT();

  SD_MSG_HIGH_1( "sdcmd_update_sys_sel_pref_req_id, req_id=%d",req_id);
  sd_invoke_info_callback(ss);

}

#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) */


/*===========================================================================

FUNCTION sdcmd_hybr_2_misc_meas_rprt

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hybr_2_misc_meas_rprt(
      sd_ss_ind_misc_meas_rprt_cmd  *cmd
)
{
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_HYBR_2);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
  return SD_SS_ACT_CONTINUE;
  }

  /* If measurement ID does not match the ID of the measurement request,
  ** instruct SS-Client to CONTINUE.
  */
  if( cmd->meas_id != ss_ptr->meas_id )
  {
    SD_MSG_HIGH_2( "meas_id=%d mismatch - req_id=%d",
                 cmd->meas_id,ss_ptr->meas_id);

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the measurement ID matches the ID of the measurement
  ** request.
  */

  /* Indicate that measurement is done being serviced.
  */
  ss_ptr->meas_is_done = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If reselection is not allowed, instruct SS-Client to CONTINUE.
  */
  if( ! cmd->resel_is_allowed )
  {
    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the measurement ID matches the ID of the measurement
  ** request and reselection is allowed, so process the SS-Event that is
  ** associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_MISC_MEAS_RPRT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_misc_meas_rprt */


/*===========================================================================

FUNCTION sdcmd_hybr_2_misc_pwr_save_enter

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hybr_2_misc_pwr_save_enter(
        sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
  return SD_SS_ACT_CONTINUE;
  }

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_MISC_PWR_SAVE_ENTER, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_misc_pwr_save_enter */

/*===========================================================================

FUNCTION sdcmd_hybr_3_misc_meas_rprt

DESCRIPTION

===========================================================================*/
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
sd_ss_act_e_type          sdcmd_hybr_3_misc_meas_rprt(
      sd_ss_ind_misc_meas_rprt_cmd  *cmd
)
{
  sd_ss_act_e_type    act;
  sd_ss_s_type        *ss_ptr = sd_ss_ptr(SD_SS_HYBR_3);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
  return SD_SS_ACT_CONTINUE;
  }

  /* If measurement ID does not match the ID of the measurement request,
  ** instruct SS-Client to CONTINUE.
  */
  if( cmd->meas_id != ss_ptr->meas_id )
  {
    SD_MSG_HIGH_2( "meas_id=%d mismatch - req_id=%d",
                 cmd->meas_id,ss_ptr->meas_id);

    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the measurement ID matches the ID of the measurement
  ** request.
  */

  /* Indicate that measurement is done being serviced.
  */
  ss_ptr->meas_is_done = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If reselection is not allowed, instruct SS-Client to CONTINUE.
  */
  if( ! cmd->resel_is_allowed )
  {
    /* If SS-Client supplied a buffer for the return SS-Action, set the
    ** returned SS-Action in this buffer to CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the measurement ID matches the ID of the measurement
  ** request and reselection is allowed, so process the SS-Event that is
  ** associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_MISC_MEAS_RPRT, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sdcmd_hybr_3_misc_meas_rprt */


/*===========================================================================

FUNCTION sdcmd_hybr_3_misc_pwr_save_enter

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hybr_3_misc_pwr_save_enter(
        sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_NORMAL))
  {
  return SD_SS_ACT_CONTINUE;
  }

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_MISC_PWR_SAVE_ENTER, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GW   |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_MEAS_DED ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sdcmd_hybr_3_misc_pwr_save_enter */

#endif 
/*===========================================================================

FUNCTION sdcmd_misc_orig_success

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_misc_orig_success(
      sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_MAX;
  sd_si_s_type       *si_ptr = sd_si_ptr( SD_SS_MAIN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(act);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifndef FEATURE_JCDMA
  /* Update the SS-Core with the origination success indication.
  */
  sdss_orig_success_update( SD_SS_MAIN );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_ORIG_SUCCESS, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the registered client (i.e., CM) that it is OK to originate as
  ** necessary.
  */
  sd_si_ok_to_orig( SD_SS_MAIN, si_ptr->data_suspend );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SD_SS_ACT_CONTINUE;

} /* sd_ss_ind_misc_orig_success */


/*===========================================================================

FUNCTION sdcmd_misc_stack_local_activate

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_misc_pwr_save_enter(
      sd_cmd_t  *cmd
)
{
  #ifdef FEATURE_HYBR_3GPP
  return sdcmd_hybr_2_misc_pwr_save_enter(cmd);
  #elif defined(CFG_HYBR_GW_3)
  return sdcmd_hybr_3_misc_pwr_save_enter(cmd);
  #else
  sd_ss_act_e_type    act;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_PWR_SAVE_ENTER, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;
  #endif //CFG_HYBR_GW

} /* sd_ss_ind_misc_pwr_save_enter */


/*===========================================================================

FUNCTION sdcmd_hybr_1_misc_pwr_save_enter

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type          sdcmd_hybr_1_misc_pwr_save_enter(
      sd_cmd_t  *cmd
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( defined(FEATURE_HDR_HYBRID))
  sd_ss_act_e_type    act;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If currently not doing CDMA+HDR hybrid operation, call on
  ** sd_ss_ind_misc_meas_rprt().
  */
  if(!sd_misc_is_hybr_opr() )
  {
    return sdcmd_misc_pwr_save_enter(cmd);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if ( !defined(FEATURE_HDR_HYBRID))
  return sdcmd_misc_pwr_save_enter(cmd);
  #else

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_hdr_event_proc( SDSS_EVT_MISC_PWR_SAVE_ENTER, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

  #endif /*FEATURE_HDR_HYBRID */
} /* sd_ss_ind_hybr_1_misc_pwr_save_enter */


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || \
    defined(FEATURE_SD_LTE)
/*===========================================================================

FUNCTION sdcmd_misc_gw_sim_state_update

DESCRIPTION

===========================================================================*/   
sd_ss_act_e_type          sdcmd_misc_gw_sim_state_update(
      sd_ss_ind_misc_gw_sim_state_update_cmd  *cmd
)
{
  sd_ss_act_e_type    act            = SD_SS_ACT_CONTINUE;
  sd_si_s_type        *si_ptr       = sd_si_ptr( cmd->ss );

  /* Get the current SS-State.
  */
  sdss_state_e_type     ss_state      = sdss_get_ss_state(cmd->ss);


  SD_ASSERT_ENUM_IS_INRANGE( cmd->ss, SD_SS_MAX );

  SD_MSG_HIGH_1("GW sim state update from REG : %d", cmd->sim_state);


  if( ( cmd->sim_state == SYS_SIM_STATE_AVAILABLE )&&
      ( ( sdss_get_sim_state(cmd->ss)== SYS_SIM_STATE_PS_INVALID ) ||
        ( sdss_get_sim_state(cmd->ss)== SYS_SIM_STATE_CS_INVALID ) ) )
  {
 
    if(sdss_is_1x_sxlte() && (cmd->ss!= SD_SS_HYBR_3))
    {
       sd_si_update_sim_state( cmd->sim_state, SD_SS_MAIN );
       sd_si_update_sim_state( cmd->sim_state, SD_SS_HYBR_2 );
    }
    else
    {
      sd_si_update_sim_state( cmd->sim_state, cmd->ss );
    }

     if( (ss_state == SDSS_STATE_OPR) && 
         ( (si_ptr->si_info.mode == SYS_SYS_MODE_CDMA ) || (si_ptr->si_info.mode == SYS_SYS_MODE_HDR )))
      {
        act = SD_SS_ACT_CONTINUE ;
      }
      else
      {
        act = sd_ss_event_proc( cmd->ss,
                              SDSS_EVT_MISC_GW_SIM_STATE_UPDATE,
                              cmd->rtrn_act_ptr );
      }
   }
   else
   {
     /* To handle the case where T3245 timer expiry is received when CM has already 
     ** sent SERVICE_REQ and waiting for SRV_CNF
     */ 
     act = SD_SS_ACT_CONTINUE ;
   }
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Make sure the returned value is one of the allowed SS-Actions for this
   ** SS-Indication.
   */
   SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_GW |
                    SD_SS_ACT_ACQ_GWL ));


   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   return act;

} /* sd_ss_ind_misc_pwr_save_enter */
#endif


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
/*===========================================================================

FUNCTION sdcmd_misc_gw_net_list_rpt_ss

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type              sdcmd_misc_gw_net_list_rpt_ss(
      sd_ss_ind_misc_gw_net_list_rpt_ss_cmd *cmd
)
{

  sd_ss_act_e_type    act;
  sd_ss_e_type        ss = cmd->ss;
  sd_ss_mode_pref_e_type ss_mode_pref, user_mode_pref;

  sd_ss_band_pref_e_type     user_band_pref, tds_band_pref;
  sys_lte_band_mask_e_type   lte_band_pref ;
  sd_ss_act_s_type    *curr_act_ptr   = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( cmd->plmn_list_ptr != NULL );
  SD_ASSERT( cmd->rplmn_ptr     != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  curr_act_ptr = (sd_ss_act_s_type *)modem_mem_alloc
                      (sizeof(sd_ss_act_s_type), MODEM_MEM_CLIENT_MMODE);
  if (curr_act_ptr != NULL)
  {
    memset(curr_act_ptr, 0, sizeof(sd_ss_act_s_type));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  sd_ss_act_get_true_curr_act(curr_act_ptr);

  
  /* Indicate that Get networks request is done when the plmn list is not
  ** empty and update the LAST_GW_USER_SYS.
  */
  if( (curr_act_ptr->prm.get_net_gw.network_list_type != SD_NETWORK_LIST_PCI_SCAN  &&
      cmd->plmn_list_ptr->plmn_list.length > 0) ||
      cmd->plmn_list_status == SYS_PLMN_LIST_AS_ABORT ||
      cmd->plmn_list_status == SYS_PLMN_LIST_REJ_IN_RLF)
  {

    SD_MSG_HIGH_2(" num of PLMN List = %d, status =%d",
                cmd->plmn_list_ptr->plmn_list.length,
                cmd->plmn_list_status);
    sd_ss_update_get_net_done( ss, TRUE );
    
    if (ss == SD_SS_MAIN && 
        (curr_act_ptr->prm.get_net_gw.network_list_type == SD_NETWORK_LIST_CSG_USER ||
         curr_act_ptr->prm.get_net_gw.network_list_type ==SD_NETWORK_LIST_CSG_SELECTION_APP) )
    { 
      sdss_get_mode_band_pref(ss, &ss_mode_pref, NULL,NULL,NULL);
      
      if (!sd_misc_is_mode_pref(ss_mode_pref, SD_SS_MODE_PREF_WCDMA)
        )
      {
        sdss_get_user_mode_pref(ss, &user_mode_pref, &user_band_pref, &lte_band_pref, &tds_band_pref);
        sdss_set_mode_band_pref(ss, user_mode_pref, user_band_pref, lte_band_pref, tds_band_pref);
       
        sdss_update_rat_pri_list_info(ss, &curr_act_ptr->prm.get_net_gw.rat_pri_list_info);
      }
    }
  }
  SD_MSG_HIGH_2("GET NET RPT plmn list status %d plmn list length %d",
                       cmd->plmn_list_status,cmd->plmn_list_ptr->plmn_list.length);
  sdss_update_plmn_status(ss, cmd->plmn_list_status, cmd->plmn_list_ptr->plmn_list.length);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( ss,
                          sdss_event_map(ss,SDSS_EVT_MISC_GW_NET_LIST),
                          cmd->rtrn_act_ptr );



  if(sd_ss_is_get_net_done( ss ) )
  {
    /* Inform the SD client of get network results.
    */
    sd_si_net_list_rpt_cb( cmd->rplmn_ptr, ss, cmd->plmn_list_ptr, cmd->plmn_list_status );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE |
                    SD_SS_ACT_GET_NET_GW) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  modem_mem_free(curr_act_ptr,MODEM_MEM_CLIENT_MMODE);
  return act;


} /* sd_ss_ind_misc_gw_net_list_rpt2() */
#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */


#ifdef FEATURE_HDR
/*===========================================================================

FUNCTION sdcmd_misc_hdr_bcmcs_srv_status

DESCRIPTION

===========================================================================*/
sd_ss_act_e_type                sdcmd_misc_hdr_bcmcs_srv_status(
      sd_ss_ind_misc_hdr_bcmcs_srv_status_cmd *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;
  sd_ss_e_type        ss  = SD_SS_MAIN;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifdef FEATURE_HDR_HYBRID
  /* If hybrid operation, work with the HDR system selection.
  */
  if( sd_misc_is_hybr_opr() )
  {
   /* Update BCMCS on the HDR instance.
   */
   ss = SD_SS_HYBR_HDR;

  }
  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update BCMCS service status.
  */
  sd_si_update_bcmcs_status( ss, cmd->bcmcs_srv_status );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if rtrn_act_ptr is not NULL and populate return action.
  */
  if ( cmd->rtrn_act_ptr != NULL )
  {
    cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
  }

  SD_ASSERT( act & SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_misc_hdr_bcmcs_srv_status */

#endif /* FEATURE_HDR */


/*===========================================================================

FUNCTION sdcmd_misc_mru_write

DESCRIPTION

===========================================================================*/
void   sdcmd_misc_mru_write ( sd_cmd_t  *cmd )
{
    sdprl_save();
}


/*===========================================================================

FUNCTION sdcmd_misc_get_curr_sys_mode

DESCRIPTION

===========================================================================*/
sys_sys_mode_e_type       sdcmd_misc_get_curr_sys_mode(
      sd_misc_get_curr_sys_mode_cmd *cmd
)
{
  sd_mode_e_type         acq_sys_mode  = SD_MODE_INACT;
  sdsr_s_type           *main_acq_sys_ptr = sdsr_list_get_ptr(
                         sdss_sr_list_map2(SD_SS_MAIN,SDSR_ACQ_SYS),0);
  sdsr_s_type           *hybrid_acq_sys_ptr = sdsr_list_get_ptr(
                         sdss_sr_list_map2(SD_SS_HYBR_1,SDSR_ACQ_SYS),0);
  sd_si_info_s_type      *si_info_main_ptr = &sd_si_ptr(SD_SS_MAIN)->si_info;
  sd_si_info_s_type      *si_info_hybr_ptr = &sd_si_ptr(SD_SS_HYBR_1)->si_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* 1. If Main stack is in service, return the mode acquired
  */
  if( sys_srv_status_is_srv( si_info_main_ptr->srv_status ) )
  {
    acq_sys_mode = si_info_main_ptr->mode;

  }
  else if(cmd->is_acq_sys_needed == SD_WITH_ACQ_MODE)
  {
    /* 2. If Main stack is not in power save mode, return the mode under acquiring
    */
    if(si_info_main_ptr->srv_status != SYS_SRV_STATUS_PWR_SAVE)
    {
      if ( main_acq_sys_ptr != NULL )
      {
        acq_sys_mode = main_acq_sys_ptr->sys.mode;
      }
    }

    /* 3. If Hybrid stack is in service, return the mode acquired
    */
    else if( sys_srv_status_is_srv( si_info_hybr_ptr->srv_status ) )
    {
      acq_sys_mode = si_info_hybr_ptr->mode;
    }

    /* 4. If Hybrid stack is not in power save, return the mode under acquiring
    */
    else if(si_info_hybr_ptr->srv_status != SYS_SRV_STATUS_PWR_SAVE)
    {
      if(hybrid_acq_sys_ptr != NULL)
      {
        acq_sys_mode = hybrid_acq_sys_ptr->sys.mode;
      }
    }
    else
    {
     /* 5. Return no service otherwise
     */
     acq_sys_mode = SD_MODE_INACT;
    }

  }
  else if(cmd->is_acq_sys_needed == SD_WITHOUT_ACQ_MODE)
  {
    acq_sys_mode = SD_MODE_INACT;
  }

  return sd_misc_map_sd_mode_to_sys_mode(acq_sys_mode);

} /* sd_misc_get_curr_sys_mode */


/*===========================================================================

FUNCTION sdcmd_misc_sys_avoid_list

DESCRIPTION

===========================================================================*/
void                       sdcmd_misc_sys_avoid_list(
    sd_misc_sys_avoid_list_cmd *cmd
)
{
  sd_ss_e_type  ss  = sdss_get_ss_for_mode(cmd->mode);
  sdsr_s_type   *tbl_ptr;
  sdsr_ref_type i;
  byte num_avoided_sys = 0;
  sys_avoided_channel_record_type *avoid_list_ptr = cmd->avoid_list_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT( avoid_list_ptr != NULL );

 

  if( ss == SD_SS_MAX )
  {
    SD_MSG_MED_1("Mode %d not supported on any stack",cmd->mode);
    return;
  }

  for( i=0; (tbl_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {

    /* Loop through all the system with the given mode preference */
    if( (tbl_ptr->sys.mode == cmd->mode) && sdss_sr_is_avoid( ss, tbl_ptr) )
    {
      num_avoided_sys++;
      if(num_avoided_sys <= cmd->max_entry)
      {
        SD_MSG_HIGH_3(" This System is avoided:  Band = %d, Mode = %d, Channel = %d ",
        sdss_map_band_pref_to_band(tbl_ptr->sys.band), tbl_ptr->sys.mode, tbl_ptr->sys.chan);

        /* if the system is avoided, append the avoided system to the list */
        avoid_list_ptr->channel.band =
          sdss_map_band_pref_to_band(tbl_ptr->sys.band);
        avoid_list_ptr->channel.chan_num = tbl_ptr->sys.chan;
        avoid_list_ptr->mode = tbl_ptr->sys.mode;

        /* update timer - used by HDR for future optimization */
        avoid_list_ptr->uptime = time_get_uptime_secs();
        avoid_list_ptr->avoided_time = tbl_ptr->sched_prm.avoid_time;

        /* Update to next pointer */
        avoid_list_ptr++;
      }
      else
      {
        /* Keep the max count to be max number entry */
        num_avoided_sys = cmd->max_entry;
        SD_ERR_0( "List Over Flow");
      }
    }
  }

  *(cmd->num_sys) = num_avoided_sys;
  return;
} /* sd_misc_sys_avoid_list */


#if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
/*===========================================================================

FUNCTION sdcmd_misc_allowed_sid_nid

DESCRIPTION

===========================================================================*/
boolean        sdcmd_misc_allowed_sid_nid (
    sd_misc_allowed_sid_nid_cmd *cmd
)
{
 sdsr_s_type         sr;
  sdsr_ref_type sr_ref;
  sdsr_s_type         *cand_sys_ptr;

  sr.sys.mode = SD_MODE_CDMA;
  sr_ref = sdsr_tbl_store(&sr);

  cand_sys_ptr = sdsr_tbl_get_ptr(sr_ref);

  if (cand_sys_ptr == NULL)
  {
    SD_MSG_LOW_0("cand_sys_ptr NULL");
    return FALSE;
  }


  /* Update the SID/NID of the candidate system.
  */
  cand_sys_ptr->sid.is95.sid = cmd->sid;
  cand_sys_ptr->sid.is95.nid = cmd->nid;

  /* Update SDSR_CAND_CDMA_CSFB_SYS with this system.
    */
  sdsr_list_put_ref( SDSR_CAND_CDMA_CSFB_SYS, sr_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return    sdprl_get_info_base_basic(SDSR_CAND_CDMA_CSFB_SYS, 0)
         && sdss_sr_is_prl(cand_sys_ptr, SDSS_PRL_VAL_ALLOWED);
}


#endif /* defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X) */


/*===========================================================================

FUNCTION sdcmd_nam_sel3

DESCRIPTION

===========================================================================*/
boolean                          sdcmd_nam_sel4(
      sd_nam_sel_cmd  *cmd
)
{
  /* Point at manual system record.
  */
  sdsr_s_type         *user_sys_ptr  = NULL;
  sd_si_s_type          *si_ptr       = sd_si_ptr( SD_SS_MAIN );

  int                 i;
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_LTE)
  uint16    scan_scope_rule_val;
  #endif
  #if defined(FEATURE_SD_LTE) && defined(SD_DEBUG)
  #error code not present
#endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(cmd->curr_nam, SD_NAM_1, SD_NAM_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cmd->curr_nam != SD_NAM_MAX )
  {
    SD_ASSERT_ENUM_IS_INRANGE( cmd->orig_mode, SD_SS_ORIG_MODE_MAX );
    SD_ASSERT( cmd->orig_mode != SD_SS_ORIG_MODE_OTASP );

    SD_ASSERT_ENUM_IS_INRANGE( cmd->mode_pref, SD_SS_MODE_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( cmd->band_pref, SD_SS_BAND_PREF_MAX );
    #if defined(FEATURE_SD_LTE) && defined(SD_DEBUG)
    #error code not present
#endif
    SD_ASSERT_ENUM_IS_INRANGE( cmd->roam_pref, SD_SS_ROAM_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( cmd->hybr_pref, SD_SS_HYBR_PREF_MAX );

  }

  sdss_set_is_sdprl_failed(SD_SS_MAIN, FALSE);
  sdss_set_is_pref_update_called(SD_SS_MAIN, FALSE);
  sdss_set_is_sub2_map_to_hybr3(SD_SS_MAIN);

  SD_MSG_HIGH_5("MAIN_SD_NAM_SEL: main, is_1x_subsc_avail=%d, is_gwl_subsc_avail=%d is_sxlte %d, is_gw2_subsc_avail=%d, is_gw3_subsc_avail=%d",
                 cmd->is_1x_subsc_avail,
                 cmd->is_gwl_subsc_avail,
                 sdss_is_sxlte(),
                 cmd->is_gw2_subsc_avail, 
                 cmd->is_gw3_subsc_avail);

  /* Update the subscription status in SS-Core.
  ** This portion has to be done first as SD-NV operation depended on
  ** subscription status.
  */
  sdss_subsc_avail_update2( SD_SS_MAIN,
                           cmd->is_gwl_subsc_avail,
                           cmd->is_1x_subsc_avail,
                           cmd->is_gw2_subsc_avail,
                           cmd->is_gw3_subsc_avail
                         );
  sdss_subsc_avail_update2( SD_SS_HYBR_HDR,
                           cmd->is_gwl_subsc_avail,
                           cmd->is_1x_subsc_avail,
                           cmd->is_gw2_subsc_avail,
                           cmd->is_gw3_subsc_avail
                         );

  if( sdss_is_sglte() || sdss_is_1x_sxlte() )
  {
    sdss_subsc_avail_update2( SD_SS_HYBR_2,
                              cmd->is_gwl_subsc_avail,
                              cmd->is_1x_subsc_avail,
                              cmd->is_gw2_subsc_avail,
                              cmd->is_gw3_subsc_avail
                           );
  }
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_LTE)
  /* Update the SIM state if GSM subscription is available
  */
  SD_MSG_HIGH_5("sdcmd_nam_sel3()-simstate(MAIN):%d,simstat(HYBR2):%d  msim:%d,issglte:%d,issxlte:%d",
  				sdss_get_sim_state(SD_SS_MAIN),
                sdss_get_sim_state(SD_SS_HYBR_2),
                sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM),
                sdss_is_sglte(),
                sdss_is_1x_sxlte());

  if( cmd->is_gwl_subsc_avail &&
     sdss_get_sim_state(SD_SS_MAIN) == SYS_SIM_STATE_NOT_AVAILABLE )
  {
    SD_MSG_HIGH_0("GW subs available, set sim_state AVAILABLE");
    sd_si_update_sim_state( SYS_SIM_STATE_AVAILABLE, SD_SS_MAIN );

    /*Checks if GCF is enabled*/
    sdss_nv_init( SD_SS_MAIN );
    
    /*Checks for setting scan_scope_rule*/
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Read scan_scope_rule from EFS
    */
    /* If EFS is not set but CMCC subsc available, then enable Acq DB except for GCF*/
    if(!sdss_is_gprs_anite_gcf(SD_SS_MAIN) && !sdss_is_scan_scope_rule_avail())
    {
      /*NV not configured*/
      if( (sdss_get_operator_name ( SYS_MODEM_AS_ID_1 ) != SD_OPERATOR_CMCC)&& 
          sdss_is_usim_compliant_for_cmcc(SYS_MODEM_AS_ID_1))
      {
        scan_scope_rule_val = sdss_get_scan_scope_rule();
        scan_scope_rule_val = scan_scope_rule_val |
                              SYS_SCAN_SCOPE_RULE_ACQ_DB_RLF |
                              SYS_SCAN_SCOPE_RULE_ACQ_DB_PWR_UP |
                              SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS;
        sdss_set_scan_scope_rule(scan_scope_rule_val);
      }
    }
  }
  /* For FUSION, when acquired HDR, we populate the more prefered list
  ** to do BSR from HDR to GWL. We need the SIM state to set properly
  ** otherwise, we will remove GW out of PREF list.
  ** (Fusion now removed)
  */

  if(cmd->is_gwl_subsc_avail &&
      (sdss_is_sglte() ||
         sdss_is_1x_sxlte()) &&
       sdss_get_sim_state(SD_SS_HYBR_2) == SYS_SIM_STATE_NOT_AVAILABLE )
  {
    sd_si_update_sim_state( SYS_SIM_STATE_AVAILABLE, SD_SS_HYBR_2);
  }

  /*gw2 subs is sent available only when multisim,norm,norm */

  if(cmd->is_gw2_subsc_avail &&
     sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) &&
      !(sdss_is_sglte() || sdss_is_1x_sxlte())&&
     sdss_get_sim_state(SD_SS_HYBR_2) == SYS_SIM_STATE_NOT_AVAILABLE )
  {
    sd_si_update_sim_state( SYS_SIM_STATE_AVAILABLE, SD_SS_HYBR_2);

    /*Checks if GCF is enabled*/
    sdss_nv_init( SD_SS_HYBR_2 );
    
    /*Checks for setting scan_scope_rule*/
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Read scan_scope_rule from EFS
    */
    if(!sdss_is_gprs_anite_gcf(SD_SS_HYBR_2) && !sdss_is_scan_scope_rule_avail())
    {
      /*NV not configured*/
      if( (sdss_get_operator_name ( sdss_map_ss_to_as_id(SD_SS_HYBR_2) ) != SD_OPERATOR_CMCC)&& 
          sdss_is_usim_compliant_for_cmcc(sdss_map_ss_to_as_id(SD_SS_HYBR_2)))
      {
        scan_scope_rule_val = sdss_get_scan_scope_rule();
        scan_scope_rule_val = scan_scope_rule_val |
                              SYS_SCAN_SCOPE_RULE_ACQ_DB_RLF |
                              SYS_SCAN_SCOPE_RULE_ACQ_DB_PWR_UP |
                              SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS;
        sdss_set_scan_scope_rule(scan_scope_rule_val);
      }
    }
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  SD_MSG_HIGH_2("sdcmd_nam_sel3()-is_gw3_subsc_avail:%d,simstate:%d",cmd->is_gw3_subsc_avail,
                sdss_get_sim_state(SD_SS_HYBR_3));
  
  if(cmd->is_gw3_subsc_avail &&
     sdss_get_sim_state(SD_SS_HYBR_3) == SYS_SIM_STATE_NOT_AVAILABLE )
  {
    sd_si_update_sim_state( SYS_SIM_STATE_AVAILABLE, SD_SS_HYBR_3);
    /*Checks if GCF is enabled*/
    sdss_nv_init( SD_SS_HYBR_3 );

    /*Checks for setting scan_scope_rule*/
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Read scan_scope_rule from EFS
    */
    if(!sdss_is_gprs_anite_gcf(SD_SS_HYBR_3) && !sdss_is_scan_scope_rule_avail())
    {
      /*NV not configured*/
      as_id = (sdss_is_sxlte()) ? SYS_MODEM_AS_ID_2:SYS_MODEM_AS_ID_3;
      if( (sdss_get_operator_name ( sdss_map_ss_to_as_id(SD_SS_HYBR_3) ) != SD_OPERATOR_CMCC)&& 
          sdss_is_usim_compliant_for_cmcc(as_id))
      {
        scan_scope_rule_val = sdss_get_scan_scope_rule();
        scan_scope_rule_val = scan_scope_rule_val |
                              SYS_SCAN_SCOPE_RULE_ACQ_DB_RLF |
                              SYS_SCAN_SCOPE_RULE_ACQ_DB_PWR_UP |
                              SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS;
        sdss_set_scan_scope_rule(scan_scope_rule_val);
      }
    }
  }
  #endif

  /* else - if subscription not available  */
  if( !cmd->is_gwl_subsc_avail )
  {
    SD_MSG_HIGH_0("GW subs not available, set sim_state NOT AVAILABLE");
    sd_si_update_sim_state( SYS_SIM_STATE_NOT_AVAILABLE, SD_SS_MAIN );
    if(sdss_is_1x_sxlte())
    {
      sd_si_update_sim_state( SYS_SIM_STATE_NOT_AVAILABLE, SD_SS_HYBR_2);
    }
  }

  if(sdss_is_c2k_alt_algr_operation())
  {
    /* For ALT_CDMA_BSR feature,
    ** Need to re-read sdssscr configuration items upon going online
    ** BSR timers could be updated through OTA
    */
    sdssscr_efs_items_init();
    sdssscr_nv_items_init();
  }

  if( !cmd->is_gw2_subsc_avail &&
      (sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM)
      && !(sdss_is_sxlte())))
  {
    sd_si_update_sim_state( SYS_SIM_STATE_NOT_AVAILABLE, SD_SS_HYBR_2);
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if( !cmd->is_gw3_subsc_avail )
  {
    sd_si_update_sim_state( SYS_SIM_STATE_NOT_AVAILABLE, SD_SS_HYBR_3);
  }
  #endif
  #endif

  /* Inform the SD-PRL component of the new NAM selection. If NAM change
  ** is not successful, return FALSE
  */
  if( ! sdprl_nam_sel( cmd->curr_nam, cmd->hybr_pref ) )
  {
     if (si_ptr->si_prl_load_complete_cb_func != NULL)
     {
        si_ptr->si_prl_load_complete_cb_func(FALSE);
     }
     sdss_set_is_sdprl_failed(SD_SS_MAIN, TRUE);

     return FALSE;
  }

  if (si_ptr->si_prl_load_complete_cb_func != NULL)
  {
     si_ptr->si_prl_load_complete_cb_func(TRUE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If a new NAM is selected, call on the SS-Core to update its origination
  ** mode and SS-Preferences per the selected NAM's SS-Preferences.
  */
  if( cmd->curr_nam != SD_NAM_MAX )
  {
       int32 read_size = 0;
       sys_voice_domain_pref_e_type voice_domain_pref;
       read_size = sizeof (sys_voice_domain_pref_e_type);
       /* Read the EFS file  voice_domain_pref */
       if ( sdefs_get_per_subs("/nv/item_files/modem/mmode/voice_domain_pref",
                    (byte*)&voice_domain_pref,
                                 read_size,
                                 SYS_MODEM_AS_ID_1))
       {
         SD_MSG_HIGH_1("Domsel: Sub 0 read /nv/item_files/modem/mmode/voice_domain_pref = %d",\
                           voice_domain_pref );
       }
       else
       {
         /* Set default value for voice_domain_pref */
         #if (defined(FEATURE_3GPP_CSFB) && !defined (FEATURE_IP_CALL))
     
         voice_domain_pref = SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY;
     
         #elif (defined(FEATURE_3GPP_CSFB) && defined (FEATURE_IP_CALL) && defined (FEATURE_LTE))
     
         voice_domain_pref = SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED;
     
         #elif (!defined(FEATURE_3GPP_CSFB) && defined (FEATURE_IP_CALL) && defined (FEATURE_LTE))
     
         voice_domain_pref = SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY;
     
         #else
     
         voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NONE;
     
         #endif
     
         SD_MSG_HIGH_0("Domsel: Sub 0 read /nv/item_files/modem/mmode/voice_domain_pref failed");
       }

    SD_MSG_HIGH_2("MAIN_SD_NAM_SEL: mode_pref=0x%x, orig_mode=%d",
                  cmd->mode_pref, cmd->orig_mode);
    SD_MSG_HIGH_4("MAIN_SD_NAM_SEL: band_pref:(MSB)(LSB) cgw=0x%08x 0x%08x, TDS = 0x%08x 0x%08x",
                 QWORD_HIGH(cmd->band_pref),
                 QWORD_LOW(cmd->band_pref),
                 QWORD_HIGH(cmd->tds_band_pref),
                 QWORD_LOW(cmd->tds_band_pref));
    sd_print_lte_band_mask(cmd->lte_band_pref);
    SD_MSG_HIGH_5("MAIN_SD_NAM_SEL: domain_pref=%d, hybr_pref=%d user_mode_pref=0x%x, voice_domain_pref=%d, acq_order_pref=%d",
                  cmd->domain_pref, cmd->hybr_pref,cmd->mode_pref,voice_domain_pref, cmd->acq_order_pref);

    sdss_set_is_pref_update_called(SD_SS_MAIN, TRUE);
    (void)sdss_pref_update( SD_SS_PREF_REAS_USER,
                            cmd->orig_mode,
                            cmd->mode_pref,
                            cmd->band_pref,
                            cmd->lte_band_pref,
                            cmd->tds_band_pref,
                            cmd->prl_pref,
                            cmd->roam_pref,
                            cmd->hybr_pref,
                            SD_BAND_MAX,
                            SD_BLKSYS_MAX,
                            cmd->domain_pref,
                            cmd->acq_order_pref,
                            SD_SS_PREF_UPDATE_REASON_NONE,
                            cmd->mode_pref,
                            NULL,
                            SD_SS_PREF_CAMP_MODE_NO_CHANGE,
                            voice_domain_pref);

   sd_ss_ptr(SD_SS_MAIN)->is_start_instance = TRUE;

   #if defined FEATURE_HDR_HYBRID
   (void)sdss_hdr_pref_update( SD_SS_PREF_REAS_USER,
                               SD_SS_ORIG_MODE_COLLOC,
                               cmd->mode_pref,
                               cmd->band_pref,
                               cmd->prl_pref,
                               cmd->roam_pref,
                               cmd->hybr_pref,
                               cmd->domain_pref,
                               cmd->acq_order_pref
                             );
    sd_ss_ptr(SD_SS_HYBR_HDR)->is_start_instance = TRUE;

    /* Force the SDSR_HDR_XXX lists to reference the lists corresponding to
    **hybrid preference.
    */
    sdsr_lists_hybr_pref_switch( cmd->hybr_pref );
    #endif /* FEATURE_HDR_HYBRID */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if manual system selection is enabled.
  */
  if ( sdss_is_orig_mode0( cmd->orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL ) &&
       cmd->manual_sys_info_ptr != NULL &&
       cmd->manual_sys_info_ptr->sys_id.id_type == SYS_SYS_ID_TYPE_UMTS
     )
  {

    for( i=0 ; i<sdsr_list_cnt(SDSR_MANUAL_LST); i++ )
    {
      /* Fetch the user system record.
      */
      user_sys_ptr   = sdsr_list_get_ptr( sdss_sr_list_map2( SD_SS_MAIN, SDSR_MANUAL_LST), i );

      /* Manual selection is enabled.
      ** Update plmn fields with propreiate values.
      */

      if(user_sys_ptr == NULL)
      {
        SD_MSG_LOW_0("user_sys_ptr NULL");
        continue;
      }

      /* Check if it GWLT system.
      */
      if ( SD_MODE_CONTAIN( sdss_map_mode_to_mode_pref(user_sys_ptr->sys.mode),
                            SD_SS_MODE_PREF_GWL
                          ) ||
           SD_MODE_CONTAIN( sdss_map_mode_to_mode_pref(user_sys_ptr->sys.mode),
                            SD_SS_MODE_PREF_TDS
                          )
         )
      {

        switch ( cmd->manual_sys_info_ptr->prm.gw.sel_type )
        {
          case SYS_MANUAL_SEL_TYPE_HPLMN:
            user_sys_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_HPLMN;
            break;

          case SYS_MANUAL_SEL_TYPE_RPLMN:
            /* There is no requirement of power-on csg selection as of now,
               hence, no change for CSG is required till such a requirement arrives.
            */
            user_sys_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_MANUAL_SYS;
            sys_plmn_undefine_plmn_id( &user_sys_ptr->sid_expected.prm.gwl.plmn.plmn_id );
            break;

          case SYS_MANUAL_SEL_TYPE_USER_SPECIFIED:
            user_sys_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_MANUAL_SYS;
            user_sys_ptr->sid_expected.prm.gwl.plmn.plmn_id = cmd->manual_sys_info_ptr->sys_id.id.plmn;
            break;

          case SYS_MANUAL_SEL_TYPE_NONE:
          case SYS_MANUAL_SEL_TYPE_MAX:
          default:
            sys_err_fatal_invalid_value_exception((int)cmd->manual_sys_info_ptr->prm.gw.sel_type);

        } /* switch ( manual_sys_info_ptr->prm.gw.sel_type ) */
        user_sys_ptr->prl = (byte)SDSS_PRL_MOST_PREF;
      } /* ( if ( user_sys_ptr->sys.mode ... ) */

    } /* for ( ...  */

  } /*  if ( sdss_is_orig_mode( orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL) )  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the timer
  */
  sd_ss_timer_reset( SD_SS_MAIN );
  sd_ss_timer_reset( SD_SS_HYBR_HDR );

  
  /* Reset action */
  sd_ss_ptr(SD_SS_MAIN)->act.act = SD_SS_ACT_MAX;
  sd_ss_ptr(SD_SS_MAIN)->true_curr_act.act = SD_SS_ACT_MAX;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, NAM change is successful - return TRUE.
  */
  return TRUE;

} /* sd_nam_sel4 */


/*===========================================================================

FUNCTION sdcmd_hybr_per_ss_nam_sel

DESCRIPTION

===========================================================================*/
boolean                     sdcmd_hybr_per_ss_nam_sel(
      sd_nam_sel_cmd *cmd
)
{

  /* Point at manual system record.
  */
  sdsr_s_type             *user_sys_ptr        = NULL;
  sd_ss_mode_pref_e_type  mode_pref_on_ss_main = SD_SS_MODE_PREF_NONE;
  sd_ss_e_type ss                              = cmd->ss;
  sdsr_e_type             manual_lst = sdss_sr_list_map2(ss, SDSR_MANUAL_LST);
  int                     i;

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  uint16    scan_scope_rule_val;
  #endif
  sys_modem_as_id_e_type as_id  = sdss_map_ss_to_as_id(ss);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(cmd->curr_nam, SD_NAM_1, SD_NAM_MAX) );


  /*lint -save -e527 */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cmd->curr_nam != SD_NAM_MAX )
  {
    SD_ASSERT_ENUM_IS_INRANGE( cmd->orig_mode, SD_SS_ORIG_MODE_MAX );
    SD_ASSERT( cmd->orig_mode != SD_SS_ORIG_MODE_OTASP );

    SD_ASSERT_ENUM_IS_INRANGE( cmd->mode_pref, SD_SS_MODE_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( cmd->band_pref, SD_SS_BAND_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( cmd->tds_band_pref, SD_SS_BAND_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( cmd->roam_pref, SD_SS_ROAM_PREF_MAX );
    SD_ASSERT_ENUM_IS_INRANGE( cmd->hybr_pref, SD_SS_HYBR_PREF_MAX );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the subscription status in SS-Core.
  ** This portion has to be done first as SD-NV operation depended on
  ** subscription status.
  */

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sdss_subsc_avail_update2( SD_SS_MAIN,
                           cmd->is_gwl_subsc_avail,
                           cmd->is_1x_subsc_avail,
                           cmd->is_gw2_subsc_avail,
                           cmd->is_gw3_subsc_avail
                         );
  #else
  sdss_subsc_avail_update( SD_SS_MAIN,
                           cmd->is_gwl_subsc_avail,
                           cmd->is_1x_subsc_avail,
                           cmd->is_gw2_subsc_avail
                         );
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sdss_subsc_avail_update2( SD_SS_HYBR_2,
                           cmd->is_gwl_subsc_avail,
                           cmd->is_1x_subsc_avail,
                           cmd->is_gw2_subsc_avail,
                           cmd->is_gw3_subsc_avail
                         );
  #else
  sdss_subsc_avail_update( SD_SS_HYBR_2,
                           cmd->is_gwl_subsc_avail,
                           cmd->is_1x_subsc_avail,
                           cmd->is_gw2_subsc_avail
                         );
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sdss_subsc_avail_update2( SD_SS_HYBR_HDR,
                           cmd->is_gwl_subsc_avail,
                           cmd->is_1x_subsc_avail,
                           cmd->is_gw2_subsc_avail,
                           cmd->is_gw3_subsc_avail
                         );
  #else
  sdss_subsc_avail_update( SD_SS_HYBR_HDR,
                           cmd->is_gwl_subsc_avail,
                           cmd->is_1x_subsc_avail,
                           cmd->is_gw2_subsc_avail
                         );
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sdss_subsc_avail_update2( SD_SS_HYBR_3,
                           cmd->is_gwl_subsc_avail,
                           cmd->is_1x_subsc_avail,
                           cmd->is_gw2_subsc_avail,
                           cmd->is_gw3_subsc_avail
                         );
  #endif

  SD_MSG_HIGH_6("ss %d, is_1x_subsc_avail=%d, is_gwl_subsc_avail=%d is_sxlte %d, is_gw2_subsc_avail=%d, is_gw3_subsc_avail=%d",
                 ss,
                 cmd->is_1x_subsc_avail, 
                 cmd->is_gwl_subsc_avail,
                 sdss_is_sxlte(),
                 cmd->is_gw2_subsc_avail, 
                 cmd->is_gw3_subsc_avail 
                 );
                 
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)

  SD_MSG_HIGH_1("simstate: %d", sdss_get_sim_state(SD_SS_HYBR_2));

 
  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM))
  {
    /* Update the SIM state if GSM subscription is available
    */
  if (ss == SD_SS_HYBR_2 && !sdss_is_sxlte())
  {
    if( cmd->is_gw2_subsc_avail &&
       sdss_get_sim_state(SD_SS_HYBR_2) == SYS_SIM_STATE_NOT_AVAILABLE )
    {
      sd_si_update_sim_state( SYS_SIM_STATE_AVAILABLE, SD_SS_HYBR_2 );
      
      SD_MSG_HIGH_2("hybr_nam_sel()-is_gw2_subsc_avail:%d,simstate:%d",cmd->is_gw2_subsc_avail,
                sdss_get_sim_state(SD_SS_HYBR_2));
        /*Checks if GCF is enabled*/
        sdss_nv_init( SD_SS_HYBR_2 );
    
      /*Checks for setting scan_scope_rule*/
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Read scan_scope_rule from EFS
      */
      /* If EFS is not set but CMCC subsc available, then enable Acq DB except for GCF*/
      if(!sdss_is_gprs_anite_gcf(SD_SS_HYBR_2) && !sdss_is_scan_scope_rule_avail())
      {
        /*NV not configured*/
        if((sdss_get_operator_name ( sdss_map_ss_to_as_id(SD_SS_HYBR_2) ) != SD_OPERATOR_CMCC)&& 
            sdss_is_usim_compliant_for_cmcc(sdss_map_ss_to_as_id(ss)))
        {
          scan_scope_rule_val = sdss_get_scan_scope_rule();
          scan_scope_rule_val = scan_scope_rule_val |
                                SYS_SCAN_SCOPE_RULE_ACQ_DB_RLF |
                                SYS_SCAN_SCOPE_RULE_ACQ_DB_PWR_UP |
                                SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS;
          sdss_set_scan_scope_rule(scan_scope_rule_val);
        }
      }
    }
  }
  
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  else if (ss == SD_SS_HYBR_3)
  {
    SD_MSG_HIGH_2("hybr3_nam_sel()-is_gw3_subsc_avail:%d,simstate:%d",cmd->is_gw3_subsc_avail,
                sdss_get_sim_state(SD_SS_HYBR_3));

    if( cmd->is_gw3_subsc_avail &&
       sdss_get_sim_state(SD_SS_HYBR_3) == SYS_SIM_STATE_NOT_AVAILABLE )
    {
      sd_si_update_sim_state( SYS_SIM_STATE_AVAILABLE, SD_SS_HYBR_3 );
      /*Checks if GCF is enabled*/
      sdss_nv_init( SD_SS_HYBR_3 );
    
      /*Checks for setting scan_scope_rule*/
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Read scan_scope_rule from EFS
      */
      if(!sdss_is_gprs_anite_gcf(SD_SS_HYBR_3) && !sdss_is_scan_scope_rule_avail())
      {
        /*NV not configured*/
        as_id = (sdss_is_sxlte()) ? SYS_MODEM_AS_ID_2:SYS_MODEM_AS_ID_3;
        if((sdss_get_operator_name ( sdss_map_ss_to_as_id(SD_SS_HYBR_3) ) != SD_OPERATOR_CMCC)&& 
            sdss_is_usim_compliant_for_cmcc(as_id))
        {
          scan_scope_rule_val = sdss_get_scan_scope_rule();
          scan_scope_rule_val = scan_scope_rule_val |
                                SYS_SCAN_SCOPE_RULE_ACQ_DB_RLF |
                                SYS_SCAN_SCOPE_RULE_ACQ_DB_PWR_UP |
                                SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS;
          sdss_set_scan_scope_rule(scan_scope_rule_val);
        }
      }
    }
  }

  #endif

  /* In case the feature is SGLTE/SVLTE/SRLTE, for ss=SD_SS_HYBR_2, 
     we need not generate the GWL list */

   if(!((sdss_is_sglte() || sdss_is_1x_sxlte()) && ss == SD_SS_HYBR_2)) 
   { 
    if (!sdprl_nam_sel_hybr_ss(ss))
    {
      return FALSE;
    }
   }
  }

  /* Update the SIM state if GSM subscription is available
  */
  if( cmd->is_gwl_subsc_avail &&
      (sdss_is_1x_sxlte() ||
       sdss_is_sglte()) &&
     sdss_get_sim_state(SD_SS_HYBR_2) == SYS_SIM_STATE_NOT_AVAILABLE )
  {
    sd_si_update_sim_state( SYS_SIM_STATE_AVAILABLE, SD_SS_HYBR_2 );
  }
  #endif

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_LTE)
  if( !cmd->is_gwl_subsc_avail )
  {
    SD_MSG_HIGH_0("GW subs not available, set sim_state NOT AVAILABLE");
    sd_si_update_sim_state( SYS_SIM_STATE_NOT_AVAILABLE, SD_SS_MAIN );
    if(sdss_is_1x_sxlte())
    {
      sd_si_update_sim_state( SYS_SIM_STATE_NOT_AVAILABLE, SD_SS_HYBR_2);
    }
  }

  if( !cmd->is_gw2_subsc_avail &&
      (sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM)
      && !(sdss_is_sxlte())))
  {
    sd_si_update_sim_state( SYS_SIM_STATE_NOT_AVAILABLE, SD_SS_HYBR_2);
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if( !cmd->is_gw3_subsc_avail )
  {
    sd_si_update_sim_state( SYS_SIM_STATE_NOT_AVAILABLE, SD_SS_HYBR_3);
  }
  #endif
  #endif
  /* No NAM SEL for HYBR_2 as it will be done during NAM SEL of MAIN */

  /* Orig mode is Manual GW, Set mode pref to GW.
  ** This will change ORIG mode to EMERG_ORIG when GW subs is
  ** not avail in Manual mode
  */
  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) && 
  	(!sdss_is_sxlte() || ss == SD_SS_HYBR_3) &&
     sdss_is_orig_mode0( cmd->orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL ))
  {
    mode_pref_on_ss_main = SD_GET_COMMON_MODE(cmd->mode_pref, SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE);
  }
  else
  {
    mode_pref_on_ss_main = cmd->mode_pref;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If a new NAM is selected, call on the SS-Core to update its origination
  ** mode and SS-Preferences per the selected NAM's SS-Preferences.
  */
  if( cmd->curr_nam != SD_NAM_MAX )
  {
     int32 read_size = 0;
     sys_voice_domain_pref_e_type voice_domain_pref;
     read_size = sizeof (sys_voice_domain_pref_e_type);
     /* Read the EFS file  voice_domain_pref */
     if(sdefs_get_per_subs("/nv/item_files/modem/mmode/voice_domain_pref",
                  (byte*)&voice_domain_pref,
                             read_size,
                             as_id))
     {
       SD_MSG_HIGH_2("Domsel: Sub %d read /nv/item_files/modem/mmode/voice_domain_pref = %d",\
                       as_id, voice_domain_pref );
     }
     else
     {
       /* Set default value for voice_domain_pref */
       #if (defined(FEATURE_3GPP_CSFB) && !defined (FEATURE_IP_CALL))
     
       voice_domain_pref = SYS_VOICE_DOMAIN_PREF_CS_VOICE_ONLY;
     
       #elif (defined(FEATURE_3GPP_CSFB) && defined (FEATURE_IP_CALL) && defined (FEATURE_LTE))
     
       voice_domain_pref = SYS_VOICE_DOMAIN_PREF_CS_VOICE_PREFERRED;
     
       #elif (!defined(FEATURE_3GPP_CSFB) && defined (FEATURE_IP_CALL) && defined (FEATURE_LTE))
     
       voice_domain_pref = SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY;
     
       #else
     
       voice_domain_pref = SYS_VOICE_DOMAIN_PREF_NONE;
     
       #endif
     
       SD_MSG_HIGH_1("Domsel: Sub %d read /nv/item_files/modem/mmode/voice_domain_pref failed", as_id);
    }
   
   SD_MSG_HIGH_3("HYBR_SD_NAM_SEL: ss=%d, mode_pref=0x%x, orig_mode=%d",
                 ss, mode_pref_on_ss_main, cmd->orig_mode);
   SD_MSG_HIGH_4("HYBR_SD_NAM_SEL: band_pref:(MSB) cgw=0x%08x 0x%08x, TDS = 0x%08x 0x%08x",
                QWORD_HIGH(cmd->band_pref),
                QWORD_LOW(cmd->band_pref),
                QWORD_HIGH(cmd->tds_band_pref),
                QWORD_LOW(cmd->tds_band_pref));
   sd_print_lte_band_mask(cmd->lte_band_pref);
   SD_MSG_HIGH_5("HYBR_SD_NAM_SEL: domain_pref=%d, hybr_pref=%d user_mode_pref=0x%x,voice_domain_pref=%d, acq_order_pref=%d",
                 cmd->domain_pref, cmd->hybr_pref,mode_pref_on_ss_main,voice_domain_pref, cmd->acq_order_pref);
   
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if (ss == SD_SS_HYBR_3)
    {
      (void)sdss_hybr_3_pref_update( SD_SS_PREF_REAS_USER,
                            cmd->orig_mode,
                            mode_pref_on_ss_main,
                            cmd->band_pref,
                            cmd->lte_band_pref,
                            cmd->tds_band_pref,
                            cmd->prl_pref,
                            cmd->roam_pref,
                            cmd->hybr_pref,
                            SD_BAND_MAX,
                            SD_BLKSYS_MAX,
                            cmd->domain_pref,
                            cmd->acq_order_pref,
                            SD_SS_PREF_UPDATE_REASON_NONE,
                            mode_pref_on_ss_main,
                            voice_domain_pref,
                            NULL
                          );
  }
  else
    #endif
  {
    (void)sdss_hybr_2_pref_update( SD_SS_PREF_REAS_USER,
                            cmd->orig_mode,
                            mode_pref_on_ss_main,
                            cmd->band_pref,
                            cmd->lte_band_pref,
                            cmd->tds_band_pref,
                            cmd->prl_pref,
                            cmd->roam_pref,
                            cmd->hybr_pref,
                            SD_BAND_MAX,
                            SD_BLKSYS_MAX,
                            cmd->domain_pref,
                            cmd->acq_order_pref,
                            SD_SS_PREF_UPDATE_REASON_NONE,
                            mode_pref_on_ss_main,
                            voice_domain_pref,
                            NULL
                          );

      }
   sd_ss_ptr(ss)->is_start_instance = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if manual system selection is enabled.
  */
  if ( sdss_is_orig_mode0( cmd->orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL ) &&
       cmd->manual_sys_info_ptr != NULL &&
       cmd->manual_sys_info_ptr->sys_id.id_type == SYS_SYS_ID_TYPE_UMTS
     )
  {

    for( i=0 ; i<sdsr_list_cnt(manual_lst); i++ )
    {
      /* Fetch the user system record.
      */
      user_sys_ptr   = sdsr_list_get_ptr(manual_lst, i );

      /* Manual selection is enabled.
      ** Update plmn fields with propreiate values.
      */

      if(user_sys_ptr == NULL)
      {
        SD_MSG_LOW_0("user_sys_ptr NULL");
        continue;
      }

      /* Check if it GWL system.
      */
      if ( SD_MODE_CONTAIN(sdss_map_mode_to_mode_pref(user_sys_ptr->sys.mode),
                           SD_SS_MODE_PREF_GWL
                          )
         )
      {

        switch ( cmd->manual_sys_info_ptr->prm.gw.sel_type )
        {
          case SYS_MANUAL_SEL_TYPE_HPLMN:
            user_sys_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_HPLMN;
            break;

          case SYS_MANUAL_SEL_TYPE_RPLMN:
            /* There is no requirement of power-on csg selection as of now,
               hence, no change for CSG is required till such a requirement arrives.
            */
            user_sys_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_MANUAL_SYS;
            sys_plmn_undefine_plmn_id( &user_sys_ptr->sid_expected.prm.gwl.plmn.plmn_id );
            break;

          case SYS_MANUAL_SEL_TYPE_USER_SPECIFIED:
            user_sys_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_MANUAL_SYS;
            user_sys_ptr->sid_expected.prm.gwl.plmn.plmn_id = cmd->manual_sys_info_ptr->sys_id.id.plmn;
            break;

          case SYS_MANUAL_SEL_TYPE_NONE:
          case SYS_MANUAL_SEL_TYPE_MAX:
          default:
            sys_err_fatal_invalid_value_exception((int)cmd->manual_sys_info_ptr->prm.gw.sel_type);

        } /* switch ( manual_sys_info_ptr->prm.gw.sel_type ) */
        user_sys_ptr->prl = (byte)SDSS_PRL_MOST_PREF;
      } /* ( if ( user_sys_ptr->sys.mode ... ) */

    } /* for ( ...  */

  } /*  if ( sdss_is_orig_mode( orig_mode, SD_SS_ORIG_MODE_VAL_MANUAL) )  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset the timer
  */
  sd_ss_timer_reset( ss );


    /* Reset action */
  sd_ss_ptr(ss)->act.act = SD_SS_ACT_MAX;
  sd_ss_ptr(ss)->true_curr_act.act = SD_SS_ACT_MAX;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, NAM change is successful - return TRUE.
  */
  return TRUE;
  /*lint -restore */

} /* sdcmd_hybr_per_ss_nam_sel */


/*===========================================================================

FUNCTION sdcmd_is_plmn_rat_allowed

DESCRIPTION

===========================================================================*/
boolean sdcmd_is_plmn_rat_allowed (
      sd_is_plmn_rat_allowed_cmd  *cmd
)
{
  sdsr_s_type         *acq_sys_ptr = sdsr_list_get_ptr(sdss_sr_list_map2(cmd->ss,SDSR_ACQ_SYS), 0 );  //used for input plmn
  sdsr_s_type         *start_sys_ptr = sdsr_list_get_ptr(sdss_sr_list_map2(SD_SS_MAIN,SDSR_START_SYS), 0);  //sys used to compare with on main
  sdsr_s_type         start_sys;
  sdsr_s_type         *start_sys_ptr_local = NULL; //local copy of start_sys
  sdsr_s_type         *start_sys_ptr_hybr = sdsr_list_get_ptr(sdss_sr_list_map2(SD_SS_HYBR_1,SDSR_START_SYS), 0);//sys used to compare with on hybrid stack
  sd_ss_act_s_type    act;
  sd_ss_e_type        ss = cmd->ss;
  sdssscr_acq_e_type  acq_type = sdss_eng_stack_get_acq_type(ss);
  sys_rat_pri_list_info_s_type    ss_rat_pri_list_info;
  sdsr_s_type *sys_ptr = NULL; 
  sdsr_s_type *temp_sys_ptr = NULL; 
  sdsr_s_type *sys_ptr_hybr = NULL;
  sys_srv_status_e_type     ss_srv_status = SYS_SRV_STATUS_NO_SRV;
  sd_si_info_s_type *si_info_ptr = &sd_si_ptr(SD_SS_MAIN)->si_info;
  sd_si_info_s_type *hdr_si_info_ptr = &sd_si_ptr(SD_SS_HYBR_1)->si_info;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Removing variable 'word geo_sys_idx = 0xFFFF;' at places marked with
     #------# for storing the return value from sdefs_write function */

  /* #------# */
  
  SD_MSG_HIGH_5("is_plmn_allow: plmn_id=0x%x 0x%x 0x%x mode=%d, ss=%d",
                                                    cmd->plmn_id.identity[0],
                                                    cmd->plmn_id.identity[1],
                                                    cmd->plmn_id.identity[2],
                                                    cmd->mode, ss);
 

  if(!sdss_is_mmss_operation(ss))
  {
    SD_MSG_MED_0("is_plmn_allow: query not needed in non-MMSS opt");
    return TRUE;
  }

  if( sd_ss_act_get_curr_act_ss(ss,&act) == SD_SS_ACT_MAX )
  {
    SD_MSG_HIGH_1("is_plmn_allow: ss %d",ss);
    return TRUE;
  }

  if(act.act != SD_SS_ACT_ACQ_GWL)
  {
    SD_MSG_HIGH_0("is_plmn_allow: srv req type allow");
    return TRUE;
  }

  sdsr_list_print(sdss_sr_list_map2(ss,SDSR_ACQ_SYS), FALSE);
  sdsr_list_print(sdss_sr_list_map2(SD_SS_MAIN,SDSR_START_SYS), FALSE);
  sdsr_list_print(sdss_sr_list_map2(SD_SS_HYBR_1,SDSR_START_SYS), FALSE);
  sdsr_list_print(sdss_sr_list_map2(SD_SS_MAIN,SDSR_SRV_SYS), FALSE);
  sdsr_list_print(sdss_sr_list_map2(SD_SS_HYBR_1,SDSR_SRV_SYS), FALSE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* get the acq sys and fill in the plmn id
  */
  if(acq_sys_ptr == NULL)
  {
    SD_MSG_HIGH_0("is_plmn_allow: ACQ sys empty");
    return TRUE;
  }

  /* Get local copy of start_sys */
  sdsr_list_get_cpy( sdss_sr_list_map2(SD_SS_MAIN,SDSR_START_SYS), 0, &start_sys );
  start_sys_ptr_local = &start_sys;
  
  *(&(acq_sys_ptr->sid.plmn.plmn_id)) = *(&(cmd->plmn_id));
  if( acq_sys_ptr->sys.mode == SD_MODE_GWL )
  {
     acq_sys_ptr->sid.gw.mode = cmd->mode;
  }
  /* update prl desig value for the acq sys
  */
  sdprl_get_info_base( ss,
                       sdss_sr_list_map2( ss, SDSR_ACQ_SYS),
                       0,
                       sdss_sr_list_map2( ss, SDSR_ACQ_SYS),
                       0,
                       SDSR_MAX,
                       sdss_sr_list_map2( ss, SDSR_ACQ_PREF_LST),  
                       SDSR_MAX,
                       SDPRL_EXTRACT_ALL,
                       NULL);

  sdsr_list_print(SDSR_ACQ_PREF_LST, FALSE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* if the input plmn is same as start sys, return TRUE
  */
  SD_MSG_MED_3("acq_sys plmn_id %d %d %d",
                 acq_sys_ptr->sid.plmn.plmn_id.identity[0],
                 acq_sys_ptr->sid.plmn.plmn_id.identity[1],
                 acq_sys_ptr->sid.plmn.plmn_id.identity[2]);
  sdsr_list_print(SDSR_START_PREF_LST, FALSE);
  if(start_sys_ptr != NULL)
  {
    SD_MSG_MED_4(" start_sys mode=%d start_sys plmn_id %d %d %d",
				 start_sys_ptr->sys.mode,
                 start_sys_ptr->sid.plmn.plmn_id.identity[0],
                 start_sys_ptr->sid.plmn.plmn_id.identity[1],
                 start_sys_ptr->sid.plmn.plmn_id.identity[2]);
 
    
  }
  if(start_sys_ptr_hybr != NULL)
  {
    SD_MSG_MED_4("hybr start_sys plmn_id %d %d %d hybr start_sys mode=%d",
                   start_sys_ptr_hybr->sid.plmn.plmn_id.identity[0],
                   start_sys_ptr_hybr->sid.plmn.plmn_id.identity[1],
                   start_sys_ptr_hybr->sid.plmn.plmn_id.identity[2],
                   start_sys_ptr_hybr->sys.mode);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( sdss_is_splmn_support_enabled() &&
      sdss_get_srv_req_type(ss) == SD_SS_SRV_REQ_TYPE_BSR &&
      acq_type == SDSSSCR_ACQ_MORE_PREF &&
      sdss_is_resel_state(ss,SDSS_RESEL_STT_SRCH) == TRUE &&
      sdss_is_splmn(cmd->plmn_id)
    )
  {
    SD_MSG_HIGH_0( "is_plmn_allow: is_plmn_allowed_by_splmn_support is false ...");
	return FALSE;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ss == SD_SS_HYBR_2 && sdss_is_1x_sxlte() )
  {
    /* make sure we use SDSR_SRV_SYS only when it is valid.
    ** since in below cases where SDSR_SRV_SYS may not have valid 1x sys
    ** 1) when 1x is still searching
    ** 2) when 1x sys found is a less prefered sys , and SD is performing more pref scan
    */
    temp_sys_ptr = sdsr_list_get_ptr(sdss_sr_list_map2(SD_SS_MAIN,SDSR_SRV_SYS), 0);  
    if (si_info_ptr->srv_status == SYS_SRV_STATUS_SRV && 
        sd_get_ss_state(SD_SS_MAIN) == SDSS_STATE_OPR &&
        temp_sys_ptr != NULL &&
        temp_sys_ptr->sys.mode == SD_MODE_CDMA)
    {
      sys_ptr = temp_sys_ptr;  
    }
    else if(hdr_si_info_ptr->srv_status == SYS_SRV_STATUS_NO_SRV_INTERNAL )
    {
      sys_ptr_hybr = sdsr_list_get_ptr(sdss_sr_list_map2(SD_SS_HYBR_1,SDSR_SRV_SYS), 0);
    }
  
    if(sys_ptr != NULL)
    {
      if(!sdprl_is_more_pref_upto_2(SD_SS_HYBR_2, SD_SS_MAIN, acq_sys_ptr, sys_ptr,SDPRL_COMP_UPTO_GEO_POS))
      {
        SD_MSG_HIGH_0("is_plmn_allow: less pref than 1x");
        return FALSE;
      }
    }
    if(sys_ptr_hybr != NULL)
    {
      if(!sdprl_is_more_pref_upto_2(SD_SS_HYBR_2, SD_SS_HYBR_1, acq_sys_ptr, sys_ptr_hybr,SDPRL_COMP_UPTO_GEO_POS))
      {
        SD_MSG_HIGH_0("is_plmn_allow: less pref than DO");
        return FALSE;
      }
    }
    if ( hdr_si_info_ptr->srv_status != SYS_SRV_STATUS_NO_SRV_INTERNAL) 
      {
        sdss_get_rat_pri_list_info(ss, &ss_rat_pri_list_info);

        /* Update more_pref_do list if not in resel state and HDR is mode pref 
        */
        if( sdss_get_rat_pri_sys_idx( ss, SD_MODE_HDR ) != ss_rat_pri_list_info.num_items )
        {
        
          sdsr_generate_more_pref_hdr_lst(ss, SDSR_ACQ_PREF_LST);
          
          sdss_read_srv_status( ss, &ss_srv_status, NULL );

          /** Check if there are more preferred DO systems and SD is not already doing 
          ** L to DO BSR scans (resel state is not MORE_PREF_HDR_SCAN) 
          ** --> Return FALSE so that immediate LTE to DO BSR can be done.
          ** In order to reacq LTE sys after HDR fail, ACQ type should be allowed 
          */
          if ((sdsr_list_cnt(SDSR_MORE_PREF_HDR_LST) > 0) &&
            !sdss_is_resel_state( ss,SDSS_RESEL_STT_MORE_PREF_HDR_SCAN) &&
            !sdss_is_lte_connected_mode(ss) && 
			  acq_type != SDSSSCR_ACQ_ALLOWED  )
          {
            sd_ss_event_proc(ss, SDSS_EVT_MISC_LTE_TO_HDR_BSR, &act );
            SD_MSG_HIGH_0("is_plmn_allow: MORE pref HDR SCAN ");
            return FALSE;
          }
          else
          {
            SD_MSG_HIGH_1("is_plmn_allow: MORE pref HDR LST is %d ",sdsr_list_cnt(SDSR_MORE_PREF_HDR_LST));
            return TRUE;
          }
        }
        else
        {
          SD_MSG_HIGH_0( "is_plmn_allow: HDR not in UE cap do not initiated lte to HDR bsr ...");
          return TRUE;
        }
      }
    SD_MSG_HIGH_0("is_plmn_allow: return TRUE for hybr_2");
    return TRUE;
  }

  if(start_sys_ptr != NULL )
  {
    SD_MSG_HIGH_0("is_plmn_allow: use start_sys to compare");
  }
  else
  {
    SD_MSG_HIGH_0("is_plmn_allow: start sys is NULL: return TRUE");
    return TRUE;
  }
  
  /* on hybr_2, if acq sys  is more preferred than start sys, return TRUE 
  ** can't do the same for main in case med pref sys was found
  L1
  1x
  acq (L2)
  1x
  start(L3)
  However if main start_sys doesnt have 1x sys , then we shouldnt compare it 
  */
  if(ss == SD_SS_HYBR_2 
     && 
      (start_sys_ptr->sys.mode != SD_MODE_CDMA ||
       sdprl_is_more_pref_upto_2(ss, SD_SS_MAIN, acq_sys_ptr, start_sys_ptr,SDPRL_COMP_UPTO_LAST))
    )
  {
    SD_MSG_HIGH_1("is_plmn_allow: TRUE %d",start_sys_ptr->sys.mode);
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Handling main stack
  */
  if(ss == SD_SS_MAIN)
  {
  
    boolean     is_no_pref_pp2 = FALSE;
    /* If no preferred 3gpp2 systems in preferred list */

    boolean     is_pref_lst_scanned = FALSE;
    /* If all the systems in acq pref list is scanned */

    boolean     is_lst_scanned = FALSE;
    /* If there are systems in acq pref list whose new acq cnt is met */

    boolean     is_acq_sys_more_preferred = FALSE;
    /* If the 3gpp acq sys is more preferred that the current system */
    
    /* if more pref list doesn't have 3gpp2 sys, return TRUE */
    if((sdsr_list_cnt(SDSR_ACQ_PREF_LST) == 0 ||
       (sdsr_list_find_sys_mode(sdss_sr_list_map2( ss, SDSR_ACQ_PREF_LST), SD_MODE_CDMA) == SDSR_POS_NULL &&
        sdsr_list_find_sys_mode(sdss_sr_list_map2( ss, SDSR_ACQ_PREF_LST), SD_MODE_HDR) == SDSR_POS_NULL)))
    {
       is_no_pref_pp2 = TRUE;
    }

    /* Populate the CDMA systems more preferred than the current GWL system
    ** being scanned.
    */
    sdsr_list_cpy(SDSR_BACKUP_LST, SDSR_ACQ_PREF_LST);
    sdsr_list_del_sys(ss, SDSR_BACKUP_LST, SDSR_CAT_GWL_SYS);
    sdsr_list_del_sys(ss, SDSR_BACKUP_LST, SDSR_CAT_NON_COMPLY );

    if( start_sys_ptr != NULL )
    {
      sdsr_list_del_list(SDSR_BACKUP_LST,
                         sdss_sr_list_map2(SD_SS_MAIN,SDSR_START_SYS));
    }

    if(sdsr_list_cnt(SDSR_BACKUP_LST)> 0)
    {
	  is_lst_scanned = sdss_is_list_scanned(ss,SDSR_BACKUP_LST);
	  SD_MSG_HIGH_1("is_plmn_allow: is_lst_scanned %d",is_lst_scanned);
    }
    else
    {
      SD_MSG_HIGH_0("is_plmn_allow: ACQ pref list is empty");
      is_lst_scanned = TRUE;
    }

    /* Populate the CDMA systems already scanned in the BSR so far.
    */
    sdsr_list_cpy(SDSR_TEMP2_LST, SDSR_MORE_PREF_SCANNED_LST);
    sdsr_list_del_sys(ss, SDSR_TEMP2_LST, SDSR_CAT_GWL_SYS);
    sdsr_list_del_sys(ss, SDSR_TEMP2_LST, SDSR_CAT_NON_COMPLY );

    sdsr_list_print(SDSR_BACKUP_LST, FALSE);
    sdsr_list_print(SDSR_TEMP2_LST, FALSE);

    /* Ensure that the SCAN_PREF_LST contains all the systems from
    ** ACQ_PREF_LST. So all the CDMA systems more preferred than current GWL
    ** system have already been scanned.
    */
    if ( sdsr_list_is_contain( SDSR_TEMP2_LST,
                               SDSR_CONTAIN_ALL,
                               SDSR_BACKUP_LST )
                            )
     {
       is_pref_lst_scanned = TRUE;
     }

    SD_MSG_MED_3("is_pref_lst_scanned:%d,is_no_pref_pp2:%d,acq_type:%d",is_pref_lst_scanned,is_no_pref_pp2,acq_type);

    /* When start sys and acq sys point to same system record, start_sys_ptr->sid.plmn.plmn_id will be 
    ** updated when updating received plmn_id in acq_sys_ptr as they point to same record
    ** In this case :
    ** 1.If they lie in same GEO, we need to compare relative priority from PLMN_ID
    **   So, use start_sys_ptr_local (local copy) so acq_sys_ptr and start_sys_ptr_local
    **   will have different plmn_id.
    ** 2.If they do not lie in same GEO, we need to compare relative position in PRL
    **   Use start_syts_ptr (pointer to actual record) for this as we need to find 
    **   reference for start_sys in TBL_LST 
    */
    if(acq_sys_ptr == start_sys_ptr && start_sys_ptr_local != NULL )
    {
      SD_MSG_HIGH_0("is_plmn_allow: acq_sys and satrt_sys point to same SR");
      is_acq_sys_more_preferred = sdprl_is_more_pref_upto_3(ss, SD_SS_MAIN, acq_sys_ptr, start_sys_ptr, start_sys_ptr_local, SDPRL_COMP_UPTO_LAST);
    }
    else
    {
    is_acq_sys_more_preferred = sdprl_is_more_pref_upto_2(ss, SD_SS_MAIN, acq_sys_ptr, start_sys_ptr,SDPRL_COMP_UPTO_LAST);
    }

    SD_MSG_HIGH_1("is_plmn_allow: is_acq_sys_more_preferred %d",is_acq_sys_more_preferred);

    /* The current GW system is more preferred than the start system and all
    ** the 3GPP2 systems more preferred than it have been already scanned.
    */
    if( is_acq_sys_more_preferred
        &&
     (is_no_pref_pp2 || is_pref_lst_scanned || is_lst_scanned || 
        ( (acq_type != SDSSSCR_ACQ_MORE_PREF_RECURSIVE) && (acq_type != SDSSSCR_ACQ_BEST_PREF) && 
        (acq_type != SDSSSCR_ACQ_REG_BEST_PREF) && (acq_type != SDSSSCR_ACQ_MORE_PREF)) )
      )
    {
      SD_MSG_MED_0("is_plmn_allow: preferred sys, return TRUE");
      return TRUE;
    }      

    /* if acq_type is not BEST_PREF/BEST_PREF_REG, allow camp */
    if(acq_type != SDSSSCR_ACQ_BEST_PREF &&
      acq_type != SDSSSCR_ACQ_REG_BEST_PREF &&
      acq_type != SDSSSCR_ACQ_MORE_PREF && 
      acq_type != SDSSSCR_ACQ_MORE_PREF_RECURSIVE)
    {
       SD_MSG_MED_1("is_plmn_allow: allow sys, acq_type=%d",acq_type);
       return TRUE;
    }

    /* When acq type is MORE PREF dont do recursive BSR if the acq sys is more preferred 
    ** than the current system
    */
    if(is_acq_sys_more_preferred && (acq_type == SDSSSCR_ACQ_MORE_PREF) && 
       (sdss_eng_stack_get_state(SD_SS_MAIN) == SDSS_ACQ_STT_START) )
    {
      SD_MSG_HIGH_2("is_plmn_allow: more pref sys acq type %d state %d", 
        acq_type, sdss_eng_stack_get_state(SD_SS_MAIN));
      return TRUE;
    }

    SD_MSG_MED_3("ss:%d,is_srv_on:%d,state:%d",ss,sd_si_is_srv_on(ss),sdss_eng_stack_get_state(ss));

    /*Condition to do recursive BSR(sdss_acq_event_proc_internal)
         1.  If acq sys is more preferred than start sys
         2.  Acq pref list is not empty and not scanned. More preferred 3GPP2 systems are available and not yet scanned.
         3.  Acq type is BEST_PREF/REG_BEST_PREF/MORE_PREF/MORE_PREF_RECURSIVE
         
         One exception to avoid recursive BSR for acq_type MORE_PREF
         1.  When acquired system is more preferred return TRUE.
    */

    if (is_acq_sys_more_preferred)
    {
      if(sdss_acq_event_proc_internal())
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      SD_MSG_MED_0("is_plmn_allow: Skip rec bsr as acq sys plmn is less preferred.");
      return FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_MSG_HIGH_0("is_plmn_allow: FALSE");
  return FALSE;
}


/*===========================================================================

FUNCTION sdcmd_user_nam_sel

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  NAM selection or information.

DEPENDENCIES
  sd_init() and must have already been called to initialize SD.

  sd_nam_sel() must have already been called to load the selected NAM's PRL
  from NV.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

===========================================================================*/
sd_ss_act_e_type          sdcmd_user_nam_sel(
  sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If operating mode is not online, call the power up indication.
  */
  if ( sdss_get_opr_mode( SD_SS_MAIN ) != SDSS_OPR_MODE_ONLINE )
  {
    sd_ss_ind_user_pwr_up_cmd pwr_up_cmd;

    pwr_up_cmd.is_cdma_unlock = FALSE;
    pwr_up_cmd.rtrn_act_ptr = cmd->rtrn_act_ptr;

    SD_MSG_HIGH_0("Converting nam_sel to pwr_up SS-Ind");
    act = sdcmd_user_pwr_up(&pwr_up_cmd);
    return act;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_NAM_SEL, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL  |
                    SD_SS_ACT_ACQ_GPS  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_nam_sel */


/*===========================================================================

FUNCTION sdcmd_user_pwr_down

DESCRIPTION
  Inform SD that the user commanded the MS power-off, perform a reset or
  enter LPM.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

===========================================================================*/
sd_ss_act_e_type          sdcmd_user_pwr_down(
  sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Deactivate SS-Timer??
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Call on the SS-Core to update its operating mode.
  */
  sdss_opr_mode_update(SD_SS_MAIN, SDSS_OPR_MODE_PWR_DOWN );

  #if defined (FEATURE_SD_LTE)

  sd_si_update_nas_addl_info( SD_SS_MAIN, SYS_LTE_CS_CAPABILITY_NONE, SYS_EXTEND_SRV_INFO_NONE);
  sdss_reset_nas_addl_info(SD_SS_MAIN);
  #endif

  #ifdef FEATURE_HDR
  if(sd_efs_is_loc_bsr_enabled())
  {
    sdss_reset_loc_bsr();
  }
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform the System Indication component that the MS is powering down.
  */
  sd_si_srv_offline( SDSS_OPR_MODE_PWR_DOWN );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save PRL-Component related information to NV.
  */
  sdprl_save();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset is_in_srv_lost_state upon pwr_down
  */
  sdss_reset_srv_lost_state(SD_SS_MAIN);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_USER_PWR_DOWN, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act == SD_SS_ACT_CONTINUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_pwr_down */


/*===========================================================================

FUNCTION sdcmd_user_term_get_net_gw_ss

DESCRIPTION
  Inform SD that the user commanded to terminate the previous get networks
  command.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_GWL  -  Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sdcmd_user_term_get_net_gw_ss(
  sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act = SD_SS_ACT_CONTINUE;
  sd_ss_e_type        ss = cmd->ss;
  sd_ss_s_type        *ss_ptr       = sd_ss_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the response is pending.
  */
  if ( ss_ptr->is_get_net_done )
  {
    SD_MSG_HIGH_0( "No get networks response pending");

    /* No response is pending, If SS-Client supplied a buffer for the
    ** return SS-Action, set the returned SS-Action in this buffer to
    ** CONTINUE.
    */
    if( cmd->rtrn_act_ptr != NULL )
    {
      cmd->rtrn_act_ptr->act = SD_SS_ACT_CONTINUE;
    }

    return SD_SS_ACT_CONTINUE;


  } /* if ( ss_ptr->is_get_net_done ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set get networks done flag to TRUE.
  */
  ss_ptr->is_get_net_done = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( ss )
  {
    case SD_SS_MAIN:
  /* Process the SS-Event that is associated with this SS-Indication.
  */
      act = sd_ss_event_proc(ss, SDSS_EVT_USER_TERM_GET_NET_GW, cmd->rtrn_act_ptr );
      break;

    case SD_SS_HYBR_2:
    #if defined(FEATURE_MMODE_DUAL_SIM) || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_2, SDSS_EVT_HYBR_2_USER_TERM_GET_NET_GW, cmd->rtrn_act_ptr );
    #endif
      break;

    case SD_SS_HYBR_3:
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Process the SS Event that is associated with thsi SS-Indication.
      */
      act = sd_ss_event_proc( SD_SS_HYBR_3, SDSS_EVT_HYBR_3_USER_TERM_GET_NET_GW, cmd->rtrn_act_ptr );
    #endif
      break;


    case SD_SS_HYBR_1:
    case SD_SS_MAX:
    default:
      SD_ERR_1(" Invalid ss %d",ss);
      act = SD_SS_ACT_CONTINUE;
      break;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_ACQ_GPS  |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_user_term_get_net_gw() */


/*===========================================================================

FUNCTION sdcmd_cdma_opr_ndss_off

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS receives a
  CDMA NDSS off message.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK- Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

===========================================================================*/
EXTERN  sd_ss_act_e_type          sdcmd_cdma_opr_ndss_off(
        sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sdss_is_c2k_alt_algr_operation() &&
     (sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE) == SD_SS_MAIN))
  {
    SD_MSG_HIGH_0("reset tele bsr due to redir");
    sdss_lte_telescope_parm_reset(SD_SS_MAIN);
    sdss_1x_telescope_parm_reset();
  }

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_CDMA_OPR_NDSS_OFF, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_MEAS_BACK|
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_cdma_opr_ndss_off */


/*===========================================================================

FUNCTION sdcmd_misc_cnf

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS confirms
  the last instructed action like pwr_scan_gw

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   - Acquire a GSM/WCDMA/LTE/TD-SCDMA system as specified
                          by the returned "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sdcmd_misc_cnf(
  sd_cmd_t  *cmd
)
{
  sd_ss_act_e_type    act;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the SS-Event that is associated with this SS-Indication.
  */
  act = sd_ss_event_proc( SD_SS_MAIN, SDSS_EVT_MISC_CNF, cmd->rtrn_act_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the returned value is one of the allowed SS-Actions for this
  ** SS-Indication.
  */
  SD_ASSERT( act & (SD_SS_ACT_CONTINUE |
                    SD_SS_ACT_ACQ_CDMA |
                    SD_SS_ACT_ACQ_AMPS |
                    SD_SS_ACT_ACQ_HDR  |
                    SD_SS_ACT_ACQ_GWL   |
                    SD_SS_ACT_MEAS_DED |
                    SD_SS_ACT_PWR_SAVE ) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return act;

} /* sd_ss_ind_misc_cnf */
/**===========================================================================

@FUNCTION sdcmd_update_cdma_splmn_list

@DESCRIPTION
CDMA updates the splmn list to SD.
calls in sdss to process the splmn info

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  HDR OH info must have already been sent by DO.

@RETURN VALUE
None

@SIDE EFFECTS
  None.

===========================================================================*/
void  sdcmd_update_cdma_splmn_list
(
   sd_ss_ind_update_splmn_list_cmd  *cmd
)
{
   if(cmd == NULL)
   {
     SD_ERR_0( "C2k_resel_splmn:cmd is null" );
     return;
   }  
   SD_MSG_HIGH_2( "C2k_resel_splmn:Received SPLMN list from 1x eutra_freqs = %d, SPLMNs = %d", 
   	            cmd->splmn_info_ptr->num_eutra_freq, cmd->splmn_info_ptr->num_splmn);
   
   sdss_splmn_info_proc(cmd->splmn_info_ptr,TRUE);

   
}
/**===========================================================================

@FUNCTION sdcmd_update_hdr_splmn_list

@DESCRIPTION
HDR updates the splmn list to SD.
calls in sdss to process the splmn info

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  HDR OH info must have already been sent by DO.

@RETURN VALUE
None

@SIDE EFFECTS
  None.

===========================================================================*/
void sdcmd_update_hdr_splmn_list
(
   sd_ss_ind_update_splmn_list_cmd  *cmd
)
{

   if(cmd == NULL)
   {
     SD_ERR_0( "C2k_resel_splmn:cmd is null");
     return;
   }
   
   SD_MSG_HIGH_2( "C2k_resel_splmn:Received SPLMN list from HDR eutra_freqs = %d, SPLMNs = %d", 
   	             cmd->splmn_info_ptr->num_eutra_freq, cmd->splmn_info_ptr->num_splmn);

   sdss_splmn_info_proc(cmd->splmn_info_ptr,FALSE);

}

/*===========================================================================

FUNCTION sdcmd_mcfg_refresh

DESCRIPTION
  SD refreshes NV/EFS items based on MCFG update

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void          sdcmd_mcfg_refresh(

   sd_ss_mcfg_refresh_cmd     *cmd 

 )
{
  #ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* read policyman config
  */
  sdss_read_policyman_config_per_sub(cmd->as_id, FALSE);
  sdss_read_policyman_tot();
  sdefs_nv_refresh();
  sdssscr_efs_items_init();
  sdssscr_nv_items_init();
  sdss_lbsr_mcc_items_init();
  sdss_1xcsfb_ecbm_init();
  sdss_1xcsfb_call_end_opt_init();
  sdss_common_nv_refresh(BM(cmd->as_id));

  /* if MCFG refresh is not on SIM 1, that means 
  lte_cs_cap should not be reset even in common
  as LTE is only on SIM for now, later fix may have to be extended 
  if LTE can be there on other SLOTS as well*/
  if (cmd->as_id != SYS_MODEM_AS_ID_1) 
  { 
    sd_ss_e_type lte_ss = sdss_get_lte_idle_ss();
	sys_lte_cs_capability_e_type lte_cs_capability;
    sys_extend_srv_info_e_type ext_srv_info;
    
	if((lte_ss == SD_SS_MAIN) || ((lte_ss == SD_SS_HYBR_2)))
	{
	/* get LTE stacks lte_cs_cap from Slot1*/
    sdss_get_nas_addl_info(lte_ss, &lte_cs_capability, &ext_srv_info);

	/* set common lte cs capability */
	  sdss_set_lte_cs_cap(lte_cs_capability);
    }
  }
  
  /* If as id is 1, refresh all stacks associated with it
  */
  if (cmd->as_id == SYS_MODEM_AS_ID_1) 
  {

    sdss_init(SD_SS_MAIN);
    sdsr_tbl_reset_per_ss(SD_SS_MAIN);
    sdss_init(SD_SS_HYBR_1);
    sdsr_tbl_reset_per_ss(SD_SS_HYBR_1);
    if (sdss_is_sfmode_1x_sxlte(0) || 
        sdss_is_sglte() )
    {
      sdss_init(SD_SS_HYBR_2);
      sdsr_tbl_reset_per_ss(SD_SS_HYBR_2);
    }
  }
  /* If as id is 2, refresh all stacks associated with it
  */
  if (cmd->as_id == SYS_MODEM_AS_ID_2)
  {
    if (sdss_is_sxlte())
    {
      sdss_init(SD_SS_HYBR_3);
      sdsr_tbl_reset_per_ss(SD_SS_HYBR_3);
    }
    else
    {
      sdss_init(SD_SS_HYBR_2);
      sdsr_tbl_reset_per_ss(SD_SS_HYBR_2);
    }
  }
  /* If as id is 3, refresh all stacks associated with it
  */
  if (cmd->as_id == SYS_MODEM_AS_ID_3) 
  {
    sdss_init(SD_SS_HYBR_3);
    sdsr_tbl_reset_per_ss(SD_SS_HYBR_3);
  }
  SD_MSG_MED_2("MCFG:refresh complete %d %d",cmd->reference_id, cmd->as_id);
  mcfg_refresh_done(cmd->reference_id);
  #endif
}
