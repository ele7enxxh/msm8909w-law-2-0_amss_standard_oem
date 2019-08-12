#ifndef WL1GSMIF_H
#define WL1GSMIF_H

/*===========================================================================
                WL1 GSM interface header file

DESCRIPTION
  This file is WL1 RF Driver interface header file that supports multimode
  external interface while maintaning backward compatibility. This file
  contains all declarations and definitions necessary for WL1 to use the
  RF drivers. WL1 modules call inline function in this and this takes care of
  all RF API changes featurization.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None. All are passive inline function calling RF modules based on feature.
  All initialization requirment of respective RF modules should be followed

Copyright (c) 2006 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1gsmif.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
---------------------------------------------------------------------------------------------------------
06/24/14   cc      Moving G2W drivers to WL1 folders
04/10/14   cc      Fix compilation error
02/26/14   cc      Initiate this file
===========================================================================*/

#include "wcdma_variation.h"
#include "gtmrs.h"
#include "gl1_hw.h"

/* =======================================================================
**                        Macros
** ======================================================================= */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================

FUNCTION  wl1_gsm_init

DESCRIPTION
  Function to initialize the GSM Layer 1 interfaces.  
  Interfaces include GSTMR, MDSP, RF.
  This function should be invoked whenever the WCDMA stack initializes.
  It also registers a TDMA frame processing handler.


  task_wait_handler - Needed by the RF component of hardware to implement
  a polling mechanism on the calling tasks context.

  task_wait_sig     - Signal used to notify the calling task that the
  RF driver is ready to proceed.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_init
(
  void          (* tdma_isr_ptr)(sys_modem_as_id_e_type as_id),
  rex_tcb_type  *task_ptr,
  void          (* task_wait_handler)(rex_sigs_type),
  rex_sigs_type task_wait_sig,
  rex_sigs_type task_wait_sig_1,
  sys_modem_as_id_e_type as_id
)
{

  gl1_hw_cm_gsm_init(NULL, (rex_tcb_type*)task_ptr,
                     NULL, (rex_sigs_type)task_wait_sig,
                     (rex_sigs_type)task_wait_sig_1
                     #ifdef FEATURE_DUAL_SIM
                     , as_id
                     #endif
                     );
}

/*===========================================================================

FUNCTION  wl1_gsm_get_FN

DESCRIPTION
  This function retrieves the GSTMR frame number

DEPENDENCIES
  None

RETURN VALUE
  uint32 - GSTMR frame number

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint32 wl1_gsm_get_FN
(
  sys_modem_as_id_e_type as_id
)
{
  return gl1_cm_get_FN(as_id);
}

/*===========================================================================

FUNCTION  wl1_gsm_set_fn

DESCRIPTION
  This function sets the GSTMR frame number to the specified ammount.
  This is for "slamming" - increments are handled by gstmr_fn_handler()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_set_fn
( 
  /* GSTMR FN to set */
  uint32 new_fn, 
  sys_modem_as_id_e_type as_id
)
{
  gl1_cm_set_fn(new_fn, as_id);
}


