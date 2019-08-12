#include "DALStdDef.h" 
#include "DALSysTypes.h" 
#include "dalconfig.h" 

#ifndef DAL_CONFIG_IMAGE_BOOT 
#define DAL_CONFIG_IMAGE_BOOT 
#endif 

const void * DALPROP_StructPtrs[1] =  {
	 NULL }; 

const uint32 DALPROP_PropBin[] = {

			0x00000074, 0x00000030, 0x00000044, 0x00000044, 0x00000044, 
			0x00000003, 0x00000000, 0x00000050, 0x02000050, 0x0000006c, 
			0x0200013f, 0x00000070, 0x456c6144, 0x5400766e, 0x65677261, 
			0x67664374, 0x00000000, 0x00000001, 0x00008909, 0x00002070, 
			0x00000002, 0x00000000, 0x00001001, 0x00000014, 0x00000007, 
			0x00000000, 0xff00ff00, 0xff00ff00, 0xff00ff00 };


 const DALProps DALPROP_PropsInfo = {(const byte*)DALPROP_PropBin, DALPROP_StructPtrs , 0, NULL};
