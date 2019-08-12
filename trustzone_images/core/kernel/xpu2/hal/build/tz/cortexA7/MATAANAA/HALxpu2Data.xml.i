typedef  unsigned long long uint64;       
typedef  unsigned long int  uint32;       
typedef  unsigned short     uint16;       
typedef  unsigned char      uint8;        
typedef  signed long int    int32;        
typedef  unsigned long int  bool32;       
typedef  unsigned char      boolean;      
typedef enum
{
  HAL_XPU2_NO_ERROR = 0,        
  HAL_XPU2_INVALID_INSTANCE,    
  HAL_XPU2_UNSUPPORTED_INSTANCE_FOR_TARGET,
  HAL_XPU2_UNSUPPORTED_HANDLER, 
  HAL_XPU2_INVALID_BASE_ADDR,   
  HAL_XPU2_INVALID_PARAM,       
  HAL_XPU2_READ_WRITE_MISMATCH  
} HAL_xpu2_Status;
typedef enum
{
  HAL_XPU2_KPSS_MPU = 0,            
  HAL_XPU2_APCS_MPU = HAL_XPU2_KPSS_MPU,  
  HAL_XPU2_KPSS_L2,                 
  HAL_XPU2_BIMC_APU,                
  HAL_XPU2_BIMC_MPU0,               
  HAL_XPU2_BIMC_MPU1,               
  HAL_XPU2_BOOT_ROM,                
  HAL_XPU2_CRYPTO0_BAM,             
  HAL_XPU2_CRYPTO1_BAM,             
  HAL_XPU2_DEHR,                    
  HAL_XPU2_CLK_CTL,                 
  HAL_XPU2_LPASS_COMMON,            
  HAL_XPU2_LPASS_CORE_CSR,          
  HAL_XPU2_LPASS_LCC,               
  HAL_XPU2_LPASS_LPAIF,             
  HAL_XPU2_LPASS_LPM,               
  HAL_XPU2_LPASS_QDSP6SS,           
  HAL_XPU2_LPASS_RSMPLR,            
  HAL_XPU2_LPASS_SLIMBUS,           
  HAL_XPU2_LPASS_TOP,               
  HAL_XPU2_CAMERA_SS,               
  HAL_XPU2_MMSS_A_VBIF_JPEG,        
  HAL_XPU2_MMSS_A_VBIF_VFE,         
  HAL_XPU2_MMSS_DM_APU,             
  HAL_XPU2_MMSS_DM_MPU,             
  HAL_XPU2_MMSS_OCMEM_MPU0,         
  HAL_XPU2_MMSS_OCMEM_MPU1,         
  HAL_XPU2_MMSS_OCMEM_APU,          
  HAL_XPU2_MMSS_OXILI_CX,           
  HAL_XPU2_MMSS_OXILI_VBIF,         
  HAL_XPU2_MMSS_SNOC,               
  HAL_XPU2_MMSS_MDSS_VBIF,          
  HAL_XPU2_MMSS_VENUS,              
  HAL_XPU2_MMSS_VENUS_VBIF,         
  HAL_XPU2_MDSS_APU,                
  HAL_XPU2_MMSS_CC,                 
  HAL_XPU2_MMSS_MISC,               
  HAL_XPU2_MMSS_MNOC,               
  HAL_XPU2_MMSS_ONOC,               
  HAL_XPU2_MMSS_RBCPR,              
  HAL_XPU2_MPM2,                    
  HAL_XPU2_MSS,                     
  HAL_XPU2_IMEM_APU,                
  HAL_XPU2_IMEM_MPU,                
  HAL_XPU2_PERIPH_SS_APU,           
  HAL_XPU2_BAM_DMA,                 
  HAL_XPU2_BAM_BLSP1_DMA,           
  HAL_XPU2_BAM_BLSP2_DMA,           
  HAL_XPU2_PNOC_MPU,                
  HAL_XPU2_BAM_SDCC1,               
  HAL_XPU2_BAM_SDCC2,               
  HAL_XPU2_BAM_SDCC3,               
  HAL_XPU2_BAM_SDCC4,               
  HAL_XPU2_TSIF,                    
  HAL_XPU2_USB1_HS,                 
  HAL_XPU2_USB2_HSIC,               
  HAL_XPU2_PMIC_ARB,                
  HAL_XPU2_PMIC_ARB_SPMI,           
  HAL_XPU2_RPM_APU,                 
  HAL_XPU2_RPM_M3,                  
  HAL_XPU2_RPM_MSG_RAM,             
  HAL_XPU2_SEC_CTRL_APU,            
  HAL_XPU2_SPDM_APU,                
  HAL_XPU2_TCSR_MUTEX,              
  HAL_XPU2_TCSR_REGS,               
  HAL_XPU2_TLMM,                    
  HAL_XPU2_USB30,                   
  HAL_XPU2_PRONTO,                  
  HAL_XPU2_EBI1_CFG,                
  HAL_XPU2_RBCR_QDSS,               
  HAL_XPU2_SNOC_CFG,                
  HAL_XPU2_QPIC_BAM,                
  HAL_XPU2_QPIC_NAND,               
  HAL_XPU2_IPA,                     
  HAL_XPU2_CRYPTO2_BAM,             
  HAL_XPU2_VBIF_APU,
  HAL_XPU2_BCSS_APU,
  HAL_XPU2_EMAC,
  HAL_XPU2_KLM,
  HAL_XPU2_MMSS_VPU_MAPLE,
  HAL_XPU2_MMSS_VPU_VBIF_APU,
  HAL_XPU2_MMSS_VCAP_APU,
  HAL_XPU2_MMSS_VCAP_VBIF_APU,
  HAL_XPU2_PCIE20_MPU,
  HAL_XPU2_PCIE20_APU,
  HAL_XPU2_USB_HS_SEC,
  HAL_XPU2_SATA_APU,
  HAL_XPU2_MMSS_APU,
  HAL_XPU2_USB3_HSIC,
  HAL_XPU2_SPSS_APU,
  HAL_XPU2_LPASS_CORE,
  HAL_XPU2_LPASS_QDSP6,
  HAL_XPU2_MMSS_MM_AVSYNC,
  HAL_XPU2_PCIE0_APU,
  HAL_XPU2_PCIE1_APU,
  HAL_XPU2_SMMUV2,
  HAL_XPU2_UFS,
  HAL_XPU2_PCIE0_MPU,
  HAL_XPU2_PCIE1_MPU,
  HAL_XPU2_LPASS_SB1,
  HAL_XPU2_MMSS_A_RICA,
  HAL_XPU2_MMSS_A_VBIF_CPP_VBIF,
  HAL_XPU2_FD_XPU2,
  HAL_XPU2_UFS_ICE,
  HAL_XPU2_MMSS_A_VBIF_FD_VBIF,
  HAL_XPU2_MMSS_A_VBIF_RICA_VBIF,
  HAL_XPU2_PCNOC_CFG,
  HAL_XPU2_PRNG_CFG,
  HAL_XPU2_RPM_CFG,
  HAL_XPU2_SMMU_500_DIME_MPU,
  HAL_XPU2_SMMU_500_GPS_MPU,
  HAL_XPU2_SMMU_500_QDSP_MPU,
  HAL_XPU2_SMMU_500_TCU_MPU,
  HAL_XPU2_QPIC_QPIC,
  HAL_XPU2_QPIC_QPIC_MPU, 
  HAL_XPU2_SMMU_500_MCDMA_MPU,
  HAL_XPU2_SEC_CTRL_XPU,
  HAL_XPU2_EMAC_0_EMAC,
  HAL_XPU2_Q6_BIMC_MPU,
  HAL_XPU2_COUNT,
  HAL_XPU2_SIZE     = 0x7FFFFFFF  
} HAL_xpu2_XPU2Type;
typedef uint32 HAL_xpu2_ResourceGroupIDType;
typedef struct
{
  uint32 uStartLower;  
  uint32 uStartUpper;  
  uint32 uEndLower;    
  uint32 uEndUpper;    
} HAL_xpu2_ResourceGroupAddrType;
typedef enum
{
  HAL_XPU2_CONFIG_SECURE     = 0x0,  
  HAL_XPU2_CONFIG_NON_SECURE = 0x1,  
  HAL_XPU2_CONFIG_MODEM      = 0x2,  
  HAL_XPU2_CONFIG_COUNT,
  HAL_XPU2_CONFIG_SIZE       = 0x7FFFFFFF,
} HAL_xpu2_ConfigType;
typedef struct
{
  bool32 bMultiVMID;           
  union
  {
    struct
    {
      uint32 auVMID[((31/32) + 1)];      
      uint32 auVMIDPerm[((31/16) + 1)];  
    } multi;  
    struct
    {
      bool32 bRWGlobalEn;   
      bool32 bRWVMIDValid;  
      uint32 uRWVMID;       
      bool32 bROGlobalEn;   
      bool32 bROVMIDValid;  
      uint32 uROVMID;       
    } single;  
  } Vmid_Type;
} HAL_xpu2_ResourceGroupPermissionType;
typedef struct
{
  boolean bSecureClientROEn : 1;
  boolean bNonSecureClientROEn : 1;
  boolean bMSAClientROEn : 1;
  boolean bNonSecureClientRWEn : 1;
  boolean bMSAClientRWEn : 1;
  boolean bSecureClientRWEn : 1;
} HAL_xpu2_ResourceGroupClientPermType;
typedef struct
{
  HAL_xpu2_ResourceGroupAddrType       resAddr;
  HAL_xpu2_ResourceGroupPermissionType resPerm;
  HAL_xpu2_ResourceGroupClientPermType clientPerm;
  HAL_xpu2_ConfigType                  eConfig; 
} HAL_xpu2_ResourceGroupConfigType;
typedef struct
{
  HAL_xpu2_ResourceGroupPermissionType *pUnmapRGPerm;
  HAL_xpu2_ResourceGroupPermissionType *pAccessMaskTablePerm;
  boolean  bEnableSecureAMT : 1;
  boolean  bEnableNonSecureAMT : 1;
  boolean  bEnableVmidCheck : 1;  
  boolean  bEnablexPROTNSCheck : 1;
  boolean  bSecureConfigWriteDisable : 1;
  boolean bEnableSecureAMT2MegPageSize : 1;
  boolean bEnableNonSecureAMT2MegPageSize : 1;
} HAL_xpu2_DefaultxPU2ConfigType;
typedef enum
{
  HAL_XPU2_ERROR_O_DECODE_EN     = 0x1,
  HAL_XPU2_ERROR_O_INTERRUPT_EN  = 0x2,
  HAL_XPU2_ERROR_O_CLIENT_RPT_EN = 0x4,
  HAL_XPU2_ERROR_O_CFG_RPT_EN    = 0x8,
  HAL_XPU2_ERROR_O_NS_RES_CLIENT_EN = 0x10,
  HAL_XPU2_ERROR_O_NS_RES_CFG_EN = 0x20,
  HAL_XPU2_ERROR_O_CLIENT_INT_EN = 0x40,
  HAL_XPU2_ERROR_O_CFG_INT_EN    = 0x80,
  HAL_XPU2_ERROR_O_SIZE   = 0x7FFFFFFF  
} HAL_xpu2_ErrorOptionsType;
typedef uint32 HAL_xpu2_ErrorOptionsConfigType;
typedef enum
{
  HAL_XPU2_DEVICE_RPU            = 0,            
  HAL_XPU2_DEVICE_APU            = 1,            
  HAL_XPU2_DEVICE_MPU            = 2,            
  HAL_XPU2_DEVICE_COUNT,
  HAL_XPU2_DEVICE_DNE            = 0x7FFFFFFE,   
  HAL_XPU2_DEVICE_SIZE           = 0x7FFFFFFF,   
} HAL_xpu2_XPUDeviceType;
typedef enum
{
  HAL_XPU2_AMT_1MB_PAGE,
  HAL_XPU2_AMT_2MB_PAGE,
  HAL_XPU2_AMT_DNE_PAGE          = 0x7FFFFFFE,   
  HAL_XPU2_AMT_SIZE              = 0x7FFFFFFF,   
} HAL_xpu2_AMTPageSizeType;
typedef struct
{
  HAL_xpu2_XPUDeviceType eXpu2Type;  
  uint32  uResourceGroupCount;  
  uint32  uNumVMID;             
  uint32  uResourceGroupRes;    
  boolean bMultiVMID : 1;       
  boolean bFullPerm : 1;        
  boolean bAMTSupport : 1;      
  boolean bMSASupport : 1;      
  HAL_xpu2_AMTPageSizeType eAMTPageSize;  
  const char *szXpu2Name;       
} HAL_xpu2_XPUDeviceParamsType;
typedef enum
{
  HAL_XPU2_ERROR_F_CONFIG_PORT = 0x1,   
  HAL_XPU2_ERROR_F_CLIENT_PORT = 0x2,   
  HAL_XPU2_ERROR_F_MULTIPLE    = 0x4,   
  HAL_XPU2_ERROR_F_MULTI_CONFIG_PORT = 0x8,
  HAL_XPU2_ERROR_F_MULTI_CLIENT_PORT = 0x10,
  HAL_XPU2_ERROR_FLAGS_SIZE    = 0x7FFFFFFF  
} HAL_xpu2_ErrorFlagsType;
typedef uint32 HAL_xpu2_ErrorFlagsConfigType;
typedef enum
{
  HAL_XPU2_BUS_F_ERROR_AC     = 0x1,
  HAL_XPU2_BUS_F_ERROR_DCD    = 0x2,     
  HAL_XPU2_BUS_F_ASHARED      = 0x4,     
  HAL_XPU2_BUS_F_AINNERSHARED = 0x8,     
  HAL_XPU2_BUS_F_APRIV        = 0x10,    
  HAL_XPU2_BUS_F_APROTNS      = 0x20,    
  HAL_XPU2_BUS_F_AINST        = 0x40,    
  HAL_XPU2_BUS_F_AWRITE       = 0x80,    
  HAL_XPU2_BUS_F_AOOO         = 0x100,   
  HAL_XPU2_BUS_F_AFULL        = 0x200,   
  HAL_XPU2_BUS_F_ABURST       = 0x400,   
  HAL_XPU2_BUS_F_BURSTLEN     = 0x800,   
  HAL_XPU2_BUS_F_ARDALLOCATE  = 0x1000,  
  HAL_XPU2_BUS_F_AEXCLUSIVE   = 0x2000,  
  HAL_XPU2_BUS_F_ARDBEADNDXEN = 0x4000,  
  HAL_XPU2_BUS_F_AMSSSELFAUTH = 0x8000,  
  HAL_XPU2_BUS_F_SAVERESTORE_IN_PROG = 0x10000,
  HAL_XPU2_BUS_F_MSA_RG_MATCH = 0x20000,  
  HAL_XPU2_BUS_F_SECURE_RG_MATCH = 0x40000,
  HAL_XPU2_BUS_F_NONSECURE_RG_MATCH = 0x80000,
  HAL_XPU2_BUS_FLAGS_SIZE     = 0x7FFFFFFF  
} HAL_xpu2_BusErrorFlagsType;
typedef uint32 HAL_xpu2_BusErrorFlagsMaskType;
typedef struct
{
  HAL_xpu2_ErrorFlagsConfigType uErrorFlags;  
  HAL_xpu2_BusErrorFlagsMaskType uBusFlags;   
  uint32 uPhysicalAddressLower32;
  uint32 uPhysicalAddressUpper32;
  uint32 uMasterId;         
  uint32 uAVMID;
  uint32 uATID;             
  uint32 uABID;             
  uint32 uAPID;             
  uint32 uALen;             
  uint32 uASize;            
  uint32 uAMemType;         
  uint32 uAPReqPriority;
} HAL_xpu2_ErrorType;
HAL_xpu2_Status HAL_xpu2_Init
  ( HAL_xpu2_XPU2Type eXpu2,
    const HAL_xpu2_DefaultxPU2ConfigType *pDefaultConfig,
    char ** ppszVersion );