/*===========================================================================

FUNCTION wl1_gsm_gstmr_rd_qsymbol_count

DESCRIPTION
  This function reads qsymbol value from GL1

DEPENDENCIES

RETURN VALUE
  uint32 - qsymbol value from GL1

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint32 wl1_gsm_gstmr_rd_qsymbol_count
( 
  sys_modem_as_id_e_type as_id  
)
{
  return gstmr_rd_qsymbol_count(
                                #ifdef FEATURE_DUAL_SIM
                                as_id
                                #endif
                                );
}

/*===========================================================================

FUNCTION wl1_gsm_gstmr_rd_hw_fn_qs_geran

DESCRIPTION
  This function reads qsymbol value from GL1

DEPENDENCIES

RETURN VALUE
  uint32 - qsymbol value from GL1

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
gsmtr_fn_qs_type wl1_gsm_gstmr_rd_hw_fn_qs
( 
  sys_modem_as_id_e_type as_id  
)
{
  return gstmr_rd_hw_fn_qs(
                           #ifdef FEATURE_DUAL_SIM
                           as_id
                           #endif
                           );
}

/*===========================================================================

FUNCTION wl1_gsm_gstmr_rd_hw_frame_count

DESCRIPTION
  This function reads the HW GSM hyperframe counter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint32 wl1_gsm_gstmr_rd_hw_frame_count
( 
  sys_modem_as_id_e_type as_id 
)
{
  return gstmr_rd_hw_frame_count(
                                 #ifdef FEATURE_DUAL_SIM
                                 as_id
                                 #endif
                                 );
}

/*===========================================================================

FUNCTION wl1_gsm_gstmr_rd_hw_frame_count_from_xo

DESCRIPTION
  This function reads the HW GSM hyperframe counter from xo value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_gstmr_rd_hw_frame_count_from_xo
( 
  /* VSTMR XO cnt */
  uint32 xo_cnt, 
  /* Hyper frame number mod by 51 returned from GL1 */
  uint32 *hwfn, 
  /* Qsymbol cnt returned from GL1 */
  uint32 *qsymbol_cnt,
  sys_modem_as_id_e_type as_id
)
{
  gstmr_rd_hw_frame_count_from_xo(xo_cnt,
                                  hwfn,
                                  qsymbol_cnt
                                  #ifdef FEATURE_DUAL_SIM
                                  , as_id
                                  #endif
                                  );
}

/*===========================================================================

FUNCTION wl1_gsm_gstmr_is_handler_registered

DESCRIPTION
  This function checks the handler table to see if the requested handler
  is present.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_gsm_gstmr_is_handler_registered
( 
  /* Function pointer to be checked */
  void *handler_ptr,
  sys_modem_as_id_e_type as_id  
)
{
  #ifdef FEATURE_DUAL_SIM  
  return gstmr_is_handler_registered((gstmr_handler_type)handler_ptr,
                                     as_id);
  #else
  return gstmr_is_handler_registered((gstmr_handler_multirat_type)handler_ptr);
  #endif
}

/*===========================================================================

FUNCTION wl1_gsm_gstmr_register_handler

DESCRIPTION
     This function allows the application code to register

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_gstmr_register_handler
( 
  /* Function pointer to be registered to GSTMR handler */
  void *handler_ptr,
  sys_modem_as_id_e_type as_id
)
{
  #ifdef FEATURE_DUAL_SIM
  gstmr_register_handler((gstmr_handler_type)handler_ptr,
                         as_id);
  #else
  gstmr_register_handler((gstmr_handler_multirat_type)handler_ptr);
  #endif
}

/*===========================================================================

FUNCTION wl1_gsm_gstmr_deregister_handler

DESCRIPTION
  This function allows the application code to remove an existing handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_gstmr_deregister_handler
( 
  /* Function pointer to be deregistered to GSTMR handler */
  void *handler_ptr,
  sys_modem_as_id_e_type as_id
)
{
  #ifdef FEATURE_DUAL_SIM
  gstmr_deregister_handler((gstmr_handler_type)handler_ptr,
                           as_id);
  #else
  gstmr_deregister_handler((gstmr_handler_multirat_type)handler_ptr);
  #endif
}

