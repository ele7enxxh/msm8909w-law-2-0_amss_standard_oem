#ifndef RFLM_API_AUTOPIN_HW_H
#define RFLM_API_AUTOPIN_HW_H

#include "rflm.h"
#include "rflm_api_cmn.h"
#include "rflm_txagc_api.h"


#ifdef __cplusplus
extern "C" {
#endif

boolean rflm_autopin_xpt_sanity_check(rflm_handle_tx_t handle, rflm_txagc_xpt_mode_t xpt_mode);

void rflm_autopin_hw_sanity_check(boolean flag, boolean *status);

boolean rflm_autopin_sanity_check(rflm_handle_tx_t handle);
  
#ifdef __cplusplus
}
#endif

#endif
