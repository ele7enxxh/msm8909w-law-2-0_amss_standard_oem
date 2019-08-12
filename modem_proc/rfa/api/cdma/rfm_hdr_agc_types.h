#ifndef RFM_HDR_AGC_TYPES_H
#define RFM_HDR_AGC_TYPES_H

/*!
  @file
  Definitions of types pertaining to RF devices.

  @details
  This file exports the definitions of types that are used in the RFM
  interface relating to RF device parameters.
*/

/*==============================================================================

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/rfm_hdr_agc_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/02/12   hdz     Added RFM_HDR_AGC_MODE_ENABLE_DIV_NORMAL  
11/11/11   bmg     Added a special diversity enable HDR AGC mode to decrease
                   acquisition time.
07/18/11   aro     Renamed RESUME agc mode to RESUME_INIT
06/14/11   sty     Deleted rfm_hdr_agc_format_type - instead, use 
                   rf_cdma_agc_format_type format
02/04/11   sty     Fixed doxygen warnings
01/18/11   aro     Renamed AGC ACQ Enums
12/20/10   aro     Initial version

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  1xEV-DO AGC modes
*/
typedef enum
{
  RFM_HDR_AGC_MODE_NORMAL,
  /*!< Acquisition bandwidth starting from G4 using 16 dispatches */

  RFM_HDR_AGC_MODE_FAST,
  /*!< Acquisition bandwidth starting from G4 using approximate system time and 
  10 dispatches */

  RFM_HDR_AGC_MODE_RESUME_WITH_NEW,
  /*!< Acquisition bandwidth starting from a known last AGC value from 1xL1. */

  RFM_HDR_AGC_MODE_RESUME_WITH_CURR,
  /*!< HDR RxAGC Mode in which RxAGC will be resumed normally with 
  AGC accumulators reset last known value in FW side. This will be used for
  cases like intelliceiver update */

  RFM_HDR_AGC_MODE_ENABLE_DIV,
  /*!< Rx AGC will start with the provided power on the diversity chain
  which is expected to be initialized from the primary chain. The RTC
  offset paraeter will be ignored, under the assumption that the primary
  chain is already time tracking correctly. */ 

  RFM_HDR_AGC_MODE_ENABLE_DIV_NORMAL,
  /*!< Rx AGC will start with the provided power on the diversity chain
  which is expected to be initialized from the primary chain. This mode 
  assume finger is not assigned and need 16 dispatch to finish RxAGC*/ 
  
  RFM_HDR_AGC_MODE__NORMAL_ACQUISITION = RFM_HDR_AGC_MODE_NORMAL,
  /*!< Acquisition bandwidth starting from G4 using 16 dispatches */

  RFM_HDR_AGC_MODE__FAST_ACQUISITION = RFM_HDR_AGC_MODE_FAST,
  /*!< Acquisition bandwidth starting from G4 using approximate system time and 
  10 dispatches */

  RFM_HDR_AGC_MODE__RESUME_ACQUISITION = RFM_HDR_AGC_MODE_RESUME_WITH_NEW,
  /*!< Acquisition bandwidth starting from a known last AGC value. */

  //RFM_HDR_AGC_MODE__TRACKING, /*!< Tracking bandwidth */

  //RFM_HDR_AGC_MODE__STOPPED /*!< HDR AGC is disabled */

} rfm_hdr_agc_mode_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  AGC parameters used in HDR fast AGC acquistion mode.
*/
typedef struct
{
  uint32 agc_rtc_offset; /*!< An estimate of HDR system time to improve AGC settling time. */
} rfm_hdr_agc_fast_acq_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  AGC parameters used in HDR resume AGC acquistion mode.
*/
typedef struct
{
  uint32 agc_rtc_offset; /*!< An estimate of HDR system time to improve AGC settling time. */
  int16 prev_agc; /*!< An estimate of the starting AGC value to improve the AGC settling time. */
} rfm_hdr_agc_resume_acq_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  AGC parameters used in HDR diversity enable AGC acquisition mode.
*/
typedef struct
{
   /*! An estimate of the starting AGC value to improve the AGC settling time. */
  int16 initial_agc;
} rfm_hdr_agc_div_enable_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Object to specify what AGC acquistion mode to use during 
  rfm_hdr_wakeup_rx(), and any parameters that go with the requested
  mode.
*/
typedef struct
{
  rfm_hdr_agc_mode_type mode; /*!< The AGC acquisition mode to use during wakeup. */
  union
  {
    rfm_hdr_agc_fast_acq_type fast_acq; /*!< Parameters used if the mode is RFM_HDR_AGC_MODE__FAST_ACQUISITION. */
    rfm_hdr_agc_resume_acq_type resume_acq; /*!< Parameters used if the mode is RFM_HDR_AGC_MODE__RESUME_ACQUISITION. */
    rfm_hdr_agc_div_enable_type div_acq; /*!< Parameters used if the mode is RFM_HDR_AGC_MODE_ENABLE_DIV */
  } params; /*!< Mode specific parameters. */
} rfm_hdr_agc_acq_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Current and recent minimum and maximum 1xEV-DO Rx AGC.

  @details
  This structure is used to capture the recent Rx AGC range when
  1xEV-DO is going to sleep and preparing for a RESUME_ACQUISITION
  on the next wakeup.
*/
typedef struct
{
  int16 last_agc; /*!< Last good AGC value */
  int16 min_agc; /*!< Lowest AGC value since AGC tracking was started */
  int16 max_agc; /*!< Largest AGC value since AGC tracking was started */
} rfm_hdr_agc_info_struct_type;


#ifdef __cplusplus
}
#endif

#endif /* RFM_HDR_AGC_TYPES_H */


