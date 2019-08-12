/*!
  @file cdmafw_logging.h

  @brief External message interface for the CDMA1x FW module

*/

/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_c2k/api/cdmafw_logging.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/
#ifndef CDMAFW_LOGGING_H
#define CDMAFW_LOGGING_H

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

// two ping-pong log buffers.
#define CDMAFW_LOG_NUM_BUFFERS      2
// 8 PCG worth of data (one set for each half-PCG) a
#define CDMAFW_LOG_NUM_HALF_PCGS    (8*2)
// 6 items per half-PCG.
#define CDMAFW_LOG_NUM_ITEMS        6
// log buffer size = 96
#define CDMAFW_LOG_BUFFER_SIZE      (CDMAFW_LOG_NUM_HALF_PCGS*CDMAFW_LOG_NUM_ITEMS)

#endif
