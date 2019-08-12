#ifndef UECOMDEF_H
#define UECOMDEF_H
/*===========================================================================

                    UE Common Definitions

DESCRIPTION
  This file contains definitions for the Common type and constants required 
  by different modules that are UE (3GPP) specific.
  
  Copyright (c) 2000-2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/uecomdef.h_v   1.18   01 Nov 2001 13:40:02   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/uecomdef.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/21/14   ymu     Compilation warning fix 2
05/06/14   geg     Use safer versions of memcpy() and memmove()
06/18/12   mrg     L2 - MAC-I and HS-RACH code changes
03/21/12   geg     Making wcdamvsif.h independent of uecomdef.h. Precursor to moving uecomdef.h out of modem/api
03/19/12   geg     Merged from //source/qcom/qct/modem/api/wcdma/rel/nikel_1.0/uecomdef.h#1
07/21/10   mc      Added declaration of WCDMA critical section mutex
02/02/10   rmsd    Corrected de-featurization for Non-MBMS targets.
08/06/09   rm      Mainlined all features as part of CMI Phase 2-defeaturization effort.
05/29/09   kp      Added support for new SNOW3g security Algo & single cipher algo usage across 
                   access stratum layers.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
09/02/08   cnp     Changed the location of WCDMA defines for interrupt lock and task lock.
09/02/08   cnp     Replaced intlock/free with wcdma_intlock/free.
           cnp     Replaced tasklock/free with wcdma_tasklock/free.
           cnp     Replaced intlock/free_sav with wcdma_intlock/free_sav.
           cnp     Removed the # defines from L1utils.h and added here
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/21/07   da      Support for FEATURE_MBMS
10/08/07   da      Featurized macros UE_MAX_UL_NUM_TRBLK and UE_MAX_UL_TB_SIZE.  
                   These are used to disallow UL TFCS based on WL1 max limitations but
                   This is now going to be done at WL1\MAC under feature flag
                   FEATURE_LIMIT_TFCS_TO_ERAM_SIZE. 
01/31/07   vr      Support for 384k on 20 ms tti.Changed UE_MAX_UL_NUM_TRBLK and 
                   UE_MAX_UL_RLC_PDUS from 16 to 24
09/12/06   sm/da   Fix code review comments and general clean-up for REL6 and HSUPA
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE_WCDMA_HSUPA.
08/22/06   tkk     Added FEATURE_TWO_LC_PER_AM_ENTITY feature to turn ON
                   control channel & data channels only when needed.
08/21/06   tkk     Added SRB#5 support.
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE_REL5 while retaining the HSDPA specifc processing 
                   under FEATURE_HSDPA.
04/14/06   vr      Added new structure rlc_sdu_tx_fail_reason_e_type to indicate
                   failure reason in L2 Ack ind
03/15/06   ttl     Increase UE_MAX_DL_RLC_PDUS under HSDPA feature.
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
04/29/05   sm      Modified Macro definition of UE_MAX_UL_UM_CHANNEL and UE_MAX_DL_UM_CHANNEL (from 3 to 8)
04/06/05   da      Added new RB id for CTCH_RADIO_BEARER_ID and new mode UE_MODE_DL_CTCH,
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE_HSDPA
12/23/04   sm      Added macros for UE_MAX_NUM_OF_AM_ENTITIES_7 and
                   UE_MAX_NUM_OF_AM_ENTITIES_30
11/29/04   svk     Added UE_MAX_UL_TB_SIZE and UE_MAX_UL_NUM_TRBLK.
11/22/04   svk     For FEATURE_WCDMA_PRACH_SELECTION, defined UE_MAX_PRACH to 8.
02/03/04   ttl     Changed UE_MAX_USER_PLAN_AM_ENTITIES to be number of primary
                   PDP context + (number of 2ndary PDP context per primary PDP
                   context * number of primary PDP context).
01/23/04   ttl     Bounded Total number of RLC AM entities w/ the number of 
                   PDP context.
09/13/02   vn      Changed MAX_RB_MUX_OPTIONS to 2 as that is the 
                   number of MUX options that we are supporting.
11/01/01   rj      Modified MAX_TRCH value to 32 from 16.
09/24/01   kmp     Removed duplicate macro MAX_DL_CHAN_CODES_DPCH.
09/13/01   kmp     Change UE_MAX_RL from 2 to 6.
08/30/01   rj      Renamed UE_MODE_ACKNOWLEDGED_DATA_PLANE and  
                   UE_MODE_ACKNOWLEDGED_CONTROL_PLANE to UE_MODE_ACKNOWLEDGED_DATA
                   and UE_MODE_ACKNOWLEDGED_CONTROL since these does represent
                   data plane and control plane.
07/17/01   kmp     Updated the UE_MAX_SCCPCH from 1 to 16. The UE only supports
                   one active SCCPCH at any time, but can select this one from
                   up to 16 separate ones. Added UE_MAX_RL macro. Added 
                   UE_MAX_DPCH macro.
07/12/01   kmp     Added UE Capabilities.
03/30/01   kmp     Change the RB IDs for the Serving and Neighbor BCCHs and 
                   PCCH and BCCH-FACH. This is due to a change in the range
                   of RB IDs from 0-31 to 1-32.
                   Updated comments in the UE capabilities section of this file.
03/02/01   kmp     Changed MAX_ASC_PERSISTENCE to MAX_ASC_PERSIST and changed
                   the value from 7 to 6. Changed MAX_ASC_MAPPINGS to 
                   MAX_ASC_MAP. Added UE_MAX_PRACH for now. Change UE_MAX_SCCPCH
                   from 2 to 1. Updated comment for UE_MAX_CCTRCH indicating 
                   that only UE_MAX_CCTRCH/2 can be active at any given time,
                   with only 1 CCTrCH active in the uplink.
02/13/00   kmp     Added the following new structures: uecomdef_trch_type,
                   uecomdef_rlc_type, and uecomdef_cctrch_type.
02/06/01   ps      Added MAX_DED_LOGCHAN_PER_TRCH. This specifies the maximum
                   number of dedicated logical channels per transport channel
02/05/01   kmp     Updated the number of TM, UM, and AM channels which are 
                   supported. Added UE_MAX_AM_ENTITY.
01/30/01   rj      Added rlc_lc_id_type which will be used by RLC, MAC and RRC
                   to index the watermark pointer. Modified 
                   uecomdef_logch_mode_e_type. Added more comments and 
                   removed references to sections of spec.
01/25/01   rj      Moved Radio Bearer define constants from RRCLCM module to
                   this file.
01/23/01   kmp     Added a typedef for PDCP sequence number.
01/09/01   kmp     Updated to remove MAX_SCCPCH and MAX_CN_DOMAIN since 
                   there were conflicts with L1 and rrcmmif.h. These should
                   be fixed in L1 and rrcmmif.h.
                   Fixed a bug in UE_MAX_UL_LOGICAL_CHANNEL and 
                   UE_MAX_DL_LOGICAL_CHANNEL macros.
12/20/00   kmp     Fixed compiler warnings.
12/18/00   kmp     Updated with constants and ue definitions. Also updated
                   for one enumeration of Logical, Transport, and Physical
                   channels enums. Other layers may then use these values
                   or assign these values to ones defined locally.
12/01/00   kmp     Added uecomdef_status_e_type.
11/08/00   kmp     Initial Release.

===========================================================================*/


