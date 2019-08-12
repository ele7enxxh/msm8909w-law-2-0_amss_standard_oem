#ifndef SM_COMMON_H
#define SM_COMMON_H

/*===========================================================================
                        S M    C O M M O N   Header File

DESCRIPTION

   Definition of common data types used in the primitives between Session 
   Management Entity and other Entities. (SM-SNDCP, SM-RABM, SM-CM, SM-GMM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/sm_common.h_v   1.7   02 May 2002 17:21:44   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sm_common.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/08   rm      CMI defeaturisation
01/29/08   rk      modified MAX_PROTOCOL_ID_CONTENT to support CHAP Username/Passwd of length upto 127 each
10/10/06   ss      Added cause to remove compiler warnings
07/18/06   kvk     Now NUM_PROTOCOL_ID will be set to 17 only in case FEATURE_IMS is enabled
07/03/06   ss      Added GPRS call control fixes
06/13/06   aw      Updated the NUM_PROTOCOL_ID to 17 to support cscf containers.
04/04/06   kvk     Add HTORPC meta-comments to export CM.
11/11/05   ss      Release 5 SM PCO and TFT changes put in.
09/12/05   kvk     Chnaged enum UNKNOWN to UNKNOWN_SOURCE_STAT_DESC to avoid 
                   redifinition of the same enum.
05/11/05   ss       Added enums for Release 99 and Release 5 QoS attributes.
                         Also added cause values for SM cause for spec CRs
03/25/05   kvk     Changed enhanced QoS elements to sync with the DS.
01/10/04   kvk     Added new elements for Rel5 QoS Profile
12/06/04   kvk     Defined macro MIN_NSAPI_NUM
08/05/03   mks     Added new enumerated types reliability_class_T, delay_class_T, 
                   precedence_class_T, peak_throughput_T, mean_throughput_T for
                   supporting QoS validation during MO PDP context activation and
                   MT PDP context modification procedures.
02/23/03   ks      Added new SM status cause SM_FEATURE_NOT_SUPPORTED
04/23/02   ks      Modified the NUM_PROTOCOL_ID to 4, MAX_PROTOCOL_ID_CONTENT
                   to 80 to account for PAP/CHAP/IPCP packets
08/24/01   ks      Fixed few enum values in sm_status_T enumeration.
05/30/01   ks      Added a "SM" prefix for all the SM causes to avoid
                   conflict with similar cause.
05/23/01   ks      Fixed the MAX_FILTER_CONTENT_LENGTH and 
                   MAX_PROTOCOL_ID_CONTENT to correct values.
04/27/01   ks      Added the Packet Flow Identifier (PFI) IE data type and 
                   also fixed the macro definitions. 
01/26/01   ks      Created file.
            
===========================================================================*/

#include "comdef.h" 
#include "ms.h"
#include "sys.h"      /* system wide common types */
#include "sys_v.h"
#include "cm_gw.h"    /* Defintions from NAS required by clients */

#define MAX_PDP_NSAPI_NUM 0x10
                                 


/*Min NSAPI number*/
#define MIN_NSAPI_NUM 5 //NSAPI starts from 5 (0-4 are reserved NSAPIs)


/* ---------------------------------------------------------------------------
* LLC-SAPI definitions for valid LLC_SAPIs for GPRS.
* --------------------------------------------------------------------------*/
#define LLC_SAPI_UNASSIGNED   0
#define LLC_SAPI_3            3
#define LLC_SAPI_5            5
#define LLC_SAPI_9            9
#define LLC_SAPI_11           11

/* ----------------------------------------------------------------------
 Reliability class enumeration type, defined in Table 10.5.156/TS 24.008 
** ----------------------------------------------------------------------*/
typedef enum 
{
  SUBSCRIBED_RELIABILITY_CLASS,
  ACK_GTP_LLC_RLC_PROTECTED_DATA_RELIABILITY_CLASS,
  UNACK_GTP_ACK_LLC_RLC_PROTECTED_DATA_RELIABILITY_CLASS,
  UNACK_GTP_LLC_ACK_RLC_PROTECTED_DATA_RELIABILITY_CLASS,
  UNACK_GTP_LLC_RLC_PROTECTED_DATA_RELIABILITY_CLASS,
  UNACK_GTP_LLC_RLC_UNPROTECTED_DATA_RELIABILITY_CLASS,
  RESERVED_RELIABILITY_CLASS = 0x0
} reliability_class_T;

