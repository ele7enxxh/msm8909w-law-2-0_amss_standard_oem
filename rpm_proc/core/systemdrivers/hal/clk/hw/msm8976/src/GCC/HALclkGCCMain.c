/*
==============================================================================

FILE:         HALclkGCCMain.c

DESCRIPTION:
   The main auto-generated file for GCC.


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/msm8976/src/GCC/HALclkGCCMain.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
03/21/2014            Auto-generated.

==============================================================================
            Copyright (c) 2014 QUALCOMM Technologies Incorporated.
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
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSAXIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSTCUClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRCPLL0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRCPLL1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRXODIV1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRXODIV1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCGPUClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP1I2CClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP1SPIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP2I2CClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP2SPIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP3I2CClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP3SPIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP4I2CClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP4SPIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP1I2CClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP1SPIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP2I2CClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP2SPIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP3I2CClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP3SPIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP4I2CClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP4SPIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2UART1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2UART2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSPUARTSIMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBYTE0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCAMSSGP0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCAMSSGP1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCAMSSTOPAHBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCCIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCPPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCRYPTOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCSI0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCSI1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCSI2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCSIPHY0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCSIPHY1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCESC0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGFX3DClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP3ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCIPAClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCJPEG0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCMCLK0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCMCLK1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCMCLK2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCMDPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCMDPVSYNCClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCLK0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCNOCBFDCDClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPDM2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQ6TBUClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSATBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSSTMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTRACEClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCRBCPRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCRPMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSDCC1APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSDCC1ICECOREClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSDCC2APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSECCTRLClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSLEEPCLKClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMBIMCDDRCH0CYDIV8ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMDEBUGCYDIV8ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMIAHBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMISERClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSYSNOCBFDCDClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSBFSICClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSBFSSYSTEMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSBHSSYSTEMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCVCODEC0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCVFE0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCVFE1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCIPAClkDomain;



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
 * NOTES:
 * - HAL_clk_SourceMapType is an array of mapped sources
 *   - see HALclkInternal.h.
 *
 * - If source index is reserved/not used in a clock diagram, please tie that
 *   to HAL_CLK_SOURCE_GROUND.
 *
 * - {HAL_CLK_SOURCE_NULL, HAL_CLK_SOURCE_INDEX_INVALID} is used to indicate
 *   the end of the mapping array. If we reach this element during our lookup,
 *   we'll know we could not find the matching source enum for the register
 *   value, or vice versa.
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap[] =
{
  {HAL_CLK_SOURCE_XO,          0},
  {HAL_CLK_SOURCE_GPLL0,       1},
  {HAL_CLK_SOURCE_GPLL6,       2},
  {HAL_CLK_SOURCE_NULL,        HAL_CLK_SOURCE_INDEX_INVALID}
};
/*
 * aBIMCSourceMap
 *
 * BIMC HW source mapping
 * 
 * NOTES:
 * - HAL_clk_SourceMapType is an array of mapped sources
 *   - see HALclkInternal.h.
 *
 * - If source index is reserved/not used in a clock diagram, please tie that
 *   to HAL_CLK_SOURCE_GROUND.
 *
 * - {HAL_CLK_SOURCE_NULL, HAL_CLK_SOURCE_INDEX_INVALID} is used to indicate
 *   the end of the mapping array. If we reach this element during our lookup,
 *   we'll know we could not find the matching source enum for the register
 *   value, or vice versa.
 * 
 */
HAL_clk_SourceMapType aBIMCSourceMap[] =
{
  {HAL_CLK_SOURCE_XO,          0},
  {HAL_CLK_SOURCE_GPLL0,       1},
  {HAL_CLK_SOURCE_BIMCPLL,     2},
  {HAL_CLK_SOURCE_GPLL5,       5},
  {HAL_CLK_SOURCE_NULL,        HAL_CLK_SOURCE_INDEX_INVALID}
};


