/*============================================================================
  FILE:         cxo_lpr.c

  OVERVIEW:     This file provides the LPR definition for the CXO clock 
                low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/lpr/cxo_lpr.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "rpm.h"
#include "rpmclient.h"
#include "kvp.h"
#include "mpmint.h"

/*===========================================================================
 *              INTERNAL VARIABLES AND MACROS
 *==========================================================================*/
static kvp_t* p_cxo_kvp;
static unsigned int cxo_req;

void cxo_shutdown_init( void )
{
  p_cxo_kvp = kvp_create(4*3);
  cxo_req = 0;
  kvp_put(p_cxo_kvp, 0x62616e45 /* 'Enab' request */, sizeof(cxo_req), (void *)&cxo_req);
  return;
}

void cxo_shutdown_enter( uint64 wakeup_tick )
{
  kvp_reset(p_cxo_kvp);
  rpm_post_request(RPM_SLEEP_SET, RPM_CLOCK_0_REQ /* 'clk0' is for 'sources' */, /*cxo == source */ 0, p_cxo_kvp);
  return;
}

void cxo_shutdown_exit( void )
{
  vmpm_TriggerInterrupts();
  return;
}

