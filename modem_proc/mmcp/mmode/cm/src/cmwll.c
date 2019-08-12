/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   L O W E R   L A Y E R

                        I N T E R F A C E   M O D U L E

GENERAL DESCRIPTION
  This source file contains the function definitions necessary for the
  Call Manager module and the Lower Layers to send commands to one
  another.

EXTERNALIZED FUNCTIONS
  cm_put_cmd                     - Allows the lower layer to send a command
                                   to CM.
  cm_get_cmd_buf                 - Allocates a CM command buffer.
  cm_mn_get_buf_else_err_fatal   - Allocates a MN command buffer.
  cm_mn_send_cmd                 - Allows CM to send a command to MN.
  cm_reg_get_buf_else_err_fatal  - Allocates a REG command buffer.
  cm_reg_send_cmd                - Allows CM to send a command to REG.
  cm_sm_get_buf_else_err_fatal   - Allocates a SM command buffer.
  cm_sm_send_cmd                 - Allows CM to send a command to SM.
  cm_rabm_get_buf_else_err_fatal - Allocates a RABM command buffer.
  cm_rabm_send_cmd               - Allows CM to send a command to RABM.
  cm_tmc_get_buf_else_err_fatal  - Allocates a TMC command buffer.
  cm_tmc_send_cmd                - Allows CM to send a command to TMC.
  cm_tc_get_buf_else_err_fatal   - Allocates a TC command buffer.
  cm_tc_send_cmd                 - Allows CM to send a command to TC.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmwll.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/15/13   fj      Add support for segment loading.   
04/02/12   gm      Remove unnecessary TMC references.
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/13/12   vk      Removal of feature flags added for CELL_DCH feature. 
03/02/12   vk      Added feature to get DCH cell info.
01/06/12   gm      Data synchronization changes for freefloating platform
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
04/29/10   sv      Add NAS stubs for QTF
02/18/09   np      Added more NAS stuff for QTF
10/27/09   sg      Initialize buffer "sm_cmd_ptr" returned by NAS API sm_get_cmd_buf().
12/22/08   fj      Separate header files included for GSM/WCDMA/LTE from 
                   GSM/WCDMA.
07/16/09   sv      Lint clean up
06/25/09   sv      Update the Interceptor declarations.
06/22/09   sv      Added NAS interceptors for QTF testcases
03/24/09   sv      updated to cmtaski.h
07/01/08   rm      Mainlining of MBMS changes
08/07/07   ic      Lint clean up
02/18/07   rp      Removed cm_wms_bc_gw_cb_filter
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
05/25/05   ic      Removed FEATURE_CM_CMD_Q_DEBUG
12/03/04   ws      Support for Remote WMS
08/04/04   ic      CM Lint clean up - cmwll.c with Lint supression verified 
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmwll_dummy() to get rid of ADS 1.2 compiler warning
06/09/04   dk      Replaced mem_malloc with cm_mem_malloc to support CM Heap.
05/06/04   ic      Changed cm_mn_get_buf_else_err_fatal() to it's easier to
                   set up break points in the code.
04/13/04   ic      CM_ASSERT() clean up
02/06/04   ws      Initial jaguar/mainline merge.
04/28/03   AT      Lint cleanup.
01/24/03   vt      Fixed the Header path information.
01/21/03   ws      Updated copyright information for 2003
06/24/02   PRK     Modified debug messages in cm_reg_send_cmd.
06/03/02   PRK     Mainlined FEATURE_PLMN.
05/06/02   PRK     Added FEATURE_CM_USE_UMTS_REG_INTERFACE.
04/30/02   PRK     Modified cm_rabm_get_buf_else_err_fatal to set the
                   message_set field to MS_CM_RAB.
02/06/02   PRK     Added cm_tc_get_buf_else_err_fatal and cm_tc_send_cmd
                   functions.
01/04/02   PRK     Replaced FEATURE_PLMN with FEATURE_NEW_PLMN.  Added more
                   debug statements.
