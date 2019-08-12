
#ifndef ADSPPM_H_
#define ADSPPM_H_



/*==============================================================================

*@fn adsppm.h

*@Brief: adsppm internal data structure and API

*This is the internal data and API for adsppm. This is not distributed to any clients that uses
*adsppm. 

*INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

* Copyright (c) 2013 Qualcomm Technologies, Inc.
*               All Rights Reserved.
*           
* Qualcomm Technologies, Inc. Confidential and Proprietary.

==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/power/acpm/inc/adsppm.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who          what, where, why
--------   ---          ---------------------------------------------------------
03/01/12   alisonc          Initial version

==============================================================================*/



#include "adsppm_types.h"
#include "adsppm_defs.h"
//#include <unistd.h>
#include "DALSys.h"
#include "coreUtils.h"
#include <stringl/stringl.h>

#define ADSPPM_REVISION 001

//define for ADSPPM testing
//#define ADSPPM_FEATURE_TEST_SUPPORT 1

#define MAX_NUM_OF_ADSPPM_CLIENTS       32
//#define MAX_NUM_OF_ADSPPM_HANDLE_PRE_ALLOCATE MAX_NUM_OF_ADSPPM_CLIENTS
#define MAX_NUM_OF_ADSPPM_CLOCK_ARRAY   8
#define MAX_NUM_OF_ADSPPM_BW_ARRAY  8

#define ADSPPM_REG_PROC_CLOCK_BW   32768
#define ADSPPM_REG_PROC_CLOCK_BW_PERCENTAGE  10
/**
 MAX client name length 
*/
#define MAX_ADSPPM_CLIENT_NAME                 64
#define MAX_ADSPPM_REQ_Q_ELEMENTS              16



/**
*@enum AdsppmApiType - ADSPPM API TYPE 
*/
typedef enum
{
     Adsppm_Api_Type_None,                            /**< None */
     Adsppm_Api_Type_Sync,                            /**< SYNC API call */
     Adsppm_Api_Type_Async,                           /**< ASYNC API call */
     Adsppm_Ape_Type_Enum_Max,                        /**< enum max */
     Adsppm_Api_Type_Force8bits = 0x7F       /**< forcing enum to be 32 bit */
} AdsppmApiType;  

/** 
*@enum ADSPPMStatusType - Values used for ADSPPM return status.
*/
typedef enum {
    Adsppm_Status_Success,                    /**< no error */
    Adsppm_Status_Failed,                     /**< general failure */
    Adsppm_Status_NoMemory,                   /**< insufficient Memory */
    Adsppm_Status_VersionNotSupport,          /**< version not supported */
    Adsppm_Status_BadClass,                   /**< NULL class object */
    Adsppm_Status_BadState,                   /**< invalid state */
    Adsppm_Status_BadParm,                    /**< invalid parameter */
    Adsppm_Status_InvalidFormat,              /**< invalid format */
    Adsppm_Status_UnSupported,                /**< API is not supported */
    Adsppm_Status_ResourceNotFound,           /**< Resouce not found */
    Adsppm_Status_BadMemPtr,                  /**< bad memory pointer */
    Adsppm_Status_BadHandle,                  /**< invalid handle */
    Adsppm_Status_ResourceInUse,              /**< a resource is in use */
    Adsppm_Status_NoBandwidth,                /**< insufficient bandwidth */
    Adsppm_Status_NullPointer,                /**< null pointer */
    Adsppm_Status_NotInitialized,             /**< not initialized */
    Adsppm_Status_ResourceNotRequested,       /**< resource not requested */
    Adsppm_Status_CoreResourceNotAvailable,   /**< Core Resource not available */
    Adsppm_Status_Max,                        /**< Maximum count */
    Adsppm_Status_Force32Bits = 0x7FFFFFFF    /**< forcing enum to be 32 bit */
} AdsppmStatusType;

typedef AdsppmStatusType                  Adsppm_Status;          /**< ADSPPM status type */

/**
*@enum AdsppmCoreIdType - ADSPPM CORE ID 
*/

