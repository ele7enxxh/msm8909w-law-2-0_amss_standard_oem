/*==============================================================================

  FILE:      npa_log.h

  DESCRIPTION: NPA logging control

  The default behavior is to log everything, however this can be
  overridden by the build system by defining NPA_NP_LOG_<X> macros

  INITIALIZATION AND SEQUENCING REQUIREMENTS: 

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
  ==============================================================================

  $Header: //components/rel/rpm.bf/2.1.1/core/power/npa/src/npa_log.h#1 $
  ==============================================================================*/

#ifdef NPA_NO_LOG_CREATE
#define NPA_LOG_CREATE(log)
#else
#define NPA_LOG_CREATE(log) log
#endif

#ifdef NPA_NO_LOG_REQUEST
#define NPA_LOG_REQUEST(log)
#else
#define NPA_LOG_REQUEST(log) log
#endif

