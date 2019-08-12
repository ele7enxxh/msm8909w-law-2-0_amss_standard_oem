/*
==============================================================================

FILE:         HALclkMSSMain.c

DESCRIPTION:
   The main auto-generated file for MSS.


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/msm8909/src/mss/HALclkMSSMain.c#1 $

when          who     what, where, why
----------    ---     --------------------------------------------------------
09/26/2013            Auto-generated.

==============================================================================
            Copyright (c) 2013 QUALCOMM Technologies Incorporated.
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
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSBUSCLKClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSBUSCLKMODEMVDDMSSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSConfigBusClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCXOUIM0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCXOUIM1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCXOUIM2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCXOUIMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSGCCCONFNOCClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSGCCSYSNOCClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSGCCPCNOCClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSPLL0MAINClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSPLL1EARLYDIV3ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSPLL1EARLYDIV5ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSPLL1MAINClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSSLEEPROOTClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSQ6ClkDomain;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * aMSSSourceMap
 *
 * MSS HW source mapping
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
static HAL_clk_SourceMapType aMSSSourceMap[] =
{
  {HAL_CLK_SOURCE_XO,          0},
  {HAL_CLK_SOURCE_NULL,        HAL_CLK_SOURCE_INDEX_INVALID}
};


/*
 * HAL_clk_mMSSClockDomainControl
 *
 * Functions for controlling MSS clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mMSSClockDomainControl =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aMSSSourceMap
};


/*
 * HAL_clk_aMSSClockDomainDesc
 *
 * List of MSS clock domains
*/
static HAL_clk_ClockDomainDescType * HAL_clk_aMSSClockDomainDesc [] =
{
  &HAL_clk_mMSSBUSCLKClkDomain,
  &HAL_clk_mMSSBUSCLKMODEMVDDMSSClkDomain, 
  &HAL_clk_mMSSConfigBusClkDomain,
  &HAL_clk_mMSSCXOClkDomain,
  &HAL_clk_mMSSCXOUIM0ClkDomain,
  &HAL_clk_mMSSCXOUIM1ClkDomain,
  &HAL_clk_mMSSCXOUIM2ClkDomain,
  &HAL_clk_mMSSCXOUIMClkDomain,
  &HAL_clk_mMSSGCCSYSNOCClkDomain,
  &HAL_clk_mMSSGCCPCNOCClkDomain,
  //&HAL_clk_mMSSPLL0MAINClkDomain,
  &HAL_clk_mMSSPLL1EARLYDIV3ClkDomain,
  &HAL_clk_mMSSPLL1EARLYDIV5ClkDomain,
  &HAL_clk_mMSSQ6ClkDomain,
  &HAL_clk_mMSSPLL1MAINClkDomain,
  &HAL_clk_mMSSSLEEPROOTClkDomain,
  NULL
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_PlatformInitMSSMain
**
** ======================================================================== */

void HAL_clk_PlatformInitMSSMain (void)
{

  /*
   * Install all clock domains
   */
  HAL_clk_InstallClockDomains(HAL_clk_aMSSClockDomainDesc, MSS_TOP_BASE);

} /* END HAL_clk_PlatformInitMSSMain */

