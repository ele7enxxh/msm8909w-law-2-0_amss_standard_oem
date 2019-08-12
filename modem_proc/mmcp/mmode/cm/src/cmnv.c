/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 C A L L   M A N A G E R   N V   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager NV Interface block, which groups
  all the functionality that is a associated with CM<->NV interfacing.




EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmnv_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1998 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmnv.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/28/13   dk      propagate cr 469865, to assign default standby pref in dsds
01/23/13   qf      Remove nv/efs write during cm initialization
08/17/12   gm      Mainline EFS feature - Remove FEATURE_EFS
03/28/12   jh      Integrated RPM feature
03/23/12   gm      DIME integration fix: TMC deprecation and others.
02/15/12   vk      Replace usage of AEE library function with corresponding 
                   CoreBSP library
01/06/12   gm      Data synchronization changes for freefloating platform
07/11/11   gm      Access internal REX TCB members is removed
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/17/11   xs      Change LTe band Pref to NV EFS item with efs_get and efs_put
02/25/11   sn      Support for graceful pdp deactivation before detach.
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
01/19/10   rm      DSDS Auto-activation changes
12/23/10   sv      Integrate DSDS feature
12/03/10   gm      Fix to compilation warnings.
11/15/10   rm      Memory optimization changes
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
10/04/10   gm      Fix UIM crash by adding default channel to UIM req.
10/01/10   ak      Merged the Fusion code from Dev branch.
09/23/10   xs      Superlint critical/high error fixed
09/14/10   aj      create EFS directory for mmode if it doesnt exist
08/31/10   aj      Fix MOB warning regarding EFS file close
08/16/10   sv      update cmnv_get_uim_spc_status to use session based read API
07/22/10   aj      NV EFS support for LTE band pref
03/03/10   rn      Move to Auto with MM17 reject with new NV
02/15/10   cl      Added support for blocking certain pages during emergency
                   callback mode
02/01/10   aj      Add support for DCTM 4.0
01/19/10   aj      Added VOIP support for emergency call
11/19/09   sv      Removing dependency on virtuim.h and FEATURE_VIRTUAL_SIM
03/26/09   ks      Added support for disable_call_type to cmnv_data_s_type
03/24/09   sv      updated to cmtaski.h
03/03/09   mh      Lint clean up
02/20/09   rm      Lint clean post CMI changes
02/16/09   ks      Adding support for QDSP 6 based compiler
01/26/09   fj      Added nv_cmd_remote() function call for NV read and
                   NV write operations.
09/15/08   np      Added HPCD Rev B.
09/05/08   sv      Added explicit cast to remove compiler warnings
06/12/08   sv      Resolve Klocwork defects
01/03/08   ka      Adding handset based plus code dialing feature.
12/12/07   rm      Adding RAT-balancing feature support
08/07/07   ic      Lint clean up
07/16/07   sk      Removed featurization to avoid garbage values in
                   bss_type_pref when ADHOC feature is not defined.
06/05/07   pk      Added support to read mob_cai_rev from NV
03/09/07   sk      RAM usage reduction.
08/28/08   ka      Increasing the queue size for buffered NV items.
11/17/06   pk      Corrected featurization for Hybrid preferences
08/29/06   sk      Added support for savign WLAN Adhoc prefs.
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
03/05/06   jqi     Merged the WCDMA BC3,4,8,9 support.
01/13/06   ic      Fixed compile errors in cmnv_ruim_report()
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ka      Adding check for NV_GPRS_ANITE_GCF_I in cmnv_read_wait()
01/06/06   ic      Lint cleanup
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/03/05   pk      Added NV_DDTM_SETTINGS_I DDTM settings NV item
07/15/05   ic      Mainlined FEATURE_FACTORY_TESTMODE
06/06/05   pk      Modified cmnv_read_wait(), to read the access overload
                   class information
03/25/05   dk      Increase the NV Q size to 8
02/09/10   dk      Expanded cm_band_pref to 64 bit.
11/05/04   dk      Added WLAN Support.
08/20/04   sj      Added support for PRL Preference.
08/11/04   ka      Adding support for AMR. NV_GSM_AMR_CALL_CONFIG_I
08/04/04   ic      Removed enabling/disabling of Lint error 788 as it is
                   globally suppressed in Lint config files.
                   Enabling it locally in CM files caused inconsistent
                   Lint issues reports when a file was linted separately
                   versus when it was linted as part of *.c batch
08/03/04   ic      CM Lint clean up - cmnv.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/02/04   sj      Fixed lint errors.
05/15/04   ka      Merged changes from 6250. Fixed Lint errors.
02/06/04   ws      Initial jaguar/mainline merge.
10/31/03   ws      Remove T_RF_ZRF6200 feature.
10/28/03   ws      Remove uim pref slot support.
10/03/03   ws      Support for nv ps data orig pref
10/03/03   ws      jar's changes for USIM.
09/16/03   vt      Added preferred slot handling.
08/19/03   ws      Merged in changes for dual slot
08/08/03   vt      Made cm band pref bitmask to 32 bit enum
07/24/03   vt      Initialized the protocol and slot fields in the ACCESS
                   command to UIM.
07/11/03   ck      Initialized the protocol and slot fields in the STATUS
                   command to UIM.
04/29/03   AT      Lint Cleanup.
04/14/03   RI      Changed CM_ERR to CM_MSG_HIGH in cmnv_read_wait().
03/11/03   vt      Set the default value of NV ftm mode item to DMSS for 1x.
02/28/03   prk     Removed FEATURE_FTM_PREF_NORMAL_MODE.
02/28/03   prk     Added code to support FEATURE_FTM_PREF_NORMAL_MODE.
02/27/03   vt      Set the default value of NV ftm mode item to FTM.
02/18/03   vt      Fixed problems to build for HDR.
02/14/03   jqi     Added support for runtime FTM.
                   Limited it to CDMA only.
01/28/03   jar     Added Virtual RUIM Support, Feature FEATURE_VIRTUAL_SIM.
01/21/03   ws      Updated copyright information for 2003
11/19/02   vt      Added changing RTRE configuration.
                   Removed handling of powerup pref mode nv item.
