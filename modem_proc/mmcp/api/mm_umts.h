#ifndef _MM_UMTS_H
#define _MM_UMTS_H
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mm_umts.h_v   1.16   13 Jun 2002 12:09:24   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mm_umts.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/12/01   jca     Modified mm_send_nas_msg_to_rrc() function prototype.
04/24/01   sbk     Added function prototypes for rrc conn management
05/03/01   jca     Rewrote functions for RRC<->MM PLMN type conversion.
05/11/01   jca     Added establishment cause to send_L3_message_to_rrc().
09/18/01   sbk     New macros to set bits for the CSN.1 encoding
04/23/02   jca     Deleted redundant function mm_send_nas_msg_to_rrc().
06/05/02   ks      Added support for Dual Mode interfaces.
09/05/02   mks     Updated the function prototype for mm_check_for_integrity_protection()
12/04/03   jca     Deleted unused function get_lai_from_service_info().
01/21/05   ks      Added function prototype for mm_waiting_for_nw_command()
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "environ.h"
#include "mm.h"
#include "mm_rr.h"
#include "rrcmmif.h"
#include "rrccmd.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define BIT_MASK(bit)      (1 << (bit))
#define BIT_CLR(x,bit)     ((x) &= ~BIT_MASK (bit))
#define BIT_SET(x,bit)     ((x) |= BIT_MASK (bit))
#define IS_BIT_SET(x,bit)  ((x) & BIT_MASK (bit))

#define UPPER_BCD(x)  (((x) & 0xF0) >> 4)
#define LOWER_BCD(x)  ( (x) & 0x0F)


/*
 * DEFINITIONS
 */
#define AUTH_3G_UMTS_CONTEXT_SUCCESS 0xDB
#define AUTH_SYNCH_FAILURE  0xDC



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


boolean  mm_waiting_for_nw_command
(
void
);

boolean  mm_per_subs_waiting_for_nw_command
(
  sys_modem_as_id_e_type as_id
);

/*
 * EXTERNS
 */

extern sys_plmn_id_s_type mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
                    rrc_plmn_identity_type plmn_id );

extern rrc_plmn_identity_type mmumts_convert_nas_plmn_id_to_rrc_plmn_id(
                                 sys_plmn_id_s_type plmn_id );


#endif /* _MM_UMTS_H */
