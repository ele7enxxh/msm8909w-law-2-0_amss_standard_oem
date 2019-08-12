#ifndef RR_CHANNEL_MODE_H
#define RR_CHANNEL_MODE_H

/*! \file rr_channel_mode.h
 
  This is the header file for rr_channel_mode.c.
  This module contains functionality related to channel mode support assessment & modification.
 
                Copyright (c) 2007-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_channel_mode.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "customer.h"
#include "comdef.h"
#include "sys_cnst.h"
#include "aconfig.h"
#include "rr_defs.h"
#include "nas_exp.h"
#include "mm_rr.h"
#include "geran_multi_sim.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Sets the channel modes supported.
 * 
 * \param voice_calls_supported 
 * \param csdata_calls_supported 
 * \param as_id 
 */
extern void rr_channel_mode_set_supported(
  boolean voice_calls_supported,
  boolean csdata_calls_supported,
  sys_modem_as_id_e_type as_id
);

/*!
 * \brief Gets the current MS capability and half-rate support.
 * 
 * \param ms_caps_ptr (out) - updated with MS capabilities
 * \param hr_data_ptr (out) - updated with HF data support
 * \param gas_id (in)
 */
extern void rr_get_ms_capabilities(
  byte *ms_caps_ptr,
  boolean *hr_data_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Determines whether the specified channel mode is supported.
 * 
 * \param channel_mode (in)
 * \param gas_id (in)
 * 
 * \return boolean - TRUE is supported, FALSE if not supported
 */
extern boolean rr_channel_mode_supported(
  channel_mode_T channel_mode,
  const gas_id_t gas_id
);

/*!
 * \brief Process the Channel Mode Modify message received OTA.
 * 
 * \param gas_id 
 */
extern void rr_handle_channel_mode_modify_msg(
  const gas_id_t gas_id
);

/*!
 * \brief Processes the MPH_CHANNEL_MODE_MODIFY_CNF message from L1.
 * 
 * \param cmd_ptr 
 * \param gas_id 
 */
extern void rr_handle_channel_mode_modify_cnf(
  void *cmd_ptr,
  const gas_id_t gas_id
);

/*!
 * \brief Determines if the supplied mode is for voice.
 * 
 * \param curr_chan_mode 
 * 
 * \return boolean - TRUE if voice mode, FALSE if not voice mode
 */
extern boolean rr_channel_mode_is_voice(
  channel_mode_T curr_chan_mode
);

/*!
 * \brief Sets traffic reason based on channel mode
 * 
 * \param channel_mode
 * \param gas_id
 * 
 * \return void
 */
extern void rr_set_traffic_reason_from_channel_mode(
  const channel_mode_T channel_mode,
  const gas_id_t gas_id);

/*!
 * \brief Retrieves and formats the data for ENG_MODE_CODEC_HO_INFO, and updates the Eng Mode data.
 * 
 * \param curr_chan_mode 
 * \param new_chan_mode 
 * \param new_chan_type 
 * \param irat_ho 
 * \param cmd 
 * \param gas_id 
 */
extern void rr_eng_mode_update_codec_ho_info(
  channel_mode_T curr_chan_mode,
  channel_mode_T new_chan_mode,
  channel_type_T new_chan_type,
  boolean        irat_ho,
  sys_codec_ho_cmd cmd,
  sys_ho_type_e_type ho_type, 
  const gas_id_t gas_id
);


uint32 rr_eng_mode_get_speech_sample_frequency(sys_speech_codec_e_type speech_codec);
boolean rr_is_voice_call_active(const gas_id_t gas_id);

/*!
 * \brief Module initialisation function. This is called once at RR task start. 
 * N.B. All data-spaces are initialised.
 */
extern void rr_channel_mode_task_start_init(
  void
);

#endif /* RR_CHANNEL_MODE_H */

/* EOF */

