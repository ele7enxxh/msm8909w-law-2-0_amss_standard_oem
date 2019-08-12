#ifndef HDRQCHATUPK_OPTS_H
#define HDRQCHATUPK_OPTS_H
/*===========================================================================
                    QCHAT UNIVERSAL PORTING KIT 
                      OPTIMIZATION DEFINITIONS

DESCRIPTION
  This file contains the external declarations for managing QChat Universal 
  Porting Kit optimizations at the HDR layer.

Copyright (c) 2000-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

QUALCOMM does not grant to you in this Agreement any rights in the OEM 
QChat Porting Kit including the accompanying associated documentation 
(the Software) for any purpose whatsoever, including without limitation 
any rights to copy, use or distribute the Software. Your rights to use the 
Software shall be only as set forth in any separate license between you and 
QUALCOMM which grants to you a license in the Software, and such license  
shall be under the terms and for the limited purposes set forth in that  
agreement
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrqchatupk_opts.h#1 $ 
$DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when        who        what, where, why
--------    ---        -----------------------------------------------------
05/27/09    srf    Created initial version
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"

                    
/*===========================================================================
                    EXTERNAL DECLARATIONS 
===========================================================================*/

/* Flag to control QChat IMO optimization in HDR layer */
extern boolean hdrqchatupk_imo_opt;

/* Flag to control QChat MOD optimization in HDR layer*/
extern boolean hdrqchatupk_mod_opt;         

/* Flag to control QChat MTD optimization in HDR layer*/
extern boolean hdrqchatupk_mtd_opt;         

/* Flag to control QChat OFTCS optimization in HDR layer*/
extern boolean hdrqchatupk_oftcs_opt;      

/* Flag to control QChat TFTCS optimization in HDR layer*/
extern boolean hdrqchatupk_tftcs_opt;      

/* Flag to control QChat MCS optimization in HDR layer*/
extern boolean hdrqchatupk_mcs_opt;          
   

/*===========================================================================

FUNCTION HDRQCHATUPK_IMO_ENABLED

DESCRIPTION
  This function retrieves the current state of IMO optimization for QChat
  in the HDR layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrqchatupk_imo_enabled(void);


/*===========================================================================

FUNCTION HDRQCHATUPK_MCS_ENABLED

DESCRIPTION
  This function retrieves the current state of MCS optimization for QChat
  in the HDR layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrqchatupk_mcs_enabled(void);


/*===========================================================================

FUNCTION HDRQCHATUPK_MOD_ENABLED

DESCRIPTION
  This function retrieves the current state of MOD optimization for QChat
  in the HDR layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrqchatupk_mod_enabled(void);


/*===========================================================================

FUNCTION HDRQCHATUPK_OFTCS_ENABLED

DESCRIPTION
  This function retrieves the current state of OFTCS optimization for QChat
  in the HDR layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrqchatupk_oftcs_enabled(void);


/*===========================================================================

FUNCTION HDRQCHATUPK_TFTCS_ENABLED

DESCRIPTION
  This function retrieves the current state of TFTCS optimization for QChat
  in the HDR layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrqchatupk_tftcs_enabled(void);


/*===========================================================================

FUNCTION HDRQCHATUPK_MTD_ENABLED

DESCRIPTION
  This function retrieves the current state of MTD optimization for QChat
  in the HDR layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrqchatupk_mtd_enabled(void);


#endif /*HDRQCHATUPK_OPTS_H*/


