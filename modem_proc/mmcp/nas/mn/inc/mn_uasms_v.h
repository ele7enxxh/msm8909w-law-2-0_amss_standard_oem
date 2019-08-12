#ifndef mn_uasms_v_h
#define mn_uasms_v_h

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

  EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_uasms.h_v   1.5   17 Jun 2002 17:43:36   rgurumoo  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/inc/mn_uasms_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
7/12/01     CD     Initial version.  Transferred mn_uasms data from mn.h
08/16/01    AB     Renamed MMI references to CM.

9/27/01     CD     Deleted tp_message_reference from SM_RL_DATA_REQ_T
                   Deleted tp_smi, and sm_rl_success from SM_RL_REPORT_IND_T
                   Changed error_type to error_status in SM_RL_REPORT_IND_T
                   Changed type of RP_OA in SM_RL_DATA_IND_T to cm_called_party_bcd_no_T

                   Added definitions for error_status in SM_RL_REPORT_IND_T

9/28/01     CD     Added definition for RP_USER_DATA_LENGTH, changed TPDU dimension
                   in SM_RL_REPORT_IND_T from TPDU_SIZE to RP_USER_DATA_LENGTH

                   Changed TPDU_SIZE to 164

1/29/02     RG     Changed the short_message_identifier in the SM_RL_DATA_REQ message 
                   to be a 'word' instead of a 'byte'.  Added "word short_message_identifier" 
                   to the SM_RL_MEMORY_AVAILABILITY_REQ_T structure.  Changed the 
                   short_message_identifier in the SM_RL_REPORT_IND message to be a 
                   'word' instead of a 'byte'.  The above changes were done to give the 
                   UASMS the ability to correlate the memory availability requests 
                   with the corresponding reponses.

6/17/02     RG     Defined the base value ( MN_WMS_MT_MESSAGE_IDENTIFIER_BASE )
                   that is added to the SMS connection id and sent in 
                   SM_RL_REPORT_IND.short_message_identifier from MN to WMS during
                   MN layer and lower layer failures in an MT SMS.

16/02/06    NR     Adding new reject cause for handling CNM MO SMS timer expiry

04/09/07    RD     SMS optimization changes are incorporated
===========================================================================*/

#include "mn_uasms.h"

/*****************************************************
 * Constants used in SM-RL primitive data structures *
 *****************************************************/


/* 
 * Base value that is added to the SMS connection id. This is the value set in
 * SM_RL_REPORT_IND.short_message_identifier from MN to WMS. 
 */
#define  MN_WMS_MT_MESSAGE_IDENTIFIER_BASE 0x1000
#define  MN_WMS_ACK_REPORT_IDENTIFIER_BASE 0x2000

/* 
 * Link control constants
 */
#define  SM_LINK_CONTROL_MAX_DURATION   5

#endif