/*===========================================================================

FUNCTION wl1_gsm_gstmr_wakeup_init

DESCRIPTION
  This function modifies necessary registers after in wake-up

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_gstmr_wakeup_init
(
  sys_modem_as_id_e_type as_id
)
{
  gstmr_wakeup_init(
                    #ifdef FEATURE_DUAL_SIM
                    as_id
                    #endif
                    );
}

/*===========================================================================

FUNCTION wl1_gsm_gstmr_sleep_init

DESCRIPTION
  This function modifies necessary registers before going to sleep

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_gstmr_sleep_init
(
  sys_modem_as_id_e_type as_id
)
{
  gstmr_sleep_init(
                   #ifdef FEATURE_DUAL_SIM
                   as_id
                   #endif               
                   );
}

/*===========================================================================

FUNCTION wl1_gsm_gstmr_enable_odd_even_check

DESCRIPTION
  This function is to realign the OE check info for the frame after a failed asynch int

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_gstmr_enable_odd_even_check
(
  sys_modem_as_id_e_type as_id
)
{
  GSTMR_ENABLE_ODD_EVEN_CHECK(
                              #ifdef FEATURE_DUAL_SIM
                              as_id
                              #endif
                              );
}

/*===========================================================================

FUNCTION wl1_gsm_gstmr_gsm_core_reset

DESCRIPTION
   This function effectively resets the entire GSM core.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_gstmr_gsm_core_reset( void )
{
  gstmr_gsm_core_reset();
}

/*===========================================================================

FUNCTION  wl1_gsm_is_gfw_error_detected

DESCRIPTION
  This function is to check if GFW is in a bad state or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE - Error
  FALSE - no error
  
SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_gsm_is_gfw_error_detected
( 
  sys_modem_as_id_e_type as_id 
)
{
  
  return gl1_is_gfw_error_detected(
                                   #ifdef FEATURE_DUAL_SIM
                                   as_id 
                                   #endif
                                  );
} 

/*===========================================================================

FUNCTION  wl1_gsm_is_gsm_fw_awake

DESCRIPTION
  This function is to check if GSM is awake

DEPENDENCIES
  None

RETURN VALUE
  TRUE - Awake
  FALSE - not awake

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_gsm_is_gsm_fw_awake
( 
  sys_modem_as_id_e_type as_id
)
{
  return gl1_hw_cm_is_gsm_fw_awake(
                                   #ifdef FEATURE_DUAL_SIM
                                   as_id
                                   #endif
                                  );
}


/*===========================================================================

FUNCTION wl1_gsm_set_async_rx_search_width

DESCRIPTION
 Sets the Async RX Search Width.
 Use of Search Width:
 Search width is divided by two and appended to each side of SCH burst length

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE
#endif 
void wl1_gsm_set_async_rx_search_width 
(
  /* Search width for Async Rx */
  uint16 width,
  sys_modem_as_id_e_type as_id
)
{
  gl1_hw_set_async_rx_search_width(width
                                   #ifdef FEATURE_DUAL_SIM
                                   , as_id
                                   #endif
                                   );
}

/*===========================================================================

FUNCTION wl1_gsm_wpll_time_us

DESCRIPTION
  Returns the W2G W PLL settling time in us

DEPENDENCIES
  None

RETURN VALUE
  uint16 - W PLL settling time in us

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint16  wl1_gsm_wpll_time_us( void )
{
  return gl1_hw_cm_wpll_time_us();
}

/*===========================================================================

FUNCTION  wl1_gsm_was_tone_detected

DESCRIPTION
  This function determines if a tone was detected and returns the tone
  offset returned by teh DSP if the FCCH was successfully decoded

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If tone was detected
  FALSE - If tone was not detected

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif 
boolean wl1_gsm_was_tone_detected
(
   /* ARFCN */
   ARFCN_T arfcn,
   /* Result ID */
   uint16 *id,
   /* FCCH tone offset */
   int32  *tone_offset,
   /* Coarse Freq */
   uint16 *coarse_freq,
   /* Fine Freq */
   uint16 *fine_freq,
   /* AFC Freq */
   uint16 *afc_freq,
   /* Cell SNR */
   uint16 *snr,
   sys_modem_as_id_e_type as_id
)
{
  return gl1_hw_was_tone_detected(arfcn,
                                  id,
                                  tone_offset,
                                  coarse_freq,
                                  fine_freq,
                                  afc_freq,
                                  snr
                                  #ifdef FEATURE_DUAL_SIM
                                  , as_id
                                  #endif
                                  );
}

