#ifndef FTM_V_H
#define FTM_V_H
/*===========================================================================

           F T M    INTERNAL H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the common FTM module

REFERENCES
  None

Copyright (c) 2006-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_v.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/11   sar     Relocate non-exported declarations from ftm.h for internal use.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ftm.h"
#include "ftm_cdma_ns_types.h"
#include "ftm_cmdi.h"
#include "rfcom.h"

extern ftm_acq_mode_type ftm_acq_mode;
extern boolean ftm_wait_for_pc_msg;
extern apt_tx_lin_type ftm_apt_tx_lin[];
extern apt_freq_comp_type ftm_apt_fcomp[]   ;
extern hdr_ftm_state_enum_type hdr_ftm_rev_curr_state;
/* To track the current RF mode */
extern ftm_rf_mode_type ftm_current_rf_mode;
#endif /* FTM_V_H */
