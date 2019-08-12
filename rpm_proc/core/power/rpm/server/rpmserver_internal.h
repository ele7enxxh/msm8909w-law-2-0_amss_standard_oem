/*===========================================================================

  rpmserver_internal.h - data related to platform-specific customization

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPMSERVER_INTERNAL_H
#define RPMSERVER_INTERNAL_H
#include "rpm_resources.h"
#include "rpm_handler.h"
#include "rpm_set_changer.h"

typedef void (*rpmserver_service_cb)(unsigned ee, void *data, unsigned len, void *context);

typedef struct rpmserver_service_s
{
    rpmserver_service_s *next;
    rpm_service_type     service_id;
    rpmserver_service_cb cb;
    void                *context;
} rpmserver_service_t;

extern rpmserver_service_t *rpm_alt_service_list;

void rpmserver_register_handler(rpm_service_type service, rpmserver_service_cb cb, void *context);

extern "C"
{
	ResourceData *rpmserver_lookup_resource(rpm_resource_type resource, unsigned id);
	ResourceData *rpmserver_insert_resource(rpm_resource_type resource, unsigned id);
};

#endif // RPMSERVER_INTERNAL_H

