#ifndef __APR_DIAG_H__
#define __APR_DIAG_H__

/*
   Copyright (C) 2010 Qualcomm Technologies Incorporated.
   All rights reserved.
   QUALCOMM Proprietary/GTDR.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/apr/datalink/diag/inc/aprv2_diag.h#1 $
   $Author: mplcsds1 $
*/

#include "apr_comdef.h"
#include "aprv2_packet.h"

APR_INTERNAL int32_t aprv2_diag_init ( void );
APR_INTERNAL int32_t aprv2_diag_deinit ( void );

APR_INTERNAL int32_t aprv2_diag_send ( aprv2_packet_t* packet );

#endif /* __APR_DIAG_H__ */

