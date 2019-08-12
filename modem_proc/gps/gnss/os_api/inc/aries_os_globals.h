/*
 * Copyright (c) 2013 Qualcomm Atheros, Inc..
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Operating System API Data Header File

GENERAL DESCRIPTION
  This file declares all the global data for any tasks that use the OS API.
  This file should be visible to modules outside of this API and included
  whenever the OS API header file is included.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/os_api/inc/aries_os_globals.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $ 

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/15   dc      Added Thread ID for WLM (Wifi Location Manager)
04/17/14   skm     NV Async Read changes  
10/25/12  mikavesa Added Thread ID for ESP (Ephemeris Self Prediction)
04/06/11   spn     Changes for Loc-API 2.0
05/29/07    jd     Added enumeration for GPS FS thread.
12/04/06    ah     Added NV write command completion Msg ID value.
04/10/06    ah     Changed MSG_ID_TIMER_ELAPSED to C_OS_MSG_ID_TIMER_EXPIRY.
03/27/06    ah     Defined out OS Test related tasks (use OSTEST_LEAN_BUILD).
03/23/06    ah     Initial creation of file for AMSS.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef OS_GLOBALS_H
#define OS_GLOBALS_H

/*
 * Include files
*/

/*
 * Constant definitions
*/

typedef enum
{
  /* NOTE - thread ID numbers cannot change, see below. */
  THREAD_ID_ERROR,
  THREAD_ID_START         =  1,
  THREAD_ID_MGP_MC = THREAD_ID_START,
  THREAD_ID_MGP_CC        =  2,
  THREAD_ID_MGP_PP        =  3,
  THREAD_ID_MGP_GPSFFT    =  4,
  THREAD_ID_MGP_NF        =  5,
  THREAD_ID_MGP_CD        =  6,
  THREAD_ID_SM_LM         =  7,
  THREAD_ID_SM_TM         =  8,
  THREAD_ID_SM_PDCOMM_TCP =  9,
  THREAD_ID_PDCOMM_TD1    = 10,  
  THREAD_ID_GPSDIAG       = 11,
  THREAD_ID_SM_PDCOMM_WMS = 12,    
  THREAD_ID_GPS_FS        = 13,
  THREAD_ID_MGP_PGI       = 14,
  THREAD_ID_LOC_MIDDLEWARE= 15,
  THREAD_ID_LBS           = 16,
  THREAD_ID_TLM           = 17,
  THREAD_ID_XTM           = 18,
  THREAD_ID_SDP           = 19,
  THREAD_ID_SM_GM         = 20,
  THREAD_ID_WWANME_CONTROL = 21,
  THREAD_ID_WWANME_MEASPROC = 22,
  THREAD_ID_GNSS_MSGR     = 23,
  THREAD_ID_ESP           = 24,
  THREAD_ID_WLM           = 25,
  /* NOTE: New Thread ID's must be added at the end (here) and then you 
     need to update Thread_ID_Last below.  
     Other functionality including AriesPePlayback, & Apex IPC
     log parsing rely on these values not changing.  - WTR */
  THREAD_ID_LAST = THREAD_ID_WLM,
  THREAD_ID_MAX
} os_ThreadIdEnumType;

typedef enum
{
  /* RESERVED: All OS-related Msg Ids (prefix C_OS_MSG_ID_) defined here */

  C_OS_MSG_ID_FIRST,                        /* First OS message */
  C_OS_MSG_ID_TIMER = C_OS_MSG_ID_FIRST,    /* Timer (generic) message */
  C_OS_MSG_ID_TIMER_EXPIRY,                 /* Timer expiration message */
  C_OS_MSG_ID_DOG_RPT,                      /* Dog Report Timer message */
  C_OS_MSG_ID_START,                        /* TMC start message */
  C_OS_MSG_ID_STOP,                         /* TMC stop message */
  C_OS_MSG_ID_OFFLINE,                      /* TMC offline message */
  C_OS_MSG_ID_NV_WRITE,                     /* NV write complete message */
  C_OS_MSG_ID_NV_RDRW  = C_OS_MSG_ID_NV_WRITE, /* NV read/write complete message, SM_TM */
  C_OS_MSG_ID_QMI_EVENT,
  C_OS_MSG_ID_MSGR_MSG,                     /* MSG router message */
  C_OS_MSG_ID_DOG_HB,                       /* Dog HB message */
  C_OS_MSG_ID_LAST = C_OS_MSG_ID_DOG_HB,    /* Last OS message */
  C_USR_MSG_ID_FIRST = 10,                  /* User IPC message IDs shall
                                             * not collapse into OS ones 
                                             */
  /* As of now , it is not allowed to add any new IDs in this enum since it will break PE playback and Apex analyzer */
  /* All user-defined Msg Ids should be enumerated within user module itself,
   * where the first Msg Id is set to C_USR_MSG_ID_FIRST. */
  
  MSG_ID_MAX
} os_MsgIdEnumType;

/*
 * Structure definitions
*/

/*
 * Function definitions
*/

#endif /* OS_GLOBALS_H */
