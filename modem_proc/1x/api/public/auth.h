#ifndef AUTH_H
#define AUTH_H

/*===========================================================================

               E X T E R N A L   A U T H E N T I C A T I O N
                      T A S K   D E F I N I T I O N S
*/
/**
@file auth.h

This contains all the declarations for the Authentication Task.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CDMA2k_1X_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and Latex. To edit or update 
      any of the file/group text in the PDF, edit the CDMA2k_1X_rapi_mainpage.dox file or 
      contact Tech Pubs.

      The above description for this file is part of the "authentication_task" group 
      description in the CDMA2k_1X_rapi_mainpage.dox file. 
===========================================================================*/

/*===========================================================================
  Copyright (c) 1995 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/auth.h_v   1.8   30 Aug 2002 13:34:04   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/auth.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/11   ssh     Added Doxygen comments. 
11/01/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup. 
10/22/09   jtm     Added back in include for comdef.h since it is required for
                   RAPI generation.
07/03/09   jj      CMI phase-1 update.
04/09/09   jj      CMI phase2 changes.
03/18/09   adw     Removed ulpn.h include.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_CP.07.02.09
11/30/05   va      Added new function auth_send_update_a_key_cmd  which is
                   remotable for calling from SVCPRG module.
11/30/05   an      Mainlined FEATURE_IS2000
05/03/05   fc      Mainlined FEATURE_IS2000.
03/19/04   sb      Removed auth_switch_to_private_lcm.
08/30/02   ts      Added support for Run Time R-UIM Enable
08/06/02   lh      Remove SVD ifdefs.
04/12/02   lh      Added support for subaddress in Orig_C.
03/12/02   lh      Added CMEA support for release A messages.
08/09/01   sb      Merged the following from common archive:
                   06/19/01   ts      Moved the VPM size constant to cave.c.  Took out the
                                      redundant #ifdef FEATURE_AUTH.
                   06/13/01   ts      Changed the Generate Keys octets from 1-65 to 0-64.
05/25/01   ts      Removed RUIM server code from auth and added interface to
                   new UIM server.
02/07/01   wli     Split RUIM_UPDATE_RECORD_ST to two states:
                   RUIM_UPDATE_LINEAR_FIXED_RECORD_ST and
                   RUIM_UPDATE_CYCLIC_RECORD_ST.
1/27/01    wli     Added new RUIM commands, INVALIDATE and REHABILITATE.
12/18/00   ck      Changed the request offset in the SSPR Configuration Request
                   Interface to a word from byte.
11/28/00   ck      Added definitions to support IS-683A with the R-UIM.
11/14/00   ts      Added access to the MSISDN in the CDMA directory.
10/11/00   ts      Moved the RUIM status outside of the rpt union field into its
                   own field.
07/16/00   ck      Added CDMA DF definition.
06/12/00   ck      Added definitions for checks related to PPS and access of
                   records from RUIM.
05/30/00   ck      Changed RUIM_UI_CHV_SIG to RUIM_CHV_SIG and also removed
                   RUIM_MAX_CHV_DIGITS as it was defined in ruim.h
05/26/00   ck      Added a comma that caused the compile to break
05/01/00   ck      Added support for RUIM software. Included the new interfaces
                   for RUIM software and featurised it under FEATURE_RUIM.
                   AUTH only interface is featurised under FEATURE_AUTH to give
                   the flexibility of defining only FEATURE_AUTH or
                   FEATURE_RUIM.
04/06/00   va      Merged the following from MSM3100_CP.03.00.17
           ry      Added OTAPA support
05/06/99   kmp     Changed T_ACP to FEATURE_ACP
03/18/99   ck      Merged the support for A-Key exchange and Re-Authentication
09/04/98   pms     Neared the variables to save ROM space
07/28/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
11/17/95   dna     Report back when keys are generated.
08/03/95   dna     Added support for message encryption
06/07/95   dna     Created Module

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/** @addtogroup authentication_task
@{ */ 

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/** Number of digits in the Authentication key (A-key). */
#define AUTH_A_KEY_DIGITS         26
 

/*===========================================================================*/
/* auth_task */
/**
  Entrance procedure for the Authentication task.

  @param[in] dummy    Parameter required for Qualcomm's Real-time Executive 
                      Operating System (REX). 

  @return 
  None.
 
  @dependencies 
  None.
*/ 
extern void auth_task (
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
);


/*===========================================================================*/
/* auth_validate_a_key */
/** 
  Tests whether a specified A-key is valid.

  @param[in] a_key[AUTH_A_KEY_DIGITS]    A-key to be validated.

  @return 
  TRUE  -- A-key is valid.\n
  FALSE -- A-key is invalid.             

  @dependencies 
  None.
*/ 
extern boolean auth_validate_a_key
(
  byte a_key[AUTH_A_KEY_DIGITS]
);

/*~ FUNCTION auth_validate_a_key */
/*~ PARAM a_key ARRAY AUTH_A_KEY_DIGITS */


/*===========================================================================*/
/* auth_send_update_a_key_cmd */
/** 
  Sends a command to update an A-key. 
  
  This function gets an available auth_cmd_type from the auth_free_q queue and 
  constructs an Update A-key command to send to the Authentication task. This 
  command is used by the Service Programming (SVCPRG) module.
  
  @param[in] a_key               Pointer to the new A-key to be used.
  @param[in] num_a_key_digits    Number of digits in the A-key.
  @param[in] nam                 Value of the Number Assignment Module.

  @return 
  TRUE  -- Authentication command was successfully queued.\n
  FALSE -- Otherwise.             

  @dependencies 
  None.

  @sideeffects 
  Queue signal is set for the Authentication task.
*/ 
extern boolean auth_send_update_a_key_cmd
(
  byte * a_key,
  int num_a_key_digits,
  int nam
);

/*~ FUNCTION auth_send_update_a_key_cmd */
/*~ PARAM a_key VARRAY LENGTH num_a_key_digits */

/** @} */ /* end_addtogroup authentication_task */

#endif /* AUTH_H */