/*===================================================================
                     INCLUDE FILES FOR MODULE
=====================================================================*/
#include "comdef.h"
#include "sys.h"
#include "rex.h"
#include "wcdmamvsif.h"
#include "stringl.h"

/*====================== WCDMA MEMMOVE/MEMCPY ======================*/
#define WCDMA_MEMMOVE(dest, dest_size, src, src_size) { \
            memsmove(dest, dest_size, src, src_size); \
        }\

#define WCDMA_MEMCPY(dest, dest_size, src, src_size) { \
            memscpy(dest, dest_size, src, src_size); \
        }\

/*===================================================================
                    UE CAPABILITIES (per 25.331)
This needs to be potentially updated on a per release basis. This
section details the UE's capabilities for a given release. No TDD
information is covered here.
====================================================================*/
/* Indicates the release version of TS34.123-2, the Implementation 
   Conformance Statement (ICS) proforma specification that is 
   applicable for the UE.
*/ 
typedef enum
{
  UE_ICS_VERSION_R_99,
  NUM_ICS_VERSION
} uecomdef_ics_version_e_type;

/* PDCP Capabilities                                                */
/********************************************************************/

/* Max HC Context space */
#define MAX_HC_CONTEXT_SPACE_512            512
#define MAX_HC_CONTEXT_SPACE_1024          1024 
#define MAX_HC_CONTEXT_SPACE_2048          2048 
#define MAX_HC_CONTEXT_SPACE_4096          4096 
#define MAX_HC_CONTEXT_SPACE_8192          8192 
 
#define IPHC_MAX_HC_CONTEXT_SPACE 1024
#define ROHC_MAX_CONTEXT_SESSIONS 1024
#define ROHC_REVERSE_COMPRESSION_DEPTH 0

/* WCDMA defines for interrupt lock and task lock.                                           */
/********************************************************************/

/* Mutex for DOB handle */
extern rex_crit_sect_type wcdma_crit_sect;

#define WCDMA_INTLOCK() REX_ISR_LOCK(&wcdma_crit_sect)
#define WCDMA_INTFREE() REX_ISR_UNLOCK(&wcdma_crit_sect)
#define WCDMA_INTLOCK_SAV(sav) REX_ISR_LOCK(&wcdma_crit_sect)
#define WCDMA_INTFREE_SAV(sav) REX_ISR_UNLOCK(&wcdma_crit_sect)

#define WCDMA_TASKLOCK() TASKLOCK()
#define WCDMA_TASKFREE() TASKFREE()
#define WCDMA_FIQLOCK() FIQLOCK()
#define WCDMA_FIQFREE() FIQFREE()


/* RLC Capabilities                                                 */
/********************************************************************/

/* AM Buffer size constants, per 25.331 */
#define UE_TOTAL_RLC_AM_BUFFER_SIZE_2K       2000
#define UE_TOTAL_RLC_AM_BUFFER_SIZE_10K     10000
#define UE_TOTAL_RLC_AM_BUFFER_SIZE_50K     50000
#define UE_TOTAL_RLC_AM_BUFFER_SIZE_100K   100000
#define UE_TOTAL_RLC_AM_BUFFER_SIZE_150K   150000
#define UE_TOTAL_RLC_AM_BUFFER_SIZE_500K   500000
#define UE_TOTAL_RLC_AM_BUFFER_SIZE_1000K 1000000

/* AM window size constants, per 25.331 */
#define UE_MAX_RLC_AM_WINDOW_SIZE_2047       2047
#define UE_MAX_RLC_AM_WINDOW_SIZE_4095       4095

/* AM entities supported, per 25.331 */
#define UE_MAX_NUM_OF_AM_ENTITIES_3             3
#define UE_MAX_NUM_OF_AM_ENTITIES_4             4
#define UE_MAX_NUM_OF_AM_ENTITIES_5             5
#define UE_MAX_NUM_OF_AM_ENTITIES_6             6
#define UE_MAX_NUM_OF_AM_ENTITIES_7             7
#define UE_MAX_NUM_OF_AM_ENTITIES_8             8
#define UE_MAX_NUM_OF_AM_ENTITIES_9             9
#define UE_MAX_NUM_OF_AM_ENTITIES_16           16
#define UE_MAX_NUM_OF_AM_ENTITIES_30           30
#define UE_MAX_NUM_OF_AM_ENTITIES_32           32

#define UE_MAX_USER_PLAN_AM_ENTITIES   ( SYS_MAX_PRIMARY_PDP_CONTEXTS +   \
                                         ( SYS_MAX_PRIMARY_PDP_CONTEXTS * \
                                          SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY ) )
#define UE_MAX_SRB_AM_EMTITIES_3                3

/* Transport Channel Capabilities                                   */
/********************************************************************/

/* Max # of bits received */
#define UE_MAX_NUM_BITS_RCV_640               640
#define UE_MAX_NUM_BITS_RCV_1280             1280
#define UE_MAX_NUM_BITS_RCV_2560             2560
#define UE_MAX_NUM_BITS_RCV_3840             3840
#define UE_MAX_NUM_BITS_RCV_5120             5120
#define UE_MAX_NUM_BITS_RCV_6400             6400
#define UE_MAX_NUM_BITS_RCV_7680             7680
#define UE_MAX_NUM_BITS_RCV_8960             8960
#define UE_MAX_NUM_BITS_RCV_10240           10240
#define UE_MAX_NUM_BITS_RCV_20480           20480
#define UE_MAX_NUM_BITS_RCV_40960           40960
#define UE_MAX_NUM_BITS_RCV_81920           81920
#define UE_MAX_NUM_BITS_RCV_163840         163840

