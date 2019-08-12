#ifndef CLOCKRPMNPA_H
#define CLOCKRPMNPA_H
/*
===========================================================================
*/
/**
  @file ClockRPMNPA.h

  NPA node definitions for the MDM9x45 RPM clock driver.
*/
/*
  ====================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/hw/mdm9x45/ClockRPMNPA.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  07/14/14   pbitra    Ported from 9x35, updated for 9x45.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <DALSys.h>
#include <railway.h>
#include <npa.h>
#include <npa_resource.h>

#include "ClockDriver.h"

/*=========================================================================
      Macros
==========================================================================*/

/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * BIMCStateType
 *
 * BIMC enum state type
 */
typedef enum
{
  BIMC_ON,           /* BIMC is on */
  BIMC_SAVING,       /* BIMC is in power collapsed, save DEHR */
  BIMC_COLLAPSED,    /* BIMC is in power collapsed */
  BIMC_RESTORING,    /* BIMC is in restoring state */
} BIMCStateType;


/*
 * ClockQDSSSpeedType
 *
 * QDSS enum speed type
 */
typedef enum
{
  CLOCK_QDSS_SPEED_XO,
  CLOCK_QDSS_SPEED_LOW,
  CLOCK_QDSS_SPEED_NORMAL,
  CLOCK_QDSS_NUM_SPEEDS,
} ClockQDSSSpeedType;


/*
 * NPAResourceType
 *
 * Structure containing the NPA clocks resource info
 */
typedef struct
{
  npa_resource_definition Resource;
  npa_node_definition     Node;
} NPAResourceType;


/*
 * ClockResourceType
 *
 * Structure containing the clocks resource info
 */
typedef struct
{
  NPAResourceType         NPANode;
  ClockNodeType           *pClock;
  uint32                  nMinLevel;
  uint32                  nMaxLevel;
  uint32                  nCurLevel;
  uint32                  nSavLevel;
} ClockResourceType;


/*
 * BIMCResourceType *
 * Structure containing the BIMC resource info
 */
typedef struct
{
  boolean                 bPowerCollapsed;  // DDR in PC or self-refresh
  BIMCStateType           eXOBIMCState;     // BIMC state in XO shutdown
  BIMCStateType           eBIMCState;
  ClockIdType             nClockDehr;
  ClockPowerDomainIdType  nBIMCPowerID; 
} BIMCResourceType;


/*
 * ClockQDSSResourceType
 *
 * Structure containing the QDSS clocks info
 */
typedef struct
{
  NPAResourceType         NPANode;
  boolean                 bTraceAccess;
  boolean                 bQDSSEnabled;
  ClockVRegLevelType      nQDSSVoltageLevel;
  ClockQDSSLevelType      nQDSSCurrLevel;
  ClockQDSSSpeedType      nQDSSCurrSpeed;
  ClockQDSSSpeedType      nQDSSSaveSpeed;
  ClockNodeType           *pQDSSCfgAHBClk;
  ClockNodeType           *pQDSSDapAHBClk;
  ClockNodeType           *pQDSSEtrUSBClk;
  ClockNodeType           *pQDSSRBCPRClk;
  ClockNodeType           *pQDSSATClk;
  ClockNodeType           *pQDSSSysNocATClk;
  ClockNodeType           *pQDSSPCNocATClk;
  ClockNodeType           *pQDSSTraceClk;
  ClockNodeType           *pQDSSSTMClk;
  ClockNodeType           *pQDSSSTMAXIClk;
  ClockNodeType           *pQDSSDapClk;
  ClockNodeType           *pQDSSTSCTRDiv2Clk;
  ClockNodeType           *pQDSSTSCTRDiv3Clk;
  ClockNodeType           *pQDSSTSCTRDiv4Clk;
  ClockNodeType           *pQDSSTSCTRDiv8Clk;
  ClockNodeType           *pQDSSTSCTRDiv16Clk;
  ClockNodeType           *pQDSSRBCPRMXClk;
} ClockQDSSResourceType;

/*
 * ClockCEResourceType
 * Structure containing the Crypto Engine resource info
 */
