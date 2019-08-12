/*======================================================================

               OEM DRE main file

 GENERAL DESCRIPTION
  This file contains the implementation of OEM DR Enablement.
  This task enables OEMs to access following positioning information
  provided from the SM layer of GPS software (on modem processor).

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/oem/src/sm_oemdre.c#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/07/13   rh      Removed NO_FIX report handling (now BestAvailPos)  
02/10/10   vp      Initial version.
======================================================================*/

#include "sm_oemdre.h"
#include "sm_log.h"

/****************** Variables ************************************************/

/****************** Function Definitions**************************************/

/*
 ******************************************************************************
 * oemdreData
 *
 * Function description: Gets data from SM and makes it available for OEMs
 *
 * 1.	Position Fixes (Intermediate and Final position fixes coming from PE only)
 * 2.	Position Measurement Reports (1 Hz position measurement reports from ME)
 * 3.	SV Polynomials (from PE, 100 sec interval)
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
 */
void oemdreData(const oemdre_data* pOemdreData)
{
	if(NULL == pOemdreData)
	{
		return;
	}

	//Populate DM logs
	switch(pOemdreData->oemdreDataType)
	{
		case OEMDRE_MEAS:
		sm_log_oemdre_measurement_report(&(pOemdreData->u.oemdreMeas));
		break;

		case OEMDRE_POS_FIX:
		sm_log_oemdre_position_fix_report(&(pOemdreData->u.oemdrePosFix));
		break;

		case OEMDRE_SVPOLY:
		sm_log_oemdre_sv_poly_report(&(pOemdreData->u.oemdreSvPoly));
		break;

	    default:
	      break;

	}

	// OEM to put code their here

} /*oemdreData() ends*/