/* Max # of convolutionally coded bits received */
#define UE_MAX_CONV_CODE_BITS_RCV_640         640
#define UE_MAX_CONV_CODE_BITS_RCV_1280       1280
#define UE_MAX_CONV_CODE_BITS_RCV_2560       2560
#define UE_MAX_CONV_CODE_BITS_RCV_3840       3840
#define UE_MAX_CONV_CODE_BITS_RCV_5120       5120
#define UE_MAX_CONV_CODE_BITS_RCV_6400       6400
#define UE_MAX_CONV_CODE_BITS_RCV_7680       7680
#define UE_MAX_CONV_CODE_BITS_RCV_8960       8960
#define UE_MAX_CONV_CODE_BITS_RCV_10240     10240
#define UE_MAX_CONV_CODE_BITS_RCV_20480     20480
#define UE_MAX_CONV_CODE_BITS_RCV_40960     40960
#define UE_MAX_CONV_CODE_BITS_RCV_81920     81920
#define UE_MAX_CONV_CODE_BITS_RCV_163840   163840

/* Maximum # of simultaneous Transport Channels */
#define UE_MAX_NUM_TRCH_2                       2 /* UL Only */
#define UE_MAX_NUM_TRCH_4                       4
#define UE_MAX_NUM_TRCH_8                       8
#define UE_MAX_NUM_TRCH_16                     16
#define UE_MAX_NUM_TRCH_32                     32

/* Maximum # of simultaneous CCTrCH, DL: FDD & TDD, UL:TDD Only */
#define UE_MAX_NUM_CCTRCH_1                     1
#define UE_MAX_NUM_CCTRCH_2                     2
#define UE_MAX_NUM_CCTRCH_3                     3
#define UE_MAX_NUM_CCTRCH_4                     4
#define UE_MAX_NUM_CCTRCH_5                     5
#define UE_MAX_NUM_CCTRCH_6                     6
#define UE_MAX_NUM_CCTRCH_7                     7
#define UE_MAX_NUM_CCTRCH_8                     8

/* Max number of received transport blocks */
#define UE_MAX_NUM_RCV_TR_BLKS_2                2 /* UL Only */
#define UE_MAX_NUM_RCV_TR_BLKS_4                4
#define UE_MAX_NUM_RCV_TR_BLKS_8                8
#define UE_MAX_NUM_RCV_TR_BLKS_16              16
#define UE_MAX_NUM_RCV_TR_BLKS_32              32
#define UE_MAX_NUM_RCV_TR_BLKS_48              48
#define UE_MAX_NUM_RCV_TR_BLKS_64              64
#define UE_MAX_NUM_RCV_TR_BLKS_96              96
#define UE_MAX_NUM_RCV_TR_BLKS_128            128
#define UE_MAX_NUM_RCV_TR_BLKS_256            256
#define UE_MAX_NUM_RCV_TR_BLKS_512            512

/* Max number of Transport Format Combinations supported in the TFCS */
#define UE_MAX_NUM_TFC_4                        4 /* UL Only */
#define UE_MAX_NUM_TFC_8                        8 /* UL Only */
#define UE_MAX_NUM_TFC_16                      16
#define UE_MAX_NUM_TFC_32                      32
#define UE_MAX_NUM_TFC_48                      48
#define UE_MAX_NUM_TFC_64                      64
#define UE_MAX_NUM_TFC_96                      96
#define UE_MAX_NUM_TFC_128                    128
#define UE_MAX_NUM_TFC_256                    256
#define UE_MAX_NUM_TFC_512                    512
#define UE_MAX_NUM_TFC_1024                  1024

/* Max number of Transport Formats supported */
#define UE_MAX_NUM_TF_32                       32
#define UE_MAX_NUM_TF_64                       64
#define UE_MAX_NUM_TF_128                     128
#define UE_MAX_NUM_TF_256                     256
#define UE_MAX_NUM_TF_512                     512
#define UE_MAX_NUM_TF_1024                   1024

/* Max # of turbo coded bits received */
#define UE_MAX_TURBO_CODE_BITS_RCV_640        640
#define UE_MAX_TURBO_CODE_BITS_RCV_1280      1280
#define UE_MAX_TURBO_CODE_BITS_RCV_2560      2560
#define UE_MAX_TURBO_CODE_BITS_RCV_3840      3840
#define UE_MAX_TURBO_CODE_BITS_RCV_5120      5120
#define UE_MAX_TURBO_CODE_BITS_RCV_6400      6400
#define UE_MAX_TURBO_CODE_BITS_RCV_7680      7680
#define UE_MAX_TURBO_CODE_BITS_RCV_8960      8960
#define UE_MAX_TURBO_CODE_BITS_RCV_10240    10240
#define UE_MAX_TURBO_CODE_BITS_RCV_20480    20480
#define UE_MAX_TURBO_CODE_BITS_RCV_40960    40960
#define UE_MAX_TURBO_CODE_BITS_RCV_81920    81920
#define UE_MAX_TURBO_CODE_BITS_RCV_163840  163840

/* HSDPA Related Parameters */
#define UE_MAX_MAC_D_FLOW 8
#define UE_MAX_HS_QUEUE 8
#define UE_MAX_HS_NO_OF_PDU_SIZE_PER_QUEUE 8
#define UE_MAX_LOGCHAN_PER_DFLOW 15

#define MAX_RB_MUX_OPTIONS_REL5 8

/* RF capabilities                                                  */
/********************************************************************/
/* UE power class as defined in 25.101 */
typedef enum
{
  UE_POWER_CLASS_1 = 1,
  UE_POWER_CLASS_2,
  UE_POWER_CLASS_3,
  UE_POWER_CLASS_4,
  NUM_UE_POWER_CLASSES
} uecomdef_ue_pwr_class_e_type;

/* Frequency separation in MHZ, as defined in 25.101 */
typedef enum
{
  UE_TX_RX_FREQ_SEPARATION_190,
  UE_TX_RX_FREQ_SEPARATION_174_205,
  UE_TX_RX_FREQ_SEPARATION_134_245,
  NUM_TX_RX_FREQ_SEPARATION
} uecomdef_freq_separation_e_type;

/* Physical Channel capabilities                                    */
/********************************************************************/
/* Downlink Physical Channel Capabilities only */

/* Maximum # of DPCH/PDSCH codes */
#define MAX_NUM_DPCH_PDSCH_CODES_1              1
#define MAX_NUM_DPCH_PDSCH_CODES_2              2
#define MAX_NUM_DPCH_PDSCH_CODES_3              3
#define MAX_NUM_DPCH_PDSCH_CODES_4              4
#define MAX_NUM_DPCH_PDSCH_CODES_5              5
#define MAX_NUM_DPCH_PDSCH_CODES_6              6
#define MAX_NUM_DPCH_PDSCH_CODES_7              7
#define MAX_NUM_DPCH_PDSCH_CODES_8              8

