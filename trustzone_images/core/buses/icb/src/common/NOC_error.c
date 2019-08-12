/*==============================================================================

FILE:      NOC_error.c

DESCRIPTION: This file implements NOC Error Handler.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/common/NOC_error.c#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2015/08/26  rc     Added HW version for Bear family FlexNoC version 2.11.3-6
2014/17/10  rc     Added HW version for Bear family FlexNoC version 2.8.11-1
2013/11/07  pm     Added HW version for Bear family FlexNoC version 2.8.6-1
2013/11/07  pm     Fixed missing break when switching on HW version
2013/08/28  pm     Removed Clock_EnableBusAccess() call in interrupt context
2013/05/17  pm     Moved NOCInfo initialization before use in NOC_REG_ADDR() 
2013/05/16  pm     Enabled clocks at right time in NOC_error_init() 
2013/05/08  pm     Added FlexNoC version 2.8 
2012/10/03  av     Created

        Copyright (c) 2013-2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "NOC_error.h"
//#include "ClockPIL.h"
#include "ClockTZ.h"
#include "tzbsp_isr.h"
#include "tzbsp_log.h"
#include "tzbsp.h"

/*============================================================================
                          MACROs
============================================================================*/
#define NOC_OUT8(addr, data)   (*((volatile uint8 *) (addr)) = ((uint8) (data)))
#define NOC_OUT16(addr, data)  (*((volatile uint16 *) (addr)) = ((uint16) (data)))
#define NOC_OUT32(addr, data)  (*((volatile uint32 *) (addr)) = ((uint32) (data)))

#define NOC_IN8(addr)   ((uint8) (*((volatile uint8 *) (addr))))
#define NOC_IN16(addr)  ((uin16) (*((volatile uint16 *) (addr))))
#define NOC_IN32(addr)  ((uint32)(*((volatile uint32 *) (addr))))

#define NOC_REG_ADDR(x,offset)   (((uint8*)NOCInfo[(x)].base_addr) + (offset))

/*============================================================================
                          DEFINEs
============================================================================*/
/* Reserved value of offset that indicates that the register does not apply to
 * this version of hardware
 */
#define REGISTER_NOT_APPLICABLE 0xFF  


/**
 * NOC Register Interface
 */
typedef struct
{
   uint8  core_id;       /**< NOC_OBS_ID_COREID offset */
   uint8  revision_id;   /**< NOC_OBS_ID_REVISIONID offset */
   uint8  errctl;        /**< NOC_OBS_ERRCTL offset -- Pre-FlexNoC 2.8 only */
   uint8  errstatus;     /**< NOC_OBS_ERRSTATUS offset -- Pre-FlexNoC 2.8 only */
   uint8  errlog0;       /**< NOC_OBS_ERRLOG0 offset */
   uint8  errlog1;       /**< NOC_OBS_ERRLOG1 offset */
   uint8  errlog2;       /**< NOC_OBS_ERRLOG2 offset */
   uint8  errlog3;       /**< NOC_OBS_ERRLOG3 offset */
   uint8  errlog4;       /**< NOC_OBS_ERRLOG4 offset */
   uint8  errlog5;       /**< NOC_OBS_ERRLOG5 offset */
   uint8  faulten;       /**< NOC_OBS_FAULTEN offset -- FlexNoC 2.8 only */
   uint8  errvld;        /**< NOC_OBS_ERRVLD offset -- FlexNoC 2.8 only */
   uint8  errclr;        /**< NOC_OBS_ERRCLR offset -- FlexNoC 2.8 only */
   uint8  errlog6;       /**< NOC_OBS_ERRLOG6 offset -- FlexNoC 2.8 only */
   /* New fields for new NoC versions go here */
}NOC_hw_type;

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

extern NOCERR_propdata_type NOCERR_propdata;
static NOCERR_info_type* NOCInfo;

static NOC_hw_type* hw;

/*============================================================================
                        NOC HW Register DEFINITION 
============================================================================*/
/* NOC HW Register Offsets Version 0.0 */
static NOC_hw_type NOC_0_0 = {
    0x00,                 /**< NOC_OBS_ID_COREID offset */
    0x04,                 /**< NOC_OBS_ID_REVISIONID offset */
    0x08,                 /**< NOC_OBS_ERRCTL offset */
    0x0C,                 /**< NOC_OBS_ERRSTATUS offset */
    0x10,                 /**< NOC_OBS_ERRLOG0 offset */
    0x14,                 /**< NOC_OBS_ERRLOG1 offset */
    0x18,                 /**< NOC_OBS_ERRLOG2 offset */
    0x1C,                 /**< NOC_OBS_ERRLOG3 offset */
    0x20,                 /**< NOC_OBS_ERRLOG4 offset */
    0x24,                 /**< NOC_OBS_ERRLOG5 offset */   
    REGISTER_NOT_APPLICABLE,
    REGISTER_NOT_APPLICABLE,
    REGISTER_NOT_APPLICABLE,
    REGISTER_NOT_APPLICABLE,
   /* When adding a new HW version, fill with REGISTER_NOT_APPLICABLE from
      here down to the length of NOC_hw_type */
};


