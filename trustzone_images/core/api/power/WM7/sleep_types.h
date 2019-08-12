#ifndef SLEEP_TYPES_H
#define SLEEP_TYPES_H

#include "comdef.h"

#define CLIENT_NAME_LENGTH 12

typedef uint32 sleep_okts_handle;

typedef struct
{
    char         name[CLIENT_NAME_LENGTH+1];
    HANDLE       process_handle;
	boolean      okts;
} tSleepRegister;


typedef enum
{
  SLEEP_SWFI_RESTRICT_OFF = 0x0,
  SLEEP_SWFI_RESTRICT_ON
} sleep_swfi_restrict_type;

typedef struct
{
    sleep_okts_handle          handle;
    sleep_swfi_restrict_type   swfi_restriction;
} tSleepSetSWFIRestriction;

typedef struct
{
  sleep_okts_handle        handle;
  uint32                   resources;
} tSleepResourceCall;

/* Specific resources that can be voted upon individually. */
typedef enum
{
  SLEEP_RESOURCE_MICROPROC_SLEEP = 0x01, /* Local processor's simple halt */
  SLEEP_RESOURCE_TCXO            = 0x02, /* The actual TCXO clock */
  SLEEP_RESOURCE_MEMORY          = 0x04, /* Access to memory */
  SLEEP_RESOURCE_VDD_MIN         = 0x08, /* Modem voltage minimization */
  SLEEP_RESOURCE_APPS_SLEEP      = 0x10, /* Apps SWFI w/notification */
  SLEEP_RESOURCE_APPS_PWRC       = 0x20, /* Apps power collapse */
  SLEEP_RESOURCE_RBC1            = 0x40  /* A variety of clocks currently shut
                                            down when memory is freed, but do
                                            not depend on that behavior. */
} sleep_resources_type;

#endif /* SLEEP_TYPES_H*/