10/31/02   vt      Mainlined feature SD20.
10/05/02   vt      Added NV_FTM_TEST_I.
06/12/02   RC      Use different NV command buffers for synchronous and
                   asynchronous NV read/write operations.
05/29/02   PRK     Modified cmnv_read_wait to process error statuses for
                   NV_ACQ_ORDER_PREF_I, NV_NET_SEL_MODE_PREF_I and
                   NV_SERVICE_DOMAIN_PREF_I.
03/05/02   az      Changed the signal name for NV read and write
02/06/02   sh      Added hybrid 3.0 support
12/11/01   sh      Added preliminary hybrid operation NV item support
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/23/01   VT/IC   Baseline for CCS
08/14/01   WLI     Added RUIM related functions.
03/14/01   RC      Fixed a problem where cmnv_q_write_done_cb was calling
                   cmnv_q_get twice instead of once.
02/07/01   RC      Added support for System Determination 2.0.
08/19/98   SH      Removed silent redial with apms feature flag
05/31/99   SH      Added CM 2.0 features
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */
#include "cmnv.h"      /* Interface to CM NV services */
#include "nv.h"        /* Interface to NV services */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "rex.h"       /* Interface to rex services */
#include <string.h>    /* Interface to string library */
#include "cmtaski.h"

#if defined( FEATURE_MMODE_CDMA_800 ) || defined( FEATURE_MMODE_CDMA_1900 )
#include "cmxll.h"      /* MC to CM interface*/
#endif

#ifdef FEATURE_UIM_RUIM
#include "uim.h"       /* Interface to R-UIM services */
#endif

#include "ftm.h"
#include "mcfg_fs.h"    /* Interface to MCFG efs wrappers */
#ifdef FEATURE_NV_RPC_SUPPORT
#include <string.h>

#include <stringl/stringl.h>

#endif

#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "cmmmgsdi.h"   /* interface to SIM card */
#endif

#ifdef CM_DEBUG
#error code not present
#endif

#ifdef FEATURE_DOMAIN_SELECTION
#include "cmsds.h"
#endif

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


static nv_cmd_type        cmnv_cmd_sync;
    /* NV command buffer to do NV synchronous NV write and read */

static nv_cmd_type        cmnv_cmd_async;
    /* NV command buffer to do NV asynchronous NV write and read */

/* Extended NV command structures used for DualSim
*/
#ifdef FEATURE_MMODE_DUAL_SIM
static nv_cmd_ext_type    cmnv_cmd_extn_sync;
    /* NV command buffer to do NV synchronous NV write and read */

static nv_cmd_ext_type    cmnv_cmd_extn_async;
    /* NV command buffer to do NV asynchronous NV write and read */
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/





/*===========================================================================
=============================================================================
=============================== NV WRITE QUEUE ==============================
=============================================================================
===========================================================================*/


/* Note that NV writes are done in asynchronous fashion; that is,
** initially write requests are only being queued in the CM NV queue.
** The CM NV queue then services the requests in the background.
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for cmnv item. Such an item stores all the information that
** is needed to buffer an NV write operation.
*/
typedef struct {

    nv_items_enum_type    item;      /* NV item */
    cmnv_data_s_type      data;      /* Data associated with NV item */
    uint16                context;

} cmnv_item_s_type;



#define CM_NV_Q_SIZE  20
    /* Size of CM NV Q */


static CIRC_Q_DEF( cmnv_item_s_type, cmnv_q, CM_NV_Q_SIZE );
    /* Queue to buffer NV write requests */




/*===========================================================================

FUNCTION cmnv_q_init

DESCRIPTION
  Initialize the NV queue object.

  This function must be called before the NV queue object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void  cmnv_q_init( void )
{
  CIRC_Q_INIT( cmnv_q )
}




/*===========================================================================

FUNCTION cmnv_q_put

DESCRIPTION
  Enqueue a CM NV item onto the NV queue.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  TRUE if item is successfully enqueued.
  FALSE if queue is full and item cannot be enqueued.

SIDE EFFECTS
  none

===========================================================================*/
static boolean  cmnv_q_put(

    cmnv_item_s_type  *item_ptr
        /* pointer to CM NV item to be enqueued */
)
/*lint -save -e818*/
{
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return CIRC_Q_PUT( cmnv_q, item_ptr );

}
/*lint -restore*/




/*===========================================================================

FUNCTION cmnv_q_get

DESCRIPTION
  Dequeue a CM NV from the NV queue.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  TRUE if item is dequeued successfully.
  FALSE if queue is empty and NO item is dequeued.

SIDE EFFECTS
  none

===========================================================================*/
static boolean  cmnv_q_get(

    cmnv_item_s_type  **item_ptr
        /* pointer to CM NV item to receive dequeued item */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return CIRC_Q_GET( cmnv_q, *item_ptr );

}




/*===========================================================================

FUNCTION cmnv_q_check

DESCRIPTION
  Return a pointer to item at the head of the CM NV queue. Note that the
  item is not removed from the queue.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  A pointer to item at the head of the CM NV queue.
  If the queue is empty, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cmnv_item_s_type  *cmnv_q_check( void )
{
  return CIRC_Q_CHECK( cmnv_q );
}




/*===========================================================================

FUNCTION cmnv_q_cnt

DESCRIPTION
  Return the number of items currently queued on the CM NV queue.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  Return the number of items currently queued on the CM NV queue.

SIDE EFFECTS
  none

===========================================================================*/
static unsigned int cmnv_q_cnt( void )
{
  return CIRC_Q_CNT( cmnv_q );
}




