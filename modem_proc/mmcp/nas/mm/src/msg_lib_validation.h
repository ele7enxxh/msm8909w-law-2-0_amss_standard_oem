/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*==============================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/msg_lib_validation.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who                          what                              
--------     --------  ------------------------------------------------

==============================================================================*/
#ifndef _MSG_LIB_VALIDATION_H_
#define _MSG_LIB_VALIDATION_H_ 
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE

/*==============================================================================
                           DATA DECLARATIONS
==============================================================================*/
#define MIN_QOS_RESIDUAL_BER 0
#define MAX_QOS_RESIDUAL_BER 15

#define MIN_QOS_SDU_ERROR_RATIO 0
#define MAX_QOS_SDU_ERROR_RATIO 15

#define MIN_QOS_TRAFFIC_HANDLING_PRIORITY 0
#define MAX_QOS_TRAFFIC_HANDLING_PRIORITY 15

#define MIN_QOS_TRAFFIC_DELAY 0
#define MAX_QOS_TRAFFIC_DELAY 64

#define MIN_TRANSFER_DELAY 0
#define MAX_TRANSFER_DELAY 63

#define MIN_QOS_UL_DL_GBR 0
#define MAX_QOS_UL_DL_GBR 255

#define MIN_QOS_EXT_UL_DL_GBR 0
#define MAX_QOS_EXT_UL_DL_GBR 255

#define MIN_QOS_SRC_STATISTICS_DESCRIPTOR 0
#define MAX_QOS_SRC_STATISTICS_DESCRIPTOR 1

#define MAX_APN_AMBR_UL_DL 255
#define MIN_APN_AMBR_UL_DL 0

#define MAX_APN_AMBR_EXT_UL_DL 250
#define MIN_APN_AMBR_EXT_UL_DL 0

#define MAX_APN_AMBR_EXT2_UL_DL 254
#define MIN_APN_AMBR_EXT2_UL_DL 0

#define MIN_PACKET_FLOW_IDENTIFIER 0
#define MAX_PACKET_FLOW_IDENTIFIER 127

//byte nas_msg_lib_cnt = 0;
/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
typedef enum
{
  RELIABILITY_CLASS_INITIAL_BIT =0,
  UNUSED =1,
  UNACK_GTP_ACK_LLC_RLC_PROTECT_DATA = 2,
  UNACK_GTP_LLC_ACK_RLC_PROTECT_DATA = 3,
  UNACK_GTP_LLC_RLC_PROTECT_DATA = 4,
  UNACK_GTP_LLC_RLC_UNPROTECT_DATA = 5
}qos_reliability_class;

typedef enum
{
  DELAY_CLASS_INITIAL_BIT =0,
  DELAY_CLASS_1 =1,
  DELAY_CLASS_2 =2,
  DELAY_CLASS_3 =3,
  DELAY_CLASS_4 =4,
  RESERVED_FOR_DELAY_CLASS = 7
}qos_delay_class;

typedef enum
{
  NAs_QOS_PRECEDENCE_CLASS_INITIAL_BIT =0,
  NAS_QOS_PREC_HIGH = 1,
  NAS_QOS_PREC_LOW =2,
  NAS_QOS_PREC_NORMAL =3,
  NAS_QOS_PRECEDENCE_CLASS_LAST_BIT =7
}qos_precedence_class;

typedef enum
{
  PEAK_THROUGHPUT_INITIAL_BIT =0,
  ONE_THOUSAND_OCTETS_PER_SEC =1,
  TWO_THOUSAND_OCTETS_PER_SEC,
  FOUR_THOUSAND_OCTETS_PER_SEC,
  EIGHT_THOUSAND_OCTETS_PER_SEC,
  SIXTEEN_THOUSAND_OCTETS_PER_SEC,
  THIRTY_TWO_THOUSAND_OCTETS_PER_SEC,
  SIXTY_FOUR_THOUSAND_OCTETS_PER_SEC,
  ONE_TWENTY_EIGHT_THOUSAND_OCTETS_PER_SEC,
  TWO_FIFTY_SIX_THOUSAND_OCTETS_PER_SEC
}qos_peak_throughputs;