/*===========================================================================

FUNCTION  wl1_gsm_was_sch_decoded

DESCRIPTION
  This function polls the MDSP buffers to verify whether there are
  SCH results available.  The availability of results indicates that
  the MDSP has detected and decoded an SCH.  The CRC is a strong indicator
  that the SCH burst is decoded correctly.  The SCH burst contains
  a 10-bit CRC.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If tone was detected
  FALSE - If tone was not detected

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif 
boolean wl1_gsm_was_sch_decoded
(
   /* ARFCN */
   ARFCN_T arfcn,
   /* Result ID */
   uint16 *id,
   /* SCH burst start time offset */
   int32  *time_offset,
   /* SCH decoded data */
   uint8  *sch_data,
   /* MDSP payload */
   uint16 *mdsp_payload,
   /* SCH data CRC */
   uint8  *crc_pass,
   sys_modem_as_id_e_type as_id
)
{
  return gl1_hw_was_sch_decoded(arfcn,
                                id,
                                time_offset,
                                sch_data,
                                mdsp_payload,
                                crc_pass
                                #ifdef FEATURE_DUAL_SIM
                                , as_id
                                #endif 
                                );
}

/*===========================================================================

FUNCTION wl1_gsm_get_async_rx_metrics

DESCRIPTION
  This function retrieves the metrics for the async burst received in the
  previous frame.
  Sch flag is true for the ncell sch and it will false for the ccch decoding

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_gsm_get_async_rx_metrics
(
   gl1_defs_metrics_rpt    *rpt,
   boolean                 last_time,
   gl1_hw_rx_tags_type     *tags,
   boolean                 sch_flag,
   sys_modem_as_id_e_type  as_id
)
{
  return  gl1_hw_get_async_rx_metrics(rpt,
                                      last_time,
                                      tags,
                                      sch_flag
                                      #ifdef FEATURE_DUAL_SIM
                                      , as_id
                                      #endif
                                      );
}

/*===========================================================================

FUNCTION  wl1_gsm_get_async_rx_data

DESCRIPTION
  This function attempts to retrieve data from the mdsp buffers for an
  async burst.

DEPENDENCIES
  None

RETURN VALUE
  TRUE   - a block has been dedoded (the CRC may have passed or failed)
  FALSE  - no block has been decoded

SIDE EFFECTS
  If last_time is TRUE or this functions returns TRUE then the data for
  this block cannot be retrieved again.

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_gsm_get_async_rx_data
(
   gl1_hw_channel_type      chan,
   gl1_defs_rx_hdr_struct   *rpt_hdr,
   uint8                    *data,
   uint16                   data_len,
   boolean                  last_time,
   sys_modem_as_id_e_type   as_id
)
{
  return gl1_hw_get_async_rx_data(chan,
                                  rpt_hdr,
                                  data,
                                  data_len,
                                  last_time
                                  #ifdef FEATURE_DUAL_SIM
                                  , as_id
                                  #endif
                                  );
}

/*===========================================================================

FUNCTION  wl1_gsm_startup

DESCRIPTION
  Calls to invoke in the beginning and end of gap

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void  wl1_gsm_startup
( 
  uint16 offset,
  const gl1_hw_cm_rf_info_t* cm_rf_info_ptr,
  sys_modem_as_id_e_type as_id
)
{
  gl1_hw_cm_startup(offset, cm_rf_info_ptr
                    #ifdef FEATURE_DUAL_SIM
                    , as_id
                    #endif
                    );
}

/*===========================================================================

FUNCTION  wl1_gsm_cleanup

DESCRIPTION
  Calls to invoke in the beginning and end of gap

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_cleanup 
( 
  uint16 offset,
  sys_modem_as_id_e_type as_id
)
{
  gl1_hw_cm_cleanup(offset
                    #ifdef FEATURE_DUAL_SIM
                    , as_id
                    #endif
                    );
}

/*===========================================================================

FUNCTION  wl1_gsm_power_measure

DESCRIPTION
  This function initiates one to nine power measurements in the next frame.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_power_measure
(
  ARFCN_T                     *ARFCN,              
  int16                       *rx_signal_strength,  
  uint8                       num,                                           
  uint16                      *offset,              
  gl1_hw_burst_position_type  *position,            
  gl1_hw_rx_tags_type         *tags,
  sys_modem_as_id_e_type      as_id
)
{
    gl1_hw_cm_power_measure(
      ARFCN,
      rx_signal_strength,
      num,
      offset,
      position,
      tags
      #ifdef FEATURE_DUAL_SIM
      , as_id
      #endif
      );
}

/*===========================================================================

FUNCTION  wl1_gsm_get_power_measure_results

DESCRIPTION
  Returns 9 rssi values.

DEPENDENCIES
  None

RETURN VALUE
  samples       - array of 9 measured powers (received signal strength)
  num           - number of results to process. This number should match
                  the num parameter used in calling the
                  gl1_hw_cm_power_measure() function.
  tags          - Pointers to where the gain will be stored

  (It is the responsiblity of the calling function to provide storage for
   results, regardless of how many bursts were recieved.  It is also
   the responsibility of the calling function to know how many bursts were
   received.)

  TRUE  - power results were available
  FALSE - power results were not available

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_gsm_get_power_measure_results
(
   gl1_hw_measurement_T   *samples,
   uint8                  num,
   gl1_hw_rx_tags_type    *tags,
   sys_modem_as_id_e_type as_id
)
{
  return gl1_hw_cm_get_power_measure_results(samples,
                                             num,
                                             tags
                                             #ifdef FEATURE_DUAL_SIM
                                             , as_id
                                             #endif
                                             );
}

/*===========================================================================

FUNCTION  wl1_gsm_start_acquisition

DESCRIPTION
  This function initiates a tone detection on the specified frequency
  at the specified offset into the frame and at the specified
  received signal level

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_start_acquisition
(
   ARFCN_T ARFCN,
   int16   rx_signal_strength,
   uint16  timing_offset,
   int16   frequency_offset,
   boolean sch_decode,
   gl1_hw_acq_afc_type AFC,
   gl1_hw_rx_tags_type *tags,
   sys_modem_as_id_e_type as_id
)
{
    gl1_hw_start_acquisition(
      ARFCN,
      rx_signal_strength,
      timing_offset,
      frequency_offset,
      sch_decode,
      AFC,
      tags
      #ifdef FEATURE_DUAL_SIM
      , as_id
      #endif
      );
}

/*===========================================================================

FUNCTION  wl1_gsm_stop_acquisition

DESCRIPTION
  This function stops the tone detection at the specified offset into the
  frame. The offset does not include the DSP's frame tick setup margin

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_stop_acquisition_part1
( 
  uint16 offset,
  sys_modem_as_id_e_type as_id
)
{
  gl1_hw_stop_acquisition_part1(offset
                                #ifdef FEATURE_DUAL_SIM                          
                                , as_id
                                #endif
                                );
}
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_stop_acquisition_part2 (void)
{
  gl1_hw_stop_acquisition_part2();
}

/*===========================================================================

FUNCTION  wl1_gsm_rx_async_sch_burst

DESCRIPTION
  This function initiates an asynchronous burst receive process for a SCH
  burst.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_rx_async_sch_burst
(
  ARFCN_T  ARFCN,
  int16    signal_strength,
  uint16   timing_offset,
  int16    frequency_offset,
  gl1_hw_rx_tags_type *tags,
  sys_modem_as_id_e_type as_id
)
{
    gl1_hw_rx_async_sch_burst(
      ARFCN,
      signal_strength,
      timing_offset,
      frequency_offset,
      tags
      #ifdef FEATURE_DUAL_SIM
      , as_id
      #endif
      );
}

/*===========================================================================

FUNCTION wl1_gsm_terminate

DESCRIPTION
  Cleanup the gl1_hw_cm layer interface.

DEPENDENCIES
  gl1_hw_cm_gsm_init() must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_gsm_terminate
( 
  sys_modem_as_id_e_type as_id 
)
{
  gl1_hw_cm_gsm_terminate(
                          #ifdef FEATURE_DUAL_SIM
                          as_id
                          #endif
                         );
}


#endif /* WL1GSMIF_H */

