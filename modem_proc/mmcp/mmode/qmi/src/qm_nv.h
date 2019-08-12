#ifndef _QM_NV_H_
#define _QM_NV_H_

#include "nv.h"
#include "qm_comdef.h"

enum qm_nv_tty_mode_e
{
  QM_NV_TTY_MODE_MIN = -1,
  QM_NV_TTY_MODE_FULL, /* TTY Full */
  QM_NV_TTY_MODE_VCO,  /* TTY Voice Carryover */
  QM_NV_TTY_MODE_HCO,  /* TTY Hearing Carryover */
  QM_NV_TTY_MODE_OFF,  /* TTY Off */
  QM_NV_TTY_MODE_MAX
};

void qm_nv_init( void );

#ifdef FEATURE_MODEM_CONFIG_REFRESH
//called in case of MCFG refresh event (sim swap)
void qm_nv_mcfg_refresh_tty( uint16 subs_index );
#endif // FEATURE_MODEM_CONFIG_REFRESH

//determines if bsr value has been changed and is pending reset
boolean qm_nv_bsr_changed( void );

nv_stat_enum_type qm_nv_get( nv_items_enum_type item, nv_item_type *data, enum qm_subs_e subs );

nv_stat_enum_type qm_nv_put( nv_items_enum_type item, nv_item_type *data, enum qm_subs_e subs );

#endif // !_QM_NV_H_

