#include "rpm.h"
#include "rpmclient.h"
#include "kvp.h"
#include "rpm_swevent.h"

//resource definition
#define RPM_SWEVENT_REQ 0x76657773

//kvp defintions
#define RPM_SWEVENT_REQ_BMSK 0x6b736d62
#define RPM_SWEVENT_REQ_TYPE 0x65707974
#define RPM_SWEVENT_REQ_SETT 0x74746573

/* 
 * RPM SWEvent settings. The possible configurations per event.
 * QDSS takes priority over RAM if they are both enabled.
 */
typedef enum
{
  RPM_SWEVENT_SETTING_RAM,
  RPM_SWEVENT_SETTING_QDSS,
  RPM_SWEVENT_SETTING_UNDEFINED,
  RPM_SWEVENT_SETTING_SIZE = 0x7FFFFFFF, //force to a uint32
} rpm_swevent_setting;

void rpm_swevent_cfg_send(uint32 setting, uint32 type, uint64 bitmask)
{
  unsigned msg_id;
  kvp_t* swevent_cfg_kvp = kvp_create((1*4*2) + 8);

  kvp_put(swevent_cfg_kvp, RPM_SWEVENT_REQ_BMSK,
          sizeof(bitmask), (void *)&bitmask);

  kvp_put(swevent_cfg_kvp, RPM_SWEVENT_REQ_TYPE,
          sizeof(type), (void *)&type);

  kvp_put(swevent_cfg_kvp, RPM_SWEVENT_REQ_SETT,
          sizeof(setting), (void *)&setting);

  msg_id = rpm_post_request(RPM_ACTIVE_SET, RPM_SWEVENT_REQ, 0, swevent_cfg_kvp);

  kvp_destroy(swevent_cfg_kvp);
}

void rpm_swevent_test(unsigned i, boolean flag)
{
  if (flag)
  {
    rpm_swevent_cfg_send(RPM_SWEVENT_SETTING_RAM, i, 0x1FF);
    rpm_swevent_cfg_send(RPM_SWEVENT_SETTING_QDSS, i, 0x1FF);
  }
  else
  {
    rpm_swevent_cfg_send(RPM_SWEVENT_SETTING_QDSS, i, 0);
    rpm_swevent_cfg_send(RPM_SWEVENT_SETTING_RAM, i, 0);
  }
}