typedef enum
{
    Adsppm_Core_Id_None        = 0,                   /**< None */
    Adsppm_Core_Id_ADSP        = 1,                  /**< Q6SS HW inside of LPASS */
    Adsppm_Core_Id_LPM         = 2,                  /**< Low Power Memory inside of LPASS */
    Adsppm_Core_Id_DML         = 3,                  /**< DML (Data Mover Lite) HW inside of LPASS */
    Adsppm_Core_Id_AIF         = 4,                  /**< AIF(Audio Intreface) HW inside of LPASS */
    Adsppm_Core_Id_SlimBus     = 5,                  /**< SLIMBus HW inside of LPASS */
    Adsppm_Core_Id_Midi        = 6,                  /**< MIDI HW inside of LPASS */
    Adsppm_Core_Id_AVsync      = 7,                  /**< AVT/ProntoSync HW inside of LPASS */
    Adsppm_Core_Id_HWRSMP      = 8,                 /**< HW Resampler HW inside of LPASS */
    Adsppm_Core_Id_SRam        = 9,                 /**< Sensors RAM */
    Adsppm_Core_Id_DCodec      = 10,                /**< Digital codec */
    Adsppm_Core_Id_Spdif       = 11,                /** < SPDIF */
	Adsppm_Core_Id_Hdmirx      = 12,                /**< HDMIRX */
    Adsppm_Core_Id_Sif         = 13,                /**< analog audio */
    Adsppm_Core_Id_BSTC        = 14,                /**< BSTC */
    Adsppm_Core_Id_Max,                               /**< Maximum count */
    Adsppm_Core_Id_Force8Bits = 0x7F           /**< forcing enum to be 8 bit */
}AdsppmCoreIdType;

/**
*@enum AdsppmRscIdType - ADSPPM resource ID 
*/

typedef enum
{
    Adsppm_Rsc_Id_None            = 0,                    /**< None */
    Adsppm_Rsc_Id_Power           = 1,
    Adsppm_Rsc_Id_Core_Clk        = 2,
    Adsppm_Rsc_Id_Sleep_Latency   = 3,
    Adsppm_Rsc_Id_Mips            = 4, 
    Adsppm_Rsc_Id_BW              = 5,
    Adsppm_Rsc_Id_Thermal         = 6,
    Adsppm_Rsc_Id_MemPower        = 7,
    Adsppm_Rsc_Id_Core_Clk_Domain = 8,
    Adsppm_Rsc_Id_Max             ,                       /**< Maximum count */
    Adsppm_Rsc_Id_Force8Bits     = 0x7F            /**< forcing enum to be 8 bit */
}AdsppmRscIdType;


/**
*@enum AdsppmCallbackEventIdType - Callback events 
*/
typedef enum {
    Adsppm_Callback_Event_Id_None,                    /**< None */
	Adsppm_Callback_Event_Id_Thermal = 0x0002,        /**< Callback event: thermal event */
	Adsppm_Callback_Event_Id_Async_Complete = 0x0004, /**< Callback event: async request complete event */
    Adsppm_Callback_Event_Id_Idle,                    /**< Callback event: except for MDP, MMSS is idle */
    Adsppm_Callback_Event_Id_Busy,                    /**< Callback event: except for MDP, MMSS is busy */
    Adsppm_Callback_Event_Id_Max,                     /**< Maximum count */
    Adsppm_Callback_Event_Id_Force32Bits = 0x7FFFFFFF /**< forcing enum to be 32 bit */
} AdsppmCallbackEventIdType;

typedef enum {
    Adsppm_ClientType_Normal,
    Adsppm_ClientType_Periodic
} AdsppmClientTypeType;

/******** rscId = Adsppm_Rsc_Id_Thermal ********/
/**
 Thermal mitigation request 
*/
typedef enum {
    Adsppm_Thermal_NONE,                             /**< None */
    Adsppm_Thermal_LOW,                              /**< Low temperature condition */
    Adsppm_Thermal_NORM,                             /**< Normal temperature condition. No thermal mitigation needed. */
    Adsppm_Thermal_High_L1,                          /**< High temperature condition level 1*/
    Adsppm_Thermal_High_L2,                          /**< High temperature condition level 2*/
    Adsppm_Thermal_High_L3,                          /**< High temperature condition level 3*/
    Adsppm_Thermal_High_L4,                          /**< High temperature condition level 4*/
    Adsppm_Thermal_High_L5,                          /**< High temperature condition level 5*/
    Adsppm_Thermal_High_L6,                          /**< High temperature condition level 6*/
    Adsppm_Thermal_High_L7,                          /**< High temperature condition level 7*/
    Adsppm_Thermal_High_L8,                          /**< High temperature condition level 8*/
    Adsppm_Thermal_High_L9,                          /**< High temperature condition level 9*/
    Adsppm_Thermal_High_L10,                         /**< High temperature condition level 10*/
    Adsppm_Thermal_High_L11,                         /**< High temperature condition level 11*/
    Adsppm_Thermal_High_L12,                         /**< High temperature condition level 12*/
    Adsppm_Thermal_High_L13,                         /**< High temperature condition level 13*/
    Adsppm_Thermal_High_L14,                         /**< High temperature condition level 14*/
    Adsppm_Thermal_High_L15,                         /**< High temperature condition level 15*/
    Adsppm_Thermal_High_L16,                         /**< High temperature condition level 16*/
    Adsppm_Thermal_Max,                              /**< Maximum count */
    Adsppm_Thermal_Force32Bits = 0x7FFFFFFF          /**< forcing enum to be 32 bit */
} AdsppmThermalType;



