#include "DALStdDef.h" 
#include "DALReg.h" 

#include"DALSysTypes.h"
extern DALREG_DriverInfo DALSystem_DriverInfo;
extern DALREG_DriverInfo DALFlash_DriverInfo;

static DALREG_DriverInfo * DALDriverInfoArr[] = {
	& DALSystem_DriverInfo,
	& DALFlash_DriverInfo,
};

DALREG_DriverInfoList gDALModDriverInfoList = {2, DALDriverInfoArr}; 

DALProps DAL_Mod_Info = {NULL, 0 ,0 , NULL};
