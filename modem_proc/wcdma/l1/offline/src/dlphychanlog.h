#ifndef DLPHYCHANLOG_H
#define DLPHYCHANLOG_H

/*===========================================================================
                       D L P H Y C H A N L O G . H


DESCRIPTION
This file contains structures for the logging of physical channel related
information.


    Copyright (c) 1999, 2000, 2001, 2008
                    by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dlphychanlog.h_v   1.9   31 May 2002 13:43:24   arao  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlphychanlog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
05/23/13    geg     WSW decoupling (except for component of QCHAT)
09/22/09    sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int_v.h
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
05/29/2002  m       Added new structure types to support LOD
                    1. dl_common_phychan_log_on_demand_struct_type
                    2. dl_ded_phychan_log_on_demand_struct_type
09/21/2001  mdl     changed the size of a few fields.  Updated the log code
04/20/2001  mdl     Removed SF from dedicated channel log packet, changed
                    Common channel log packet to one channel per packet vice
                    multiple channels per packet.

01/15/2001  mdl     initial cut
===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "log.h"
#include "log_codes.h"
#include "l1dlphychancfg.h"
#include "log_codes_wcdma_int.h"

/*--------------------------------------------------------------------------
         LOG RECORD: WCDMA Common Physical Channel Downlink (0x??0A)

--------------------------------------------------------------------------*/

LOG_RECORD_DEFINE(WCDMA_COMMON_DL_PHYCHAN_LOG_PACKET)
  /* whether the channel is being added or dropped. TRUE if the channel
     is being added, FALSE if it is being dropped */
  boolean active;

  /* the type of physical channel */
  uint8 phch_type;

  /* the slot format of the physical channel.  Only applies to the
     SCCPCH */
  uint8 slot_fmt;

  /* the scrambling code for the channel.  If it happens to be the same
   as the cell scrambling code then it is the Primary scrambling code.
   Otherwise it is a secondary scrambling code. */
  uint16 scr_code;

  /* STTD usage on the physical channel.  TRUE if STTD is used */
  boolean sttd_use;

  /* The time offset of the channel.  The PCCPCH and AICH have offsets of
     0.  The SCCPCH has a variable offset.  The PICH has an offset that
     is 3 slots prior to its associated SCCPCH */
  uint8 offset;

  /* the OVSF code the channel is using.  This can be a uint8 as the
     common physical channels have a maximum OVSF of 255. */
  uint8 ovsf;

LOG_RECORD_END


#define COMMON_DL_PHYCHAN_LOG_PKT_SIZE() \
  (sizeof(WCDMA_COMMON_DL_PHYCHAN_LOG_PACKET_type))

#ifdef FEATURE_L1_LOG_ON_DEMAND
typedef struct
{
  /* snapshot of common dl trCH setup parameters */
  WCDMA_COMMON_DL_PHYCHAN_LOG_PACKET_type channel_log;

  /* flag to determine snapshot has atleast one valid value */
  boolean                                 valid;
} dl_common_phychan_log_on_demand_struct_type;
#endif

/*--------------------------------------------------------------------------
         LOG RECORD: WCDMA Dedicated Physical Channel Downlink (0x??0B)

--------------------------------------------------------------------------*/


typedef PACKED struct PACKED_POST {
  /* the secondary scrambling code.  0 if the primary is used */
  uint8 sec_scr_code;

  /* the OVSF code.  This needs to be a uint16 because if we ever support
     SF512 the maximum OVSF code will be 511. */
  uint16 ovsf;

} per_dpch_info_struct_type;

typedef PACKED struct PACKED_POST {
  /* The timing offset used by this radio link for the DPCH */
  uint8 offset;

  /* the primary scrambling code of the radio link */
  uint16 pri_scr_code;

  /* which pilot CPICH, primary or secondary, is used for channel
     estimation */
  boolean use_pri_cpich;

  /* the secondary CPICH scrambling code.  0 if the primary CPICH is used */
  uint8 sec_cpich_scr_code;

  /* the secondary CPICH channelisation code. 0 if the primary CPICH is
     used */
  uint8 sec_cpich_ch_code;

  /* the TPC index */
  uint8 tpc;

  /* how many DPCH multicode channels this radio link is transmitting */
  uint8 num_dpch;

  /* array of structures for per DPCH info */
  per_dpch_info_struct_type dpch[MAX_DPCH];

} per_rl_info_struct_type;

LOG_RECORD_DEFINE(WCDMA_DED_DL_PHYCHAN_LOG_PACKET)
  /* TRUE if STTD is being used for this channel */
  boolean sttd_present;

  /* whether the channel is being added or dropped.  TRUE if being added,
     FALSE if being dropped */
  boolean active;

  /* the common slot format */
  uint8 slot_fmt;

  /* number of radio links */
  uint8 num_rl;

  /* array of per radio link information structures */
  per_rl_info_struct_type rl[L1_MAX_RL];

LOG_RECORD_END



#define DED_DL_PHYCHAN_LOG_PKT_SIZE(num_rl) (         \
          2*sizeof(boolean) +                         \
          2*sizeof(uint8) +                           \
          (num_rl)*sizeof(per_rl_info_struct_type))

#ifdef FEATURE_L1_LOG_ON_DEMAND
typedef struct
{
  /* snapshot of ded dl trCH setup parameters */
  WCDMA_DED_DL_PHYCHAN_LOG_PACKET_type  channel_log;

  /* flag to determine snapshot has atleast one valid value */
  boolean                               valid;
} dl_ded_phychan_log_on_demand_struct_type;
#endif

#ifdef FEATURE_L1_LOG_ON_DEMAND
/* Header definition since the header definition is defined in log.c */
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
} l1_dl_log_hdr_struct_type;
#endif

#endif
