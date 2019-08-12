#ifndef UIMUTIL_H
#define UIMUTIL_H
/*===========================================================================

        E X T E R N A L   U I M  U T I L   S T A T E   M A C H I N E
                            D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the UIM UTIL state machine.

Copyright (c) 2003, 2011-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
*****************************************************************************
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uimutil.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/16   lm      Iccid length is being filled from file size received in FCP of EF-ICCID
01/25/16   ssr     Delay local card power up in Modem SSR in BTSAP mode
11/11/15   sam     Explicit MF selection after call connect envelope command
10/16/15   sam     Added util function to check if a given AID is ARA-M aid
06/03/15   ks      Select MF after EF-PSLOCI update for a buggy card
03/26/15   hyo     Select MF after EF-LOCI update for a buggy card
07/03/14   sam     Explicit MF selection after location status envelop when 
                   NV 'UIMDRV_FEATURE_ENABLE_EXPLICIT_SELECTION_OF_MF_OR_ADF' is enabled
06/11/14   ll      Add interruptible time delay and return value for update params
06/04/14   na/js   Handling of case 4 commands for non telecom apps
03/21/14   ll      Remove return_ptr and remove/change uim_free to UIM_FREE
01/14/14   nmb     Move MANAGE CHANNEL command to generic cmd processing
01/14/14   nmb     Remove ACCESS_F and related unused functionality
09/16/13   yk      Removed the code for old targets and for old features
08/13/31   js      UIMDRV Memory De-Couple changes
07/12/13   nmb     Global Reorganization
12/05/12   js      UIM Parallel Processing changes
02/18/12   av      Cleaned up remaining ALWAYS ON features
09/09/11   ssr     Fix to allow the reset command when switch is un-pressed where
                   client want to disable the hotswap interrupt at runtime,
                   Added new API to return the real time card status,
                   Added new API to disable the hotswap interrupt
09/01/11   ak      GPIO hotswap support for 9x15
08/11/11   nmb     Expanded Logical Channels
04/25/11   yb      Adding support for hotswap
04/03/06   pv      Lint Fixes
02/10/03   jk      Created module.  This module contains the util state
                   machine definitions.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "target.h"
#include "comdef.h"
#include "uimi.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define UIM_FIRST_EXTENDED_CHANNEL               0x04
#define UIM_NO_LOGICAL_CHANNEL_SET               0xFF
/* Proactive command tag */
#define UIM_PROACTIVE_CMD_TAG                    0xD0
/* Command Tag for More Time */
#define PROA_MORE_TIME                           0x02

/* envelope command tags*/
#define UIM_EVT_DOWNLOAD_ENVELOPE                0xD6
#define UIM_ENVELOPE_EVENT_TAG                   0x19
#define UIM_LOCATION_STATUS_EVT                  0x03

/*===========================================================================

FUNCTION UIM_UTIL_GET_UIM_CLASS

DESCRIPTION
  Returns the appropriate standard or extended logical channel class byte.

DEPENDENCIES
  None

RETURN VALUE
  Class byte

SIDE EFFECTS
  None

===========================================================================*/
byte uim_util_get_uim_class
(
  byte             uim_ins_class,
  uim_channel_type channel
);


/*===========================================================================

FUNCTION UIM_MALLOC

DESCRIPTION
  The uim_malloc called TMC alloc with different heap_size based the size
  of the allocation required.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * uim_malloc(dword size);

/*===========================================================================

FUNCTION UIM_MEMSCPY

DESCRIPTION
  This Function provides the wrapper to memscpy, which is a secure version of
  memory copy

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_memscpy
(
  void        *dst_buff,
  size_t       dst_len,
  const void  *src_buff,
  size_t       src_len
);


/*===========================================================================

FUNCTION UIM_FORMAT_CHV

DESCRIPTION
  This procedure memsets the trailing characters in the CHV buffer with pad
  character (0xFF)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the value of the CHV passed in as a parameter.

===========================================================================*/
void uim_format_chv
(
  byte                     *chv,   /* This points to the CHV buffer that holds the CHV digits.
                                      The digits in this buffer are formatted  */
  uim_instance_global_type *uim_ptr
);


/*========================================================================

FUNCTION UIM_PROCESSING_STREAM_APDU

DESCRIPTION
  This function returns whether UIM is processing a stream apdu command.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : UIM processing STREAM APDU COMMAND
  FALSE: UIM not processing STREAM APDU COMMAND

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_processing_stream_apdu
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_IS_MORE_TIME_PCMD_FETCHED

DESCRIPTION
  This procedure tells whether the fetched command is MORE TIME or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if the data buffer contains MORE TIME proactive command
  FALSE - if the data buffer does not contain MORE TIME proactive cmd

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_more_time_pcmd_fetched
(
  const uint8    *data
);

/*===========================================================================

FUNCTION uim_is_location_status_envelope

DESCRIPTION
The function verifies if the current envelop is location status envwlop or not
DEPENDENCIES
  None

RETURN VALUE
  TRUE -- if location status envelop
  FALSE -- if not a location status envelop

SIDE EFFECTS
 NONE
===========================================================================*/
boolean uim_is_location_status_envelope
(
  const uint8    *data
);


