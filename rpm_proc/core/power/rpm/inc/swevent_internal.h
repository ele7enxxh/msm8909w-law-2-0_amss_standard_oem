#ifndef SWEVENT_INTERNAL_H
#define SWEVENT_INTERNAL_H

#include "comdef.h"
#include "tracer_event_ids.h"

#define DDR_NUM_EVENTS 64
#define BUS_NUM_EVENTS 128
#define RPM_NUM_EVENTS 128
#define SLEEP_NUM_EVENTS 64
#define CLOCK_NUM_EVENTS 128
#define PMIC_NUM_EVENTS 128
#define OCMEM_NUM_EVENTS 10
#define RAILWAY_NUM_EVENTS 20 

#define SWEVENT_REQ_BMSK 0x6b736d62
#define SWEVENT_REQ_TYPE 0x65707974
#define SWEVENT_REQ_SETT 0x74746573

typedef struct
{
  uint64 swevent[16];
} swevent_bitmask;

typedef struct 
{
  uint64     timestamp;
  tracer_event_id_t id;
  uint32     msg[4];
} swevent_msg;


/* RPM SWEvent types. This enum is used to refer to a group of events based on tech area */
typedef enum
{
  SWEVENT_TYPE_DDR = 0,
  SWEVENT_TYPE_BUS_0 = 1,
  SWEVENT_TYPE_BUS_1 = 2,
  SWEVENT_TYPE_RPM_0 = 3,
  SWEVENT_TYPE_RPM_1 = 4,
  SWEVENT_TYPE_SLEEP = 5,
  SWEVENT_TYPE_CLOCK_0 = 6,
  SWEVENT_TYPE_CLOCK_1 = 7,
  SWEVENT_TYPE_PMIC_0 = 8,
  SWEVENT_TYPE_PMIC_1 = 9,
  SWEVENT_TYPE_OTHER = 10,
  SWEVENT_TYPE_LAST, 
  SWEVENT_TYPE_MAX = 16, 
  SWEVENT_TYPE_UNDEFINED, 
  SWEVENT_TYPE_SIZE = 0x7FFFFFFF,  //force to a uint32
} swevent_type;

/* RPM SWEvent configuration. QDSS takes precedent over RAM if they are both enabled for a given event */ 
typedef enum
{
  SWEVENT_SETTING_RAM = 0,
  SWEVENT_SETTING_QDSS = 1,
  SWEVENT_SETTING_ETM  = 2,
  SWEVENT_SETTING_UNDEFINED,
} swevent_setting;

/* The internal representation of a software event config request                                           *
 * single_event - an individual event. This corresponds to the ID in core/api/debugtrace/tracer_event_ids.h *
 * cfg_typ - an event type based on tech area                                                               *
 * cfg_setting - the configuration setting to be used                                                       *
 * cfg_req - the type of request. This is necessary to determine how to apply the inrep                     */
typedef struct
{
  uint64 bmsk;
  swevent_type type;
  swevent_setting setting;
} swevent_inrep;


#endif /* SWEVENT_INTERNAL_H */