typedef struct
{
  ClockResourceType      ClkCE;
  ClockNodeType         *pCEAXIClk;   // gcc_crypto_axi_clk
  ClockNodeType         *pCEAHBClk;   // gcc_crypto_ahb_clk
} ClockCEResourceType;



/*
 * Clock_NPAResourcesType
 *
 * Structure containing the NPA node and resource data for local nodes.
 */
typedef struct
{
  uint32                  eCXmicroVolts;
  NPAResourceType         CXONPANode;
  NPAResourceType         DCVSEnaNPANode;
  ClockResourceType       CPUClockResource;
  ClockResourceType       SNOCClockResource;
  ClockResourceType       PCNOCClockResource;
  ClockResourceType       BIMCClockResource;
  BIMCResourceType        BIMCResource;
  ClockResourceType       QPICClockResource;
  ClockQDSSResourceType   QDSSClockResource;
  ClockResourceType       IPAClockResource;
  ClockCEResourceType     CEClockResource;
  boolean                 bQPICEnable;
  ClockNodeType           *pQPICSysClk;
  ClockNodeType           *pQPICAHBClk;
  ClockNodeType           *pIPAAHBClk;
  ClockNodeType           *pIPASysAxiClk;
  ClockNodeType           *pIPASleepClk;
  ClockPowerDomainIdType  nIPAPowerDomainID;
  npa_client_handle 	  XOHandle;
  npa_client_handle 	  BIMCHandle;
} Clock_NPAResourcesType;


/*=========================================================================
      Prototypes
==========================================================================*/


/*===========================================================================

  FUNCTION      Clock_SetQDSSClocks

  DESCRIPTION   This QDSS function is used for toggle the QDSS clocks ON/OFF

  PARAMETERS    nCurrentLevel - Current QDSS level
                nNewLevel - New QDSS level

  DEPENDENCIES  None.

  RETURN VALUE  None

  SIDE EFFECTS  None.

===========================================================================*/
void Clock_SetQDSSClocks
(
  ClockQDSSResourceType *pQDSSResource,
  ClockQDSSSpeedType eSpeed
);


/*===========================================================================

  FUNCTION      Clock_ChangeQDSSLevel

  DESCRIPTION   This function is used for change QDSS clock to new Level

  PARAMETERS    eCurLevel - (0=Off, 1=Debug, 2=High Speed)
                eNewLevel - (0=Off, 1=Debug, 2=High Speed)

  DEPENDENCIES  None.

  RETURN VALUE  New Level.

  SIDE EFFECTS  None.

===========================================================================*/
ClockQDSSLevelType Clock_ChangeQDSSLevel
(
  ClockQDSSResourceType *pQDSSResource,
  ClockQDSSLevelType    eCurLevel,
  ClockQDSSLevelType    eNewLevel
);


/*===========================================================================

  FUNCTION      Clock_InitNPA

  DESCRIPTION   This function is used setup NPA node to all the clock
                resource
                  
  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None

  SIDE EFFECTS  None.

===========================================================================*/
DALResult Clock_InitNPA
(
  void
);


/*===========================================================================

  FUNCTION      Clock_GetNPAResources

  DESCRIPTION   This function is for getting the point to all clocks NPA node
                  
  PARAMETERS    None

  DEPENDENCIES  None.

  RETURN VALUE  Pointer to all clocks NPA resource

  SIDE EFFECTS  None.

===========================================================================*/
Clock_NPAResourcesType* Clock_GetNPAResources
(
  void
);


/*===========================================================================

  FUNCTION      Clock_SetClockSpeed

  DESCRIPTION   This function is for setting the clock speed
                  
  PARAMETERS    pClockResource [in] - Clock to be change
                nFreqHz [in]        - New clock speed

  DEPENDENCIES  None.

  RETURN VALUE  New clock speed

  SIDE EFFECTS  None.

===========================================================================*/
npa_resource_state Clock_SetClockSpeed
(
  ClockResourceType   *pClockResource,
  uint32              nFreqHz
);

#endif /* #ifndef CLOCKRPMNPA_H */
