#ifndef DSWCSDV80_H
#define DSWCSDV80_H
/*===========================================================================

         W C D M A   C I R C U I T - S W I T C H E D   D A T A   V 8 0

                          H E A D E R   F I L E

DESCRIPTION
  This file contains common data declarations and function prototypes for
  Synchronous Access Mode, Trnasparent sub-Mode of ITU-T V.80.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003-2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:$
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dswcsdv80.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
02/12/03   dwp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)

#ifdef FEATURE_DATA_WCDMA_CS

#ifdef FEATURE_DATA_V80

#include "comdef.h"
#include "queue.h"
#include "dsm.h"
#include "sys_cnst.h"
#include "sys_stru.h"
#include "sys_type.h"
#include "cm.h"
#include "ds_cap.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef enum
{
/*---------------------------------------------------------------------------
  V.80 Synchronous Access Mode In-Band Commands. Reference Table 9/V.80.
  For now, only the RATE command is supported towards the DTE.

  General V.80 SAM commands.
---------------------------------------------------------------------------*/
  DSWCSDV80_MARK_CMD      = 0xB0,
  DSWCSDV80_FLAG_CMD      = 0xB1,
  DSWCSDV80_ERR_CMD       = 0xB2,
  DSWCSDV80_HUNT_CMD      = 0xB3,
  DSWCSDV80_UNDER_CMD     = 0xB4,
  DSWCSDV80_TOVER_CMD     = 0xB5,
  DSWCSDV80_ROVER_CMD     = 0xB6,
  DSWCSDV80_RESUME_CMD    = 0xB7,
  DSWCSDV80_BNUM_CMD      = 0xB8,
  DSWCSDV80_UNUM_CMD      = 0xB9,
/*---------------------------------------------------------------------------
  V.80 SAM Duplex Carrier Control commands
---------------------------------------------------------------------------*/
  DSWCSDV80_EOT_CMD       = 0xBA,
  DSWCSDV80_ECS_CMD       = 0xBB,
  DSWCSDV80_RRN_CMD       = 0xBC,
  DSWCSDV80_RTN_CMD       = 0xBD,
  DSWCSDV80_RATE_CMD      = 0xBE,
/*---------------------------------------------------------------------------
  V.80 SAM Half Duplex Carrier Control commands
---------------------------------------------------------------------------*/
  DSWCSDV80_PRI_CMD       = 0xBC,
  DSWCSDV80_CTL_CMD       = 0xBF,
  DSWCSDV80_RTNH_CMD      = 0xBD,
  DSWCSDV80_RTNC_CMD      = 0xC0,
  DSWCSDV80_RATEH_CMD     = 0xBE,
  DSWCSDV80_EOTH_CMD      = 0xBA,
  DSWCSDV80_CMD_MAX       = 0xFF
} dswcsdv80_cmd_enum_type;

/*---------------------------------------------------------------------------
  V.80 SAM Transparency Characters
---------------------------------------------------------------------------*/
#define DSWCSD_V80_EM7           0x19
#define DSWCSD_V80_EM8           0x99
#define DSWCSD_V80_DC1           0x11
#define DSWCSD_V80_DC3           0x13

/*---------------------------------------------------------------------------
  SYN1 variable, from +ESA. 

  This variable will be set based on values read from dsat_esa_export_val[].
---------------------------------------------------------------------------*/

void dswcsdv80_dl_inband_escape
(
  dsm_item_type **pkt_ptr
);

void dswcsdv80_send_cmd_to_dte
(
  dswcsdv80_cmd_enum_type cmd /* the command to be sent to the DTE */
);

dsm_item_type *dswcsdv80_build_ul_sdu_with_flag(void);

boolean dswcsd_v80_from_dte
(
  dsm_item_type **item_ptr_passed
);

void dswcsdv80_init(void);

/*===========================================================================

FUNCTION DSWCSDV80_GET_UL_R_IFACE_MODE_IS_V80

DESCRIPTION
  Returns the value of dswcsdv80_ul_r_iface_mode_is_v80

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean dswcsdv80_get_ul_r_iface_mode_is_v80(void);

/*===========================================================================

FUNCTION DSWCSDV80_SET_UL_R_IFACE_MODE_IS_V80

DESCRIPTION
  Sets the value of dswcsdv80_ul_r_iface_mode_is_v80

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsdv80_set_ul_r_iface_mode_is_v80
(
  boolean value
);

/*===========================================================================

FUNCTION DSWCSDV80_GET_DL_R_IFACE_MODE_IS_V80

DESCRIPTION
  Returns the value of dswcsdv80_dl_r_iface_mode_is_v80

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean dswcsdv80_get_dl_r_iface_mode_is_v80(void);

/*===========================================================================

FUNCTION DSWCSDV80_SET_DL_R_IFACE_MODE_IS_V80

DESCRIPTION
  Sets the value of dswcsdv80_dl_r_iface_mode_is_v80

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsdv80_set_dl_r_iface_mode_is_v80
(
  boolean value
);

/*===========================================================================

FUNCTION DSWCSDV80_SET_SYN1

DESCRIPTION
  Sets the value of dswcsdv80_syn1

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsdv80_set_syn1
(
  uint8 value
);

#endif /* FEATURE_DATA_V80 */

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


#endif /* DSWCSDV80_H */