/**
*@struct AdsppmCallbackParamType - Callback parameter structure 
*/
/** 
 Data structure for event callback
*/

typedef struct {
    AdsppmCallbackEventIdType     eventId;
    uint32                        clientId;
	uint32						  callbackDataSize;   /**< size of callbackData */
    void*                         callbackData;       /**< data structure of callbackData to be defined when async calls are supported */
} AdsppmCallbackParamType;




/**
*@typedef
*@struct AdsppmHandleType - adsppm handle definition for saving information for client.
*/
typedef struct AdsppmHandle {
    coreUtils_Q_LinkType            AdsppmHandleLink;  //!< link list
    uint32                          clientId;       //returned clientId from a successful registration
    AdsppmCoreIdType                coreId;
    char                            clientName[MAX_ADSPPM_CLIENT_NAME];
    uint32                          callBackFlag;
    uint32                          (*ADSPPM_Callback)(AdsppmCallbackParamType *pCbParam);
    void                            *pSetParamConfig;
    AdsppmClientTypeType            clientType;
    //void                            *pClientRMCtx;
} AdsppmHandleType; 


/******************************************************************************
 *                     Bus/Clock definitions
 *****************************************************************************/
/**
 * @enum AdsppmClkIdType - defines clock IDs as used by ADSPPM internally
 */
typedef enum{
    AdsppmClk_None = 0,                  //!< AdsppmClk_None
    AdsppmClk_Adsp_Core,                 //!< AdsppmClk_Adsp_Core
    AdsppmClk_Ahb_Root,                  //!< AdsppmClk_Ahb_Root
    AdsppmClk_Adsp_Hclk,                 //!< AdsppmClk_Adsp_Hclk
    AdsppmClk_HwRsp_Hclk,                //!< AdsppmClk_HwRsp_Hclk
    AdsppmClk_Dml_Hclk,
    AdsppmClk_Aif_Hclk,
    AdsppmClk_Aif_Csr_Hclk,
    AdsppmClk_Slimbus_Hclk,
    AdsppmClk_Midi_Hclk,
    AdsppmClk_AvSync_Hclk,
    AdsppmClk_Lpm_Hclk,
    AdsppmClk_Sram_Hclk,
    AdsppmClk_Lcc_Hclk,
    AdsppmClk_AhbE_Hclk,
    AdsppmClk_AhbI_Hclk,
    AdsppmClk_AhbX_Hclk,
    AdsppmClk_Csr_Hclk,
    AdsppmClk_Security_Hclk,
    AdsppmClk_Wrapper_Security_Hclk,
    AdsppmClk_Wrapper_Br_Hclk,
    AdsppmClk_Dcodec_Hclk,
    AdsppmClk_Spdif_Hmclk,
    AdsppmClk_Spdif_Hsclk,
    AdsppmClk_Smmu_Adsp_Hclk,
    AdsppmClk_Smmu_Lpass_Hclk,
    AdsppmClk_Smmu_Sysnoc_Hclk,
	AdsppmClk_Hdmirx_Hclk,
    AdsppmClk_Sif_Hclk,
    AdsppmClk_Bstc_Hclk,
    AdsppmClk_HwRsp_Core,                //!< AdsppmClk_HwRsp_Core Core clocks start here
    AdsppmClk_Midi_Core,
    AdsppmClk_AvSync_Xo,
    AdsppmClk_AvSync_Bt,
    AdsppmClk_AvSync_Fm,
    AdsppmClk_Slimbus_Core,
    AdsppmClk_Avtimer_core,
    AdsppmClk_Atime_core,
	AdsppmClk_Atime2_core,
	AdsppmClk_BSTC_core,
    AdsppmClk_EnumMax,                   //!< Max number of the items in the enum
    AdsppmClk_EnumForce32Bit = 0x7fffffff//!< AdsppmClk_EnumForce32Bit
}AdsppmClkIdType;


