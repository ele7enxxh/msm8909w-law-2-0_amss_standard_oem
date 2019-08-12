
#ifndef LSM_DEVCFG_H
#define LSM_DEVCFG_H

#ifdef __cplusplus
#include <new>
extern "C" {
#endif //__cplusplus

#include "qurt_elite.h"
#include "lsm_target_devcfg.h"


/** @brief Read driver Configure Data

   @param[in, out] pDev - pointer to device property type struct

   @return
   Success/failure of configure data read
*/
ADSPResult lsm_devcfg_read_mmpm_data(lpasshwio_prop_lsm_mmpm_struct_t *pDev);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef LSM_DEVCFG_H
