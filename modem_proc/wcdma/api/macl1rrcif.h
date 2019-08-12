#ifndef MACL1RRCIF_H
#define MACL1RRCIF_H

/*========================================================================================

                MAC L1 RRC INTERFACE HEADER FILE

DESCRIPTION
  This file contains the declaration of the interface functions and
  data types used by MAC, L1, and RRC

  Copyright (c) 2002 by Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2004,2007-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macl1rrcif.h_v   1.0   10 May 2002 13:24:00   yiz  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/macl1rrcif.h#1 $  $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------------------------
01/04/13   kc    Removing duplicate/redundant code
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/26/12    grk     Added new API file exposing L2 command structures
10/18/10    ssg     Added changes for new WCDMA-L2 interface.
05/07/09    ssg     Updated Copyright Information
03/01/09    rm      Added new featurization for FEATURE_CMI.
02/18/08    mrg     Added mvs.h in #include list under feature flag FEATURE_VOC_AMR_WB
03/03/07    sk      Added support for WB-AMR under FEATURE_VOC_AMR_WB
09/02/04    sk      Added TFC_MANDATORY field to the mac_ul_tfc_control_e_type.
                    This indicates that this TFCI is part of the minimum set and hence can't be
                    subjected to the elimination due to CM by HLS or Tx power
                    restriction
11/21/02    scm     Don't #ifdef entire file with FEATURE_WCDMA_COMP_GF.
05/08/02    yiz     Initial release

========================================================================================*/

/*=======================================================================================

                     INCLUDE FILES

========================================================================================*/
#include "customer.h"
#include "comdef.h"
#include "wl2api.h"
/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

#endif /* MACL1RRCIF_H */