/******************************************************************************
 *                     Bus definitions
 *****************************************************************************/
/**
 * @enum AdsppmBusPortIdType - defines Bus ports to be used by ADSPPM internally
 */
typedef enum{
    AdsppmBusPort_None = 0,                   //!< AdsppmBusMaster_None
    AdsppmBusPort_Adsp_Master,                //!< AdsppmBusMaster_Adsp
    AdsppmBusPort_Dml_Master,                 //!< AdsppmBusMaster_Dml
    AdsppmBusPort_Aif_Master,                 //!< AdsppmBusMaster_Aif
    AdsppmBusPort_Slimbus_Master,             //!< AdsppmBusMaster_Slimbus
    AdsppmBusPort_Midi_Master,                //!< AdsppmBusMaster_Midi
    AdsppmBusPort_HwRsmp_Master,              //!< AdsppmBusMaster_HwRsmp
    AdsppmBusPort_Ext_Ahb_Master,             //!< SNOC Master port from AHB-X
    AdsppmBusPort_Spdif_Master,               //!< SPDIF master port
    AdsppmBusPort_Hdmirx_Master,              //!< HDMIRX slave port
    AdsppmBusPort_Sif_Master,                 //!< SIF slave port    
    AdsppmBusPort_Dml_Slave,                  //!< AdsppmBusSlave_Dml
    AdsppmBusPort_Aif_Slave,                  //!< AdsppmBusSlave_Aif
    AdsppmBusPort_Slimbus_Slave,              //!< AdsppmBusSlave_Slimbus
    AdsppmBusPort_Midi_Slave,                 //!< AdsppmBusSlave_Midi
    AdsppmBusPort_HwRsmp_Slave,               //!< AdsppmBusSlave_HwRsmp
    AdsppmBusPort_AvSync_Slave,               //!< AdsppmBusSlave_AvSync
    AdsppmBusPort_Lpm_Slave,                  //!< AdsppmBusSlave_Lpm
    AdsppmBusPort_Sram_Slave,                 //!< AdsppmBusSlave_Sram
    AdsppmBusPort_Ext_Ahb_Slave,              //!< SNOC Slave port from AHB-E
    AdsppmBusPort_Ddr_Slave,                  //!< AdsppmBusSlave_Ddr
    AdsppmBusPort_Ocmem_Slave,                //!< AdsppmBusSlave_Ocmem
    AdsppmBusPort_PerifNoc_Slave,             //!< AdsppmBusSlave_PerifNoc
    AdsppmBusPort_DCodec_Slave,				  //!< Digital Codec Slave port
    AdsppmBusPort_Spdif_Slave,                //!< SPDIF slave port
    AdsppmBusPort_Hdmirx_Slave,               //!< HDMIRX slave port
    AdsppmBusPort_Sif_Slave,                  //!< SIF slave port
    AdsppmBusPort_Bstc_Slave,                 //!< BSTC slave port
    AdsppmBusPort_Core,                       //!< Core current client is registered for. Should be specified in the srcPortId for Core-toMemory transfers*/
    AdsppmBusPort_EnumMax,                    //!< AdsppmBusSlave_EnumMax
    AdsppmBusPort_EnumForce32Bit = 0x7fffffff //!< AdsppmBusSlave_EnumForce32Bit
} AdsppmBusPortIdType;

/**
 * Definition for the bus route type (master/slave pair)
 */
typedef struct{
    AdsppmBusPortIdType masterPort;
    AdsppmBusPortIdType slavePort;
} AdsppmBusRouteType;

/**
 * Data type holding BW value for type 3 requests (Ab/Ib)
 */
typedef struct{
    uint64 Ab; //!< Average BW in bytes per second
    uint64 Ib; //!< Instanteneous BW in bytes per second
} AdsppmBusBWDataIbAbType;


