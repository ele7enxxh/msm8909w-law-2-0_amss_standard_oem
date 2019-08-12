/*
==============================================================================

FILE:         HALclkGCCMain.c

DESCRIPTION:
   The main auto-generated file for GCC.


==============================================================================

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x07/src/GCC/HALclkGCCMain.c#2 $

==============================================================================
            Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkTest.h"
#include "HALclkGeneric.h"
#include "HALclkHWIO.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Prototypes
** ==========================================================================*/


/* ============================================================================
**    Externs
** ==========================================================================*/


/*
 * Clock domains
 */
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCACCClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSAHBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSTCUClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCMSSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP1I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP1SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP2I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP2SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP3I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP3SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP4I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP4SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP5I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP5SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP6I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP6SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART1APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART2APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART3APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART4APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART5APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART6APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSPUARTSIMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCRYPTOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCEMAC0125MClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCEMAC0RXClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCEMAC0SYS25MClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCEMAC0TXClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCDEBUGPREDIVCLKWITHTCKClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCSLEEPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOEXTMCLK2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOLPAIFAUXI2SClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOLPAIFPRII2SClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOLPAIFSECI2SClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOPCMDATAOEClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOSECPCMDATAOEClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOTERPCMDATAOEClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP3ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCNOCBFDCDClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPDM2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSATClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSSTMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTRACECLKINClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQPICClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCRBCPRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCRPMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSDCC1APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSDCC2APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSECCTRLClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMIAHBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMISERClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCULTAUDIOAHBFABRICClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSBHSICClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSBHSICIOCALClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSBHSICSYSTEMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSBHSSYSTEMClkDomain;

/*
 * Power domains
 */
extern HAL_clk_PowerDomainDescType HAL_clk_mGCCBIMCPowerDomain;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * aGCCSourceMap
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_GPLL1,              2 },
  { HAL_CLK_SOURCE_BIMCPLL,            3 },
  { HAL_CLK_SOURCE_GPLL2,              4 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap
};


