#ifndef GLLC_V_H
#define GLLC_V_H
/*========================================================================
                      GPRS LLC Layer Service Interface


DESCRIPTION

  Provides the interface functions and type definitions which form the
  GPRS LLC Layer Service Provision. 

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
========================================================================== */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gllc_v.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
11-03-01   tjw      API content split out from the original GERAN internal file
                    previously used by GERAN clients. 
========================================================================== */

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif
#include "queue.h" 
#include "gmmllcif.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* ========================================================================
   LLGMM MESSAGING DEFINITIONS

   Messaging definitions for use on the LLGMM interface connection point in
   the direction of GMM to LLC.
   ======================================================================== */

typedef union
{
  gmmllc_llgmm_prim_e_type            prim;

  gmmllc_llgmm_assign_req_type        llgmm_assign_req;
  gmmllc_llgmm_trigger_req_type       llgmm_trigger_req;
  gmmllc_llgmm_suspend_req_type       llgmm_suspend_req;
  gmmllc_llgmm_resume_req_type        llgmm_resume_req;
  gmmllc_llgmm_strt_rdy_tmr_req_type  llgmm_strt_rdy_tmr_req;
  gmmllc_llgmm_stop_rdy_tmr_req_type  llgmm_stop_rdy_tmr_req;
  gmmllc_llgmm_test_mode_req_type     llgmm_test_mode_req;
  gmmllc_llgmm_psho_req_type          llgmm_psho_req;

} gmmllc_llgmm_mes_u_type;


typedef struct
{
  q_link_type                         link;
  gmmllc_llgmm_mes_u_type             mes;

} gmmllc_llgmm_mes_type;

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GMMLLC_LLGMM_GET_MES_BUF()
===
===  DESCRIPTION
===
===    Permits the GMM entity to request the dynamic allocation of sufficient
===    memory to accomodate a message for conveyance to the LLC entity over
===    the LLGMM interface connection point.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to allocated gmmllc_llgmm_mes_type message space.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

extern gmmllc_llgmm_mes_type *gmmllc_llgmm_get_mes_buf( void );


/*===========================================================================
===
===  FUNCTION      GMMLLC_LLGMM_PUT_MES_BUF()
===
===  DESCRIPTION
===
===    Permits the GMM entity to convey an LLGMM message to the LLC entity's
===    LLGMM message queue over the LLGMM interface connection point.
===
===  DEPENDENCIES
===
===    Message pointer must have previously been allocated with a call to
===    gmmllc_llgmm_get_mes_buf().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    May cause a context switch.
===
===========================================================================*/

extern void gmmllc_llgmm_put_mes_buf( gmmllc_llgmm_mes_type *mes_ptr );






#endif
