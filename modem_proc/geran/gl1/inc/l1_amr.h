#ifndef L1_AMR__H
#define L1_AMR__H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 1   A M R   T Y P E S   H E A D E R   F I L E

DESCRIPTION
   Layer 1 AMR specific type declarations.


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_amr.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- --------------------------------------------------------
01/04/11   pg       CR280752 - Only transition to new ACS conveyed in RATSCCH after 
                    sending previous audio packet to MVS  
10-04-01   tjw      Add customer.h and comdef.h to all SU API files
06/04/10   dv       CR228666 AHS codec rate fluctuations in link adaptation TCs@Nortel IOT
12/05/08   dv       Added FEATURE_GSM_AMR_WB - L1 component
CR 102919  ws       only use AMR C/I optimisation if we have less than 8
                    hopping frequencies in MA list
07/08/06   ws       CR 95398 - Optimization of C/I calculation for frequency hopping
01/05/05   dp       Merged AMR GCF fixes to mainline
08/11/03   gsc      Some new function prototypes to incorporate C/I
                    filter changes to use the new C/I field in the burst
                    metrics.
05/07/03   gsc      First revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#ifdef FEATURE_GSM_AMR

#include "gl1_msg.h"
#include "gl1_msg_g.h"
#include "sys_cnst.h"

/*
 * Max codec modes in the Active Codec Set
 */
#define MAX_CODEC_MODES                      4

/*
 * Max codec modes supported in NB AMR Full Rate
 */
#define MAX_FULL_RATE_MODES                  8

#ifdef FEATURE_GSM_AMR_WB
/*
 * Max codec modes supported in  WB AMR Full Rate
 */
#define MAX_WB_FULL_RATE_MODES               3

#endif

/*
 * Max codec modes supported in AMR Half Rate
 */
#define MAX_HALF_RATE_MODES                  6

/*
 * Pending RATSCCH request is activating on N+12th speech frame 
 * (counter is incremented on frame N, so count val = 12+1) 
 */
#define MAX_RATSCCH_COUNTER_VAL              13

/*---------------------------------------------------------------------------
 * Codec Mode Indication Phase type
 *-------------------------------------------------------------------------*/
typedef enum
  {
    CMI_PHASE_EVEN,
    CMI_PHASE_ODD,
    CMI_PHASE_INVALID
  }cmi_phase_type;

/*---------------------------------------------------------------------------
 * RATSCCH Request Types
 *-------------------------------------------------------------------------*/
typedef enum
  {
    RATSCCH_CMI_PHASE_REQ,
    RATSCCH_AMR_CONFIG_REQ,
    RATSCCH_THRESH_REQ,
    RATSCCH_UNDEF_REQ
  } ratscch_req_type;

/*---------------------------------------------------------------------------
 * RATSCCH Response Types
 *-------------------------------------------------------------------------*/
typedef enum
  {
    ACK_OK  = 0x1,
    ACK_ERR = 0x2,
    ACK_UNKNOWN = 0x3,
    ACK_UNDEF = 0x4
  }ratscch_rsp_type;

/*======================================================================
 *                       GLOBAL FUNCTIONS
 *
 *=====================================================================*/

/*===========================================================================
FUNCTION: ratscch_process_req

DESCRIPTION:
  This is an interface function to the RATSCCH handler which the PL1
  layer uses when a RATSCCH message is received on the D/L. This function
  determines the type of the request received and then acts upon it by
  calling the appropriate request handlers.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void ratscch_process_req( uint8 *data, boolean crc_status,gas_id_t gas_id );

/*===========================================================================
FUNCTION: ratscch_update_req_activation_counter

DESCRIPTION:
  This is an interface function to the PL1 layer and provides a mechanism
  for updating the RATSCCH Req_Activation_Counter. If the req_act_counter
  is 0 (which means there is no currently pending request) this function
  has no effect.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  If the req_act_counter reaches a maximum value of
  MAX_RATSCCH_COUNTER_VAL (12), the pending request in the D/L is activated
  and the req_act_counter is reset to 0.
===========================================================================*/
extern void ratscch_update_req_activation_counter( gas_id_t gas_id );

/*===========================================================================
FUNCTION: ratscch_update_ack_activation_counter

DESCRIPTION:
  This is an interface function to the PL1 layer and provides a mechanism
  for updating the RATSCCH Ack_Activation_Counter. If the ack_act_counter
  is 0 (which means there is no currently pending request) this function
  has no effect.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  If the ack_act_counter reaches a maximum value of
  MAX_RATSCCH_COUNTER_VAL (12), the pending request in the U/L is activated
  and the ack_act_counter is reset to 0.
===========================================================================*/
extern uint8 ratscch_update_ack_activation_counter( gas_id_t gas_id );


