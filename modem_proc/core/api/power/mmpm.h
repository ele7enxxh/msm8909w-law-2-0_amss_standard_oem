#ifndef MMPM_H
#define MMPM_H


/*============================================================================*/
/**
 @file mmpm.h

 @brief This file contains data types, constants that make up the
        Multi Media Power Management (MMPM) public API.

 DESCRIPTION:
     Public Function Declarations 
     This is the MMPM client API. 

 EXTERNALIZED FUNCTIONS:    
     See below.
*/
/*==============================================================================
NOTE: The @brief description above does not appear in the PDF.

      The adsppm_2.x_mainpage.dox file contains the group/module descriptions 
      that are displayed in the output PDF generated using Doxygen and LaTeX. 
      To edit or update any of the group/module text in the PDF, edit the
      adsppm_2.x_mainpage.dox file or contact Tech Pubs.
==============================================================================*/
/*==============================================================================
  Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/api/power/mmpm.h#1 $

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
--------     ---     ---------------------------------------------------------
06/15/09    Shiju    Initial version
06/30/09    PK       Updating format to conform to QCT Coding Standard.
08/05/09    Shiju    Updated with review feedbacks and fixed compilation issues
08/19/09    CK       Added data structures for MMSS common components.
                     Updated comments for the camera components' parameters.
09/29/09    AC       Updated video parameters.
10/16/09    PK       Updated comment format for Doxygen.
10/28/09    CK       Removed API's that are not directly exposed to clients.
11/12/09    CY       Revised Audio MMPM APIs.
05/12/10    CY       Design changes to Audio MMPM API.
05/17/10    AC       Modified bandwidth request data structure.
07/15/10    CK       Added ddr clock to use case parameter structure of MipiCsi
08/20/10    Shiju    Added support for DSI clks
02/10/11    CK       Added parameter to define ISPIF interface
06/19/11    CK       Created MMPM 2.0 structures, backward compatible with 1.0
08/16/11    CK       Added comments on each clock enum ID with the expected behavior on them.
                     Merged 1.0 header file updates (till rev68) into the 2.0 header file's 1.0 part
12/12/11    YR       Extended interface for LPASS
04/12/13    llg     (Tech Pubs) Edited/added Doxygen comments and markup.
==============================================================================*/

/*============================================================================*/
/*
*                           INCLUDE FILES FOR MODULE
*/
/*============================================================================*/

/*======================================================================
  Enumerations
  ======================================================================*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*======================================================================*/
/*
  Types/Typedefs
*/
/*======================================================================*/
/** @addtogroup mmpm_datatypes 
@{ */

#ifndef NULL
#define NULL  ((void*) 0)
#endif

#ifndef _BOOLEAN_DEFINED
typedef unsigned char       boolean;     /**< Boolean value. */
#define _BOOLEAN_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef unsigned long int   uint32;      /**< Unsigned 32-bit value. */
#define _UINT32_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef signed long int     int32;       /**< Signed 32-bit value. */
#define _INT32_DEFINED
#endif

#ifndef _UINT64_DEFINED
typedef unsigned long long  uint64;      /**< Unsigned 64-bit value. */
#define _UINT64_DEFINED
#endif

#ifndef _INT64_DEFINED
typedef signed long long    int64;       /**< Signed 64-bit value. */
#define _INT64_DEFINED
#endif

#ifndef _BYTE_DEFINED
typedef unsigned char       byte;        /**< Unsigned 8-bit value. */
#define _BYTE_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef signed char         int8;        /**< Signed 8-bit value. */
#define _INT8_DEFINED
#endif

#ifndef _BOOL32_DEFINED
typedef unsigned long int   bool32;      /**< Unsigned long boolean value. */
#define _BOOL32_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef unsigned char       uint8;       /**< Unsigned 8-bit value. */
#define _UINT8_DEFINED
#endif


/** 
  MMPM status type used for the MMPM return status.
*/
typedef enum {
    MMPM_STATUS_SUCCESS,                   /**< Success. */
    MMPM_STATUS_FAILED,                   /**< General failure. */
    MMPM_STATUS_NOMEMORY,                 /**< Insufficient memory. */
    MMPM_STATUS_VERSIONNOTSUPPORT,        /**< Version is not supported. */
    MMPM_STATUS_BADCLASS,                 /**< NULL class object. */
    MMPM_STATUS_BADSTATE,                 /**< Invalid state. */
    MMPM_STATUS_BADPARM,                  /**< Invalid parameter. */
    MMPM_STATUS_INVALIDFORMAT,            /**< Invalid format. */
    MMPM_STATUS_UNSUPPORTED,              /**< API is not supported. */
    MMPM_STATUS_RESOURCENOTFOUND,         /**< Resource is not found. */
    MMPM_STATUS_BADMEMPTR,                /**< Bad memory pointer. */
    MMPM_STATUS_BADHANDLE,                /**< Invalid handle. */
    MMPM_STATUS_RESOURCEINUSE,            /**< Resource is in use. */
    MMPM_STATUS_NOBANDWIDTH,              /**< Insufficient bandwidth. */
    MMPM_STATUS_NULLPOINTER,              /**< NULL pointer. */
    MMPM_STATUS_NOTINITIALIZED,           /**< Not initialized. */
    MMPM_STATUS_RESOURCENOTREQUESTED,     /**< Resource was not requested. */
    MMPM_STATUS_CORERESOURCENOTAVAILABLE, /**< Core resource is not 
                                               available. */
    MMPM_STATUS_MAX,                      /**< Maximum count. */
    MMPM_STATUS_FORCE32BITS = 0x7FFFFFFF  /**< Forces the enumeration to 
                                               32 bits. */
} MmpmStatusType;


typedef MmpmStatusType                  MMPM_STATUS;  /**< MMPM status type. */
/** @} */ /* end_addtogroup mmpm_datatypes  */


/******************************************************************************************/
/*                                                                                        */
/*                          MMPM_Register_Ext() Data Structures                           */
/*                                                                                        */
/******************************************************************************************/
   
/** @ingroup mmpm_constants_macros
 MMPM revision.
*/
#define MMPM_REVISION                   0x00000002

/** @addtogroup mmpm_datatypes 
@{ */
/**
 MMPM owner ID type. 
 Each MMPM core has a unique core ID. All requests to MMPM are routed to 
 the right core based its ID. Maximum number of core IDs: 1024.
*/
typedef enum {
    MMPM_CORE_ID_NONE        = 0,     /**< None. */
/** @cond apps_only */
    MMPM_CORE_ID_2D_GRP      = 1,     /**< 2D graphics. */
    MMPM_CORE_ID_3D_GRP      = 2,     /**< 3D graphics. */
    MMPM_CORE_ID_MDP         = 3,     /**< MDP. */     
    MMPM_CORE_ID_VCODEC      = 4,     /**< Video codec. */
    MMPM_CORE_ID_VPE         = 5,     /**< VPE. */
    MMPM_CORE_ID_VFE         = 6,     /**< VFE. */
    MMPM_CORE_ID_MIPICSI     = 7,     /**< Mobile Industry Processor Interface 
                                           (MIPI) CSI. */
    MMPM_CORE_ID_SENSOR      = 8,     /**< Camera sensor. */
    MMPM_CORE_ID_JPEGD       = 9,     /**< JPEG decoder. */
    MMPM_CORE_ID_JPEGE       = 10,    /**< JPEG encoder. */
    MMPM_CORE_ID_FABRIC      = 11,    /**< MMSS FABRIC. */
    MMPM_CORE_ID_IMEM        = 12,    /**< MMSS IMEM. */
    MMPM_CORE_ID_SMMU        = 13,    /**< MMSS system memory management units. */
    MMPM_CORE_ID_ROTATOR     = 14,    /**< Rotator. */
    MMPM_CORE_ID_TV          = 15,    /**< TV. */
    MMPM_CORE_ID_DSI         = 16,    /**< DSI. */
    MMPM_CORE_ID_AUDIOIF     = 17,    /**< Audio interface. */
    MMPM_CORE_ID_GMEM        = 18,    /**< MMSS GMEM. */
/** @endcond */
    // LPASS core ID START
    MMPM_CORE_ID_LPASS_START = 100,   /**< LPASS core ID start. */
    MMPM_CORE_ID_LPASS_ADSP  = 101,   /**< aDSP hardware inside LPASS. */
    MMPM_CORE_ID_LPASS_LPM   = 102,   /**< Low-power memory inside LPASS. */
    MMPM_CORE_ID_LPASS_DML   = 103,   /**< Data Mover Lite hardware inside 
                                           LPASS. */
    MMPM_CORE_ID_LPASS_AIF   = 104,   /**< Audio Interface hardware inside 
                                           LPASS. */
    MMPM_CORE_ID_LPASS_SLIMBUS=105,   /**< SLIMBus@regns hardware inside 
                                           LPASS. */
    MMPM_CORE_ID_LPASS_MIDI  = 106,   /**< MIDI hardware inside LPASS. */
    MMPM_CORE_ID_LPASS_AVSYNC= 107,   /**< AVT/ProntoSync hardware inside 
                                           LPASS. */
    MMPM_CORE_ID_LPASS_HWRSMP= 108,   /**< Hardware Resampler hardware inside 
                                           LPASS. */
    MMPM_CORE_ID_LPASS_SRAM  = 109,   /**< SRAM used by LPASS. */
    MMPM_CORE_ID_LPASS_DCODEC = 110,  /**< Digital codec inside LPASS. */
    MMPM_CORE_ID_LPASS_SPDIF = 111,   /** < SPDIF in LPASS */
    MMPM_CORE_ID_LPASS_HDMIRX = 112,  /**< HDMI Rx hardware in the LPASS. */
    MMPM_CORE_ID_LPASS_SIF = 113,     /**< SIF (analog audio) in the LPASS. */
    MMPM_CORE_ID_LPASS_BSTC = 114,    /**< Broadcast System Time Clock (STC) 
                                           in the LPASS. */
    MMPM_CORE_ID_LPASS_END,           /**< LPASS core ID end. */
    //LPASS CORE ID END
    MMPM_CORE_ID_MAX,                 /**< Maximum count. */
    MMPM_CORE_ID_FORCE32BITS = 0x7FFFFFFF
                                     /**< Forces the enumeration to 32 bits. */
} MmpmCoreIdType;