11/22/01   RI      Added message_set field in the SM commands.
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Added support for PLMN selection/reselection (FEATURE_PLMN).
                   Re-added Serving System group and moved srv_state, roam_status,
                   rssi from phone group to serving system group (FEATURE_NEWSS).
10/17/01   RI/HQ   Initial release
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE))
#include "cmwll.h"     /* Interface to lower layer tasks */
#include "cmtaski.h"    /* Interface to command queue processing */
#include "cmidbg.h"    /* Interface to CM_ERR_FATAL and etc */
#endif

#ifdef CM_GW_SUPPORTED
#include "cmi.h"       /* CM internal definitions */
#include "cmdbg.h"     /* Interface to CM debug services */

#if (defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || defined (FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH))
#include "rrccmd.h"
#endif

#ifdef FEATURE_SEGMENT_LOADING
#include "IWCDMA.h"
#endif

#ifdef CM_DEBUG
#error code not present
#endif


/*lint -restore */

// instance pointer for segment loading of TDS or WCDMA
#ifdef FEATURE_SEGMENT_LOADING
extern interface_t *pi_mmode_wcdma;
#endif

/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                              REG INTERFACE FUNCTIONS
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION cm_reg_get_buf_else_err_fatal

DESCRIPTION
  Allocate a REG Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated REG command buffer.

SIDE EFFECTS
  none

===========================================================================*/
reg_cmd_type *cm_reg_get_buf_else_err_fatal(
            cm_to_reg_cmd_type cmd
)
{
  reg_cmd_type *reg_cmd_ptr = reg_cm_get_cmd_buf(cmd);
      /* Try allocating a MN Task command buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If allocation failed, error fatal.
  */
  if( reg_cmd_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  return reg_cmd_ptr;
}



/*===========================================================================

FUNCTION cm_reg_send_cmd

DESCRIPTION
  Send a command to the REG task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_reg_send_cmd(

  reg_cmd_type *reg_cmd_ptr
      /* Pointer to mc command */
)
{
  CM_ASSERT( reg_cmd_ptr != NULL );

  CM_MSG_LOW_0( "========= FORWARD CMD TO REG ========");
  CM_MSG_HIGH_1( "Send cmd %d to REG",reg_cmd_ptr->cmd.hdr.message_id);
  CM_MSG_LOW_0( "====================================");

  reg_put_cmd( reg_cmd_ptr );
}

/*---------------------------------------------------------------------------
                              MN INTERFACE FUNCTIONS
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION cm_mn_get_buf_else_err_fatal

DESCRIPTION
  Allocate a MN Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MN command buffer.

SIDE EFFECTS
  none

===========================================================================*/
mn_cnm_cmd_type *cm_mn_get_buf_else_err_fatal( byte cmd )
{
  mn_cnm_cmd_type *mn_cmd_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Try allocating a MN Task command buffer
  */
  mn_cmd_ptr = mn_cm_get_cmd_buf(cmd);

  /* If allocation failed, error fatal.
  */
  if( mn_cmd_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  return mn_cmd_ptr;
}

#if defined (FEATURE_MBMS)
#error code not present
#endif /*#if defined (FEATURE_MBMS)*/


/*===========================================================================

FUNCTION cm_mn_send_cmd

DESCRIPTION
  Send a command to the MN task.  For CM-MN interface, as_id indicates subscription
  id not stack id.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_mn_send_cmd(

  mn_cnm_cmd_type *mn_cmd_ptr
      /* Pointer to mc command */
)
{
  CM_ASSERT( mn_cmd_ptr != NULL );

  CM_MSG_LOW_0( "========= FORWARD CMD TO MN ========");
  //CM_MSG_HIGH_1( "Send cmd %d to MN",mn_cmd_ptr->cmd.mmi_mo_normal_call_req.message_header.message_id);
  CM_MSG_LOW_0( "====================================");

  mn_cnm_put_cmd( mn_cmd_ptr );
}

