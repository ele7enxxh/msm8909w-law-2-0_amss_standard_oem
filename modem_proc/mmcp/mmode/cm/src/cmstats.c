/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  C A L L   M A N A G E R   S T A T S  M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager Modem Statistics module.

  The Statistics object is responsible for:
  1. Processing client's required statistics information
  2. Processing modem statistics information received from protocol
     and passing it to clients

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2009 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmstats.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/16/14   jvo     Enforce ASSERTs with runtime input validation
12/20/13   jvo     Add FEATURE_SEGMENT_LOADING support to fix crash
09/04/13   mh/jvo  Mainline the modem statistics code
03/23/12   gm      DIME integration fix: TMC deprecation and others.
02/15/12   vk      Replace usage of AEE library function with corresponding
                   CoreBSP library
01/10/12   gm      Mainlining HDR API change integration
11/02/11   gm      Changes for HDR API cleanup
05/26/11   rm      Adding cmstats_map_cmapi_to_cm_sys_mode
05/19/11   rm      LTE Modem statistics
04/11/11   am      Cleaning stubs and runnning qtf SUTE
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
07/21/10   rm      Adding hopping status changes
07/14/10   mh      Fixed compile warnings
05/24/10   rm      Correcting featurisation of FEATURE_MODEM_STATISTICS
04/29/10   rm      Adding Modem Statistics changes for 1x/HDR
04/01/10   rm      Keeping GSM and WCDMA defintions separately
02/03/10   rm      Making certain statistics implementaion under GW feature
12/14/09   rm      Fixed memset issue
12/10/09   rm      Correcting F3 message
11/18/09   rm      Adding NMR/MRL support
10/28/09   rm      Added missing F3 messages
10/09/09   rm      Initial release

===========================================================================*/



/****************************************************************************

 Organization of the file:

    The file is divided into multiple sections.
    You can jump from one sections to the other by searching for / followed
    by 2 *'s. The order of objects defined is as follows:

        includes
        forward declarations
        #defines

        functions
        - Internal
        - External

****************************************************************************/

#include "mmcp_variation.h"
#include "customer.h"
/*lint -esym(766,customer.h)*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "comdef.h"
#include "cmdbg.h"
#include "cmtaski.h"
#include "cmclient.h"
#include "cmstats.h"
#include "cm_msgr_msg.h"
#include "cmi.h"

#ifdef CM_GW_SUPPORTED
#include "mm.h"
#endif
#ifdef FEATURE_GSM
#include "geran_eng_mode_read_api.h"
#endif
#include "sd.h"
#include "bit.h"
#include "cmph.h"
#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "mccdma_v.h"    /* Interface to current base station */
#endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */
#ifdef FEATURE_HDR
#include "hdrcp_msg.h"
#include "hdrcp_api.h"
#include "dsmgr_api.h" /* Interface to DS */
#endif /* FEATURE_HDR */
#ifdef FEATURE_CM_LTE
#include "cmapi.h"    /* Interface to LTE */
#endif

#include "cmmsc.h"
#include "cmsimcoord.h"

#ifdef CM_DEBUG
#error code not present
#endif

#ifdef FEATURE_SEGMENT_LOADING
#include "IWCDMA.h"
extern interface_t *pi_mmode_wcdma;
#endif

#include <string.h>

#include <stringl/stringl.h>

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/
/* Bit mask values */
/* Umbrella feature guard for bit mask varaiables */
#ifdef CM_GW_SUPPORTED
static uint64  nas_bit_mask;
#ifdef FEATURE_GSM
static uint64  gsm_bit_mask;
#endif /* FEATURE_GSM */
#ifdef FEATURE_WCDMA
static uint64  wcdma_bit_mask;
#endif /* FEATURE_WCDMA */
#endif /* FEATURE_GSM || FEATURE_WCDMA */
#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
static uint64  cdma_bit_mask;
#endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */
#ifdef FEATURE_HDR
static uint64  ds_bit_mask;
#endif /* FEATURE_HDR */
#ifdef FEATURE_CM_LTE
static uint64  lte_bit_mask = 0;
#endif /* FEATURE_CM_LTE */


/* CM variable to store global bit mask */
static uint32  global_cm_bit_mask = 0;
/* Number of time user has pressed 'send' key */
static uint16 num_orig_count = 0;

#ifdef CM_FTD_DEBUG
#error code not present
#endif

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
** Functions - internal, common
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION  cmstats_client_cmd_check

DESCRIPTION

  Check for stats command parameter errors.

DEPENDENCIES


RETURN VALUE
  CM_STATS_CMD_ERR_NOERR if input parameters are
  correct, specific cm_call_cmd_err_e_type otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static cm_stats_cmd_err_e_type  cmstats_client_cmd_check(

    cm_stats_cmd_s_type          *stats_cmd_ptr
        /* Pointer to a CM command */
)
/*lint -esym(528, cmstats_client_cmd_check) not referenced */
{

  cm_stats_cmd_err_e_type       cmd_err       = CM_STATS_CMD_ERR_NOERR;
    /* Command error (if any) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Runtime checks made by cmstats_client_cmd_proc */
  CM_ASSERT( stats_cmd_ptr           != NULL );
  CM_ASSERT( stats_cmd_ptr->cmd_type == CM_CMD_TYPE_STATS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether module specified is correct.
  */
  if (! BETWEEN (stats_cmd_ptr->info.module,
                 SYS_MODEM_MODULE_NONE,
                 SYS_MODEM_MODULE_MAX))
  {
    cmd_err = CM_STATS_CMD_ERR_INVALID_TYPE;
  }

  /* If this is for wcdma under segment loading feature,
  ** check if wcdma is loaded.
  */
  #ifdef FEATURE_SEGMENT_LOADING
  if(stats_cmd_ptr->info.module == SYS_MODEM_MODULE_WCDMA &&
     pi_mmode_wcdma == NULL)
  {
      /* If WCDMA isn't loaded, we shouldn't process commands for it */
      CM_ERR_0("cmstats - pi_mmode_wcdma is NULL");
  
      /* Report this error back to clients */
      cmd_err = CM_STATS_CMD_ERR_NOT_LOADED;
  }
  #endif

  return cmd_err;
} /* cmstats_client_cmd_check() */

/*===========================================================================

FUNCTION cmstats_client_cmd_err

DESCRIPTION
  Notify clients of a specified stats command error.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmstats_client_cmd_err(

    const cm_stats_cmd_s_type       *stats_cmd_ptr,
        /* Pointer to a CM command */

    cm_stats_cmd_err_e_type    cmd_err
        /* Indicate the phone command error */
)
/*lint -esym(528, cmstats_client_cmd_err) not referenced */
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Runtime null and type checks made by cmstats_client_cmd_proc */
  CM_ASSERT( stats_cmd_ptr != NULL );
  CM_ASSERT( stats_cmd_ptr->cmd_type == CM_CMD_TYPE_STATS );
  CM_ASSERT( BETWEEN( cmd_err, CM_STATS_CMD_ERR_NONE, CM_STATS_CMD_ERR_MAX) );

  if ( ! BETWEEN( cmd_err, CM_STATS_CMD_ERR_NONE, CM_STATS_CMD_ERR_MAX) )
  {
    CM_MSG_HIGH_1("Invalid error type %d", cmd_err);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( stats_cmd_ptr->cmd_cb_func != NULL )
  {
    stats_cmd_ptr->cmd_cb_func( stats_cmd_ptr->data_block_ptr,
                                stats_cmd_ptr->cmd,
                                cmd_err );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If command error is OTHER than NO ERROR,
  ** notify the client list of command error, as well.
  */
  if( cmd_err != CM_STATS_CMD_ERR_NOERR )
  {
    CM_MSG_HIGH_3( "Stats cmd err!, cmd=%d, err=%d, client=%ld",
                 stats_cmd_ptr->cmd, cmd_err, stats_cmd_ptr->client_id );

    cmclient_list_stats_cmd_err_ntfy( cmd_err, stats_cmd_ptr );
  }
} /* cmstats_client_cmd_err() */


/*===========================================================================

FUNCTION cmstats_map_cm_prl_pref_to_sys_prl_pref

DESCRIPTION
  Maps CM prl pref to sys prl pref

DEPENDENCIES
  None

RETURN VALUE
  sys_stats_prl_pref_e_type

SIDE EFFECTS
  None

===========================================================================*/
sys_stats_prl_pref_e_type cmstats_map_cm_prl_pref_to_sys_prl_pref (

  cm_prl_pref_e_type  cm_prl_pref
  /* CM PRL preference */
)
/*lint -esym(765, cmstats_map_cm_prl_pref_to_sys_prl_pref) */
{
  sys_stats_prl_pref_e_type sys_prl_pref = SYS_STATS_PRL_PREF_NONE;

  switch (cm_prl_pref)
  {
    case CM_PRL_PREF_NONE:
     sys_prl_pref = SYS_STATS_PRL_PREF_NONE;
     break;

   case CM_PRL_PREF_AVAIL_BC0_A:
     sys_prl_pref = SYS_STATS_PRL_PREF_AVAIL_BC0_A;
     break;

   case CM_PRL_PREF_AVAIL_BC0_B:
     sys_prl_pref = SYS_STATS_PRL_PREF_AVAIL_BC0_B;
     break;

   case CM_PRL_PREF_ANY:
     sys_prl_pref = SYS_STATS_PRL_PREF_ANY;
     break;

   case CM_PRL_PREF_NO_CHANGE:
     sys_prl_pref = SYS_STATS_PRL_PREF_NO_CHANGE;
     break;

   case CM_PRL_PREF_PERSISTENT:
     sys_prl_pref = SYS_STATS_PRL_PREF_PERSISTENT;
     break;

   case CM_PRL_PREF_MAX:
     sys_prl_pref = SYS_STATS_PRL_PREF_MAX;
     break;

   #ifdef FEATURE_RPC
   case CM_PRL_PREF_BIG:
     sys_prl_pref = SYS_STATS_PRL_PREF_BIG;
     break;
   #endif

   default:
     CM_MSG_HIGH_1( "Invalid prl pref %d",
                    cm_prl_pref );

  }
  return sys_prl_pref;
}

#if (defined FEATURE_CM_LTE)
/*===========================================================================

FUNCTION cmstats_map_cmapi_to_cm_sys_mode

DESCRIPTION
  Maps CMAPI sys mode to sys mode

DEPENDENCIES
  None

RETURN VALUE
  sys_stats_prl_pref_e_type

SIDE EFFECTS
  None

===========================================================================*/
static sys_sys_mode_e_type cmstats_map_cmapi_to_cm_sys_mode (

  cmapi_sys_mode_e_type  cmapi_sys_mode
  /* CMAPI sys mode */
)
{
  sys_sys_mode_e_type sys_mode = SYS_SYS_MODE_NONE;

  switch (cmapi_sys_mode)
  {
    case CMAPI_SYS_MODE_CDMA:
      sys_mode = SYS_SYS_MODE_CDMA;
      break;
    case CMAPI_SYS_MODE_HDR:
      sys_mode = SYS_SYS_MODE_HDR;
      break;
    case CMAPI_SYS_MODE_GSM:
      sys_mode = SYS_SYS_MODE_GSM;
      break;
    case CMAPI_SYS_MODE_WCDMA:
      sys_mode = SYS_SYS_MODE_WCDMA;
      break;
    case CMAPI_SYS_MODE_LTE:
      sys_mode = SYS_SYS_MODE_LTE;
      break;
    default:
      CM_MSG_HIGH_1( "Invalid CMAPI sys mode %d",
                     cmapi_sys_mode );
  }
  return sys_mode;
}
#endif

/*===========================================================================

FUNCTION cmstats_process_cm_stats_mask

DESCRIPTION
  Process bit_mask set request that has come to CM.
  CM will fill the statistics structures for all the required bit masks
  set by the user.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmstats_process_cm_stats_mask(

  uint32  bit_mask
  /* CM bit mask that is set */
)
/*lint -esym(528, cmstats_process_cm_stats_mask) not referenced */
{
  sys_modem_stats_info_s_type   *stats_info_ptr;

  /*
  ** Dynamically allocate a stats object
  */
  stats_info_ptr = (sys_modem_stats_info_s_type *)
                          cm_mem_malloc ( sizeof(sys_modem_stats_info_s_type));

  /* Initialise CM Stats variables */
  stats_info_ptr->type                        = SYS_MODEM_MODULE_CM;
  stats_info_ptr->prm.cm_info.changed_bitmask = SYS_CM_INFO_MASK_NONE;


  /* Check the bit mask value and act accordingly */

  /* Get phone related information */
  if ( bit_mask & SYS_CM_PH_PREF_BIT_MASK )
  {

   /* Preferred system */
   /*lint -e666 */
   stats_info_ptr->prm.cm_info.ph_info.pref_sys =
           cmstats_map_cm_prl_pref_to_sys_prl_pref
                                        (CMPH_PRST_PRL_PREF( cmph_ptr() ) );

   /* Phone mode */
   /*lint -e666 */
   stats_info_ptr->prm.cm_info.ph_info.mode_pref =
          (sys_stats_mode_pref_e_type) (CMPH_PRST_MODE_PREF ( cmph_ptr() ));

   /* Band capability */
   stats_info_ptr->prm.cm_info.ph_info.band_cap = cmph_ptr()->band_capability;

   /* Set changed_fields */
   stats_info_ptr->prm.cm_info.changed_bitmask |=
                                  SYS_CM_PH_PREF_BIT_MASK;

  }

  /* Get system selection related info */
  if ( bit_mask & SYS_CM_SYS_SEL_INFO_BIT_MASK )
  {

   /* Selected system */
   *(&(stats_info_ptr->prm.cm_info.sys_sel_info.sel_sys)) = *(&(cmss_ptr()->info.sys_id));

   /* Raoming indication */
   stats_info_ptr->prm.cm_info.sys_sel_info.roam_ind =
                     (sys_stat_roam_e_type) cmss_ptr()->info.roam_status;

   /* Set changed_fields */
   stats_info_ptr->prm.cm_info.changed_bitmask |=
                                  SYS_CM_SYS_SEL_INFO_BIT_MASK;
  }

  /* Get PRL related info */
  if ( bit_mask & SYS_CM_PRL_INFO_BIT_MASK )
  {
    /* PRL version */
    stats_info_ptr->prm.cm_info.prl_ver = sd_misc_prl_ver_get();

    /* Set changed_fields */
    stats_info_ptr->prm.cm_info.changed_bitmask |=
                                  SYS_CM_PRL_INFO_BIT_MASK;
  }

  /* Get Orig info */
  if ( bit_mask & SYS_CM_CALL_STAT_BIT_MASK )
  {
    /* Number of originations */
    stats_info_ptr->prm.cm_info.send_key_orig_count = num_orig_count;

    /* Set changed_fields */
    stats_info_ptr->prm.cm_info.changed_bitmask |=
                                  SYS_CM_CALL_STAT_BIT_MASK;
  }

  if (stats_info_ptr->prm.cm_info.changed_bitmask != SYS_CDMA_INFO_MASK_NONE )
  {
    /* Print all the information. For Debug purpose */
    #ifdef CM_FTD_DEBUG
    #error code not present
#endif

    /* Send event to clients */
    cm_client_stats_event_ntfy ( CM_STATS_EVENT_MODEM_INFO,
                                 stats_info_ptr);
  }

  /* Deallocate the memory after sending the stats event. */
  cm_mem_free(stats_info_ptr);

}


