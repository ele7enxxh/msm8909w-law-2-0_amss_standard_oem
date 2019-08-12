# 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtData.xml"

























# 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/inc/HALvmidmt.h"








 


































 








 
# 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/inc/8909w/HALvmidmtTarget.h"








 

















 
 


 
 
# 50 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/inc/8909w/HALvmidmtTarget.h"

# 60 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/inc/8909w/HALvmidmtTarget.h"







# 55 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/inc/HALvmidmt.h"
# 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/inc/HALvmidmtDataTypes.h"







 

















 


 




 






 


















 

typedef  unsigned long int  uint32;       
typedef  unsigned short     uint16;       
typedef  unsigned char      uint8;        
typedef  signed long int    int32;        
typedef  unsigned long int  bool32;       
typedef  unsigned char      boolean;      







# 56 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/inc/HALvmidmt.h"



 
 


 




 
 
typedef enum
{
  HAL_VMIDMT_NO_ERROR,            
  HAL_VMIDMT_INVALID_INSTANCE,    
  HAL_VMIDMT_UNSUPPORTED_INSTANCE_FOR_TARGET,
     
  HAL_VMIDMT_UNSUPPORTED_HANDLER, 
  HAL_VMIDMT_INVALID_BASE_ADDR,   
  HAL_VMIDMT_INVALID_PARAM,       
  HAL_VMIDMT_INVALID_HW_VALUE,    
  HAL_VMIDMT_READ_WRITE_MISMATCH  
} HAL_vmidmt_Status;

 
typedef enum
{
  HAL_VMIDMT_CRYPTO0_AXI = 0,       
  HAL_VMIDMT_CRYPTO1_AXI,           
  HAL_VMIDMT_CRYPTO0_BAM,           
  HAL_VMIDMT_CRYPTO1_BAM,           
  HAL_VMIDMT_DEHR,                  
  HAL_VMIDMT_LPASS_DM,              
  HAL_VMIDMT_LPASS_LPAIF,           
  HAL_VMIDMT_LPASS_MIDI,            
  HAL_VMIDMT_LPASS_Q6AHB,           
  HAL_VMIDMT_LPASS_Q6AXI,           
  HAL_VMIDMT_LPASS_RESAMPLER,       
  HAL_VMIDMT_LPASS_SLIMBUS,         
  HAL_VMIDMT_CAMERA_SS,             
  HAL_VMIDMT_CAMSS_VBIF_JPEG,       
  HAL_VMIDMT_CAMSS_VBIF_VFE,        
  HAL_VMIDMT_MMSS_DDR,              
  HAL_VMIDMT_OCMEM,                 
  HAL_VMIDMT_OXILI,                 
  HAL_VMIDMT_VENUS_CPUSS,           
  HAL_VMIDMT_VENUS_VBIF,            
  HAL_VMIDMT_MSS_A2BAM,             
  HAL_VMIDMT_MSS_NAV_CE,            
  HAL_VMIDMT_MSS_Q6,                
  HAL_VMIDMT_BAM_DMA,               
  HAL_VMIDMT_BAM_BLSP1_DMA,         
  HAL_VMIDMT_BAM_BLSP2_DMA,         
  HAL_VMIDMT_BAM_SDCC1,             
  HAL_VMIDMT_BAM_SDCC2,             
  HAL_VMIDMT_BAM_SDCC3,             
  HAL_VMIDMT_BAM_SDCC4,             
  HAL_VMIDMT_TSIF,                  
  HAL_VMIDMT_USB1_HS,               
  HAL_VMIDMT_USB2_HSIC,             
  HAL_VMIDMT_QDSS_VMIDDAP,          
  HAL_VMIDMT_QDSS_VMIDETR,          
  HAL_VMIDMT_RPM_MSGRAM,            
  HAL_VMIDMT_SPDM_WRAPPER,          
  HAL_VMIDMT_USB30,                 
  HAL_VMIDMT_PRONTO,                
  HAL_VMIDMT_QPIC_BAM,              
  HAL_VMIDMT_IPA,                   
  HAL_VMIDMT_APCS,                  
  HAL_VMIDMT_CRYPTO2_AXI,           
  HAL_VMIDMT_CRYPTO2_BAM,           
  HAL_VMIDMT_EMAC,
  HAL_VMIDMT_LPASS_HDMI,
  HAL_VMIDMT_LPASS_SPDIF,
  HAL_VMIDMT_VENUS_VBIF2,
  HAL_VMIDMT_MMSS_VPU_MAPLE,
  HAL_VMIDMT_USB_HS_SEC,
  HAL_VMIDMT_SATA,
  HAL_VMIDMT_PCIE20,
  HAL_VMIDMT_PCIE0,
  HAL_VMIDMT_PCIE1,
  HAL_VMIDMT_USB3_HSIC,
  HAL_VMIDMT_UFS,
  HAL_VMIDMT_LPASS_SB1,
  HAL_VMIDMT_QPIC,
  HAL_VMIDMT_COUNT,        
  HAL_VMIDMT_SIZE = 0x7FFFFFFF      
} HAL_vmidmt_InstanceType; 

 
 



 



 



 
typedef struct
{
  uint32 auVMID[((31/32) + 1)];       
  uint32 auVMIDPerm[((31/32) + 1)];   
} HAL_vmidmt_AccessConfigType;

 
typedef enum
{
  HAL_VMIDMT_ERROR_O_SMCFCFG_EN = 0x1,
    

 
  HAL_VMIDMT_ERROR_O_USFCFG_EN  = 0x2,
    
 
  HAL_VMIDMT_ERROR_O_GCFGFIE    = 0x4,
     
  HAL_VMIDMT_ERROR_O_GCFGFRE    = 0x8,
     
  HAL_VMIDMT_ERROR_O_GFIE       = 0x10,
     
  HAL_VMIDMT_ERROR_OPTIONS_SIZE = 0x7FFFFFFF  
} HAL_vmidmt_ErrorOptionType;

 
typedef uint32 HAL_vmidmt_ErrorOptionConfigType;

 
typedef struct
{
  uint16 uEntryCount;       
  uint16 uNumVmid;          
  uint8 bStreamMapSupport;  
  uint8 uNumSSDIndexBits;
    
 
  uint8 uNumStreamIDBits; 
     
} HAL_vmidmt_DeviceParamsType;

 
typedef enum
{
  HAL_VMIDMT_ERROR_F_CLMULTI     = 0x1,
    
 
  HAL_VMIDMT_ERROR_F_CFGMULTI    = 0x2,
    
 
  HAL_VMIDMT_ERROR_F_PF          = 0x4,
     
  HAL_VMIDMT_ERROR_F_CAF         = 0x8,
    
 
  HAL_VMIDMT_ERROR_F_SMCF        = 0x10,
    
 
  HAL_VMIDMT_ERROR_F_USF         = 0x20,
    
 
  HAL_VMIDMT_ERROR_FLAGS_SIZE    = 0x7FFFFFFF  
} HAL_vmidmt_ErrorFlagsType;

 
typedef uint32 HAL_vmidmt_ErrorFlagsConfigType;

 
typedef enum
{
  HAL_VMIDMT_BUS_F_ERROR_NSATTR  = 0x1,  
  HAL_VMIDMT_BUS_F_ERROR_NSSTATE = 0x2,  
  HAL_VMIDMT_BUS_F_ERROR_WNR     = 0x4,  
  HAL_VMIDMT_BUS_FLAGS_SIZE     = 0x7FFFFFFF  
} HAL_vmidmt_BusErrorFlagsType;

 
typedef uint32 HAL_vmidmt_BusErrorFlagsMaskType;

 
typedef struct
{
  HAL_vmidmt_ErrorFlagsConfigType uErrorFlags;  
  HAL_vmidmt_BusErrorFlagsMaskType uBusFlags;   
  uint32 uPhysicalAddressLower32; 
     
  uint32 uPhysicalAddressUpper32; 
     
  uint32 uSSDIndex;        
  uint32 uSID;             
  uint32 uMasterId;        
  uint32 uAVMID;           
  uint32 uATID;            
  uint32 uABID;            
  uint32 uAPID;            
} HAL_vmidmt_ErrorType;

 
typedef uint32 HAL_vmidmt_SecureStatusDetType;


 
typedef enum
{
  HAL_VMIDMT_NSCFG_XTRAN = 0,  
  HAL_VMIDMT_NSCFG_SECURE  = 2,     
  HAL_VMIDMT_NSCFG_NONSECURE  = 3,  
  HAL_VMIDMT_NSCFG_DEFAULT = 4,     
} HAL_vmidmt_NonSecureAllocConfigType;

 
typedef enum
{
  HAL_VMIDMT_WACFG_XTRAN = 0,
     
  HAL_VMIDMT_WACFG_ALLOC  = 2,     
  HAL_VMIDMT_WACFG_NONALLOC  = 3,  
  HAL_VMIDMT_WACFG_DEFAULT = 4,    
} HAL_vmidmt_WriteAllocConfigType;

 
typedef enum
{
  HAL_VMIDMT_RACFG_XTRAN = 0,
     
  HAL_VMIDMT_RACFG_ALLOC  = 2,     
  HAL_VMIDMT_RACFG_NONALLOC  = 3,  
  HAL_VMIDMT_RACFG_DEFAULT = 4,    
} HAL_vmidmt_ReadAllocConfigType;

 
typedef enum
{
  HAL_VMIDMT_SHCFG_XTRAN = 0,
     
  HAL_VMIDMT_SHCFG_OUTER_SHARE = 1,   
  HAL_VMIDMT_SHCFG_INNER_SHARE  = 2,  
  HAL_VMIDMT_SHCFG_NON_SHARE  = 3,    
  HAL_VMIDMT_SHCFG_DEFAULT = 4,       
} HAL_vmidmt_SharedConfigType;

 
typedef enum
{
  HAL_VMIDMT_MTCFG_XTRAN = 0,
     
  HAL_VMIDMT_MTCFG_MEMATTR = 1,  
  HAL_VMIDMT_MTCFG_DEFAULT = 2,  
} HAL_vmidmt_MemTypeConfigType;

 
typedef enum
{
  HAL_VMIDMT_TRANSIENTCFG_XTRAN = 0,
     
  HAL_VMIDMT_TRANSIENTCFG_NON_TRANSIENT = 2,  
  HAL_VMIDMT_TRANSIENTCFG_TRANSIENT = 3,      
  HAL_VMIDMT_TRANSIENTCFG_DEFAULT = 4,        
} HAL_vmidmt_TransientConfigType;

 
typedef struct
{
  HAL_vmidmt_NonSecureAllocConfigType eNSCFG;
     
  HAL_vmidmt_WriteAllocConfigType eWACFG;  
  HAL_vmidmt_ReadAllocConfigType eRACFG;   
  HAL_vmidmt_SharedConfigType eSHCFG;      
  HAL_vmidmt_MemTypeConfigType eMTCFG;     
  uint8 uMemAttr;  
  HAL_vmidmt_TransientConfigType eTransientCfg;
     
} HAL_vmidmt_BusAttribType;

 
typedef enum
{
  HAL_VMIDMT_RCNSH_DISABLE = 0,
     
  HAL_VMIDMT_RCNSH_ENABLE = 1,
     
  HAL_VMIDMT_RCNSH_DEFAULT = 2,  
} HAL_vmidmt_RedirCacheNonShareableConfigType;

 
typedef enum
{
  HAL_VMIDMT_RCISH_DISABLE = 0,
     
  HAL_VMIDMT_RCISH_ENABLE = 1,
     
  HAL_VMIDMT_RCISH_DEFAULT = 2,  
} HAL_vmidmt_RedirCacheInnerShareableConfigType;

 
typedef enum
{
  HAL_VMIDMT_RCOSH_DISABLE = 0,
     
  HAL_VMIDMT_RCOSH_ENABLE = 1,
     
  HAL_VMIDMT_RCOSH_DEFAULT = 2,  
} HAL_vmidmt_RedirCacheOuterShareableConfigType;

 
typedef enum
{
  HAL_VMIDMT_REQPRICFG_XTRAN = 0,
     
  HAL_VMIDMT_REQPRICFG_ACR_REQPRI = 1,
    
 
  HAL_VMIDMT_REQPRICFG_DEFAULT = 2,  
} HAL_vmidmt_ReqPriorityConfigType;

 
typedef enum
{
  HAL_VMIDMT_REQPRI_NORMAL = 0,   
  HAL_VMIDMT_REQPRI_HIGH = 1,     
  HAL_VMIDMT_REQPRI_HIGHER = 2,   
  HAL_VMIDMT_REQPRI_HIGHEST = 3,  
  HAL_VMIDMT_REQPRI_DEFAULT = 4,  
} HAL_vmidmt_ReqPriorityType;

 
typedef struct
{
  HAL_vmidmt_RedirCacheNonShareableConfigType eRCNSH;
     
  HAL_vmidmt_RedirCacheInnerShareableConfigType eRCISH;
     
  HAL_vmidmt_RedirCacheOuterShareableConfigType eRCOSH;
     
  HAL_vmidmt_ReqPriorityConfigType eReqPriorityCfg;
     
  HAL_vmidmt_ReqPriorityType eReqPriority;
     
} HAL_vmidmt_AuxConfigType;



 
typedef struct
{
   HAL_vmidmt_BusAttribType     *pBypassBusAttrib;
      
   HAL_vmidmt_AuxConfigType     *pBypassAuxConfig;
      
   HAL_vmidmt_AccessConfigType  *pAccessControl;
      
   bool32                        bVmidPrivateNamespaceEnable;
      
   uint8                         bypassVmid;
      
} HAL_vmidmt_DefaultVmidConfigType;


 
typedef struct
{
   HAL_vmidmt_DefaultVmidConfigType     *pDefaultSecureConfig;
      
   bool32                                bGlbAddrSpaceRestrictedAccEnable;
      
   uint8                                 secureExtensions;
     
 
} HAL_vmidmt_DefaultSecureVmidConfigType;


 
typedef struct
{
  HAL_vmidmt_BusAttribType        *pBusAttrib;  
  HAL_vmidmt_AuxConfigType        *pAuxConfig;  
  uint8                            uVmid;       
} HAL_vmidmt_ContextConfigType;




 













 
HAL_vmidmt_Status HAL_vmidmt_Init
        ( HAL_vmidmt_InstanceType                       eVmidmt,
          const HAL_vmidmt_DefaultSecureVmidConfigType *pSecureConfig,
          const HAL_vmidmt_DefaultVmidConfigType       *pNonSecureConfig,
          char                                        **ppszVersion );









 
