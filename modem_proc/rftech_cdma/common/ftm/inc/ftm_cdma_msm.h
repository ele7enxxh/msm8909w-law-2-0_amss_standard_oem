#ifndef FTM_CDMA_MSM_H
#define FTM_CDMA_MSM_H

/*!
  @file
  ftm_cdma_msm.h
 
  @brief
  RF Driver's Common CDMA Modem HAL.

  @details
  This file includes definitions and prototypes used by the CDMA Modem HAL.
 
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/inc/ftm_cdma_msm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/22/14   JJ      Added ftm_cdma_msm_get_curr_rtc_cx1()
03/18/12   jfc     Remove tramp.h references
10/21/11   APU     Removed ftm_cdma_rxf_configure_dbg_bus() as it will not 
                   supported.     
05/18/11   aro     Removed CDMA timer to use common timer
03/31/11   aro     Moved IQ Capture and RxF Debug interfaces to FTM HAL
03/04/11   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_cdma_carrier_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  List of RXF debug Out Test Points
*/
typedef enum
{
  /* Start of test Points for WB Section : No NB Test points here */
  RXF_WB_ALL_ZERO = 0, /*!< all '0's */
  RXF_WB_Y1I_Y2I_Y1Q_Y2Q = 1, /*!< rx_y1i & rx_y2i & rx_y1q & rx_y2q */
  RXF_WB_ECI_ECQ = 2, /*!< ec_out_i & ec_out_q */
  RXF_WB_DEC1I_DEC1Q = 3, /*!< dec1_out_i & dec1_out_q */
  RXF_WB_DEC2I_DEC2Q = 4,/*!< dec2_out_i & dec2_out_q */
  RXF_WB_SHIFTER_I = 5, /*!< shifter_out_i */
  RXF_WB_SHIFTER_Q = 6, /*!< shifter_out_q */
  RXF_WB_DC_I = 7, /*!< wbdc_out_i */
  RXF_WB_DC_Q = 8, /*!< wbdc_out_q */
  RXF_WB_IQMC_I = 9, /*!< iqmc_out_i */
  RXF_WB_IQMC_Q = 10, /*!< iqmc_out_q */

  /* Start of test Points for NB Section : No WB Test points here */
  RXF_NB_ALL_ZERO = 20, /*!< all'0's */
  RXF_NB_SRC_I = 21, /*!< src_out_i */
  RXF_NB_SRC_Q = 22, /*!< src_out_q */
  RXF_NB_DC_I = 23, /*!< nbdc_out_i */
  RXF_NB_DC_Q = 24, /*!< nbdc_out_q */
  RXF_NB_DVGAI_DVGAQ = 25, /*!< dvga_out_i & dvga_out_q */
  RXF_NB_IQMUXI_IQMUXQ = 26, /*!< iqmuc_out_i & iqmux_out_q */
  RXF_NB_GDAI_GDAQ = 27 /*!< gda_out_i & gda_out_q */
} ftm_cdma_rxf_dbg_out_type;

/*----------------------------------------------------------------------------*/
boolean
ftm_cdma_msm_get_rx_iq_samples
(
  rfm_device_enum_type device,
  rfm_cdma_carrier_type carrier,
  unsigned short num_iq_samples,
  uint16 *buffer
);

/*----------------------------------------------------------------------------*/
uint32
ftm_cdma_msm_get_curr_rtc_cx1
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_CDMA_MSM_H */

