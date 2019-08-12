#include "DALStdDef.h" 
#include "DALReg.h" 

#include"DALSysTypes.h"
extern DALREG_DriverInfo DALicb_rpm_DriverInfo;
extern DALREG_DriverInfo DALClock_DriverInfo;
extern DALREG_DriverInfo DALTimetick_DriverInfo;

static DALREG_DriverInfo * DALDriverInfoArr[] = {
	& DALicb_rpm_DriverInfo,
	& DALClock_DriverInfo,
	& DALTimetick_DriverInfo,
};

DALREG_DriverInfoList gDALModDriverInfoList = {3, DALDriverInfoArr}; 



StringDevice DAL_Mod_driver_list[] = {
			{"/dev/icb/rpm",2545934574u, 444, &DALicb_rpm_DriverInfo, 0, NULL },
			
};

DALProps DAL_Mod_Info = {NULL, 0 ,1, DAL_Mod_driver_list};
