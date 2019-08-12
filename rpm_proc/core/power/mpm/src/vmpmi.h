#ifndef VMPMI_H
#define VMPMI_H

#include "comdef.h"
#include "HALmpmint.h"

/* structure describing a master */
typedef struct
{
  /* addresses of the virtual registers */
  volatile uint32 *enableRegAddr;
  volatile uint32 *detectCtlRegAddr;
  volatile uint32 *detectCtl3RegAddr;
  volatile uint32 *polarityRegAddr;
  volatile uint32 *statusRegAddr;

  uint32 target_id;
  void (*pfnSendWakeup)(void);
  void (*pfnSendInterrupt)(void);
  unsigned vmpm_interrupt;
} vmpm_master_info_type;

typedef struct
{
  uint32 vmpm_enable[2];
  uint32 vmpm_detect_ctl[2];
  uint32 vmpm_detect_ctl3[2];
  uint32 vmpm_polarity[2];
} vmpm_master_cache_type;

/* defined in vmpm_xxxx.c where xxxx is the target id, e.g. 8660 */
extern const uint32 vmpm_num_masters;
extern const vmpm_master_info_type vmpm_masters_info[];

#endif /* VMPMI_H */

