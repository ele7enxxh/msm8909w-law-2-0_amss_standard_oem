#ifndef SAMP_SERV_H
#define SAMP_SERV_H
/*===========================================================================
                      S A M P S E R V . H

GENERAL DESCRIPTION
  This file provides support for logging sample server data to diag.
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) 2006 - 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/sampserv.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
05/23/13    geg     WSW decoupling (except for component of QCHAT)
09/22/09    sup     Removed log code assignments and placed them in 
                    log_codes_wcdma_int_v.h
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
01/11/08    mg      Add samp_serv_update_mdsp_config_data
10/24/07    rmak    Externed samp_serv_log_pkt_enabled
11/28/06    mg      Correct the log packet definition
11/13/06    mg      7200 support
01/23/06    mg      Initial Version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "log.h"
#include "log_codes_wcdma_int.h"

#endif /* SAMP_SERV_H */