/* Maximum # of Physical channel bits received */
#define MAX_NUM_PHY_CHAN_BITS_RCV_600         600
#define MAX_NUM_PHY_CHAN_BITS_RCV_1200       1200
#define MAX_NUM_PHY_CHAN_BITS_RCV_2400       2400
#define MAX_NUM_PHY_CHAN_BITS_RCV_3600       3600
#define MAX_NUM_PHY_CHAN_BITS_RCV_4800       4800
#define MAX_NUM_PHY_CHAN_BITS_RCV_7200       7200
#define MAX_NUM_PHY_CHAN_BITS_RCV_9600       9600
#define MAX_NUM_PHY_CHAN_BITS_RCV_14400     14400
#define MAX_NUM_PHY_CHAN_BITS_RCV_19200     19200
#define MAX_NUM_PHY_CHAN_BITS_RCV_28800     28800
#define MAX_NUM_PHY_CHAN_BITS_RCV_38400     38400
#define MAX_NUM_PHY_CHAN_BITS_RCV_48000     48000
#define MAX_NUM_PHY_CHAN_BITS_RCV_57600     57600
#define MAX_NUM_PHY_CHAN_BITS_RCV_67200     67200
#define MAX_NUM_PHY_CHAN_BITS_RCV_76800     76800

/* Max number of SCCPCH RL if support Simultaneous SCCPCH & DPCH */
#define UE_MAX_SCCPCH_RL                        1

/* Uplink Physical Channel Capabilities only */
/* Maximum # of DPDCH bits transmitted per 10ms */
#define MAX_NUM_BITS_XMIT_ON_DPCH_600         600
#define MAX_NUM_BITS_XMIT_ON_DPCH_1200       1200
#define MAX_NUM_BITS_XMIT_ON_DPCH_2400       2400
#define MAX_NUM_BITS_XMIT_ON_DPCH_4800       4800
#define MAX_NUM_BITS_XMIT_ON_DPCH_9600       9600
#define MAX_NUM_BITS_XMIT_ON_DPCH_19200     19200
#define MAX_NUM_BITS_XMIT_ON_DPCH_28800     28800
#define MAX_NUM_BITS_XMIT_ON_DPCH_38400     38400
#define MAX_NUM_BITS_XMIT_ON_DPCH_48000     48000
#define MAX_NUM_BITS_XMIT_ON_DPCH_57600     57600

#define RLC_AM_MAX_PDU_SIZE_7_BITS     1008 /* 126 Bytes */


/* Multi-Mode/Multi-RAT capabilities                                */
/********************************************************************/

/* Selects the FDD/TDD capabilities of the UE */
typedef enum
{
  UE_MULTI_MODE_CAPABILITY_FDD,
  UE_MULTI_MODE_CAPABILITY_TDD,
  UE_MULTI_MODE_CAPABILITY_FDD_TDD,
  NUM_MULTI_MODE_CAPABILITY
} uecomdef_mm_cap_e_type;

/* Security capabilities                                            */
/********************************************************************/
/* Ciphering Information, a bit string/mask */
#define UE_CIPHERING_ALGORITHM_UEA0         0x0001
#define UE_CIPHERING_ALGORITHM_UEA1         0x0002

/* Integrity Protection Information, a bit string/mask */
#define UE_INTEGRITY_PROTECT_ALGORITHM_UEA1 0x0002

/* UP capabilities                                                  */
/********************************************************************/

/* Selects the GPS capabilities of the UE */
typedef enum
{
  UE_NETWORK_BASED_GPS,
  UE_UE_BASED_GPS,
  UE_NETWORK_AND_UE_BASED_GPS,
  UE_NO_GPS,
  NUM_GPS_CAPABILITY
} uecomdef_gps_capability_e_type;

/* Measurement capabilities                                         */
/********************************************************************/
/* None in this file */
/*===================================================================
                    End UE CAPABILITIES (per 25.331)
====================================================================*/


/*===================================================================
                        CONSTANTS
=====================================================================*/
/*********************************************************************
* If there is a change between the constants defined in the 25.331
* standard (RRC) and implementation it is noted. For instance, MAX_TRCH
* is 32 as defined in the standard, and UE_MAX_TRCH is 8 as defined
* in the QCT implementation. In other words as UE_ in front of the
* constant defined in the standard indicates that the QCT implementation
* differs from that specified in the RRC spec, 25.331. Those values are
* listed prior to all of the RRC constants as defined in 25.331.
**********************************************************************/
/*===================================================================
                 Implemenation Specific UE Capabilities
=====================================================================*/
/*-------------------------------------------------------------------
* This defines the Radio Bearer ID for the Serving Cell BCCH
* Implementation Specific.
---------------------------------------------------------------------*/
#define BCCH_S_RADIO_BEARER_ID          40 


/*-------------------------------------------------------------------
* This defines the Radio Bearer ID for the Neighbor Cell BCCH
* Implementation Specific.
---------------------------------------------------------------------*/
#define BCCH_N_RADIO_BEARER_ID          41


/*-------------------------------------------------------------------
* This defines the Radio Bearer ID for the Serving Cell BCCH when it
* is mapped to a FACH while in CELL_FACH state.
* Implementation Specific.
---------------------------------------------------------------------*/
#define BCCH_FACH_RADIO_BEARER_ID       42


/*-------------------------------------------------------------------
* This defines the Radio Bearer ID for the PCCH logical channel.
* Implementation Specific.
---------------------------------------------------------------------*/
#define PCCH_RADIO_BEARER_ID            43


/*-------------------------------------------------------------------
* This defines the Radio Bearer ID for the PCCH logical channel.
* Implementation Specific.
---------------------------------------------------------------------*/
#define CTCH_RADIO_BEARER_ID            44