/*===========================================================================
FUNCTION: ratscch_start_ack_activation_counter

DESCRIPTION:
  This is an interface function to PL1 which is used after a RATSCCH message
  is sent on the U/L.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void ratscch_start_ack_activation_counter( gas_id_t gas_id );


/*===========================================================================
FUNCTION: ratscch_reset_pending_proc

DESCRIPTION:
  This is an interface function for the PL1 layer to reset all the pending
  RATSCCH messages and counters.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void ratscch_reset_pending_proc( gas_id_t gas_id );


/*===========================================================================
FUNCTION: ratscch_activate_req_in_dl

DESCRIPTION:
  This function activates the currently pending RATSCCH request to take effect
  in the D/L. All three RATSCCH messages are applicable on the D/L and
  any one of these three message can be pending at a give time.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  The new RATSCCH parameters takes effect from the next speech frame. The
  RATSCCH parameter can be ACS, ICM, PHASE, THRESHOLD/HYSTERESIS pairs
  etc.
===========================================================================*/
extern void ratscch_activate_req_in_dl(gas_id_t gas_id);

/*===========================================================================
FUNCTION: ratscch_activate_req_in_ul

DESCRIPTION:
  This function activates the currently pending RATSCCH request to take effect
  in the U/L. Only AMR_CONFIG_REQ is applicable on the U/L.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  The new RATSCCH parameters takes effect from the next speech frame. The
  RATSCCH parameter can be ACS, ICM, THRESHOLD/HYSTERESIS pairs
  etc.
===========================================================================*/
extern void ratscch_activate_req_in_ul(gas_id_t gas_id);

/*===========================================================================
FUNCTION: gen_codec_mode_request

DESCRIPTION:
  This function uses the current set of thresholds and hysteresis values
  and the normalized C/I value to adjust the current Codec Mode Request.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void gen_codec_mode_request( gas_id_t gas_id );

/*===========================================================================
FUNCTION: filter_c_over_i_fr

DESCRIPTION:
  This function implements the SNR filter needed for AMR full rate and
  returns a normalized C/I value that is used to generate the Codec Mode
  Request (CMR) to be transmitted on the U/L.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void filter_c_over_i_fr (uint16 c_over_i,gas_id_t gas_id);

/*===========================================================================
FUNCTION: set_ul_default_icm

DESCRIPTION:
  This function implements the set of rules outlined in GSM 05.09 standard
  to determine the U/L Initial Codec Mode in case it is not specified in
  the multi-rate IE.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void set_ul_default_icm(gas_id_t gas_id);

/*===========================================================================
FUNCTION: set_dl_default_icm

DESCRIPTION:
  This function implements the set of rules outlined in GSM 05.09 standard
  to determine the D/L Initial Codec Mode in case it is not specified in
  the multi-rate IE.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void set_dl_default_icm(gas_id_t gas_id);

/*===========================================================================
FUNCTION: set_ul_acs

DESCRIPTION:
  This function sets the U/L ACS.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if successful, otherwise FALSE

SIDE EFFECTS:
  None
===========================================================================*/
boolean set_ul_acs (gl1_msg_amr_mode_type *acs, uint8 num_modes, gas_id_t gas_id);

/*===========================================================================
FUNCTION: set_dl_acs

DESCRIPTION:
  This function sets the D/L ACS.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if successful, otherwise FALSE

SIDE EFFECTS:
  None
===========================================================================*/
boolean set_dl_acs (gl1_msg_amr_mode_type *acs, uint8 num_modes, gas_id_t gas_id);
#ifdef FEATURE_GSM_AMR_WB
/*===========================================================================
FUNCTION: gl1_msg_ul_set_wb_amr_mode

DESCRIPTION:
  This function passes the WB AMR state down to the drivers.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void gl1_msg_ul_set_wb_amr_mode(boolean wb_amr_flag,gas_id_t gas_id);

/*===========================================================================
FUNCTION: gl1_msg_dl_set_wb_amr_mode

DESCRIPTION:
  This function passes the WB AMR state down to the drivers.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void gl1_msg_dl_set_wb_amr_mode(boolean wb_amr_flag,gas_id_t gas_id);
#endif /* FEATURE_GSM_AMR_WB */

/*===========================================================================
FUNCTION l1_amr_get_hr_subchannel

DESCRIPTION
   This function returns the value of amr_hr_variable

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
uint8 l1_amr_get_hr_subchannel(gas_id_t gas_id);

/*===========================================================================
FUNCTION l1_amr_get_channel_type

DESCRIPTION
   This function returns the value of amr_hr_variable

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
gl1_msg_chan_type l1_amr_get_channel_type(gas_id_t gas_id);

#endif /* FEATURE_GSM_AMR */
#endif /* L1_AMR__H */
