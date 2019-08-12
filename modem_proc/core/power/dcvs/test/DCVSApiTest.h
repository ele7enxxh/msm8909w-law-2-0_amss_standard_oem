#ifndef DCVSAPITEST_H
#define DCVSAPITEST_H
/*============================================================================
  @file DCVSApiTest.h

  ULogDevelopment - The public test functions for testing DCVS components.  

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/test/DCVSApiTest.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------
 * Type Declarationsno,
 * -------------------------------------------------------------------------*/

/* Structure to maintain the state related to DCVS testing */
typedef struct
{
  /* Max frequency and mips supported on the target */
  int32 maxFreq;
  int32 maxMips;

}DCVS_TEST_STRUCT;

static DCVS_TEST_STRUCT dcvsTest;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
  @brief DCVSTestApiInitialize
  
  This function is used to initialize the components used for 
  DCVS testing. This function needs to be called before the 
  DCVS testing is started . 

  @param  : None

  @return : void.

*/
void DCVSTestApiInitialize( void );


/**
  @brief CPUSimHandleReturn
  
  This function is used to return the handle to the current CPU PC simulator.

  @param  handle1: This handle is filled in with the PC CPU simulator handle.

  @return : void.

*/
void* CPUSimHandleReturn(void);


#endif // DCVSAPITEST_H
