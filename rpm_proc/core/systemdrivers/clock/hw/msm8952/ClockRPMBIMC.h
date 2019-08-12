#ifndef CLOCKRPMBIMC_H
#define CLOCKRPMBIMC_H
/*
===========================================================================
*/
/**
  @file ClockRPMBIMC.h
  
  BIMC definitions for the MSM8974 RPM clock driver.
*/
/*  
  ====================================================================

  Copyright (c) 2013 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/clock/hw/msm8952/ClockRPMBIMC.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/29/13   vph     Initial version
 
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/
#include <DALSys.h>
#include <railway.h>
#include <npa.h>
#include <npa_resource.h>


/*=========================================================================
      Macros
==========================================================================*/
/* BIMC settle time needs to wait for 100us */
#define BIMC_WAIT 100


/*=========================================================================
      Type Definitions
==========================================================================*/
extern const npa_resource_plugin npa_bimc_max_plugin;


/*=========================================================================
      Prototypes
==========================================================================*/

/*===========================================================================

  FUNCTION      Clock_BIMCSetFrequency

  DESCRIPTION   This function is used for changing BIMC frequency.
  
  PARAMETERS    pClockRsc [in]  - Clock Resource
                nNewClkKHz [in] - New clock speed in KHz
                bNoWait [in] - TRUE - No wait for BIMC to settle for restoring
                             - FALSE - Wait for BIMC to restore completely before use

  DEPENDENCIES  None.

  RETURN VALUE  New clock speed

  SIDE EFFECTS  None.

===========================================================================*/
uint32 Clock_BIMCSetFrequency
( 
  ClockResourceType *pClockRsc, 
  uint32            nNewClkKHz,
  boolean           bNoWait
);


/*===========================================================================

  FUNCTION      Clock_SetBIMCClockConfig

  DESCRIPTION   This function is for setting BIMC frequency.  Due to the
                dependency on DDR with voltage, this function is using for 
                switching the voltage first, call ddr_pre_clock_switch, then
                switch the clock.
                  
  PARAMETERS    pDomain    - Clock domain for setting voltage
                pNewConfig - New config frequency

  DEPENDENCIES  None.

  RETURN VALUE  Pointer to all clocks NPA resource

  SIDE EFFECTS  None.

===========================================================================*/
DALResult Clock_SetBIMCClockConfig
(
  ClockDomainNodeType *pDomain,
  ClockMuxConfigType  *pNewConfig
);

#endif /* #ifndef CLOCKRPMBIMC_H */
