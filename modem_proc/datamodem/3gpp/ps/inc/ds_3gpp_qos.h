
/*!
  @file
  ds_3gpp_qos.h

  @brief
  Common Qos functions and definitions.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_qos.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/12   ss       3GPP MH LTE-Off compilation fixes
02/18/10   vd       Initial version.
===========================================================================*/

#ifndef DS3GPPQOS_H
#define DS3GPPQOS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "dsumtspsqos.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define  QOS_LTE_MAX_DL_KBIT_RATE                  150000
#define  QOS_LTE_MAX_UL_KBIT_RATE                  50000



#ifdef FEATURE_DATA_LTE
#define QOS_3GPP_MAX_DL_KBIT_RATE  QOS_LTE_MAX_DL_KBIT_RATE
#define QOS_3GPP_MAX_UL_KBIT_RATE  QOS_LTE_MAX_UL_KBIT_RATE
#else
#define QOS_3GPP_MAX_DL_KBIT_RATE  QOS_MAX_DL_KBIT_RATE
#define QOS_3GPP_MAX_UL_KBIT_RATE  QOS_MAX_UL_KBIT_RATE
#endif /* FEATURE_DATA_LTE */

#define DS_TFT_MAX_FILTERS                    16

/*---------------------------------------------------------------------------
  The enum type for TFT packet filter direction  
  (per 3GPP TS24.008 10.5.6.12)
---------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_TFT_PRE_REL_7        = 0,    /* PRE REL 7 Filter                 */
  DS_3GPP_TFT_DOWNLINK         = 1,    /* Downlink only Filter             */
  DS_3GPP_TFT_UPLINK           = 2,    /* Uplink only Filter               */
  DS_3GPP_TFT_BIDIRECTIONAL    = 3     /*  Bi-Directional Filter           */
} ds_tft_pkt_filter_direction_enum_type;

#endif /* DS3GPPQOS_H */

