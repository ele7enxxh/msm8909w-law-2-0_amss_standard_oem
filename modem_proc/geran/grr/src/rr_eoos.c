/*==============================================================================

                             R R   EOOS

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010-2013 Qualcomm Technologies, Inc.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_eoos.c#1 $

==============================================================================*/

/* EOOS - Enhanced Out Of Service
 *
 * EOOS is a multirat out of service search facility.
 * An EOOS search is triggered in a RR_PLMN_SELECT_REQ message in the rat_pri_list
 * scan_type field.
 * If use_timer is TRUE then an EOOS search is active and EOOS timer is started.
 *
 * An EOOS search is time bounded and can occur over many awake periods.  The
 * search is continued in each awake period unless the RR_PLMN_SELECT_REQ indicates
 * a new search.
 * The search order of an EOOS search is:
 *  1) Acq DB power scan
 *  2) Acq DB BCCH Decode
 *  3) Full power scan
 *  4) Full BCCH Decode
 *
 *  After step 4) if no service is found the EOOS search is determined to be complete.
 *  If the EOOS timer expires at any point during steps 1-4 the search is abandoned
 *  and RR goes back to an INACTIVE state. RR resumes the EOOS from where it
 *  left off when it receives another RR_PLMN_SELECT_REQ new_search=F.
 *
 *  The BCCH decode in step 4 is interspersed with acq DB channels.  The Acq DB
 *  Channels in the Acq DB power scan req are saved in step 1.  These channels
 *  are interspersed every N channels into the full BCCH Decode. The last
 *  camped cell is also inserted at the start of the BCCH List for every awake
 *  period of step 4.
 *
 *  A power scan timer restricts full power scans in EOOS to 1 in every 15 seconds,
 *  note this timer is linked to bands.  So if a different band to the timer linked
 *  band is requested then the timer does not restrict the power scan.  RR currently
 *  maintains 2 power scan timers to handle the possibility of two band groups being
 *  used.
 *
 *  RR reverts to COOS search by cancelling the EOOS timer and resetting the EOOS
 *  active flag.  Reversion to COOS occurs when a suitable cell has been found
 *  and MPH_SELECT_SPECIFIC_BCCH has been sent.  It also occurs when a suitable
 *  cell has been found that matches all of the search criteria except the
 *  requested PLMN is different.
*/

/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