/*===========================================================================

FUNCTION cmnv_q_srv

DESCRIPTION
  Services the cmnv_q by writing the item at the head of the queue to NV.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmnv_q_srv( void )
{

  /* Point at item at the head of the CM NV Q.
  */
  cmnv_item_s_type *item_ptr = cmnv_q_check();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If Q is NOT empty, prepare the NV write command
  ** and send it to be serviced by the NV task.
  */
  if( item_ptr != NULL )
  {
    cmnv_cmd_async.item       = item_ptr->item;/* item to write */
    cmnv_cmd_async.cmd        = NV_WRITE_F;    /* NV operation */
    /*lint -save -e826 */
    cmnv_cmd_async.data_ptr   = (nv_item_type*) &item_ptr->data;/* data to write */
    /*lint -restore */
    cmnv_cmd_async.tcb_ptr    = rex_self();    /* Notify CM task when done */
    cmnv_cmd_async.sigs       = CM_NV_ASYNC_SIG;/* Signal with this sig when done */
    cmnv_cmd_async.done_q_ptr = NULL;          /* Return cmd to NO Q when done */

    #ifdef FEATURE_MMODE_DUAL_SIM
    /* Add the context and queue the command to NV
    */
    cmnv_cmd_extn_async.nvcmd   = &cmnv_cmd_async;
    cmnv_cmd_extn_async.context = item_ptr->context;
    nv_cmd_ext( &cmnv_cmd_extn_async );

    #else
    nv_cmd( &cmnv_cmd_async );                 /* Send command to NV task */
    #endif
  }

}




