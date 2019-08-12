#ifndef __MCALWCDMA_WENC_FTM_H__
#define __MCALWCDMA_WENC_FTM_H__
/*============================================================================*/
/** @file
 * This module has declarations related to Factory test mode support for
 * Enhanced Uplink.
 */
/*============================================================================*/

/*============================================================================
                        M C A L W C D M A _ W E N C _ F T M. H
DESCRIPTION
  This module has definition and declaration related to MCAL (modem core
  abstraction layer) W Encoder APIs utilized by RF in Factory test mode.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_wenc_ftm.h#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
11/10/14  tsk     Mainline code under feature FEATURE_WCDMA_JOLOKIA_MODEM on Jolokia branch.
07/28/14  pkg     Code Check-in for 16QAM feature.
06/12/14  tsk     Added Jolokia featurization for FTM related changes
06/08/14  ar      File created 
*/ 

/*=========================================================================*/
/*                         External Variable Declarations                  */
/*=========================================================================*/
extern const uint32 mcalwcdma_wenc_scr_type_shft[2];
extern const uint16 mcalwcdma_wenc_endianness_cntrl;

/*=========================================================================*/
/*                External Functions Declarations                          */
/*=========================================================================*/
void mcalwcdma_wenc_ftm_write_async_mod_parms(void);

#endif //__MCALWCDMA_WENC_FTM_H__