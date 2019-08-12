#ifndef SYS_GW_V_H
#define SYS_GW_V_H
/*===========================================================================

              S Y S T E M   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions that are shared between Call Manager,
  Call Manager clients and the GSM/WCDMA protocol stacks.

Copyright (c) 2002 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sys_gw_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/28/11   rk      Adding lte_csfb_service_type for S102 less 1XCSFB changes
01/04/11   rm      CMI-4: Removing #ifdef's
09/21/10   rk      Added service type enum for CSFB
07/30/08   rm      CMI defeaturisation
07/01/08   rm      Mainlining of MBMS changes
02/19/08   sk      Moved cause.h out of FEATURE_MM_SUPERSET
10/15/07   rn      Added support for UE initiated dormancy
09/14/07   dm      Added support for FEATURE_MM_SUPERSET
03/16/05   ic      Include cmwmsg.h in cm.h directly instead of indirectly 
                   (through sys_gw.h) as cm.h uses MAXNO_FORWARD_TO_NUMBER 
                   that is defined in cmwmsg.h
02/06/04   ws      Initial jaguar/mainline merge.
01/21/03   ws      Updated copyright information for 2003
08/29/02   PRK     Featurized include files.
05/07/02   PRK     Initial version.
===========================================================================*/
#include "cm_gw.h"          /* Defintions from NAS required by clients */  
#include "cause.h"          /* Contains definitions taken from GSM 04.08   */
                            /* Table 10.86 and more                        */
#include "sys_gw.h"

/*--------------------------------------------------------------------------
   MBMS (Multimedia broadcast multicast service) MACRO definitions.   
   MBMS is specific to WCDMA.
--------------------------------------------------------------------------*/

/* TMGI is a parameter used in 
** MBMS service act req. Gives
** the max length of TMGI.
** TMGI stands for ???
*/
#define SYS_GW_MBMS_MAX_TMGI_LENGTH 6






/* TMGI structure in array format */
typedef struct
{
  /* TRUE  -  TMGI is present
  ** FALSE -  TMGI is not present
  */
  boolean present;

  /* Length of TMGI */
  uint8   length;

  /* Actual TMGI value */
  uint8   value[SYS_GW_MBMS_MAX_TMGI_LENGTH];

}sys_gw_mbms_tmgi_s_type;



/* This is used for CSFB calls (In case of 1XCSFB it used in CM->NAS
** and for 3GPP CSFB its used internally in NAS) to convey why 
** is call origination request is triggered or ESR procedure is being 
** initiated.
*/

typedef enum
{
  LTE_MO_CSFB_1xCSFB,               /* MO CSFB call */
  LTE_MT_CSFB_1XCSFB,               /* MT CSFB call */
  LTE_MO_CSFB_ECALL_1XCSFB_ECALL,   /* Emergency CSFB call */
  LTE_MO_CSFB_S102_LESS_1XCSFB,     /* ESR trigger for S102 less 1X access*/
  LTE_PACKET_SERVICES_VIA_S1 = 8    /* LTE PACKET SERVICES VIA S1*/
}lte_csfb_service_type;



#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
FUNCTION      sys_gw_mbms_conv_tmgi_to_tmgi_uint64

DESCRIPTION   Given the TMGI in array format, converts it to TMGI in uint64 
              format.

              Index 0 in the array becomes the MSB of the uint64 value.
              For eg:-
              
              Input: tmgi->value[0] = 0x01, tmgi->value[1] = 0x02, ...
              Output: tmgi_uint->tmgi = 0x0102...
 
              
PARAMETERS    tmgi      -- TMGI from Network. TMGI is character array of digits.
              tmgi_uint -- TMGI in uint64 format. 


DEPENDENCIES  NONE

RETURN VALUE  TRUE- if TMGI passed is correct.
              FALSE - if TMGI passed is incorrect.

SIDE EFFECTS  None.
===========================================================================*/

extern boolean sys_gw_mbms_conv_tmgi_to_tmgi_uint64
(
  sys_gw_mbms_tmgi_s_type *tmgi,
  sys_gw_mbms_tmgi_uint_s_type *tmgi_uint
);


/*===========================================================================
MACRO         sys_gw_mbms_conv_tmgi_uint64_to_tmgi

DESCRIPTION   Given the TMGI in the UINT64 format , converts it to an array 
              of bytes
              MSB of the input is stored in index 0 of the output array. 
              For eg:-

              Input: tmgi_uint->tmgi = 0x0102...
              Output: tmgi->value[0] = 0x01, tmgi->value[1] = 0x02 and so on.

PARAMETERS    tmgi      -- TMGI in uint64 format
              tmgi_uint -- TMGI in array format


DEPENDENCIES  

RETURN VALUE  TRUE- if TMGI passed is correct.
              FALSE - if TMGI passed is incorrect.

SIDE EFFECTS  None.
===========================================================================*/

extern boolean sys_gw_mbms_conv_tmgi_uint64_to_tmgi
(
  sys_gw_mbms_tmgi_uint_s_type *tmgi_uint,
  sys_gw_mbms_tmgi_s_type *tmgi
);

#ifdef __cplusplus
}
#endif
#endif /*SYS_GW_V_H*/
