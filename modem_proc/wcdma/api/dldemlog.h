#ifndef L1LOG_H
#define L1LOG_H

/*===========================================================================
                             D L D E M L O G . H

DESCRIPTION
This file contains or includes files that contain Layer 1 demod log packet
structure definitions, prototypes for Layer 1 logging functions, any
constant definitions that are needed, and any external variable declarations
needed for Layer 1 logging.


    Copyright (c) 2001 - 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dldemlog.h_v   1.10   12 Jun 2002 13:28:18   yshi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/dldemlog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
12/20/11    at      Changed hdet to int16 in agc_sample_struct_type
04/25/11    geg     Separated dldemlog.h into dldemlog.h and dldemlog_v.h
04/25/11    geg     Moved this file to /wcdma/api from /l1/inc
02/26/10    rvs     Fixes for Mustang.
11/24/09    hk      TA and AGC Log packet changes for DC
09/22/09    sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int.h
07/14/09    sv      Corrected the EUL bit fields for use in the TA log packet.
05/07/09    vc      Fixed the apfAlpha size in TA packet.
05/01/09    vc      Added new entries ttlstepsize,apfAlpha in the TA log packet.
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
07/22/08    vc      Added the TA log packet (0x4186) and cell status (0x4187) log packet definitions.
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
10/24/07    vsr     Mainlining MBMS and Enhanced DL changes 
09/06/07    gnk     Added support for combiner log packet when FEATURE_WCDMA_DL_ENHANCED
                    is not defined
10/17/06    bd      Added support for EUL finger info logging
08/30/06    rgn     Added log code for wcdma rxd ant stats log packet
07/07/06    vb      modified the TA and AGC log code to new codes
06/29/06    bbd     Updated TA and AGC log packet featurization.
05/16/06    bbd     Fixed featurization problem from previous change.
05/01/06    bbd     Added defintion for new TA log packet.
04/11/06    asm     Added definition for new AGC log packet 0x4728
08/19/05    mc      Changed the per path/finger info struct and the 
                    definition of the TA log packet for feature
                    FEATURE_WCDMA_L1_TA_INFO_LOG_CODE_4006.
03/10/05    asm     Added start_cfn field to AGC log packet 0x4165
03/08/05    gs      Corrected macro WCDMA_FING_HS_LOG_INFO_SET_FIELD
02/26/05    gs      Added prenthesis around macro argument for macro
                    WCDMA_FING_HS_LOG_INFO_SET_FIELD to make it safe
02/25/05    asm     Added definition for new AGC log packet 0x4165
12/28/04    gs      Changed WCDMA_FINGER_TA_INFO_LOG_PACKET to
                    LOG_WCDMA_BASE_C + 0x202. it's defined under
                    WCDMA_TA_INFO_LOG_CODE_4202 when FEATURE_HSDPA is defined.
                    Added finger HS info to TA log packet
02/05/03    yus     Changed WCDMA_FINGER_TA_INFO_LOG_PACKET to
                    LOG_WCDMA_BASE_C + 0x003. it's defined under
                    WCDMA_TA_INFO_LOG_CODE_4003.
06/12/02    yus     Changed eng of per_path_info_struct_type to uint32
05/17/02    yus     1) Changed WCDMA_FINGER_TA_INFO_LOG_PACKET to
                    LOG_WCDMA_BASE_C + 0x147.
                    2) Add tx_timing, nc and nn in TA log packet.
03/13/02    yus     Changed typedef of rot from uint16 to int16 in structure of
                    per_finger_info_struct_type.
02/22/02    cpe     Removed the WCDMA_SET_REF_POS_LOG_PACKET packet as the contents
                    of this packet have been merged with ASET and NSET packets
02/19/02     eh     Added rx_agc_adj_pdm and tx_agc_adj_pdm field in the WCDMA AGC 
                    LOG packet.
01/22/2002  asr     removed WCDMA_DRX_MODE_LOG_PKT (DRX logging defined in drxlog.h)
09/24/2001  mdl     removed lna_gain_adj from AGC packet
09/21/2001  mdl     updated log code offsets
08/30/2001  mdl     changed finger and path ecio to energies and changed them
                    from a uint8 to a uint16.
                    changed secondary scrambling code fields to a uint8.
                    changed S-CPICH ovsf code to a uint8.
                    Modified a comment regarding use of the finger index field
                    in the TA packet.
4/20/2001   mdl     added TA, AGC, Cell Ref, and DRX packets, changed from
                    absolute use of the 0x4100 packet index range to relative
                    use of the LOG_WCDMA_L1_BASE_C range.

01/09/2001  mdl     initial cut
===========================================================================*/

#include "comdef.h"
#include "msm.h"
#include "log.h"
#include "log_codes_wcdma_int.h"

/*--------------------------------------------------------------------------
                      LOG_PACKET: WCDMA_AGC_LOG_PACKET

This struct is used for storing and logging AGC information samples.
There is room for 25 individual samples.
--------------------------------------------------------------------------*/
/* the log code for the AGC log packet */
#define WCDMA_AGC_LOG_PACKET WCDMA_AGC_0x4176_LOG_PACKET

#define WCDMA_AGC_PKT_MAX_SAMPLES 50

  /* an array of structs representing each AGC sample */
typedef PACKED struct PACKED_POST {
  
  uint16 amp_flags;

  /* rx_agc carr info*/
  int16 rx_agc_carr0_chain0;
  int16 rx_agc_carr0_chain1;
  int16 rx_agc_carr1_chain0;
  int16 rx_agc_carr1_chain1;

  /* the TX AGC value */
  int16 tx_agc;
  /* the TX AGC ADJ PDM value */
  int16 tx_agc_adj_pdm;
  /* Max Tf value*/
  int16 tx_pwr_lim;
  /* HDET- internal use only */
  int16 hdet;

  /* internal use only */
  int16 switch_point_jammer;
  int16 switch_point_no_jammer;
} agc_sample_struct_type;


LOG_RECORD_DEFINE(WCDMA_AGC_LOG_PACKET)
/* Packet version num */
  uint8 version;
  /* the number of AGC samples accumulated in this packet */
  uint8 num_samp;
  uint8 start_cfn; 
  /* array of AGC samples */
  agc_sample_struct_type agc_samp[WCDMA_AGC_PKT_MAX_SAMPLES];

LOG_RECORD_END

#endif
