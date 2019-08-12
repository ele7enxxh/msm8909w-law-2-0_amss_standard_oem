/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Diagnostic Packet Definitions for Aries GPS

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes which 
  define the interface between the diagnostic module and the Aries GPS.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/diag/src/aries_gpsdiag_defs.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $ 

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/18/06    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef GPS_DIAG_DEFS_H
#define GPS_DIAG_DEFS_H

/*
 * Constant definitions
*/

/* NOTE: THESE DEFINES ARE USED TO ENABLE/DISABLE ALL CODE THAT IS FOUND
 * WITHIN THIS GPSDIAG MODULE. PLEASE WHEN ADDING ADDITIONAL DEFINES, TO
 * BE SURE TO DEFINE THEM HERE.
 */

/* Enable PDAPI client implementation */
#define INCLUDE_PDAPI_TEST       1

/* Enable Application Tracking functionality */
#define INCLUDE_APP_TRACKING     1

/* Enable LCS Commands functionality */
#define INCLUDE_LCS_CMDS         1

#endif /* GPS_DIAG_DEFS_H */
