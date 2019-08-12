#ifndef TDSDLCCTRCHLOG_H
#define TDSDLCCTRCHLOG_H

/*===========================================================================
                     D L C C T R C H L O G . H

DESCRIPTION
  This header file describes the CcTrCh log packet definitions

Copyright (c) 2001-2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dlcctrchlog.h_v   1.9.1.0   18 Jun 2002 17:54:56   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdlcctrchlog.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
       
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "qw.h"
#include "log.h"
#include "tdsl1rrcif.h"
//#include "tdsdec.h"
// zdang_CR #include "tdsdltri.h"
//#include "log_codes_tds_int.h"
#include "tdssrchset.h"  // zdang_CR add include header file

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
 
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Header definition since the header definition is defined in log.c */
typedef  struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} tdsdlcctrchlog_l1_log_hdr_struct_type;

#endif