/*===========================================================================

FUNCTION cmstats_client_cmd_proc

DESCRIPTION
  Process clients' STATS data commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmstats_client_cmd_proc(

  cm_stats_cmd_s_type  *stats_cmd_ptr
    /* pointer to a CM command
    */
)
/*lint -esym(714, cmstats_client_cmd_proc) */
/*lint -esym(715, cmd_ptr) */
/*lint -esym(765, cmd_ptr) */
/*lint -esym(818, cmd_ptr)*/
{

  cm_stats_cmd_err_e_type     stats_cmd_err = CM_STATS_CMD_ERR_NONE;
  cm_stats_cmd_info_s_type    *cmd_info_ptr;
  #ifdef FEATURE_GSM
  geran_eng_mode_info_bitmask_t geran_bit_mask;
  #endif

  CM_ASSERT( stats_cmd_ptr != NULL );
  CM_ASSERT( stats_cmd_ptr->cmd_type == CM_CMD_TYPE_STATS );

  if ( stats_cmd_ptr == NULL )
  {
    CM_ERR_0("stats_cmd_ptr is NULL!");
    return;
  }

  if ( stats_cmd_ptr->cmd_type != CM_CMD_TYPE_STATS )
  {
    CM_ERR_0("Non-stats command received!");
    return;
  }

  /* Start by checking whether this is a valid command
  */
  stats_cmd_err = cmstats_client_cmd_check( stats_cmd_ptr );

  if (stats_cmd_err != CM_STATS_CMD_ERR_NOERR)
  {
    CM_MSG_HIGH_1("stats cmd err %d", stats_cmd_err);
        cmstats_client_cmd_err( stats_cmd_ptr, stats_cmd_err );
    return;
  }

  cmd_info_ptr  = CMD_INFO_PTR (stats_cmd_ptr);

  /* Set asubs_id to default value for single sim */
  if (cmph_is_ssim())
  {
    stats_cmd_ptr->asubs_id = SYS_MODEM_AS_ID_1;
  }
  
  if(!(stats_cmd_ptr->asubs_id > SYS_MODEM_AS_ID_NONE &&
      stats_cmd_ptr->asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    CM_MSG_HIGH_1("Incorrect asubs_id %d, default to AS_ID_1",stats_cmd_ptr->asubs_id);
    stats_cmd_ptr->asubs_id = SYS_MODEM_AS_ID_1;
  }

  /* Fill the appropriate bit mask values and inform
  ** protocol layers.
  */
  switch (cmd_info_ptr->module)
  {
    #ifdef CM_GW_SUPPORTED
    case SYS_MODEM_MODULE_NAS:
     /* Copy NAS bit mask */
     nas_bit_mask = cmd_info_ptr->bit_mask;
     
     /* Inform NAS */
     mm_per_stacks_set_nas_bit_mask((uint16)nas_bit_mask, stats_cmd_ptr->asubs_id,SYS_MODEM_STACK_ID_1);
     CM_MSG_HIGH_1("stats: NAS bit mask set, asubs_id %d", stats_cmd_ptr->asubs_id);

     break;

    #ifdef FEATURE_GSM
    case SYS_MODEM_MODULE_GSM:
     /* Copy GSM bit mask */
     gsm_bit_mask = cmd_info_ptr->bit_mask;
     geran_bit_mask.geran_modem_stats_mask = gsm_bit_mask;
     #ifdef FEATURE_MMODE_DUAL_SIM
     /* Inform RR */
     geran_multisim_eng_mode_set_modem_stats_mask( geran_bit_mask, stats_cmd_ptr->asubs_id);
     CM_MSG_HIGH_1("stats: GSM bit mask set, asubs_id %d",  stats_cmd_ptr->asubs_id);
     #else
     /* Inform RR */
     geran_eng_mode_set_modem_stats_mask( geran_bit_mask );
     CM_MSG_HIGH_1("stats: GSM bit mask set - %d", gsm_bit_mask);
     #endif
     break;
     #endif /* FEATURE_GSM */

    #ifdef FEATURE_WCDMA
    case SYS_MODEM_MODULE_WCDMA:
     /* Copy WCDMA bit mask */
     wcdma_bit_mask = cmd_info_ptr->bit_mask;

     /* Inform RRC */

      /* For segment loading, we already checked if wcdma is loaded when
      ** when we checked the command for errors.
      */
      #ifdef FEATURE_SEGMENT_LOADING
      IWCDMA_wcdma_set_modem_stats_mask (pi_mmode_wcdma,
                                         (uint16)wcdma_bit_mask );
      #else
     wcdma_set_modem_stats_mask ((uint16)wcdma_bit_mask );
      #endif

     CM_MSG_HIGH_1("stats: WCDMA bit mask set - %d", wcdma_bit_mask);
     break;
    #endif /* FEATURE_WCDMA */
    #endif /* FEATURE_GSM || FEATURE_WCDMA */

    case SYS_MODEM_MODULE_CM:
      /* Copy CM/SD bit mask */
      global_cm_bit_mask = (uint32) cmd_info_ptr->bit_mask;
      CM_MSG_HIGH_1("stats: CM bit mask set - %d", global_cm_bit_mask);
      /* Process CM request */
      cmstats_process_cm_stats_mask( global_cm_bit_mask );
      break;

    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    case SYS_MODEM_MODULE_CDMA:
     /* Copy CDMA bit mask */
     cdma_bit_mask = cmd_info_ptr->bit_mask;
     #ifndef FEATURE_MMODE_REMOVE_1X
     /* Inform CDMA 1x protocol*/
     mcc_set_cdma_stats_mask ( (uint32) cdma_bit_mask );
     CM_MSG_HIGH_1("stats: CDMA bit mask set - %d", cdma_bit_mask);
     #else
     #error code not present
#endif
     break;
    #endif

   #ifdef FEATURE_HDR
   case SYS_MODEM_MODULE_HDR:
    {
      hdrcp_msg_req_u       *hdrcp_req;


      hdrcp_req = (hdrcp_msg_req_u*) cm_mem_malloc(sizeof(hdrcp_msg_req_u));

      hdrcp_req->ftd_stats.bit_mask = cmd_info_ptr->bit_mask;

      CM_MSG_HIGH_1("stats: HDR bit mask set - %d", cmd_info_ptr->bit_mask);
      (void) cm_msgr_send( HDR_CP_SET_FTD_STATS_MASK_REQ, MSGR_MM_CM, &(hdrcp_req->hdr), sizeof(hdrcp_msg_req_u) );

      cm_mem_free(hdrcp_req);
    }
    break;

   case SYS_MODEM_MODULE_DS:
     /* Copy DS bit mask */
     ds_bit_mask = cmd_info_ptr->bit_mask;
     /* Inform CDMA 1x protocol*/
     ds3g_update_stats_mask ((uint16) ds_bit_mask );
     CM_MSG_HIGH_1("stats: DS bit mask set - %d", ds_bit_mask);
     break;
    #endif /* FEATURE_HDR */

   #ifdef FEATURE_CM_LTE
   case SYS_MODEM_MODULE_LTE:
     /* Copy LTE bit mask */
     lte_bit_mask = cmd_info_ptr->bit_mask;

     /* Inform LTE protocol*/
     CM_MSG_HIGH_1("stats: LTE bit mask set - %d", lte_bit_mask);
     break;
   #endif /* FEATURE_CM_LTE */

   default:
     CM_MSG_HIGH_0("Invalid module type");
     return;

  }

}
/*lint +esym(715, cmd_ptr) */
/*lint +esym(818, cmd_ptr)*/

#ifdef CM_GW_SUPPORTED
/*===========================================================================

FUNCTION cmstats_process_nas_stats_per_subs_stack_info

DESCRIPTION
  Read updated stats info from NAS and send teh data to clients.
  Report CM_STATS_EVENT_MODEM_INFO to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmstats_process_nas_stats_per_subs_stack_info(

  sys_modem_as_id_e_type    asubs_id,
  sys_modem_stack_id_e_type stack_id
)
{

  sys_modem_stats_info_s_type    *stats_info_ptr;

  /*
  ** Dynamically allocate a stats object
  */
  stats_info_ptr = (sys_modem_stats_info_s_type *)
                          cm_mem_malloc ( sizeof(sys_modem_stats_info_s_type));

  stats_info_ptr->type = SYS_MODEM_MODULE_NAS;

  /* Set asubs_id to default value for single sim */
  if (cmph_is_ssim())
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }

  CM_MSG_HIGH_2("calling mm_per_stacks_get_nas_stats_info asubs_id %d, stack_id %d",asubs_id, stack_id);
  stats_info_ptr->asubs_id = asubs_id;
  if (mm_per_stacks_get_nas_stats_info( &(stats_info_ptr->prm.nas_info), asubs_id, stack_id )!= TRUE)
  {
    CM_ERR_0("Can't get NAS statistics");
    /* Deallocate the memory */
    cm_mem_free( stats_info_ptr );
  }

  /* Print all the information. For Debug purpose */
  #ifdef CM_FTD_DEBUG
  #error code not present
