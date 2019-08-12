#ifndef _HDR_CONST_H_
#define _HDR_CONST_H_

/*!
  @file
  hdrfw_const.h

  
  Defines the HDR firmware constants the HDRFW module

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_c2k/api/hdrfw_const.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/


#define HDRFW_NUM_FINGERS                       8
#define HDRFW_NUM_FINGERS_PER_ASP               4
#define HDRFW_DEMOD_CARRIERS                    3
/*! \brief This is number of physical carriers supported. */
#define HDRFW_TOTAL_CARRIERS                    3
#define HDRFW_NUM_ANTENNAS                      2
#define HDRFW_NUM_ASP                           6
#define HDRFW_NUM_CELLS                         HDRFW_NUM_ASP       
#define HDRFW_NUM_DRC                           28
#define HDRFW_NUM_EQ_PASSESS                    (1+3+1)
#define HDRFW_DECOB_MAX_PACKETS                 24
#define HDRFW_PKT_HEADER_SIZE                   3
#define HDRFW_LOG_BUFFERS                       2
#define HDRFW_LOG_SLOTS                         4
#define HDRFW_LOG_SLOTS_IN_FRAME                16
#define HDRFW_LOG_DGAIN_HS                      2
#define HDRFW_NUM_AVGA_SSBI_WRITES              4
/*! \brief # of NOTCH filters */
#define HDRFW_NUM_RF_NOTCH_FILTERS               2

#define HDRFW_NUM_TIMERS                         4

#define HDRFW_EPT_MAX_NUM_RGI                    CFW_EPT_MAX_NUM_RGI 

/*! \brief Depth of FAB result buffer that are for FAC gain estimation */
#define HDRFW_FAC_GAIN_EST_DEPTH                 12

/*! \brief Size in word of each decob packet in mempool. */
#define HDRFW_DECODE_SINGLE_PACKET_SIZE_IN_WORD   (260)

/*! \brief PA Backoff related definations */
#define HDRFW_PA_BACKOFF_LUT_SIZE  8

/*! \brief Maximum finger deskew depth in CX1 that is supported. This depth
 * is defined as the distance from the earliest finger of all carriers to
 * the latest finger of all carriers. */
#define HDRFW_MAX_FINGER_DESKEW_DEPTH_CX1     (380)

#endif
