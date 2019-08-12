/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Routines
                      RRC available Routines

General Description
  RRC diagnostic packet processing routines.

Copyright (c) 2003-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/11/06   vm      RVDS2.1 compilation related changes.
01/25/06   bd      Lint clean-up
09/07/05   vm      Fixed error reported by Coverity tool. Error was that the 
                   return value of memory alloc was not checked for NULL.
10/11/04   vm      Modified code to remove/suppress lint errors.
09/03/04   vm      Created file. Returns current RRC state to DIAG.

===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "customer.h"
#include "rrcdiag.h"
#include "diagcmd.h" 
#include "rrcscmgr.h"

extern PACKED void * 
rrcdiag_rrc_get_state_handler (PACKED void *req_pkt, uint16 pkt_len);


/*==========================================================================

PACKET   RRCDIAG_RRC_GET_STATE

PURPOSE  Request sent from the DM to the DMSS, to return the current state
         of RRC

RETURN VALUE
         Pointer to response packet.

============================================================================*/

PACKED void * 
rrcdiag_rrc_get_state_handler (PACKED void *req_pkt, uint16 pkt_len)
{
  rrcdiag_rrc_get_state_rsp_type *rsp = ( rrcdiag_rrc_get_state_rsp_type *)req_pkt; /* to suppress lint error code 715 */

  uint16 rsp_len = pkt_len; /* to suppress lint error code 715 */
  rsp_len = sizeof(rrcdiag_rrc_get_state_rsp_type);

  /* Allocate the length as the response packet */
  rsp = (rrcdiag_rrc_get_state_rsp_type *) 
    diagpkt_subsys_alloc ((diagpkt_subsys_id_type)DIAG_SUBSYS_RRC, RRCDIAG_RRC_GET_STATE_F, rsp_len);

  if(rsp)
  {
    rsp->rrc_curr_state = (uint8) rrc_get_state();
  }
  else
  {
    WRRC_MSG0_HIGH("Error: mem alloc failure for RRC state rsp pkt");
  }
 
  return rsp;
} /* rrcdiag_rrc_get_state_handler */



/**************************************************************************
** RRC diag DISPATCH TABLE.
***************************************************************************/

static const diagpkt_user_table_entry_type rrcdiag_tbl[] =
{
  {RRCDIAG_RRC_GET_STATE_F, RRCDIAG_RRC_GET_STATE_F, rrcdiag_rrc_get_state_handler}
};

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER ((word)DIAG_SUBSYS_RRC, rrcdiag_tbl);

#endif

void rrcdiag_init (void)
{
#ifndef __cplusplus

   DIAGPKT_DISPATCH_TABLE_REGISTER ((word)DIAG_SUBSYS_RRC, rrcdiag_tbl);
   
#endif
}



#ifdef __cplusplus
  }
#endif