/*===========================================================================

FUNCTION cmnv_q_write_done_cb

DESCRIPTION
  Callback function for completion of NV write operations

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  Dequeue item from the CM NV queue.

===========================================================================*/
void cmnv_q_write_done_cb( void )
{


  cmnv_item_s_type* nv_item = NULL;
      /* Container to hold dequeued NV item */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue should NOT be empty!.
  */
  //CM_ASSERT( cmnv_q_cnt() > 0 );

  /* Dequeue item from head of CM NV queue into the
  ** nv_item buffer. If queue is empty log an error and return.
  */
  if( ! cmnv_q_get(&nv_item) )
  {
    CM_MSG_HIGH_0( "cmnv_q is empty!" );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If status is other than NV_DONE_S,
  ** NV write failed!
  */
  if( cmnv_cmd_async.status != NV_DONE_S )
  {
    CM_ERR_1( "NV write failed, stat=%d",
            cmnv_cmd_async.status);
  }


  if (nv_item != NULL)
  {
    cm_mem_free(nv_item);
    nv_item = NULL;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Service the next item (if any).
  */
  cmnv_q_srv();

}

/*===========================================================================

FUNCTION cmnv_map_efs_id_to_filename

DESCRIPTION
 Maps efs id to filename

DEPENDENCIES
  None

RETURN VALUE
  File path name

SIDE EFFECTS
  None.

===========================================================================*/

const char *            cmnv_map_efs_id_to_filename(

        cmnv_efs_id_e_type          efs_id
            /* passing the efs Id to map
            */

)
{
   static const struct {

       cmnv_efs_id_e_type    efs_id;

       const char            *file_name;
           /* EFS file name  */

  } cmnv_efs_id_to_path_mapping_tbl[] = {

     { CMNV_EFS_ID_RPM_INFO,     "/nv/item_files/modem/mmode/rpm_info" },
     { CMNV_EFS_ID_RPM_MCC_MNC_LIST, "/nv/item_files/data/3gpp/rpm_suppported_sim" },
     { CMNV_EFS_ID_TDS_RPM_INFO, "/nv/item_files/modem/mmode/tds_cmrpm_info"},
     { CMNV_EFS_ID_MODE_DISABLED_IN_TSTS, "/nv/item_files/modem/mmode/mode_disabled_in_TSTS"},
     { CMNV_EFS_ID_RPM_SYS_TIME,     "/nv/item_files/modem/mmode/rpm_sys_time"},
     { CMNV_EFS_ID_MTCSFB_CALL_REDIAL_OVER_1X, "/nv/item_files/modem/mmode/mtcsfb_call_redial"},
  };

  if(!BETWEEN( efs_id,
               CMNV_EFS_ID_NONE,
               CMNV_EFS_ID_MAX))
  {
    /* We should not get to this point, since error handling
    ** is already being done in calling function
    */
    sys_err_fatal_invalid_value_exception(efs_id);
  }
  return cmnv_efs_id_to_path_mapping_tbl[efs_id].file_name;
}


/*===========================================================================
=============================================================================
=========================== EXTERNALIZED FUNCTIONS ==========================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION cmnv_efs_write

DESCRIPTION
  Write contents of data_ptr to EFS item denoted by efs_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void                  cmnv_efs_write(

        cmnv_efs_id_e_type          efs_id,
            /* passing the efs Id to write.
            */
        const byte                  *data_ptr,
            /* Pointer to a data buffer to write to EFS.
            */
        int32                       size
            /* Size of the data_ptr buffer.
            */
)
{
  mcfg_fs_status_e_type       status;
  const char                  *file_name;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( data_ptr != NULL );
  CM_ASSERT( size     != 0 );
  if(!BETWEEN( efs_id,
               CMNV_EFS_ID_NONE,
               CMNV_EFS_ID_MAX))
  {
    CM_ERR_1( "write:Bad efs_id = %d",efs_id);
    return;
  }

  /* Map efs_id to filename
  */
  file_name = cmnv_map_efs_id_to_filename(efs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Open the specified resource file.
  ** If open fails, return error code
  */
  CM_MSG_HIGH_1( "Put file=%s", file_name );

  status = mcfg_fs_write ( file_name, 
                           (void*)data_ptr, 
                           size,
                           MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR, 
                           MCFG_FS_ALLPERMS, 
                           MCFG_FS_TYPE_EFS, 
                           MCFG_FS_SUBID_0 );

  if( status != MCFG_FS_STATUS_OK )
  {
    CM_ERR_2( "mcfg fs write fail status=%d, errno %d", status, mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return;
  }

  return;

}

/*===========================================================================

FUNCTION cmnv_efs_read

DESCRIPTION
  Read EFS item denoted by efs_id and copy to data_ptr.
  If not read successfully, substitute default values.

DEPENDENCIES
  None

RETURN VALUE
  *size_ptr is set to the actual number of bytes that were copied into the
  buffer pointed by data_ptr. If the read failed and there is no default
  value defined, *size_ptr = 0

SIDE EFFECTS
  None.


==========================================================================*/
void                  cmnv_efs_read(

        cmnv_efs_id_e_type          efs_id,
            /* passing the efs Id to read.
            */
        byte                        *data_ptr,
            /* Pointer to a data buffer where to store the data that is
            ** associated with the EFS item.
            */
        int32                       *size_ptr
            /* On input this parameter is the  maximum number
            ** of bytes to be copied into the buffer pointed by data_ptr.
            ** This parameter must not be nil.
            ** On output it is set *size_ptr to the actual number of bytes
            ** that were copied into the buffer pointed by data_ptr.
            */
)
{
  const char                  *file_name;
  mcfg_fs_status_e_type       status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( data_ptr != NULL );
  CM_ASSERT( size_ptr != NULL );
  if(!BETWEEN( efs_id,
               CMNV_EFS_ID_NONE,
               CMNV_EFS_ID_MAX))
  {
    CM_ERR_1( "read:Bad efs_id = %d",efs_id);
    return;
  }

  /* Map efs_id to filename
  */
  file_name = cmnv_map_efs_id_to_filename(efs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 
  status = mcfg_fs_read (file_name, 
                         data_ptr, 
                         *size_ptr, 
                         MCFG_FS_TYPE_EFS, 
                         MCFG_FS_SUBID_0 );

  if(status != MCFG_FS_STATUS_OK)
  {
    CM_MSG_HIGH_3("get efs=%d status=%d errno=%d", efs_id, status, mcfg_fs_errno(MCFG_FS_TYPE_EFS));

    /* If read failed, substitute a default value
    */
    switch(efs_id)
    {

      case CMNV_EFS_ID_MODE_DISABLED_IN_TSTS:
      {
        uint16 mode_disabled_in_TSTS = SYS_SYS_MODE_MASK_NONE;
        *size_ptr = (int32)MIN(sizeof(uint16),(uint32)*size_ptr);
        memscpy( data_ptr, (uint32)*size_ptr, &mode_disabled_in_TSTS, sizeof(mode_disabled_in_TSTS) );
      }
      CM_MSG_HIGH_0("Use default CMNV_EFS_ID_MODE_DISABLED_IN_TSTS as SYS_SYS_MODE_MASK_NONE");
      break;

      case CMNV_EFS_ID_MTCSFB_CALL_REDIAL_OVER_1X:
      {
        boolean is_mtcsfb_call_redial = FALSE;
        *size_ptr = (int32)MIN(sizeof(boolean),(uint32)*size_ptr);
        *data_ptr = is_mtcsfb_call_redial;
        memscpy( data_ptr, (uint32)*size_ptr, &is_mtcsfb_call_redial, sizeof(is_mtcsfb_call_redial) );
      }
      CM_MSG_HIGH_0("Use default CMNV_EFS_ID_MTCSFB_CALL_REDIAL_OVER_1X as FALSE");
      break;

      case CMNV_EFS_ID_NONE:
      case CMNV_EFS_ID_MAX:
      default:
        *size_ptr = 0;
        break;
    }//switch(efs_id)
  }

  return;
}

/*===========================================================================

FUNCTION cmnv_init

DESCRIPTION
  Initialize the NV interface block.

  This function must be called before the NV interface block
  is being used in any way, place or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_init( void )
{

  /* Initializes cmnv_q.
  */
  cmnv_q_init();
}




/*===========================================================================

FUNCTION cmnv_write

DESCRIPTION
  Write an item to NV memory.

  Note that NV writes are done in asynchronous fashion; that is, upon
  return of this function the write request is only being queued in the
  CM NV queue. The CM NV queue then services the request in the background.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_write(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    const nv_item_type    *data_ptr
        /* pointer to data associated with the NV item */
)
{
  cmnv_write_extn(nv_item, data_ptr, 0);
}



/*===========================================================================

FUNCTION cmnv_write_extn

DESCRIPTION
  Write an item to NV memory.

  Note that NV writes are done in asynchronous fashion; that is, upon
  return of this function the write request is only being queued in the
  CM NV queue. The CM NV queue then services the request in the background.

DEPENDENCIES
  CM NV queue must have already been initialized with
  cmnv_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_write_extn(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    const nv_item_type    *data_ptr,
        /* pointer to data associated with the NV item */

    uint16                context

)
{
  cmnv_item_s_type*  cmnv_item = NULL;
      /* Container to hold NV item to be enqueued */


  
  cmnv_item = ( cmnv_item_s_type*)cm_mem_malloc(sizeof(cmnv_item_s_type));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(data_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Prepare item for enqueuing.
  */
  cmnv_item->item    = nv_item;
  cmnv_item->data    = *((cmnv_data_s_type*) data_ptr);
  cmnv_item->context = context;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Try enqueuing item onto the CM NV write queue.
  */
  if( cmnv_q_put(cmnv_item) )
  {
    CM_MSG_LOW_1( "cmnv_write(), nv_q_cnt=%d",cmnv_q_cnt());

    /* Enqueuing is successful - if queue was previously
    ** empty, start servicing the queue now.
    */
    if( cmnv_q_cnt() == 1 )
    {
      cmnv_q_srv();
    }
  }
  else
  {
    /* Queue is full!
    */
    CM_MSG_MED_1( "NV write Q is full, nv_item=%d",cmnv_item->item);
  }
}


/*===========================================================================

FUNCTION cmnv_write_wait_per_subs

DESCRIPTION
  Write an item from the NV memory in a synchronous fashion; that is, this
  function only returns  after the NV write is done being serviced by the
  NV task.

  This function is only expected to be used during power-up initialization.
  During operation cmnv_write() must be used to write items to NV.

DEPENDENCIES
  mc_task_start() must have already been called.

  This function is only expected to be used during power-up initialization.
  During operation cmnv_write() must be used to items to NV.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_write_wait_extn(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    nv_item_type          *data_ptr,
        /* pointer to data associated with the NV item */

    uint16                context
        /* Context of the subscription */

)
/*lint -esym(715, context) */
{

  /* Prepare the NV write command
  */
  cmnv_cmd_sync.item       = nv_item;       /* Item to write */
  cmnv_cmd_sync.cmd        = NV_WRITE_F;    /* NV operation */
  cmnv_cmd_sync.data_ptr   = data_ptr;      /* Read into this buffer */
  cmnv_cmd_sync.tcb_ptr    = rex_self();    /* Notify CM task when done */
  cmnv_cmd_sync.sigs       = CM_NV_SYNC_SIG;/* Signal with this sig when done */
  cmnv_cmd_sync.done_q_ptr = NULL;          /* Return cmd to NO Q when done */

  #ifdef FEATURE_MMODE_DUAL_SIM
  /* populate the extended nv command with the NV context
  */
  cmnv_cmd_extn_sync.context = context;
  cmnv_cmd_extn_sync.nvcmd   = &cmnv_cmd_sync;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Write from NV by:
  ** 1. Clearing the NV signal
  ** 2. Queuing a write command to the NV task
  ** 3. Waiting for NV task to finish servicing the write command.
  */
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );
  #ifdef FEATURE_MMODE_DUAL_SIM
  nv_cmd_ext( &cmnv_cmd_extn_sync );
  #else
  nv_cmd( &cmnv_cmd_sync );
  #endif
  cm_wait( CM_NV_SYNC_SIG );
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV write failed, generate an error message.
  */
  if( cmnv_cmd_sync.status != NV_DONE_S )
  {
    CM_ERR_2( "NV write failed, item=%d, stat=%d",
             cmnv_cmd_sync.item, cmnv_cmd_sync.status);
  }
} /* cmnv_write_wait_extn */
/*lint -esym(715, context) */