/* NOC HW Register Offsets Version 2.8 */
static NOC_hw_type NOC_2_8 = {
    0x00,                 /**< NOC_OBS_ID_COREID offset */
    0x04,                 /**< NOC_OBS_ID_REVISIONID offset */
    REGISTER_NOT_APPLICABLE,
    REGISTER_NOT_APPLICABLE,
    0x14,                 /**< NOC_OBS_ERRLOG0 offset */
    0x18,                 /**< NOC_OBS_ERRLOG1 offset */
    0x1C,                 /**< NOC_OBS_ERRLOG2 offset */
    0x20,                 /**< NOC_OBS_ERRLOG3 offset */
    0x24,                 /**< NOC_OBS_ERRLOG4 offset */ 
    0x28,                 /**< NOC_OBS_ERRLOG5 offset */ 

    0x08,                 /**< NOC_OBS_FAULTEN offset */
    0x0C,                 /**< NOC_OBS_ERRVLD offset */
    0x10,                 /**< NOC_OBS_ERRCLR offset */

    0x2C,                  /**< NOC_OBS_ERRLOG6 offset */ 
   /* When adding a new HW version, fill with REGISTER_NOT_APPLICABLE from
      here down to the length of NOC_hw_type */
};

/* Add future HW versions below...*/

/*============================================================================
                                 FUNCTIONS
============================================================================*/
/*===========================================================================*/
/**
@brief 
      Function to dump BIMC error registers if they are not 0.
 
@param[in]  void

@return    
      void

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*==============================================================================*/
void Dump_BIMC_Registers(void)
{
  int i;
  for(i=0;i<NOCERR_propdata.BIMC_num_reg;i++)
  {
    if(NOC_IN32(NOCERR_propdata.BIMC_reg_addr[i]) != 0x00000000)
    {
      TZBSP_LOG_ERR("%s = 0x%08x", NOCERR_propdata.BIMC_reg_names[i], NOC_IN32(NOCERR_propdata.BIMC_reg_addr[i]));
    }
  }
}

/*===========================================================================*/
/**
@brief 
      Handler function for NOC error interrupt.
 
@param[in]  void* arg Context data for client.

@return    
      void* arg.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*==============================================================================*/
void* NOC_Error_Handle_Interrupt(void* arg)
{
 uint8 i;
 uint32 intr_vector = (uint32) arg;

 // Find which NOC generated interrupt
 for(i=0; i < NOCERR_propdata.len && NOCInfo[i].intr_vector != intr_vector; i++);

 // Check if the error is valid (for FlexNoC 2.8 or later)
 if (hw->errvld != REGISTER_NOT_APPLICABLE) 
 {
    if ( (NOC_IN32(NOC_REG_ADDR(i,hw->errvld)) & 0x1) != 0x1 )
    {
       TZBSP_LOG_ERR("NoC Error is not valid!");
       return arg;
    }
 }

 if(i <  NOCERR_propdata.len)
 {
     // Log Syndrome Registers
     if (hw->errstatus != REGISTER_NOT_APPLICABLE)  // Pre-FlexNoC 2.8 
     {
        TZBSP_LOG_ERR("%s ERROR: ERRSTATUS = 0x%08x",
                     NOCInfo[i].name, NOC_IN32(NOC_REG_ADDR(i,hw->errstatus)));
     }

     if(!(NOCInfo[i].errlog_filter & NOC_ERRLOG0))
     {
         TZBSP_LOG_ERR("%s ERROR: ERRLOG0 = 0x%08x",
                       NOCInfo[i].name, NOC_IN32(NOC_REG_ADDR(i,hw->errlog0)));
     }
     if(!(NOCInfo[i].errlog_filter & NOC_ERRLOG1))
     {
         TZBSP_LOG_ERR("%s ERROR: ERRLOG1 = 0x%08x",
                       NOCInfo[i].name, NOC_IN32(NOC_REG_ADDR(i,hw->errlog1)));
     }
     if(!(NOCInfo[i].errlog_filter & NOC_ERRLOG2))
     {
         TZBSP_LOG_ERR("%s ERROR: ERRLOG2 = 0x%08x",
                       NOCInfo[i].name, NOC_IN32(NOC_REG_ADDR(i,hw->errlog2)));
     }
     if(!(NOCInfo[i].errlog_filter & NOC_ERRLOG3))
     {
         TZBSP_LOG_ERR("%s ERROR: ERRLOG3 = 0x%08x",
                       NOCInfo[i].name, NOC_IN32(NOC_REG_ADDR(i,hw->errlog3)));
     }
     if(!(NOCInfo[i].errlog_filter & NOC_ERRLOG4))
     {
         TZBSP_LOG_ERR("%s ERROR: ERRLOG4 = 0x%08x",
                       NOCInfo[i].name, NOC_IN32(NOC_REG_ADDR(i,hw->errlog4)));
     }
     if(!(NOCInfo[i].errlog_filter & NOC_ERRLOG5))
     {
         TZBSP_LOG_ERR("%s ERROR: ERRLOG5 = 0x%08x",
                       NOCInfo[i].name, NOC_IN32(NOC_REG_ADDR(i,hw->errlog5)));
     }
     if (hw->errlog6 != REGISTER_NOT_APPLICABLE)    // FlexNoC 2.8
     {
        if(!(NOCInfo[i].errlog_filter & NOC_ERRLOG6))
        {
            TZBSP_LOG_ERR("%s ERROR: ERRLOG6 = 0x%08x",
                          NOCInfo[i].name, NOC_IN32(NOC_REG_ADDR(i,hw->errlog6)));
        }
     }

     Dump_BIMC_Registers();

     if(!(NOCInfo[i].errlog_filter & NOC_ERRFATAL))
     {
         tzbsp_err_fatal(TZBSP_ERR_FATAL_NOC_ERROR);
     }
     else
     {
         // Clear err status
         if (hw->errstatus != REGISTER_NOT_APPLICABLE)  // Pre-FlexNoC 2.8
         {
            NOC_OUT32(NOC_REG_ADDR(i,hw->errstatus), (NOC_IN32(NOC_REG_ADDR(i,hw->errstatus)) | 0x04));
         }
         if (hw->errclr != REGISTER_NOT_APPLICABLE)    // FlexNoC 2.8
         {
            NOC_OUT32(NOC_REG_ADDR(i,hw->errclr), (NOC_IN32(NOC_REG_ADDR(i,hw->errclr)) | 0x01));
         }
     }
 }
 else
 {
     TZBSP_LOG_ERR("Invalid Interrupt Vector!");
 }
 
 return arg;
}

