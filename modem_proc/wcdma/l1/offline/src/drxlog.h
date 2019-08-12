#ifndef DRXLOG_H
#define DRXLOG_H

/*===========================================================================
                              D R X L O G . H

DESCRIPTION
  This header file describes the DRX log packet definitions

Copyright (c) (2002-2008) by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/drxlog.h_v   1.1   16 Dec 2001 18:19:38   arao  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/drxlog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
05/23/13    geg     WSW decoupling (except for component of QCHAT)
09/22/09    sup     Removed log code assignments and placed them in
                    log_codes_wcdma_int_v.h
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
05/15/03    scm     Replace old DRX log item 0x4107 with new, smaller 0x4004.
12/14/01    asr     Added pich_pwr_offset
12/12/01    asr     Created this file

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "qw.h"
#include "comdef.h"
#include "log.h"
#include "log_codes_wcdma_int.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Log header type declaration */
typedef PACKED struct PACKED_POST
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
} drx_log_hdr_struct_type;

/* DRX information */
typedef PACKED struct PACKED_POST
{
  /* which drx cycle are we logging for; 0,1,2,... */
  uint16 drx_cycle_num;
  
  /* decision whether PICH demodlation indicated paged (1)/not-paged (0) */
  uint8  pi_bit;
  
  /* mdsp accumulated value of the PICH bits */
  int32 pi_sum;

} drx_log_info_struct_type;


/* DRX parameters log packet.
   Log Packet Id: 0x4004 */
LOG_RECORD_DEFINE(DRX_LOG_PKT)
  /* DRX procedure summary */
  drx_log_info_struct_type drx_log_info;
LOG_RECORD_END

#define DRX_LOG_PKT_SIZE() (sizeof(DRX_LOG_PKT_type))


#endif //DRXLOG_H

