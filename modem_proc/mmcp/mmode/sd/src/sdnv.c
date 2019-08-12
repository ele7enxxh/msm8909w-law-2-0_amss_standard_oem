/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

                       N V   I N T E R F A C E

GENERAL DESCRIPTION
  This file makes up the NV-Interface component of the SD.

  The SD NV-Interface component encapsulates all the functionality that is
  associated with reading and writing from/to NV SD related items, such as
  the PRL and MRU table.

EXTERNALIZED FUNCTIONS
  sdnv_init
  sdnv_write
  sdnv_read

REGIONAL FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  sdnv_init() must be call to initialize this component before any other
  function declared in this component is called.


Copyright (c) 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdnv.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/12   gm      Mainline EFS feature - Remove FEATURE_EFS
16/02/12   vk      Replace usage of AEE library function with corresponding 
                   CoreBSP library functions
07/11/11   gm      Eliminating accessing private members of rex TCB
02/10/11   am      Adding QTF support.
12/02/10   gm      Added fixes for MOB naming conflicts.
07/30/09   fj      Removed FEATURE_LTE_PENDING for 2G/3G UT environment.
07/10/09   np      Adding LTE Code 
08/25/08   sv      Memory reduction changes
08/12/08   fj      Modified sdnv_nv_cmd() to support new API prl_commit2()
08/25/07   jqi/cl  Removed the cell/pcs band mapping features.
01/04/07   jqi     Lint fixes for barred API -sprintf.
08/15/06   pk      UOOS for GW modifications, added support to read GCF flag
                   from NV
05/04/06   jqi     Lint fixes.
02/08/06   pk      Replaced SD_ERR with SD_MSG_ERROR
01/09/06   sk      Added support for custom homes bitmap in NV.
12/02/05   jqi     Lint8 fixes.
11/02/05   sk      Fixed all Lint and compiler warnings.
10/05/05   sk      Added support for configuring SD values from NV.
02/24/05   pk      Fixed lint errors.
01/30/05   sj      Added support for WLAN.
01/07/05   jqi     Lint fixes.
12/04/04   jqi     Do not read PRL and SSPR_P_REV from NV if neither 1x nor
                   gw subscription is available.
11/02/04   jqi     Fixed lint warnings.
11/01/04   jqi     More read subscription related changes.
10/26/04   jqi     Read subscription items only when the subscription is
                   available.
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
04/19/04   jqi     Lint fix - for error level : error.
07/15/03   JQ      Added Extended PRL support
05/27/03   JQ      Featurize NV dependent code for IS683B PRL support.
05/06/03   JQ      Added IS683B PRL support.
03/24/03   RC      Changed some SD_ERR() to SD_MSG_ERR().
01/25/02   sj      Added support to read PUZL for User Zone feature.
09/07/01   RC      Added support for T53 feature.
02/26/01   RC      Changes to support dual-mode targets.
11/17/00   RC      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "sdnv.h"     /* External interface to sdnv.c */
#include "sd.h"       /* External interface to sd.c */
#include "sd_v.h"
#include "sddbg.h"    /* SD debug macros */

#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */

#include "nv.h"        /* Interface to NV services */
#include "rex.h"       /* Interface to rex services */

#include "prl.h"
#include "sdss.h"

#ifdef FEATURE_NV_RPC_SUPPORT
#include <string.h>
#endif

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

#include <stringl/stringl.h>

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* Type for holding the information that is associated with the SD
** NV-Interface component.
*/
typedef struct {

  /* Rex signal to be used by SD for waiting on NV to completes a read/write
  ** operation.
  */
  rex_sigs_type             nv_sig;


  /* Rex signal to be used by SD for watchdog reporting while SD waits on NV
  ** to complete a read/write operation.
  */
  rex_sigs_type             dog_sig;


  /* Pointer to a watchdog reporting function to be called by SD when the
  ** watchdog sig is set while SD waits on NV to complete a read/write
  ** operation.
  */
  void                      (*dog_rpt_func_ptr) (void);

} sdnv_s_type;


/* SD-Interface item.
*/
static sdnv_s_type  sdnv;


