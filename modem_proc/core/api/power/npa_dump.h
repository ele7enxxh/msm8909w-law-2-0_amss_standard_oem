/*==============================================================================
@file npa_dump.h

NPA Runtime Dump Functionality

Copyright (c) 2010-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/api/power/npa_dump.h#1 $
============================================================================*/
#ifndef NPA_DUMP_H
#define NPA_DUMP_H

#include "ULogFront.h"

/**
 * <!-- npa_dump -->
 *
 * @brief Dump a resource or the whole resource graph
 * 
 * @dump_log : ULog to write dump data into
 * @param resource_name : name of resource to be dumped. If NULL,
 * all resources will be dumped
 */
void npa_dump( ULogHandle dump_log, const char *resource_name );

#endif /* NPA_DUMP_H */