/*===========================================================================

FUNCTION cmnv_write_wait

DESCRIPTION
  Write an item from the NV memory in a synchronous fashion; that is, this
  function only returns  after the NV write is done being serviced by the
  NV task.

  This function is only expected to be used during power-up initialization.
  During operation cmnv_write() must be used to write items to NV.

DEPENDENCIES
  mc_task_start() must have already been called.

  This function is only expected to be used during power-up initialization.
  During operation cmnv_write() must be used to items to NV.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_write_wait(

    nv_items_enum_type    nv_item,
        /* NV item to write */

    nv_item_type          *data_ptr
        /* pointer to data associated with the NV item */
)
{
  cmnv_write_wait_extn(nv_item, data_ptr, 0);
}



/*===========================================================================

FUNCTION cmnv_read_wait_extn

DESCRIPTION
  Read an item from the NV memory.

  Note that NV reads are done in a synchronous fashion; that is, this
  function only returns  after the NV read is done being serviced by the
  NV task.

DEPENDENCIES
  mc_task_start() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
nv_stat_enum_type cmnv_read_wait_extn(

    nv_items_enum_type  nv_item,
        /* NV item to read */

    nv_item_type        *data_ptr,
        /* pointer to buffer to place data associated with the NV item */

    uint16              context
        /* Context of the subscription */

)
{
  nv_stat_enum_type nv_read_status = NV_DONE_S;
  #if defined(FEATURE_NV_RPC_SUPPORT) && defined (FEATURE_REX_OPAQUE_TCB_APIS)
  char rex_task_name[REX_TASK_NAME_LEN + 1];
  int err_code;
  #endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(data_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* Prepare the NV read command
  */
  cmnv_cmd_sync.item       = nv_item;       /* Item to read */
  cmnv_cmd_sync.cmd        = NV_READ_F;     /* NV operation */
  cmnv_cmd_sync.data_ptr   = data_ptr;      /* Read into this buffer */
  cmnv_cmd_sync.tcb_ptr    = rex_self();    /* Notify CM task when done */
  cmnv_cmd_sync.sigs       = CM_NV_SYNC_SIG;/* Signal with this sig when done */
  cmnv_cmd_sync.done_q_ptr = NULL;          /* Return cmd to NO Q when done */

  #ifdef FEATURE_MMODE_DUAL_SIM
  /* set the extended nv command with context
  */
  cmnv_cmd_extn_sync.context = context;
  cmnv_cmd_extn_sync.nvcmd   = &cmnv_cmd_sync;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV by:
  ** 1. Clearing the NV signal
  ** 2. Queuing a read command to the NV task
  ** 3. Waiting for NV task to finish servicing the read command.
  */
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );

  cmnv_cmd_sync.tcb_ptr = rex_self();

  #ifdef FEATURE_NV_RPC_SUPPORT
  #ifdef FEATURE_REX_OPAQUE_TCB_APIS
  /*lint -esym(119,rex_get_task_name) complains of false prototype mismatch */
  err_code = rex_get_task_name(cmnv_cmd_sync.tcb_ptr, rex_task_name,
                               sizeof(rex_task_name), NULL);
  if ((err_code == REX_SUCCESS) && (0 == strncmp (rex_task_name,"ONCR", 4)))
  #else
  if (0 == strncmp ((cmnv_cmd_sync.tcb_ptr)->task_name,"ONCR", 4))
  #endif
  {
    (void)nv_cmd_remote(cmnv_cmd_sync.cmd,
                        cmnv_cmd_sync.item,
                        cmnv_cmd_sync.data_ptr
                        );
  }
  else
  #endif
  {
    #ifdef FEATURE_MMODE_DUAL_SIM
    nv_cmd_ext( &cmnv_cmd_extn_sync );
    #else
    nv_cmd( &cmnv_cmd_sync );
    #endif
  }

  cm_wait( CM_NV_SYNC_SIG );
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );
  
  //store the NV return value in a local variable
  //since cmnv_cmd_sync.status can be modified while restoring default value

  nv_read_status = cmnv_cmd_sync.status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV read failed, substitute a default value.
  */
  if( cmnv_cmd_sync.status != NV_DONE_S )
  {
      CM_MSG_HIGH_3( "NV read failed, item=%d, stat=%d, context=%d",\
             cmnv_cmd_sync.item, cmnv_cmd_sync.status, context);

    /*lint -save -e641 */
    switch( cmnv_cmd_sync.item )
    {
      case NV_PREF_MODE_I:

        /* If this target supports ACP, default the mode preference
        ** to automatic. Else, default mode preference to digital only.
        */
        if (context == 0)
        {
        data_ptr->pref_mode.mode = NV_MODE_DIGITAL_ONLY;
        }
        else
        {
          // Keeping mode_pref as GSM only, otherwise DIGITAL will change to GW, 
          // based on feature flag definition
          data_ptr->pref_mode.mode = NV_MODE_GSM_ONLY;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_SYSTEM_PREF_I:
        data_ptr->system_pref.sys = NV_SP_STANDARD;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_BAND_PREF_I:
        data_ptr->band_pref.band = (nv_band_pref_enum_type)CM_BAND_PREF_ANY;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_BAND_PREF_16_31_I:
        data_ptr->band_pref.band = (nv_band_pref_enum_type)(CM_BAND_PREF_ANY >> 16) ;
                                               //(NV_BAND_PREF_ANY & 0xFFFF);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_BAND_PREF_32_63_I:
        data_ptr->band_pref_32_63.band = (uint32)(CM_BAND_PREF_ANY >> 32 );

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_ROAM_PREF_I:
        data_ptr->roam_pref.roam = NV_ROAM_PREF_ANY;
        break;

      #if (defined (FEATURE_HDR_HYBRID))
       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_HYBRID_PREF_I:
        data_ptr->hybrid_pref = CM_HYBR_PREF_ON;
        break;
      #endif /* FEATURE_HDR_HYBRID */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_CURR_NAM_I:
        data_ptr->curr_nam = (int)CM_NAM_1;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_FTM_MODE_I:
        #if defined( FEATURE_MMODE_CDMA_800 ) || defined( FEATURE_MMODE_CDMA_1900 )
        data_ptr->ftm_mode = DMSS_MODE;
        #else
        data_ptr->ftm_mode = AMSS_MODE;
        #endif /* defined( FEATURE_MMODE_CDMA_800 ) || defined( FEATURE_MMODE_CDMA_1900 ) */
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

       case NV_AUTO_NAM_I:
         data_ptr->auto_nam = FALSE;
         break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_SRDA_ENABLED_I:
        data_ptr->srda_enabled = FALSE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_SEC_CODE_I:
        (void) memset( (byte*) data_ptr->sec_code.digits,
                       '0',
                       sizeof(data_ptr->sec_code.digits) );
        return nv_read_status;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case NV_ACQ_ORDER_PREF_I:
      #ifdef FEATURE_WCDMA
        data_ptr->acq_order_pref.acq_order = CM_GW_ACQ_ORDER_PREF_WCDMA_GSM;
      #else
        data_ptr->acq_order_pref.acq_order = CM_GW_ACQ_ORDER_PREF_GSM_WCDMA;
      #endif
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_NET_SEL_MODE_PREF_I:
        data_ptr->net_sel_mode_pref.net_sel_mode = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_SERVICE_DOMAIN_PREF_I:
        data_ptr->service_domain_pref.srv_domain = CM_SRV_DOMAIN_PREF_CS_ONLY;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #if (defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE))
      case NV_RTRE_CONFIG_I:
        data_ptr->rtre_config = NV_RTRE_CONFIG_NV_ONLY;
        break;
      #endif /* (defined(FEATURE_UIM_RUIM) && defined(FEATURE_UIM_RUN_TIME_ENABLE)) */

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_UIM_CDMA_PREF_SLOT_I:
        data_ptr->uim_cdma_pref_slot = NV_UIM_SLOT_NONE;
        break;

      case NV_UIM_GSM_PREF_SLOT_I:
        data_ptr->uim_gsm_pref_slot = NV_UIM_SLOT_NONE;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case NV_PS_DATA_ORIG_PREF_I:
        data_ptr->ps_data_orig_pref = NV_PS_DATA_ORIG_PREF_ANY;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     case NV_GSM_AMR_CALL_CONFIG_I:
        data_ptr->gsm_amr_call_config = FALSE;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_PRL_PREF_I:
        data_ptr->prl_pref.prl = CM_PRL_PREF_ANY;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_ACCOLC_I:

        (void) memset( (byte*)  data_ptr->accolc.ACCOLCpClass,
                       0,
                       sizeof( data_ptr->accolc.ACCOLCpClass) );
        return nv_read_status;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_DDTM_CNTL
      case NV_DDTM_SETTINGS_I:

        /* Return error, the calling function should write the
        ** correct values to NV
        */
        data_ptr->ddtm_settings.num_srv_opt = CM_INVALID_DDTM_NUM_SRV_OPT;
        return nv_read_status;

      case NV_DDTM_ALLOW_SO_PAGES_I:

        /* Return error, the calling function should write the
        ** correct values to NV
        */
        data_ptr->ddtm_allow_so_pages.allow_num_srv_opt = CM_INVALID_DDTM_NUM_SRV_OPT;
        return nv_read_status;
      #endif


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_GPRS_ANITE_GCF_I:
        data_ptr->gprs_anite_gcf = FALSE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_UMTS_CALL_VT_CODEC_LIST_I:
        data_ptr->umts_call_vt_codec_list = FALSE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_MOB_CAI_REV_I:
        #if (defined( FEATURE_MMODE_CDMA_800 ) || defined( FEATURE_MMODE_CDMA_1900 ) && \
             !defined(FEATURE_MMODE_REMOVE_1X))
        /* P_REV_IS2000 (6) is the default suggested by CP
        */
        data_ptr->mob_cai_rev = P_REV_IS2000;
        #else
        data_ptr->mob_cai_rev = 0;
        #endif
        break;

      case NV_ENS_ENABLED_I:
        #ifdef CM_GW_SUPPORTED
        data_ptr->ens_enabled = FALSE;
        #endif
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_HOME_MCC_I:
        data_ptr->home_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_USR_SID_TO_MCC_ASSOC_TBL_I:
        data_ptr->usr_sid_to_mcc_assoc_tbl.tbl_len = 0;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_HS_BASED_PLUS_DIAL_SETTING_I:
        data_ptr->hs_based_plus_dial_setting = CM_HS_BASED_PLUS_DIAL_DISABLED;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_IMSI_MCC_I:
        data_ptr->imsi_mcc.imsi_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_DISABLE_CM_CALL_TYPE_I:
        data_ptr->disable_cm_call_type = CM_CALL_TYPE_NONE_MASK;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef  FEATURE_VOIP_E911_SUPPORT
      case NV_CS_TO_VOIP_FALLBACK_TIMER_I:
        data_ptr->cs_to_voip_fallback_timer =
                          CM_DEFAULT_CS_TO_VOIP_FALLBACK_TIME;
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_VOIP_CANCEL_RETRY_TIMER_I:
        data_ptr->voip_cancel_retry_timer =
                           CM_DEFAULT_VOIP_CANCEL_RETRY_TIME;
        break;
     #endif //  FEATURE_VOIP_E911_SUPPORT

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #if defined FEATURE_CALL_THROTTLE && defined FEATURE_EHRPD
      case NV_HARD_FAIL_THR_TIME_I:
        data_ptr->hard_fail_thr_time = CMSS_HARD_FAIL_THROTTLE_TIME;
        break;
      #endif

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

       case NV_LU_REJECT_AUTO_ENABLED_I:
         data_ptr->lu_reject_auto_enabled = FALSE;
         break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_DUAL_STANDBY_CFG_ITEMS_I:
        data_ptr->dual_standby_cfg_items.dedicated_voice_subs = SYS_MODEM_AS_ID_1;
        data_ptr->dual_standby_cfg_items.dedicated_data_subs = SYS_MODEM_AS_ID_1;
        data_ptr->dual_standby_cfg_items.active_subs = SYS_MODEM_AS_ID_1_MASK;
        data_ptr->dual_standby_cfg_items.priority_subs = SYS_MODEM_AS_ID_1;
        data_ptr->dual_standby_cfg_items.dual_standby_pref = SYS_MODEM_DS_PREF_AUTO;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_GRACEFUL_PDP_DEACTIVATION_B4_DETACH
      case NV_PDP_DEACTIVATE_BEFORE_DETACH_SUPPORTED_I:
        data_ptr->pdp_deactivate_before_detach_supported = FALSE;
        break;
      #endif

      case NV_GSMUMTS_IMSI_I:
        memset( (byte*) data_ptr->gsmumts_imsi, 0, sizeof(data_ptr->gsmumts_imsi));
      break;

      case NV_UE_IMEI_I:
        memset( (byte*) data_ptr->ue_imei.ue_imei, 0, sizeof(data_ptr->ue_imei));
      break;

      case NV_ESN_ME_I:
        memset( (byte*) data_ptr->esn_me.esn_me, 0, sizeof(data_ptr->esn_me.esn_me));
      break;

      case NV_MEID_ME_I:
        data_ptr->meid_me = 0;
      break;

      case NV_IMSI_11_12_I:
        data_ptr->imsi_11_12.imsi_11_12=0;
        data_ptr->imsi_11_12.nam=0;
      break;

      case NV_MIN2_I:
        data_ptr->min2.nam=0;
	memset( (byte*) data_ptr->min2.min2, 0, sizeof(data_ptr->min2.min2));
      break;

      case NV_MIN1_I:
        data_ptr->min1.nam=0;
	memset( (byte*) data_ptr->min1.min1, 0, sizeof(data_ptr->min1.min1));
      break;

      default:
        CM_ERR_1( "Unexpected NV item=%d",cmnv_cmd_sync.item);

    } /* switch */
    /*lint -restore */

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Only if FEATURE_DISABLE_CM_INIT_NV_WRITE is NOT defined, then perform nv write
   */  
  #ifndef FEATURE_DISABLE_CM_INIT_NV_WRITE
  /* If NV read failed because of NV_NOTACTIVE_S or NV_FAIL_S
  ** try writing the defaulted value back to NV.
  */
  if( cmnv_cmd_sync.status == NV_NOTACTIVE_S ||
      cmnv_cmd_sync.status == NV_FAIL_S )
  {
    CM_MSG_HIGH_1("Write default value to NV, item=%d",cmnv_cmd_sync.item);

    /* Write to NV by:
    ** 1. Indicating a write command.
    ** 2. Clearing the NV signal
    ** 3. Queuing a write command to the NV task
    ** 4. Waiting for NV task to finish servicing the write command.
    */
    cmnv_cmd_sync.cmd = NV_WRITE_F;
    (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );

    cmnv_cmd_sync.tcb_ptr = rex_self();

    #ifdef FEATURE_MMODE_DUAL_SIM
    /* populate the extended nv command with the NV context
    */
    cmnv_cmd_extn_sync.context = context;
    cmnv_cmd_extn_sync.nvcmd   = &cmnv_cmd_sync;
    #endif

    #ifdef FEATURE_NV_RPC_SUPPORT
    #ifdef FEATURE_REX_OPAQUE_TCB_APIS
    /*lint -esym(119,rex_get_task_name) complains false prototype mismatch */
    err_code = rex_get_task_name(cmnv_cmd_sync.tcb_ptr, rex_task_name,
                                 sizeof(rex_task_name), NULL);
    if ((err_code == REX_SUCCESS) && (0 == strncmp (rex_task_name,"ONCR", 4)))
    #else
    if (0 == strncmp ((cmnv_cmd_sync.tcb_ptr)->task_name,"ONCR", 4))
    #endif
    {
      (void)nv_cmd_remote(cmnv_cmd_sync.cmd,
                          cmnv_cmd_sync.item,
                          cmnv_cmd_sync.data_ptr
                          );
    }
    else
    #endif
    {
      #ifdef FEATURE_MMODE_DUAL_SIM
      nv_cmd_ext( &cmnv_cmd_extn_sync );
      #else
      nv_cmd( &cmnv_cmd_sync );
      #endif
    }
  
    cm_wait( CM_NV_SYNC_SIG );
    (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );
  }
  #endif

  return nv_read_status;

}

