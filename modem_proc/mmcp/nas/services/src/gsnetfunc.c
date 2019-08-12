/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/msm5200/nas/vcs/gsnetfunc.c_v   1.0   06 Apr 2001 12:04:12   tschwarz  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/gsnetfunc.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------



===========================================================================*/


/* Include files */
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "aconfig.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"


/*******************************************************************************
 *
 *  Function name: normalise_mnc() 
 *  ----------------------------
 *
 *  Description:
 *  ------------
 *  Converts 2 digit GSM1900 Mobile Network Codes into 3 digit format.
 *  If all GSM1900 MNCs in the protocol stack are 3 digits, they can be
 *  compared.
 *
 *  e.g. net code = 234 15 (UK Vodafone)
 *                  input =    [32.F4.51]
 * on GSM 900/1800, change to: [32.F4.51]  i.e. no change
 * on GSM 1900,     change to: [32.54.10]
 *   
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC normalise_mnc(byte * net, byte num_nets)  /* ptr to 3 bytes per net*/
{
#ifdef PCS_1900
   #error code not present
#endif
}

/*******************************************************************************
 *
 *  Function name: pack_net_list() 
 *  ------------------------------
 *
 *  Description:
 *  ------------
 *  Converts a list of networks from array of PLMN_id_T into
 *  continuos stream of bytes, each plmn of 3 bytes length.
 *  Although PLMN_id_T is 3 bytes in length, some compilers will add
 *  padding bytes between PLMN_ids.
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC pack_net_list(byte *      net_list_bytes,
                        PLMN_id_T * net_list_struct,
                        word        num_nets)
{
   word  n = 0;
   dword index = 0;

   for(n = 0; n < num_nets; n++)
   {
      net_list_bytes[index++] = net_list_struct[n].identity[0];
      net_list_bytes[index++] = net_list_struct[n].identity[1];
      net_list_bytes[index++] = net_list_struct[n].identity[2];
   }
}


/*******************************************************************************
 *
 *  Function name: unpack_net_list() 
 *  --------------------------------
 *
 *  Description:
 *  ------------
 *  Opposite of pack_net_list().
 *
 *  Parameters:
 *  -----------
 *  None
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC unpack_net_list(byte *      net_list_bytes,
                          PLMN_id_T * net_list_struct,
                          word        num_nets)
{
   word  n = 0;
   dword index = 0;

   for(n = 0; n < num_nets; n++)
   {
      net_list_struct[n].identity[0] = net_list_bytes[index++];
      net_list_struct[n].identity[1] = net_list_bytes[index++];
      net_list_struct[n].identity[2] = net_list_bytes[index++];
   }
}