/*-------------------------------------------------------------------
* This defines the Maximum number of Uplink Transparent Mode Channels
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_UL_TM_CHANNEL             8


/*-------------------------------------------------------------------
* This defines the Maximum number of Downlink Transparent Mode Channels
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_DL_TM_CHANNEL             8


/*-------------------------------------------------------------------
* This defines the Maximum number of Uplink Unacknowleged Mode Channels
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_UL_UM_CHANNEL             8

/*-------------------------------------------------------------------
* This defines the Maximum number of Downlink Unacknowleged Mode Channels
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_DL_UM_CHANNEL             8

/*-------------------------------------------------------------------
* This defines the Maximum number of Acknowleged Mode Entities that
* are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_AM_ENTITY                ( UE_MAX_SRB_AM_EMTITIES_3 + \
                                         UE_MAX_USER_PLAN_AM_ENTITIES )


/*-------------------------------------------------------------------
* This defines the Maximum number of Uplink Data Plane Acknowleged
* Mode Channels that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_UL_AM_DATA_CHANNEL        UE_MAX_AM_ENTITY


/*-------------------------------------------------------------------
* This defines the Maximum number of Downlink Data Plane Acknowleged
* Mode Channels that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_DL_AM_DATA_CHANNEL        UE_MAX_AM_ENTITY


/*-------------------------------------------------------------------
* This defines the Maximum number of Uplink Control Plane Acknowleged
* Mode Channels that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_UL_AM_CONTROL_CHANNEL     UE_MAX_AM_ENTITY


/*-------------------------------------------------------------------
* This defines the Maximum number of Downlink Control Plane Acknowleged
* Mode Channels that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_DL_AM_CONTROL_CHANNEL     UE_MAX_AM_ENTITY


/*-------------------------------------------------------------------
* This defines the Maximum number of Uplink Acknowleged Mode Channels
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_UL_AM_CHANNEL             UE_MAX_UL_AM_DATA_CHANNEL


/*-------------------------------------------------------------------
* This defines the Maximum number of Downlink Acknowleged Mode Channels
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_DL_AM_CHANNEL             UE_MAX_DL_AM_DATA_CHANNEL


/*-------------------------------------------------------------------
* This defines the Maximum number of all Uplink Logical Channel types
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_UL_LOGICAL_CHANNEL        ( UE_MAX_UL_TM_CHANNEL + \
                                           UE_MAX_UL_UM_CHANNEL + \
                                           UE_MAX_UL_AM_CHANNEL   \
                                         )

/*-------------------------------------------------------------------
* This defines the Maximum number of all Downlink Logical Channel types
* that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_DL_LOGICAL_CHANNEL        ( UE_MAX_DL_TM_CHANNEL + \
                                           UE_MAX_DL_UM_CHANNEL + \
                                           UE_MAX_DL_AM_CHANNEL   \
                                         )

/*-------------------------------------------------------------------
* This defines the Maximum number of Uplink Packet Data Units (PDUs)
* allowed per frame that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_UL_RLC_PDUS              24

/*-------------------------------------------------------------------
* This defines the Maximum number of Downlink Packet Data Units (PDUs)
* allowed per frame that are supported by the QCT implementation.
---------------------------------------------------------------------*/
#define UE_MAX_DL_RLC_PDUS              70

/*-------------------------------------------------------------------
* This defines the Maximum number of Signalling Radio Bearers that 
* can be established as supported by the QCT UE Implementation
---------------------------------------------------------------------*/
#define UE_MAX_SRB_SETUP                 5

/* This defines the Maximum number of TFC subsets that be stored in
   our UE */
#define UE_MAX_TFC_SUBSET                8


/*-------------------------------------------------------------------
* This defines the Maximum number of Transport Channels that can be
* supported by the QCT UE Implementation.
* This is the same for both the uplink and downlink.
---------------------------------------------------------------------*/
#define UE_MAX_TRCH                      (UE_MAX_NUM_TRCH_8)

/*-------------------------------------------------------------------
* This defines the Maximum number of Preconfigured Transport channels
* supported by the QCT UE Implementation.
---------------------------------------------------------------------*/
#define UE_MAX_PRE_CONFIG_TRCH           (UE_MAX_NUM_TRCH_8)


/*-------------------------------------------------------------------
* This defines the Maximum number of Coded Composite Transport Channels
* (CCTrCH) supported by the QCT UE Implementation. Note that only
* UE_MAX_CCTRCH / 2 CCTrCHs can be active at a time. Only 1 CCTrCH 
* is allowed in the Uplink.
* Note that this is uplink + downlink. The number of allowed CCTrCHs
* to be setup by RRC is 5 since L1 reserves one for BCHs while in
* CELL_DCH state.
---------------------------------------------------------------------*/
#define UE_MAX_CCTRCH                    6

/*-------------------------------------------------------------------
* This defines the Maximum number of different transport formats that
* can be included in the Transport format set for all transport
* channels as supported by the UE according to 25.331 in the Downlink.
* Note that this value as calculated by using values defined the 
* standard (25.331) would be 32x32 = 1024.
---------------------------------------------------------------------*/
#define UE_MAX_TF                        (UE_MAX_NUM_TF_64)


/*-------------------------------------------------------------------
* This defines the Maximum number of transport format combinations
* as supported by the QCT UE Implemenation.
---------------------------------------------------------------------*/
#define UE_MAX_TFC                       (UE_MAX_NUM_TFC_128)

/*-------------------------------------------------------------------
* This defines the Maximum number of SCCPCHs per cell
* as supported by the QCT UE Implementation.
---------------------------------------------------------------------*/
#define UE_MAX_SCCPCH                    16


/*-------------------------------------------------------------------
* This defines the Maximum number of PRACHs per cell
* as supported by the QCT UE Implementation.
---------------------------------------------------------------------*/
#define UE_MAX_PRACH                     8

/*-------------------------------------------------------------------
* This defines the Maximum number of Simultaneous Radio Links
* as supported by the QCT UE Implementation.
---------------------------------------------------------------------*/
#define UE_MAX_RL                        6

/*-------------------------------------------------------------------
* This defines the Maximum number of Simultaneous DPCH
* as supported by the QCT UE Implementation.
---------------------------------------------------------------------*/
#define UE_MAX_DPCH                      1

/*-------------------------------------------------------------------
* This defines the Maximum number of Core Network Domains
* as supported by the QCT UE Implementation.
---------------------------------------------------------------------*/
#define UE_MAX_CN_DOMAINS                2
/*===================================================================
               End Implemenation Specific UE Capabilities
=====================================================================*/

/*===================================================================
                        Radio Bearer ID Constants
=====================================================================*/
/* This defines the Radio Bearer ID for CCCH Logical Channel.*/
#define CCCH_RADIO_BEARER_ID             0  

/* This defines the Radio Bearer ID for Unacknowledged mode transmission.*/
#define DCCH_UM_RADIO_BEARER_ID          1 

/* THis defines the Radio Bearer ID for Acknowledged mode transmission.*/
#define DCCH_AM_RADIO_BEARER_ID          2 
 
/* Radio Bearer for High priority Direct Transfer mode  transmission.*/
#define DCCH_DT_HIGH_PRI_RADIO_BEARER_ID 3
                                            