/**
 BW usage types for LPASS
*/
typedef enum {
    Adsppm_BwUsage_None,                      /**< None */
    Adsppm_BwUsage_DSP,                       /**< BW usage by ADSP*/
    Adsppm_BwUsage_DMA,                       /**< BW usage by DMA/BAM*/
    Adsppm_BwUsage_EXT_CPU,                   /**< BW usage by CPU outside of LPASS */
    Adsppm_BwUsage_Enum_Max,                  /**< Maximum count */
    Adsppm_BwUsage_Force32Bbits = 0x7FFFFFFF   /**< forcing enum to be 32 bit */
} AdsppmBwUsageLpassType;


/**
 * Data type holding BW value for type 1 requests (BW, usage)
 */
typedef struct{
    uint64 bwBytePerSec;                       //!< BW value in bytes per second
    uint32 usagePercentage;                    //!< utilization percentage
    AdsppmBwUsageLpassType usageType;          //!< BW usage type
} AdsppmBusBWDataUsageType;

typedef union{
    AdsppmBusBWDataIbAbType busBwAbIb;
    AdsppmBusBWDataUsageType busBwValue;
} AdsppmBusBWDataType;

typedef enum{
    AdsppmBusBWOperation_BW = 1,     //!< Normal BW request
    AdsppmBusBWOperation_RegAccess   //!< BW for register access
}AdsppmBusBWOperationType;

/**
 * Data type specifying BW request parameters
 */
typedef struct{
    AdsppmBusRouteType busRoute; //!< master/slave pair the BW is requested for
    AdsppmBusBWDataType bwValue; //!< BW value
    AdsppmBusBWOperationType bwOperation; //!< BW request reason
} AdsppmBusBWRequestValueType;

/**
 adsppm BW value array that contains BW settings for each corresponding mem ID
*/
typedef struct {
    uint32                         numOfBw;            /**< number of BW settings in the array, max is 16 per client */
    AdsppmBusBWRequestValueType    pBwArray[MAX_NUM_OF_ADSPPM_BW_ARRAY];   /**< the array of BW settings */
} AdsppmBwReqType;


typedef enum{
    MipsRequestOperation_None = 0,                   //!< MipsRequestOperation_None
    MipsRequestOperation_MIPS = 1,                   //!< MipsRequestOperation_MIPS
    MipsRequestOperation_BWandMIPS = 2,              //!< MipsRequestOperation_BWandMIPS
    MipsRequestOperation_MAX = 3,                    //!< MipsRequestOperation_MAX
    MipsRequestOperation_EnumForce8Bit = 0x7f    //!< MipsRequestOperation_EnumForce8Bit
}AdsppmMipsOperationType;

/**
*@struct MIPS request structure for 
*/
typedef struct {
    uint32                 mipsTotal;                          /**< Total MIPS value*/
    uint32                 mipsPerThread;                      /**< MIPS value per HW thread*/
    AdsppmBusPortIdType    codeLocation;                       /**< Location where code is executed from */
    AdsppmMipsOperationType  reqOperation;                     /**< Which operations the MIPS request should include*/
    // The above structure matches MmpmMipsReqType
	AdsppmClientTypeType   clientType; 			               /**< Type of the client*/
} AdsppmMipsRequestType;


/**
 Frequecy clk match 
*/
typedef enum {
    Adsppm_Freq_At_Least,                             /**< At least */
    Adsppm_Freq_At_Most,                              /**< At most */
    Adsppm_Freq_Closest,                              /**< Closest */
    Adsppm_Freq_Exact,                                /**< Exact */
    Adsppm_Freq_Max,                                  /**< Maximum count */
    Adsppm_Freq_Force8Bits = 0x7F                    /**< forcing enum to be 8 bit */
} AdsppmFreqMatchType;

/**
 Adsppm info type definition. 
*/
typedef enum {
    Adsppm_Info_Type_None,                              /**< None */
    Adsppm_Info_Type_Max_Value,                         /**< get MAX value */   
    Adsppm_Info_Type_Min_Value,                      	/**< get Min Value */   
	Adsppm_Info_Type_Current_Value,                     /**< Get current Value*/
    Adsppm_Info_Type_Max,                               /**< Maximum count */ 
    Adsppm_Info_Type_Force8Bits = 0x7F          /**< forcing enum to be 32 bit */
} AdsppmInfoType;

/**
 Clock setting for one clock ID 
*/
typedef struct {
    AdsppmClkIdType           clkId;              /**< Core clock ID(s) defined by enums of AdsppmClkIdXyzType, Xyz is the core name */
    uint32                      clkFreqHz;          /**< Unit of clock Freq is Hz */ 
    AdsppmFreqMatchType           freqMatch;          /**< Frequency match: at least, at most, closest, and exact */
} AdsppmClkValType;

