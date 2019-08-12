#ifndef MAC_UL_HS_SUBTASK_H
#define MAC_UL_HS_SUBTASK_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              U P L I N K   M A C   H S U P A   H E A D E R    F I L E

DESCRIPTION
   MAC HSUPA specific type declarations.


Copyright (c) 2001, 2002, 2003 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2006-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/maculhssubtask.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- --------------------------------------------------------
11/07/13   ts       DC-HSUPA: Changes for SI timer redesign by keeping a single base timer and reducing signals
05/10/13   kc       Added DC-HSUPA Changes
08/22/12   kc       New log packet 0x4314 for HS RACH MAC timer info
05/07/09   ssg      Updated Copyright Information
07/29/08   mrg      Prototype changed for mac_hs_ul_process_header_log_packet()
                    and mac_hs_ul_process_etfci_status_log_packet() functions as
                    part of the l2_ul_cmd_data_type changes.
05/03/07   sk       Moved the definition of the MAC UL HS
                    timers from the maculhssubtask.h to this file.
02/10/07   sk       Logging support for the internal log packet - 0x4323
01/31/07   sk       Added functions mac_hs_ul_handle_log_tmr_expiry and mac_hs_ul_process_header_log_packet
01/02/06   sk       Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#ifdef FEATURE_WCDMA_HSUPA

#include "macrrcif_v.h"
#include "macinternal.h"
#include "maculhs.h"

/*===========================================================================

                     DEFINES AND MACROS FOR MODULE

===========================================================================*/
/*===========================================================================

                     KEY MACROS

===========================================================================*/

/*===========================================================================

                     GENERAL PURPOSE MACROS

===========================================================================*/


/*===========================================================================

                     TYPE DEFINITIONS FOR MODULE

===========================================================================*/
/*--------------------------------------------------------------------------
ENUM Indicates which timer expired
--------------------------------------------------------------------------*/
typedef enum
{
  /* Initialization purposes*/
  MAC_HS_UL_TIMER_INVALID=0,

  MAC_HS_UL_SI_TMR_EXPIRED,

  MAC_HS_UL_SECONDARY_SI_TMR_EXPIRED

}mac_hs_ul_si_tmr_expired_enum_type;


/*===========================================================================

                         GLOBAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION:   mac_hs_ul_handle_si_tmr_expiry

DESCRIPTION:
This fucntion is called from l2ultask.c when one of the SI timers
expires. Note that at any time only one SI timer can be active
either the timer related to no grant periodicity or the timer related
to grant periodicity

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_hs_ul_handle_si_tmr_expiry
(
  mac_hs_ul_si_tmr_expired_enum_type    si_tmr_exp_type
);
#ifdef FEATURE_MAC_HS_UL_QXDM_LOGGING
/*===========================================================================
FUNCTION:   mac_hs_ul_handle_status_tmr_expiry

DESCRIPTION:
This fucntion is called from l2ultask.c when the status log timer expires

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_hs_ul_handle_log_tmr_expiry
(
  void
);

/*===========================================================================
FUNCTION:   mac_hs_ul_process_header_log_packet

DESCRIPTION:
This fucntion is called from l2ultask.c when the status log timer expires

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_hs_ul_process_header_log_packet(void);

/*===========================================================================
FUNCTION:   mac_hs_ul_process_etfci_status_log_packet

DESCRIPTION:
This fucntion is called from l2ultask.c when the status log timer expires

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void mac_hs_ul_process_etfci_status_log_packet(void);

#ifdef FEATURE_MAC_I
void mac_hs_ul_process_maci_is_log_packet(void);
#endif

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================
FUNCTION mac_hs_ul_process_hsrach_timers_log_pkt

DESCRIPTION
  This function would be called from l2ultask.c when corresponding command is processed by MAC HS task. 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resest the global buffer from which info is logged - memset it to 0x0
===========================================================================*/
void mac_hs_ul_process_hsrach_timers_log_pkt(void);

#endif //FEATURE_WCDMA_HS_RACH
#endif //FEATURE_MAC_HS_UL_QXDM_LOGGING

#endif /* FEATURE_WCDMA_HSUPA */
#endif /* MAC_HS_UL_H */