#endif

  /* Send event to clients */
  cm_client_stats_event_ntfy ( CM_STATS_EVENT_MODEM_INFO,
                               stats_info_ptr);


  /* Deallocate the memory after sending the ph event. */
  cm_mem_free(stats_info_ptr);

}

/*===========================================================================

FUNCTION cmstats_process_nas_stats_info

DESCRIPTION
  Read updated stats info from NAS and send teh data to clients.
  Report CM_STATS_EVENT_MODEM_INFO to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmstats_process_nas_stats_info(

  sys_modem_as_id_e_type    asubs_id
)
{

  sys_modem_stats_info_s_type    *stats_info_ptr;

  /*
  ** Dynamically allocate a stats object
  */
  stats_info_ptr = (sys_modem_stats_info_s_type *)
                          cm_mem_malloc ( sizeof(sys_modem_stats_info_s_type));

  stats_info_ptr->type = SYS_MODEM_MODULE_NAS;

  /* Set asubs_id to default value for single sim */
  if (cmph_is_ssim())
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }

  stats_info_ptr->asubs_id = asubs_id;
  if (mm_per_stacks_get_nas_stats_info( &(stats_info_ptr->prm.nas_info), asubs_id,SYS_MODEM_STACK_ID_1 )!= TRUE)
  {
    CM_ERR_0("Can't get NAS statistics");
    /* Deallocate the memory */
    cm_mem_free( stats_info_ptr );
  }

  /* Print all the information. For Debug purpose */
  #ifdef CM_FTD_DEBUG
  #error code not present
#endif

  /* Send event to clients */
  cm_client_stats_event_ntfy ( CM_STATS_EVENT_MODEM_INFO,
                               stats_info_ptr);


  /* Deallocate the memory after sending the ph event. */
  cm_mem_free(stats_info_ptr);

}

#ifdef FEATURE_GSM
/*===========================================================================

FUNCTION cmstats_process_gsm_stats_info

DESCRIPTION
  Read updated stats info from NAS and send teh data to clients.
  Report CM_STATS_EVENT_MODEM_INFO to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmstats_process_gsm_stats_info(

  sys_modem_as_id_e_type    asubs_id
)
{

  sys_modem_stats_info_s_type    *stats_info_ptr;
  geran_eng_mode_modem_stats_t   *gsm_info_ptr;

  /* Temp pointers to copy data */
  sys_gsm_information_s_type  *info_ptr;
  int i;

  /*------------------------------------------------------------------*/

  if (!(asubs_id > SYS_MODEM_AS_ID_NONE && asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    CM_MSG_HIGH_1("Warning: Invalid as_id %d", asubs_id);
    return;
  }

  /*
  ** Dynamically allocate a stats object
  */
  stats_info_ptr = (sys_modem_stats_info_s_type *)
                          cm_mem_malloc ( sizeof(sys_modem_stats_info_s_type));

  /*
  ** Dynamically allocate memory for GSM object
  */
  gsm_info_ptr = (geran_eng_mode_modem_stats_t *)
                          cm_mem_malloc ( ( asubs_id + 1 ) * sizeof(geran_eng_mode_modem_stats_t));
  #ifdef FEATURE_MMODE_DUAL_SIM
  stats_info_ptr->asubs_id  = asubs_id;
  /* Call NAS function to copy all updated fields */
  geran_multisim_eng_mode_data_read ( ENG_MODE_GERAN_MODEM_STATS_INFO,
                             (void *) gsm_info_ptr,
                             asubs_id);
  #else
  /*------------------------------------------------------------------*/

  /* Call NAS function to copy all updated fields */
  geran_eng_mode_data_read ( ENG_MODE_GERAN_MODEM_STATS_INFO,
                             (void *) gsm_info_ptr );
  #endif
  /*------------------------------------------------------------------*/

  stats_info_ptr->type = SYS_MODEM_MODULE_GSM;

  info_ptr = &(stats_info_ptr->prm.gsm_info);

  /* Copy data from recd from GSM */
  /* bit mask value */
  info_ptr->changed_bitmask   =
                    gsm_info_ptr->updated_info.geran_modem_stats_mask;

  /* timing advance value */
  info_ptr->timing_advance_value  = gsm_info_ptr->timing_advance_value;

  /* NMR info */
  info_ptr->nmr_info.num_of_items = gsm_info_ptr->nmr_info.num_of_items;

  CM_ASSERT (info_ptr->nmr_info.num_of_items <= SYS_ENG_MODE_MAX_NMR_ITEMS);

  CM_CHECK_MAX_AND_DISCARD_ITEMS(info_ptr->nmr_info.num_of_items,
                                 SYS_ENG_MODE_MAX_NMR_ITEMS,
                                 "NMR")

  memscpy( info_ptr->nmr_info.time_stamp,
           sizeof(info_ptr->nmr_info.time_stamp),
           gsm_info_ptr->nmr_info.time_stamp,
           sizeof(time_type) );

  /* Initiliase NMR elemnt type to 0xFF */
  memset (info_ptr->nmr_info.nmr,
          0xFF,
          sizeof (info_ptr->nmr_info.nmr));

  for (i = 0 ; i< info_ptr->nmr_info.num_of_items; i++ )
  {
    info_ptr->nmr_info.nmr[i].arfcn = gsm_info_ptr->nmr_info.nmr[i].arfcn;
        info_ptr->nmr_info.nmr[i].bsic  = gsm_info_ptr->nmr_info.nmr[i].bsic;
    info_ptr->nmr_info.nmr[i].rxlev = gsm_info_ptr->nmr_info.nmr[i].rxlev;
    info_ptr->nmr_info.nmr[i].C1 = gsm_info_ptr->nmr_info.nmr[i].C1;
    info_ptr->nmr_info.nmr[i].C2 = gsm_info_ptr->nmr_info.nmr[i].C2;
    info_ptr->nmr_info.nmr[i].C31 = gsm_info_ptr->nmr_info.nmr[i].C31;
    info_ptr->nmr_info.nmr[i].C32 = gsm_info_ptr->nmr_info.nmr[i].C32;	

    info_ptr->nmr_info.nmr[i].ncell_id.cell_id =
                        gsm_info_ptr->nmr_info.nmr[i].optional_info.cell_id;
    info_ptr->nmr_info.nmr[i].ncell_id.lac =
                        gsm_info_ptr->nmr_info.nmr[i].optional_info.lac;

    memscpy ( &info_ptr->nmr_info.nmr[i].ncell_id.plmn,
              sizeof(info_ptr->nmr_info.nmr[i].ncell_id.plmn),
              &gsm_info_ptr->nmr_info.nmr[i].optional_info.plmn,
              sizeof (sys_plmn_id_s_type) );
  }

  /* rx qual */
  info_ptr->rx_qual = gsm_info_ptr->rx_qual;

  /* tx_pwr_in_use */
  for (i = 0; i < SYS_ENG_MODE_TX_PWR_NUM_TIMESLOTS ; i++)
  {
    info_ptr->tx_pwr_in_use[i] = gsm_info_ptr->tx_pwr_in_use[i];
  }

  /* scell_info */
  info_ptr->scell_info.cell_id = gsm_info_ptr->scell_info.cell_id;
  info_ptr->scell_info.lac     = gsm_info_ptr->scell_info.lac;
  //              b_unpackw ( gsm_info_ptr->scell_info.lac, 0, 16);
  memscpy ( &info_ptr->scell_info.plmn,
            sizeof(info_ptr->nmr_info.nmr[i].ncell_id.plmn),
            &gsm_info_ptr->scell_info.plmn,
            sizeof (sys_plmn_id_s_type) );

  info_ptr->scell_info.arfcn = gsm_info_ptr->scell_info.arfcn;
  info_ptr->scell_info.bsic  = gsm_info_ptr->scell_info.bsic;
  info_ptr->scell_info.cell_info_cause =
                          (sys_eng_mode_cell_cb_cause_e_type)
                           gsm_info_ptr->scell_info.cell_info_cause;
  info_ptr->scell_info.rac = gsm_info_ptr->scell_info.rac;

  /* scell_measurements */
  info_ptr->scell_measurements.C1 = gsm_info_ptr->scell_measurements.C1;
  info_ptr->scell_measurements.C2 = gsm_info_ptr->scell_measurements.C2;
  info_ptr->scell_measurements.rx_lev =
                                gsm_info_ptr->scell_measurements.rx_lev;
  info_ptr->scell_measurements.rx_qual_sub =
                                gsm_info_ptr->scell_measurements.rx_qual_sub;
  info_ptr->scell_measurements.channel_type =
  (sys_rx_level_channel_type_e_type)gsm_info_ptr->scell_measurements.channel_type;

  /* amr_info */
  memscpy ( &info_ptr->amr_info,
            sizeof(info_ptr->amr_info),
            &gsm_info_ptr->amr_info,
            sizeof (sys_eng_mode_amr_info_s_type) );

  /* dtx_ul_status */
  info_ptr->dtx_ul_status = gsm_info_ptr->dtx_ul_status;

  /* tbf_info */
  memscpy ( &info_ptr->tbf_info,
            sizeof(info_ptr->tbf_info),
            &gsm_info_ptr->tbf_info,
            sizeof (sys_eng_mode_tbf_info_s_type) );

  /* nw_params */
  memscpy ( &info_ptr->nw_params,
            sizeof(info_ptr->nw_params),
            &gsm_info_ptr->nw_params,
            sizeof (sys_eng_mode_nw_params_s_type) );

  /* gprs_ul_cs */
  info_ptr->gprs_ul_cs = gsm_info_ptr->gprs_ul_cs;

  /* gprs_dl_cs */
  info_ptr->gprs_dl_cs = gsm_info_ptr->gprs_dl_cs;

  /* egprs_ul_mcs */
  info_ptr->egprs_ul_mcs = gsm_info_ptr->egprs_ul_mcs;

  /* egprs_dl_mcs */
  info_ptr->egprs_dl_mcs = gsm_info_ptr->egprs_dl_mcs;

  /* edge_meas */
  info_ptr->edge_meas.lq_meas_mode = gsm_info_ptr->edge_meas.lq_meas_mode;
  info_ptr->edge_meas.bep_period   = gsm_info_ptr->edge_meas.bep_period;

  /* scell_config */
  info_ptr->scell_config.pbcch_present =
                                gsm_info_ptr->scell_config.pbcch_present;

  info_ptr->scell_config.gprs_rxlev_access_min = 
                                gsm_info_ptr->scell_config.gprs_rxlev_access_min;
  info_ptr->scell_config.gprs_ms_txpwr_max_cch = 
                                gsm_info_ptr->scell_config.gprs_ms_txpwr_max_cch;							

  /* hopping status */
  info_ptr->hopping_status.hopping_status =
               gsm_info_ptr->hopping_status.hopping_status;

  /* QRD Feature Modem Stats */
  info_ptr->g_bcch =
	           gsm_info_ptr->scell_info.arfcn;
  info_ptr->g_ta =
                   gsm_info_ptr->timing_advance_value;

  /*------------------------------------------------------------------*/

  info_ptr->current_timeslot.timeslot = gsm_info_ptr->current_timeslot.timeslot;			   

  /* Print all the information. For Debug purpose */
  #ifdef CM_FTD_DEBUG
  #error code not present
#endif


  /* Send event to clients */
  cm_client_stats_event_ntfy ( CM_STATS_EVENT_MODEM_INFO,
                               stats_info_ptr);


  /* Deallocate the memory after sending the ph event. */
  cm_mem_free( stats_info_ptr );
  cm_mem_free( gsm_info_ptr );

}
#endif /* FEATURE_GSM */

