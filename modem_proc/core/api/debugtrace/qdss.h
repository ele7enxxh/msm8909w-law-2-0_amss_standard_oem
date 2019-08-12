/*=========================================================================
                       QDSS PUBLIC API

GENERAL DESCRIPTION 
   The APIs expose minimal functionalies to initialize and check status of the 
   QDSS tracer framework.
    
 
EXTERNALIZED FUNCTIONS
   QDSSInit
   QDSSisEnabled
   QDSSFlush
 
 
INITIALIZATION AND SEQUENCING REQUIREMENTS 
   QDSSInit is always invoked in ALL builds where QDSS peripheral is present 
regardless of the efuse setting which might disable run time access to QDSS. 
QDSSOn and QDSSOff needs to be explicitly invoked by tracer to enable QDSS 
clocks.

      Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*========================================================================== 
 $Header: //components/rel/core.mpss/3.5.c12.3/api/debugtrace/qdss.h#1 $
==========================================================================*/  

#include "comdef.h"

/**
Input: None

Description:
   Initializes QDSS drivers and places QDSS software state in
   the READY state. However, QDSS clocks are not turned ON
   unless this API is invoked by a client.

Return: None
**/
void QDSSInit(void);


/**
Input:None

Description:
   Checks if QDSS is in IN_USE state. The IN_USE state indicates
   whether QDSS clocks are turned on/off.
Return: 
   TRUE -  QDSS clocks are enabled and there are tracer clients using QDSS.
   FALSE - QDSS clocks are disabled and no tracer client is using QDSS.
**/
boolean QDSSisInUse(void);



/**
Input: None

Description:
   Flushes pending trace data out of buffers in various QDSS components and 
   drains them into the QDSS sink.

Return: None
**/
extern void QDSSFlush(void);

/**
Input: None

Description:
   Request to turn ON QDSS clocks. The request can be denied if QDSS peripheral 
   is inaccessible based on fuse settings or due to a software override. If successful 
   the QDSS software state is placed in IN_USE state. A ON request in the IN_USE state also 
   returns SUCCESS.
   
Return:
   0: SUCCESS - request was accepted
  -1: FAILURE - request denied
**/
int32 QDSSOn(void);

/**
Input: None

Description:
   Request to turn OFF QDSS clocks. If successful, the QDSS software state is placed in READY.
   A OFF request in the READY state returns a SUCCESS(0).
   
Return:
   0: SUCCESS - request was accepted
  -1: FAILURE - request denied
**/
int32 QDSSOff(void);
