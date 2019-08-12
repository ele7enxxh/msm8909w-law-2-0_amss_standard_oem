/*==========================================================================*/
/*!
  @file 
  ds_Net_Init.cpp

  @brief
  This file provides functions that are used to perform initializations 
  of DSNET module.

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_Init.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-12-01 aa  Removing usage of scratchpad. Using system heap instead.
  2009-08-17 pp  DS_Net module uses system heap memory via scratchpad.
  2009-02-27 hm  Added scratchpad memory for DSNET client.
  2008-05-02 hm  Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Net_Init.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_EventManager.h"
#include "ds_Net_Platform.h"
#include "ds_Net_NetworkFactory.h"

#include "ds_Net_INetworkFactory.h"
#include "ds_Net_ITechUMTS.h"
#include "ds_Net_CNetworkFactory.h"
#include "ds_Net_CreateInstance.h"

#include "ds_Net_NetworkFactory.h"
#include "ds_Net_TechUMTSFactory.h"

using namespace ds::Net;

/*---------------------------------------------------------------------------
  Local declarations.
---------------------------------------------------------------------------*/

void DSNetPowerup
(
  void
)
{
  int res = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MemoryManager::MemPoolInit();

  NetPlatform::Init();

  ds::Net::EventManager::Init();

  res = NetworkFactory::InstanceInit();
  if (AEE_SUCCESS != res) {
    LOG_MSG_ERROR_1("DSSGlobals::Init(): "
      "Can't create NetworkFactory singleton (%d)", res);
    ASSERT(0);
  }

  res = TechUMTSFactory::InstanceInit();
  if (AEE_SUCCESS != res) {
    LOG_MSG_ERROR_1("DSSGlobals::Init(): "
      "Can't create TechUMTSFactory singleton (%d)", res);
    ASSERT(0);
  }

} /* DSNetPowerupInit() */

void DSNetPowerdown
(
  void
)
{
  int res = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  EventManager::Deinit();

  res = NetworkFactory::InstanceRelease();
  if (AEE_SUCCESS != res) {
    LOG_MSG_ERROR_1("DSSGlobals::Deinit(): "
      "NetworkFactory singleton already released (%d)", res);
    ASSERT(0);
  }

  res = TechUMTSFactory::InstanceRelease();
  if (AEE_SUCCESS != res) {
    LOG_MSG_ERROR_1("DSSGlobals::Deinit(): "
      "TechUMTSFactory singleton already released (%d)", res);
    ASSERT(0);
  }

} /* DSNetPowerdown() */


void DSNetInit
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* DSNetInit() */

void DSNetDeinit
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* DSNetDeinit() */


