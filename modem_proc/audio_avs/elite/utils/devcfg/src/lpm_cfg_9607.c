/*========================================================================

*//** @file lpm_cfg_9607.c
This file contains LPM configuration for MDM9607

Copyright (c) 2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header:

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/30/15   udayb  Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "lpm_prop.h"

/* LPM configuration for MDM9607 */
const lpm_prop_struct_t lpm_prop =
{
    0x7718000,     /**< Starting physical address, 64-bit */
    8*1024,        /**< Size of LPM in bytes */
    0,             /**< Size allocated for audio bit-stream buffer, in bytes */
    8*1024,        /**< Size allocated for AFE DMA buffers, in bytes */
    0,             /**< Size allocated for voice buffer, in bytes */
    0,             /**< Size allocated for LPA heap from LPM memory */
    0              /**< Size allocated for LPA heap from DDR */
};