/*
 * aQDSS_ATSourceMap
 *
 * QDSS AT source mapping
 * 
 * NOTES:
 * - HAL_clk_SourceMapType is an array of mapped sources
 *   - see HALclkInternal.h.
 *
 * - If source index is reserved/not used in a clock diagram, please tie that
 *   to HAL_CLK_SOURCE_GROUND.
 *
 * - {HAL_CLK_SOURCE_NULL, HAL_CLK_SOURCE_INDEX_INVALID} is used to indicate
 *   the end of the mapping array. If we reach this element during our lookup,
 *   we'll know we could not find the matching source enum for the register
 *   value, or vice versa.
 * 
 */
HAL_clk_SourceMapType aQDSS_ATSourceMap[] =
{
  {HAL_CLK_SOURCE_XO,          0},
  {HAL_CLK_SOURCE_GPLL0,       1},
  {HAL_CLK_SOURCE_GPLL4,       2},
  {HAL_CLK_SOURCE_GPLL6,       3},
  {HAL_CLK_SOURCE_NULL,        HAL_CLK_SOURCE_INDEX_INVALID}
};


/* 
 * aQDSSTRACE_TSCTRSourceMap
 * The source map is different for QDSS TRACECLKIN and QDSS TSCTR
 */    
static HAL_clk_SourceMapType aQDSSTRACE_TSCTRSourceMap[] =
{
  {HAL_CLK_SOURCE_XO,          0},
  {HAL_CLK_SOURCE_GPLL0,       2},
  {HAL_CLK_SOURCE_GPLL4,       1},
  {HAL_CLK_SOURCE_GPLL6,       3},
  {HAL_CLK_SOURCE_NULL,        HAL_CLK_SOURCE_INDEX_INVALID}
};

/*
 * aIPASourceMap
 * The source map is different for QDSS TRACECLKIN and QDSS TSCTR
 */    
static HAL_clk_SourceMapType aIPASourceMap[] =
{
  {HAL_CLK_SOURCE_XO,          0},
  {HAL_CLK_SOURCE_GPLL0,       2},
  {HAL_CLK_SOURCE_GPLL6,       1},
  {HAL_CLK_SOURCE_NULL,        HAL_CLK_SOURCE_INDEX_INVALID}
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
 * HAL_clk_mQDSSATBClockDomainControl
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mQDSSATBClockDomainControl =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aQDSS_ATSourceMap
};

/*
 * HAL_clk_mQDSSTRACE_TSCTRClockDomainControl
 *
 * Functions for controlling QDSS TRACEIN and TSCTR  clock domains.We register the different sourcemap here.
 */
HAL_clk_ClockDomainControlType HAL_clk_mQDSSTRACE_TSCTRDomainControl =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aQDSSTRACE_TSCTRSourceMap
}; 


/*
 * HAL_clk_mIPAClockDomainControl
 *
 * Functions for controlling IPA  clock domains.We register the different sourcemap here.
 */
