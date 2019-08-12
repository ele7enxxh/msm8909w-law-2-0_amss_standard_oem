/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#ifndef CLKMGR_H
#define CLKMGR_H

/*==============================================================================

FILE:      clkmgr.h

DESCRIPTION: Public Function Declarations

This is the clocks routines for ADSPPM platform. This is not distributed to any clients that uses
ADSPPM. This is internal to ADSPPM.

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

   Copyright © 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
         QUALCOMM Proprietary/GTDR
==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/power/acpm/src/core/src/clkmgr.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/19/11   Shiju    Intial release
==============================================================================*/

/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "requestmgr.h"

Adsppm_Status CLK_Init(void);

Adsppm_Status CLK_ProcessRequest(coreUtils_Q_Type *pClkReqQ);

Adsppm_Status CLKDomain_ProcessRequest(coreUtils_Q_Type *pClkDomainReqQ);

Adsppm_Status CLK_GetInfo(AdsppmInfoClkFreqType * pClockInfo);

Adsppm_Status CLK_TurnOnSharedClk(void);

Adsppm_Status CLK_TurnOffSharedClk(void);

/*============================================================================
Core Clk Info
============================================================================*/


#endif
