#ifndef QC_MULTI_THREADING
#define QC_MULTI_THREADING
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
              M u l t i t h r e a d i n g  W r a p p e r  C l a s s
                         D E C L A R A T I O N S

  The following declarations are used to wrap REX and windows mobile functions.


  -------------

    Copyright (c) 1991-2001,2010-2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/qcmt.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#ifdef QC_WINCE_VERSION
#include <Windows.h>
#define qc_init_crit_sect InitializeCriticalSection
#define qc_enter_crit_sect EnterCriticalSection
#define qc_leave_crit_sect LeaveCriticalSection
#define qc_crit_sect_type CRITICAL_SECTION
#else
#include "rex.h"
#define qc_init_crit_sect rex_init_crit_sect
#define qc_enter_crit_sect rex_enter_crit_sect
#define qc_leave_crit_sect rex_leave_crit_sect
#define qc_crit_sect_type rex_crit_sect_type
#endif /* QC_WINCE_VERSION */
#endif /* QC_MULTI_THREADING */