HAL_clk_ClockDomainControlType HAL_clk_mIPADomainControl =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aIPASourceMap
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
  &HAL_clk_mGCCAPSSAXIClkDomain,
  &HAL_clk_mGCCAPSSTCUClkDomain,
  &HAL_clk_mGCCBIMCDDRCPLL0ClkDomain,
  &HAL_clk_mGCCBIMCDDRCPLL1ClkDomain,
  &HAL_clk_mGCCBIMCDDRClkDomain,
  &HAL_clk_mGCCBIMCDDRXOClkDomain,
  //&HAL_clk_mGCCBIMCDDRXODIV1ClkDomain,
  //&HAL_clk_mGCCBIMCDDRXODIV1ClkDomain,
  &HAL_clk_mGCCBIMCGPUClkDomain,
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
  //&HAL_clk_mGCCBYTE1ClkDomain,
  //&HAL_clk_mGCCCAMSSGP0ClkDomain,
  //&HAL_clk_mGCCCAMSSGP1ClkDomain,
  //&HAL_clk_mGCCCAMSSTOPAHBClkDomain,
  //&HAL_clk_mGCCCCIClkDomain,
  //&HAL_clk_mGCCCPPClkDomain,
  //&HAL_clk_mGCCCSI0ClkDomain,
  //&HAL_clk_mGCCCSI1ClkDomain,
  //&HAL_clk_mGCCCSI2ClkDomain,
  //&HAL_clk_mGCCCSIPHY0ClkDomain,
  //&HAL_clk_mGCCCSIPHY1ClkDomain,
  &HAL_clk_mGCCCRYPTOClkDomain,
  //&HAL_clk_mGCCCSI0ClkDomain,
  //&HAL_clk_mGCCCSI1ClkDomain,
  //&HAL_clk_mGCCCSI2ClkDomain,
  //&HAL_clk_mGCCCSIPHY0ClkDomain,
  //&HAL_clk_mGCCESC0ClkDomain,
  //&HAL_clk_mGCCESC1ClkDomain,
  //&HAL_clk_mGCCGFX3DClkDomain,
  //&HAL_clk_mGCCGP1ClkDomain,
  //&HAL_clk_mGCCGP2ClkDomain,
  //&HAL_clk_mGCCGP3ClkDomain,
  //&HAL_clk_mGCCJPEG0ClkDomain,
  //&HAL_clk_mGCCMCLK0ClkDomain,
  //&HAL_clk_mGCCMCLK1ClkDomain,
  //&HAL_clk_mGCCMCLK2ClkDomain,
  //&HAL_clk_mGCCMDPClkDomain,
  //&HAL_clk_mGCCMDPVSYNCClkDomain,
  //&HAL_clk_mGCCPCLK0ClkDomain,
  //&HAL_clk_mGCCPCLK1ClkDomain,
  &HAL_clk_mGCCPCNOCBFDCDClkDomain,
  //&HAL_clk_mGCCPDM2ClkDomain,
  &HAL_clk_mGCCQ6TBUClkDomain,
  &HAL_clk_mGCCQDSSATBClkDomain,
  &HAL_clk_mGCCQDSSSTMClkDomain,
  &HAL_clk_mGCCQDSSTRACEClkDomain,
  &HAL_clk_mGCCQDSSTSCTRClkDomain,
  &HAL_clk_mGCCRBCPRClkDomain,
  &HAL_clk_mGCCRPMClkDomain,
  //&HAL_clk_mGCCSDCC1APPSClkDomain,
  //&HAL_clk_mGCCSDCC2APPSClkDomain,
  //&HAL_clk_mGCCSECCTRLClkDomain,
  //&HAL_clk_mGCCSLEEPCLKClkDomain,
  &HAL_clk_mGCCSLEEPCLKClkDomain,
  &HAL_clk_mGCCSPDMBIMCDDRCH0CYDIV8ClkDomain,
  &HAL_clk_mGCCSPDMDEBUGCYDIV8ClkDomain,
  &HAL_clk_mGCCSPMIAHBClkDomain,
  &HAL_clk_mGCCSPMISERClkDomain,
  &HAL_clk_mGCCSYSNOCBFDCDClkDomain,
  //&HAL_clk_mGCCULTAUDIOAHBFABRICClkDomain,
  //&HAL_clk_mGCCULTAUDIODIGCODECClkDomain,
  //&HAL_clk_mGCCULTAUDIOEXTMCLK2ClkDomain,
  //&HAL_clk_mGCCULTAUDIOLPAIFAUXI2SClkDomain,
  //&HAL_clk_mGCCULTAUDIOLPAIFPRII2SClkDomain,
  //&HAL_clk_mGCCULTAUDIOLPAIFSECI2SClkDomain,
  &HAL_clk_mGCCXOClkDomain,
  &HAL_clk_mGCCIPAClkDomain,
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

