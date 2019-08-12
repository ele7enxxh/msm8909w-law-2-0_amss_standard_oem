#include "DALReg.h"
#include "DALDeviceId.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

DALResult 
Timetick_DalTimetick_Attach(const char *, DALDEVICEID, DalDeviceHandle **);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

static DALDEVICEID DalTimetick_DeviceId[1] = {DALDEVICEID_QTIMER};

const DALREG_DriverInfo 
DALTimetick_DriverInfo = { Timetick_DalTimetick_Attach,
     					   1,
						   DalTimetick_DeviceId
                         };


