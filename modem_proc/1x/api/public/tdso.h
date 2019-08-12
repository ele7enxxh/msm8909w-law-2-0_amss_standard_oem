#ifndef TDSO_H
#define TDSO_H
/*===========================================================================


         T E S T   D A T A   S E R V I C E   O P T I O N

                  H E A D E R    F I L E


DESCRIPTION
  This header file contains functions that are intended to be used internally
  within the simple/Full Test Data Service Options for configuring, managing
  and updating statistics the S-TDSO service option.

  Copyright (c) 1998 - 2013 Qualcomm Technologies, Inc. 
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

            EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/tdso.h_v   1.15   29 Aug 2002 19:28:16   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/tdso.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
04/13/09   adw     Split file for CMI phase-I and added temporary _v.h include.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
07/09/04   ht      Added TDSO support for Rel C DV channels
01/10/03   lh      Added FEATURE_IS2000_REL_A around 32x counters.
08/28/02   fc      Cleaned lint errors.
06/28/02   lh      Reverse MuxPUD 5 support & Forward MuxPDU clean up.
05/24/02   lh      Fixed FTDSO.
05/07/02   ph      Fixed compile errors when REL A is turned off.
04/08/02   va      Fixed the array size of tdso_data_len
02/12/01   yll     Merged from common archive CP VU 116.
                   12/13/01   fc
                   Changed tdso_tst_par_type and tdso_buffer_info.
12/02/01   kk      Secondary seupport for S-TDSO
08/10/01   kk      32X support.
08/09/01   kk      Merged from common archive.
                   08/06/01   kk      DCCH Full TDSO support.
06/07/01   kk      Mixed Rate Support
05/22/01   kk      RS2 Changes.
05/14/01   fc      Added definition of new log packet for DCCH S-TDSO.
05/14/01   fc      Changed MAX_PDU_LENGTH to 68.
05/11/01   fc      Added support for DCCH STDSO.
                   Cleaned lint errors.
03/21/01   lcc     Added function prototype for tdso_end_call and replaced
                   tdso_send_handoff_complete with tdso_notify_hho_success.
03/12/01   fc      Removed "#include msg.h" and "#include err.h".
01/20/01   ks      Fixed PN initialization, ON/OFF period and counter updates.
11/13/00   ks      Added PN and RAND info to the cicular_buffer_info structure.
11/05/00   ks      Added Support for FULL TDSO Service Option.
09/20/00   fc      Added support for TDSO control directives and counter
                   retrieval directives.
08/11/00   va      Changed names of  process_rev_fch_frame and process_rev_sch_frame
                   to process_rx_fch_frame and process_rx_sch_frame respectively.
07/06/00   ks      Created file.

===========================================================================*/

/* <EJECT> */
/*==========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION tdso_task

DESCRIPTION
  This function is the entrance procedure for the TDSO Task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  None

===========================================================================*/
void tdso_task
(
  dword ignored
);

#endif /* TDSO_H */