#if defined (FEATURE_MBMS)
#error code not present
#endif /*#if defined (FEATURE_MBMS)*/
/*lint +esym(715,msm_cmd_ptr) */
/*lint +esym(818,msm_cmd_ptr) */

/*---------------------------------------------------------------------------
                              SM INTERFACE FUNCTIONS
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION cm_sm_get_buf_else_err_fatal

DESCRIPTION
  Allocate a SM Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated SM command buffer.

SIDE EFFECTS
  none

===========================================================================*/
sm_cmd_type *cm_sm_get_buf_else_err_fatal( sm_cmd_enum_type cmd )
{
  sm_cmd_type *sm_cmd_ptr = sm_get_cmd_buf();
      /* Try allocating a SM Task command buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If allocation failed, error fatal.
  */
  if( sm_cmd_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(sm_cmd_ptr, 0, sizeof(sm_cmd_type));
  }
  sm_cmd_ptr->header.cmd_id = cmd;
  sm_cmd_ptr->header.message_set = MS_CM_SM;

  return sm_cmd_ptr;
}



/*===========================================================================

FUNCTION cm_sm_send_cmd

DESCRIPTION
  Send a command to the SM task.  For CM-SM interface, as_id indicates subscription
  id not stack id.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_sm_send_cmd(

  sm_cmd_type *sm_cmd_ptr
      /* Pointer to mc command */
)
{
  CM_ASSERT( sm_cmd_ptr != NULL );

  CM_MSG_LOW_0( "========= FORWARD CMD TO SM ========");
  CM_MSG_HIGH_1( "Send cmd %d to SM",sm_cmd_ptr->header.cmd_id);
  CM_MSG_LOW_0( "====================================");

  sm_put_cmd( sm_cmd_ptr );
}

/*---------------------------------------------------------------------------
                              RABM INTERFACE FUNCTIONS
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION cm_rabm_get_buf_else_err_fatal

DESCRIPTION
  Allocate a RABM Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated RABM command buffer.

SIDE EFFECTS
  none

===========================================================================*/
rabm_cmd_type *cm_rabm_get_buf_else_err_fatal( rabm_cmd_enum_type cmd )
{
  rabm_cmd_type *rabm_cmd_ptr = rabm_get_cmd_buf();
      /* Try allocating a RABM Task command buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If allocation failed, error fatal.
  */
  if( rabm_cmd_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  rabm_cmd_ptr->header.cmd_id      = cmd;
  rabm_cmd_ptr->header.message_set = MS_CM_RABM;

  return rabm_cmd_ptr;
}



/*===========================================================================

FUNCTION cm_rabm_send_cmd

DESCRIPTION
  Send a command to the RABM task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_rabm_send_cmd(

  rabm_cmd_type *rabm_cmd_ptr
      /* Pointer to rabm command */
)
{
  CM_ASSERT( rabm_cmd_ptr != NULL );

  CM_MSG_LOW_0( "========= FORWARD CMD TO RABM ========");
  CM_MSG_HIGH_1( "Send cmd %d to RABM",rabm_cmd_ptr->header.cmd_id);
  CM_MSG_LOW_0( "====================================");

  rabm_put_cmd( rabm_cmd_ptr );
}

#ifdef FEATURE_TC
/*---------------------------------------------------------------------------
                              TC INTERFACE FUNCTIONS
---------------------------------------------------------------------------*/



