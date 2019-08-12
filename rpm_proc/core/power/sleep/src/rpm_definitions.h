#ifndef RPM_DEFINITIONS_H
#define RPM_DEFINITIONS_H
/*===========================================================================
                             rpm_definitions.h

SERVICES:

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the classes, macros, etc. defined in
  this file.

PUBLIC CLASSES AND FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "sleep_lpr.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

#define SLEEP_VDD_MIN_ENTER_LATENCY 0x6000
#define SLEEP_VDD_MIN_EXIT_LATENCY 0x9000

#define SLEEP_HALT_ENTER_LATENCY 0x100
#define SLEEP_HALT_EXIT_LATENCY 0x700

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
extern int    cx_id, mx_id;
boolean is_sleep_debug_enabled(void);

//===========================================================================
//                             Class Definitions
//===========================================================================


#endif
