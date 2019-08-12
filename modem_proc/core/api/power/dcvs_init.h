#ifndef DCVS_INIT_H
#define DCVS_INIT_H

/*============================================================================
  @file dcvs_init.h

  DCVS Initialization

  Provide prototypes for initialization purposes.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/api/power/dcvs_init.h#1 $ */

/**
 @brief Initialize the DCVS System.

 This function causes the initialization of the DCVS Subsystem.
 It is the only call required.
 */
void DCVSInitialize (void);

/**
 @brief Initialize the /core/cpu node.

 This function causes the initialization of the /core/cpu node.
  It is not a required call since DCVSInitialize will call it.
  It can be called in order to initialize the nodes before
  initializing DCVS.
 */
void CoreCpuInitialize (void);

#endif // DCVS_INIT_H

