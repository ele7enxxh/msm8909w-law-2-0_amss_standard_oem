/*============================================================================
  FILE:         dcvs_nv.c
 
  OVERVIEW:     This file provides common nv implementation for DCVS 
                parameters.
 
  DEPENDENCIES: This code requires no previous initialization.
 
                Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE
  
  $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/common/dcvs_nv.c#1 $
  $DateTime: 2016/12/13 07:59:23 $

  when       who     what, where, why
  --------   ----    ---------------------------------------------------
  11/28/11   sg      Replaced comdef with DALStdDef.h to get rid of warnings.
  10/25/11   sg      Limit length of strings that can written to the file
                     system to DCVS_NV_STRING_LEN_LIMIT.
  09/25/11   ss      Correcting the name of the NV path in the EFS 
                     to "CoreAll".                      
  09/25/11   sg      Fixed compiler warnings.
  04/26/11   ss      Fixing Q6 compiler warnings.
  02/14/10   bjs     Initial version
========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "DALStdDef.h"
#include "string.h"
#include "dcvs_nv.h"
#include "dcvs_algorithm.h"
#include <stdio.h>

#ifdef WINSIM
#define snprintf sprintf_s 
#define strlcpy(d, s, c) strcpy_s(d, c, s)
#else
#include "stringl/stringl.h"
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

// ToDo: Should these be moved out of this file and a
// function be provided to initialize the defaults for these
// "algorithm" parameters?

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/


static DALBOOL DCVSNvWrite(const char * dir, 
                           const char * alg, 
                           const DCVS_NV_PARAM_ENTRY * entry)
{
  char path[128];

  DcvsNvJoin(path, 128, "CoreCpu", dir, alg, NULL);
  switch(entry->type)
  {
    case DCVS_NV_PARAM_UINT32:
      return(DcvsNvUint32Write(path, 
                               entry->name, 
                               entry->defaultValue.uValue));

    case DCVS_NV_PARAM_STRING:
      return(DcvsNvStringWrite(path, 
                               entry->name, 
                               entry->defaultValue.sValue));
    case DCVS_NV_PARAM_NULL:
      break;

  }

  return(FALSE);
}

static DALBOOL DCVSNvRead(const char * dir, 
                          const char * alg, 
                          const DCVS_NV_PARAM_ENTRY * entry)
{
  char path[128];
  uint32 outUint;
  char outStr[DCVS_NV_STRING_LEN_LIMIT];

  DcvsNvJoin(path, 128, "CoreCpu", dir, alg, NULL);
  switch(entry->type)
  {
    case DCVS_NV_PARAM_UINT32:
      return(DcvsNvUint32Read(path, 
                              entry->name, 
                              &outUint));

    case DCVS_NV_PARAM_STRING:
      return(DcvsNvStringRead(path, 
                              entry->name, 
                              outStr,
                              DCVS_NV_STRING_LEN_LIMIT));
    case DCVS_NV_PARAM_NULL:
      break;

  }

  return(FALSE);
}

static void DCVSNvListInit (const char * dir, 
                            const char * alg, 
                            const DCVS_NV_PARAM_ENTRY * list)
{
  int index = 0;

  while(list[index].name != NULL)
  {
    if (!DCVSNvRead(dir, alg, &list[index]))
    {
      DCVSNvWrite(dir, alg, &list[index]);
    }
    index++;
  }
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

void DCVSNvDefaultParamsInit (const char * alg, 
                              const DCVS_NV_PARAM_ENTRY * list)
{
  DCVSNvListInit("Default", alg, list);
}

void DcvsNvInitialize(void)
{
  int index;
  char coreName[10];

  // Step through the Target Specific NV Parameters
  index = 0;
  while (dcvsTargetNvDefaults[index].name != NULL)
  {
    if (dcvsTargetNvDefaults[index].coreId == DCVS_NV_CORE_ID_ALL)
    {
      strlcpy(coreName, "CoreAll", 10);
    }
    else
    {
      snprintf(coreName, 10, "Core%d", dcvsTargetNvDefaults[index].coreId);
    }

    DCVSNvListInit (coreName, 
                    dcvsTargetNvDefaults[index].name, 
                    dcvsTargetNvDefaults[index].paramList);
    index++;
  }

  // Step through the Algorithm Default NV Parameters.  
  index = 0;
  while (dcvsTargetAlgList[index] != NULL)
  {
    DCVSNvListInit ("Default", 
                    dcvsTargetAlgList[index]->DCVSAlgoName, 
                    dcvsTargetAlgList[index]->defaultParameters);
    index++;
  }
}

void DcvsNvParamStringRead(int coreId, 
                      const char * alg, 
                      const DCVS_NV_PARAM_ENTRY * entry, 
                      char * outStr, 
                      uint32 outStrLen)
{
  char path[128];

  // ToDo: Build the core string instead of just using Core0
  //
  DcvsNvJoin(path, 128, "CoreCpu", "Core0", alg, NULL);
  if(!DcvsNvStringRead(path, 
                       entry->name, 
                       outStr,
                       outStrLen))
  {
    DcvsNvJoin(path, 128, "CoreCpu", "CoreAll", alg, NULL);
    if(!DcvsNvStringRead(path, 
                         entry->name, 
                         outStr,
                         outStrLen))
    {
      DcvsNvJoin(path, 128, "CoreCpu", "Default", alg, NULL);
      if(!DcvsNvStringRead(path, 
                           entry->name, 
                           outStr,
                           outStrLen))
      {
        strlcpy(outStr, entry->defaultValue.sValue, outStrLen);
      }
    }
  }
}

void DcvsNvParamUint32Read(int coreId, 
                      const char * alg, 
                      const DCVS_NV_PARAM_ENTRY * entry, 
                      uint32 * outVal)
{
  char path[128];

  // ToDo: Build the core string instead of just using Core0
  //
  DcvsNvJoin(path, 128, "CoreCpu", "Core0", alg, NULL);
  if(!DcvsNvUint32Read(path, 
                       entry->name, 
                       outVal))
  {
    DcvsNvJoin(path, 128, "CoreCpu", "CoreAll", alg, NULL);
    if(!DcvsNvUint32Read(path, 
                         entry->name, 
                         outVal))
    {
      DcvsNvJoin(path, 128, "CoreCpu", "Default", alg, NULL);
      if(!DcvsNvUint32Read(path, 
                           entry->name, 
                           outVal))
      {
        *outVal = entry->defaultValue.uValue;
      }
    }
  }
}

