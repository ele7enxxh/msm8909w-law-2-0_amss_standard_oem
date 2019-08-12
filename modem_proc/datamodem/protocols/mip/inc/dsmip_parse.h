#ifndef DSMIP_PARSE_H
#define DSMIP_PARSE_H
/*===========================================================================

                   SNOOP HEADER FILE

DESCRIPTION

 The Data Services Snoop Header File. Contains shared variables and enums, as
 well as declarations for functions.

Copyright (c) 2000,2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_parse.h_v   1.4   12 Jul 2002 15:11:54   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsmip_parse.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/13   vl      Optimization of F3 messages
04/23/09    ls     Add the support to report MIP rrp code
05/07/02   jd      Moved FA info block to dsmip.c
08/14/01   jd      added from_ip argument to mip_parse_msg()
07/17/01   jd      changed mip_parse_msg argument types for memory efficiency
06/11/01   jd      dsmip_parse_msg accepts arbitrary pointer to data to parse
10/20/00   na/mvl  Created module.
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/*---------------------------------------------------------------------------
   Registration Reply codes
---------------------------------------------------------------------------*/
#define MIP_RRP_CODE_SUCCESS                                   (0)
#define MIP_RRP_CODE_SUCCESS_NO_SIM_BINDINGS                   (1)
#define MIP_RRP_CODE_FAILURE_FA_REASON_UNSPECIFIED             (64)
#define MIP_RRP_CODE_FAILURE_FA_ADMIN_PROHIBITED               (65)
#define MIP_RRP_CODE_FAILURE_FA_INSUFFICIENT_RESOURCES         (66)
#define MIP_RRP_CODE_FAILURE_FA_MOBILE_NODE_FAILED_AUTH        (67)
#define MIP_RRP_CODE_FAILURE_FA_HA_FAILED_AUTH                 (68)
#define MIP_RRP_CODE_FAILURE_FA_REQUESTED_LIFETIME_TOO_LONG    (69)
#define MIP_RRP_CODE_FAILURE_FA_MALFORMED_REQUEST              (70)
#define MIP_RRP_CODE_FAILURE_FA_MALFORMED_REPLY                (71)
#define MIP_RRP_CODE_FAILURE_FA_ENCAPSULATION_UNAVAILABLE      (72)
#define MIP_RRP_CODE_FAILURE_FA_VJHC_UNAVAILABLE               (73)
#define MIP_RRP_CODE_FAILURE_FA_CANT_REV_TUN                   (74)
#define MIP_RRP_CODE_FAILURE_FA_MUST_REV_TUN                   (75)
#define MIP_RRP_CODE_FAILURE_FA_BAD_TTL                        (76) 
/* set TTL = 255 */
#define MIP_RRP_CODE_FAILURE_FA_DELIVERY_STYLE_NOT_SUPPORTED   (79)    
#define MIP_RRP_CODE_FAILURE_FA_VS_REASON                      (89)  
/* vendor specific */
#define MIP_RRP_CODE_FAILURE_MISSING_MISSING_NAI               (97)
#define MIP_RRP_CODE_FAILURE_MISSING_HA_ADDR                   (98)
#define MIP_RRP_CODE_FAILURE_MISSING_HOMEADDR                  (99)
#define MIP_RRP_CODE_FAILURE_UNKNOWN_CHALLENGE                 (104)
#define MIP_RRP_CODE_FAILURE_MISSING_CHALLENGE                 (105)
#define MIP_RRP_CODE_FAILURE_STALE_CHALLENGE                   (106)
#define MIP_RRP_CODE_FAILURE_HA_REASON_UNSPECIFIED             (128)
#define MIP_RRP_CODE_FAILURE_HA_ADMIN_PROHIBITED               (129)
#define MIP_RRP_CODE_FAILURE_HA_INSUFFICIENT_RESOURCES         (130)
#define MIP_RRP_CODE_FAILURE_HA_MOBILE_NODE_FAILED_AUTH        (131)
#define MIP_RRP_CODE_FAILURE_HA_FA_FAILED_AUTH                 (132)
#define MIP_RRP_CODE_FAILURE_HA_REG_ID_MISMATCH                (133)
#define MIP_RRP_CODE_FAILURE_HA_MALFORMED_REQUEST              (134)

#define MIP_RRP_CODE_FAILURE_UNKNOWN_HA                        (136)
#define MIP_RRP_CODE_FAILURE_HA_CANT_REV_TUN                   (137)
#define MIP_RRP_CODE_FAILURE_HA_MUST_REV_TUN                   (138)
#define MIP_RRP_CODE_FAILURE_HA_ENCAPSULATION_UNAVAILABLE      (139)

/*===========================================================================

                      REGIONAL DATA DECLARATIONS 

===========================================================================*/
typedef enum
{
  FA_FAIL_DMU_IN_MIP_DEREG = 1,
  FA_FAIL_DMU_INIT_FAILED,
  FA_FAIL_3GPD_AUTH_R_UIM,
  FA_FAIL_USING_UI_USER_INFO,
  FA_FAIL_USING_SHADOW_PARAMS,
  FA_FAIL_ENCR_FAILED,
  FA_FAIL_NO_PUBLIC_KEY,
  FA_FAIL_RE_ENCR_FAILED,
  FA_FAIL_KEY_NO_MATCH,
  FA_FAIL_UNKNOWN_REASON  
} mip_fa_failure_cause;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                      PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*===========================================================================

FUNCTION MIP_PARSE_MSG

DESCRIPTION
  Processes a received message passed from the mipio layer

PARAMETERS
  prot - rx'd message protocol (PS_IPPROTO_ICMP or PS_IPPROTO_UDP)
  data - pointer to rx'd message
  len  - length of rx'd message

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mip_parse_msg
(
  uint8   idx,                            /* connection on which msg rx'd  */
  void *  data,                           /* data received                 */
  uint32  from_ip,                        /* sender's IP address length    */
  uint16  len                             /* data length                   */
);

#endif /* DSMIP_PARSE_H */
