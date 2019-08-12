/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   L O W E R   L A Y E R

                        I N T E R F A C E   M O D U L E

GENERAL DESCRIPTION
  This source file contains the function definitions necessary for the
  Call Manager module and the Lower Layers to send commands to one
  another.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmll.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/12   gm      DIME integration fix: TMC deprecation and others.
01/06/12   gm      Data synchronization changes for freefloating platform
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
03/24/09   sv      updated to cmtaski.h
02/20/09   rm      Lint clean post CMI changes
08/07/07   ic      Lint clean up
03/09/06   pk      Added support for Generic reports
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/18/05   sk      Added CM API cm_sd_rpt_free_buf() to free CM SD reports.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
08/03/04   ic      CM Lint clean up - cmll.c with Lint supression verified 
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
02/06/04   ws      Initial jaguar/mainline merge.
04/28/03   AT      Lint cleanup.
01/21/03   vt      Fixed the Header line to display proper file path
10/09/02   vt      Initial version
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "customer.h"                       /* Customer configuration file */
#include "comdef.h"               /* Definition for basic types and macros */
#include "cmdbg.h"
#include "cmtaski.h"
//#include "cmll.h"
#include "cmll_v.h"

#include <msgr.h>
#include <msgr_umid.h>

#ifdef CM_DEBUG
#error code not present
#endif

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

/****************************************************************************
              FUNCTIONS CALLED BY SD TO COMMUNICATE WITH CM
                    FOR INTERNAL AS WELL AS SD'S USE
****************************************************************************/


/*===========================================================================

FUNCTION cm_sd_rpt_get_buf_else_err_fatal

DESCRIPTION
  Allocate a CM report command buffer from the cm_mc_rpt_free_q. Error
  fatal if allocation is NOT successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated CM report command buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_sd_rpt_u_type *cm_sd_rpt_get_buf_else_err_fatal( void )
{
  cm_sd_rpt_u_type *sd_rpt_ptr;
    /* CM SD report pointer */

  /*-----------------------------------------------------------------------*/

  /* Try allocating a CM report command buffer
  */
  sd_rpt_ptr = (cm_sd_rpt_u_type *) cm_mem_malloc(sizeof(cm_sd_rpt_u_type));

  return sd_rpt_ptr;

} /* cm_sd_rpt_get_buf_else_err_fatal() */



/*===========================================================================

FUNCTION CM_SD_RPT

DESCRIPTION
  Allows MC to queue up messages to the CM.  Grab a buffer from the
  cm_mc_rpt_free_q, fill it in, and call this function with it. All this will
  do is queue it and set a signal for the CM task.  When the command
  is done, the buffer will be queued as directed in the command header,
  and a task you specify may be signaled.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_sd_rpt
(
  cm_sd_rpt_u_type *cmd_ptr               /* Pointer to a CM report buffer */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) q_link( cmd_ptr, &cmd_ptr->hdr.link );                /* init link */
  q_put( cmtask_get_cm_rpt_q(), &cmd_ptr->hdr.link );          /* and queue it */
  (void) rex_set_sigs( get_cm_tcb(), CM_RPT_Q_SIG );       /* signal the CM task */

} /* cm_sd_rpt() */



/*===========================================================================

FUNCTION cm_generic_rpt_get_buf_else_err_fatal

DESCRIPTION
  Allocate a CM report command buffer from the heap. Error
  fatal if allocation is NOT successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated CM report command buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_generic_rpt_s_type *cm_generic_rpt_get_buf_else_err_fatal( void )
{
  cm_generic_rpt_s_type *generic_rpt_ptr;
    /* CM Generic report pointer */

  /*-----------------------------------------------------------------------*/

  /* Try allocating a CM report command buffer
  */
  generic_rpt_ptr = (cm_generic_rpt_s_type *) cm_mem_malloc(sizeof(cm_generic_rpt_s_type));

  return generic_rpt_ptr;

} /* cm_generic_rpt_get_buf_else_err_fatal */



/*===========================================================================

FUNCTION CM_GENERIC_RPT

DESCRIPTION
  Allows generic clients to queue up messages to the CM. Dynamically allocate
  a buffer, fill it in, and call this function with it. All this will
  do is queue it and set a signal for the CM task.  When the command
  is done, the buffer will be queued as directed in the command header,
  and a task you specify may be signaled.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_generic_rpt
(
  cm_generic_rpt_s_type *cmd_ptr               /* Pointer to a CM report buffer */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) q_link( cmd_ptr, &cmd_ptr->hdr.link );                /* init link */
  q_put( cmtask_get_cm_rpt_q(), &cmd_ptr->hdr.link );          /* and queue it */
  (void) rex_set_sigs( get_cm_tcb(), CM_RPT_Q_SIG );       /* signal the CM task */

} /* cm_generic_rpt() */


/*===========================================================================

FUNCTION  cm_msgr_send

DESCRIPTION
  This is a utility function that initializes message header and sends the 
  message through MSGR primitives.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
errno_enum_type cm_msgr_send
(
  msgr_umid_type             umid,

  msgr_tech_module_type      from,

  msgr_hdr_struct_type*      msg_ptr,
    /* Pointer to message to be sent. Note that the first member of the message
     *  that is pointed to by this pointer must be of msgr_hdr_struct_type type
     */
  uint32                     msg_len
    /* Total message length (header and payload) in bytes */
)
{
  errno_enum_type msgr_error = E_SUCCESS;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( msg_ptr != NULL )
  {
    msgr_init_hdr_inst(msg_ptr, from, umid, msg_ptr->inst_id);

    msgr_error = msgr_send( msg_ptr, msg_len );

    CM_MSG_HIGH_3 ( "cm_msgr_send(), umid=%d, inst_id=%d, msgr_error=%d", 
                    umid, msg_ptr->inst_id, msgr_error );

    if ( msgr_error != E_SUCCESS )
    {
      CM_MSG_HIGH_1( "MSGR send error %d", msgr_error);
    }

    #ifdef FEATURE_MMODE_QTF
    #error code not present
#endif 
  }

  return msgr_error;

} /* cm_msgr_send */