/** 
 * get clock freq in Hz   */

typedef struct {
    uint32                      clkId;              /**< Input: Core clock ID(s) defined by AdsppmClkIdType */
    uint32                      clkFreqHz;          /**< Output: Clock frequency in (hz) */
    uint32                      forceMeasure;       /**< Control to force the clock to be measured, clock freq may be read from a cached table if this is not set to 1. */
} AdsppmInfoClkFreqType;


/**
 Clock value array that contains clock frequencies for each corresponding clock ID
*/
typedef struct {
    uint32                      numOfClk;           /**< number of clock settings in the array*/  
    AdsppmClkValType            pClkArray[MAX_NUM_OF_ADSPPM_CLOCK_ARRAY];         /**< the array of clock settings */
} AdsppmClkRequestType;

/**
  Clock ID for LPASS.
*/
typedef enum {
    Adsppm_Clk_Domain_Src_Id_Lpass_None,
    /**< None. */
    Adsppm_Clk_Domain_Src_Id_Q6PLL,
    /**< AVSYNC clock. */
    Adsppm_Clk_Domain_Src_Id_AudioPLL,
    /**< AVTIMER clock. */
    Adsppm_Clk_Domain_Src_Id_PriusPLL,
    /**< ATIME clock. */
    Adsppm_Clk_Domain_Src_Id_Lpass_Max,
    /**< Maximum count. */
    Adsppm_Clk_Domain_Src_Id_Lpass_Force32bits = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} AdsppmClkDomainSrcIdLpassType;



/**
  Clock ID for all cores.
*/
typedef union {

	AdsppmClkDomainSrcIdLpassType clkDomainSrcIdLpass;
	/**< Clock domain source IDs of LPASS	@newpagetable */
} AdsppmClkDomainSrcIdType;

/**
  Clock domain setting for one clock ID. 
*/
typedef struct {
    AdsppmClkIdType             clkId;
    /**< Core clock ID defined by enumerations of AdsppmClkIdXyzType, where 
         Xyz is the core name. */
    uint32                      clkFreqHz; 			 
	/**< Unit of clock Freq is Hz. This is the frequency of clkId that gets configured using mnd */ 
    AdsppmClkDomainSrcIdType      clkDomainSrc;
    /**< Clock domain source ID. */
} AdsppmClkDomainType;
/**
  Clock domain array that contains clock domain settings for each 
  corresponding clock ID.
*/
typedef struct {
    uint32                      numOfClk;
    /**< Number of clock settings in the array. */
    AdsppmClkDomainType           pClkDomainArray[MAX_NUM_OF_ADSPPM_CLOCK_ARRAY];
    /**< Array of clock domain settings. */
} AdsppmClkDomainReqType;


/**
* structure for MIPS aggregation data
*/

typedef struct{
       uint32              numOfClients;
       uint32              mipsTotal;         /**< Total MIPS value*/
       uint32              mipsPerThread;     /**< MIPS value per HW thread*/
       uint32              cppFactor;         /**< CPP factor represented as total CPP*1000*/
       uint32              numOfPeriodicClients;               /**< Number of periodic clients*/
       uint32              footprintFactor;  /**< memory footprint indicator (SUM(BW/MPPS)) */
}AdsppmMIPSAggregateType;


/**
* structure for mipstoBW aggregate data 
*/

typedef struct{
       AdsppmBusPortIdType mipsBWSlavePort;   /**< */
       AdsppmBusBWRequestValueType  mipsToBW; /**< [OUT] returns BW and bus route*/
}AdsppmMIPSToBWAggregateType;


/**
*@struct MIPS data structure for Aggregate
*/
typedef struct
{
	AdsppmMIPSAggregateType mipsAggregate;
    AdsppmMIPSToBWAggregateType  mipsToBWAggregateDataDdr;
}AdsppmMipsAggregateDataType;

/******** rscId = Adsppm_Rsc_Id_MemPower ********/
/**
 Memory power state
*/
typedef enum {
    Adsppm_Mem_Power_None,                             /**< None */
    Adsppm_Mem_Power_Off,                              /**< Memory off */
    Adsppm_Mem_Power_Retention,                        /**< Memory in retention */
    Adsppm_Mem_Power_Active,                           /**< Memory in active state */
    Adsppm_Mem_Power_Max,                              /**< Maximum count */
    Adsppm_Mem_Power_Force32Bits = 0x7FFFFFFF          /**< forcing enum to be 32 bit */
} AdsppmMemPowerStateType;

