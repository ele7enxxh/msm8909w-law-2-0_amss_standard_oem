#include "DALStdDef.h" 
#include "DALReg.h" 

#include"DALSysTypes.h"
extern DALREG_DriverInfo DALI2C_DriverInfo;
extern DALREG_DriverInfo DALSystem_DriverInfo;
extern DALREG_DriverInfo DALVAdc_DriverInfo;
extern DALREG_DriverInfo DALFlash_DriverInfo;
extern DALREG_DriverInfo DALChipInfo_DriverInfo;
extern DALREG_DriverInfo DALPlatformInfo_DriverInfo;
extern DALREG_DriverInfo DALTLMM_DriverInfo;

static DALREG_DriverInfo * DALDriverInfoArr[] = {
	& DALI2C_DriverInfo,
	& DALSystem_DriverInfo,
	& DALVAdc_DriverInfo,
	& DALFlash_DriverInfo,
	& DALChipInfo_DriverInfo,
	& DALPlatformInfo_DriverInfo,
	& DALTLMM_DriverInfo,
};

DALREG_DriverInfoList gDALModDriverInfoList = {7, DALDriverInfoArr}; 



StringDevice DAL_Mod_driver_list[] = {
			{"DALDEVICEID_VADC",105770992u, 2444, &DALVAdc_DriverInfo, 0, NULL },
			
};

DALProps DAL_Mod_Info = {NULL, 0 ,1, DAL_Mod_driver_list};