/* ----------------------------------------------------------------
 Delay class enumeration type, defined in Table 10.5.156/TS 24.008 
** ----------------------------------------------------------------*/
typedef enum
{
  SUBSCRIBED_DELAY_CLASS,
  DELAY_CLASS_ONE,
  DELAY_CLASS_TWO,
  DELAY_CLASS_THREE,
  DELAY_CLASS_BEST_EFFORT,
  RESERVED_DELAY_CLASS
} delay_class_T;

/* ---------------------------------------------------------------------
 Precedence class enumeration type, defined in Table 10.5.156/TS 24.008 
** ---------------------------------------------------------------------*/
typedef enum
{
  SUBSCRIBED_PRECEDENCE_CLASS,
  HIGH_PRIORITY_PRECEDENCE_CLASS,
  NORMAL_PRIORITY_PRECEDENCE_CLASS,
  LOW_PRIORITY_PRECEDENCE_CLASS,
  RESERVED_PRECEDENCE_CLASS
} precedence_class_T;

/* ---------------------------------------------------------------------
 Peak Throughput enumeration type, defined in Table 10.5.156/TS 24.008 
** ---------------------------------------------------------------------*/
typedef enum
{
  SUBSCRIBED_PEAK_THROUGHPUT,
  PEAK_THROUGHPUT_UP_TO_1000_OPS,
  PEAK_THROUGHPUT_UP_TO_2000_OPS,
  PEAK_THROUGHPUT_UP_TO_4000_OPS,
  PEAK_THROUGHPUT_UP_TO_8000_OPS,
  PEAK_THROUGHPUT_UP_TO_16000_OPS,
  PEAK_THROUGHPUT_UP_TO_32000_OPS,
  PEAK_THROUGHPUT_UP_TO_64000_OPS,
  PEAK_THROUGHPUT_UP_TO_128000_OPS,
  PEAK_THROUGHPUT_UP_TO_256000_OPS,
  RESERVED_PEAK_THROUGHPUT
} peak_throughput_T;

/* ---------------------------------------------------------------------
 Mean Throughput enumeration type, defined in Table 10.5.156/TS 24.008 
** ---------------------------------------------------------------------*/
typedef enum
{
  SUBSCRIBED_MEAN_THROUGHPUT,
  MEAN_THROUGHPUT_UP_TO_100_OPH,
  MEAN_THROUGHPUT_UP_TO_200_OPH,
  MEAN_THROUGHPUT_UP_TO_500_OPH,
  MEAN_THROUGHPUT_UP_TO_1000_OPH,
  MEAN_THROUGHPUT_UP_TO_2000_OPH,
  MEAN_THROUGHPUT_UP_TO_5000_OPH,
  MEAN_THROUGHPUT_UP_TO_10000_OPH,
  MEAN_THROUGHPUT_UP_TO_20000_OPH,
  MEAN_THROUGHPUT_UP_TO_50000_OPH,
  MEAN_THROUGHPUT_UP_TO_100000_OPH,
  MEAN_THROUGHPUT_UP_TO_200000_OPH,
  MEAN_THROUGHPUT_UP_TO_500000_OPH,
  MEAN_THROUGHPUT_UP_TO_1000000_OPH,
  MEAN_THROUGHPUT_UP_TO_2000000_OPH,
  MEAN_THROUGHPUT_UP_TO_5000000_OPH,
  MEAN_THROUGHPUT_UP_TO_10000000_OPH,
  MEAN_THROUGHPUT_UP_TO_20000000_OPH,
  MEAN_THROUGHPUT_UP_TO_50000000_OPH,
  RESERVED_MEAN_THROUGHPUT = 0x1E,
  BEST_EFFORT_MEAN_THROUGHPUT = 0x1F
} mean_throughput_T;

/* ---------------------------------------------------------------------
 Traffic Class enumeration type, defined in Table 10.5.156/TS 24.008 
** ---------------------------------------------------------------------*/
typedef enum
{
  SUBSCRIBED_TRAFFIC_CLASS,
  CONVERSATIONAL_TRAFFIC_CLASS,
  STREAMING_TRAFFIC_CLASS,
  INTERACTIVE_TRAFFIC_CLASS,
  BACKGROUND_TRAFFIC_CLASS,
  RESERVED_TRAFFIC_CLASS = 0x7
} traffic_class_T;