/*===========================================================================

FUNCTION uim_is_call_connect_envelope

DESCRIPTION
The function verifies if the current envelop is call connect evt envwlop or not
DEPENDENCIES
  None

RETURN VALUE
  TRUE -- if location status envelop
  FALSE -- if not a location status envelop

SIDE EFFECTS
 NONE
===========================================================================*/
boolean uim_is_call_connect_envelope
(
  const uint8    *data
);


/*===========================================================================

FUNCTION uim_is_mf_selection_needed_after_update_bin_command

DESCRIPTION
The function verifies if the current update operation is on location status EF
DEPENDENCIES
  None

RETURN VALUE
  TRUE -- if location status EF or EF PSLOCI is the one to be updated
  FALSE --if location status EF or EF PSLOCI is not the one to be updated

SIDE EFFECTS
 NONE

===========================================================================*/


boolean uim_is_mf_selection_needed_after_update_bin_command
(
  uim_path_type *cmd_path_ptr
);


/*===========================================================================

FUNCTION uim_clk_busy_wait

DESCRIPTION
  This function is a wrapper around clk_busy_wait to avoid data conversion
  issues for different targets and to validate the supplied values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_clk_busy_wait(dword busy_wait_time);


/*===========================================================================

FUNCTION uim_timed_sleep_ext

DESCRIPTION
  This function is a wrapper around rex_timed_wait() to replace rex_sleep()
  with a non-deferrable timer with additional signals to wait for if needed.

DEPENDENCIES
  None

RETURN VALUE
  return the signals

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type uim_timed_sleep_ext
(
  unsigned long ms,
  uim_instance_global_type *uim_ptr,
  rex_sigs_type             additional_sigs_to_wait
);

/*===========================================================================

FUNCTION uim_timed_sleep

DESCRIPTION
  This function is a wrapper around rex_timed_wait() to replace rex_sleep()
  with a non-deferrable timer with additional signals to wait for if needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_timed_sleep
(
  unsigned long ms,
  uim_instance_global_type *uim_ptr,
  rex_sigs_type             additional_sigs_to_wait

);


/*===========================================================================
FUNCTION UIM_MAX_CH_NUM_OPENED

DESCRIPTION
  This function returns the value of the maximum channel number which was
  opened before recovery

PARAMETER
  None

DEPENDENCIES
  uim_is_ch_open

RETURN VALUE
  uim_channel_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uim_channel_type uim_max_ch_num_opened
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_START_POWER_SAVE

DESCRIPTION
  For USB cards this function informs the USB driver that the USB interface is
  not currently needed for any ICCD transaction and hence it can be suspended.
  For legacy ISO cards this function processes a request to turn the UIM clock
  off. It uses a global control variable to determine how to control the clock.

DEPENDENCIES
  For legacy ISO cards this function uses a global variable to determine
  how to control the UIM clock.

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  For USB cards if all functional interfaces over the USB interface have voted
  for a suspend, the USB interface to the card will be suspended.
  For legacy ISO cards the UIM clock will either be left high or low or will be
   left on, depending on the preferences of the UIM card.

===========================================================================*/
boolean uim_start_power_save
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================

FUNCTION UIM_STOP_POWER_SAVE

DESCRIPTION
  For USB cards this function sends a request to the USB driver to resume
  the USB interface as we need to communicate with the card over the ICCD
  functional interface.
  For legacy ISO cards this function processes a request to turn the UIM clock
  back on. It uses a global control variable to determine how to control the
  clock. It also uses a global variable to determine what frequency to set
  the clock.

DEPENDENCIES
  For legacy ISO cards this function uses a global variable to determine how
  to control the UIM clock and another global variable to determine the
  clock frequency.

RETURN VALUE
  None

SIDE EFFECTS
  For USB cards the USB driver will be requested to resume the interface.
  For ISO cards the UIM clock will be turned on and set to a frequency
  previously determined.

===========================================================================*/
void uim_stop_power_save
(
  uim_instance_global_type *uim_ptr
);

/*===========================================================================
FUNCTION UIM_IS_TELECOM_AID

DESCRIPTION
 This function is used to determine apps based on AID AID could belong to
 TELECOM as well as NON-TELECOM.For NON-TELECOM app,GET RESPONSE will not be
 issued upon receipt of sw1 as 0x62 and sw2 as 0x83

DEPENDENCIES
  None

RETURN VALUE
  TRUE   if AID is TELECOM or non-ADF type file is present, also default return value.
  FALSE  if AID is NON-TELECOM.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_telecom_aid
(
  uim_aid_type aid
);


/*===========================================================================
FUNCTION UIM_IS_ARAM_AID

DESCRIPTION
 This function is used to determine is the AID is an ARA-M app AID

DEPENDENCIES
  None

RETURN VALUE
  TRUE   if AID is ARA-M AID.
  FALSE  if AID is not ARA-M AID.

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_aram_aid
(
  uim_aid_type aid
);

/*===========================================================================
FUNCTION UIM_UTIL_GET_EF_FILE_SIZE

DESCRIPTION
 This function is used to retrieve the file size from FCP parameter

DEPENDENCIES
  None

RETURN VALUE
  File size of EF

SIDE EFFECTS
  None

===========================================================================*/
unsigned int uim_util_get_ef_file_size
(
  uim_instance_global_type *uim_ptr,
  uim_rsp_buf_type         *rsp_ptr
);
#endif /* UIMUTIL_H */
