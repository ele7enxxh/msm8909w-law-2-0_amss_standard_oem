#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
mpm_Dalmpm_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

const DALREG_DriverInfo 
DALmpm_DriverInfo = { mpm_Dalmpm_Attach, 0, NULL };