/*===========================================================================

FUNCTION cmnv_read_wait

DESCRIPTION
  Read an item from the NV memory.

  Note that NV reads are done in a synchronous fashion; that is, this
  function only returns  after the NV read is done being serviced by the
  NV task.

DEPENDENCIES
  mc_task_start() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
nv_stat_enum_type cmnv_read_wait(

    nv_items_enum_type  nv_item,
        /* NV item to read */

    nv_item_type        *data_ptr
        /* pointer to buffer to place data associated with the NV item */
)
{
  return cmnv_read_wait_extn(nv_item, data_ptr, 0);
}

#ifdef FEATURE_UIM_RUIM

#ifndef FEATURE_MMGSDI_SESSION_LIB
/*---------------------------------------------------------------------------
                          R-UIM INTERFACE BLOCK
---------------------------------------------------------------------------*/

/* Status reported by R-UIM for each command */
static uim_rpt_status cm_uim_status_report = UIM_FAIL;

/*===========================================================================

FUNCTION cmnv_ruim_report

DESCRIPTION
  Accept status reports from R-UIM
  Set the global status and signal CM task that status has arrived.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmnv_ruim_report
(
  /*lint -e{818}
  ** Can't be const or compile errors due to diff function prototype in UIM
  */
  uim_rpt_type *report           /* R-UIM command status report */
)
/*lint -save -e818*/
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(report != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cm_uim_status_report = report->rpt_status;

  (void) rex_set_sigs( get_cm_tcb(), CM_RUIM_STATUS_SIG );

} /* cmnv_ruim_report */
/*lint -restore*/
#endif /* FEATURE_MMGSDI_SESSION_LIB */