#if defined(FEATURE_WCDMA)
/*===========================================================================

FUNCTION cmstats_process_wcdma_stats_info

DESCRIPTION
  Read updated stats info from NAS and send teh data to clients.
  Report CM_STATS_EVENT_MODEM_INFO to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmstats_process_wcdma_stats_info(
		sys_modem_as_id_e_type   asubs_id
)
{

  sys_modem_stats_info_s_type    *stats_info_ptr;
  wcdma_UI_event_info_type       *wcdma_info_ptr;

  /* Temp pointers to copy data */
  sys_wcdma_event_info_type      *info_ptr1;
  wcdma_event_info_type          *info_ptr2;
  int i, j;

  /* For segment loading, check if wcdma is loaded
  ** We should never get a report for WCDMA if it is unloaded,
  ** but we are checking anyway to be safe.
  */
  #ifdef FEATURE_SEGMENT_LOADING
  if(pi_mmode_wcdma == NULL)
  {
    /* Bail out here and ignore the report */
    CM_ERR_0("cmstats - pi_mmode_wcdma is NULL");
    return;
  }
  #endif

  /*
  ** Dynamically allocate a stats object
  */
  stats_info_ptr = (sys_modem_stats_info_s_type *)
                          cm_mem_malloc ( sizeof(sys_modem_stats_info_s_type));


  /* Allocate memory for WCDMA buffer */
  wcdma_info_ptr = (wcdma_UI_event_info_type *)
                     cm_mem_malloc ( sizeof(wcdma_UI_event_info_type));

  /* Call WCDMA function to get updated fields */
  #ifdef FEATURE_SEGMENT_LOADING
  IWCDMA_wcdma_get_modem_stats_info( pi_mmode_wcdma, wcdma_info_ptr );
  #else
  wcdma_get_modem_stats_info(wcdma_info_ptr);
  #endif

  /* Copy information recd from WCDMA into CM structure */
  stats_info_ptr->type = SYS_MODEM_MODULE_WCDMA;

  stats_info_ptr->asubs_id = asubs_id;

  CM_MSG_HIGH_1 ("WCDMA stats being processed for subs %d",stats_info_ptr->asubs_id);

  info_ptr1 = &(stats_info_ptr->prm.wcdma_info.wcdma_event_info);
  info_ptr2 = &(wcdma_info_ptr->wcdma_event_info);

  stats_info_ptr->prm.wcdma_info.changed_bitmask = wcdma_info_ptr->bitmask;

  /* Copy wcdma_serving_cell_info */
  info_ptr1->wcdma_serving_cell_info.uarfcn =
            info_ptr2->wcdma_serving_cell_info.uarfcn;
  info_ptr1->wcdma_serving_cell_info.psc =
            info_ptr2->wcdma_serving_cell_info.psc;
  info_ptr1->wcdma_serving_cell_info.plmn_id =
            cmtask_convert_rrc_plmn_id_to_nas_plmn_id(
               info_ptr2->wcdma_serving_cell_info.plmn_id);
  info_ptr1->wcdma_serving_cell_info.lac_id =
                 b_unpackw(info_ptr2->wcdma_serving_cell_info.lac, 0, 16);
  if (info_ptr2->wcdma_serving_cell_info.is_rac_present == TRUE)
  {
    info_ptr1->wcdma_serving_cell_info.rac_id =
                   info_ptr2->wcdma_serving_cell_info.rac[0];
  }
  info_ptr1->wcdma_serving_cell_info.ura =
            info_ptr2->wcdma_serving_cell_info.ura;
  info_ptr1->wcdma_serving_cell_info.cell_id =
            info_ptr2->wcdma_serving_cell_info.cell_id;
  info_ptr1->wcdma_serving_cell_info.nw_operation_mode =
            info_ptr2->wcdma_serving_cell_info.nw_operation_mode;


  /* Copy wcdma_serving_cell_power_info */
  info_ptr1->wcdma_serving_cell_power_info.psc =
             info_ptr2->wcdma_serving_cell_power_info.psc;
  info_ptr1->wcdma_serving_cell_power_info.rscp =
             info_ptr2->wcdma_serving_cell_power_info.rscp;
  info_ptr1->wcdma_serving_cell_power_info.ecio =
             info_ptr2->wcdma_serving_cell_power_info.ecio;
  info_ptr1->wcdma_serving_cell_power_info.squal =
             info_ptr2->wcdma_serving_cell_power_info.squal;
  info_ptr1->wcdma_serving_cell_power_info.srxlev =
             info_ptr2->wcdma_serving_cell_power_info.srxlev;			 


  /* Copy wcdma_active_set_reference_rl_info */
  info_ptr1->wcdma_active_set_reference_rl_info.uarfcn =
             info_ptr2->wcdma_active_set_reference_rl_info.uarfcn;
  info_ptr1->wcdma_active_set_reference_rl_info.psc =
             info_ptr2->wcdma_active_set_reference_rl_info.psc;
  info_ptr1->wcdma_active_set_reference_rl_info.plmn_id =
               cmtask_convert_rrc_plmn_id_to_nas_plmn_id (
                info_ptr2->wcdma_active_set_reference_rl_info.plmn_id);

  info_ptr1->wcdma_active_set_reference_rl_info.lac_id =
         b_unpackw(info_ptr2->wcdma_active_set_reference_rl_info.lac, 0, 16);
  if (info_ptr2->wcdma_active_set_reference_rl_info.is_rac_present == TRUE)
  {
    info_ptr1->wcdma_active_set_reference_rl_info.rac_id =
               info_ptr2->wcdma_active_set_reference_rl_info.rac[0];

  }
  info_ptr1->wcdma_active_set_reference_rl_info.cell_id =
             info_ptr2->wcdma_active_set_reference_rl_info.cell_id;


  /* Copy wcdma_active_set_info */
  info_ptr1->wcdma_active_set_info.num_rls =
               info_ptr2->wcdma_active_set_info.num_rls;

  CM_ASSERT (info_ptr1->wcdma_active_set_info.num_rls <=
                                    SYS_WCDMA_MAX_ASET_CELLS);

  CM_CHECK_MAX_AND_DISCARD_ITEMS(info_ptr1->wcdma_active_set_info.num_rls,
                                 SYS_WCDMA_MAX_ASET_CELLS,
                                 "RLS")

  /* Initialise the values to 0xFF */
  memset (info_ptr1->wcdma_active_set_info.psc,
          0xFF,
          SYS_WCDMA_MAX_ASET_CELLS);
  memset (info_ptr1->wcdma_active_set_info.cell_id,
          0xFF,
          SYS_WCDMA_MAX_ASET_CELLS);
  memset (info_ptr1->wcdma_active_set_info.rscp,
          0xFF,
          SYS_WCDMA_MAX_ASET_CELLS);
  memset (info_ptr1->wcdma_active_set_info.ecio,
          0xFF,
          SYS_WCDMA_MAX_ASET_CELLS);


  for (i = 0;
       i < info_ptr1->wcdma_active_set_info.num_rls;
       i++)
  {
    info_ptr1->wcdma_active_set_info.ecio[i] =
               info_ptr2->wcdma_active_set_info.ecio[i];

    info_ptr1->wcdma_active_set_info.cell_id[i] =
               info_ptr2->wcdma_active_set_info.cell_id[i];

    info_ptr1->wcdma_active_set_info.psc[i] =
               info_ptr2->wcdma_active_set_info.psc[i];

    info_ptr1->wcdma_active_set_info.rscp[i] =
               info_ptr2->wcdma_active_set_info.rscp[i];
  }

  info_ptr1->wcdma_active_set_info.sf = 
              (sys_wcdma_l1_sf_e_type)info_ptr2->wcdma_active_set_info.sf;
  info_ptr1->wcdma_active_set_info.phy_chan_type = 
              (sys_wcdma_l1_dl_phychan_e_type)info_ptr2->wcdma_active_set_info.phy_chan_type;
  info_ptr1->wcdma_active_set_info.slot_format = 
              info_ptr2->wcdma_active_set_info.slot_format;
  info_ptr1->wcdma_active_set_info.is_compressed_mode_on = 
              info_ptr2->wcdma_active_set_info.is_compressed_mode_on;  

  /* Copy wcdma_monitored_set_info */
  info_ptr1->wcdma_monitored_set_info.num_of_freqs =
                       MIN(info_ptr2->wcdma_monitored_set_info.num_of_freqs,
                           SYS_WCDMA_MAX_MONITORED_FREQ);

  CM_ASSERT (info_ptr1->wcdma_monitored_set_info.num_of_freqs <=
                                      SYS_WCDMA_MAX_MONITORED_FREQ );

  CM_CHECK_MAX_AND_DISCARD_ITEMS(
                             info_ptr1->wcdma_monitored_set_info.num_of_freqs,
                             SYS_WCDMA_MAX_MONITORED_FREQ,
                             "WCDMA Freqs")

  /* Initialise the values to 0xFF */
  memset (info_ptr1->wcdma_monitored_set_info.freq,
          0xFF,
          sizeof (sys_wcdma_monitored_freq_info_s_type) *
                  SYS_WCDMA_MAX_MONITORED_FREQ);

  for (i = 0;
       i < info_ptr1->wcdma_monitored_set_info.num_of_freqs ;
           i++ )
  {
    info_ptr1->wcdma_monitored_set_info.freq[i].uarfcn =
          info_ptr2->wcdma_monitored_set_info.freq[i].uarfcn;
    info_ptr1->wcdma_monitored_set_info.freq[i].num_of_cells =
          info_ptr2->wcdma_monitored_set_info.freq[i].num_of_cells;

    CM_ASSERT (info_ptr1->wcdma_monitored_set_info.freq[i].num_of_cells <=
                                  SYS_WCDMA_MAX_MONITORED_CELLS_PER_FREQ );

    CM_CHECK_MAX_AND_DISCARD_ITEMS(
                     info_ptr1->wcdma_monitored_set_info.freq[i].num_of_cells,
                     SYS_WCDMA_MAX_MONITORED_CELLS_PER_FREQ,
                     "WCDMA cells per freq")

    for (j = 0;
         j < info_ptr1->wcdma_monitored_set_info.freq[i].num_of_cells;
         j++)
    {
      info_ptr1->wcdma_monitored_set_info.freq[i].cell[j].psc =
            info_ptr2->wcdma_monitored_set_info.freq[i].cell[j].psc;
      info_ptr1->wcdma_monitored_set_info.freq[i].cell[j].rscp =
            info_ptr2->wcdma_monitored_set_info.freq[i].cell[j].rscp;
      info_ptr1->wcdma_monitored_set_info.freq[i].cell[j].ecio =
            info_ptr2->wcdma_monitored_set_info.freq[i].cell[j].ecio;
      info_ptr1->wcdma_monitored_set_info.freq[i].cell[j].squal =
            info_ptr2->wcdma_monitored_set_info.freq[i].cell[j].squal;
      info_ptr1->wcdma_monitored_set_info.freq[i].cell[j].srxlev =
            info_ptr2->wcdma_monitored_set_info.freq[i].cell[j].srxlev;
      info_ptr1->wcdma_monitored_set_info.freq[i].cell[j].rank =
            info_ptr2->wcdma_monitored_set_info.freq[i].cell[j].rank;
      info_ptr1->wcdma_monitored_set_info.freq[i].cell[j].set =
            info_ptr2->wcdma_monitored_set_info.freq[i].cell[j].set;			
    }
  }

  info_ptr1->wcdma_monitored_set_info.num_gsm_cells =
             info_ptr2->wcdma_monitored_set_info.num_gsm_cells;

  CM_ASSERT (info_ptr1->wcdma_monitored_set_info.num_gsm_cells <=
                                      SYS_WCDMA_MAX_MONITORED_GSM_CELLS );

  CM_CHECK_MAX_AND_DISCARD_ITEMS(
                            info_ptr1->wcdma_monitored_set_info.num_gsm_cells,
                            SYS_WCDMA_MAX_MONITORED_GSM_CELLS,
                            "GSM Cells")

  /* Initialise the values to 0xFF */
  memset (info_ptr1->wcdma_monitored_set_info.gsm_cell,
          0xFF,
          sizeof (sys_wcdma_gsm_cell_info_s_type) *
                  SYS_WCDMA_MAX_MONITORED_GSM_CELLS);

  for (i = 0;
       i < info_ptr1->wcdma_monitored_set_info.num_gsm_cells ;
           i++ )
  {
    info_ptr1->wcdma_monitored_set_info.gsm_cell[i].arfcn =
         info_ptr2->wcdma_monitored_set_info.gsm_cell[i].arfcn;
    info_ptr1->wcdma_monitored_set_info.gsm_cell[i].bsic_ncc =
         info_ptr2->wcdma_monitored_set_info.gsm_cell[i].bsic_ncc;
    info_ptr1->wcdma_monitored_set_info.gsm_cell[i].bsic_bcc =
         info_ptr2->wcdma_monitored_set_info.gsm_cell[i].bsic_bcc;
    info_ptr1->wcdma_monitored_set_info.gsm_cell[i].rssi =
         info_ptr2->wcdma_monitored_set_info.gsm_cell[i].rssi;
    info_ptr1->wcdma_monitored_set_info.gsm_cell[i].bcch =
         info_ptr2->wcdma_monitored_set_info.gsm_cell[i].arfcn;
    info_ptr1->wcdma_monitored_set_info.gsm_cell[i].rank =
         info_ptr2->wcdma_monitored_set_info.gsm_cell[i].rank;
  }

  info_ptr1->wcdma_monitored_set_info.num_lte_cells =
              info_ptr2->wcdma_monitored_set_info.num_lte_cells;

  if (info_ptr1->wcdma_monitored_set_info.num_lte_cells > SYS_WCDMA_MAX_MONITORED_LTE_CELLS)
  {
    info_ptr1->wcdma_monitored_set_info.num_lte_cells = SYS_WCDMA_MAX_MONITORED_LTE_CELLS;
    CM_ERR_1(" num_lte_cells =%d > MAX cell",info_ptr1->wcdma_monitored_set_info.num_lte_cells);
  }

  /* Initialise the values to 0xFF */
  memset (info_ptr1->wcdma_monitored_set_info.lte_cell,
          0xFF,
          sizeof (sys_wcdma_lte_cell_info_s_type) *
                  info_ptr1->wcdma_monitored_set_info.num_lte_cells);

   for (i = 0;
       i < info_ptr1->wcdma_monitored_set_info.num_lte_cells ;
           i++ )
   {
    info_ptr1->wcdma_monitored_set_info.lte_cell[i].earfcn=
         info_ptr2->wcdma_monitored_set_info.lte_cell[i].earfcn;
    info_ptr1->wcdma_monitored_set_info.lte_cell[i].cell_id=
         info_ptr2->wcdma_monitored_set_info.lte_cell[i].cell_id;
    info_ptr1->wcdma_monitored_set_info.lte_cell[i].rsrp=
         info_ptr2->wcdma_monitored_set_info.lte_cell[i].rsrp;
    info_ptr1->wcdma_monitored_set_info.lte_cell[i].rsrq=
         info_ptr2->wcdma_monitored_set_info.lte_cell[i].rsrq;
    info_ptr1->wcdma_monitored_set_info.lte_cell[i].s_rxlev=
         info_ptr2->wcdma_monitored_set_info.lte_cell[i].s_rxlev;
    info_ptr1->wcdma_monitored_set_info.lte_cell[i].is_tdd_freq=
         info_ptr2->wcdma_monitored_set_info.lte_cell[i].is_tdd_freq;
   }

  /* Copy wcdma_rrc_state */
  info_ptr1->wcdma_rrc_state = info_ptr2->wcdma_rrc_state;

  /*QRD Feature Modem Statistics*/
  info_ptr1->gw_dl_bler = info_ptr2->wcdma_serving_cell_power_info.dl_bler;
  info_ptr1->w_agc = info_ptr2->wcdma_serving_cell_power_info.rx_agc;
  info_ptr1->w_txagc = info_ptr2->wcdma_serving_cell_power_info.tx_agc;
  

  /* Print all the information. For Debug purpose */
  #ifdef CM_FTD_DEBUG
  #error code not present
