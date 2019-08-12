#ifndef FTM_1X_RADIO_H
#define FTM_1X_RADIO_H

/*!
  @file
  ftm_1x_radio.h

  @brief
  This module contains defintions and prototypes. FTM 1x Radio Control 
  code which is used to exclusively control Radio for test purposes.
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/inc/ftm_1x_radio.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/08/14   JJ      Add a new FTM command for timed sleep rx 
01/18/13   aro     Removed nikel-XPT specific code
01/18/13   aro     Deleted nikel-XPT specific code
11/16/12   Saul    Sample Capture. Use common response.
11/08/12   cd      - Changed legacy override to FW override
09/19/12   Saul    CDMA Sample Capture. Capture type, do leakage flag
08/01/12   aro     Added EPT override Test function
03/18/12   aro     Moved common radio commands to common module
12/21/11   aro     Added 1x Enable and disable radio functions
11/12/11   aro     Moved CDMA common commands from 1x to CDMA module
10/13/11   bmg     FTM RFM interface support to set SAR state.
10/13/11   aro     Added interface to enable and disable various debug mode for
                   various debug modules
08/29/11   aro     Created a Driver verification sandbox
08/11/11   aro     Moved Cal v3 configure rx to be done under ISR context
08/08/11   aro     Added support to perform Rx configrue in FTM Task context
08/08/11   aro     Added interface to test Cal v3 sequence
08/03/11   aro     Added support for radio tear down for cal v3
08/03/11   aro     Added support for retune for cal v3
07/27/11   aro     Added function to configure radio for Cal v3
07/12/11   aro     Added support to query intelliceiver power mode
07/01/11   aro     Added Debug interface to override Power Mode
06/13/11   aro     Added 1x Tx Override command
05/16/11   aro     Added Clear LNA override interface
05/06/11   aro     Added 1x disable diversity radio control
05/06/11   aro     Added 1x enable diversity radio control
02/13/11   aro     Added Set Tx Power Interface
02/13/11   aro     [1] Updated LNA query interface
                   [2] Added support for Get TxAGC
02/12/11   aro     Deleted Get LNA state interface to merge with Get RxAGC
02/08/11   aro     Moved the 1x Radio control to FTM
02/04/11   sty     Deleted ftm_hwtc_1x_set_tx and ftm_hwtc_1x_start_tx
02/03/11   aro     Doxygen Warning Removal
01/27/11   aro     Added support for 1x retune
01/24/11   aro     Added LNA Range Query Interfaces
01/24/11   sty     Added ftm_hwtc_1x_set_tx
01/18/11   sty     Added ftm_hwtc_1x_radio_wakeup_tx
01/07/11   aro     Renamed function names to reflect 1x
01/06/11   aro     Added RxAGC Query function
01/05/11   aro     Added Response Packets for HWTC CDMA
01/05/10   aro     Added Debug interface for rfm_1x_enter()
12/30/10   aro     Initial Release : Based on SVDO Design

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmdiag.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_enter
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_enable
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_disable
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_wakeup_rx
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_retune
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_enable_diversity
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_disable_diversity
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_wakeup_tx
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_get_rx_agc
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_get_tx_agc
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_sleep_rx
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_sleep_tx
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_set_tx_power
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_set_tx_override
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_clear_lna_override
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_calv3_configure_radio
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_calv3_retune_radio
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_calv3_tear_down_radio
(
  void  *ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_radio_timed_sleep
(
   void  *ftm_req_data
);


#ifdef __cplusplus
}
#endif

#endif /*FEATURE_FACTORY_TESTMODE */

#endif /* FTM_1X_RADIO_H */