/**
 * Memory Id definition for Memory Power state requests
 */
typedef enum{
    Adsppm_Mem_None,                              /**< None*/
    Adsppm_Mem_Lpass_LPM,                         /**< LPM*/
    Adsppm_Mem_Max,                               /**< Maximum count */
    Adsppm_MEM_FORCE32BITS = 0x7FFFFFFF           /**< forcing enum to be 32 bit */
} AdsppmMemIdType;

/**
 * Memory power state request parameter structure
 */
typedef struct{
    AdsppmMemIdType memory;
    AdsppmMemPowerStateType powerState;
}AdsppmMemPowerReqParamType;


/** Adsppm Parameter Configuration
 *
 */

/**
 * @enum AdsppmParameterIdType - defines parameter IDs for Adsppm_SetParameter()
 */
typedef enum{
    Adsppm_Param_Id_None = 0,         //!< Invalid parameter
    Adsppm_Param_Id_Resource_Limit,   //!< Set resource limits (min and max). Works globally
    Adsppm_Param_Id_Memory_Map,              //!< Set memory address map
    Adsppm_Param_Id_Enum_Max,         //!< Enumeration max
    Adsppm_Param_Id_Force32bit = 0x7fffffff
} AdsppmParameterIdType;


/** @struct AdsppmParameterConfigType - data structure for parameter configuration API
 *
 *  Based on paramId pParamConfig will take different struvture pointers:
 *  paramId:
 *      Adsppm_Param_Id_Client_Ocmem_Map - pParamConfig points to AdsppmOcmemMapType structure
 */
typedef struct{
    AdsppmParameterIdType paramId; //!< Parameter ID
    void*               pParamConfig; //!< Pointer to Parameter-specific structure
} AdsppmParameterConfigType;

typedef enum{
    Adsppm_State_ACMInit = 0x1,
    Adspmm_State_HalHwIoInit = 0x2,
    Adsppm_State_HalClkRgmInit = 0x4, 
    Adsppm_State_HalBusInit = 0x8,
    Adsppm_State_HalSlpInit = 0x10,
    Adsppm_State_HalCAInit = 0x20,
    Adsppm_State_CoreCtxLockInit = 0x40,
    Adsppm_State_CoreRMInit = 0x80,
    Adsppm_State_CoreAMAsyncInit = 0x100,
    Adsppm_State_CoreMIPSInit = 0x200,
    Adsppm_State_CoreBUSInit = 0x400,
    Adsppm_State_CoreAHBMInit = 0x800,
    Adsppm_State_CorePWRInit = 0x1000,
    Adsppm_State_CoreCLKInit = 0x2000,
    Adsppm_State_CoreSLEEPInit = 0x4000,
    Adsppm_State_CoreTHERMALInit = 0x8000,
    Adsppm_State_CoreMEMPWRInit = 0x10000,
    Adsppm_State_Force32bit = 0x7fffffff
}AdsppmInitStateType;
/******************************************************************************
                                 Utilities
*******************************************************************************/
#define ADSPPM_ASSERT(x)                       assert(x)

#ifndef MAX
#define MAX(a,b)                               ( ((a)>(b))?(a):(b) )
#endif

#ifndef MIN
#define MIN(a,b)                               ( ((a)<(b))?(a):(b) )
#endif

/*******************************************************************************
 *                       Functions
 *******************************************************************************/

Adsppm_Status ADSPPM_Init(void);
extern __inline uint32 ADSPPM_IsInitialized(void);

/**
 * This function is called by Sleep driver at the very late stage of entering ADSP PC.
 * The function will ensure that AHB clock is set as low as possible while ADSP is in power collapse.
 * Limitation: the function should only be called from Sleep context after entering STM.
 */
void ADSPPM_EnterSleep(void);

/**
 * This function is called by SLeep driver early during exit from ADSP power collapse.
 * The function will restore AHB clock to the frequency , which was set before entering power collapse.
 * Limitation: the function should only be called from Sleep context after entering STM.
 */
void ADSPPM_ExitSleep(void);

#endif /* ADSPPM_H_ */
