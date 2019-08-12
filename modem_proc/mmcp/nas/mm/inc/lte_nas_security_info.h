/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*==============================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/lte_nas_security_info.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when         who                          what                              
--------     --------  ------------------------------------------------
11/10/09      hnam     Added flags for int and enc for UL/DL
09/28/09      hnam     Moved nas_msg_integrity_check() extern defs to "lte_nas_msg_parse.h", 
                       to avoid compiler warning
06/15/09      hnam     Added "integrity_protect_nas_msg" & 
                       add_nas_sec_hdr_first_octet extern definitions
==============================================================================*/

#ifndef _LTE_NAS_SECURITY_INFO_H_
#define _LTE_NAS_SECURITY_INFO_H_

/*==============================================================================
                         HEADER 
==============================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "lte.h"
#include "lte_nas.h"
#include "lte_security.h"

/*==============================================================================
                      DATA DECLARATIONS
==============================================================================*/

//#define LTE_NAS_FEATURE_NULL_INTEGRITY
//#define  SECURITY_SUPPORTED_FOR_ESM_MESSAGES
typedef struct
{
  lte_cipher_algo_e    ciphering_alg;
  byte                 ciphering_alg_key[16];
  lte_integrity_algo_e integrity_alg;
  byte                 integrity_alg_key[16];

}lte_nas_emm_sec_alg_info_type;

typedef struct 
{  
  lte_nas_emm_sec_alg_info_type   security_alg;
  byte                            nas_count[4]; /*8 spare bits + 16 bits NAS overflow + 8 bits NAS SQN*/
  byte                            bearer;
  boolean                         cipher_valid;
  boolean                         int_valid;
  
}lte_nas_ul_security_info_type;

typedef struct
{
  lte_nas_emm_sec_alg_info_type   security_alg;
  byte                            nas_count[4]; /*8 spare bits + 16 bits NAS overflow + 8 bits NAS SQN*/
  byte                            bearer;
  byte                            maci[4];
  boolean                         nas_connected_mode;
}lte_nas_dl_security_info_type;

typedef struct
{
  byte sqn;
  byte maci[4];

}lte_nas_sec_info_params_type;


void integrity_protect_nas_msg
(
  byte                          *ota_buf,
  lte_nas_ul_security_info_type *security_info,
  word                          *msg_len,
  lte_nas_outgoing_msg_type     *out_msg 
);

void add_nas_sec_hdr_first_octet
(
  lte_nas_outgoing_msg_type     *out_msg,
  boolean                       encryption_rqd,
  byte                          *ota_buf
);

#endif

#endif /*FEATURE_LTE*/
