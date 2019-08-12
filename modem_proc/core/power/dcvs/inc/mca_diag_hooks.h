/*==============================================================================
@file mca_diag_hooks.h

MCA Fixed Output Hooks

Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/inc/mca_diag_hooks.h#1 $
============================================================================*/
#ifndef MCA_DIAG_HOOKS_H
#define MCA_DIAG_HOOKS_H

#include "ULogFront.h"
#include "dcvs_core.h"

/**
 * Type enumeration of commands that may be sent to 
 * mca_ulog_diag_plugin plugin
 */
typedef enum mca_ulog_diag_plugin_command
{
  MCA_SET_OUTPUT_MODE_FIXED = 0,
  MCA_SET_OUTPUT_MODE_REAL,
  MCA_SET_Q6,
  MCA_SET_IBW,
  MCA_SET_ABW,
  MCA_SET_LATENCY,
  MCA_QUERY,
  NUMBER_OF_COMMANDS
} mca_ulog_diag_plugin_command;


/**
 * 
 * @brief
 * @param command 
 * 
 * @return int 
 */
int mca_ulog_diag_plugin( uint32 command );

#endif /* MCA_DIAG_HOOKS_H */