HAL_xpu2_Status HAL_xpu2_Reset   ( HAL_xpu2_XPU2Type eXpu2 ) ;
void HAL_xpu2_Save    ( HAL_xpu2_XPU2Type eXpu2 );
void HAL_xpu2_Restore ( HAL_xpu2_XPU2Type eXpu2 );
HAL_xpu2_Status HAL_xpu2_ConfigAccessPermissions
  ( HAL_xpu2_XPU2Type                           eXpu2,
    const HAL_xpu2_ResourceGroupPermissionType *pAccessConfig );
HAL_xpu2_Status HAL_xpu2_ConfigAccessMaskTablePermissions
  ( HAL_xpu2_XPU2Type                           eXpu2,
    const HAL_xpu2_ResourceGroupPermissionType *pAMTPerm );
HAL_xpu2_Status HAL_xpu2_ConfigUnmappedPartitionPermissions
  ( HAL_xpu2_XPU2Type                           eXpu2,
    const HAL_xpu2_ResourceGroupPermissionType *pUnmapRGPerm );
void HAL_xpu2_EnableMSSSelfAuth( HAL_xpu2_XPU2Type eXpu2 );
HAL_xpu2_Status HAL_xpu2_EnableMPUPartition
  ( HAL_xpu2_XPU2Type eXpu2,
    HAL_xpu2_ResourceGroupIDType uResID );