void HAL_vmidmt_Reset( HAL_vmidmt_InstanceType eVmidmt ) ;










 
void HAL_vmidmt_Save( HAL_vmidmt_InstanceType eVmidmt );










 
void HAL_vmidmt_Restore( HAL_vmidmt_InstanceType eVmidmt );



 















 
HAL_vmidmt_Status HAL_vmidmt_EnableClientPort
        ( HAL_vmidmt_InstanceType    eVmidmt,
          bool32                     bSecure );

















 
HAL_vmidmt_Status HAL_vmidmt_DisableClientPort
        ( HAL_vmidmt_InstanceType   eVmidmt,
          bool32                    bSecure );




















 
HAL_vmidmt_Status HAL_vmidmt_ConfigSecureStatusDetTable
        ( HAL_vmidmt_InstanceType               eVmidmt,
          const HAL_vmidmt_SecureStatusDetType *pSecureStatusDetArray,
          uint32                                uNumElements );

















 
HAL_vmidmt_Status HAL_vmidmt_ConfigSecureStatusDetTableExt
        ( HAL_vmidmt_InstanceType               eVmidmt,
          const HAL_vmidmt_SecureStatusDetType  secureStatusDetIndex,
          uint32                                secState );


















 
HAL_vmidmt_Status HAL_vmidmt_ConfigVmidContext
        ( HAL_vmidmt_InstanceType             eVmidmt,
          uint32                              uStreamID,
          const HAL_vmidmt_ContextConfigType *pContextConfig );


















 