/* Radio Bearer for Low priority Direct Transfer mode  transmission.*/
#define DCCH_DT_LOW_PRI_RADIO_BEARER_ID  4
/*===================================================================
                      End Radio Bearer ID Constants
=====================================================================*/
                                            

/*===================================================================
                        Core Network (CN) Information
=====================================================================*/
/*-------------------------------------------------------------------
* This defines the Maximum number of Core Network Domains that can be 
* supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_CN_DOMAINS                   4


/*-------------------------------------------------------------------
* This defines the Maximum number of Signalling Flow Ids that can be 
* supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_SIGNALLING_FLOW             16
/*===================================================================
                   End Core Network (CN) Information
=====================================================================*/


/*===================================================================
                     UTRAN Mobility Information
=====================================================================*/
/*-------------------------------------------------------------------
* This defines the Maximum number of other Radio Access Technologies
* that can be supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_OTHER_RAT                   15


/*-------------------------------------------------------------------
* This defines the Maximum number of Radio Access Technologies that
* can be supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_RAT                          (MAX_OTHER_RAT + 1) 


/*-------------------------------------------------------------------
* This defines the Maximum number of URAs in a cell that can be
* supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_URA                          8


/*-------------------------------------------------------------------
* This defines the Maximum number of Inter System Messages that
* can be supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_INTER_SYS_MSGS               4


/*-------------------------------------------------------------------
* This defines the Maximum number of Radio Access Bearers (RABs) to
* be established that can be supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_RAB_TO_SETUP                16 
/*===================================================================
                      End UTRAN Mobility Information
=====================================================================*/


/*===================================================================
                     User Equipment (UE) Information
=====================================================================*/
/*-------------------------------------------------------------------
* This defines the Maximum number of PDCP Algorithms that can be
* supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_NUM_PDCP_ALGOR               8


/*-------------------------------------------------------------------
* This defines the Maximum number of Frequency Bands that can be
* supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_FREQ_BANDS                   4


/*-------------------------------------------------------------------
* This defines the Maximum number of UEs that can be paged in a
* Paging Type 1 message that can be supported by the UE according 
* to 25.331.
---------------------------------------------------------------------*/
#define MAX_PAGE1                        8


/*-------------------------------------------------------------------
* This defines the Maximum number of System Capabilities that can be
* supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_SYS_CAP_REQ                 16
/*===================================================================
                    End User Equipment (UE) Information
=====================================================================*/


/*===================================================================
                      Radio Bearer (RB) Information
=====================================================================*/
/*-------------------------------------------------------------------
* This defines the Maximum number of predefined configurations 
* supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PRE_DEFINE_CONFIG           16 


/*-------------------------------------------------------------------
* This defines the Maximum number of Radio Bearers that can be 
* supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_RB                          32


/*-------------------------------------------------------------------
* This defines the Maximum number of Signalling Radio Bearers that 
* can be established as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_SRB_SETUP                    8


/*-------------------------------------------------------------------
* This defines the Maximum number of Radio Bearers per Radio Access
* Bearer that can be supported by the UE according  to 25.331.
---------------------------------------------------------------------*/
#define MAX_RB_PER_RAB                   8


/*-------------------------------------------------------------------
* This defines the Maximum number of non-signalling Radio Bearers
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_RB_ALL_RABS                 27


/*-------------------------------------------------------------------
* This defines the Maximum number of RB multiplexing options
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_RB_MUX_OPTIONS               2


/*-------------------------------------------------------------------
* This defines the Maximum number of logical channels per RLC entity
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_LOGCHAN_PER_RLC_ID           2

/*-------------------------------------------------------------------
* This defines the Maximum number of dedicated logical channels per 
* transport channel as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_DED_LOGCHAN_PER_TRCH        15

/*-------------------------------------------------------------------
* This defines the Maximum number of RLC PDU size that can be
* associated with any logical channel in UE according to 25.331 (REL6)
---------------------------------------------------------------------*/
#define MAX_RLC_PDU_SIZE_PER_LOGCHAN    32

/*-------------------------------------------------------------------
* This defines the Maximum number of E-DCH MAC-D flows 
* according to 25.331 (REL6)
---------------------------------------------------------------------*/
#define MAX_E_DCH_MAC_D_FLOW            8

/*===================================================================
                     End Radio Bearer (RB) Information
=====================================================================*/


/*===================================================================
                   Transport Channel (TrCH) Information
=====================================================================*/
/*-------------------------------------------------------------------
* This defines the Maximum number of Transport Channels that can be
* supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_TRCH                        32


/*-------------------------------------------------------------------
* This defines the Maximum number of Preconfigured Transport channels
* supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PRE_CONFIG_TRCH             16


/*-------------------------------------------------------------------
* This defines the Maximum number of Coded Composite Transport Channels
* (CCTrCH) supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_CCTRCH                       8


/*-------------------------------------------------------------------
* This defines the Maximum number of different transport formats that
* can be included in the Transport format set for one transport
* channel as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_TF                          32


/*-------------------------------------------------------------------
* This defines the Maximum number of different transport formats that
* can be included in a CPCH set as supported by the UE according
* to 25.331.
---------------------------------------------------------------------*/
#define MAX_CPCH_TF                     16


/*-------------------------------------------------------------------
* This defines the Maximum number of transport format combinations
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_TFC                       1024


/*-------------------------------------------------------------------
* This defines the Maximum number of transport format combination
* index (TFCI-1) combinations as supported by the UE according 
* to 25.331.
---------------------------------------------------------------------*/
#define MAX_TFCI1                      512


/*-------------------------------------------------------------------
* This defines the Maximum number of transport format combination
* index (TFCI-2) combinations as supported by the UE according 
* to 25.331.
---------------------------------------------------------------------*/
#define MAX_TFCI2                     512


/*-------------------------------------------------------------------
* This defines the Maximum number of CPCH sets per call
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_CPCH_SETS                  16


/*-------------------------------------------------------------------
* This defines the Maximum number of System Information blocks per
* SYSTEM INFORMATION Msg as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_SIB_PER_MSG                16


/*-------------------------------------------------------------------
* This defines the Maximum number of references to other system 
* information blocks as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_SIB                        32


/*-------------------------------------------------------------------
* This defines the Maximum number of references to system information
* blocks on the FACH as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_SIB_FACH                    8
/*===================================================================
                  End Transport Channel (TrCH) Information
=====================================================================*/


