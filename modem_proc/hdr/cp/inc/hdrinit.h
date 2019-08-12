#ifndef HDRINIT_H
#define HDRINIT_H

/*===========================================================================

          I N I T I A L I Z A T I O N  S T A T E  P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the initialization state protocol (INIT).

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrinit.h_v   1.11   09 Jan 2003 11:02:06   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrinit.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/12   cnx     Redir abort to SD when hystr timer expires.
07/27/11   cnx     Merged Optimized Handoff changes.
05/19/10   pba     Added support for Reverse Time Transfer.
01/08/03   mpa     Removed unnecessary dependency on hdrsrch.h
01/18/02   mpa     Added hdrinit_get_sync_time()
01/04/02   SH      Divided the init to powerup init and regular init
05/29/00   SH      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "rex.h"
#include "dsm.h"
#include "hdrind.h"
#include "hdrhai.h"
#include "hdrerrno.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef struct
{
  boolean           is_valid;
  /* Whether the timing data is valid ot not */

  sys_channel_type  sys_info;
  /* Band and Frequency  */

  uint32            lte_ostmr;
  /* LTE RTC counter */

  uint64            cdma_sys_time;
  /* CDMA System time */

} hdrinit_timing_info_type;

/*===========================================================================

FUNCTION HDRINIT_ACTIVATE

DESCRIPTION
  This function commands the initialization state protocol to activate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_activate ( hdrhai_protocol_name_enum_type sender );




/*===========================================================================

FUNCTION HDRINIT_DEACTIVATE

DESCRIPTION
  This function commands the initialization state protocol to deactivate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_deactivate ( hdrhai_protocol_name_enum_type sender );


/*===========================================================================

FUNCTION HDRINIT_REACQUIRE

DESCRIPTION
  This function commands the initialization state protocol to re-acquire,
  as if it were deactivated then re-activated

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_reacquire ( hdrhai_protocol_name_enum_type sender );


/*===========================================================================

FUNCTION HDRINIT_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRINIT.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrinit_ind_cb
(
  hdrind_ind_name_enum_type  ind_name,
  void                    *ind_data_ptr
);


/*===========================================================================

FUNCTION HDRINIT_INIT

DESCRIPTION
  This function initialize the initialzation state protocol.

DEPENDENCIES
  This function must called before the initialization state protocol can be
  activated

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRINIT_POWERUP_INIT

DESCRIPTION
  This function performs the powerup initialization.
  
DEPENDENCIES
  This function must called before the initialization state protocol can be
  activated

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_powerup_init ( void );
 
/*===========================================================================

FUNCTION HDRINIT_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRINIT.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_msg_cb
( 
  dsm_item_type *item_ptr 
);

/*===========================================================================

FUNCTION HDRINIT_GET_SYNC_TIME

DESCRIPTION
  This function returns the 64-bit system time in units of 26.666msec time.

DEPENDENCIES
  This function must be called shortly after a network acquired indication is
  received (valid only when a sync msg is received)

PARAMETERS
  None

RETURN VALUE
  The current system time right after acquisition

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_get_sync_time
(
  qword sys_time
);

#ifdef FEATURE_LTE_TO_HDR
/*===========================================================================

FUNCTION HDRINIT_GET_TIMING_INFO

DESCRIPTION
  This function returns the stored timing information.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrinit_get_timing_info 
( 
  hdrinit_timing_info_type *timing_info_ptr 
);

/*===========================================================================
FUNCTION HDRINIT_SET_HYSTR_TIMER_IS_EXPIRED

DESCRIPTION
  This function sets the flag that indicating whether LTE to HDR hystersis 
  timer is expired.

DEPENDENCIES
  None

PARAMETERS
  hystr_timer_is_expired - TRUE if hystersis timer is expired.
                           FALSE otherwise.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrinit_set_hystr_timer_is_expired
( 
  boolean hystr_timer_is_expired
);


/*===========================================================================
FUNCTION HDRINIT_HYSTR_TIMER_IS_EXPIRED

DESCRIPTION
  This function sets the flag that indicating whether LTE to HDR hystersis 
  timer is expired.

DEPENDENCIES
  None

PARAMETERS
  hystr_timer_is_expired - TRUE if hystersis timer is expired.
                           FALSE otherwise.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrinit_hystr_timer_is_expired( void );

#endif /* FEATURE_LTE_TO_HDR */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRINIT_CONTINUE_ACQUISITION

DESCRIPTION
  This function commands the initialization state protocol to continue with
  acquisition.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrinit_continue_acquisition ( hdrhai_protocol_name_enum_type sender );
#endif /* FEATURE_LTE_TO_HDR_OH */

#endif /* HDRINIT_H */

