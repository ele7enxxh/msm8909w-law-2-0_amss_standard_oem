/*==========================================================================*/
/*!
  @file
  dss_init.c

  @brief
  This file provides functions that are used to perform initializations
  of DSS module.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/dss_init.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-11-21 pgm Added dss ping critical section.
  2009-07-14 hm  Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "comdef.h"
#include "dss_init.h"
#include "ps_sock_mem_pool.h"
#include "ps_crit_sect.h"
#include "DSS_GlobalInit.h"

#include "dss_ping_comm_mgr.h"

#include "dssdns.h"
#include "dss_net_mgr.h"
/*---------------------------------------------------------------------------
  dss ping critical section
---------------------------------------------------------------------------*/
ps_crit_sect_type dss_ping_crit_section;

/*---------------------------------------------------------------------------
  Local declarations.
---------------------------------------------------------------------------*/


void dss_powerup
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Initialize DSS memory pools.
  -------------------------------------------------------------------------*/
  ps_sock_mem_pool_init();

  /*------------------------------------------------------------------------
    If rearch is enabled, initialize DSS module.
  -------------------------------------------------------------------------*/
  DSSGlobalInit();
  PS_INIT_CRIT_SECTION(&dss_ping_crit_section);

  /*-------------------------------------------------------------------------
    Initialize the DSS net manager.
  -------------------------------------------------------------------------*/
  dss_net_mgr_init();

  /*-------------------------------------------------------------------------
    Initialize DNS subsystem during powerup
  -------------------------------------------------------------------------*/
  dss_dns_init();


} /* dss_powerup() */

void dss_powerdown
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    If rearch is enabled, cleanup DSS module.
  -------------------------------------------------------------------------*/
  DSSGlobalDeinit();
  PS_DESTROY_CRIT_SECTION(&dss_ping_crit_section);

} /* dss_powerdown() */

void dss_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize ping engine.
  -------------------------------------------------------------------------*/
  dss_ping_comm_mgr_init_ping_engine();

} /* dss_init() */

void dss_deinit
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* dss_deinit() */