/*===========================================================================

FUNCTION cmnv_get_uim_spc_status

DESCRIPTION
  Read the spc status from RUIM.

DEPENDENCIES

RETURN VALUE
  TRUE    if the spc status is other than 0.
  FLASE   if the spc status is 0.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmnv_get_uim_spc_status( void )
{
  byte spc_status    = (byte)~0;
      /* Status buffer */

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_type_enum_type     session_type = MMGSDI_1X_PROV_PRI_SESSION;
    /* CDMA subscription is from 1X_PRI */

  mmgsdi_file_enum_type             file_name = MMGSDI_CDMA_SPC_STATUS;

  /* Get the EF name to read based on the app present */
  switch(cm_mmgsdi_ssn_tbl_get_app_type_for_type(session_type))
  {
    case MMGSDI_APP_RUIM:
      file_name = MMGSDI_CDMA_SPC_STATUS;
      break;
    case MMGSDI_APP_CSIM:
      file_name = MMGSDI_CSIM_SPC_STATUS;
      break;
    default:
      /* Unknown app type so set to zero as default */
      return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the spc_status */
  if(cmmmgsdi_read ( MMGSDI_1X_PROV_PRI_SESSION, 
                      file_name, 0, 1, &spc_status,
                      sizeof(spc_status), transparent) )
  {
    spc_status &= UIM_SPC_STATUS_MASK;
  }
  else
  {
    /* If it failed set to zero as default */
    spc_status = 0;
  }

  #else

  uim_cmd_type cmd;
      /* R-UIM command buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate callback function to be called upon command completion.
  */
  cmd.hdr.options            = UIM_OPTION_ALWAYS_RPT;
  cmd.hdr.rpt_function       = cmnv_ruim_report;

  /* Indicate command completion is NOT to be signaled.
  */
  cmd.hdr.cmd_hdr.task_ptr   = NULL;
  cmd.hdr.cmd_hdr.sigs       = 0;
  cmd.hdr.cmd_hdr.done_q_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate the protocol and the slot for the command
  */
  cmd.hdr.protocol           = UIM_CDMA;
  cmd.hdr.slot               = UIM_SLOT_1;

  /* Indicate an access operation.
  */
  cmd.hdr.command            = UIM_ACCESS_F;

  /* Default channel
  */
  cmd.hdr.channel            = UIM_CHANNEL0;

  cmd.access_uim.hdr.slot    = UIM_SLOT_1;

  cmd.access_uim.access      = UIM_READ_F;
  cmd.access_uim.item        = UIM_CDMA_SPC_STATUS;
  cmd.access_uim.offset      = 0;
  cmd.access_uim.num_bytes   = 1;
  cmd.access_uim.data_ptr    = &spc_status;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) rex_clr_sigs( get_cm_tcb(), CM_RUIM_STATUS_SIG );
  uim_cmd(&cmd);
  (void) rex_wait( CM_RUIM_STATUS_SIG );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( cm_uim_status_report == UIM_PASS )
  {
    spc_status &= UIM_SPC_STATUS_MASK;
  }
  else
  {
    /* If it failed set to zero as default */
    spc_status = 0;
  }

  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  if( spc_status )
  {
    return TRUE;
  }

  return FALSE;
}

