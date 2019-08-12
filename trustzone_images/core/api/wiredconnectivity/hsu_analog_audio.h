#ifndef _HSU_ANALOG_AUDIO_H_
#define _HSU_ANALOG_AUDIO_H_

/*=============================================================================

Analog Audio over High Speed USB
H E A D E R   F I L E
=============================================================================*/
/**
@file hsu_analog_audio.h

Contains the API for switching between Analog Audio USB and USB
normal mode.

Externalized functions:
- hsu_analog_audio_init
- hsu_analog_audio_config

Initialization and sequencing requirements:\n
hsu_analog_audio_init
*/

/*=============================================================================
Copyright (c) 2008-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_analog_audio.h
when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/24/10  lo   (Tech Pubs) Edited/added Doxygen comments and markup.
03/30/08 tli   First revision of file    
=============================================================================*/

/*=============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_common.h"

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================
Typedefs
=============================================================================*/
/** Defines the modes each core can be in.
*/
typedef enum{
  HSU_AUDIO_MODE_STEREO_SPK = 0,     /**< -- Stereo speakers. */
  HSU_AUDIO_MODE_STEREO_SPK_MIC = 1, /**< -- Stereo speakers and microphone. */
  HSU_AUDIO_MODE_MONO_SPK = 2,       /**< -- Mono speakers. */
  HSU_AUDIO_MODE_MONO_SPK_MIC = 3,   /**< -- Mono speakeres and microphone. */
  HSU_AUDIO_MODE_USB = 4,            /**< -- USB normal mode (default)
									         value. */
  HSU_AUDIO_MODE_INVALID = 5         /**< &nbsp; */
} hsu_analog_audio_mode_type ;

/** Callback to be called on completing the analog audio configuration.
    This type returns the result of the configuration process
	(success/failure).
*/
typedef void (*hsu_analog_audio_conf_cb_type)(boolean result);

/** Describes the parameter passed to hsu_analog_audio_config().
*/
typedef struct{
  hsu_analog_audio_mode_type new_mode;
  /**< The mode to switch to. */
  hsu_analog_audio_conf_cb_type completion_cb;
  /**< The callback to be called on completing the configuration. */
} hsu_analog_audio_conf_param_type ; 

/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_analog_audio_config
=============================================================================*/
/**
Switches between various audio modes and regular USB mode.

When switching from USB mode to Analog Audio mode, the USB core is stopped and 
the D+/D- signals are set to high-impedance mode. This function recieves the
hsu_analog_audio_conf_param_type parameter.

@note The function is not reentrant!

@param *param [IN] -- Audio configuration.

@dependencies
hsu_analog_audio_init() must be called prior to this function and should be
called from the AL task context.

@sideeffects
None.

@return
None
*/
void hsu_analog_audio_config(void *param);

/*=============================================================================
FUNCTION    hsu_analog_audio_init
=============================================================================*/
/**
Initializes internal module data structures.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_analog_audio_init(void);

/*=============================================================================
FUNCTION    hsu_analog_audio_get_curr_mode
=============================================================================*/
/**
Returns the current mode the USB core is in.

@dependencies
None.

@sideeffects
None.

@return
Current mode the USB core is in.
*/
hsu_analog_audio_mode_type hsu_analog_audio_get_curr_mode(void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* _HSU_ANALOG_AUDIO_H_ */