HAL_vmidmt_Status HAL_vmidmt_StaticConfigVmidContext
        ( HAL_vmidmt_InstanceType             eVmidmt,
          const HAL_vmidmt_ContextConfigType *pContextConfig );




















 
HAL_vmidmt_Status HAL_vmidmt_ConfigVmidContextWithSIDList
        ( HAL_vmidmt_InstanceType             eVmidmt,
          uint32                              uIndex,
          const uint32                       *pSIDList,
          uint32                              uNumElements,
          const HAL_vmidmt_ContextConfigType *pContextConfig );





















 
HAL_vmidmt_Status HAL_vmidmt_ConfigErrorReporting
        ( HAL_vmidmt_InstanceType          eVmidmt,
          bool32                           bSecure,
          HAL_vmidmt_ErrorOptionConfigType errConfig );



 












 
HAL_vmidmt_Status HAL_vmidmt_GetDeviceParameters
        ( HAL_vmidmt_InstanceType      eVmidmt,
          HAL_vmidmt_DeviceParamsType *pDevParams );

















 
HAL_vmidmt_Status HAL_vmidmt_GetVmidContext
        ( HAL_vmidmt_InstanceType       eVmidmt,
          uint32                        uStreamID,
          HAL_vmidmt_ContextConfigType *pContextConfig );




















 
