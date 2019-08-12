/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Diagnostics Packet Processing Routines
                  for debug parameters

General Description
  Diagnostic packet processing routines for debug parameters

Copyright (c) 2005,2009-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/rtos/src/paramsdiag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/12   sg      Removed support for PARAMSDIAG_DIAG_STATUS_F except when
                   FEATURE_DIAG_STATUS_RSP is defined.
09/17/10   is      Migrate from deprecated clk driver APIs
08/20/09   sg      Added support for command PARAMSDIAG_DIAG_STATUS_F
02/06/09   ps	   Changes to support wrapping of response Id count after 0xFFFF & Return wrap count.
10/03/08   vg      Updated code to use PACK() vs. PACKED
10/31/05   as      Fixed lint errors.
01/07/04   as      Created file.
===========================================================================*/

#ifdef __cplusplus
  extern "C" {
#endif

#include "paramsdiag.h"
#include "time_svc.h" /* for time_get_uptime_m() */

/*===========================================================================
                   GLOBAL DEFINITIONS
===========================================================================*/

static uint16 wrapflag = 0;

/* Tell lint that we are OK with the fact that pkt_len and req_ptr are not 
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req_ptr) */


/*===========================================================================

                   DIAG COMMAND FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION PARAMSDIAG_CLK_UPTIME
DESCRIPTION
  This packet gives the time elapsed in milliseconds, since the last reset.

===========================================================================*/
PACK(void *) paramsdiag_clk_uptime (
  PACK(void *) req_pkt,
  uint16 pkt_len       
)
{
  paramsdiag_clk_uptime_rsp_type *rsp_ptr;
  const int rsp_len = sizeof (paramsdiag_clk_uptime_rsp_type);

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */
  
  /* Allocate memory for the response packet */
  rsp_ptr = (paramsdiag_clk_uptime_rsp_type *) diagpkt_subsys_alloc 
                                   (DIAG_SUBSYS_PARAMS,
                                    PARAMSDIAG_UPTIME_F, 
                                    rsp_len);

  if (rsp_ptr != NULL)
  {
    /* Fill in the response with uptime */
    time_get_uptime_ms( rsp_ptr->uptime );
  }

  return (rsp_ptr);

} /* paramsdiag_clk_uptime */
/*===========================================================================

FUNCTION PARAMSDIAG_WRAP_STATUS

DESCRIPTION
  Return Wrap Flag status for delayed response id count.

===========================================================================*/
PACK(void *) paramsdiag_wrap_status (
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  paramsdiag_wraptid_status_rsp_type *rsp;
  const int rsp_len = sizeof (paramsdiag_wraptid_status_rsp_type);
  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */
  
  rsp = (paramsdiag_wraptid_status_rsp_type *) diagpkt_subsys_alloc 
									 (DIAG_SUBSYS_PARAMS,
									  PARAMSDIAG_WRAPTID_STATUS_F, 
									  rsp_len);
  if (rsp)
  {
    /* Return wrap status */
    rsp->wrapflag = wrapflag;
    rsp->pad=0;
  }
  return (rsp);

} /* paramsdiag_wrap_status */


/*===========================================================================

FUNCTION PARAMSDIAG_WRAP_RSPID

DESCRIPTION
  Wrap and return delayed response id count after DIAGPKT_MAX_DELAYED_RSP (0xFFFF)

===========================================================================*/
PACK(void *) paramsdiag_wrap_rspid (
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  paramsdiag_wrap_rspid_rsp_type *rsp;
  const int rsp_len = sizeof (paramsdiag_wrap_rspid_rsp_type);
  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */
  
  rsp = (paramsdiag_wrap_rspid_rsp_type *) diagpkt_subsys_alloc 
									 (DIAG_SUBSYS_PARAMS,
									  PARAMSDIAG_WRAP_RSPID_F, 
									  rsp_len);
  if (rsp)
  {
    /* Set flag*/
    wrapflag = 1;
    getWrapcount(wrapflag);
    /*-------------------------------------------------------------------------
     Return wrap count. 
     -------------------------------------------------------------------------*/
    rsp->wrapcount = getWrapcount(wrapflag);
  }
  return (rsp);
} /* paramsdiag_wrap_rspid */

/*===========================================================================

FUNCTION PARAMSDIAG_DIAG_STATUS

DESCRIPTION
  Return Diag Status to detect diag when there is no modem.

===========================================================================*/
#ifdef FEATURE_DIAG_STATUS_RSP
PACK(void *) paramsdiag_diag_status (
  PACK(void *)req_pkt,
  uint16 pkt_len
)
{
  paramsdiag_diag_status_rsp_type *rsp;
  const int rsp_len = sizeof (paramsdiag_diag_status_rsp_type);
  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */
  
  rsp = (paramsdiag_diag_status_rsp_type *) diagpkt_subsys_alloc 
									 (DIAG_SUBSYS_PARAMS,
									  PARAMSDIAG_DIAG_STATUS_F, 
									  rsp_len);
  if (rsp)
  {
    /* Return diag status */
    rsp->status = 0;
    rsp->proc_id = 0;
    rsp->reserved = 0;
  }
  return (rsp);

} /* paramsdiag_diag_status */
#endif /* FEATURE_DIAG_STATUS_RSP */

/*===========================================================================

                  DIAG COMMAND FUNCTION DISPATCH TABLE

===========================================================================*/

static const diagpkt_user_table_entry_type paramsdiag_tbl[] =
{
  {PARAMSDIAG_UPTIME_F, PARAMSDIAG_UPTIME_F, paramsdiag_clk_uptime},
  {PARAMSDIAG_WRAPTID_STATUS_F, PARAMSDIAG_WRAPTID_STATUS_F, paramsdiag_wrap_status},
  {PARAMSDIAG_WRAP_RSPID_F, PARAMSDIAG_WRAP_RSPID_F, paramsdiag_wrap_rspid},
  #ifdef FEATURE_DIAG_STATUS_RSP
  {PARAMSDIAG_DIAG_STATUS_F, PARAMSDIAG_DIAG_STATUS_F, paramsdiag_diag_status},
  #endif /* FEATURE_DIAG_STATUS_RSP */
};



/*===========================================================================

FUNCTION PARAMSDIAG_INIT

DESCRIPTION
  Register the diag packet function dispatch table.

===========================================================================*/
#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_PARAMS, paramsdiag_tbl);

#else

  void paramsdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_PARAMS, paramsdiag_tbl);
  }

#endif


/* Restore error reporting for these symbols */
/*lint +esym(715,pkt_len,req_ptr) */

#ifdef __cplusplus
  }
#endif