#ifdef FEATURE_UIM_RUN_TIME_ENABLE

/*===========================================================================

FUNCTION cmnv_send_rtre_command

DESCRIPTION
  Sends RTRE configuration to NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmnv_send_rtre_command
(
  nv_item_type *data_ptr                     /* pointer to space for item */
)
{
  /* Prepare the NV command
  */
  cmnv_cmd_sync.item = NV_RTRE_CONFIG_I;
  cmnv_cmd_sync.cmd = NV_RTRE_OP_CONFIG_F;
  cmnv_cmd_sync.data_ptr = data_ptr;
  cmnv_cmd_sync.tcb_ptr = rex_self();
  cmnv_cmd_sync.sigs = CM_NV_SYNC_SIG;
  cmnv_cmd_sync.done_q_ptr = NULL;

  /* Send RTRE command to NV by:
  ** 1. Clearing the NV signal
  ** 2. Queuing a write command to the NV task
  ** 3. Waiting for NV task to finish servicing the write command.
  */
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );
  nv_cmd( &cmnv_cmd_sync );
  cm_wait( CM_NV_SYNC_SIG );
  (void) rex_clr_sigs( rex_self(), CM_NV_SYNC_SIG );

  if( cmnv_cmd_sync.status != NV_DONE_S )
  {
    CM_ERR_1( "Failed sending the CONFIG Command to NV %d",
      cmnv_cmd_sync.status);
  }

} /* cmnv_send_rtre_command() */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */


#endif /* FEATURE_UIM_RUIM */