void HAL_vmidmt_GetErrorReportingConfig
        ( HAL_vmidmt_InstanceType           eVmidmt,
          bool32                            bSecure,
          HAL_vmidmt_ErrorOptionConfigType *pErrConfig );















 
bool32 HAL_vmidmt_IsError( HAL_vmidmt_InstanceType eVmidmt,
                           bool32                  bSecure );
















 
void HAL_vmidmt_GetError( HAL_vmidmt_InstanceType   eVmidmt,
                          bool32                    bSecure,
                          HAL_vmidmt_ErrorType     *pError );












 
HAL_vmidmt_Status HAL_vmidmt_ClearError( HAL_vmidmt_InstanceType eVmidmt,
                                         bool32                  bSecure );















 
HAL_vmidmt_Status HAL_vmidmt_ClearErrorExt
        ( HAL_vmidmt_InstanceType         eVmidmt,
          bool32                          bSecure,
          HAL_vmidmt_ErrorFlagsConfigType uErrorFlags );







# 27 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtData.xml"
# 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/src/HALvmidmtInfoImpl.h"

























 


 



 
# 35 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/src/HALvmidmtInfoImpl.h"



 


# 99 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/src/HALvmidmtInfoImpl.h"



 
 
typedef struct
{
  uint16 uEntryCount;  
  uint16 uNumVmid;  
  uint8 uNumSSDIndexBits;  
  uint8 uNumStreamIDBits;  
  uint8 uInputAddrSize;  
  uint8 bStreamMatchSupport;  
} HAL_vmidmt_IntVMIDMTDevParamsType;

 
typedef struct 
{
  uint32                             uBaseAddr;
  HAL_vmidmt_IntVMIDMTDevParamsType  devParams;





} HAL_vmidmt_InfoType;

