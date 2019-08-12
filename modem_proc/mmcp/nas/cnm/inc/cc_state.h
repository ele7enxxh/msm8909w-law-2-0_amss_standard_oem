#ifndef cc_state_h
#define cc_state_h

/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cc_state.h_v   1.6   01 Nov 2001 12:30:26   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/inc/cc_state.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
24-Oct-2001  VT    DTMF: Added enum cc_dtmf_state_e_type
26-Oct-2001  VT    DTMF: Removed #define U10_DTMF ( 10+ 0x80 )
29-Oct-2001  AB    SS:  Moved SS state defiens to cc_state_T location
                        nas_exp.h.  The cc_state_T was from byte to enum
                        type for code security.
01-Nov-2001  AB    SS: Moved ss_state_T and cc_state_T here from nas_exp.h
                       for coherent purpose.
08/01/02     CD    Removed SMS_CONN_REESTABLISH definition since it is not used

05/10/05     HS    Added support for FEATURE_CCBS
===========================================================================*/
#include "mmcp_variation.h"


/*
 * definitions of SS states
 */
typedef enum ss_state_e {
    SS_IDLE = 0x00, 
    SS_MM_CONN_PEND,  
    SS_CONN_EST,  
    SS_CONN_REESTABLISH, 
    SS_UNKNOWN = 0xFF 
} ss_state_T;

/*
 * definitions of DTMF states
 */
typedef enum cc_dtmf_state_e {
    DTMF_IDLE = 0x00, 
    WAIT_FOR_START_DTMF_ACK,  
    WAIT_FOR_STOP_DTMF_ACK  
} cc_dtmf_state_e_type;

/*
 * definitions of SMS states
 */

#define SMS_IDLE               0
#define SMS_MM_CONN_PEND       1
#define SMS_WAIT_CP_ACK        2
#define SMS_MM_CONN_EST        4

#endif
