/*===========================================================================
  FILE: tracer_portmgr.c

  OVERVIEW: Tracer management of a pool of STM ports.

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tracer/src/tracer_portmgr.c#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "DALStdErr.h"

#include "tracer_portmgr.h"
#include "tracer_stp.h"

/*---------------------------------------------------------------------------
 * Variable Definitions
 * ------------------------------------------------------------------------*/
boolean bPortMgrInitialized = FALSE;

struct portMgr_s _gPortMgr;

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
// Must successfully obtain a trash and a d32 STM port before
// port manager is considered initialized.
boolean tracer_portmgr_init(void)
{
   int ii;

   _gPortMgr.nextKey = 0;
   _gPortMgr.inUseCount = 0;
   _gPortMgr.inUseHiMark = 0;

   if (DAL_SUCCESS != DalSTMTrace_NewPort(tracer_stp_get_hStm(),
      &(_gPortMgr.trashPort)))
   {
      bPortMgrInitialized = FALSE;
      return bPortMgrInitialized;
   }
   if (DAL_SUCCESS != DalSTMTrace_NewPort(tracer_stp_get_hStm(),
      &(_gPortMgr.d32Port)))
   {
      DalSTMTrace_FreePort(tracer_stp_get_hStm(), _gPortMgr.trashPort);
      bPortMgrInitialized = FALSE;
      return bPortMgrInitialized;
   }
   for (ii = 0; ii < TRACER_PORT_POOL_SIZE; ii++)
   {
      if (DAL_SUCCESS != DalSTMTrace_NewPort(tracer_stp_get_hStm(),
         &(_gPortMgr.pool[ii].port)))
      {
         _gPortMgr.pool[ii].inUseFlag = TRUE;
         _gPortMgr.inUseCount++;
      }
      else
      {
         _gPortMgr.pool[ii].inUseFlag = FALSE;
      }
   }
   if (TRACER_PORT_POOL_SIZE >_gPortMgr.inUseCount)
   {
      bPortMgrInitialized = TRUE;   // Consider initialized
   }
   else
   {
      bPortMgrInitialized = FALSE;  // Unable to obtain any ports.
   }
   return bPortMgrInitialized;
}

void tracer_portmgr_deinit(void)
{
   int ii;
   if (TRUE == bPortMgrInitialized)
   {
      bPortMgrInitialized = FALSE;

      for (ii = 0; ii < TRACER_PORT_POOL_SIZE; ii++)
      {
         _gPortMgr.pool[ii].inUseFlag = FALSE;
         if (0 != _gPortMgr.pool[ii].port)
         {
            DalSTMTrace_FreePort(tracer_stp_get_hStm(), _gPortMgr.pool[ii].port);
         }
      }
      DalSTMTrace_FreePort(tracer_stp_get_hStm(), _gPortMgr.trashPort);
      DalSTMTrace_FreePort(tracer_stp_get_hStm(), _gPortMgr.d32Port);
   }

   return;
}
//=========================================================================//
STMTracePortType tracer_port_d32(void)
{
   return _gPortMgr.d32Port;
}

//=========================================================================//
int tracer_port_himark(void)
{
   return (_gPortMgr.inUseHiMark);
}