/******** rscId = MMPM_RSC_ID_EVENT_CALLBACK ********/
/**
 Callback events. 
*/
typedef enum {
    MMPM_CALLBACK_EVENT_ID_NONE,
    /**< None. */
    MMPM_CALLBACK_EVENT_ID_IDLE,
    /**< Except for MDP, MMSS is idle. */
    MMPM_CALLBACK_EVENT_ID_BUSY,
    /**< Except for MDP, MMSS is busy. */
    MMPM_CALLBACK_EVENT_ID_THERMAL,
    /**< Thermal event */
    MMPM_CALLBACK_EVENT_ID_COMPLETE,
    /**< Async request is completed. */
    MMPM_CALLBACK_EVENT_ID_MAX,
    /**< Maximum count. */
    MMPM_CALLBACK_EVENT_ID_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmCallbackEventIdType;


/**
 Callback parameter.
*/
/* 
 Data structure for event callback
*/
typedef struct {
    MmpmCallbackEventIdType     eventId;
    /**< Callback event ID. */
    uint32                      clientId;
    /**< MMPM client ID. */
    uint32                      callbackDataSize;
    /**< Size of callbackData. */
    void*                       callbackData;
    /**< Data structure of callbackData to be defined when asynchronous 
         calls are supported. For an asynchronous call, this points to the 
         #MmpmCompletionCallbackDataType structure. */
} MmpmCallbackParamType;



/** 
  Asynchronous request callback return value.
*/
typedef struct {
    uint32 reqTag;      /**< Request tag. */
    MMPM_STATUS result; /**< Request completion result. @newpagetable */
}MmpmCompletionCallbackDataType;
/** @} */ /* end_addtogroup mmpm_datatypes */

/** @addtogroup mmpm_constants_macros 
@{ */
/** @name Power control flag 
  @xreflabel{hdr:powerControlFlag}

  Bitmask that indicates which power control the client wants.
@{ */
/** Not participating in any power control. */
#define PWR_CTRL_NONE               0x0000
/** Participate in static display. */
#define PWR_CTRL_STATIC_DISPLAY     0x0001
/** Participate in thermal; MMPM manages thermal for the client. */
#define PWR_CTRL_THERMAL            0x0002
/** The client is a special periodic type requiring special handling for its 
  requests. Periodic clients assume that their requests are only applied if 
  they run concurrently with other clients. Otherwise, a pre-defined baseline 
  state is applied in order to optimize the timeline. */
#define PWR_CTRL_PERIODIC_CLIENT    0x0100          
/** @} */ /* end_name_group Power control flag */

/** @name Callback flag 
  @xreflabel{hdr:callbackFlag} 

  Bitmask that indicates which callback the client wants to receive.
@{ */
/** No callbacks */
#define CALLBACK_NONE               0x0000
/** Callback for static display. */
#define CALLBACK_STATIC_DISPLAY     0x0001
/** Callback for thermal. */
#define CALLBACK_THERMAL            0x0002
/** Callback for asynchronous request completion notification. */
#define CALLBACK_REQUEST_COMPLETE   0x0004
/** @} */ /* end_name_group Callback flag */
/** @} */ /* end_addtogroup mmpm_constants_macros */

/** @ingroup mmpm_datatypes 
  Core instance ID.
*/
typedef enum{
    MMPM_CORE_INSTANCE_NONE,  /**< None. */
    MMPM_CORE_INSTANCE_0,     /**< Core instance 0. */
    MMPM_CORE_INSTANCE_1,     /**< Core instance 1. */
    MMPM_CORE_INSTANCE_2,     /**< Core instance 2. */
    MMPM_CORE_INSTANCE_MAX,   /**< Maximum count. */
    MMPM_CORE_INSTANCE_FORCE32BITS = 0x7FFFFFFF
                             /**< Forces the enumeration to 32 bits. */
} MmpmCoreInstanceIdType;


/** @ingroup mmpm_constants_macros
  Maximum client name length.
*/
#define MAX_CLIENT_NAME                 32


/** @addtogroup mmpm_datatypes 
@{ */
/** 
 MMPM registration.
*/
typedef struct {
    uint32                      rev;
    /**< Must be set to #MMPM_REVISION. */

    MmpmCoreIdType              coreId;
    /**< Specifies the ID of the hardware block for which the client is 
         registering. Usually, this is the hardware core the client wants to 
         control. */
    MmpmCoreInstanceIdType      instanceId;
    /**< Instance of the same hardware core, e.g., the first instance of the 
         two Graphics2D cores. */
    char                        *pClientName;
    /**< Client name. This must be unique for each combination of coreId and 
         instanceId. Maximum size: 32 characters. */

    uint32                      pwrCtrlFlag;
    /**< Bitmask that indicates whether the client is participating in the 
         power control; see @xhyperref{hdr:powerControlFlag,Power control flag}. */
    
    uint32                      callBackFlag;
    /**< Bitmask that indicates which callback the client wants to receive; 
         see @xhyperref{hdr:callbackFlag,Callback flag}. */
    uint32                      (*MMPM_Callback)(MmpmCallbackParamType *pCbParam);
    /**< Callback function for event notification. */
    uint32                      cbFcnStackSize;
    /**< Callback function stack size. */
} MmpmRegParamType;  


/******************************************************************************************/
/*                                                                                        */
/*                    MMPM_Request() Data Structures                                  */
/*                                                                                        */
/******************************************************************************************/

/** 
  MMPM resource ID definition.  
*/
typedef enum {
    MMPM_RSC_ID_NONE            = 0,
    /**< None. */
    MMPM_RSC_ID_POWER           = 1,
    /**< GDFS for MMSS cores. */             
/** @cond apps_only */
    MMPM_RSC_ID_VREG            = 2,
    /**< Power rails for panels and sensors. */             
/** @endcond */
    MMPM_RSC_ID_REG_PROG        = 3,
    /**< Register programming clocks. */
    MMPM_RSC_ID_CORE_CLK        = 4,
    /**< Core clocks requested by passing in clock settings. Depending on 
         the clock ID, there are three types of operations: enable only, 
         set frequency and enable, and set clock source and enable. */
    MMPM_RSC_ID_CORE_CLK_DOMAIN  = 5,
/** @cond apps_only */ 
    /**< Core clocks requested by passing in clock domain settings. */
    MMPM_RSC_ID_MEM_BW          = 6,
    /**< Bandwidth requested by passing in values. The AXI clock is enabled 
         with the bandwidth request. */
    MMPM_RSC_ID_AXI_EN          = 7,
    /**< Core is to enable AXI clock only. */
    MMPM_RSC_ID_MIPS            = 8,
    /**< MIPS requested by passing in values. */
/** @endcond */
    MMPM_RSC_ID_SLEEP_LATENCY   = 9,
    /**< Interrupt latency requested by passing in values. */
/** @cond apps_only */
    MMPM_RSC_ID_ACTIVE_STATE    = 10,
    /**< Core is to notify MMPM that it is in the Active state. */
    MMPM_RSC_ID_PMIC_GPIO       = 11,
    /**< PMIC GPIO resource ID. */
    MMPM_RSC_ID_RESET           = 12,
    /**< Reset of core through the clock registers. */
/** @endcond */
    MMPM_RSC_ID_MIPS_EXT        = 13,
    /**< Extended MIPS requested by passing in MIPS, request type, and 
         code location values. */
    MMPM_RSC_ID_GENERIC_BW      = 14,
    /**< Generic bandwidth defined by endpoints. */
    MMPM_RSC_ID_THERMAL         = 15,
    /**< Thermal mitigation request. */
    MMPM_RSC_ID_MEM_POWER       = 16,
    /**< Request for Memory Power state. */
    MMPM_RSC_ID_MAX             ,
    /**< Maximum count. */
    MMPM_RSC_ID_FORCE32BITS     = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmRscIdType;

/******** rscId = MMPM_RSC_ID_REG_PROG ********/
/**
  Register programming match. 
*/
typedef enum {
    MMPM_REG_PROG_NONE,  /**< None. */
    MMPM_REG_PROG_NORM,  /**< Register programming at normal speed. */
    MMPM_REG_PROG_FAST,  /**< Register programming at fast speed. */
    MMPM_REG_PROG_MAX,   /**< Maximum count. */
    MMPM_REG_PROG_FORCE32BITS = 0x7FFFFFFF
                         /**< Forces the enumeration to 32 bits. */
} MmpmRegProgMatchType;


/******** rscId = MMPM_RSC_ID_CORE_CLK ********/
/**
  Clock ID for 2D graphics. 
*/
typedef enum {
    MMPM_CLK_ID_2D_GRP_NONE,
    /**< None. */
    MMPM_CLK_ID_2D_GRP,
    /**< 2D graphics clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable. */
    MMPM_CLK_ID_2D_GRP_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_2D_GRP_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkId2dGrpType;


/**
   Clock ID for 3D graphics.
*/
typedef enum {
    MMPM_CLK_ID_3D_GRP_NONE,
    /**< None. */
    MMPM_CLK_ID_3D_GRP,
    /**< 3D graphics clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable. */
    MMPM_CLK_ID_3D_GRP_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_3D_GRP_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkId3dGrpType;


/**
  Clock ID for MDP. 
*/
typedef enum {
    MMPM_CLK_ID_MDP_NONE,
    /**< None. */
    MMPM_CLK_ID_MDP,
    /**< MDP clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and enable. */
    MMPM_CLK_ID_MDP_VSYNC,
    /**< MDP Vsync clock; set by MMPM_RSC_ID_CORE_CLK: enable only. */
    MMPM_CLK_ID_MDP_LUT,
    /**< MDP LUT clock; set by MMPM_RSC_ID_CORE_CLK: enable only. */
    MMPM_CLK_ID_MDP_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_MDP_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdMdpType;


/**
  Clock ID for video codec.
*/
typedef enum {
    MMPM_CLK_ID_VCODEC_NONE,
    /**< None. */
    MMPM_CLK_ID_VCODEC,
    /**< Video codec clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable. */
    MMPM_CLK_ID_VCODEC_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_VCODEC_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdVcodecType;


/**
  Clock ID for VPE.
*/
typedef enum {
    MMPM_CLK_ID_VPE_NONE,
    /**< None. */
    MMPM_CLK_ID_VPE,
    /**< VPE clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and enable. */
    MMPM_CLK_ID_VPE_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_VPE_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdVpeType;


/**
  Clock ID for VFE.
*/
typedef enum {
    MMPM_CLK_ID_VFE_NONE,
    /**< None. */
    MMPM_CLK_ID_VFE,
    /**< VFE clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and enable. */
    MMPM_CLK_ID_VFE_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_VFE_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdVfeType;


/**
  Clock ID for MIPI CSI.
*/
typedef enum {
    MMPM_CLK_ID_CSI_NONE,
    /**< None. */  
    MMPM_CLK_ID_CSI,
    /**< CSI clock; instance ID as registered; 
         set by MMPM_RSC_ID_CORE_CLK: set frequency and enable. */  
    MMPM_CLK_ID_CSI_VFE,
    /**< CSI VFE clock; instance ID as registered; 
         set by MMPM_RSC_ID_CORE_CLK: enable only. */
    MMPM_CLK_ID_CSI_PHY,
    /**< CSI PHY clock; instance ID as registered; 
         set by MMPM_RSC_ID_CORE_CLK: enable only. */
    MMPM_CLK_ID_CSI_PHY_TIMER,
    /**< CSI PHY timer clock; instance ID as registered; 
         set by MMPM_RSC_ID_CORE_CLK: set frequency and enable. */
    MMPM_CLK_ID_CSI_PIX0,
    /**< CSI pixel clock 0; sourced by CSI instance ID; 
         set by MMPM_RSC_ID_CORE_CLK: set source and enable. @newpage */
    MMPM_CLK_ID_CSI_PIX1,
    /**< CSI pixel clock 1; sourced by CSI instance ID; 
         set by MMPM_RSC_ID_CORE_CLK: set source and enable. */
    MMPM_CLK_ID_CSI_RDI0,
    /**< CSI raw interface clock 0; sourced by CSI instance ID; 
         set by MMPM_RSC_ID_CORE_CLK: set source and enable. */
    MMPM_CLK_ID_CSI_RDI1,
    /**< CSI raw interface clock 1; sourced by CSI instance ID; 
         set by MMPM_RSC_ID_CORE_CLK: set source and enable. */
    MMPM_CLK_ID_CSI_RDI2,
    /**< CSI raw interface clock 2; sourced by CSI instance ID; 
         set by MMPM_RSC_ID_CORE_CLK: set source and enable. */
    MMPM_CLK_ID_CSI_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_CSI_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdCsiType;


/**
  Clock ID for sensor.
*/
typedef enum {
    MMPM_CLK_ID_SENSOR_NONE,
    /**< None. */  
    MMPM_CLK_ID_SENSOR,
    /**< Sensor clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable. */
    MMPM_CLK_ID_SENSOR_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_SENSOR_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdSensorType;


/**
  Clock ID for JPEG decoder.
*/
typedef enum {
    MMPM_CLK_ID_JPEGD_NONE,
    /**< None. */  
    MMPM_CLK_ID_JPEGD,
    /**< JPEG decoder clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable. */
    MMPM_CLK_ID_JPEGD_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_JPEGD_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdJpegdType;


/**
  Clock ID for JPEG encoder.
*/
typedef enum {
    MMPM_CLK_ID_JPEGE_NONE,
    /**< None. */  
    MMPM_CLK_ID_JPEGE,
    /**< JPEG encoder clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable. */
    MMPM_CLK_ID_JPEGE_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_JPEGE_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. @newpage */
} MmpmClkIdJpegeType;


/**
  Clock ID for rotator.
*/
typedef enum {
    MMPM_CLK_ID_ROTATOR_NONE,
    /**< None. */
    MMPM_CLK_ID_ROTATOR,
    /**< Rotator clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable. */
    MMPM_CLK_ID_ROTATOR_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_ROTATOR_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdRotatorType;
    

/**
  Clock ID for TV.
*/
typedef enum {
    MMPM_CLK_ID_TV_NONE,
    /**< None. */ 
    MMPM_CLK_ID_TV_ENC,
    /**< ENC TV clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and 
    enable. */ 
    MMPM_CLK_ID_TV_DAC,
    /**< DAC TV clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable. */
    MMPM_CLK_ID_TV_MDP,
    /**< MDP TV clock; set by either MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable, or by MMPM_RSC_ID_CORE_CLK_DOMAIN. */
    MMPM_CLK_ID_TV_HDMI_APP,
    /**< HDMI APP TV clock; set by MMPM_RSC_ID_CORE_CLK: enable only. */ 
    MMPM_CLK_ID_TV_HDMI,
    /**< HDMI TV clock; set by MMPM_RSC_ID_CORE_CLK_DOMAIN: set frequency and 
         enable. */
    MMPM_CLK_ID_TV_RGB,
    /**< RGB TV clock; set by MMPM_RSC_ID_CORE_CLK_DOMAIN: enable only. */
    MMPM_CLK_ID_TV_NPL,
    /**< NPL TV clock; set by MMPM_RSC_ID_CORE_CLK_DOMAIN: enable only. */
    MMPM_CLK_ID_TV_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_TV_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdTvType;


/**
  Clock ID for DSI.
*/
typedef enum {
    MMPM_CLK_ID_DSI_NONE,
    /**< None. */  
    MMPM_CLK_ID_DSI,
    /**< DSI clock; set by MMPM_RSC_ID_CORE_CLK_DOMAIN. */  
    MMPM_CLK_ID_DSI_ESC,
    /**< DSI Esc clock; set by either MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable, or by MMPM_RSC_ID_CORE_CLK_DOMAIN. */
    MMPM_CLK_ID_DSI_PIX,
    /**< DSI pixel clock; set by MMPM_RSC_ID_CORE_CLK_DOMAIN: set frequency 
         and enable. */
    MMPM_CLK_ID_DSI_BYTE,
    /**< DSI byte clock; set by MMPM_RSC_ID_CORE_CLK_DOMAIN: set frequency 
         and enable. */
    MMPM_CLK_ID_DSI_LVDS,
    /**< DSI LVDS clock; set by MMPM_RSC_ID_CORE_CLK_DOMAIN: enable only. */
    MMPM_CLK_ID_DSI_MDP_P2,
    /**< DSI MDP P2 clock; set by MMPM_RSC_ID_CORE_CLK_DOMAIN: enable only. */
    MMPM_CLK_ID_DSI_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_DSI_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdDsiType;


/**
  Clock ID for audio interface.
*/
typedef enum {
    MMPM_CLK_ID_AUDIOIF_NONE,
    /**< None. */ 			
    MMPM_CLK_ID_AUDIOIF_PCM,
    /**< Audio PCM clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and 
         enable. */
    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_SPKR_MCLK,
    /**< Audio I2S codec speaker master clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_SPKR_SCLK,
    /**< Audio I2S codec speaker slave clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_SPKR_MCLK,
    /**< Audio I2S spare speaker master clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_SPKR_SCLK,
    /**< Audio I2S spare speaker slave clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_MIC_MCLK,
    /**< Audio I2S codec mic master clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_I2S_CODEC_MIC_SCLK,
    /**< Audio I2S codec mic slave clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_MIC_MCLK,
    /**< Audio I2S spare mic master clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_I2S_SPARE_MIC_SCLK,
    /**< Audio I2S spare mic slave clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_MCLK,
    /**< Audio MI2S codec master clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_SCLK,
    /**< Audio MI2S codec slave clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_TX_MCLK,
    /**< Audio MI2S codec Tx master clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_MI2S_CODEC_TX_SCLK,
    /**< Audio MI2S codec Tx slave clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_LPASS_SB_REF_CLK,
    /**< Audio LPASS SLIMBus reference clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_SPS_SB_REF_CLK,
    /**< Audio SPS SLIMBus reference clock; set by MMPM_RSC_ID_CORE_CLK: 
         set frequency and enable. */
    MMPM_CLK_ID_AUDIOIF_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_AUDIOIF_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. @newpage */
} MmpmClkIdAudioIfType;


/**
  Clock ID for LPASS aDSP. Only core clocks are included. 
  The HCLK for each block is controlled by either register programming or 
  generic bandwidth requests. 
*/
typedef enum {
    MMPM_CLK_ID_LPASS_NONE = 0,
    /**<  None. */
    MMPM_CLK_ID_LPASS_HWRSP_CORE,
    /**<  Hardware resampler core clock. */
    MMPM_CLK_ID_LPASS_MIDI_CORE,
    /**<  MIDI core clock. */
    MMPM_CLK_ID_LPASS_AVSYNC_XO,
    /**<  AVT/ProntoSync XO clock. */
    MMPM_CLK_ID_LPASS_AVSYNC_BT,
    /**<  AVT/ProntoSync BT XO clock. */
    MMPM_CLK_ID_LPASS_AVSYNC_FM,
    /**<  AVT/ProntoSync FM XO clock. */
    MMPM_CLK_ID_LPASS_SLIMBUS_CORE,
    /**<  SLIMBus core clock. */
	MMPM_CLK_ID_LPASS_AVTIMER_CORE,
	/**< AVTIMER core clock */
	MMPM_CLK_ID_LPASS_ATIME_CORE,
	/**< ATIME core clock */
/* the following clkid only for getinfo api.*/
	MMPM_CLK_ID_LPASS_ADSP_CORE,
    /**<  aDSP core clock; only applicable for MMPM_GetInfo(). */
	MMPM_CLK_ID_LPASS_AHB_ROOT,
    /**<  AHB root clock. */
    MMPM_CLK_ID_LPASS_ENUM_MAX,
    /**<  Maximum count. */
    MMPM_CLK_ID_LPASS_FORCE32BITS = 0x7fffffff
    /**<  Forces the enumeration to 32 bits. */
} MmpmClkIdLpassType;

/**
  Clock ID for VCAP.
*/
typedef enum {
    MMPM_CLK_ID_VCAP_NONE,
    /**< None. */
    MMPM_CLK_ID_VCAP,
    /**< VCAP clock; set by MMPM_RSC_ID_CORE_CLK: set frequency and enable. */
    MMPM_CLK_ID_VCAP_NPL,
    /**< VCAP_NPL clock; set by MMPM_RSC_ID_CORE_CLK: enable only. */
    MMPM_CLK_ID_VCAP_MAX,
    /**< Maximum count. */
    MMPM_CLK_ID_VCAP_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkIdVcapType;

/**
 Clock ID for the core. 
*/
typedef union {
/** @cond apps_only */
    MmpmClkId2dGrpType          clkId2dGrp;   /**< 2D graphics. */
    MmpmClkId3dGrpType          clkId3dGrp;   /**< 3D graphics. */
    MmpmClkIdMdpType            clkIdMdp;     /**< Mobile Display Processor 
                                                   (MDP). */
    MmpmClkIdVcodecType         clkIdVcodec;  /**< Video codec. */
    MmpmClkIdVpeType            clkIdVpe;     /**< Video Preprocessing Engine 
                                                   (VPE). */
    MmpmClkIdVfeType            clkIdVfe;     /**< Video Front End (VFE). */
    MmpmClkIdCsiType            clkIdCsi;     /**< Camera Serial Interface 
                                                   (CSI). */
    MmpmClkIdSensorType         clkIdSensor;  /**< Sensor. */
    MmpmClkIdJpegdType          clkIdJpegd;   /**< JPEG decoder. */
    MmpmClkIdJpegeType          clkIdJpege;   /**< JPEG encoder. */
    MmpmClkIdRotatorType        clkIdRotator; /**< Rotator. */
    MmpmClkIdTvType             clkIdTv;      /**< TV. */
    MmpmClkIdDsiType            clkIdDsi;     /**< Display Serial Interface 
                                                   (DSI). */
    MmpmClkIdAudioIfType        clkIdAudioIf; /**< Audio interface. */
    MmpmClkIdVcapType           clkIdVcap;    /**< Video Capture and Processing 
                                                   (VCAP). */
/** @endcond */
    MmpmClkIdLpassType          clkIdLpass;   /**< Low Power Audio Subsystem 
                                                   (LPASS). */
} MmpmCoreClkIdType;


/**
  Frequency clock match.
*/
typedef enum {
    MMPM_FREQ_AT_LEAST,  /**< At least. */
    MMPM_FREQ_AT_MOST,   /**< At most. */
    MMPM_FREQ_CLOSEST,   /**< Closest. */
    MMPM_FREQ_EXACT,     /**< Exact. */
    MMPM_FREQ_MAX,       /**< Maximum count. */
    MMPM_FREQ_FORCE32BITS = 0x7FFFFFFF
                         /**< Forces the enumeration to 32 bits. @newpage */
} MmpmFreqMatchType;


/**
  Clock setting for one clock ID. 
*/
typedef struct {
    MmpmCoreClkIdType           clkId;
    /**< Core clock ID defined by enumerations of MmpmClkIdXyzType, where 
         Xyz is the core name. */

    uint32                      clkFreqHz;
    /**< Clock frequency is in units of Hz. */ 
    MmpmFreqMatchType           freqMatch;
    /**< Frequency match: at least, at most, closest, or exact. */
} MmpmClkValType;


/**
  Clock value array that contains clock frequencies for each corresponding 
  clock ID.
*/
typedef struct {
    uint32                      numOfClk;
    /**< Number of clock settings in the array. */  
    MmpmClkValType              *pClkArray;
    /**< Array of clock settings. @newpagetable */
} MmpmClkReqType;


/******** rscId = MMPM_RSC_ID_CORE_CLK_DOMAIN ********/
/**
  Clock ID for HDMI.
*/

/** @cond apps_only */
typedef enum {
    MMPM_CLK_DOMAIN_SRC_ID_HDMI_NONE,
    /**< None. */
    MMPM_CLK_DOMAIN_SRC_ID_HDMI0,
    /**< HDMI0 clock. */
    MMPM_CLK_DOMAIN_SRC_ID_HDMI_MAX,
    /**< Maximum count. */
    MMPM_CLK_DOMAIN_SRC_ID_HDMI_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkDomainSrcIdHdmiType;


/**
  Clock ID for DSI.
*/
typedef enum {
    MMPM_CLK_DOMAIN_SRC_ID_DSI_NONE,
    /**< None. */
    MMPM_CLK_DOMAIN_SRC_ID_DSI0,
    /**< DSI0 clock. */
    MMPM_CLK_DOMAIN_SRC_ID_DSI1,
    /**< DSI1 clock. */
    MMPM_CLK_DOMAIN_SRC_ID_LVDS,
    /**< LVDS clock. */
    MMPM_CLK_DOMAIN_SRC_ID_DSI_MAX,
    /**< Maximum count. */
    MMPM_CLK_DOMAIN_SRC_ID_DSI_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkDomainSrcIdDsiType;
/** @endcond */
/**
  Clock ID for LPASS.
*/
typedef enum {
    MMPM_CLK_DOMAIN_SRC_ID_LPASS_NONE,
    /**< None. */
    MMPM_CLK_DOMAIN_SRC_ID_Q6PLL,
    /**< AVSYNC clock. */
    MMPM_CLK_DOMAIN_SRC_ID_AUDIOPLL,
    /**< AVTIMER clock. */
    MMPM_CLK_DOMAIN_SRC_ID_PRIUSPLL,
    /**< ATIME clock. */
    MMPM_CLK_DOMAIN_SRC_ID_LPASS_MAX,
    /**< Maximum count. */
    MMPM_CLK_DOMAIN_SRC_ID_LPASS_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmClkDomainSrcIdLpassType;


/**
  Clock ID for all cores.
*/
typedef union {
/** @cond apps_only */
    MmpmClkDomainSrcIdHdmiType  clkDomainSrcIdHdmi;
    /**< Clock domain source IDs of HDMI. */
    MmpmClkDomainSrcIdDsiType   clkDomainSrcIdDsi;
    /**< Clock domain source IDs of DSI. */
/** @endcond */
	MmpmClkDomainSrcIdLpassType clkDomainSrcIdLpass;
	/**< Clock domain source IDs of LPASS	@newpagetable */
} MmpmClkDomainSrcIdType;

/**
  Clock domain setting for one clock ID. 
*/
typedef struct {
    MmpmCoreClkIdType           clkId;
    /**< Core clock ID defined by enumerations of MmpmClkIdXyzType, where 
         Xyz is the core name. */

    uint32                      M_val;
    /**< M value. */
    uint32                      N_val;
    /**< N value. */
	uint32                      n2D_val;            
	/**< 2D value */ 
    uint32                      div_val;            
	/**< Pre Divider value */
    uint32                      clkFreqHz; 			 
	/**< Unit of clock Freq is Hz. This is the frequency of clkId that gets configured using mnd */ 
    MmpmClkDomainSrcIdType      clkDomainSrc;
    /**< Clock domain source ID. */
} MmpmClkDomainType;

/**
  Clock domain array that contains clock domain settings for each 
  corresponding clock ID.
*/
typedef struct {
    uint32                      numOfClk;
    /**< Number of clock settings in the array. */
    MmpmClkDomainType           *pClkDomainArray;
    /**< Array of clock domain settings. */
} MmpmClkDomainReqType;
/** @endcond */


/******** rscId = MMPM_RSC_ID_THERMAL ********/
/**
  Thermal mitigation request. 
*/
typedef enum {
    MMPM_THERMAL_NONE,      /**< None. */
    MMPM_THERMAL_LOW,       /**< Low temperature condition. */
    MMPM_THERMAL_NORM,      /**< Normal temperature condition. No thermal 
                                 mitigation is needed. */
    MMPM_THERMAL_HIGH_L1,   /**< High temperature condition level 1. */
    MMPM_THERMAL_HIGH_L2,   /**< High temperature condition level 2. */
    MMPM_THERMAL_HIGH_L3,   /**< High temperature condition level 3. */
    MMPM_THERMAL_HIGH_L4,   /**< High temperature condition level 4. */
    MMPM_THERMAL_HIGH_L5,   /**< High temperature condition level 5. */
    MMPM_THERMAL_HIGH_L6,   /**< High temperature condition level 6. */
    MMPM_THERMAL_HIGH_L7,   /**< High temperature condition level 7. */
    MMPM_THERMAL_HIGH_L8,   /**< High temperature condition level 8. */
    MMPM_THERMAL_HIGH_L9,   /**< High temperature condition level 9. */
    MMPM_THERMAL_HIGH_L10,  /**< High temperature condition level 10. */
    MMPM_THERMAL_HIGH_L11,  /**< High temperature condition level 11. */
    MMPM_THERMAL_HIGH_L12,  /**< High temperature condition level 12. */
    MMPM_THERMAL_HIGH_L13,  /**< High temperature condition level 13. */
    MMPM_THERMAL_HIGH_L14,  /**< High temperature condition level 14. */
    MMPM_THERMAL_HIGH_L15,  /**< High temperature condition level 15. */
    MMPM_THERMAL_HIGH_L16,  /**< High temperature condition level 16. */
    MMPM_THERMAL_MAX,       /**< Maximum count. */
    MMPM_THERMAL_FORCE32BITS = 0x7FFFFFFF
                            /**< Forces the enumeration to 32 bits. */
} MmpmThermalType;



/******** rscId = MMPM_RSC_ID_MEM_BW, MMPM_RSC_ID_GENERIC_BW ********/
/**
  Bandwidth usage for 2D graphics.
*/
typedef enum {
    MMPM_BW_USAGE_2D_GRP_NONE,  /**< None. */
    MMPM_BW_USAGE_2D_GRP,       /**< 2D graphics bandwidth usage. */
    MMPM_BW_USAGE_2D_GRP_MAX,   /**< Maximum count. */
    MMPM_BW_USAGE_2D_GRP_FORCE32BITS = 0x7FFFFFFF
                                /**< Forces the enumeration to 32 bits. */
} MmpmBwUsage2dGrpType;


/**
  Bandwidth usage for 3D graphics.
*/
typedef enum {
    MMPM_BW_USAGE_3D_GRP_NONE,  /**< None. */
    MMPM_BW_USAGE_3D_GRP,       /**< 3D graphics bandwidth usage. */
    MMPM_BW_USAGE_3D_GRP_MAX,   /**< Maximum count. */
    MMPM_BW_USAGE_3D_GRP_FORCE32BITS = 0x7FFFFFFF
                                /**< Forces the enumeration to 32 bits. */
} MmpmBwUsage3dGrpType;


/**
  Bandwidth usage for MDP.
*/
typedef enum {
    MMPM_BW_USAGE_MDP_NONE,  /**< None. */
    MMPM_BW_USAGE_MDP,       /**< MDP bandwidth usage. */
    MMPM_BW_USAGE_MDP_HRES,  /**< MDP bandwidth usage for high resolution. */
    MMPM_BW_USAGE_MDP_MAX,   /**< Maximum count. */
    MMPM_BW_USAGE_MDP_FORCE32BITS = 0x7FFFFFFF
                             /**< Forces the enumeration to 32 bits. */
} MmpmBwUsageMdpType;


/**
  Bandwidth usage for video codec.
*/
typedef enum {
    MMPM_BW_USAGE_VCODEC_NONE,
    /**< None. */
    MMPM_BW_USAGE_VCODEC_ENC,
    /**< Video codec bandwidth usage when doing encoding. */
    MMPM_BW_USAGE_VCODEC_DEC,
    /**< Video codec bandwidth usage when doing decoding. */
    MMPM_BW_USAGE_VCODEC_ENC_DEC,
    /**< Video codec bandwidth usage when doing both encoding and decoding. */
    MMPM_BW_USAGE_VCODEC_MAX,
    /**< Maximum count. */
    MMPM_BW_USAGE_VCODEC_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmBwUsageVcodecType;


/**
  Bandwidth usage for VPE.
*/
typedef enum {
    MMPM_BW_USAGE_VPE_NONE,  /**< None. */
    MMPM_BW_USAGE_VPE,       /**< VPE bandwidth usage. */
    MMPM_BW_USAGE_VPE_MAX,   /**< Maximum count. */
    MMPM_BW_USAGE_VPE_FORCE32BITS = 0x7FFFFFFF
                             /**< Forces the enumeration to 32 bits. */
} MmpmBwUsageVpeType;


/**
  Bandwidth usage for VFE.
*/
typedef enum {
    MMPM_BW_USAGE_VFE_NONE,  /**< None. */
    MMPM_BW_USAGE_VFE,       /**< VFE bandwidth usage. */
    MMPM_BW_USAGE_VFE_MAX,   /**< Maximum count. */
    MMPM_BW_USAGE_VFE_FORCE32BITS = 0x7FFFFFFF
                             /**< Forces the enumeration to 32 bits. */
} MmpmBwUsageVfeType;


/**
  Bandwidth usage for JPEG decoder.
*/
typedef enum {
    MMPM_BW_USAGE_JPEGD_NONE,  /**< None. */
    MMPM_BW_USAGE_JPEGD,       /**< JPEG decoder bandwidth usage. */
    MMPM_BW_USAGE_JPEGD_MAX,   /**< Maximum count. */
    MMPM_BW_USAGE_JPEGD_FORCE32BITS = 0x7FFFFFFF
                               /**< Forces the enumeration to 32 bits. */
} MmpmBwUsageJpegdType;


/**
  Bandwidth usage for JPEG encoder.
*/
typedef enum {
    MMPM_BW_USAGE_JPEGE_NONE,  /**< None. */
    MMPM_BW_USAGE_JPEGE,       /**< JPEG encoder bandwidth usage. */
    MMPM_BW_USAGE_JPEGE_MAX,   /**< Maximum count. */
    MMPM_BW_USAGE_JPEGE_FORCE32BITS = 0x7FFFFFFF
                               /**< Forces the enumeration to 32 bits. @newpage */
} MmpmBwUsageJpegeType;


/**
  Bandwidth usage for rotator.
*/
typedef enum {
    MMPM_BW_USAGE_ROTATOR_NONE,  /**< None. */
    MMPM_BW_USAGE_ROTATOR,       /**< Rotator bandwidth usage. */
    MMPM_BW_USAGE_ROTATOR_MAX,   /**< Maximum count. */
    MMPM_BW_USAGE_ROTATOR_FORCE32BITS = 0x7FFFFFFF
                                 /**< Forces the enumeration to 32 bits. */
} MmpmBwUsageRotatorType;


/**
  Bandwidth usage for LPASS.
*/
typedef enum {
    MMPM_BW_USAGE_LPASS_NONE,      /**< None. */
    MMPM_BW_USAGE_LPASS_DSP,       /**< Bandwidth usage by aDSP. */
    MMPM_BW_USAGE_LPASS_DMA,       /**< Bandwidth usage by DMA/BAM. */
    MMPM_BW_USAGE_LPASS_EXT_CPU,   /**< Bandwidth usage by CPU outside 
                                        LPASS. */
    MMPM_BW_USAGE_LPASS_ENUM_MAX,  /**< Maximum count. */
    MMPM_BW_USAGE_LPASS_FORCE32BITS = 0x7FFFFFFF
                                   /**< Forces the enumeration to 32 bits. */
} MmpmBwUsageLpassType;


/**
  Bandwidth usage for the core.
*/
typedef union {
/** @cond apps_only */
    MmpmBwUsage2dGrpType        bwUsage2dGrp;    /**< 2D graphics. */ 
    MmpmBwUsage3dGrpType        bwUsage3dGrp;    /**< 3D graphics. */
    MmpmBwUsageMdpType          bwUsageMdp;      /**< MDP. */
    MmpmBwUsageVcodecType       bwUsageVcodec;   /**< Video codec. */
    MmpmBwUsageVpeType          bwUsageVpe;      /**< VPE. */
    MmpmBwUsageVfeType          bwUsageVfe;      /**< VFE. */
    MmpmBwUsageJpegdType        bwUsageJpegd;    /**< CSI. */
    MmpmBwUsageJpegeType        bwUsageJpege;    /**< Sensor. */
    MmpmBwUsageRotatorType      bwUsageRotator;  /**< JPEG decoder. */
/** @endcond  */
    MmpmBwUsageLpassType        bwUsageLpass;    /**< LPASS. */
} MmpmCoreBwUsageType;

/**
  Generic bandwidth port ID.
*/
typedef enum {
    MMPM_BW_PORT_ID_NONE = 0,        /**< None. */
    MMPM_BW_PORT_ID_ADSP_MASTER,     /**< aDSP master. */
    MMPM_BW_PORT_ID_DML_MASTER,      /**< Data Mover Lite master. */
    MMPM_BW_PORT_ID_AIF_MASTER,      /**< Audio interface master. */
    MMPM_BW_PORT_ID_SLIMBUS_MASTER,  /**< SLIMBus master. */
    MMPM_BW_PORT_ID_MIDI_MASTER,     /**< MIDI master. */
    MMPM_BW_PORT_ID_HWRSMP_MASTER,   /**< Hardware Resampler master. */
    MMPM_BW_PORT_ID_EXT_AHB_MASTER,  /**< External AHB master. This is the 
                                          SNOC port to AHB. */
    MMPM_BW_PORT_ID_SPDIF_MASTER,    /**< SPDIF master */
    MMPM_BW_PORT_ID_HDMIRX_MASTER,    /**< HDMI Slave */
    MMPM_BW_PORT_ID_SIF_MASTER,       /**< SIF Slave */
 
    MMPM_BW_PORT_ID_DML_SLAVE,       /**< Data Mover Lite slave. */
    MMPM_BW_PORT_ID_AIF_SLAVE,       /**< Audio interface slave. */
    MMPM_BW_PORT_ID_SLIMBUS_SLAVE,   /**< SLIMBus slave. */
    MMPM_BW_PORT_ID_MIDI_SLAVE,      /**< MIDI slave. */
    MMPM_BW_PORT_ID_HWRSMP_SLAVE,    /**< Hardware Resampler slave. */
    MMPM_BW_PORT_ID_AVSYNC_SLAVE,    /**< AVT/ProntoSync slave. */
    MMPM_BW_PORT_ID_LPM_SLAVE,       /**< Low-power memory slave. */
    MMPM_BW_PORT_ID_SRAM_SLAVE,      /**< SRAM slave. */
    MMPM_BW_PORT_ID_EXT_AHB_SLAVE,   /**< Extended AHB slave. */
    MMPM_BW_PORT_ID_DDR_SLAVE,       /**< DDR slave. */
    MMPM_BW_PORT_ID_OCMEM_SLAVE,     /**< On-chip memory slave. */
    MMPM_BW_PORT_ID_PERIFNOC_SLAVE,  /**< PERIFNOC slave. @newpage */
	MMPM_BWP_PORT_ID_DCODEC_SLAVE,   /**< DCodec Slave */
    MMPM_BW_PORT_ID_SPDIF_SLAVE,     /**< SPDIF slave*/
    MMPM_BW_PORT_ID_HDMIRX_SLAVE,    /**< HDMI Slave */
    MMPM_BW_PORT_ID_SIF_SLAVE,       /**< SIF Slave */
    MMPM_BW_PORT_ID_BSTC_SLAVE,      /**< BSTC Slave*/
    MMPM_BW_PORT_ID_CORE,            /**< Core for which the current client is 
                                          registered. This must be specified 
                                          in the srcPortId for core-to-memory 
                                          transfers. */
    MMPM_BW_PORT_ID_MAX,             /**< Maximum count. */
    MMPM_BW_PORT_ID_FORCE32BITS = 0x7FFFFFFF
                                    /**< Forces the enumeration to 32 bits. */
} MmpmBwPortIdType;

/** @cond apps_only */
/**
  Bandwidth value structure that includes information on source-destination, 
  bandwidth value, and bandwidth characteristics.
*/
typedef struct {
    /* master-slave */
    uint32                      memPhyAddr;
    /**< Physical memory address. */
    uint32                      axiPort;
    /**< AXI port index. */
    
    /* bw */
    uint32                      bwBytePerSec;
    /**< Bytes per second. */

    /* bw characteristics */
    uint32                      usagePercentage;
    /**< Utilization percentage. */
    MmpmCoreBwUsageType         bwUsageType;
    /**< Core's bandwidth usage type defined by enumerations of 
         MmpmBwUsageXyzType, where Xyz is the core name. */
} MmpmBwValType;

/**
  Bandwidth value array that contains bandwidth settings for each 
  corresponding memory ID.
*/
typedef struct {
    uint32                      numOfBw;
    /**< Number of bandwidth settings in the array; maximum: 16 per client. */
    MmpmBwValType               *pBandWidthArray;
    /**< Array of bandwidth settings. @newpagetable */
} MmpmBwReqType;
/** @endcond */


/* general BW definition start here */
/** 
  Bus route type (master/slave pair).
*/
typedef struct{
    MmpmBwPortIdType masterPort;  /**< Master port. */
    MmpmBwPortIdType slavePort;   /**< Slave port. */
} MmpmBusRouteType;

/**
  Data type that holds the bandwidth value for type 3 requests (Ab/Ib).
*/
typedef struct{
    uint64 Ab;  /**< Average bandwidth in bytes per second. */
    uint64 Ib;  /**< Instantaneous bandwidth in bytes per second. */
} MmpmBusBWDataIbAbType;

/**
  Data type that holds the bandwidth value for type 1 requests 
  (bandwidth/usage).
*/
typedef struct{
    uint64 bwBytePerSec;
    /**< Bandwidth value in bytes per second. */
    uint32 usagePercentage;
    /**< Utilization percentage. */
    MmpmBwUsageLpassType usageType;
    /**< Bandwidth usage type. */
} MmpmBusBWDataUsageType;

/** @cond apps_only */
/**
  Bandwidth request for Ab/Ib.
*/
typedef union{
    MmpmBusBWDataIbAbType busBwAbIb;
    /**< Bus bandwidth for Ab/Ib. */
    MmpmBusBWDataUsageType busBwValue;
    /**< Bus bandwidth for bandwidth/usage. */
} MmpmBusBWDataType;
/** @endcond */

/**
  Data type that specifies the bandwidth request parameters.
*/
typedef struct{
    MmpmBusRouteType busRoute;
    /**< Master/slave pair for which the bandwidth is requested. */
    MmpmBusBWDataType bwValue;
    /**< Bandwidth value. */
} MmpmGenBwValType;

/**
  Generic bandwidth value array that contains bandwidth settings for each 
  corresponding master/slave pair.
*/
typedef struct {
    uint32                         numOfBw;
    /**< Number of bandwidth settings in the array; maximum: 16 per client. */
    MmpmGenBwValType               *pBandWidthArray;
    /**< Array of bandwidth settings. */
} MmpmGenBwReqType;

/******** rscId = MMPM_RSC_ID_MEM_POWER ********/
/**
  Memory power state.
*/
typedef enum {
    MMPM_MEM_POWER_NONE,       /**< None. */
    MMPM_MEM_POWER_OFF,        /**< Memory is off. */
    MMPM_MEM_POWER_RETENTION,  /**< Memory is in retention. */
    MMPM_MEM_POWER_ACTIVE,     /**< Memory is in the Active state. */
    MMPM_MEM_POWER_MAX,        /**< Maximum count. */
    MMPM_MEM_POWER_FORCE32BITS = 0x7FFFFFFF
                               /**< Forces the enumeration to 32 bits. */
} MmpmMemPowerStateType;

/**
  Memory ID definition for memory power state requests.
*/
typedef enum{
    MMPM_MEM_NONE,       /**< None. */
    MMPM_MEM_LPASS_LPM,  /**< Low-power memory. */
    MMPM_MEM_SRAM,       /**< Sensors RAM. */
    MMPM_MEM_MAX,        /**< Maximum count. */
    MMPM_MEM_FORCE32BITS = 0x7FFFFFFF
                        /**< Forces the enumeration to 32 bits. */
} MmpmMemIdType;

/**
  Memory power state request parameter.
*/
typedef struct{
    MmpmMemIdType memory;              /**< Memory. */
    MmpmMemPowerStateType powerState;  /**< Power state. @newpagetable */
}MmpmMemPowerReqParamType;

/** @cond apps_only */
/**
 PMIC GPIO parameter.
*/
typedef struct {
    uint32                      gpioId;
    /**< Which GPIO to configure. */
    boolean                     configGpio;
    /**< Whether to configure GPIO. */
    uint32                      gpioVoltageSource;
    /**< Which voltage source. */
    boolean                     gpioModeOnOff;
    /**< Enables/disables the mode selection. */
    uint32                      gpioModeSelect;
    /**< Mode select type. */
    uint32                      gpioOutBufferConfig;
    /**< Output buffer configuration. */
    boolean                     invertExtPin;
    /**< Invert external pin. */
    uint32                      gpioCurrentSourcePulls;
    /**< Current source pull configuration. */
    uint32                      gpioOutBufferDriveStrength;
    /**< Output buffer drive strength. */
    uint32                      gpioDtestBufferOnOff;
    /**< Dtest buffer configuration. */
    uint32                      gpioExtPinConfig;
    /**< External pin configuration. */
    uint32                      gpioSourceConfig;
    /**< GPIO source configuration. */
    boolean                     interrupPolarity;
    /**< Interrupt polarity. */
    uint32                      uartPath;
    /**< UART path. */
} MmpmPmicGpioParamType;
/** @endcond */

/**
 PMIC GPIO configuration.
*/
typedef enum {
     MMPM_PMIC_CONFIG,
     /**< General configuration. */
     MMPM_PMIC_CONFIG_BIAS_VOLTAGE,
     /**< Bias voltage. */
     MMPM_PMIC_CONFIG_DIGITAL_INPUT,
     /**< Digital input. */
     MMPM_PMIC_CONFIG_DIGITAL_OUTPUT,
     /**< Digital output. */
     MMPM_PMIC_CONFIG_SET_VOLTAGE_SOURCE,
     /**< Set the voltage source. */
     MMPM_PMIC_CONFIG_MODE_SELECTION,
     /**< Mode selection. */
     MMPM_PMIC_CONFIG_SET_OUTPUT_BUFFER,
     /**< Set the output buffer configuration. */
     MMPM_PMIC_CONFIG_SET_INVERSION,
     /**< Set the inversion configuration. */
     MMPM_PMIC_CONFIG_SET_CURRENT_SOURCE_PULLS,
     /**< Set the current source pulls. */
     MMPM_PMIC_CONFIG_SET_EXT_PIN,
     /**< Set the external pin configuration. */ 
     MMPM_PMIC_CONFIG_SET_OUTPUT_BUF_DRIVE_STRG,
     /**< Set the output buffer drive strength. */
     MMPM_PMIC_CONFIG_SET_SOURCE,
     /**< Set the source configuration. */
     MMPM_PMIC_CONFIG_SET_INT_POLARITY,
     /**< Set the interrupt polarity. */
     MMPM_PMIC_CONFIG_SET_MUX_CTRL,
     /**< Set the MUX control. */
} MmpmPmicGpioConfigType;

/** @cond apps_only */
/**
  The PMIC GPIO value structure includes information on the configuration ID 
  and GPIO parameters. This is a passthrough from the client to the PMIC 
  driver. For each congigId, pass in the corresponding parameters in 
  MmpmPmicGpioParamType. 
*/
/* Refer to PMIC GPIO API header file for details of parameters
   that need to be filled for a configId. */
typedef struct {
    MmpmPmicGpioConfigType      configId;
    /**< PMIC GPIO configuration ID. */
    MmpmPmicGpioParamType       gpioParam;
    /**< PMIC GPIO configuration parameters. */
} MmpmPmicGpioReqType;
/** @endcond */

/**
  Type of reset to be done through the resource ID MMPM_RSC_ID_RESET.
*/
typedef enum {
     MMPM_RESET_NONE,      /**< Reset is ignored. */
     MMPM_RESET_DEASSERT,  /**< Reset with a deassert. */
     MMPM_RESET_ASSERT,    /**< Reset with an assert. */
     MMPM_RESET_PULSE,     /**< Reset with a pulse. */
     MMPM_RESET_ENUM_MAX,  /**< Maximum count. */
     MMPM_RESET_ENUM_FORCE32BITS = 0x7FFFFFFF
                          /**< Forces the enumeration to 32 bits. */
} MmpmResetType;

/** @cond apps_only */
/**
  Parameter for the reset.
*/
typedef struct {
    MmpmCoreClkIdType         clkId;
    /**< Clock ID (clkId) corresponding to a coreId and instanceId. */
    MmpmResetType             resetType;
    /**< Reset type. */
} MmpmResetParamType;

/**
  Request for the reset.
*/
typedef struct {
    uint32                    numOfReset;
    /**< Number of reset requests. */
    MmpmResetParamType        *pResetParamArray;
    /**< Reset parameter. */
} MmpmResetReqType;
/** @endcond */

/**
  Possible operations to perform during an extended MIPS request.
*/
typedef enum {
    MMPM_MIPS_REQUEST_NONE = 0,
    /**< None. */
    MMPM_MIPS_REQUEST_CPU_CLOCK_ONLY,
    /**< MIPS request adjusts only the CPU clock. The bandwidth to cover 
         instruction fetches is requested separately. */
    MMPM_MIPS_REQUEST_CPU_CLOCK_AND_BW,
    /**< MIPS request adjusts CPU clock and bandwidth required to support 
         instruction fetches. The location of the code must be provided in 
         this request.*/
    
    MMPM_MIPS_REQUEST_ENUM_MAX,
    /**< Maximum count. */
    MMPM_MIPS_REQUEST_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmMipsRequestFnType;

/**
  MIPS request structure for an extended MIPS request.
*/
typedef struct {
    uint32                 mipsTotal;
    /**< Total MIPS value. */
    uint32                 mipsPerThread;
    /**< MIPS value per hardware thread. */
    MmpmBwPortIdType       codeLocation;
    /**< Location from where the code is executed. */ //TODO create a new enum 
    MmpmMipsRequestFnType  reqOperation;
    /**< Which operations the MIPS request includes. */
} MmpmMipsReqType;

/******************************************************************************************/
/*                                                                                        */
/*                    MMPM_Release() Data Structure                                   */
/*                                                                                        */
/******************************************************************************************/
/**
  An array of clock IDs for the same core, defined by the same enumeration of 
  MmpmClkIdXyzType, where Xyz is the core name. For example, MmpmClkId2dGrpType.
*/
typedef struct {
    uint32                      numOfClk;
    /**< Number of clocks. */
    MmpmCoreClkIdType           *pClkIdArray;
    /**< Array of clock IDs. @newpagetable */
} MmpmClkIdArrayParamType;


/******************************************************************************************/
/*                                                                                        */
/*                    MMPM_Request(), MMPM_Release(),                             */
/*                    MMPM_Pause(), and MMPM_Resume() Data Structures                      */
/*                                                                                        */
/******************************************************************************************/
/**
  MMPM resource parameter structure definition. 
  
  The parameter maps to different structures depending on the rscId 
  (in MmpmRscParamType) and whether the function is MMPM_Request, 
  MMPM_Release, MMPM_Pause, or MMPM_Resume. The following table summarizes 
  the mapping.
  @vertspace{9}
  @inputtable{MmpmRscParamStructType_parameters.tex}
*/

/* 
  For MMPM_Request(): \n
     -if rscId = MMPM_RSC_ID_POWER,              NULL;        Return MMPM_SUCESS if the GDHS request was sucessfull or if the support is broken in h/w or s/w.
                                                              If it is broken no call is made to clkregime driver to enable the GDHS bit.
                                                              MMPM_UNSUPPORTED will be returned if the h/w block does not have support for GDHS; \n
     -if rscId = MMPM_RSC_ID_VREG,               vregMilliVolt; \n
     -if rscId = MMPM_RSC_ID_REG_PROG,           regProgMatch; \n
     -if rscId = MMPM_RSC_ID_PMIC_GPIO,          pPmicGpio; \n
     -if rscId = MMPM_RSC_ID_CORE_CLK,           pCoreClk; \n
     -if rscId = MMPM_RSC_ID_CORE_CLK_DOMAIN,    pCoreClkDomain; \n
     -if rscId = MMPM_RSC_ID_MEM_BW,             pBwReq; \n
     -if rscId = MMPM_RSC_ID_GENERIC_BW,         pGenBwReq; \n
     -if rscId = MMPM_RSC_ID_AXI_EN,             NULL; .\n
     -if rscId = MMPM_RSC_ID_SLEEP_LATENCY,      sleepMicroSec;   This request should be used by clients who have restrictions on the maximum interrupt 
                                                                  latency that they can endure and still function correctly.A value of 0 is rejected with 
                                                                  a return status MMPM_STATUS_BADPARM.Make sure you only make your latency request when 
                                                                  you are expecting your interrupt to fire. And cancel your request when you are no longer 
                                                                  expecting your interrupt to fire. Otherwise, you will be preventing low power modes by 
                                                                  having a latency restriction in place that is not being used.\n
     -if rscId = MMPM_RSC_ID_MIPS,               mips; \n
     -if rscId = MMPM_RSC_ID_MIPS_EXT,           pMipsExt;
     -if rscId = MMPM_RSC_ID_RESET,              pResetReq; \n \n
     -if rscId = MMPM_RSC_ID_ACTIVE_STATE,       NULL; Request: core is active; \n
     -if rscId = MMPM_RSC_ID_THERMAL,            thermalMitigation; Request for thermal mitigation for the condition described in thermalMitigation; \n
     -if rscId = MMPM_RSC_MEM_POWER,             pMemPowerState; Memory power request data\n

  For MMPM_Release(): \n
     -if rscId = MMPM_RSC_ID_POWER,              NULL;        Return MMPM_SUCESS if the GDHS request was sucessfull or if the support is broken in h/w or s/w.
                                                              If it is broken no call is made to clkregime driver to disable the GDHS bit.
                                                              MMPM_UNSUPPORTED will be returned if the h/w block does not have support for GDHS; \n
     -if rscId = MMPM_RSC_ID_VREG,               NULL; \n
     -if rscId = MMPM_RSC_ID_REG_PROG,           NULL; \n
     -if rscId = MMPM_RSC_ID_CORE_CLK,           pRelClkIdArray; \n
     -if rscId = MMPM_RSC_ID_CORE_CLK_DOMAIN,    pRelClkIdArray; \n
     -if rscId = MMPM_RSC_ID_MEM_BW,             NULL; \n
     -if rscId = MMPM_RSC_ID_GENERIC_BW,         NULL; \n
     -if rscId = MMPM_RSC_ID_AXI_EN,             NULL; \n
     -if rscId = MMPM_RSC_ID_SLEEP_LATENCY,      NULL; \n
     -if rscId = MMPM_RSC_ID_MIPS,               NULL; \n
     -if rscId = MMPM_RSC_ID_MIPS_EXT,           NULL; \n
     -if rscId = MMPM_RSC_ID_ACTIVE_STATE,       NULL; Release: core is idle; \n
     -if rscId = MMPM_RSC_ID_THERMAL,            thermalMitigation; No thermal mitigation is necessary. Equivalent to request with 
                                                                    thermalMitigation =  MMPM_THERMAL_NORM  \n
     -if rscId = MMPM_RSC_MEM_POWER,             NULL;\n


  For MMPM_Pause()/MMPM_Resume(): \n
     -if rscId = MMPM_RSC_ID_CLK,                gateClkId; \n
     -if rscId = MMPM_RSC_ID_CORE_CLK,           gateClkId; \n
     -if rscId = MMPM_RSC_ID_REG_PROG,           NULL; \n Pause/Resumes AHB clks.    
     -if rscId = MMPM_RSC_ID_MEM_BW,             NULL; \n Pause/Resume AXI clks
*/
typedef union {
    /* For MMPM_Request() */
    uint32                      vregMilliVolt;
    /**< For rscId = MMPM_RSC_ID_VREG in MMPM_Request: \n 
         VREG in millivolts. */
    MmpmRegProgMatchType        regProgMatch;
    /**< For rscId = MMPM_RSC_ID_REG_PROG in MMPM_Request: \n 
         Register programming clock match. */
    MmpmPmicGpioReqType         *pPmicGpio;
    /**< For rscId = MMPM_RSC_ID_PMIC_GPIO in MMPM_Request: \n 
         PMIC GPIO parameters. */
    MmpmClkReqType              *pCoreClk;
    /**< For rscId = MMPM_RSC_ID_CORE_CLK in MMPM_Request: \n 
         Core clock request parameter. */
    MmpmClkDomainReqType        *pCoreClkDomain;
    /**< For rscId = MMPM_RSC_ID_CORE_CLK_DOMAIN in MMPM_Request: \n 
         Core clock domain parameter. */
    MmpmBwReqType               *pBwReq;
    /**< For rscId = MMPM_RSC_ID_MEM_BW in MMPM_Request: \n 
         Bandwidth request parameter. */
    MmpmGenBwReqType            *pGenBwReq;
    /**< For rscId = MMPM_RSC_ID_GENERIC_BW in MMPM_Request: \n 
         Bandwidth request parameter. */
    uint32                      sleepMicroSec;
    /**< For rscId = MMPM_RSC_ID_SLEEP_LATENCY in MMPM_Request: \n 
         Sleep latency in microseconds. */
    uint32                      mips;
    /**< For rscId = MMPM_RSC_ID_MIPS in MMPM_Request: \n 
         MIPS. MIPS = 0 is treated as an impulse request. No release is 
         expected for an impulse request. */
    MmpmResetReqType            *pResetReq;
    /**< For rscId = MMPM_RSC_ID_RESET in MMPM_Request: \n 
         Reset parameters. */
    MmpmMipsReqType             *pMipsExt;
    /**< For rscId = MMPM_RSC_ID_MIPS_EXT in MMPM_Request: \n 
         MIPS, request type, and code location. */
    MmpmThermalType             thermalMitigation;
    /**< For rscId = MMPM_RSC_ID_THERMAL in MMPM_Request: \n 
         Thermal mitigation requested. */
    MmpmMemPowerReqParamType    *pMemPowerState;
    /**< For rscId = MMPM_RSC_ID_MEM_POWER in MMPM_Request: \n 
         Memory power state request data. */
    
    /* For MMPM_Release(). */
    MmpmClkIdArrayParamType     *pRelClkIdArray;
    /**< For rscId = MMPM_RSC_ID_CORE_CLK and MMPM_RSC_ID_CORE_CLK_DOMAIN 
         in MMPM_Release: \n
         Clock ID array. */
    
    /* For MMPM_Pause()/MMPM_Resume(). */
    MmpmCoreClkIdType           gateClkId;
    /**< In MMPM_Pause or MMPM_Resume: \n
         Clock ID, defined by enumerations of MmpmClkIdXyzType, where Xyz is 
         the core name. For example, MmpmClkId2dGrpType. @newpagetable */
}MmpmRscParamStructType;


/**
  Resource parameter for MMPM_Request(), MMPM_Release(), MMPM_Pause(), and 
  MMPM_Resume().
*/
typedef struct {
    MmpmRscIdType               rscId;
    /**< Indicates which resource to request, release, pause, or resume. */
    MmpmRscParamStructType      rscParam;
    /**< Parameter data structure matching to rscId. */
} MmpmRscParamType;  


/******************************************************************************************/
/*                                                                                        */
/*                    MMPM_Request_Ext()/MMPM_Release_Ext()/                              */
/*                    MMPM_Pause_Ext()/MMPM_Resume_Ext() Data Structures                  */
/*                                                                                        */
/******************************************************************************************/

/**
  Parameter for MMPM_Request_Ext() and MMPM_Release_Ext().
*/
typedef enum{
     MMPM_API_TYPE_NONE,      /**< None. */
     MMPM_API_TYPE_SYNC,      /**< Synchronous API call. */
     MMPM_API_TYPE_ASYNC,     /**< Asynchronous API call. */
     MMPM_API_TYPE_ENUM_MAX,  /**< Enumeration maximum. */
     MMPM_API_TYPE_FORCE32BITS = 0x7FFFFFFF
                             /**< Forces the enumeration to 32 bits. */
} MmpmApiType;  

/**
  Resource parameter for MMPM_Request_Ext() and MMPM_Release_Ext().
*/
typedef struct {
    MmpmApiType                 apiType;
    /**< Function call type. */

    uint32                      numOfReq;
    /**< Total number of requests; maximum: 8. */
    MmpmRscParamType            *pReqArray;
    /**< Request array. */
    MMPM_STATUS                 *pStsArray;
    /**< Return status array. */

    uint32                      reqTag;
    /**< Request tag for the asynchronous callback; only used for an 
         asynchronous apiType. */
    void                        *pExt;
    /**< For future expansion of the API. */
} MmpmRscExtParamType;  


/******************************************************************************************/
/*                                                                                        */
/*                          MMPM_GetInfo() Data Structures                                */
/*                                                                                        */
/******************************************************************************************/
/**
  MMPM information ID definition. 
*/
typedef enum {
    MMPM_INFO_ID_NONE,
    /**< None. */
/** @cond apps_only */
    MMPM_INFO_ID_CORE_CLK,
    /**< Core clock frequency. */   
/** @endcond */
    MMPM_INFO_ID_CORE_CLK_MAX,
    /**< Maximum core clock frequency for the core. */   
/** @cond apps_only */
    MMPM_INFO_ID_CORE_CLK_MAX_SVS,
    /**< Maximum core clock frequency for the core's SVS mode. This is zero 
         if the core does not have SVS mode. */   
    MMPM_INFO_ID_CORE_CLK_MAX_NOMINAL,
    /**< Maximum core clock frequency for the core's Nominal mode. This is 
         zero if the core does not have NOMINAL mode. */   
    MMPM_INFO_ID_CORE_CLK_MAX_TURBO,
    /**< Maximum core clock frequency for the core's Turbo mode. This is zero 
         if the core does not have Turbo mode. */
/** @endcond */
    MMPM_INFO_ID_MIPS_MAX,
    /**< Maximum/minimum MIPS for the core; only supported for aDSP. */
    MMPM_INFO_ID_BW_MAX,
    /**< Maximum/minimum bandwidth; only supported for LPASS. */	
/** @cond apps_only */
    MMPM_INFO_ID_CRASH_DUMP,
    /**< Crash dump. */
    MMPM_INFO_ID_POWER_SUPPORT,
    /**< Information on whether a block supports GDHS. */
/** @endcond */
    MMPM_INFO_ID_CLK_FREQ,
    /**< Measures a clock frequency. */
/** @cond apps_only */
    MMPM_INFO_ID_PERFMON,
    /**< Controls the performance monitor. */
    MMPM_INFO_ID_PMIC_GPIO,
    /**< Returns the PMIC GPIO status. */
    MMPM_INFO_ID_SET_DEBUG_LEVEL,
    /**< Sets the debug level for MMPM2. */
    MMPM_INFO_ID_MMSS_BUS,
    /**< MMSS FABRIC bus status. */
    MMPM_INFO_ID_LPASS_BUS,
    /**< LPASS FABRIC bus status. */
/** @endcond */
    MMPM_INFO_ID_MAX,
    /**< Maximum count. */ 
    MMPM_INFO_ID_FORCE32BITS = 0x7FFFFFFF
    /**< Forces the enumeration to 32 bits. */
} MmpmInfoIdType;

/** @cond apps_only */
/**
    Client gets the current clock frequency. \n
    InfoId = MMPM_INFO_ID_CORE_CLK.
*/
typedef struct {
    MmpmCoreClkIdType           clkId;
    /**< Input: core clock IDs. */ /* defined by MmpmClkIdType enums of mmpm2_platformclocks.h */
    uint32                      clkFreqHz;
    /**< Output: clock frequency in Hz. @newpagetable */
} MmpmInfoClkType;
/** @endcond */
/** @} */ /* end_addtogroup mmpm_datatypes */

/** @ingroup mmpm_constants_macros
  Maximum clock name length.
*/
#define MMPM_MAX_CLOCK_NAME 64

/** @ingroup mmpm_datatypes
  Clock frequency information.
*/
typedef struct {
    uint32                      clkId;
    /**< Input: core clock IDs. */ /* defined by MmpmClkIdType enums of mmpm2_platformclocks.h */
    uint32                      clkFreqHz;
    /**< Output: clock frequency in Hz. */
    uint32                      forceMeasure;
    /**< Control to force the clock to be measured. Clock frequency may be read 
         from a cached table if this is not set to 1. @newpagetable */
} MmpmInfoClkFreqType;


/********************************************************************************
*                        MMPM PERFMON                                           *
********************************************************************************/
/** @addtogroup mmpm_constants_macros 
@{ */
/** @name MMPM performance monitor 

   Resource parameters.
@{ */
/** Measure the type read burst. */
#define READ_BURSTS_MMPM        0
/** Measure the type write burst. */
#define WRITE_BURSTS_MMPM       1
/** Measure the type read transfers. */
#define READ_TRANSFERS_MMPM     3
/** Measure the type write transfers. */
#define WRITE_TRANSFERS_MMPM    4
/** Three columns reserved for time stamp sec, ns, and latency fields. */
#define NR_PERFMON2_PARAM       3
/** Maximum number of clocks to measure. */
#define NR_REC_CLOCK            10
/** Maximum number of ports to measure. */
#define NR_PORTS                7
/** Total size of the line to write to the log file. */
#define NR_RECORDED_VALUES2     (NR_PERFMON2_PARAM + NR_REC_CLOCK + NR_PORTS)
/** Maximum filename size, including path. */
#define MAX_PATH_LENGTH         64
/** @} */ /* end_name_group MMPM performance monitor */
/** @} */ /* end_addtogroup mmpm_constants_macros */

/** @addtogroup mmpm_datatypes 
@{ */

/** @cond apps_only */
/**
  MMPM performance monitor information.
*/
typedef struct {
    uint32                      masterMeasureArray[NR_PORTS];
    /**< List of ports to measure. The first port in the list is used as the 
         trigger point. */
    uint32                      measurementConfig[NR_PORTS];
    /**< Type of measurement: read = 3, write = 4, read burst = 0, and 
         write burst = 1. */
    uint32                      latencyMasterPort;
    /**< Master port on which to measure the latency. */
    uint32                      holdoffTime;
    /**< Time to wait before taking the measurement. */
    uint32                      triggerMode;
    /**< Oneshot or continuous.  Oneshot only fills the ulog buffer once and 
         stops. Continuous runs forever until another measurement is started. */
    void                        *pDataMsg;
    /**< For future expansion of the API. */
    char                        pFilename[MAX_PATH_LENGTH];
    /**< Log filename. */
    uint32                      axiClockFreq;
    /**< Specified frequency at which FABRIC will be running during the 
         performance measurement. */
    uint32                      clockInfo[NR_REC_CLOCK];
    /**< Array that contains the clock IDs of all the clocks to be measured. */
    /* clock id values are defined by MmpmClkIdType enums of mmpm2_platformclocks.h. */
    uint32                      isClockFreqCalc[NR_REC_CLOCK];
    /**< Array that contains the parameter to check if the user wants to 
         calculate the clock or get the clock frequency using a clock 
         measurement circuit. @newpagetable */
} MmpmInfoPerfmonType;

/**
  PMIC GPIO status.
*/
typedef struct {
    uint32                      gpioModeSelect;
    /**< Mode selection. */
    uint32                      gpioVoltageSource;
    /**< Voltage source. */
    uint32                      gpioModeOnOff;
    /**< Mode selection is enabled or disabled. */
    uint32                      gpioOutBufferConfig;
    /**< Output buffer configuration. */
    uint32                      gpioOutBufferDriveStrength;
    /**< Output buffer drive strength. */
    uint32                      gpioCurrentSourcePulls;
    /**< Current source pulls. */
    uint32                      gpioSourceConfig;
    /**< GPIO source configuration. */
    uint32                      gpioDtestBufferOnOff;
    /**< Dtest buffer configuration. */
    uint32                      gpioExtPinConfig;
    /**< External pin configuration. */
} MmpmPmicGpioStatusType;

/**
  Client gets the returned PMIC GPIO status. \n
  InfoId = MMPM_INFO_ID_PMIC_GPIO.
*/
typedef struct {
    uint32                      gpioId;  /**< PMIC GPIO to be configured. */
    MmpmPmicGpioStatusType      gpioSts; /**< GPIO status. @newpagetable */
} MmpmPmicGpioInfoType;
/** @endcond */
/** @} */ /* end_addtogroup mmpm_datatypes */


/** @addtogroup mmpm_constants_macros 
@{ */
/** @name Corresponding bus status

    Client gets the corresponding bus status. \n
    InfoId = MMPM_INFO_ID_MMSS_BUS, MMPM_INFO_ID_LPASS_BUS.
@{ */
/** Maximum number of active master/slave pairs. */
#define MAX_ACT_MASTER_SLAVE    16
/** Maximum number of clients for each master/slave pair. */
#define MAX_ACT_CLIENT          16
/** @} */ /* end_name_group Corresponding bus status */
/** @} */ /* end_addtogroup mmpm_constants_macros */

/** @addtogroup mmpm_datatypes 
@{ */
/** @cond apps_only */
/**
  Bandwidth client.
*/
typedef struct {
    uint32                      clientId;
    /**< MMPM client ID. */
    char                        clientName[MAX_CLIENT_NAME];
    /**< MMPM client name. */
    char                        fabClientName[MAX_CLIENT_NAME];
    /**< FABRIC node client name. */
    uint32                      uIbBytePerSec;
    /**< FABRIC node client's total Ib, in bytes per second, requested on a 
         master/slave pair. */
    uint32                      uAbBytePerSec;
    /**< FABRIC node client's total Ab, in bytes per second, requested on a 
         master/slave pair. */
} MmpmBwClientType;

/**
  Master/slave information.
*/
typedef struct {
    uint32                      coreId;
    /**< Master information: core ID. */
    uint32                      instanceId;
    /**< Master information: instance ID. */
    uint32                      axiPort;
    /**< Master information: AXI port. */
    uint32                      slaveId;
    /**< Slave information: slave ID. */
    uint32                      uIbBytePerSec;
    /**< Bandwidth: total Ib, in bytes per second, requested from all FABRIC 
         node clients on this master/slave. */
    uint32                      uAbBytePerSec;
    /**< Bandwidth: total Ab, in bytes per second, requested from all FABRIC 
         node clients on this master/slave. */
    uint32                      numOfClients; 
    /**< Number of bandwidth clients on the same master/slave. */
    MmpmBwClientType            client[MAX_ACT_CLIENT];
    /**< Bandwidth clients' array. @newpagetable */
} MmpmMasterSlaveType;

/**
  FABRIC status information.
*/
typedef struct {
    uint32                      mmFabClkFreq;
    /**< Multimedia Subsystem (MMSS) FABRIC clock frequency. */
    uint32                      appsFabClkFreq;
    /**< Apps FABRIC clock frequency. */
    uint32                      sysFabClkFreq;
    /**< System FABRIC clock frequency. */
    uint32                      lpassFabClkFreq;
    /**< LPASS FABRIC clock frequency. */
    uint32                      ebiClkFreq;
    /**< EBI memory clock frequency. */
    uint32                      mmImemClkFreq;
    /**< MMSS IMEM memory clock frequency. */
    uint32                      numOfMasterSlave;
    /**< Number of master/slave pairs. */
    MmpmMasterSlaveType         masterSlave[MAX_ACT_MASTER_SLAVE];
    /**< Active master/slave array. */
} MmpmFabStatusInfoType;
/** @endcond */

/**
  Information data structure.
*/
typedef union {
/** @cond apps_only */
    MmpmInfoClkType             *pInfoClk;
    /**< Information data structure for InfoId = MMPM_INFO_ID_CORE_CLK and 
         MMPM_INFO_ID_CORE_CLK_MAX_SVS, _MAX_NOMINAL, and _MAX_TURBO. */
/** @endcond */
    boolean                     bInfoPower;
    /**< TRUE if a core has support for GDHS; otherwise, FALSE. */
/** @cond apps_only */
    MmpmInfoPerfmonType         *pInfoPerfmon;
    /**< Information data structure for InfoId = MMPM_INFO_ID_PERFMON. This is 
         used to trigger a memory bandwidth measurement. */
/** @endcond */
    MmpmInfoClkFreqType         *pInfoClkFreqType;
    /**< Information data structure for InfoId = MMPM_INFO_ID_CLK_FREQ. This is 
         used to get the clock frequency or measure the clock frequency. */
/** @cond apps_only */
    MmpmPmicGpioInfoType        *pPmicGpioStatus;
    /**< PMIC GPIO status. */
    uint32                      infoSetLotLevel;
    /**< Sets the MMPM2 debug level; debug level range: 0 to 0xf. */
    MmpmFabStatusInfoType       *pFabStatus;
    /**< FABRIC status. */
/** @endcond */
    uint32                      mipsValue;
    /**< Information data structure for infoId = MMPM_INFO_ID_MIPS_MAX. This 
         is for aDSP get the maximum MIPS. */
    uint64                      bwValue;
    /**< Information data structure for infoId = MMPM_INFO_ID_BW_MAX. This is 
         for LPASS get the maximum bandwidth. */
} MmpmInfoDataStructType;


/**
  MMPM information data structure definition.

  infoId specifies which type of information. Currently, ADSPPM 
  supports infoId = \n
  - MMPM_INFO_ID_CLK_FREQ
  - MMPM_INFO_ID_CORE_CLK_MAX
  - MMPM_INFO_ID_MIPS_MAX
  - MMPM_INFO_ID_BW_MAX
*/
typedef struct {
    MmpmInfoIdType              infoId;
    /**< Information ID. */

    MmpmCoreIdType              coreId;
    /**< Which hardware core. */
    MmpmCoreInstanceIdType      instanceId;
    /**< Which instance of the same hardware core, e.g., the first instance 
         of the two Graphics2D cores. */

    MmpmInfoDataStructType      info;
    /**< Information data structure that contains the output information. 
         @newpagetable */
} MmpmInfoDataType;



/*   MMPM Parameter Configuration  */

/**
  Parameter ID for MMPM_SetParameter().
*/
typedef enum{
    MMPM_PARAM_ID_NONE = 0,
    /**< Invalid parameter. */
/** @cond apps_only */
    MMPM_PARAM_ID_RESOURCE_LIMIT,
    /**< Sets the resource limits (minimum and maximum). This works globally. */
/** @endcond */
    /**< Sets the client's on-chip memory allocation map. */
    MMPM_PARAM_ID_MEMORY_MAP,
    MMPM_PARAM_ID_ENUM_MAX,
    /**< Enumeration maximum. */
    MMPM_PARAM_ID_Force32bit = 0x7fffffff
    /**< Forces the enumeration to 32 bits. @newpage */
} MmpmParameterIdType;


/** 
 *  Memory address mapping parameter.
*/
typedef struct{
    MmpmMemIdType memory;   /**< Memory type identifier. */
    uint64  startAddress;   /**< Virtual address of the start of the memory
                                 range. */
    uint32  size;           /**< Size of the memory range in bytes. */
}MmpmMemoryMapType;


/** 
 * Data structure used by MMPM_SetParameter().  
*/
typedef struct{
    MmpmParameterIdType paramId;
    /**< Parameter ID.
    
         @values See #MmpmParameterIdType */

    void*               pParamConfig;
    /**< Pointer to a parameter-specific structure.
         - If paramId is #MMPM_PARAM_ID_MEMORY_MAP -- pParamConfig points to
         the #MmpmMemoryMapType structure. @tablebulletend */
} MmpmParameterConfigType;

/*======================================================================
   FUNCTION: MMPM_Init
======================================================================*/
/** @ingroup mmpm_func_init
  Initializes all MMPM components and registers all MMPM nodes.

  This function is to be called only once at system runtime. It is typically 
  called by the OS during system boot up.
       
  @param[in] cmd_line String containing one of the following: \n
                      - Initialization command line parameters 
                      - NULL 
                      @tablebulletend
 
  @return
  #MMPM_STATUS_SUCCESS -- On success.

  @dependencies
  %MMPM_Init() has multiple dependencies. Among them are clock driver, NPA, 
  bus driver, and sleep. All the dependencies are handled by the RCInit 
  framework and are not apparent to customers using ADSPPM API.  

@callgraph
@callergraph
*/
uint32 MMPM_Init(char * cmd_line);

/*======================================================================
   FUNCTION: MMPM_Register_Ext
======================================================================*/
/** @ingroup mmpm_func_register_ext
  Registers a new client for a specified hardware block. 

  @datatypes
  MmpmRegParamType

  @param[in] pRegParam Pointer to the %MmpmRegParamType structure.
        
  @detdesc
  This function must be called prior to calls of MMPM_Request_Ext(), 
  MMPM_Release_Ext(), and MMPM_SetParameter() because the returned client ID 
  from %MMPM_Register_Ext() is required as input to these function calls. 
  @par
  The MMPM_GetInfo() call might not require registration with ADSPPM. 
  Whether a client ID is required depends on the type of information 
  requested.

  @return
  Unique client ID -- On success. \n
  0 -- On failure.

  @dependencies
  None.

  @callgraph
  @callergraph
*/
uint32          MMPM_Register_Ext(MmpmRegParamType  *pRegParam);

/*======================================================================
   FUNCTION: MMPM_Deregister_Ext
======================================================================*/
/** @ingroup mmpm_func_deregister_ext
  Deregisters access to the device. 

  All use cases associated with the device are removed and its requested 
  resources are released. 

  @param[in] clientId Registered client ID.
   
  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  None.

  @callgraph
  @callergraph
*/
MMPM_STATUS     MMPM_Deregister_Ext(uint32          clientId);

/*======================================================================
   FUNCTION: MMPM_Request
======================================================================*/
/** @ingroup mmpm_func_request
  Requests resources for a hardware core via a pre-registered client ID. 

  @datatypes
  MmpmRscParamType
  
  @param[in] clientId      Registered client ID.
  @param[in] pMmpmRscParam Pointer to the %MmpmRscParamType structure.

  @detdesc
  Resources include all those defined by the MmpmRscIdType enumeration, which 
  is specified inside the %MmpmRscParamType structure.
  @par
  If multiple requests are from the same registered client ID, MMPM performs 
  an ``update'', i.e., the latest request to the resource replaces the 
  previous ones. 
  @par
  If multiple requests are from different registered client IDs, and they are 
  all making requests to the same resource, MMPM's behavior for subsequent 
  requests is: \n
  - ``max'' for MMPM_RSC_IDs of: \n
      -- VREG \n
      -- CORE_CLK -- Sets the frequency (for corresponding clock IDs
  - ``min'' for MMPM_RSC_IDs of SLEEP_LATENCY
  - ``aggregate'' for MMPM_RSC_IDs of MEM_BW and MIPS
  - ``voting'' for MMPM_RSC_IDs of: \n
      -- POWER \n
      -- CORE_CLK -- Enables (for corresponding clock IDs) \n
      -- ACTIVE_STATE
  - ``reject'' for MMPM_RSC_IDs of: \n
      -- CORE_CLK -- Sets the source (for corresponding clock IDs) \n
      -- CORE_CLK_DOMAIN (for corresponding clock IDs) because the clock 
         is in use

  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  MMPM_Register_Ext() must have already been called.

  @callgraph
  @callergraph
*/   
MMPM_STATUS     MMPM_Request(uint32             clientId, 
                             MmpmRscParamType   *pMmpmRscParam);

/*======================================================================
   FUNCTION: MMPM_Release
======================================================================*/
/** @ingroup mmpm_func_release
  Releases resources for a hardware core via a pre-registered client ID. 

  Resources include all those defined by the MmpmRscIdType enumeration, 
  which is specified inside the %MmpmRscParamType structure. All resources 
  under the same resource ID are released for the registered client.

  @datatypes
  MmpmRscParamType

  @param[in] clientId      Registered client ID.
  @param[in] pMmpmRscParam Pointer to the %MmpmRscParamType structure.

  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  MMPM_Register_Ext() must have already been called.

  @callgraph
  @callergraph
*/
MMPM_STATUS     MMPM_Release(uint32             clientId,
                             MmpmRscParamType   *pMmpmRscParam);

/*======================================================================
   FUNCTION: MMPM_Pause
======================================================================*/
/** @ingroup mmpm_func_pause
  Gates the resources of a hardware core via a pre-registered client ID. 

  @datatypes
  MmpmRscParamType

  @param[in] clientId      Registered client ID.
  @param[in] pMmpmRscParam Pointer to the %MmpmRscParamType structure.

  @detdesc
  Only core clock and VREG resources are supported for this function.
  @par
  If multiple requests are from the same registered client ID, MMPM 
  performs an ``update'', i.e., the latest request to the resource replaces 
  the previous ones. 
  @par
  If multiple requests are from different registered client IDs, and they 
  are all making the same resource gate to the same hardware core instance 
  (same core ID + same core instance ID), MMPM's behavior is the first 
  ungate to ungate the resource and the last gate to gate the resource. 

  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  MMPM_Register_Ext() must have already been called.
   
  @callgraph
  @callergraph
*/
MMPM_STATUS     MMPM_Pause(uint32               clientId, 
                           MmpmRscParamType     *pMmpmRscParam);

/*======================================================================
   FUNCTION: MMPM_Resume
======================================================================*/
/** @ingroup mmpm_func_resume
  Ungates the resources of a hardware core via a pre-registered client ID.

  Only core clock and VREG resources are supported for this function.

  @datatypes
  MmpmRscParamType

  @param[in] clientId      Registered client ID.
  @param[in] pMmpmRscParam Pointer to the %MmpmRscParamType structure.

  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  MMPM_Register_Ext() must have already been called.
   
  @callgraph
  @callergraph
*/
MMPM_STATUS     MMPM_Resume(uint32              clientId, 
                            MmpmRscParamType    *pMmpmRscParam);

/*======================================================================
   FUNCTION: MMPM_Request_Ext
======================================================================*/
/** @ingroup mmpm_func_request_ext
  Requests resources for a hardware core via a pre-registered client ID. 
  The function allows requests for several resources in a single call.

  @datatypes
  MmpmRscExtParamType

  @param[in] clientId     Registered client ID.
  @param[in] pRscExtParam Pointer to the %MmpmRscExtParamType structure that 
                          contains the parameters of the multiple requests.

  @detdesc
  Resources include all those defined by the MmpmRscIdType enumeration, which 
  is specified inside the %MmpmRscExtParamType structure.
  @par
  If multiple requests are from the same registered client ID, MMPM performs 
  an ``update'', i.e., the latest request to the resource replaces the 
  previous ones. 
  @par
  If multiple requests are from different registered client IDs, and they are 
  all making requests to the same resource, MMPM's behavior for subsequent 
  requests is: \n
  - ``max'' for MMPM_RSC_IDs of: \n
      -- VREG \n
      -- CORE_CLK -- Sets the frequency (for corresponding clock IDs
  - ``min'' for MMPM_RSC_IDs of SLEEP_LATENCY
  - ``aggregate'' for MMPM_RSC_IDs of MEM_BW and MIPS
  - ``voting'' for MMPM_RSC_IDs of: \n
      -- POWER \n
      -- CORE_CLK -- Enables (for corresponding clock IDs) \n
      -- ACTIVE_STATE
  - ``reject'' for MMPM_RSC_IDs of: \n
      -- CORE_CLK -- Sets the source (for corresponding clock IDs) \n
      -- CORE_CLK_DOMAIN (for corresponding clock IDs) because the clock 
         is in use
  @par
  This function also allows selection of an asynchronous (non-blocking) 
  execution. If an asynchronous execution is requested, the function 
  returns immediately after storing the request parameters. The requests 
  are then processed in the background and the callback function specified 
  by the client during registration is called when all requests in the 
  bundle are processed.

  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  MMPM_Register_Ext() must have already been called.
   
  @callgraph
  @callergraph
*/
MMPM_STATUS     MMPM_Request_Ext(uint32 clientId, 
                                 MmpmRscExtParamType *pRscExtParam);

/*======================================================================
   FUNCTION: MMPM_Release_Ext
======================================================================*/
/** @ingroup mmpm_func_release_ext
  Releases resources for a hardware core via a pre-registered client ID. 

  Resources include all those defined by the MmpmRscIdType enumeration, 
  which is specified inside the %MmpmRscExtParamType structure. All resources 
  under the same resource ID are released for the registered client.

  @datatypes
  MmpmRscExtParamType

  @param[in] clientId     Registered client ID.
  @param[in] pRscExtParam Pointer to the %MmpmRscExtParamType structure that 
                          contains the parameters of the multiple releases.

  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  MMPM_Register_Ext() must have already been called.
   
  @callgraph
  @callergraph
*/
MMPM_STATUS     MMPM_Release_Ext(uint32 clientId, 
                                 MmpmRscExtParamType *pRscExtParam);

/*======================================================================
   FUNCTION: MMPM_Pause_Ext
======================================================================*/
/** @ingroup mmpm_func_pause_ext
  Makes multiple pause calls within one call.

  @datatypes
  MmpmRscExtParamType

  @param[in] clientId     Registered client ID.
  @param[in] pRscExtParam Pointer to the %MmpmRscExtParamType structure 
                          that contains the parameters of the multiple pauses.

  @detdesc
  Multiple pauses are executed and the function call ends when all 
  pauses complete execution.
  @par
  When each pause inside the multiple pauses is executed, its behavior 
  is identical to that of MMPM_Pause().
  @par
  For the individual return status for each pause, the client must parse 
  the pStsArray inside the pRscExtParam parameter.

  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  MMPM_Register_Ext() must have already been called.
   
  @callgraph
  @callergraph
*/
MMPM_STATUS     MMPM_Pause_Ext(uint32 clientId, 
                               MmpmRscExtParamType *pRscExtParam);

/*======================================================================
   FUNCTION: MMPM_Resume_Ext
======================================================================*/
/** @ingroup mmpm_func_resume_ext
  Makes multiple resume calls within one call.

  @datatypes
  MmpmRscExtParamType

  @param[in] clientId     Registered client ID.
  @param[in] pRscExtParam Pointer to the %MmpmRscExtParamType structure that 
                          contains the parameters of the multiple resumes.

  @detdesc
  Multiple resumes are executed and the function call ends when all 
  resumes complete execution.
  @par
  When each resume inside the multiple resumes is executed, its behavior 
  is identical to that of MMPM_Resume().
  @par
  For the individual return status for each resume, the client must parse 
  the pStsArray inside the pRscExtParam parameter.
  
  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  MMPM_Register_Ext() must have already been called.
   
  @callgraph
  @callergraph
*/
MMPM_STATUS     MMPM_Resume_Ext(uint32 clientId, 
                                MmpmRscExtParamType *pRscExtParam);

/*======================================================================
   FUNCTION: MMPM_GetInfo
======================================================================*/
/** @ingroup mmpm_func_get_info
  Retrieves resources information. This function does not require 
  pre-registration.

  Based on the information ID specified in the %MmpmInfoDataType structure, 
  the MMPM returns the requested information for the ADSPPM resources.

  @datatypes
  MmpmInfoDataType

  @param[in] pInfoData Pointer to the %MmpmInfoDataType structure.

  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  MMPM must be initialized before calling %MMPM_GetInfo().
   
  @callgraph
  @callergraph
*/
MMPM_STATUS     MMPM_GetInfo(MmpmInfoDataType   *pInfoData);

/*======================================================================
   FUNCTION: MMPM_SetParameter
======================================================================*/
/** @ingroup mmpm_func_set_parameter
  Sets various client-specific and global parameters.

  @datatypes
  MmpmParameterConfigType

  @param[in] clientId     Client ID.
  @param[in] pParamConfig Pointer to the %MmpmParameterConfigType structure.

  @return
  #MMPM_STATUS_SUCCESS -- On success. \n
  Otherwise, an error code on failure.

  @dependencies
  MMPM_Register_Ext() must have already been called.

  @callgraph
  @callergraph
*/
MMPM_STATUS     MMPM_SetParameter(uint32 clientId, MmpmParameterConfigType *pParamConfig);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* MMPM_H */  