/*===================================================================
                   Physical Channel (PhyCH) Information
=====================================================================*/
/*-------------------------------------------------------------------
* This defines the Maximum number of subchannels on the PRACH
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PRACH_SUB_CHAN             12


/*-------------------------------------------------------------------
* This defines the Maximum number of available subchannels for AP
* signature on PCPCH as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PCPCH_AP_SUB_CHAN          12


/*-------------------------------------------------------------------
* This defines the Maximum number of available subchannels for CD 
* signature on PCPCH as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PCPCH_CD_SUB_CHAN          12


/*-------------------------------------------------------------------
* This defines the Maximum number of available signatures for AP on
* the PCPCH as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PCPCH_AP_SIG               16


/*-------------------------------------------------------------------
* This defines the Maximum number of available signatures for CD on
* the PCPCH as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PCPCH_CD_SIG               16


/*-------------------------------------------------------------------
* This defines the Maximum number of Access Classes 
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_AC                         16


/*-------------------------------------------------------------------
* This defines the Maximum number of Access Service Classes 
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_ASC                         8


/*-------------------------------------------------------------------
* This defines the Maximum number of Access Class (AC) to Access
* Service Class (ASC) mappings as supported by the UE according 
* to 25.331.
---------------------------------------------------------------------*/
#define MAX_ASC_MAP                     7


/*-------------------------------------------------------------------
* This defines the Maximum number of Access Service Classes (ASC) for
* which persistence scaling factors are specified as supported by the
* UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_ASC_PERSIST                 6


/*-------------------------------------------------------------------
* This defines the Maximum number of PRACHs in a cell
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PRACH                      16


/*-------------------------------------------------------------------
* This defines the Maximum number of FACHs and PCHs that can be
* mapped to one SCCPCH as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_FACH_PCH_ON_SCCPCH          8


/*-------------------------------------------------------------------
* This defines the Maximum number of Radio Links (RL)
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_RL                          8


/*-------------------------------------------------------------------
* This defines the Maximum number of SCCPCHs per cell
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
/* Commented out to fix a conflict with L1 */
//#define MAX_SCCPCH                     16


/*-------------------------------------------------------------------
* This defines the Maximum number of DPDCHs per cell
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_DPDCH                       6


/*-------------------------------------------------------------------
* This defines the Maximum number of channelization codes for downlink
* DPCHs as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_DL_CHAN_CODES_DPCH          8


/*-------------------------------------------------------------------
* This defines the Maximum number of PDSCHs
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PDSCH                       8


/*-------------------------------------------------------------------
* This defines the Maximum number of codes for PDSCHs
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PDSCH_CODES                16


/*-------------------------------------------------------------------
* This defines the Maximum number of TFCI groups for PDSCHs
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PDSCH_TFCI_GROUPS         256


/*-------------------------------------------------------------------
* This defines the Maximum number of code groups for PDSCHs
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PDSCH_CODE_GROUPS         256


/*-------------------------------------------------------------------
* This defines the Maximum number of PCPCH channels in a CPCH set
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PCPCH                      64


/*-------------------------------------------------------------------
* This defines the Maximum number of available Spreading Factors (SFs)
* on PCPCH as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_SF_PCPCH                    7


/*-------------------------------------------------------------------
* This defines the Maximum number of PDSCH Identities (IDs)
* as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_PDSCH_IDS                  64
/*===================================================================
                 End Physical Channel (PhyCH) Information
=====================================================================*/


/*===================================================================
                         Frequency Information
=====================================================================*/
/*-------------------------------------------------------------------
* This defines the Maximum number of FDD carrier frequencies to be
* stored in the USIM as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_STORED_FDD_FREQ_LIST        4


/*-------------------------------------------------------------------
* This defines the Maximum number of FDD neighboring cells to be
* stored in the USIM as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_STORED_FDD_NEIGHS_LIST     32


/*-------------------------------------------------------------------
* This defines the Maximum number of GSM cells to be stored in the
* USIM as supported by the UE according to 25.331.
---------------------------------------------------------------------*/
#define MAX_STORED_GSM_CELLS_LIST      32
/*===================================================================
                       End Frequency Information
=====================================================================*/




/*-------------------------------------------------------------------
TYPE: uecomdef_status_e_type

This is an enumerated type designed to indicate if a certain 
process completed sucessfully or completed/could not complete due
to a failure of some type.
--------------------------------------------------------------------*/
typedef enum
{
  SUCCESS,
  FAILURE
} uecomdef_status_e_type;

/*-------------------------------------------------------------------
TYPE: rlc_sdu_tx_fail_reason_e_type

This enum type indicates the failure reason in L2 Ack ind
--------------------------------------------------------------------*/
typedef enum
{
  NO_FAILURE,
  RLC_RESET_REQ,
  RLC_RE_ESTABLISH_REQ,
  RLC_PDU_MAX_DAT,
  WM_PURGE_REQ,
  MAX_REASON
} rlc_sdu_tx_fail_reason_e_type;

/*-------------------------------------------------------------------
TYPE: uecomdef_logchan_e_type

The following enumeration is for all supported Logical channels
in the UE.
--------------------------------------------------------------------*/

typedef enum 
{
                                  
  UE_LOGCHAN_NONE = -1,                   /* Invalid Logical Channel.*/  
  UE_LOGCHAN_BCCH,                        /* Broadcast Control Channel */
  UE_LOGCHAN_PCCH,                        /* Paging Control Channel.*/
  UE_LOGCHAN_CCCH,                        /* Common Control Channel.*/
  UE_LOGCHAN_DCCH,                        /* Dedicated Control Channel.*/
  UE_LOGCHAN_CTCH,                        /* Comman Traffic Channel.*/
  UE_LOGCHAN_DTCH,                        /* Dedicated Traffic Channel.*/
  UE_LOGCHAN_TM_DCCH_DL,
  UE_LOGCHAN_MAX_NUM
} uecomdef_logchan_e_type;


/*-------------------------------------------------------------------
TYPE: uecomdef_logchan_e_type

The following enumeration is for Logical channels modes.
--------------------------------------------------------------------*/   
typedef enum
{                      
              
  UE_MODE_TRANSPARENT,                     /* Transparent Mode.*/
  UE_MODE_UNACKNOWLEDGED,                  /* Unacknowledged Mode.*/
  UE_MODE_ACKNOWLEDGED,
  UE_MODE_ACKNOWLEDGED_DATA,               /* Acknowledged Mode for 
                                              Data */
  UE_MODE_ACKNOWLEDGED_CONTROL,            /* Acknowledged Mode for 
                                              Control */ 
  UE_MODE_DL_CTCH,
  UE_MODE_MAX_NUM
} uecomdef_logch_mode_e_type; 


