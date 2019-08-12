#ifndef DCVS_ALGORITHM_H
#define DCVS_ALGORITHM_H
/**********************************************************************
 * dcvs_algorithm.h
 *
 * 
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2006, 2007, 2008, 2009 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/inc/dcvs_algorithm.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    ---------------------------------------------------

09/02/09   ss      Initial check in.
01/20/10   bjs     Cleanup
========================================================================*/

#include "dcvs_nv.h"

/*=======================================================================

                  PUBLIC FUNCTION DECLARATIONS

========================================================================*/

// Algorithm Structure and VTable
// An algorithm is defined by filling out this structure.
//
typedef struct
{
  const char* DCVSAlgoName;
  const DCVS_NV_PARAM_ENTRY *defaultParameters;
  uint32 (*DCVSCoreAlgoInitialize)(void*);
  void (*DCVSCoreAlgoBuildParams)(void*);
  uint32 (*DCVSCoreAlgoDecideMips)(void*);
  uint32 (*DCVSCoreAlgoReset)(void*);
  uint32 (*DCVSCoreAlgoEnable)(void*);
  uint32 (*DCVSCoreAlgoDisable)(void*);
} DCVS_CORE_ALGO;

// This variable must be defined on every target.
// It contains the list of algorithms supported
// on that target.
// It should be defined in dcvs_target.c in the appropriate tgt directory.
extern const DCVS_CORE_ALGO * dcvsTargetAlgList[];

#endif /* DCVS_ALGORITHM_H */




