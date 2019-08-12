#ifndef TZBSP_CONFIG_H
#define TZBSP_CONFIG_H

/**
 * @brief All Trustzone features flags are provided here. The use of these
 * features is to aif in bringup/debugging. All features are enabled in release
 * builds.
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_config.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
04/19/11   rv       Enable SSD for 8660
07/28/10   tk       Initial version
============================================================================*/

/*===========================================================================

                              DEFINES

===========================================================================*/

/* By default XPU interrupt handler doesn't dump the errored XPU syndrome
 * registers. Turning this flag to 1 will enable XPU syndrome dumping. */
#define TZBSP_XPU_SYNDROME_DUMPING              1

extern uint32 g_tzbsp_boot_cluster_idx; /* 4 byte aligned since used when MMU is disable */
extern uint8 g_tzbsp_cpu_count;
extern uint8 g_tzbsp_cluster_count;
extern uint8 g_tzbsp_boot_cpu_idx;

/* This enum needed to differentiate chipset for both across & same family chipsets. 
** This is done because current chipinfo driver from sys drivers doesn't allow 
** to differentiate between chipsets of same family e.g. 8936/8939/8239 etc */

typedef enum tzbsp_chip_id_e{
TZBSP_MSM_8x16,
TZBSP_MSM_8x36,
TZBSP_MSM_8x39,
TZBSP_MSM_8x09,
TZBSP_MDM_9x07,
TZBSP_MSM_8x08,
TZBSP_MDM_9x09,
TZBSP_MSM_8x09W,
TZBSP_CHIP_ID_MAX = 0x7FFFFFFF
}tzbsp_chip_id_t;

extern tzbsp_chip_id_t tzbsp_get_chip_id(void);
extern uint8 tzbsp_get_cpu_count(void);
#endif  /* TZBSP_CONFIG_H */
