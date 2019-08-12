#ifndef _SYS_PLMN_H_
#define _SYS_PLMN_H_
/*==============================================================================

      S Y S T E M   P U B L I C   L A N D   M O B I L E   N E T W o R K
                              H E A D E R   F I L E

DESCRIPTION

  This header file contains the type containing the ID of a  Public Land Mobile
  Network (PLMN).  The type and associated functions in this file can used by
  all software layers.  As a result, the functions in this file may be reentrant
  and appropriate caution should be taken when modifying existing functions or
  adding new functions to this file.

  A PLMN ID is defined within a GSM network to be the combination of a Mobile
  Country Code (MCC) and Mobile Network Code (MNC).  A PLMN ID is stored in 3
  octets as specified in 3G TS 24.008 Section 10.5.1.3 and shown below:

                                        Bit
                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
                 |===============================================|
         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
                 |-----------------------------------------------|
         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
                 |-----------------------------------------------|
         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
                 |===============================================|

  The criteria for comparing one PLMN against another is specified in
  3G TS 23.122 Annex A.

         COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
         EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_plmn.h_v   1.1   28 May 2002 11:11:20   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sys_plmn.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/04/02   kwa     Initial version.
05/07/02   kwa     Changed typedef for PLMN_id_T to sys_plmn_id_s_type.
==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/
#include "sys.h"

/*==============================================================================

                             DATA DECLARATIONS

==============================================================================*/

typedef sys_plmn_id_s_type PLMN_id_T;

/*==============================================================================

                           FUNCTION DECLARATIONS

==============================================================================*/

#endif /* #ifndef _SYS_PLMN_H_ */
