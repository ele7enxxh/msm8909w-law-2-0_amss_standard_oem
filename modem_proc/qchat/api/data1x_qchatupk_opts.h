#ifndef QCHATUPK_OPTS
#define QCHATUPK_OPTS
/*===========================================================================
                    QCHAT UNIVERSAL PORTING KIT 
                      OPTIMIZATION DEFINITIONS

DESCRIPTION
  This file contains the external declarations for managing QChat Universal 
  Porting Kit optimizations at the DATA layer.

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/qchat/api/data1x_qchatupk_opts.h#1 $ 
$DateTime: 2016/12/13 07:57:51 $ $Author: mplcsds1 $

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

/* Flag to control QChat MOD optimization in DATA layer*/
extern boolean qchatupk_data_mod_opt;         

/* Flag to control QChat MTD optimization in DATA layer*/
extern boolean qchatupk_data_mtd_opt;         

/* Flag to control QChat MCS optimization in DATA layer*/
extern boolean qchatupk_data_mcs_opt;          

/* Flag to control QChat Q1X optimization in DATA layer*/
extern boolean qchatupk_data_q1x_opt;       

/* Flag to control QChat ADF optimization in DATA layer*/
extern boolean qchatupk_data_adf_opt;


/*===========================================================================

FUNCTION QCHATUPK_DATA_MCS_ENABLED

DESCRIPTION
  This function retrieves the current state of MCS optimization for QChat
  in the DATA layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qchatupk_data_mcs_enabled(void);


/*===========================================================================

FUNCTION QCHATUPK_DATA_MOD_ENABLED

DESCRIPTION
  This function retrieves the current state of MOD optimization for QChat
  in the DATA layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qchatupk_data_mod_enabled(void);


/*===========================================================================

FUNCTION QCHATUPK_DATA_MTD_ENABLED

DESCRIPTION
  This function retrieves the current state of MTD optimization for QChat
  in the DATA layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qchatupk_data_mtd_enabled(void);


/*===========================================================================

FUNCTION QCHATUPK_DATA_Q1X_ENABLED

DESCRIPTION
  This function retrieves the current state of Q1X optimization for QChat
  in the DATA layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qchatupk_data_q1x_enabled(void);

/*===========================================================================

FUNCTION QCHATUPK_LU_ENABLED

DESCRIPTION
  This function retrieves if LU optimization is turned on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qchatupk_lu_enabled(void);

/*===========================================================================

FUNCTION QCHATUPK_DATA_ADF_ENABLED

DESCRIPTION
  This function retrieves the current state of ADF optimization for QChat
  in the DATA layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qchatupk_data_adf_enabled(void);

#endif /*QCHATUPK_OPTS*/

