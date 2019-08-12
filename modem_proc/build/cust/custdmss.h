#ifndef CUSTDMSS_H
#define CUSTDMSS_H
/*===========================================================================

DESCRIPTION
  Configuration for general DMSS features

  Copyright (c) 2002  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/cust/custdmss.h#1 $ $DateTime: 2016/12/13 07:59:25 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/02/05   avm     Disable Feature FEATURE_QCAMERA_TEST_AUTOMATION needed to enable qpst screen capture
02/27/03   wd      Enable FTM Asynchronous packets
05/01/02   jct     Created
===========================================================================*/
#ifdef FEATURE_GSM

/* Customer includes for GSM - preserve inclusion order!! */
//kting TEMP bringup #include "custgsm.h"
#include "custgsmdrv.h"
//#include "custgsm6500.h"
//kting TEMP bringup #include "custruim.h" 

#endif /* FEATURE_GSM */
/* EVRC service option supported
*/
//top level feature removed: #define FEATURE_EVRC

/* Enable Factory Test Mode
*/
//#define FEATURE_FACTORY_TESTMODE -abp 

/* Enable RF-Cal via Factory Test Mode
*/
#define FEATURE_FTM_RF

/* Enable FTM Task
*/
//top level feature removed: #define FEATURE_FTM_TASK

/* Enable FTM Asynchronous packets
*/
//#define FEATURE_FTM_ASYNC_PKT -abp

/* Put 8 bits and 16 bits shadow registers in one array.
*/
//top level feature removed: #define FEATURE_16BIT_SHADOW_REG

/* Search generated clock uptime to prevent losing time - affects deep sleep
*/
#define T_ALT_UPTIME

/* Define features of interest in field test and validation
*/
#undef  TMOBILE

/* Tell MC to use clk_uptime() for some of its timers.
*/
//top level feature removed: #define FEATURE_MC_TIMER_FIX

/*  Display register contents on Arm Mode and Reentrant Data Aborts
*/
#undef FEATURE_BOOT_DATA_ABORT_DEBUG

/* Enable GPIO group interrupt
*/
//top level feature removed: #define FEATURE_GPIO_GROUP_INT

/* Removes the reference of self pointer and queue pointer in Queue elements.
** Saves 8 Bytes of RAM per queue element.
*/
//top level feature removed: #define FEATURE_Q_NO_SELF_QPTR

/* Links queue elements as Singly-link list. Saves 4 Bytes of RAM per queue element.
*/
//top level feature removed: #define FEATURE_Q_SINGLE_LINK

/* Fast Interrupt
*/
//top level feature removed: #define FEATURE_INT_125_FIQ

/* Dynamic Clock Switching
*/
#define FEATURE_DYNAMIC_CLOCK_SWITCHING

/* Test Automation
*/
#define FEATURE_PORTMAP_TEST_AUTOMATION
/* #define FEATURE_MEDIAPLAYER_TEST_AUTOMATION */
#define FEATURE_AUDIOSETTINGS_TEST_AUTOMATION

/* XML library
 */
#define FEATURE_XMLLIB

#endif /* CUSTDMSS_H */