#endif

  /* Send event to clients */
  cm_client_stats_event_ntfy ( CM_STATS_EVENT_MODEM_INFO,
                               stats_info_ptr);

  /* Deallocate the memory after sending the ph event. */
  cm_mem_free( wcdma_info_ptr );
  cm_mem_free( stats_info_ptr );


}
#endif /* FEATURE_WCDMA */
#endif /* CM_GW_SUPPORTED */

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*===========================================================================

FUNCTION cmstats_process_cdma_stats_info

DESCRIPTION
  Read updated stats info from CDMA 1x and send the data to clients.
  Report CM_STATS_EVENT_MODEM_INFO to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmstats_process_cdma_stats_info(

  void
)
{

  sys_modem_stats_info_s_type    *stats_info_ptr;

  /*
  ** Dynamically allocate a stats object
  */
  stats_info_ptr = (sys_modem_stats_info_s_type *)
                          cm_mem_malloc ( sizeof(sys_modem_stats_info_s_type));

  stats_info_ptr->type = SYS_MODEM_MODULE_CDMA;
  stats_info_ptr->asubs_id = SYS_MODEM_AS_ID_1;

  #ifndef FEATURE_MMODE_REMOVE_1X
  /* Call CDMA function to copy all updated fields */
  if ( mcc_get_cdma_stats_info( &(stats_info_ptr->prm.cdma_info)) == TRUE )
  {
    /* Print all the information. For Debug purpose */
    #ifdef CM_FTD_DEBUG
    #error code not present
#endif

    /* Send event to clients */
    cm_client_stats_event_ntfy ( CM_STATS_EVENT_MODEM_INFO,
                                 stats_info_ptr);
  }
  else
  #endif
  {
    CM_ERR_0("Can't get CDMA statistics");
  }

  /* Deallocate the memory after sending the ph event. */
  cm_mem_free(stats_info_ptr);


}
#endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

