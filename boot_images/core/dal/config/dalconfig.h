
/* Target: GENERIC*/

#ifndef DAL_CONFIG_H
#define DAL_CONFIG_H


#include "DALPropDef.h"

/* Defined in top level scons
#ifndef DAL_CONFIG_TARGET_ID
    #define DAL_CONFIG_TARGET_ID  0x9025
#endif
*/

/* Enable when smem support available on BOOTLOADERS2.0 */
#define DAL_BUILD_MULTI_PROC_IMAGE 

/* ARM V7 arch/Krait */
#define DAL_CONFIG_ARCH   DAL_CONFIG_ARCH_ARM_V7

#define DALGLBCTXT_SPINLOCK_PID  DAL_CONFIG_PROC_APSS

#endif /* DAL_CONFIG_H */

