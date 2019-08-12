#ifndef FTM_TDSCDMA_DISPATCH_H
#define FTM_TDSCDMA_DISPATCH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      F T M  W C D M A  D I S P A T C H   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM TDSCDMA Commands 


Copyright (c) 2002, 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/ftm_tdscdma_dispatch.h#1 $
$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/11   hy     Created
===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmdiag.h"
#include "ftm.h"

//PROTOTYPES

/*===========================================================================

FUNCTION FTM_TDSCDMA_DISPATCH

DESCRIPTION
   This function handles tdscdma ftm requests to run tests and  other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/     
ftm_rsp_pkt_type ftm_tdscdma_dispatch(rfm_device_enum_type device, ftm_pkt_type  *cmd_ptr);

/*===========================================================================

FUNCTION      FTM_TDSCDMA_GET_RX_CHAIN_HANDLE

DESCRIPTION   Performa a mapping from rfcom device type to an 
              FTM receive chain type.  The mapping is simple, and
              always implies that Transceiver 0 is the Primary 
              receive path during FTM mode.


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_receive_chain_handle_type ftm_tdscdma_get_rx_chain_handle( rfcom_device_enum_type device);


#endif  // FEATURE_FACTORY_TESTMODE
#endif  // FTM_TDSCDMA_DISPATCH_H
