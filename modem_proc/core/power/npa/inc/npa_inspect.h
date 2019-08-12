/*============================================================================
@file npa_inspect.h

NPA introspection API; intended for use by test teams and, perhaps, other
teams within CoreBSP Power

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/power/npa/inc/npa_inspect.h#1 $
============================================================================*/
#ifndef NPA_INSPECT_H
#define NPA_INSPECT_H

#include "npa_resource.h"

/**
   @brief npa_resource_has_client - Check if the given client is a client to
   the given resource
 
   @param resource : The resource in question
   @param client : The client to check
   
   @return : 1 on success, else 0
 */
unsigned int npa_resource_has_client( npa_resource     *resource, 
                                      npa_client_handle client);

#endif /* NPA_INSPECT_H */
