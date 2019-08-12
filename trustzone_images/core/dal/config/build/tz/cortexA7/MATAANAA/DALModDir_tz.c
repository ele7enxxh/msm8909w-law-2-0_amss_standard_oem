#include "DALStdDef.h" 
#include "DALReg.h" 

#include"DALSysTypes.h"
extern DALREG_DriverInfo DALSpi_DriverInfo;
extern DALREG_DriverInfo DALSystem_DriverInfo;

static DALREG_DriverInfo * DALDriverInfoArr[] = {
	& DALSpi_DriverInfo,
	& DALSystem_DriverInfo,
};

DALREG_DriverInfoList gDALModDriverInfoList = {2, DALDriverInfoArr}; 



StringDevice DAL_Mod_driver_list[] = {
			{NULL}

};

DALProps DAL_Mod_Info = {NULL, 0 ,0, DAL_Mod_driver_list};