/* ---------------------------------------------------------------------
 Delivery Order enumeration type, defined in Table 10.5.156/TS 24.008 
** ---------------------------------------------------------------------*/
typedef enum
{
  SUBSCRIBED_DELIVERY_ORDER,
  WITH_DELIVERY_ORDER,
  WITHOUT_DELIVERY_ORDER,
  RESERVED_DELIVERY_ORDER
} delivery_order_T;

/* ---------------------------------------------------------------------
 Delivery Of Erroneous SDUs enumeration type, defined in Table 10.5.156/TS 24.008 
** ---------------------------------------------------------------------*/
typedef enum
{
  SUBSCRIBED_DELIVERY_OF_ERRONEOUS_SDUS,
  NO_DETECT_DELIVERY_OF_ERRONEOUS_SDUS,
  ERRONEOUS_SDUS_ARE_DELIVERED,
  ERRONEOUS_SDUS_ARE_NOT_DELIVERED,
  RESERVED_DELIVERY_OF_ERRONEOUS_SDUS
} delivery_of_erroneous_sdus_T;

/* ---------------------------------------------------------------------
 Traffic Handling Priority enumeration type, defined in Table 10.5.156/TS 24.008 
** ---------------------------------------------------------------------*/
typedef enum
{
  SUBSCRIBED_TRAFFIC_HANDLING_PRIORITY,
  TRAFFIC_HANDLING_PRIORITY_LEVEL_1,
  TRAFFIC_HANDLING_PRIORITY_LEVEL_2,
  TRAFFIC_HANDLING_PRIORITY_LEVEL_3
} traffic_handling_priority_T;

/* ---------------------------------------------------------------------
 Signalling Indication enumeration type, defined in Table 10.5.156/TS 24.008 
** ---------------------------------------------------------------------*/
typedef enum
{
  NOT_OPTIMISED_FOR_SIGNALLING_TRAFFIC,
  OPTIMISED_FOR_SIGNALLING_TRAFFIC
} signalling_indication_T;

/* ---------------------------------------------------------------------
  Source Statistics Descriptor enumeration type, defined in Table 10.5.156/TS 24.008 
** ---------------------------------------------------------------------*/
typedef enum
{
  UNKNOWN_SOURCE_STAT_DESC,
  SPEECH
} source_statistics_descriptor_T;


/* --------------------------------------------------------------------------                       
** The purpose of the MBMS protocol configuration options IE is to 
** transfer protocol options associated with the bearer level of an MBMS context activation.
** The IE is defined in 10.5.6.15.
** ------------------------------------------------------------------------*/                       
typedef struct mbms_prot_config_options
{
  boolean valid;
  byte data;
} mbms_prot_config_options_T;

/* --------------------------------------------------------------------------                       
** The purpose of the TMGI element is for group paging in MBMS.
** The IE is defined in 10.5.6.13.
** ------------------------------------------------------------------------*/                       
typedef struct tmgi
{
  boolean valid;
  byte mbms_serv_id[3];
  sys_plmn_id_s_type plmn_id;
} tmgi_T;

/* --------------------------------------------------------------------------                       
** The purpose of the MBMS bearer capabilities information element is to indicate the maximum bit rate for downlink
** supported by the MS for an MBMS context.
** The IE is defined in 10.5.6.14.
** ------------------------------------------------------------------------*/                       
typedef struct mbms_bearer_cap
{
  boolean valid;
  byte    max_bit_rare_downlink;
  byte    ext_max_bit_rate_downlink;
} mbms_bearer_cap_T;


/* --------------------------------------------------------------------------                       
** Radio Priority is to specify the priority level that the MS shall use
** at lower layers for transmission of data related to PDP context.
** The IE is defined in 10.5.7.2
** ------------------------------------------------------------------------*/                       
typedef struct rad_priority
{
  boolean valid;
  byte    radio_priority;
} rad_priority_T;


/* --------------------------------------------------------------------------                       
** SM cause information element is to indicate the reason why a session 
** management request is rejected. The IE is defined in 10.5.6.6.
** ------------------------------------------------------------------------*/
typedef struct cause_s
{
  boolean valid;
  byte  cause;
} cause_T;
       
/* --------------------------------------------------------------------------                       
** Packet Flow Identifier (PFI), Indicates Packet Flow identifier for a 
** Packet Flow content.
** ------------------------------------------------------------------------*/
typedef struct pfi
{
  boolean valid;
  byte packet_flow_id;
} pfi_T;

#endif
