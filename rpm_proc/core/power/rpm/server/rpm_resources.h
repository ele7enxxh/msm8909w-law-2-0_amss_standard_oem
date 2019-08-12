/*===========================================================================

  rpm_resources.h - data models for resources

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_RESOURCES_H
#define RPM_RESOURCES_H

#include <stdint.h>

#include "rpmserver.h"
#include "rpm_estimator.h"
#include "rpm_config.h"

enum rpm_transition_type
{
    ACTIVE_TO_SLEEP,
    SLEEP_TO_ACTIVE,
};

void resource_init(ResourceData *r, unsigned class_idx, unsigned id);

bool resource_ee_request(ResourceData *r, unsigned ee, rpm_set_type set, kvp_t *data, uint64 &settling_time);

uint64 resource_ee_transition(ResourceData *r, unsigned ee, rpm_transition_type transition);
bool resource_ee_has_transition_work(ResourceData *r, unsigned ee, rpm_transition_type transition);

unsigned resource_num_clients(const ResourceData *r);

void resource_get_request(const ResourceData *r, unsigned client, unsigned *client_type, const void **request);

unsigned resource_create_client(ResourceData *r, unsigned client_type);
uint64 resource_client_request(ResourceData *r, unsigned client, unsigned size, void *data, uint64 settling_time);

static unsigned resource_estimate(ResourceData *r, unsigned operating_point)
{
    unsigned estimate = r->estimates[operating_point];
    if(estimate)
        return estimate;
    // No estimate yet
    unsigned new_operating_point = (operating_point + 1);
    for (unsigned i = 0; i < (NUM_CLOCK_RATES - 1); i++)
    {
        if (new_operating_point >= NUM_CLOCK_RATES) new_operating_point = 0;
        estimate = r->estimates[new_operating_point];
        if(estimate) break;
        new_operating_point++;
    }
    if(!estimate)
        return 0x4B00; // No estimate yet, approximate ~1 ms.
    unsigned khz = theEstimator().getClockRate(operating_point);
    unsigned new_khz = theEstimator().getClockRate(new_operating_point);
    estimate = (estimate * (new_khz / 1000)) / (khz / 1000);
    return estimate;
}

static unsigned resource_settling_estimate(ResourceData *r)
{
    return (&(rpm->classes[r->class_idx]))->settling_estimate;
}

static void resource_observe(ResourceData *r, unsigned operating_point, Observation &obs, unsigned settling_time)
{
    if(obs.time > r->estimates[operating_point])
        r->estimates[operating_point] = obs.time;

    if(settling_time > (&(rpm->classes[r->class_idx]))->settling_estimate)
        (&(rpm->classes[r->class_idx]))->settling_estimate = settling_time;
}

void resource_class_add_client_type(ResourceClassData *rc, unsigned client_type, unsigned inrep_size);

uint64 resource_ee_settling_apply(ResourceData *r, unsigned ee, uint64 settling_time);

uint64 resource_ee_settling_query(ResourceData *r, unsigned ee);
void rpm_query_npa_settling(rpm_application_info *info);
unsigned rpm_init_shadow(unsigned id, unsigned class_idx);

#endif // RPM_RESOURCES_H

