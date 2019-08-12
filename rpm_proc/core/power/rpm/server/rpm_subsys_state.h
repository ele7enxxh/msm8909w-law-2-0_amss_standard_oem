#ifndef __SUBSYS_RESOURCE_H__
#define __SUBSYS_RESOURCE_H__

#include "rpmserver.h"
#include <npa.h>
#include <npa_resource.h>

//keys
#define RPM_SUBSYS_REQUEST_STAT            0x74617473 // 'stat' in little endian

typedef enum
{
  SUBSYS_PC_STATE      = 0,  /* Sleep State */
  SUBSYS_ACTIVE_STATE  = 1, /* Active State */
  SUBSYS_MAX_NUM_STATE,
}rpm_subSystemState;

void rpm_subsys_pc_init(void);

#endif

