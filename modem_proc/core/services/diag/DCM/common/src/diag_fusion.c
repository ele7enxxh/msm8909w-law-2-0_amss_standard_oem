/*==========================================================================

              DIAG_FUSION  STUB FILE

DESCRIPTION
  This source file contains all the functions whose name was changed to work
  on LTE modem. The functions declared here acts as a wrapper over the
  original function calls present on the LTE modem.

  This file was AUTO GENERATED on: Wed May 25 2011
  
Copyright (c) 2002 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
/*=============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diag_fusion.c#1 $
=============================================================================*/

/* Include Files */
#include "diag_fusion.h"


/* Wrapper Functions for the LTE modem */




/* Wrapper Function for diag_cmd_req */

  boolean diag_cmd_req_fusion (const byte *req, unsigned int length, 
                        const diag_cmd_rsp_fusion rsp_cb, void *param)
{

   boolean  ret_val;

 #ifdef DIAG_MV_CMD_RPC

 ret_val = diag_cmd_req_fusion_on_modem (
 req ,
 length ,
 rsp_cb ,
 param
 );
#else
 ret_val = diag_cmd_req (
  req ,
  length ,
  rsp_cb ,
  param
  );
#endif

 
 return ret_val;
}