/*-------------------------------------------------------------------
TYPE: uecomdef_trch_e_type

The following enumeration is for all supported Transport channels
in the UE.
--------------------------------------------------------------------*/   
typedef enum
{
  UE_TRCH_INVALID,

  /* Downlink Transport channels */
  UE_DL_TRCH_BCH,
  UE_DL_TRCH_S_BCH,
  UE_DL_TRCH_N_PCH,
  UE_DL_TRCH_PCH,
  UE_DL_TRCH_FACH,
  UE_DL_TRCH_DSCH,

  /* Uplink Transport channel */
  UE_UL_TRCH_RACH,

  /* Downlink and Uplink common Transport channel */
  UE_TRCH_DCH,
  UE_TRCH_MAX_NUM

} uecomdef_trch_e_type;


/*-------------------------------------------------------------------
TYPE: uecomdef_phychan_e_type

The following enumeration is for all supported Physical channels
in the UE.
--------------------------------------------------------------------*/   
typedef enum
{
  /* Downlink Physical channels */
  UE_DL_PHYCHAN_NOCHAN,
  UE_DL_PHYCHAN_PCCPCH_S,
  UE_DL_PHYCHAN_PCCPCH_N,
  UE_DL_PHYCHAN_SCCPCH_0,
  UE_DL_PHYCHAN_SCCPCH_1,
  UE_DL_PHYCHAN_PICH,
  UE_DL_PHYCHAN_AICH,
  UE_DL_PHYCHAN_PDSCH,

  /* Uplink Physical channels */
  UE_UL_PHYCHAN_NOCHAN,
  UE_UL_PHYCHAN_PRACH,
  UE_UL_PHYCHAN_DPCCH,

  /* Downlink and Uplink common Physical channels */
  UE_PHYCHAN_DPDCH,
  UE_PHYCHAN_MAX_NUM

} uecomdef_phychan_e_type;

/*-------------------------------------------------------------------
TYPE: uecomdef_ciphering_algorithm_e_type

The following enumeration is for all supported Ciphering Algorithm
types.
--------------------------------------------------------------------*/   
typedef enum
{
  UE_WCDMA_CIPHER_ALGO_NONE = 0x0,

  /*Ciphering Algo for No encryption, Value 1 for Bit 0 – indicate UEA0*/
  UE_WCDMA_CIPHER_ALGO_UEA0 = 0x1,
  
  /*KASUMI Ciphering Algo, Value 1 for Bit 1 – indicate UEA1 */
  UE_WCDMA_CIPHER_ALGO_UEA1 = 0x2
  
  /*SNOW-3G Ciphering Algo,Value 1 for Bit 2 – indicate UEA2 */
  ,UE_WCDMA_CIPHER_ALGO_UEA2 = 0x4
} uecomdef_wcdma_cipher_algo_e_type;

/*-------------------------------------------------------------------
TYPE: uecomdef_integrity_protection_algorithm_e_type

The following enumeration is for all supported Integrity Protection
Algorithmtypes.
--------------------------------------------------------------------*/   
typedef enum
{
  /*Invalid Integrity Algo */
  UE_WCDMA_INTEGRITY_PROT_ALGO_NONE = 0x0,
  
  /*KASUMI Integrity Algo */
  UE_WCDMA_INTEGRITY_PROT_ALGO_UIA1 = 0x1
  
  /*SNOW-3G Integrity Algo */
  ,UE_WCDMA_INTEGRITY_PROT_ALGO_UIA2 = 0x2
} uecomdef_wcdma_integrity_protection_algorithm_e_type;

/*===================================================================
                           DATA TYPES
====================================================================*/
/*-------------------------------------------------------------------
TYPE: rb_id_type

This defines the Radio Bearer identity as described in 25.331 (RRC). 
--------------------------------------------------------------------*/
typedef uint16  rb_id_type;


/*-------------------------------------------------------------------
TYPE: log_ch_id_type

This defines the Logical Channel identity as described in 25.331(RRC). 
--------------------------------------------------------------------*/
typedef uint16 log_ch_id_type;


/*-------------------------------------------------------------------
TYPE: tr_ch_id_type

This defines the Transport Channel identity as described in  25.331(RRC). 
--------------------------------------------------------------------*/
typedef uint16 tr_ch_id_type;


/*-------------------------------------------------------------------
TYPE: cctrch_id_type

This defines the CCTrCH Id. 
--------------------------------------------------------------------*/
typedef uint8  cctrch_id_type;


/*-------------------------------------------------------------------
TYPE: pdcp_seq_num_type

This defines the PDCP Sequence number. 
--------------------------------------------------------------------*/
typedef uint16  pdcp_seq_num_type;

/*-------------------------------------------------------------------
STRUCTURE: uecomdef_rlc_type

This structure defines the information necessary to uniquely describe
the channel/radio bearer mappings to/from a logical channel.
--------------------------------------------------------------------*/
typedef struct
{
  rb_id_type                 rb_id;
  rlc_lc_id_type             rlc_id;
  uecomdef_logchan_e_type    logchan;
  uecomdef_logch_mode_e_type logchan_mode;
} uecomdef_rlc_type;

/*-------------------------------------------------------------------
STRUCTURE: uecomdef_cctrch_type

This structure defines the information necessary to uniquely describe
the channel/radio bearer mappings to/from a logical channel.
--------------------------------------------------------------------*/
typedef struct
{
  cctrch_id_type             cctrch_id;
  uecomdef_phychan_e_type    logchan;
} uecomdef_cctrch_type;

/*-------------------------------------------------------------------
STRUCTURE: uecomdef_trch_type

This structure defines the information necessary to uniquely describe
channel mapping to/from a given transport channel.
--------------------------------------------------------------------*/
typedef struct
{
  tr_ch_id_type         trch_id;
  uecomdef_trch_e_type  trch;
  uint8                 num_of_log_chan;
  uecomdef_rlc_type     rlc[UE_MAX_DL_LOGICAL_CHANNEL+UE_MAX_UL_LOGICAL_CHANNEL];
  uecomdef_cctrch_type  cctrch;
} uecomdef_trch_type;

/*-------------------------------------------------------------------
TYPE: mac_hs_e_type

The following enumeration is for indicating if MAC HS or MAC-HS is chosen
in the UE.
--------------------------------------------------------------------*/
typedef enum _mac_hs_e_type
{
  MAC_HS_TYPE_INVALID,
  MAC_HS,
  MAC_EHS
}mac_hs_e_type;

#endif /* UECOMDEF_H */