HAL_xpu2_Status HAL_xpu2_DisableMPUPartition
  ( HAL_xpu2_XPU2Type eXpu2,
    HAL_xpu2_ResourceGroupIDType uResID );
HAL_xpu2_Status HAL_xpu2_ConfigResourceGroup
  ( HAL_xpu2_XPU2Type                       eXpu2,
    HAL_xpu2_ResourceGroupIDType            uResID,
    const HAL_xpu2_ResourceGroupConfigType *pResConfig );
HAL_xpu2_Status HAL_xpu2_ConfigAccessMaskTable
  ( HAL_xpu2_XPU2Type             eXpu2,
    const uint32                 *pMaskAttribArray,
    uint32                        uNumElements );
HAL_xpu2_Status HAL_xpu2_ClearAccessMaskTable
  ( HAL_xpu2_XPU2Type           eXpu2,
    const uint32               *pMaskAttribArray,
    uint32                      uNumElements );
HAL_xpu2_Status HAL_xpu2_ConfigAccessMaskTablePageSize
  ( HAL_xpu2_XPU2Type           eXpu2,
    HAL_xpu2_AMTPageSizeType    eAMTPageSize,
    HAL_xpu2_ConfigType         eConfig );
HAL_xpu2_Status HAL_xpu2_ConfigErrorReporting
  ( HAL_xpu2_XPU2Type               eXpu2,
    HAL_xpu2_ConfigType             eConfig,
    HAL_xpu2_ErrorOptionsConfigType errOpt );
