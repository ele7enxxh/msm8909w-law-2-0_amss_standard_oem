#ifndef WHL1MCALIF_H
#define WHL1MCALIF_H

/*============================================================================*/
/** @file  
 * This module has the interfaces used by both MCAL WCDMA and WHL1
 * modules
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/whl1mcalif.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
07/16/09    hk      Reorganized the code to remove INC/SRC violations

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "mcalwcdma_demod.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
//#define DL_MAX_NUM_CELLS MCALWCDMA_MAX_NUM_CELLS
//#define MSMHW_MAX_NUM_FINGERS MCALWCDMA_MAX_NUM_FINGERS
//#define DL_MAX_NUM_FINGERS MCALWCDMA_MAX_NUM_FINGERS 


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/** MCAL DEMOD finger configuration type */
typedef mcalwcdma_demod_fing_config_struct_type dl_tri_fing_info_struct_type;

/** MCAL DEMOD cell configuration type */
typedef mcalwcdma_demod_cell_config_struct_type dl_cell_config_info_struct_type;

/** MCAL DEMOD status dump type */
typedef mcalwcdma_demod_status_dump_info_struct_type dl_fing_status_dump_struct_type;

typedef mcalwcdma_demod_status_dump_info_struct_type demod_status_dump_struct_type;

/* Call back function type declaration */
/* ----------------------------------- */

/* Enum type declaration */
/* --------------------- */

/* Structure type declaration */
/* -------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*============================================================================*/
/**
 *
 */
/*============================================================================*/

#endif /* WHL1MCALIF_H */

