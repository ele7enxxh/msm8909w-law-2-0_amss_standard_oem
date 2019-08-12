#include "DALSysInt.h"
#include "DDIChipInfo.h"
#include "DALSysCmn.h"
#ifdef DEVCFG_BOOT_TEST
   #include "DevCfgTest.h"
#endif

//we assume that there will be such a binary information linked into this module
//extern unsigned int __8974_devcfg_data_addr_base__;
//extern unsigned int __8626_devcfg_data_addr_base__;
//DALProps * DALPROP_PropsInfo = (DALProps *)&__devcfg_data_loc__;

extern DALProps DAL_Mod_Info;
extern DEVCFG_TARGET_INFO devcfg_target_soc_info[];
DALProps * DALPROP_PropsChipInfo = NULL;
DALProps * DALPROP_PropsPlatformInfo = NULL;
DEVCFG_PLATFORM_INFO gtcsrSOCHWVersion = {0, 0}; //family_number_device_number, major_ver_minor_ver

#ifdef DEVCFG_BOOT_TEST
   extern void DALSYS_PropertyTest(uint32);
#endif

// Forward Declaration
void DevCfg_Init_Platform(void);

void
DALSYS_SOC_PropsSymLookUp(DEVCFG_TARGET_INFO *pTargetInfoTableIter)
{
    if(!pTargetInfoTableIter)
        return;
    
    if(!DALPROP_PropsChipInfo)
    {        
      /* Iterate over the table until you reach end of it {0, NULL, NULL, 0}*/
      while(pTargetInfoTableIter->plinker_soc_seg_sym)
      {
         if(pTargetInfoTableIter->tcsr_soc_info == gtcsrSOCHWVersion.soc_number)
         {
            DALPROP_PropsChipInfo = (DALProps *)(pTargetInfoTableIter->plinker_soc_seg_sym);
         }
         pTargetInfoTableIter++;
      }    
    }
}

void
DALSYS_Platform_PropsSymLookUp(DEVCFG_TARGET_INFO *pTargetInfoTableIter)
{
    DalPlatformInfoPlatformType platform_id = DALPLATFORMINFO_TYPE_UNKNOWN;
    uint32 dwPlatformCount = 0;
    if(!pTargetInfoTableIter)
        return;
    
    if(!DALPROP_PropsPlatformInfo)
    {       
      /* Iterate over the table until you reach end of it {0, NULL, NULL, 0}*/
      while(pTargetInfoTableIter->plinker_soc_seg_sym)
      {
         if(pTargetInfoTableIter->tcsr_soc_info == gtcsrSOCHWVersion.soc_number)
         {
            if(pTargetInfoTableIter->num_platform_type > 0)
            {
               platform_id = DalPlatformInfo_Platform();
            }
            else
            {
                return;
            }   
            for( dwPlatformCount = 0; dwPlatformCount < pTargetInfoTableIter->num_platform_type; 
               dwPlatformCount++) 
            {
               if(pTargetInfoTableIter->platform_info->platform_id == platform_id)
               {
                  DALPROP_PropsPlatformInfo =  (DALProps *)(pTargetInfoTableIter->platform_info->plinker_platform_seg_sym);
                  break;
               }
               pTargetInfoTableIter->platform_info++;
            }
         }
         pTargetInfoTableIter++;
      }    
    }
}

/*
  Called at DALSYS Init to detect chipset info by mapping the 
  TCSR_SOC_HW_VERSION register physical address. The register
  address is assumed to be fixed for a family and defined in ipcat
*/

void
DALSYS_ReadSocHwInfo(void)
{
    DALSYSMemInfo MemInfo;
    DALResult eResult;
    DALSYSMemHandle hMem;
        
    eResult = DALSYS_MemRegionAlloc(
       DALSYS_MEM_PROPS_HWIO,
       DALSYS_MEM_ADDR_NOT_SPECIFIED,
       DEVCFG_TCSR_SOC_HW_ADDR, // 0xFD4A8000 TCSR_SOC_HW_VERSION
       0x1000, //4k , One page
       &hMem,
       NULL);
    
   if (eResult != DAL_SUCCESS) 
    {
      DALSYS_LOG_FATAL_EVENT("Failed to map DEVCFG_TCSR_SOC_HW_ADDR");
      return; 
   }

   eResult = DALSYS_MemInfo(hMem, &MemInfo);
   if (eResult != DAL_SUCCESS) 
   {
      DALSYS_LOG_FATAL_EVENT("Failed to get memory info");
      DALSYS_DestroyObject(hMem);
      return; 
    }
    
    _DALSYS_memscpy(&gtcsrSOCHWVersion, sizeof(uint32), (uint32*)MemInfo.VirtualAddr, sizeof(uint32));

    DALSYS_DestroyObject(hMem);  
}

/*
  Called at Runtime by clients
*/
DALProps *
DALSYS_GetPropsInfo(DEVCFG_TARGET_INFO_TYPE dwInfoType)
{  
  DALProps * DALPROP_PropsInfo;
  
   switch(dwInfoType)
   {
      case DEVCFG_TARGET_INFO_SOC :
         DALPROP_PropsInfo = DALPROP_PropsChipInfo;
         break;
      case DEVCFG_TARGET_INFO_PLATFORM:
         if(!DALPROP_PropsPlatformInfo)
         {
            DevCfg_Init_Platform();
         }
         DALPROP_PropsInfo = DALPROP_PropsPlatformInfo;
         break;
      default:
         DALPROP_PropsInfo = NULL;
         break; 
   }
   return DALPROP_PropsInfo;
}


DALProps *
DALSYS_GetDAL_Mod(void){
    return (DALProps *)&DAL_Mod_Info;
}
  
void 
DALSYS_GetPropDir(DALPropsDir ** pDALPropsDir, 
				  DEVCFG_TARGET_INFO_TYPE dwInfoType)
{
    DALProps * DALPROP_PropsInfo = DALSYS_GetPropsInfo(dwInfoType);
    *pDALPropsDir = (DALPropsDir *) DALPROP_PropsInfo->pDALPROP_PropBin;
}

const void **
DALSYS_GetPropStructPtrArray(DALProps *DALPROP_PropsInfo)
{
    return DALPROP_PropsInfo->pDALPROP_StructPtrs;
}

void DevCfg_Init(void)
{
   // Order for the below 2 api call must not be altered. 
   DALSYS_ReadSocHwInfo();
   DALSYS_SOC_PropsSymLookUp(devcfg_target_soc_info);

   // Testing
#ifdef DEVCFG_BOOT_TEST
   DALSYS_PropertyTest(0);
   DALSYS_PropertyTest(1);
   DALSYS_PropertyTest(DALDEVICEID_DALTEST);
#endif
}

void DevCfg_Init_Platform(void)
{
   DALSYS_Platform_PropsSymLookUp(devcfg_target_soc_info);
}