/*===========================================================================

FUNCTION cm_tc_get_buf_else_err_fatal

DESCRIPTION
  Allocate a TC Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated TC command buffer.

SIDE EFFECTS
  none

===========================================================================*/
tc_cmd_type *cm_tc_get_buf_else_err_fatal( tc_cmd_enum_type  cmd )
{
  tc_cmd_type *tc_cmd_ptr = tc_get_cmd_buf();
    /* Try allocating a TC Task command buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If allocation failed, error fatal.
  */
  if( tc_cmd_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  tc_cmd_ptr->header.cmd_id      = cmd;
  tc_cmd_ptr->header.message_set = MS_CM_TC;

  return tc_cmd_ptr;
} /* cm_tc_get_buf_else_err_fatal */



/*===========================================================================

FUNCTION cm_tc_send_cmd

DESCRIPTION
  Send a command to the TC task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_tc_send_cmd(

  tc_cmd_type *tc_cmd_ptr
    /* Pointer to cc command */
)
{
  CM_ASSERT( tc_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_0( "========= FORWARD CMD TO TC ========");
  CM_MSG_HIGH_1( "Send cmd %d to TC",tc_cmd_ptr->header.cmd_id);
  CM_MSG_LOW_0( "====================================");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tc_put_cmd( tc_cmd_ptr );
} /* cm_tc_send_cmd */

#endif /* FEATURE_TC */


/*---------------------------------------------------------------------------
                           LOWER LAYER INTERFACE FUNCTIONS
---------------------------------------------------------------------------*/

/*
** The following functions are called by the lower layers to communicate
** with CM.
*/


/*===========================================================================

FUNCTION cm_get_cmd_buf

DESCRIPTION
  Allocate a CM Task command buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated CM command buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_rpt_type *cm_get_cmd_buf( void )
{
  cm_rpt_type *cm_cmd_ptr;

  /* Allocate memory */
  if( (cm_cmd_ptr = (cm_rpt_type *)cm_mem_malloc(sizeof(cm_rpt_type)))
      == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* If we got here, allocation is successful,
  ** so indicate queue to place command buffer on when done,
  ** and specify NO task to be signalled when done
  */
  cm_cmd_ptr->hdr.task_ptr    = NULL;

  return cm_cmd_ptr;
}



/*===========================================================================

FUNCTION CM_PUT_CMD

DESCRIPTION
  Allows MN to queue up messages to the CM.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_put_cmd(
  cm_rpt_type *cmd_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr           != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) q_link( cmd_ptr, &cmd_ptr->hdr.link );  /* init link */
  q_put( cmtask_get_cm_rpt_q(), &cmd_ptr->hdr.link );  /* and queue it */
  (void) rex_set_sigs( get_cm_tcb(), CM_RPT_Q_SIG );  /* signal the CM task */
}


/*===========================================================================

FUNCTION cmwll_send_rrc_get_dch_cell_info

DESCRIPTION
  Queues DCH CELL INFO REQ to RRC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cmwll_send_rrc_get_dch_cell_info_req(void)
{
#if (defined(FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH) && defined (FEATURE_WCDMA))
  rrc_cmd_type*  cmd_ptr = NULL;
  CM_MSG_HIGH_0( "Send DCH cell info req");

#ifdef FEATURE_SEGMENT_LOADING
  if(pi_mmode_wcdma == NULL)
  {
    CM_ERR_0("pi_mmode_wcdma is NULL");
  }
  else
  {
    cmd_ptr = IWCDMA_rrc_get_cm_cmd_buf( pi_mmode_wcdma );
  }
  #else
  cmd_ptr = rrc_get_cm_cmd_buf();
  #endif

  

  if ( cmd_ptr == NULL )
  {
    CM_MSG_HIGH_0("No free buffer");
    return;
  }

  cmd_ptr->cmd_hdr.cmd_id = (rrc_cmd_e_type)RRC_CM_DCH_CELL_ID_REQUEST;

  #ifdef FEATURE_SEGMENT_LOADING
  if(pi_mmode_wcdma != NULL)
  {
    IWCDMA_rrc_put_cm_cmd( pi_mmode_wcdma,cmd_ptr );
  }
  #else
  rrc_put_cm_cmd( cmd_ptr );
  #endif
#endif        
}

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif

#else /* FEATURE_WCDMA || FEATURE_GSM */

/*===========================================================================
FUNCTION cmwll_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmwll_dummy( void )
/*lint -esym(714,cmwll_dummy) */
/*lint -esym(765,cmwll_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}


#endif /* FEATURE_GSM || FEATURE_WCDMA */

