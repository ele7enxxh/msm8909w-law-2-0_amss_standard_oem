/*===========================================================================

               N V   I T E M   M A N A G E R - A P P S   S I D E
DESCRIPTION

EXTERNALIZED FUNCTIONS
  nv_cmd
    This is the applications read and write interface to the NV subsystem
    from the apps side, it calls the synchronous nv_cmd_remote() interface
    which makes an RPC call to the modem side where NV resides.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The RPC task must be up and running on the apps side and communication
  through RPC with the modem side must be established before this interface
  is used.
  
REFERENCES
Copyright (c)  2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/rapi/services/src/nvimnv_apps.c#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/09   pc      Dual SIM/Dual Standby support for LCU
01/18/08   ptm     Add critical section around remoted NV call. Since NV is
                   serialized on the modem, there's no point in having more
                   than one outstanding RPC call. It just ties up proxy tasks
                   on the modem.
03/23/05   hn      First revision of this file.
===========================================================================*/

/* ======================================================================

                       INCLUDE FILES FOR MODULE

========================================================================*/

#include "target.h"
#include "comdef.h"
#include "rex.h"

#ifdef FEATURE_NV_ITEM_MGR
#include "nv.h"

/* Currently this variable is only accessed by the apps side prldiag.c
 * which should never execute prl diag commands since those should be
 * handled on the modem side.
 */
nv_roaming_list_type nv_pr_list;

static rex_crit_sect_type nv_crit;
static boolean nv_crit_initialized = FALSE;


/*===========================================================================

FUNCTION NV_CMD

DESCRIPTION
  This is the apps side's interface to the NV subsystem residing on the
  modem side. This function sets the status field of the cmd_ptr argument
  to NV_BUSY_S before calling the synchronous nv_cmd_remote() interface
  which makes an RPC call that results in an NV cmd being queued on the
  modem side to service the request. The RPC call returns once NV has
  finished its operation on the modem side.
  
DEPENDENCIES
  RPC communication must be established between the apps and modem sides
  before this interface is called.

RETURN VALUE
  None directly.  The status variable of the nv_cmd_type reflects the current
  status of this command as it is processed.

SIDE EFFECTS
  None.
  
===========================================================================*/

void  nv_cmd
(
  nv_cmd_type  *cmd_ptr                   /* Pointer to NV command buffer */
)
{
#ifdef FEATURE_NV_RPC_SUPPORT

  /* quick and dirty way to prevent critical section from getting initialized 
   * twice 
   */
  TASKLOCK();
  if(!nv_crit_initialized)
  {
    nv_crit_initialized = TRUE;
    rex_init_crit_sect(&nv_crit);
  }
  TASKFREE();

  /* Before calling the nv_cmd_remote api which is an RPC call that waits
   * for NV to fully finish servicing the request on the remote side, set
   * the status to NV_BUSY_S.
   */
  cmd_ptr->status = NV_BUSY_S;
  
  /* Serialize NV requests since it is a slow operation and we don't want to
   * hog all the proxy tasks on the modem
   */
  rex_enter_crit_sect(&nv_crit);

  cmd_ptr->status = nv_cmd_remote( cmd_ptr->cmd,
                                   cmd_ptr->item,
                                   cmd_ptr->data_ptr );
  rex_leave_crit_sect(&nv_crit);

  if( cmd_ptr->tcb_ptr != NULL ) {
    rex_set_sigs( cmd_ptr->tcb_ptr, cmd_ptr->sigs );
  }

#else
  MSG_HIGH("nv_cmd(): not implemented since FEATURE_NV_RPC_SUPPORT not defined"
           ,0,0,0);
#endif /* FEATURE_NV_RPC_SUPPORT */
} /* nv_cmd */


void  nv_cmd_ext
(
  nv_cmd_ext_type  *cmd_ext_ptr                   /* Pointer to NV command buffer */
)
{

  /* quick and dirty way to prevent critical section from getting initialized 
   * twice 
   */
  TASKLOCK();
  if(!nv_crit_initialized)
  {
    nv_crit_initialized = TRUE;
    rex_init_crit_sect(&nv_crit);
  }
  TASKFREE();

  /* Before calling the nv_cmd_remote api which is an RPC call that waits
   * for NV to fully finish servicing the request on the remote side, set
   * the status to NV_BUSY_S.
   */
  cmd_ext_ptr->nvcmd->status = NV_BUSY_S;
  
  /* Serialize NV requests since it is a slow operation and we don't want to
   * hog all the proxy tasks on the modem
   */
  rex_enter_crit_sect(&nv_crit);

  cmd_ext_ptr->nvcmd->status = nv_cmd_ext_remote( cmd_ext_ptr->nvcmd->cmd,
                                   cmd_ext_ptr->nvcmd->item,
                                   cmd_ext_ptr->nvcmd->data_ptr,
								   cmd_ext_ptr->context );
  rex_leave_crit_sect(&nv_crit);

  if( cmd_ext_ptr->nvcmd->tcb_ptr != NULL ) {
    rex_set_sigs( cmd_ext_ptr->nvcmd->tcb_ptr, cmd_ext_ptr->nvcmd->sigs );
  }

} /* nv_cmd_ext */


#endif /* FEATURE_NV_ITEM_MGR */
