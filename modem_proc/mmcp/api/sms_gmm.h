#ifndef SMS_GMM_H
#define SMS_GMM_H

/*===========================================================================
                        S M S - G M M  Header File

DESCRIPTION

   Definition of primitive message types between the Short Message Entity
   and the GPRS Mobility Management Entity (SMS <-> GMM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001, 2002, 2003 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/msm5200/mm/vcs/sms_gmm.h_v   1.0   04 Apr 2001 16:57:34   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sms_gmm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/03   vdr     Added definition to support SMS Bearer Selection
02/06/02   vdr     Created file.
            
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"


#include "sys.h"



/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define MM_SMS_BEARER_SELECTION_CMD 0x01
#define MM_SMS_STATUS_CMD 0x02

typedef enum
{
  CS_PREFERRED,
  PS_PREFERRED,
  CS_ONLY,
  PS_ONLY
} mm_sms_bearer_selection_enum_T;


typedef struct
{
   IMH_T              header;
   boolean sms_over_ip_status;/*0-inactive 1-active*/

   boolean sms_over_nas_status;/*0-inactive 1-active*/

   boolean sms_over_s102;/*0-inactive 1-active*/

   sys_modem_as_id_e_type                   as_id;

}wms_mm_sms_call_info_T;

typedef struct
{
  /* Message header needed by MM */
  IMH_T                            message_header;

  mm_sms_bearer_selection_enum_T   sms_bearer_selection;

  sys_modem_as_id_e_type           as_id;


  
} mm_sms_bearer_selection_req_T ;

#endif /* SMS_GMM_H */