#ifdef FEATURE_HDR
/*===========================================================================

FUNCTION cmstats_process_hdr_stats_info

DESCRIPTION
  Read updated stats info from HDR and send the data to clients.
  Report CM_STATS_EVENT_MODEM_INFO to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmstats_process_hdr_stats_info(

  void
)
{

  sys_modem_stats_info_s_type    *stats_info_ptr;

  /*
  ** Dynamically allocate a stats object
  */
  stats_info_ptr = (sys_modem_stats_info_s_type *)
                          cm_mem_malloc ( sizeof(sys_modem_stats_info_s_type));

  stats_info_ptr->type = SYS_MODEM_MODULE_HDR;
  stats_info_ptr->asubs_id = SYS_MODEM_AS_ID_1;
  /* Call HDR function to copy all updated fields */

  hdrcp_mc_get_stats_info ( &(stats_info_ptr->prm.hdr_info) );

  /* Print all the information. For Debug purpose */
  #ifdef CM_FTD_DEBUG
  #error code not present
#endif

  /* Send event to clients */
  cm_client_stats_event_ntfy ( CM_STATS_EVENT_MODEM_INFO,
                               stats_info_ptr);


  /* Deallocate the memory after sending the ph event. */
  cm_mem_free(stats_info_ptr);

}

/*===========================================================================

FUNCTION cmstats_process_ds_stats_info

DESCRIPTION
  Read updated stats info from HDR and send the data to clients.
  Report CM_STATS_EVENT_MODEM_INFO to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmstats_process_ds_stats_info(

  void
)
{

  sys_modem_stats_info_s_type    *stats_info_ptr;

  /*
  ** Dynamically allocate a stats object
  */
  stats_info_ptr = (sys_modem_stats_info_s_type *)
                          cm_mem_malloc ( sizeof(sys_modem_stats_info_s_type));

  stats_info_ptr->type = SYS_MODEM_MODULE_DS;
  /* Call DS function to copy all updated fields */
  if (ds3g_get_stats_info( &(stats_info_ptr->prm.ds_info)) == TRUE )
  {
    /* Print all the information. For Debug purpose */
    #ifdef CM_FTD_DEBUG
    #error code not present
#endif

    /* Send event to clients */
    cm_client_stats_event_ntfy ( CM_STATS_EVENT_MODEM_INFO,
                                 stats_info_ptr);
  }
  else
  {
    CM_ERR_0("Can't get DS statistics");
  }

  /* Deallocate the memory after sending the ph event. */
  cm_mem_free(stats_info_ptr);


}
#endif /* FEATURE_HDR */

#ifdef FEATURE_CM_LTE
/*===========================================================================

FUNCTION cmstats_process_lte_stats_info

DESCRIPTION
  Read updated stats info from NAS and send teh data to clients.
  Report CM_STATS_EVENT_MODEM_INFO to clients.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmstats_process_lte_stats_info(

  void
)
/*lint -esym(714,cmstats_process_lte_stats_info) */
{

  sys_modem_stats_info_s_type    *stats_info_ptr;
  cmapi_rat_meas_info_s_type     *lte_info_ptr;
  sys_lte_ngbr_ind_s_type        *temp_sys_info_ptr;
  cmapi_lte_ngbr_ind_s           *temp_lte_info_ptr;
  uint16                         cnt, cnt2;

  /*
  ** Dynamically allocate a stats object
  */
  stats_info_ptr = (sys_modem_stats_info_s_type *)
                          cm_mem_malloc ( sizeof(sys_modem_stats_info_s_type));


  lte_info_ptr   = (cmapi_rat_meas_info_s_type *)
                          cm_mem_malloc ( sizeof(cmapi_rat_meas_info_s_type));

  stats_info_ptr->type = SYS_MODEM_MODULE_LTE;
  stats_info_ptr->prm.lte_info.changed_bitmask = SYS_LTE_MEAS_INFO_BIT_MASK;
  /* Call LTE function to copy all updated fields */

  if (cmapi_get_rat_meas_info(CMAPI_SYS_MODE_LTE,
                              lte_info_ptr) != CMAPI_SUCCESS)
  {
    CM_ERR_0("Can't get LTE statistics");
    /* Deallocate the memory */
    cm_mem_free( stats_info_ptr );
    cm_mem_free( lte_info_ptr );
    return;
  }

  /* Copy the LTE info to sys structure */
  temp_sys_info_ptr =
        &(stats_info_ptr->prm.lte_info.lte_meas_info.lte_nbr_info);
  temp_lte_info_ptr = &(lte_info_ptr->meas_info_u.lte_nbr_info);

  stats_info_ptr->prm.lte_info.lte_meas_info.is_service_available =
                                          lte_info_ptr->is_service_available;
  stats_info_ptr->prm.lte_info.lte_meas_info.sys_mode =
          cmstats_map_cmapi_to_cm_sys_mode( lte_info_ptr->sys_mode );

  temp_sys_info_ptr->valid = temp_lte_info_ptr->valid;
  temp_sys_info_ptr->idle  = temp_lte_info_ptr->idle;
  /* lte_intra */
  temp_sys_info_ptr->lte_intra.earfcn = temp_lte_info_ptr->lte_intra.earfcn;
  temp_sys_info_ptr->lte_intra.serving_cell_id =
                                  temp_lte_info_ptr->lte_intra.serving_cell_id;
  temp_sys_info_ptr->lte_intra.num_lte_cells =
                                   temp_lte_info_ptr->lte_intra.num_lte_cells;

  CM_ASSERT (temp_sys_info_ptr->lte_intra.num_lte_cells <= SYS_LTE_NGBR_IND_NUM_CELLS );

  CM_CHECK_MAX_AND_DISCARD_ITEMS( temp_sys_info_ptr->lte_intra.num_lte_cells,
                                  SYS_LTE_NGBR_IND_NUM_CELLS,
                                  "LTE Cells")
                                  
  for (cnt = 0; cnt < temp_sys_info_ptr->lte_intra.num_lte_cells; cnt++ )
  {
    temp_sys_info_ptr->lte_intra.cell[cnt].pci  =
                                   temp_lte_info_ptr->lte_intra.cell[cnt].pci;
    temp_sys_info_ptr->lte_intra.cell[cnt].rsrq =
                                  temp_lte_info_ptr->lte_intra.cell[cnt].rsrq;
    temp_sys_info_ptr->lte_intra.cell[cnt].rsrp =
                                  temp_lte_info_ptr->lte_intra.cell[cnt].rsrp;
    temp_sys_info_ptr->lte_intra.cell[cnt].rssi =
                                  temp_lte_info_ptr->lte_intra.cell[cnt].rssi;
    temp_sys_info_ptr->lte_intra.cell[cnt].idle.srxlev =
                           temp_lte_info_ptr->lte_intra.cell[cnt].idle.srxlev;
  }
  temp_sys_info_ptr->lte_intra.idle.cell_resel_priority =
                        temp_lte_info_ptr->lte_intra.idle.cell_resel_priority;
  temp_sys_info_ptr->lte_intra.idle.s_intra_search =
                             temp_lte_info_ptr->lte_intra.idle.s_intra_search;
  temp_sys_info_ptr->lte_intra.idle.s_non_intra_search =
                         temp_lte_info_ptr->lte_intra.idle.s_non_intra_search;
  temp_sys_info_ptr->lte_intra.idle.thresh_serving_low =
                         temp_lte_info_ptr->lte_intra.idle.thresh_serving_low;

  /*  lte_inter */
  temp_sys_info_ptr->lte_inter.num_freqs =
                                      temp_lte_info_ptr->lte_inter.num_freqs;
  CM_ASSERT ( temp_sys_info_ptr->lte_inter.num_freqs <= SYS_LTE_NGBR_IND_NUM_FREQS);

  CM_CHECK_MAX_AND_DISCARD_ITEMS( temp_sys_info_ptr->lte_inter.num_freqs,
                                  SYS_LTE_NGBR_IND_NUM_FREQS,
                                  "LTE Freqs")
  
  for (cnt = 0; cnt < temp_sys_info_ptr->lte_inter.num_freqs; cnt++)
  {
    temp_sys_info_ptr->lte_inter.freqs[cnt].earfcn =
                               temp_lte_info_ptr->lte_inter.freqs[cnt].earfcn;
    temp_sys_info_ptr->lte_inter.freqs[cnt].num_lte_cells =
                        temp_lte_info_ptr->lte_inter.freqs[cnt].num_lte_cells;
    CM_ASSERT ( temp_sys_info_ptr->lte_inter.freqs[cnt].num_lte_cells <= SYS_LTE_NGBR_IND_NUM_FREQS);


    CM_CHECK_MAX_AND_DISCARD_ITEMS(
                        temp_sys_info_ptr->lte_inter.freqs[cnt].num_lte_cells,
                        SYS_LTE_NGBR_IND_NUM_FREQS,
                        "LTE cells per freq")

    for (cnt2 = 0; cnt2 < temp_sys_info_ptr->lte_inter.freqs[cnt].num_lte_cells; cnt2++)
    {
      temp_sys_info_ptr->lte_inter.freqs[cnt].cell[cnt2].pci =
                       temp_lte_info_ptr->lte_inter.freqs[cnt].cells[cnt2].pci;
      temp_sys_info_ptr->lte_inter.freqs[cnt].cell[cnt2].rsrp =
                      temp_lte_info_ptr->lte_inter.freqs[cnt].cells[cnt2].rsrp;
      temp_sys_info_ptr->lte_inter.freqs[cnt].cell[cnt2].rsrq =
                      temp_lte_info_ptr->lte_inter.freqs[cnt].cells[cnt2].rsrq;
      temp_sys_info_ptr->lte_inter.freqs[cnt].cell[cnt2].rssi =
                      temp_lte_info_ptr->lte_inter.freqs[cnt].cells[cnt2].rssi;
      temp_sys_info_ptr->lte_inter.freqs[cnt].cell[cnt2].idle.srxlev=
               temp_lte_info_ptr->lte_inter.freqs[cnt].cells[cnt2].idle.srxlev;
    }
    temp_sys_info_ptr->lte_inter.freqs[cnt].idle.threshX_high =
                     temp_lte_info_ptr->lte_inter.freqs[cnt].idle.threshX_high;
    temp_sys_info_ptr->lte_inter.freqs[cnt].idle.threshX_low  =
                      temp_lte_info_ptr->lte_inter.freqs[cnt].idle.threshX_low;
    temp_sys_info_ptr->lte_inter.freqs[cnt].idle.cell_resel_priority=
              temp_lte_info_ptr->lte_inter.freqs[cnt].idle.cell_resel_priority;
  }

  /* gsm */
  temp_sys_info_ptr->gsm.num_freq_groups = temp_lte_info_ptr->gsm.num_freq_groups;
  CM_ASSERT (temp_lte_info_ptr->gsm.num_freq_groups <= CMAPI_LTE_NGBR_IND_NUM_FREQS);

  CM_CHECK_MAX_AND_DISCARD_ITEMS( temp_lte_info_ptr->gsm.num_freq_groups,
                                  CMAPI_LTE_NGBR_IND_NUM_FREQS,
                                  "GSM Freq groups")
  
  for (cnt = 0; cnt < temp_lte_info_ptr->gsm.num_freq_groups; cnt++)
  {
    temp_sys_info_ptr->gsm.freq_group[cnt].num_gsm_arfcn =
                            temp_lte_info_ptr->gsm.freq_group[cnt].num_gsm_arfcn;
    CM_ASSERT (temp_sys_info_ptr->gsm.freq_group[cnt].num_gsm_arfcn <= SYS_LTE_NGBR_IND_NUM_CELLS);

    CM_CHECK_MAX_AND_DISCARD_ITEMS(
                         temp_sys_info_ptr->gsm.freq_group[cnt].num_gsm_arfcn,
                         SYS_LTE_NGBR_IND_NUM_CELLS,
                         "GSM arfcn")

    for (cnt2 = 0; cnt2 < temp_lte_info_ptr->gsm.freq_group[cnt].num_gsm_arfcn; cnt2++)
    {
      temp_sys_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].arfcn =
                      temp_lte_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].arfcn;
      temp_sys_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].band_1900 =
                  temp_lte_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].band_1900;
      temp_sys_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].bsic_id =
                    temp_lte_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].bsic_id;
      temp_sys_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].cell_id_valid =
              temp_lte_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].cell_id_valid;
      temp_sys_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].rssi =
                       temp_lte_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].rssi;
      temp_sys_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].idle.srxlev =
                temp_lte_info_ptr->gsm.freq_group[cnt].arfcn[cnt2].idle.srxlev;
    }
    temp_sys_info_ptr->gsm.freq_group[cnt].idle.cell_resel_priority =
                temp_lte_info_ptr->gsm.freq_group[cnt].idle.cell_resel_priority;
    temp_sys_info_ptr->gsm.freq_group[cnt].idle.thresh_gsm_high =
                    temp_lte_info_ptr->gsm.freq_group[cnt].idle.thresh_gsm_high;
    temp_sys_info_ptr->gsm.freq_group[cnt].idle.thresh_gsm_low =
                     temp_lte_info_ptr->gsm.freq_group[cnt].idle.thresh_gsm_low;
    temp_sys_info_ptr->gsm.freq_group[cnt].idle.ncc_permitted  =
                      temp_lte_info_ptr->gsm.freq_group[cnt].idle.ncc_permitted;
  }

  /* wcdma */
  temp_sys_info_ptr->wcdma.num_wcdma_freqs = temp_lte_info_ptr->wcdma.num_wcdma_freqs;
  CM_ASSERT (temp_lte_info_ptr->wcdma.num_wcdma_freqs <= CMAPI_LTE_NGBR_IND_NUM_FREQS)

  CM_CHECK_MAX_AND_DISCARD_ITEMS( temp_lte_info_ptr->wcdma.num_wcdma_freqs,
                                  CMAPI_LTE_NGBR_IND_NUM_FREQS,
                                  "W freqs")

  for (cnt=0; cnt < temp_lte_info_ptr->wcdma.num_wcdma_freqs; cnt++)
  {
    temp_sys_info_ptr->wcdma.freq[cnt].uarfcn =
                                      temp_lte_info_ptr->wcdma.freq[cnt].uarfcn;
    temp_sys_info_ptr->wcdma.freq[cnt].num_wcdma_cells =
                             temp_lte_info_ptr->wcdma.freq[cnt].num_wcdma_cells;
    CM_ASSERT (temp_lte_info_ptr->wcdma.freq[cnt].num_wcdma_cells <= CMAPI_LTE_NGBR_IND_NUM_FREQS);

    CM_CHECK_MAX_AND_DISCARD_ITEMS(
                           temp_lte_info_ptr->wcdma.freq[cnt].num_wcdma_cells,
                           CMAPI_LTE_NGBR_IND_NUM_FREQS,
                           "W cells")
    
    for (cnt2 = 0; cnt2 < temp_lte_info_ptr->wcdma.freq[cnt].num_wcdma_cells ; cnt2++ )
    {
      temp_sys_info_ptr->wcdma.freq[cnt].cell[cnt2].psc =
                             temp_lte_info_ptr->wcdma.freq[cnt].cell[cnt2].psc;
      temp_sys_info_ptr->wcdma.freq[cnt].cell[cnt2].cpich_ecno =
                      temp_lte_info_ptr->wcdma.freq[cnt].cell[cnt2].cpich_ecno;
      temp_sys_info_ptr->wcdma.freq[cnt].cell[cnt2].cpich_rscp =
                      temp_lte_info_ptr->wcdma.freq[cnt].cell[cnt2].cpich_rscp;
      temp_sys_info_ptr->wcdma.freq[cnt].cell[cnt2].idle.srxlev =
                     temp_lte_info_ptr->wcdma.freq[cnt].cell[cnt2].idle.srxlev;
    }
    temp_sys_info_ptr->wcdma.freq[cnt].idle.cell_resel_priority =
                   temp_lte_info_ptr->wcdma.freq[cnt].idle.cell_resel_priority;
    temp_sys_info_ptr->wcdma.freq[cnt].idle.thresh_Xhigh =
                          temp_lte_info_ptr->wcdma.freq[cnt].idle.thresh_Xhigh;
    temp_sys_info_ptr->wcdma.freq[cnt].idle.thresh_Xlow =
                           temp_lte_info_ptr->wcdma.freq[cnt].idle.thresh_Xlow;
  }



  /* Print all the information. For Debug purpose */
  #ifdef CM_FTD_DEBUG
  #error code not present
