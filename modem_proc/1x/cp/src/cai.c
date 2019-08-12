/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       CDMA2000   P R O T O C O L   A C C E S S O R   F U N C T I O N S

DESCRIPTION

  This file contains the accessor functions for some fields' values which
  are dependent on certain features while the mobile station is operating in
  CDMA2000 mode.

  Copyright (c) 1990 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/cai.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/15   ppr     Feature clean up changes
12/02/14   agh     Support for new customer specific voice service option
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
09/13/10   ag      Removed cai_get_xtra_last_parm_id() as last parm id is not
                   part of api/1x/cai.h file.
08/09/10   ag      Added file version information in the file header.
01/20/10   ag      Changed the value of cai_ac_alt_so_max to correspond with
                   CAI_AC_ALT_SO_MAX.
07/17/09   jj      Fixed Linker error
07/14/09   jj      Added function to initialtize cai_ac_alt_so_max.
05/20/09   ag      Fixed compiler warnings
05/13/09   ag      Changed the file header and replaced frwd by fwd.
04/09/09   jj      CMI phase2 changes.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "comdef.h"
#include "cai_v.h"
#include "modem_1x_defs.h"

const uint8 cai_ac_alt_so_max = 10;

uint16 fwd_tc_msg_len;
uint16 rev_tc_msg_len;

/*===========================================================================

FUNCTION CAI_GET_FWD_TC_EXTENDED_MSG_LEN

DESCRIPTION
  This function returns fwd tc msg len to caller based on whether
  FEATURE_TC_EXTENDED_MSG_LENGTH is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  Max Forward Traffic Channel msg length

SIDE EFFECTS
  None

===========================================================================*/
uint16 cai_get_fwd_tc_extended_msg_len(void)
{
  fwd_tc_msg_len = 255;
  /* see IS-95A Section 7.7.3.2.1 */
  return fwd_tc_msg_len;
}

/*===========================================================================

FUNCTION CAI_GET_REV_TC_EXTENDED_MSG_LEN

DESCRIPTION
  This function returns rev tc msg len to caller based on whether
  FEATURE_TC_EXTENDED_MSG_LENGTH is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  Max Reverse Traffic Channel msg length

SIDE EFFECTS
  None

===========================================================================*/
uint16 cai_get_rev_tc_extended_msg_len(void)
{
  rev_tc_msg_len = 255;
  /* see IS-95A Section 6.7.2.2 */
  return rev_tc_msg_len;
}

/*===========================================================================
FUNCTION CAI_GET_AC_ALT_SO_MAX

DESCRIPTION
  This function returns maximum value of ALT SO.

DEPENDENCIES
  None

RETURN VALUE
  maximum value of ALT SO

SIDE EFFECTS
  None

===========================================================================*/
uint8 cai_get_ac_alt_so_max(void)
{
  return cai_ac_alt_so_max;
}