void HAL_xpu2_GetDeviceParameters( HAL_xpu2_XPU2Type             eXpu2,
                                   HAL_xpu2_XPUDeviceParamsType *pDevParams );
void HAL_xpu2_GetAccessPermissions
  ( HAL_xpu2_XPU2Type                     eXpu2,
    HAL_xpu2_ResourceGroupPermissionType *pAccessConfig );
void HAL_xpu2_GetAccessMaskTablePermissions
  ( HAL_xpu2_XPU2Type                     eXpu2,
    HAL_xpu2_ResourceGroupPermissionType *pAMTPerm );
void HAL_xpu2_GetUnmappedPartitionPermissions
  ( HAL_xpu2_XPU2Type                     eXpu2,
    HAL_xpu2_ResourceGroupPermissionType *pUnmapRGPerm );
void HAL_xpu2_GetResourceGroupConfig
  ( HAL_xpu2_XPU2Type                     eXpu2,
    HAL_xpu2_ResourceGroupIDType          uResID,
    HAL_xpu2_ResourceGroupConfigType     *pResConfig );
void HAL_xpu2_GetAccessMaskTable
  ( HAL_xpu2_XPU2Type           eXpu2,
    uint16                     *pMaskAttribArray,
    uint32                     *puNumElements );
bool32 HAL_xpu2_AreAMTBitsSet
  ( HAL_xpu2_XPU2Type             eXpu2,
    const uint16                 *pMaskAttribArray,
    uint32                        uNumElements );
