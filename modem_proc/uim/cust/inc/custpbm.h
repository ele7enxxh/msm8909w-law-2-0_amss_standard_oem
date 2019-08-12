#ifndef CUSTPBM_H
  #define CUSTPBM_H
  /*===========================================================================

              " C u s t -  P B M "   H E A D E R   F I L E

  DESCRIPTION
  Configuration for PBM Feature.

  Copyright (c) 2008-2010, 2014 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
  ===========================================================================*/
  /*===========================================================================

                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/cust/inc/custpbm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  11/28/14   av      Enable FEATURE_MMCP_DISABLE_PBM_EFS under FEATURE_CUST_1
  08/05/14   NR      Removal of unused feature flags
  12/06/10   pk      Initial Revision.
  ===========================================================================*/

#ifdef FEATURE_CUST_1
#define FEATURE_MMCP_DISABLE_PBM_EFS
#endif /* FEATURE_CUST_1 */

#define FEATURE_QMI_PBM_TASK
#endif /* CUSTPBM_H */