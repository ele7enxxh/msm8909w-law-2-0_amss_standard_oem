#ifndef _DS_QMI_CSVT_H
#define _DS_QMI_CSVT_H
/*===========================================================================

                         D S _ Q M I _ C S V T . H

DESCRIPTION

 The Qualcomm MSM Interface for Circuit-switched Video Telephony header file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_csvt.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
01/05/12    sa     New CSVT service.
08/22/11    kk     Initial version.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dsucsdappif.h"

#if (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS))
/*===========================================================================

                            VARIABLE DEFINITIONS

===========================================================================*/

/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_CSVT_INIT()

  DESCRIPTION
    Initializes the QMI CSVT service on the given instance

  PARAMETERS
    inst - the QMI instance on which the service initialization is sought

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern void qmi_csvt_init
(
  void
);

#endif /* (defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)) */
#endif /* _DS_QMI_CSVT_H */
