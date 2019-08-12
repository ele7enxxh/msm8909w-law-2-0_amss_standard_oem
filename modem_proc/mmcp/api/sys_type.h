#ifndef sys_type_h
#define sys_type_h

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_type.h_v   1.10   05 Feb 2002 09:42:20   chatlani  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sys_type.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/08   rm      CMI defeaturisation
01/15/01   JC      Added dBx16_T.
04/04/01   jca     Fixed some Lint errors.
08/01/01   cd      Removed definition of connection_id_T and included nas_exp.h
08/14/01   AB      Renamed connection manager reference to CNM from CM.
08/21/01   CD      Moved the following to nas_exp.h
                      cc_state_T
                      CNMServiceType_T
                      hold_auxiliary_state_T
                      mpty_auxiliary_state_T
                      channel_mode_T
                      lower_layer_cause_T
                      ss_state_T
08/23/01   TS      Moved several types to nas_exp.h
11/21/01   TS      Added sword for GSM lower layer usage.                 
10/09/03   cd      Merged in GSM pref band support
09/20/04   ks      Added value INVALID_BAND in sys_band_T
09/22/04   ks      Fixed the missing ',' in sys_band_T enum.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "nas_exp.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
 * general definitions
 */

typedef signed long  sdword;         /* 4 byte variable */
typedef signed short sword;          /* 2 byte variable */


/*typedef byte                      connection_id_T;*/
typedef byte                      message_type_T;
typedef dword                     milli_secs_T;
typedef byte                      cnm_service_state_T;
typedef byte                      progress_indicator_T;
typedef byte                      cc_cause_T; /* cause definitions in cause.h */
typedef byte                      invoke_id_T;
typedef byte                      call_ss_operation_T;
typedef byte                      transaction_id_T;
typedef byte                      channel_type_T;

typedef sint15                    dBx16_T;


/* definition of enum rr_internal_band_T is based on definition of enum
   sys_band_T. */
typedef enum
{
  SYS_BAND_EGSM_900,
  SYS_BAND_PGSM_900,
  SYS_BAND_PCS_1900,
  SYS_BAND_DCS_1800,
  SYS_BAND_CELL_850,
  INVALID_BAND,
  SYS_BAND_NONE = 0xFF  
} sys_band_T;

/* ARFCN type, includes band if quadband is supported */
typedef struct
{
  uint16     num;
  sys_band_T band;
} ARFCN_T;

#endif