# 28 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtData.xml"
# 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/HALhwio.h"




 







 









  



 




 
# 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/dal/HALcomdef.h"





























 




 




 




# 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/services/com_dtypes.h"

















 














 






 








 
# 56 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/services/com_dtypes.h"



 





















 



 


typedef  unsigned char      boolean;      




# 105 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/services/com_dtypes.h"


typedef  unsigned long int  uint32;       




typedef  unsigned short     uint16;       




typedef  unsigned char      uint8;        




typedef  signed long int    int32;        




typedef  signed short       int16;        




typedef  signed char        int8;         





 

typedef  unsigned char      byte;          



typedef  unsigned short     word;          
typedef  unsigned long      dword;         

typedef  unsigned char      uint1;         
typedef  unsigned short     uint2;         
typedef  unsigned long      uint4;         

typedef  signed char        int1;          
typedef  signed short       int2;          
typedef  long int           int4;          

typedef  signed long        sint31;        
typedef  signed short       sint15;        
typedef  signed char        sint7;         

typedef uint16 UWord16 ;
typedef uint32 UWord32 ;
typedef int32  Word32 ;
typedef int16  Word16 ;
typedef uint8  UWord8 ;
typedef int8   Word8 ;
typedef int32  Vect32 ;

# 178 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/services/com_dtypes.h"
   


      typedef long long           int64;



      typedef unsigned long long  uint64;
# 196 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/services/com_dtypes.h"







# 46 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/dal/HALcomdef.h"



 





 

typedef  unsigned long int  bool32;







 
































 

  

 



















# 34 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/HALhwio.h"





 




  







 









 








 





  













 
























 















 





























 


















 







































 


































 
# 265 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/HALhwio.h"
 











 


 



   


 





 
# 330 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/HALhwio.h"

