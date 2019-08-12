#ifndef PUZL_H
#define PUZL_H

/*===========================================================================

                   P R E F E R R E D   U S E R   Z O N E   L I S T

                             H E A D E R   F I L E

DESCRIPTION
  This header file contains all the declarations necessary for
  PUZL module interface

Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/puzl.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/30/10   rm      Initial release.

===========================================================================*/


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== EXTERNAL TYPES ===============================
=============================================================================
=============================================================================
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Defines for field lengths in PUZL as given in I-683B specifications
** useful when extracting bit packed fields in PUZL.
*/
/* Maximum user zone name length.
*/
#define  PUZL_MAX_UZ_NAME_LEN          32

/* Enumeration for User Zone Active flag type
** I-683B specifications 3GPP2 C.S0016-A, Section 3.5.7.
*/
typedef enum {
  PUZL_UZ_ACT_FLAG_PASSIVE         = 0,
  PUZL_UZ_ACT_FLAG_REGISTER        = 1,
  PUZL_UZ_ACT_FLAG_MAX
} puzl_uz_act_flag_e_type;

/* structure to hold user zone name information.
*/
typedef struct {
  byte     uz_name_encoding;
  byte     uz_name_n_octets;
  byte     uz_name[PUZL_MAX_UZ_NAME_LEN];
} puzl_uz_name_s_type;

#endif /* PUZL_H */