#endif

  /* Send event to clients */
  cm_client_stats_event_ntfy ( CM_STATS_EVENT_MODEM_INFO,
                               stats_info_ptr);


  /* Deallocate the memory after sending the ph event. */
  cm_mem_free(stats_info_ptr);
  cm_mem_free(lte_info_ptr);

}
#endif /* FEATURE_CM_LTE */
/*===========================================================================

FUNCTION cmstats_process_stats_signal

DESCRIPTION
  Process stats signal set for respective protocol module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmstats_process_stats_signal
(
  sys_modem_module_e_type module,
  sys_modem_as_id_e_type    asubs_id
)
/*lint -esym(714, cmstats_process_stats_signal) */
/*lint -esym(765, cmstats_process_stats_signal) */
{
  CM_MSG_HIGH_2( "cmstats_process_stats_signal. Module - %d, asubs_id - %d",
                 module, asubs_id );

  switch(module)
  {
    #ifdef CM_GW_SUPPORTED
    case SYS_MODEM_MODULE_NAS:
     /* Read NAS fields that got updated */
     cmstats_process_nas_stats_info(asubs_id);
     break;

    #ifdef FEATURE_GSM
    case SYS_MODEM_MODULE_GSM:
     /* Read GSM fields that got updated */
     cmstats_process_gsm_stats_info(asubs_id);
     break;
    #endif /* FEATURE_GSM */

    #ifdef FEATURE_WCDMA
    case SYS_MODEM_MODULE_WCDMA:
     /* Read WCDMA fields that got updated */
     cmstats_process_wcdma_stats_info(asubs_id);
     break;
    #endif /* FEATURE_WCDMA */
    #endif /* FEATURE_GSM || FEATURE_WCDMA */

    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    case SYS_MODEM_MODULE_CDMA:
     /* Read CDMA fields that got updated */
     cmstats_process_cdma_stats_info();
     break;
    #endif

    #ifdef FEATURE_HDR
    case SYS_MODEM_MODULE_HDR:
     /* Read HDR fields that got updated */
     cmstats_process_hdr_stats_info();
     break;

    case SYS_MODEM_MODULE_DS:
     /* Read DS fields that got updated */
     cmstats_process_ds_stats_info();
     break;
    #endif /* FEATURE_HDR */

    #ifdef FEATURE_CM_LTE
    case SYS_MODEM_MODULE_LTE:
     /* Read LTE fields that got updated */
     cmstats_process_lte_stats_info();
     break;
     #endif

        default:
     /* Invalid module */
     CM_MSG_HIGH_0("Invalid module. Can't read protocol stats");
  }

}


#ifdef CM_FTD_DEBUG
#error code not present
#endif /* CM_FTD_DEBUG*/


/**--------------------------------------------------------------------------
** Functions - external, common
** --------------------------------------------------------------------------
*/
/**===========================================================================

@FUNCTION cm_stats_set_modem_info_signal

@DESCRIPTION
  Command CM to set the Modem Statistics info signal

  Protocols layers (NAS/RR/RRC)will call this API to set
  respective signal.

@DEPENDENCIES


@RETURNS
  TRUE - request ack, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/

boolean cm_stats_set_modem_info_signal (

   sys_modem_module_e_type           module_type
    /* Module for which mask is being set */

)
/*lint -esym(715,module_type) */
{

  switch(module_type)
  {
    case SYS_MODEM_MODULE_NAS:
     (void) rex_set_sigs( get_cm_tcb(), CM_STATS_NAS_SIG );
     break;

    case SYS_MODEM_MODULE_GSM:
     (void) rex_set_sigs( get_cm_tcb(), CM_STATS_GSM_SIG );
     break;

    case SYS_MODEM_MODULE_WCDMA:
     (void) rex_set_sigs( get_cm_tcb(), CM_STATS_WCDMA_SIG );
     break;

    case SYS_MODEM_MODULE_CDMA:
     (void) rex_set_sigs( get_cm_tcb(), CM_STATS_CDMA_SIG );
     break;

    case SYS_MODEM_MODULE_HDR:
     (void) rex_set_sigs( get_cm_tcb(), CM_STATS_HDR_SIG );
     break;

    case SYS_MODEM_MODULE_DS:
     (void) rex_set_sigs( get_cm_tcb(), CM_STATS_DS_SIG );
     break;

    case SYS_MODEM_MODULE_LTE:
     (void) rex_set_sigs( get_cm_tcb(), CM_STATS_LTE_SIG );
     break;

    default:
     CM_MSG_HIGH_0("Invalid module type. No signal set");
     return FALSE;
  }

  return TRUE;

}
/*lint +esym(715,module_type) */