# 355 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/HALhwio.h"








 






 
# 377 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/HALhwio.h"


# 406 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/HALhwio.h"




 
# 422 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/HALhwio.h"




 
# 438 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/HALhwio.h"




 
# 454 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/HALhwio.h"

 



# 29 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtData.xml"
# 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"




 










 














 

# 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/api/systemdrivers/hwio/msm8909w/phys/msmhwiobase.h"




 



 


























 








 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 







 






# 34 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"



 




# 83 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 99 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 113 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 135 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 151 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 165 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 177 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 191 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 207 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 219 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 229 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 251 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 273 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 289 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 303 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 321 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 335 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 377 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 391 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 413 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 427 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 441 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 453 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 467 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 483 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 495 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 505 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 529 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 553 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 569 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 583 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 601 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 615 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 629 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 671 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 685 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 707 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 717 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 741 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 765 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 781 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 795 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 813 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 827 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 841 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 855 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 873 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 904 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 927 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 946 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"



 




# 993 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1009 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1023 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1045 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1061 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1075 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1087 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1101 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1117 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1129 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1139 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1159 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1179 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1195 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1209 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1227 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1241 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1281 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1295 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1317 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1331 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1345 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1357 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1371 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1387 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1399 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1409 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1431 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1453 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1469 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1483 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1501 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1515 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1529 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1569 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1583 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1605 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1615 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1637 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1659 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1675 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1689 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1707 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1721 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1735 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1749 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1767 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1798 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1821 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"



 




# 1868 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1884 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1898 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1920 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1936 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1950 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1962 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1976 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 1992 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2004 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2014 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2034 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2054 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2070 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2084 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2102 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2116 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2156 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2170 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2192 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2206 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2220 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2232 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2246 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2262 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2274 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2284 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2306 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2328 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2344 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2358 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2376 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2390 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2404 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2444 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2458 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2480 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2490 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2512 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2534 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2550 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2564 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2582 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2596 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2610 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2624 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2642 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2673 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2696 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"



 




# 2745 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2761 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2775 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2797 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2813 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2827 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2839 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2853 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2869 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2881 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2891 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2913 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2935 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2951 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2965 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2983 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 2997 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3039 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3053 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3075 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3089 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3103 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3115 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3129 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3145 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3157 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3167 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3191 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3215 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3231 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3245 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3263 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3277 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3291 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3333 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3347 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3369 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3379 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3403 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3427 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3443 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3457 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3475 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3489 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3503 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3517 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3535 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3566 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3589 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3608 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"



 




# 3657 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3673 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3687 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3709 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3725 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3739 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3751 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3765 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3781 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3793 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3803 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3825 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3847 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3863 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3877 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3895 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3909 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3951 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3965 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 3987 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4001 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4015 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4027 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4041 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4057 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4069 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4079 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4103 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4127 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4143 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4157 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4175 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4189 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4203 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4245 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4259 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4281 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4291 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4315 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4339 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4355 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4369 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4387 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4401 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4415 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4429 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4447 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4478 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4501 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"

# 4520 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtHWIOTarget.h"


# 30 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/kernel/vmidmt/hal/config/8909w/HALvmidmtData.xml"

<driver name="NULL">

  <device id="/dev/vmidmt">
    <props id=24 type="HAL_vmidmt_InfoType">
      {((0x07880000 + 0x00000000) + 0x00000000), {17, 0, 0, 0, 0, 0}}
    </props>

    <props id=2 type="HAL_vmidmt_InfoType">
      {(((0x00700000 + 0x04800000) + 0x00000000) + 0x00000000), {9, 0, 0, 0, 0, 0}}
    </props>

    <props id=4 type="HAL_vmidmt_InfoType">
      {(((0x004b0000 + 0x04800000) + 0x00001000) + 0x00000000), {1, 0, 0, 0, 0, 0}}
    </props>

    <props id=57 type="HAL_vmidmt_InfoType">
      {((0x07980000 + 0x00000000) + 0x00000000), {1, 0, 0, 0, 0, 0}}
    </props>

    <props id=35 type="HAL_vmidmt_InfoType">
      {(((0x00200000 + 0x04800000) + 0x00088000) + 0x00000000), {1, 0, 0, 0, 0, 0}}
    </props>
  </device>

</driver>
