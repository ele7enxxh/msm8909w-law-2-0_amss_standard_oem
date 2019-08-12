#ifndef TM_OSYSINIT_H
#define TM_OSYSINIT_H
/*===========================================================================
                          TM_OSYSINIT.H

DESCRIPTION
  This header file contains function prototypes for the functions in 
  tm_osysinit.c.

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_common/inc/tm_osysinit.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $


===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

#include "uecomdef.h"
#include "rtxMemory.h"
#include "rtContext.h"
#include "rtxError.h"
#include "rtxPrintStream.h"

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "tm_asn1_common.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define NUM_PROTOS 5

/*! @brief OSYS PDU Types */
#define   	tm_osys_SUPL_Message_PDU 		1
#define     tm_osys_RRLP_Message_PDU 	2

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                          Macro Definitions
** ======================================================================= */


/* =======================================================================
**                        Function Declarations
** ======================================================================= */


void tm_osys_init( void );

#endif /* TM_OSYSINIT_H */