void HAL_xpu2_GetErrorReportingConfig
  ( HAL_xpu2_XPU2Type                eXpu2,
    HAL_xpu2_ConfigType              eConfig,
    HAL_xpu2_ErrorOptionsConfigType *pErrConfig );
bool32 HAL_xpu2_IsError          ( HAL_xpu2_XPU2Type   eXpu2,
                                   HAL_xpu2_ConfigType eConfig );
void HAL_xpu2_GetError( HAL_xpu2_XPU2Type                     eXpu2,
                        HAL_xpu2_ConfigType                   eConfig,
                        HAL_xpu2_ErrorType                   *pError );
HAL_xpu2_Status HAL_xpu2_ClearError( HAL_xpu2_XPU2Type    eXpu2,
                                     HAL_xpu2_ConfigType  eConfig );
HAL_xpu2_Status HAL_xpu2_ClearErrorExt( HAL_xpu2_XPU2Type    eXpu2,
                                        HAL_xpu2_ConfigType  eConfig,
                                        HAL_xpu2_ErrorFlagsConfigType uErrorFlags );
typedef struct
{
  HAL_xpu2_XPUDeviceType eXpu2Type;            
  uint32                 uResourceGroupCount;  
  uint32                 uNumVMID;             
  uint32                 uClientAddrWidth;     
  uint32                 uRevision;            
  uint8                  uMSB;                 
  uint8                  uLSB;                 
  boolean                bAMTSupport : 1;      
  boolean                bMSASupport : 1;      
  boolean                bMultiVMID : 1;       
  boolean                bFullPerm : 1;        
} HAL_xpu2_IntXPU2DeviceParamsType;
typedef struct
{
  uint32                          uBaseAddr;
  const char                     *szXpu2Name;
  HAL_xpu2_IntXPU2DeviceParamsType  devParams;  
} HAL_xpu2_InfoType;
typedef  unsigned char      boolean;      
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
      typedef long long           int64;
      typedef unsigned long long  uint64;