/**===========================================================================

@FUNCTION cm_per_subs_stats_set_modem_info_signal

@DESCRIPTION
  Command CM to set the Modem Statistics info signal

  Protocols layers (NAS/RR/RRC)will call this API to set
  respective signal.

@DEPENDENCIES


@RETURNS
  TRUE - request ack, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
boolean cm_per_subs_stats_set_modem_info_signal (

   sys_modem_module_e_type        module_type,
    /**< Module for which mask is being set */

   sys_modem_as_id_e_type         asubs_id


)
{
  CM_MSG_HIGH_2( "cm_per_subs_stats_set_modem_info_signal(): subs_id %d, module_type %d",
                 asubs_id, module_type );

  #if  defined( FEATURE_MMODE_DUAL_SIM )
  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    switch(module_type)
    {
      case SYS_MODEM_MODULE_NAS:
       (void) rex_set_sigs( get_cm_tcb(), CM_STATS_NAS2_SIG );
       break;

      case SYS_MODEM_MODULE_GSM:
       (void) rex_set_sigs( get_cm_tcb(), CM_STATS_GSM2_SIG );
       break;

      case SYS_MODEM_MODULE_WCDMA:
       (void) rex_set_sigs( get_cm_tcb(), CM_STATS_WCDMA2_SIG );
       break;

    default:
       return FALSE;
    }
  }
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  else if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    switch(module_type)
    {
      case SYS_MODEM_MODULE_GSM:
       (void) rex_set_sigs( get_cm_tcb(), CM_STATS_GSM3_SIG );
       break;
  
      case SYS_MODEM_MODULE_WCDMA:
       (void) rex_set_sigs( get_cm_tcb(), CM_STATS_WCDMA_SIG );
       break;
  
      default:
       return FALSE;      
    }
  }
  #endif /* FEATURE_MMODE_TRIPLE_SIM */
  else
  #endif
  {
    return cm_stats_set_modem_info_signal(module_type);
  }

  return TRUE;
}

/**===========================================================================

@FUNCTION cm_per_subs_stacks_stats_set_modem_info_signal

@DESCRIPTION
  Command CM to set the Modem Statistics info signal on a spcific stack

  Protocols layers (NAS/RR/RRC)will call this API to set
  respective signal.

@DEPENDENCIES


@RETURNS
  TRUE - request ack, FALSE - No buffer to send request

@SIDE_EFFECT
  If command is successful, CM clients will be notified of the
  corresponding CM event.

@x
===========================================================================*/
extern boolean cm_per_subs_stacks_stats_set_modem_info_signal (

   sys_modem_module_e_type           module_type,
    /**< Module for which mask is being set */

   sys_modem_as_id_e_type        asubs_id,
   /**< Subscription on which signal will be set */

   sys_modem_stack_id_e_type     stack_id
   /**< Stack of asubs_id on which signal will be set */

)
{

  CM_MSG_HIGH_3("Modem stats signal recd for module %d as_id %d, stack_id %d", module_type, asubs_id, stack_id);
  #if  defined( FEATURE_MMODE_DUAL_SIM )
  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    switch(module_type)
    {
      case SYS_MODEM_MODULE_NAS:
        switch(stack_id)
        {
          case SYS_MODEM_STACK_ID_2:
            (void) rex_set_sigs( get_cm_tcb(), CM_STATS_NAS3_SIG );
            break;
          case SYS_MODEM_STACK_ID_1:
          default:
            (void) rex_set_sigs( get_cm_tcb(), CM_STATS_NAS2_SIG );
            break;
        }
       break;

      case SYS_MODEM_MODULE_GSM:
       (void) rex_set_sigs( get_cm_tcb(), CM_STATS_GSM2_SIG );
       break;

      case SYS_MODEM_MODULE_WCDMA:
       (void) rex_set_sigs( get_cm_tcb(), CM_STATS_WCDMA2_SIG );
       break;

    default:
       CM_MSG_HIGH_0("Invalid module type. No signal set");
       return FALSE;
    }
  }
  else
  #endif
  {
    switch(module_type)
    {
      case SYS_MODEM_MODULE_NAS:
        switch(stack_id)
        {
          case SYS_MODEM_STACK_ID_2:
            (void) rex_set_sigs( get_cm_tcb(), CM_STATS_NAS3_SIG );
            break;
          case SYS_MODEM_STACK_ID_1:
          default:
            (void) rex_set_sigs( get_cm_tcb(), CM_STATS_NAS_SIG );
            break;
        }
      break;
      default:
        return cm_stats_set_modem_info_signal(module_type);
    }
  }

  return TRUE;
}


/*===========================================================================

FUNCTION cmstats_ptr

DESCRIPTION
  Return a pointer to the one and only stats object.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
sys_modem_stats_info_s_type  *cmstats_ptr( void )
/*lint -esym(765, cmstats_ptr) */
{

  static sys_modem_stats_info_s_type cmstats_local;
    /* The one and only phone object */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &cmstats_local;

} /* cmstats_ptr() */

/*===========================================================================

FUNCTION cmstats_update_stats_info

DESCRIPTION
  Updates the stats structure with changed fields.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmstats_update_stats_info (

  sys_cm_information_s_type  *cm_info,
  /* Place holder for CM stats structure */

  uint32 bit_mask_category
  /* bit mask category selected by user */
)
{
  /* Fill the structure as per the bit mask received */
  switch( bit_mask_category )
  {
    case SYS_CM_PH_PREF_BIT_MASK:
      if ( global_cm_bit_mask &  SYS_CM_PH_PREF_BIT_MASK )
      {
        cm_info->ph_info.band_cap  =
                              cmstats_ptr()->prm.cm_info.ph_info.band_cap;
        cm_info->ph_info.mode_pref =
                              cmstats_ptr()->prm.cm_info.ph_info.mode_pref;
        cm_info->ph_info.pref_sys  =
                              cmstats_ptr()->prm.cm_info.ph_info.pref_sys;

        /* Update the changed_fields */
        cm_info->changed_bitmask |= bit_mask_category;
      }
      break;

    case SYS_CM_SYS_SEL_INFO_BIT_MASK:
      if ( global_cm_bit_mask & SYS_CM_SYS_SEL_INFO_BIT_MASK )
      {
        *(&(cm_info->sys_sel_info.sel_sys)) = *(&(cmstats_ptr()->prm.cm_info.sys_sel_info.sel_sys));

        cm_info->sys_sel_info.roam_ind =
                           cmstats_ptr()->prm.cm_info.sys_sel_info.roam_ind;

        /* Update the changed_fields */
        cm_info->changed_bitmask |= bit_mask_category;
      }
      break;

    case SYS_CM_PRL_INFO_BIT_MASK:
      if ( global_cm_bit_mask & SYS_CM_PRL_INFO_BIT_MASK )
      {
        cm_info->prl_ver = cmstats_ptr()->prm.cm_info.prl_ver;

        /* Update the changed_fields */
        cm_info->changed_bitmask |= bit_mask_category;
      }
      break;

    case SYS_CM_CALL_STAT_BIT_MASK:
      if ( global_cm_bit_mask & SYS_CM_CALL_STAT_BIT_MASK )
      {
        cm_info->send_key_orig_count = num_orig_count;

        /* Update the changed_fields */
        cm_info->changed_bitmask |= bit_mask_category;
      }
      break;

    default:
      cm_info->changed_bitmask = SYS_CM_INFO_MASK_NONE;
      CM_ERR_0 ("Invalid bit mask categ for CM");
  }
}
/*===========================================================================

FUNCTION cmstats_update_stats_and_post_event

DESCRIPTION
  Updates the stats structure with changed field and posts STATS event
  to clients. Do this only for the bit masks set by client.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmstats_update_stats_and_post_event (

   uint32 bit_mask_category
   /* Bit mask modified */
)
/*lint -esym(714, cmstats_update_stats_and_post_event) */
/*lint -esym(765, cmstats_update_stats_and_post_event) */
{
  sys_modem_stats_info_s_type    *stats_info_ptr;

  /*
  ** Dynamically allocate a stats object
  */
  stats_info_ptr = (sys_modem_stats_info_s_type *)
                          cm_mem_malloc ( sizeof(sys_modem_stats_info_s_type));


  /* Set the initial values */
  stats_info_ptr->type = SYS_MODEM_MODULE_CM;
  stats_info_ptr->prm.cm_info.changed_bitmask = SYS_CM_INFO_MASK_NONE;

  /* check for all the set bits in the input and update stats */
  if (bit_mask_category & SYS_CM_PH_PREF_BIT_MASK)
  {
    cmstats_update_stats_info ( &stats_info_ptr->prm.cm_info,
                                SYS_CM_PH_PREF_BIT_MASK);
  }
  if (bit_mask_category & SYS_CM_SYS_SEL_INFO_BIT_MASK)
  {
    cmstats_update_stats_info ( &stats_info_ptr->prm.cm_info,
                                SYS_CM_SYS_SEL_INFO_BIT_MASK);
  }
  if (bit_mask_category & SYS_CM_PRL_INFO_BIT_MASK)
  {
    cmstats_update_stats_info ( &stats_info_ptr->prm.cm_info,
                                SYS_CM_PRL_INFO_BIT_MASK);
  }
  if (bit_mask_category & SYS_CM_CALL_STAT_BIT_MASK)
  {
    cmstats_update_stats_info ( &stats_info_ptr->prm.cm_info,
                                SYS_CM_CALL_STAT_BIT_MASK);
  }

  /* If any fields were updated, inform clients with STATS event */
  if (stats_info_ptr->prm.cm_info.changed_bitmask != SYS_CM_INFO_MASK_NONE)
  {
    /* Send event to clients */
    cm_client_stats_event_ntfy ( CM_STATS_EVENT_MODEM_INFO,
                                 stats_info_ptr);

        /* Print all the information. For Debug purpose */
    #ifdef CM_FTD_DEBUG
    #error code not present
#endif

    /* Reset the stats_ptr() */
    cmstats_ptr()->prm.cm_info.ph_info.band_cap  = SYS_BAND_MASK_EMPTY;
    cmstats_ptr()->prm.cm_info.ph_info.mode_pref = SYS_STAT_MODE_PREF_NONE;
    cmstats_ptr()->prm.cm_info.ph_info.pref_sys  = SYS_STATS_PRL_PREF_NONE;
    memset ( &cmstats_ptr()->prm.cm_info.sys_sel_info.sel_sys,
              0,
              sizeof (sys_sys_id_s_type));
    cmstats_ptr()->prm.cm_info.sys_sel_info.roam_ind = SYS_STAT_ROAM_OFF;
    cmstats_ptr()->prm.cm_info.prl_ver      = 0;
    cmstats_ptr()->prm.cm_info.send_key_orig_count = 0;
  }

  /* Deallocate the memory after sending the stats event. */
  cm_mem_free(stats_info_ptr);
}

/*===========================================================================

FUNCTION cmstats_increment_send_key_press_count

DESCRIPTION
  Increment the number of times the user had originated a call

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmstats_increment_send_key_press_count (

  void
)
/*lint -esym(714, cmstats_increment_send_key_press_count) */
/*lint -esym(765, cmstats_increment_send_key_press_count) */
{
  /* Increment the global count */
  num_orig_count ++;
}

/*===========================================================================

FUNCTION cmstats_get_lte_bit_mask

DESCRIPTION
  Returns the bit mask set for LTE

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_CM_LTE
uint64 cmstats_get_lte_bit_mask(

  void
)
{
  return lte_bit_mask;
}
#endif