/*===========================================================================

FUNCTION sdnv_init

DESCRIPTION
  Initialize the SD NV-Interface component.

  This function must be called before the NV interface block
  is being used in any way, place or form.

DEPENDENCIES
  This function must be called before any other sdnv_xxx() function is
  ever called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN  void                      sdnv_init(

        rex_sigs_type             nv_sig,
            /* Rex signal to be used by SD for waiting on NV to completes a
            ** read/write operation.
            */

        rex_sigs_type             dog_sig,
            /* Rex signal to be used by SD for watchdog reporting while SD
            ** waits on NV to complete a read/write operation.
            */

        void                      (*dog_rpt_func_ptr) (void)
            /* Pointer to a watchdog reporting function to be called by SD
            ** when the watchdog sig is set while SD waits on NV to complete
            ** a read/write operation.
            */
)
{
  sdnv_s_type         *sdnv_ptr = &sdnv;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( nv_sig != 0 );
  SD_ASSERT( dog_sig != 0 );
  SD_ASSERT( dog_rpt_func_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the SD NV-Interface sigs and callback function.
  */
  sdnv_ptr->nv_sig              = nv_sig;
  sdnv_ptr->dog_sig             = dog_sig;
  sdnv_ptr->dog_rpt_func_ptr    = dog_rpt_func_ptr;

} /* sdnv_init */




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_wait

DESCRIPTION
  Suspend the SS-Client from executing until NV completes the read/write
  operation.

  Note that this function keep reporting to the watchdog while waiting
  for the NV signal to be set.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static  void                      sdnv_wait( void )
{

  sdnv_s_type       *sdnv_ptr   = &sdnv;
  rex_sigs_type     sigs;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for the NV signal to be set.
  */
  do
  {
    /* Wait on the NV and watchdog signals.
    */
    sigs = rex_wait( (rex_sigs_type) (sdnv_ptr->nv_sig | sdnv_ptr->dog_sig));

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the watchdog signal is set, call on the watchdog reporting function
    ** that was provided by the SS-Client.
    */
    if( sigs & sdnv_ptr->dog_sig )
    {
      sdnv_ptr->dog_rpt_func_ptr();
    }

  } while( ! (sigs & sdnv_ptr->nv_sig) );

} /* sdnv_wait */




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_nv_cmd

DESCRIPTION


DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static  void                      sdnv_nv_cmd(

        nv_cmd_type               *nv_cmd_ptr
           /* pointer to the NV command block */
)
{
  sdnv_s_type                     *sdnv_ptr   = &sdnv;
  sdss_subsc_e_type               subsc;
                                      /* Subscription type in the SS-core */

  #if defined(FEATURE_NV_RPC_SUPPORT) && defined (FEATURE_REX_OPAQUE_TCB_APIS)
  char rex_task_name[REX_TASK_NAME_LEN + 1];
  int err_code;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( nv_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get subscription status from SS-core.
  */
  subsc = sdss_get_subsc_type( SD_SS_MAIN );



  switch ( nv_cmd_ptr->cmd )
  {

    case NV_READ_F:

      /* If the subscription does not contains 1x subscription
      ** skip the following NV items.Or both 1x and GW subscription are
      ** unavailable, skip reading PRL.
      ** Otherwise send command to nv command queue as usual.
      */
      if( ( ! SDSS_SUBSC_CONTAIN( subsc, SDSS_SUBSC_1X ) &&
            ( nv_cmd_ptr->item == NV_HOME_SID_NID_I      ||
              nv_cmd_ptr->item == NV_SID_NID_I           ||
              nv_cmd_ptr->item == NV_ANALOG_HOME_SID_I   ||
              nv_cmd_ptr->item == NV_PCDMACH_I           ||
              nv_cmd_ptr->item == NV_SCDMACH_I           ||
              nv_cmd_ptr->item == NV_JTACS_PCDMACH_I     ||
              nv_cmd_ptr->item == NV_JTACS_SCDMACH_I))
                         ||
          ( ! SDSS_SUBSC_CONTAIN( subsc, SDSS_SUBSC_1X ) &&
            ! SDSS_SUBSC_CONTAIN( subsc, SDSS_SUBSC_GW ) &&
            ( nv_cmd_ptr->item == NV_SSPR_P_REV_I        ||
              nv_cmd_ptr->item == NV_ROAMING_LIST_683_I )))
      {

        /*
        ** Set status other than NV_NOTACTIVE_S or NV_FAIL_S
        ** to subtitue a default value and do not report an error in
        ** sdnv_read function.
        */

        nv_cmd_ptr->status = NV_NOTALLOC_S;

        return;
      }
      break;

    default:
      break;
  }/* switch */

  /* If coming here, do regular nv operation.*/
  nv_cmd_ptr->tcb_ptr = rex_self();

  #ifdef FEATURE_NV_RPC_SUPPORT
    
  #ifdef FEATURE_REX_OPAQUE_TCB_APIS
  /*lint -esym(119,rex_get_task_name) complains of false prototype mismatch */
  err_code = rex_get_task_name(nv_cmd_ptr->tcb_ptr, rex_task_name,
                               sizeof(rex_task_name), NULL);
  if (err_code == REX_SUCCESS && 0 == strncmp (rex_task_name,"ONCR", 4))
  #else
  if (0 == strncmp ((nv_cmd_ptr->tcb_ptr)->task_name,"ONCR", 4))
  #endif
  {

    (void)nv_cmd_remote(nv_cmd_ptr->cmd,
                  nv_cmd_ptr->item,
                  nv_cmd_ptr->data_ptr
                  );

  }
  else
  #endif
  {
    nv_cmd( nv_cmd_ptr );
  }

  sdnv_wait();
  (void) rex_clr_sigs( rex_self(), sdnv_ptr->nv_sig );

} /* sdnv_nv_cmd */




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_write

DESCRIPTION
  Write a SD related item to NV memory.

  Note that the NV write is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the write operation.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN  boolean                      sdnv_write(

        nv_items_enum_type        nv_item,
            /* NV item to write.
            */

        nv_item_type              *data_ptr
            /* Pointer to the data that is associated with the NV item.
            */
)
{
  sdnv_s_type         *sdnv_ptr   = &sdnv;
  nv_cmd_type         sdnv_cmd;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( data_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Prepare the NV write command buffer.
  */
  sdnv_cmd.item       = nv_item;          /* item to write */
  sdnv_cmd.cmd        = NV_WRITE_F;       /* NV operation */
  sdnv_cmd.data_ptr   = data_ptr;         /* data to write */
  sdnv_cmd.tcb_ptr    = rex_self();       /* Notify this task when done */
  sdnv_cmd.sigs       = sdnv_ptr->nv_sig; /* Signal with this sig when done */
  sdnv_cmd.done_q_ptr = NULL;             /* Return cmd to NO Q when done */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write to NV by:
  **
  ** 1. Clearing the NV signal.
  ** 2. Queuing the write command to the NV task.
  ** 3. Waiting for the NV task to finish servicing the write command.
  */
  (void) rex_clr_sigs( rex_self(), sdnv_ptr->nv_sig );
  sdnv_nv_cmd( &sdnv_cmd );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV write failed, generate an error.
  */
  if( sdnv_cmd.status != NV_DONE_S )
  {
    SD_MSG_MED_2( "NV write failed, item=%d, stat=%d",
                  sdnv_cmd.item, sdnv_cmd.status);

    return FALSE;
  }
  else
  {
    return TRUE;
  }

} /* sdnv_write */




/* <EJECT> */
/*===========================================================================

FUNCTION sdnv_read

DESCRIPTION
  Read a SD related item to NV memory.

  Note that the NV read is done in a synchronous fashion. That is, this
  function only returns after the NV task completes the read operation.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  TRUE if NV read operation is successful (or alternatively the NV item is
  set to a default value). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                  sdnv_read(

        nv_items_enum_type        nv_item,
            /* NV item to read.
            */

        nv_item_type              *data_ptr
            /* Pointer to a data buffer where to store the data that is
            ** associated with the NV item.
            */
)
{
  sdnv_s_type                    *sdnv_ptr   = &sdnv;
  nv_cmd_type                    sdnv_cmd;
  unsigned int                   i;
  sd_mru_table_entry_u_type      *sd_mru_entry_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( data_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Prepare the NV read command buffer.
  */
  sdnv_cmd.item       = nv_item;          /* item to read */
  sdnv_cmd.cmd        = NV_READ_F;        /* NV operation */
  sdnv_cmd.data_ptr   = data_ptr;         /* buffer to store read data  */
  sdnv_cmd.tcb_ptr    = rex_self();       /* Notify this task when done */
  sdnv_cmd.sigs       = sdnv_ptr->nv_sig; /* Signal with this sig when done */
  sdnv_cmd.done_q_ptr = NULL;             /* Return cmd to NO Q when done */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read to NV by:
  **
  ** 1. Clearing the NV signal.
  ** 2. Queuing the read command to the NV task.
  ** 3. Waiting for the NV task to finish servicing the read command.
  */
  (void) rex_clr_sigs( rex_self(), sdnv_ptr->nv_sig );
  sdnv_nv_cmd( &sdnv_cmd );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV the read operation is successful, return TRUE.
  */
  if( sdnv_cmd.status == NV_DONE_S )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the NV read operation has failed. If appropriate,
  ** substitute a default value for the NV item. Else return FALSE.
  */
  SD_MSG_MED_2( "NV read failed, item=%d, stat=%d",
                sdnv_cmd.item, sdnv_cmd.status);

  switch( sdnv_cmd.item )
  {
    case NV_HOME_SID_NID_I:
      data_ptr->home_sid_nid.pair[0].sid = SD_WILDCARD_SID;
      data_ptr->home_sid_nid.pair[0].nid = SD_WILDCARD_NID;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_SID_NID_I:
      data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][0].sid = SD_WILDCARD_SID;
      data_ptr->sid_nid.pair[NV_CDMA_MIN_INDEX][0].nid = SD_WILDCARD_NID;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_ANALOG_HOME_SID_I:
      data_ptr->analog_home_sid.sid = SD_WILDCARD_SID;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_MRU2_TABLE_I:

      for( i=0; i < ARR_SIZE(data_ptr->mru2_table.entry); i++ )
      {
        data_ptr->mru2_table.entry[i].mode = NV_MODE_INACTIVE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_MRU3_TABLE_I:

      for( i=0; i < ARR_SIZE(data_ptr->mru3_table.entry); i++ )
      {
        sd_mru_entry_ptr = (sd_mru_table_entry_u_type *)
                           &data_ptr->mru3_table.entry[i];
        sd_mru_entry_ptr->cdma.mode = (byte)SD_MODE_INACT;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_PCDMACH_I:
        data_ptr->pcdmach.channel_a = 283;
        data_ptr->pcdmach.channel_b = 384;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_SCDMACH_I:
        data_ptr->scdmach.channel_a = 691;
        data_ptr->scdmach.channel_b = 777;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_PRL_EXTENDED
    case NV_JTACS_PCDMACH_I:
        data_ptr->jtacs_pcdmach.channel_a = 76;
        data_ptr->jtacs_pcdmach.channel_b = 76;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_JTACS_SCDMACH_I:
        data_ptr->jtacs_scdmach.channel_a = 872;
        data_ptr->jtacs_scdmach.channel_b = 872;
      break;
  #endif
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_ROAMING_LIST_683_I:
    case NV_PUZL_ENABLED_I:

      /* Default value substitution is not appropriate for these items -
      ** return FALSE.
      */
      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_PRL_ENABLED_I:
      data_ptr->prl_enabled.enabled = TRUE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_SRDA_ENABLED_I:

      /* If FEATURE_MC_SILENT_REDIAL_AMPS is defined, default the SRDA to
      ** enabled. Else default the SRDA to disabled.
      */
      #ifdef FEATURE_MC_SILENT_REDIAL_AMPS
        data_ptr->srda_enabled = TRUE;
      #else
        data_ptr->srda_enabled = FALSE;
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_AMPS_BACKSTOP_ENABLED_I:

      /* For dual-band targets default AMPS backstop to enabled. For all
      ** other targets default AMPS backstop to disabled.
      */
      data_ptr->amps_backstop_enabled.enabled = FALSE;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_SID_NID_LOCK_I:
      for( i=0; i < ARR_SIZE(data_ptr->sid_nid_lock.pair); i++ )
      {
        data_ptr->sid_nid_lock.pair[i].sid = SD_WILDCARD_SID;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_SSPR_P_REV_I:
      data_ptr->sspr_p_rev = (byte) PRL_SSPR_P_REV_1;
      break;

    case NV_SD_CFG_ITEMS_I:
      /* Default value substitution taken care of in sdssscr_nv_items_init().
      */
      return FALSE;

    case NV_ROAM_IND_CUSTOM_HOME_I:
      /* Default value substitution taken care of in sdprl_custom_home_read().
      */
      return FALSE;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case NV_GPRS_ANITE_GCF_I:
      data_ptr->gprs_anite_gcf = FALSE;
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      SD_ERR_1( "Unexpected NV item=%d",sdnv_cmd.item);
      return FALSE;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write back the default value only if FEATURE_DISABLE_SD_INIT_NV_WRITE 
  ** is not defined
  */
  #ifdef FEATURE_DISABLE_SD_INIT_NV_WRITE
  return FALSE;
  #else
  /* If we got here, a default value was substituted for the NV item. If the
  ** NV read operation failed because of NV_NONACTIVE_S or NV_FAIL_S, attempt
  ** writing the default value back to NV.
  */
  if( sdnv_cmd.status == NV_NOTACTIVE_S ||
      sdnv_cmd.status == NV_FAIL_S )
  {
    SD_MSG_HIGH_1( "Write default value to NV, item=%d",sdnv_cmd.item);
    (void)sdnv_write( sdnv_cmd.item, sdnv_cmd.data_ptr );
  }
    
  return TRUE;
  #endif

} /* sdnv_read */