typedef  unsigned long int  bool32;
<driver name="NULL">
  <device id="/dev/xpu2">
    <props id=3 type="HAL_xpu2_InfoType">
      {(((0x00400000 + 0x04800000) + 0x0004a000) + 0x00000000), "BIMC_MPU0", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=45 type="HAL_xpu2_InfoType">
      {((0x07880000 + 0x00002000) + 0x00000000), "BAM_BLSP1_DMA", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=6 type="HAL_xpu2_InfoType">
      {(((0x00700000 + 0x04800000) + 0x00002000) + 0x00000000), "CRYPTO0_BAM", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=8 type="HAL_xpu2_InfoType">
      {(((0x004b0000 + 0x04800000) + 0x00000000) + 0x00000000), "DEHR", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=112 type="HAL_xpu2_InfoType">
      {((0x07980000 + 0x00002000) + 0x00000000), "QPIC", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=113 type="HAL_xpu2_InfoType">
      {((0x07980000 + 0x00031000) + 0x00000000), "QPIC_MPU", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=66 type="HAL_xpu2_InfoType">
      {((0x0a000000 + 0x0021f000) + 0x00000000), "WCSS_APU", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=55 type="HAL_xpu2_InfoType">
      {((0x02000000 + 0x0000e000) + 0x00000000), "PMIC_ARB", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=105 type="HAL_xpu2_InfoType">
      {(((0x0002e000 + 0x04800000) + 0x00000000) + 0x00000000), "PCNOC_CFG", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=106 type="HAL_xpu2_InfoType">
      {(((0x0002f000 + 0x04800000) + 0x00000000) + 0x00000000), "PRNG_CFG", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=107 type="HAL_xpu2_InfoType">
      {(((0x00033000 + 0x04800000) + 0x00000000) + 0x00000000), "RPM_CFG", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=69 type="HAL_xpu2_InfoType">
      {(((0x0002d000 + 0x04800000) + 0x00000000) + 0x00000000), "SNOC_CFG", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=9 type="HAL_xpu2_InfoType">
      {(((0x01800000 + 0x04800000) + 0x00080000) + 0x00000000), "CLK_CTL", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=39 type="HAL_xpu2_InfoType">
      {(((0x004a0000 + 0x04800000) + 0x00007000) + 0x00000000), "MPM2", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=40 type="HAL_xpu2_InfoType">
      {((0x04000000 + 0x00000000) + 0x00000000), "MSS", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=57 type="HAL_xpu2_InfoType">
      {(((0x00200000 + 0x04800000) + 0x00087000) + 0x00000000), "RPM_APU", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=60 type="HAL_xpu2_InfoType">
      {(((0x00058000 + 0x04800000) + 0x00007000) + 0x00000000), "SEC_CTRL_APU", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=63 type="HAL_xpu2_InfoType">
      {(((0x01900000 + 0x04800000) + 0x00036000) + 0x00000000), "TCSR_REGS", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=31 type="HAL_xpu2_InfoType">
      {(((0x01d00000 + 0x04800000) + 0x000f0000) + 0x00000000), "VENUS0", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=5 type="HAL_xpu2_InfoType">
      {(((0x00100000 + 0x04800000) + 0x000ff000) + 0x00000000), "BOOT_ROM", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=108 type="HAL_xpu2_InfoType">
      {(((0x00034000 + 0x04800000) + 0x00001000) + 0x00000000), "DIME", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=109 type="HAL_xpu2_InfoType">
      {(((0x00034000 + 0x04800000) + 0x00002000) + 0x00000000), "GPS", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=42 type="HAL_xpu2_InfoType">
      {(((0x00050000 + 0x04800000) + 0x00003000) + 0x00000000), "IMEM_MPU", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=110 type="HAL_xpu2_InfoType">
      {(((0x00034000 + 0x04800000) + 0x00003000) + 0x00000000), "QDSP", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=111 type="HAL_xpu2_InfoType">
      {(((0x00034000 + 0x04800000) + 0x00000000) + 0x00000000), "TCU", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
    <props id=64 type="HAL_xpu2_InfoType">
      {(((0x01000000 + 0x04800000) + 0x00300000) + 0x00000000), "TLMM", {HAL_XPU2_DEVICE_DNE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
    </props>
  </device>
</driver>
