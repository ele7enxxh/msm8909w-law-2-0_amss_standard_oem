/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    C D M A    R E C E I V E   M E S S A G E   D E L I V E R Y   L A Y E R

GENERAL DESCRIPTION
  This module delivers L3 messages to the calling entity (MC).

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc. 
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcmsg.h_v   1.0   06 Dec 2001 12:39:02   ldefauw  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/src/rxcmsg.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/10   jtm     Initial SU API split for CP.
11/05/10   jtm     Added more _i.h includes for Modem 1x SU API support.
03/26/09   jtm     Removed cai.h include. It is not needed.
11/19/08   jtm     Split various header files into internal and external
                   declarations for Common Modem Interface.
08/11/05   bn      Added logic to relay the page match event to MC.
12/06/01   lad     Created file.

===========================================================================*/

//common
#include "comdef.h"
#include "customer.h"

#include "1x_variation.h"
#ifdef FEATURE_IS2000_REL_A

//cp
#include "cai_v.h"
#include "caix.h"
#include "caix_i.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION RXCMSG_DELIVER

DESCRIPTION
  This function delivers a message to the mcc subtask from the rxtx task.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean indicating success.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxcmsg_deliver (
  caix_chan_type chan,
  cai_rx_msg_type *ext_msg_ptr,
  unsigned int length,
  qword *frame_num,
  boolean page_match
);
#endif /* FEATURE_IS2000_REL_A */