/*
 * aGCCSourceMap_1
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_1[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL1,              1 },
  { HAL_CLK_SOURCE_GPLL0,              4 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_1
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_1 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_1
};

/*
 * aGCCSourceMap_2
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_2[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_GPLL2,              2 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_2
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_2 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_2
};

/*
 * aGCCSourceMap_4
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_4[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_BIMCPLL,            2 },
  { HAL_CLK_SOURCE_GPLL2,              3 },
  { HAL_CLK_SOURCE_GPLL1,              4 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_4
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_4 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_4
};

/*
 * aGCCSourceMap_5
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_5[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL1,              1 },
  { HAL_CLK_SOURCE_GPLL2,              2 },
  { HAL_CLK_SOURCE_GPLL0,              3 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_5
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_5 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_5
};


/*
 * aGCCSourceMap_7
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_7[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_GPLL1,              2 },
  { HAL_CLK_SOURCE_GPLL2,              3 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};

/*
 * HAL_clk_mGCCClockDomainControl_7
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_7 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_7
};

/*
 * HAL_clk_aGCCClockDomainDesc
 *
 * List of GCC clock domains
*/
static HAL_clk_ClockDomainDescType * HAL_clk_aGCCClockDomainDesc [] =
{
  //&HAL_clk_mGCCACCClkDomain,
  //&HAL_clk_mGCCAPSSAHBClkDomain,
  &HAL_clk_mGCCAPSSTCUClkDomain,
  &HAL_clk_mGCCBIMCDDRClkDomain,
  &HAL_clk_mGCCBIMCDDRXOClkDomain,
  &HAL_clk_mGCCBIMCMSSClkDomain,
//  &HAL_clk_mGCCBIMCGPUClkDomain,
  //&HAL_clk_mGCCBLSP1QUP1I2CClkDomain,
  //&HAL_clk_mGCCBLSP1QUP1SPIClkDomain,
  //&HAL_clk_mGCCBLSP1QUP2I2CClkDomain,
  //&HAL_clk_mGCCBLSP1QUP2SPIClkDomain,
  //&HAL_clk_mGCCBLSP1QUP3I2CClkDomain,
  //&HAL_clk_mGCCBLSP1QUP3SPIClkDomain,
  //&HAL_clk_mGCCBLSP1QUP4I2CClkDomain,
  //&HAL_clk_mGCCBLSP1QUP4SPIClkDomain,
  //&HAL_clk_mGCCBLSP1QUP5I2CClkDomain,
  //&HAL_clk_mGCCBLSP1QUP5SPIClkDomain,
  //&HAL_clk_mGCCBLSP1QUP6I2CClkDomain,
  //&HAL_clk_mGCCBLSP1QUP6SPIClkDomain,
  //&HAL_clk_mGCCBLSP1UART1ClkDomain,
  //&HAL_clk_mGCCBLSP1UART2ClkDomain,
  //&HAL_clk_mGCCBLSPUARTSIMClkDomain,
  //&HAL_clk_mGCCBYTE0ClkDomain,
  //&HAL_clk_mGCCCAMSSGP0ClkDomain,
  //&HAL_clk_mGCCCAMSSGP1ClkDomain,
  //&HAL_clk_mGCCCAMSSTOPAHBClkDomain,
  &HAL_clk_mGCCCRYPTOClkDomain,
  //&HAL_clk_mGCCCSI0ClkDomain,
  //&HAL_clk_mGCCCSI1ClkDomain,
  //&HAL_clk_mGCCCSIPHY0ClkDomain,
  //&HAL_clk_mGCCESC0ClkDomain,
  //&HAL_clk_mGCCGFX3DClkDomain,
  //&HAL_clk_mGCCGP1ClkDomain,
  //&HAL_clk_mGCCGP2ClkDomain,
  //&HAL_clk_mGCCGP3ClkDomain,
  //&HAL_clk_mGCCMCLK0ClkDomain,
  //&HAL_clk_mGCCMCLK1ClkDomain,
  //&HAL_clk_mGCCMDPClkDomain,
  //&HAL_clk_mGCCMDPVSYNCClkDomain,
  //&HAL_clk_mGCCPCLK0ClkDomain,
  &HAL_clk_mGCCPCNOCBFDCDClkDomain,
  //&HAL_clk_mGCCPDM2ClkDomain,
  &HAL_clk_mGCCQDSSATClkDomain,
  &HAL_clk_mGCCQDSSSTMClkDomain,
  &HAL_clk_mGCCQDSSTRACECLKINClkDomain,
  &HAL_clk_mGCCQDSSTSCTRClkDomain,
  &HAL_clk_mGCCQPICClkDomain,
  &HAL_clk_mGCCRBCPRClkDomain,
  &HAL_clk_mGCCRPMClkDomain,
  //&HAL_clk_mGCCSDCC1APPSClkDomain,
  //&HAL_clk_mGCCSDCC2APPSClkDomain,
  //&HAL_clk_mGCCSECCTRLClkDomain,
  &HAL_clk_mGCCGCCSLEEPClkDomain,
  //&HAL_clk_mGCCSPMIAHBClkDomain,
  &HAL_clk_mGCCSPMISERClkDomain,
  //&HAL_clk_mGCCULTAUDIOAHBFABRICClkDomain,
  //&HAL_clk_mGCCULTAUDIODIGCODECClkDomain,
  //&HAL_clk_mGCCULTAUDIOEXTMCLK2ClkDomain,
  //&HAL_clk_mGCCULTAUDIOLPAIFAUXI2SClkDomain,
  //&HAL_clk_mGCCULTAUDIOLPAIFPRII2SClkDomain,
  //&HAL_clk_mGCCULTAUDIOLPAIFSECI2SClkDomain,
  //&HAL_clk_mGCCULTAUDIOPCMDATAOEClkDomain,
  //&HAL_clk_mGCCULTAUDIOXOClkDomain,
  //&HAL_clk_mGCCUSBHSSYSTEMClkDomain,
  //&HAL_clk_mGCCVCODEC0ClkDomain,
  //&HAL_clk_mGCCVFE0ClkDomain,
  &HAL_clk_mGCCGCCXOClkDomain,
  NULL
};


/*
 * HAL_clk_aGCCPowerDomainDesc
 *
 * List of GCC power domains
 */
static HAL_clk_PowerDomainDescType * HAL_clk_aGCCPowerDomainDesc [] =
{
  &HAL_clk_mGCCBIMCPowerDomain,
  NULL
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_PlatformInitGCCMain
**
** ======================================================================== */

void HAL_clk_PlatformInitGCCMain (void)
{

  /*
   * Install all clock domains
   */
  HAL_clk_InstallClockDomains(HAL_clk_aGCCClockDomainDesc, CLK_CTL_BASE);

  /*
   * Install all power domains
   */
  HAL_clk_InstallPowerDomains(HAL_clk_aGCCPowerDomainDesc, CLK_CTL_BASE);

} /* END HAL_clk_PlatformInitGCCMain */

