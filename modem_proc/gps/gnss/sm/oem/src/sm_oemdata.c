/*======================================================================

               OEM Data main file

 GENERAL DESCRIPTION
  This file contains the implementation of OEM Data (PRM/PPM/SA/AA)

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/oem/src/sm_oemdata.c#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/11   rh      Initial version: provide PRM/PPM/SA/AA data in OEM layer
======================================================================*/

#include "sm_oemdata.h"


/****************** Variables ************************************************/

/****************** Function Definitions**************************************/

/*
 ******************************************************************************
 * oemData_ProcessData
 *
 *   OEM should process the OEM data inside this function
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
 */
void oemData_ProcessData(const oemData_data* pOemData)
{
	if(NULL == pOemData)
	{
		return;
	}

	switch(pOemData->oemDataType)
	{
      case OEMDATA_PPM_DATA:
        // OEM to use pOemData->u.oemPPMData;
        break;

      case OEMDATA_PRM_DATA:
        // OEM to use pOemData->u.oemPRMData;
        break;

      case OEMDATA_SENSTIVITY_ASSIST_DATA:
        // OEM to use pOemData->u.oemSAData;
        break;

      case OEMDATA_ACQUISITION_ASSIST_DATA:
        // OEM to use pOemData->u.oemAAData;
        break;

	  default:
	    break;
	}

} /*oemData_ProcessData() ends*/


/*
 ******************************************************************************
 * oemData_SendDataType
 *
 *    Send data to OEM layer (will call oemData_ProcessData internally)
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
 */
boolean oemData_SendDataType(oemData_dataType oemDataType, const void *oem_data)
{
  boolean bRet = TRUE;
  oemData_data pOemData;

  // Set data type
  pOemData.oemDataType = oemDataType;
  switch(oemDataType)
  {
  case OEMDATA_PPM_DATA:
    memscpy((void *)&(pOemData.u.oemPPMData), sizeof(pOemData.u.oemPPMData), oem_data, sizeof(pOemData.u.oemPPMData));
    break;

  case OEMDATA_PRM_DATA:
    memscpy((void *)&(pOemData.u.oemPRMData), sizeof(pOemData.u.oemPRMData), oem_data, sizeof(pOemData.u.oemPRMData));
    break;

  case OEMDATA_SENSTIVITY_ASSIST_DATA:
    memscpy((void *)&(pOemData.u.oemSAData), sizeof(pOemData.u.oemSAData), oem_data, sizeof(pOemData.u.oemSAData));
    break;

  case OEMDATA_ACQUISITION_ASSIST_DATA:
    memscpy((void *)&(pOemData.u.oemAAData), sizeof(pOemData.u.oemAAData), oem_data, sizeof(pOemData.u.oemAAData));
    break;

  default:
    bRet = FALSE;
    break;
  }

  if (bRet == TRUE)
  {
    // Send this to OEM to process 
    oemData_ProcessData(&pOemData);
  }

  return bRet;
}

