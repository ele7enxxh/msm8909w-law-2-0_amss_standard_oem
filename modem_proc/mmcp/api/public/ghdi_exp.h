#ifndef GHDI_EXPORT_H
#define GHDI_EXPORT_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/ghdi_exp.h_v   1.9   17 Jul 2002 11:52:16   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/ghdi_exp.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/01/10   kj      Added ghdi_ds_read_imei 
02/26/04   ab      Added GSM GHDI_TURN_SPEECH_TCH_AMR_ON and
                     GHDI_TURN_SPEECH_AMR_OFF.
05/11/01   jca     Fixed Lint errors.
06/11/01   TS      Added GSM build support.
08/15/01   ab      Renamed all MMI references to CM
09/18/01   sbk     NV related functionality added and prototypes declared
01-Apr-2002  VT    GSM EFR: Added GHDI_TURN_SPEECH_TCH_EF_ON and 
                     GHDI_TURN_SPEECH_TCH_EF_OFF to the enum related to 
                     ghdi_tch_control_T.
                   Actually merged the above changes from rev 1.7.1.0 (branch) 
                     of this file.
05/01/02   rmc     Added ghdi_read_ue_revision_level function
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "nv_items.h" /* NV item definitions */
#include "sys.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define GHDI_SUCCESS                   0
#define GHDI_FAILURE                   1

typedef dword                          ghdi_status_T;

extern ghdi_status_T ghdi_read_imei(byte *imei);

extern ghdi_status_T ghdi_ds_read_imei(byte *imei,sys_modem_as_id_e_type as_id);

#endif /* GHDI_EXPORT_H */
