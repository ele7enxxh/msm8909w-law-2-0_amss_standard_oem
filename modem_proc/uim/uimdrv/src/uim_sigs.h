#ifndef UIMSIGS_H
#define UIMSIGS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  S I G N A L   H A N D L E R  H E A D E R

                  U S E R   I D E N T I T Y   M O D U L E

GENERAL DESCRIPTION
  This header defines the external interface for the uim_sigs module.

EXTERNALIZED FUNCTIONS

  uim_signal_handler
    Calls the signal handling functions for each UIM task signal that needs to be
    handled.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  uim_signal_handler shall be called after rex_wait returns a mask of signals that
  are set.

Copyright (c) 2001-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_sigs.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/14   lxu     USB UICC modem implement
01/14/14   ak      Created for uim task re-factoring/cleanup

===========================================================================*/

/**
*  This function handles all the signals that are set for the UIM task by
*  calling the associated handler function for each signal that
*  is set.
*
*  SIDE EFFECTS
*  Can potentially change the value of the imask_ptr(set of
*  signals to that UIM task is interested in) depending on logic
*  in the handlers.
*
*  @param sig_mask_ptr Pointer to the OR'd set of signals to
*                      handle.
*  @param imask_ptr Pointer to the mask of signals that the uim task is interested
*                   in handling.   The mask may be changed by this function.
*  @param uim_ptr Pointer to the current instance of the UIM
*                 global structure
*  @return boolean Return TRUE to indicate that immediately
*          going back to the top of the main loop is desired.
*/
boolean uim_signal_handler(rex_sigs_type *sigMask_ptr, rex_sigs_type *iMask_ptr, uim_instance_global_type *uim_ptr);

#endif /* UIMSIGS_H */
