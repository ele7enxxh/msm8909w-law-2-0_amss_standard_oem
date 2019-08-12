#ifndef GL1_HW_DEBUG_H
#define GL1_HW_DEBUG_H
/*===========================================================================
                      GSM GPRS NPL1 DEBUG HEADER FILE
DESCRIPTION
   This file defines the interfaces to the GSM/GPRS DEBUG.

   The implentation for the functions are distributed in several files.
   gl1_hw_debug.c     - File handling common functionality.

EXTERNALIZED FUNCTIONS
   gl1_hw_iq_samples_init
   gl1_hw_get_debug_data
   gl1_hw_iq_samples_enable
   gl1_hw_set_iq_state
   gl1_hw_get_iq_samples
   gl1_hw_save_iq_samples


INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gdrivers/inc/gl1_hw_debug.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
20/06/13   cs      Major Triton TSTS Syncup
10/04/13   ap      472264 DSDS: Fix the gas_id problems when the IQ logging is enabled
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
01/12/08    cs      New file based on code in original gl1_hw.h
===========================================================================*/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif



#include "geran_dual_sim.h"

/* -----------------------------------------------------------------------
**                             GSM Declarations
** ----------------------------------------------------------------------- */

/* Define this to enable saving/logging of IQ samples to EFS/QXDM. If this is enabled
   then appropriate calls to gl1_hw_iq_samples_enable must be added to
   gl1_hw_init. */
/* #define FEATURE_GSM_SAVE_IQ_SAMPLES */
/* #define FEATURE_GSM_LOG_IQ_SAMPLES  */

/*
 * Define this if you have esaic mDSP that doesn't have the total eSAIC metric buffer size
 * embedded correctly in the debug buffer
 * Needs to be on presently for main eSAIC enabled builds and off for others
 * so may need to featurised for those targets that don't have eSAIC support
 * in the F/W
 */
/* #define MDSP_IQ_ESAIC_SIZE_BUG */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                             GSM/GPRS Declarations
** ----------------------------------------------------------------------- */

/****************************************************************************
                            GSM/GPRS Common FUNCTION PROTOTYPES
 ***************************************************************************/

/*===========================================================================

FUNCTION gl1_hw_get_debug_data

DESCRIPTION
  This function retrieves a debug packet from the mDSP if available.

  data   - pointer to where to put decoded data results
  offset - offset (in words) into the mDSP debug buffer
  length - number of words to retrieve starting at offset

DEPENDENCIES
  The receive command must have been issued 2 frames earlier.

RETURN VALUE
  TRUE if packet available, FALSE otherwise

SIDE EFFECTS
  data will be modified even if results are FALSE.
===========================================================================*/
extern boolean gl1_hw_get_debug_data( uint16 *data,   uint16   offset,
                                      uint16  length, gas_id_t gas_id );

#if  defined ( FEATURE_GSM_LOG_IQ_SAMPLES )

/* Used to set the desired logging ts, can be modified globally */
extern uint16 gl1_hw_gprs_iq_slot[];

/* Which timeslot to save IQ samples for, default to first one */
#define IQ_SAMPLES_TN_INDEX 0

/*===========================================================================

FUNCTION gl1_hw_iq_samples_init

DESCRIPTION
 Init the IQ sample saving.

===========================================================================*/
extern void gl1_hw_iq_samples_init( gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_hw_iq_samples_enable

DESCRIPTION
 Enables the IQ sample saving.

===========================================================================*/
extern void gl1_hw_iq_samples_enable( gl1_hw_channel_type channel, boolean enable,
                                      gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_hw_is_iq_channel_enabled

DESCRIPTION
  Returns the status if IQ logging for the given channel type.

===========================================================================*/
extern boolean gl1_hw_is_iq_channel_enabled( gl1_hw_channel_type channel,
                                             gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_hw_set_iq_samples_tn

DESCRIPTION
  Sets which timeslot to save IQ samples for.

===========================================================================*/
extern void gl1_hw_set_iq_samples_tn( gl1_hw_channel_type channel,
                                      gl1_defs_tn_type tn, gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_hw_iq_samples_force_enable

DESCRIPTION
  Force the enable the IQ sample saving for the given channel type.

===========================================================================*/
extern void gl1_hw_iq_samples_force_enable( gl1_hw_channel_type channel,
                                            boolean enable, gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_hw_get_iq_samples

DESCRIPTION
  This function will get IQ samples from the mdsp and store them according
  to the message index (0-2) and frame index (0-3) given.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void gl1_hw_get_iq_samples( gl1_hw_channel_type channel, uint32 msg_index,
                                   uint32 frm_index, uint8 TSC, gas_id_t gas_id );

/*===========================================================================

FUNCTION gl1_hw_set_iq_state

DESCRIPTION
   This sets/resets gl1_trigger_iq_stateto trigger IQ Samples or not.

===========================================================================*/
extern void gl1_hw_set_iq_state( boolean state, gas_id_t gas_id );
#endif /* FEATURE_GSM_SAVE_IQ_SAMPLES | FEATURE_GSM_LOG_IQ_SAMPLES */



#endif /* GL1_HW_DEBUG_H */

/* EOF */