typedef enum
{
  MEAN_THROUGHTPUT_INITIAL_BIT =0,
  ONE_HUNDRED_OCTETS_PER_HOUR = 1,
  TWO_HUNDRED_OCTETS_PER_HOUR = 2,
  FIVE_HUNDRED_OCTETS_PER_HOUR = 3,
  ONE_THOUSAND_OCTETS_PER_HOUR = 4,
  TWO_THOUSAND_OCTETS_PER_HOUR = 5,
  FIVE_THOUSAND_OCTETS_PER_HOUR = 6,
  TEN_THOUSAND_OCTETS_PER_HOUR = 7,
  ONE_MILLION_OCTETS_PER_HOUR = 8,
  TWO_MILLION_OCTETS_PER_HOUR = 9,
  FIVE_MILLION_OCTETS_PER_HOUR = 10,
  TEN_MILLION_OCTETS_PER_HOUR = 11,
  FIFTY_MILLION_OCTETS_PER_HOUR = 12,
  HUNDRED_MILLION_OCTETS_PER_HOUR = 13,
  TWO_HUNDRED_MILLION_OCTETS_PER_HOUR = 14, 
  FIVE_HUNDRED_MILLION_OCTETS_PER_HOUR = 15,
  ONE_BILLION_OCTETS_PER_HOUR =16,
  TWO_BILLION_OCTETS_PER_HOUR =17,
  FIVE_BILLION_OCTETS_PER_HOUR =18,
  FINAL_RESERVED_MEAN_THROUGHTPUT =62,
  BEST_EFFORT =63
  
}qos_mean_throughput;


typedef enum
{
 SDU_DELIVERY_INITIAL_BIT =0,
 NOT_DETECTED =1,
 ERRONEOUS_SDU_DELIVERED =2,
 CORRECT_SDU_DELIVERED =3,
 SDU_DELIVERY_LAST_BIT =7

}qos_erroneous_sdu_delivery;


typedef enum
{
  DEIVERY_CLASS_INITIAL_BIT =0,
  DELIVERED_IN_ORDER = 1,
  NOT_DELIVERED_IN_ORDER =2,
  DELIVERY_CLASS_LAST_BIT =7

}qos_delivery_class;



//boolean qos_signalling_indication =  FALSE;

typedef enum
{
  MAX_SDU_LEN_INITIAL_BIT =0,
  TEN_OCTETS= 150,
  ONE_FIVE_ZERO_TWO_OCTETS =151,
  ONE_FIVE_ONE_ZERO_OCTETS = 152,
  ONE_FIVE_TWO_ZERO_OCTETS = 153,
  MAX_SDU_LEN_LAST_BIT =255

}qos_max_sdu_len;

typedef enum
{
  QCI_ZERO = 0,
  QCI_ONE,
  QCI_TWO,
  QCI_THREE,
  QCI_FOUR,
  QCI_FIVE,
  QCI_SIX,
  QCI_SEVEN,
  QCI_EIGHT,
  QCI_NINE = 9

}qos_class_identifier;

typedef enum /*This enum holds good for Uplink, Downlink, Guaranteed Uplink and Guaranteed Downlink*/
{
  RESERVED_LTE =0,  
  ONE_KBPS = 1,
  SIXTY_THREE_KBPS = 63,

  SIXTY_FOUR_KBPS = 64,
  FIVE_sIXTY_EIGHT_KBPS = 127,

  FIVE_SEVENTY_SIX = 128,
  EIGHT_THOUSAND_SIX_HUNDRED_FORTY_KBPS = 254,

  ZERO_KBPS = 255

}qos_max_uplink_downlink_gbr;

#endif

#endif /*FEATURE_LTE*/
