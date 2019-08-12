#ifndef RFC_COMMON_TDD_H
#define RFC_COMMON_TDD_H

/*===========================================================================


      R F C  C o m m o n   GSM/TDSCDMA  c o n t r o l   A P I

                            H e a d e r  F i l e

DESCRIPTION
  This file contains common functions for the RFC GSM/TDSCDMA control code.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/inc/rfc_common_tdd.h#1 $
  $DateTime: 2016/12/13 07:59:52 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/13/13   cc      Unify gpio type to rfc_sig_cfg_type for FDD and TDD tech 
08/01/12   jyu     Added support to allow override logic and time from AG 
07/27/12   sr      Added API to append the grfc info for the specified signal type
07/20/12   sr      Added API to update the rf_buffer with start timing and logic
07/09/12   sr      changes to use the tdd signal type instead of gsm signal type.
07/02/12   sr      changed the #ifdef include macro according to the file name
06/29/12   sr      rfc api name change.
06/28/12   sr      made changes to use the rf_hal_buffer.
06/28/12   sr      dime rfc changes.
05/18/12   sb      made APIs to get base address of fw grfc engine from msm signal and signal type 
11/04/11    sr     added copy signal list API.
10/17/11    sr     changed the rfc api to get the timing info. for the specified signal.
08/26/11   sr      RFC V1.3.35 SS release changes. SS format has changed.
08/20/09   sr      added function to update the GRFC timing.
08/17/11   av      Set GSM GRFCs to default/low state when mode transition to GSM. 
                   Temp change till we change the RF/FW interface to handle grfc logic
                   in burst scripts 
08/11/11   av      Added new API for constructing XtoG, GtoX grfc scripts. 
08/06/11   sr      RFC Cleanup: removed gsm specific code.
07/08/11   sr     Added new API to get the GRFC number from list of signals
03/10/11    sr     warnings fix
03/03/11    sr     merge from mdm9k branch
02/10/11   tnt     Modify for nikeL  modem
01/31/11   sr      removed un-used code.
01/25/11   sr      replaced the tlmm.h file with DDITlmm.h
10/18/10  av/lcl   Clean up common RFC support
09/18/10   av      Fix compiler warnings
09/14/10   av      LtoG support and compiler warning fixes
09/01/10   av      GSM RFC Rearch
05/20/10   tws     Initial version
============================================================================*/



/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "rf_hal_grfc.h"
#include "rfcom.h"
#include "rfc_card.h"
#include "DDITlmm.h"
#include "DALSys.h"
#include "rfc_common.h"




boolean rfc_common_tdd_config_signal(rfc_sig_cfg_type *rf_signals);

uint32 rfc_common_tdd_update_grfc_script(rfc_sig_cfg_type *rf_signals, void *start_script_ptr, void *stop_script_ptr);

uint32 rfc_common_tdd_update_start_grfc_script(rfc_sig_cfg_type *rf_signals, void *start_script_ptr);

boolean rfc_common_tdd_get_grfc_buffer(rfc_sig_cfg_type *rf_signals, rfc_signal_type sig_type,
                                       rfc_script_enum_type script_type, rf_buffer_intf *rf_buffer,
                                       uint8 num_override_data_entry,                                       
                                       rfc_sig_timing_info_type *override_data,
                                       boolean override_logic, boolean override_time);

boolean rfc_common_tdd_get_grfc_info(rfc_sig_cfg_type *rf_signals, rfc_signal_type sig_type,
                                     rfc_grfc_script_entry_type *grfc_info);

void rfc_common_tdd_update_timing(rfc_sig_cfg_type *rf_signals, 
                                  rfc_signal_type sig_type, 
                                  int32 start_time_offset, int32 stop_time_offset);

boolean rfc_common_tdd_init_default(rfc_sig_cfg_type *rfc_signals);

int32 rfc_common_tdd_copy_sig_list(rfc_sig_cfg_type *src_signals, rfc_sig_cfg_type *dest_signals);


#ifdef __cplusplus
}
#endif


#endif /* RFC_COMMON_TDD_H */