//*============================================================================*/
/**
@brief
      Performs initialization for NOC Error Handler.
      It enables interrupts required to handle NOC errors.

@param[in]  None.

@return
      None.

@dependencies
      None.

@sideeffects
      None.
*/
/*============================================================================*/
void NOC_Error_Init( void )
{
    uint8 i;

    NOCInfo = NOCERR_propdata.NOCInfo;
    if(NOCInfo == NULL)
    {
        return;  //NOC data structure is not initialized in data file
    }

    hw = &NOC_0_0;  // Temporarily assign a hw type to allow query of NoC version

    //Enable NOC cfg Clocks
    //Clock_EnableBusAccess(TRUE);
    Clock_EnableClockGroup(CLOCK_GROUP_BUS);

    /* This assumes all NoC versions on the chip are the same, so use index 0.
     * Mask 31:8 (AND with 0xFFFFFF00) to get the FlexNoC ID. 
     */
    switch ( 0xFFFFFF00 & NOC_IN32(NOC_REG_ADDR(0,hw->revision_id)) ) 
    {
       // Future: Mask/check major revision only
       case 0x0113D100:  // FlexNoc 2.8.3
       case 0x01222B00:  // FlexNoC 2.8.6-1
       case 0x000C2400:   // FlexNoC 2.11.1	       
       case 0x002D2D00:   // FlexNoC 2.11.3-6	       	       
       {
           hw = &NOC_2_8;
           break;
       }
       default: // Future: Find full set of old NoC major versions (0x00F32100?)
       {
          hw = &NOC_0_0;
       }
       /* 
       default:
       {
          TZBSP_LOG_ERR("Unrecognized NoC Revsion ID!");
          tzbsp_err_fatal(TZBSP_ERR_FATAL_NOC_ERROR);
          return;
       }
       */
    }

    for(i=0; i < NOCERR_propdata.len; i++)
    {
        // TZ register FIQ handler
        if(0 != tzbsp_register_isr(NOCInfo[i].intr_vector,
                                   "NOCERR",
                                   NOC_Error_Handle_Interrupt,
                                   (void*)NOCInfo[i].intr_vector,
                                   TZBSP_INTF_TRIGGER_LEVEL,
                                   NOCInfo[i].intr_enable))
        {
            TZBSP_LOG_ERR("NOC init error !");
            tzbsp_err_fatal(TZBSP_ERR_FATAL_NOC_ERROR);
            return;
        }

        // Enable Interrupts (Set FAULTEN)
        if(NOCInfo[i].intr_enable)
        {
            if (hw->errctl != REGISTER_NOT_APPLICABLE)
            {
               NOC_OUT32(NOC_REG_ADDR(i,hw->errctl), (NOC_IN32(NOC_REG_ADDR(i,hw->errctl)) | 0x02));
            }
            if (hw->faulten != REGISTER_NOT_APPLICABLE)    // FlexNoC 2.8
            {
               NOC_OUT32(NOC_REG_ADDR(i,hw->faulten), (NOC_IN32(NOC_REG_ADDR(i,hw->faulten)) | 0x01));
            }
        }
    }

    //Disable NOC cfg Clocks, TZ will restore the state of clocks.
    //Clock_EnableBusAccess(FALSE);
    Clock_DisableClockGroup(CLOCK_GROUP_BUS);
}


