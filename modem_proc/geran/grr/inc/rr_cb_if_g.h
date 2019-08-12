/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              rr_cb_if.h

GENERAL DESCRIPTION
  RR callback functionality.  Provides callback for multiple regstrations
  (set by RR_MAX_NUM_OF_REG_CB).  

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002-2013 Qualcomm Technologies, Inc.

========================================================================== */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/rr_cb_if_g.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
11-02-16    tjw     Internal content split out from the original GERAN file
                    previously used by GERAN clients.


===========================================================================*/

#ifndef RR_CB_IF_G_H
#define RR_CB_IF_G_H


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
#include "geran_dual_sim.h"

/*===========================================================================

                         DEFINITIONS

===========================================================================*/
#define RR_MAX_NUM_OF_REG_CB  (NUM_GERAN_DATA_SPACES)

/*========================================================================== */
/* Check rr_cb_if.h FEATURE assumptions */
/*========================================================================== */

#ifndef FEATURE_GSM_RR_CELL_CALLBACK
  #pragma warning ("FEATURE_GSM_RR_CELL_CALLBACK assumed defined but isn't")
#endif

#endif /* RR_CB_IF_H */

