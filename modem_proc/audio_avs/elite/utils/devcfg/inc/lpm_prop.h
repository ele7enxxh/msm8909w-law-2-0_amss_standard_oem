/*========================================================================

*//** @file lpm_prop.h
This file contains structure definition for LPM hardware properties.

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: 

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/14/12   rkc     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "mmdefs.h"

#ifndef LPM_DEVCFG_H
#define LPM_DEVCFG_H

/** @brief LPM Properties Structure
*/

typedef struct  {
    uint64_t base_phy_addr; 
    /**< Starting physical address, 64-bit */
    
    uint32_t size; 
    /**< Size of LPM in bytes */
    
    uint32_t size_audio_bitstrm_buf; 
    /**< Size allocated for audio bitstream buffer, in bytes */
    
    uint32_t size_afe_dma_buf;
    /**< Size allocated for AFE DMA buffers, in bytes */
    
    uint32_t size_voice_buf;
    /**< Size allocated for voice buffer, in bytes */

    uint32_t size_lpa_heap_from_lpm; 
    /**< Size allocated for LPA heap from LPM memory */
	
    uint32_t size_lpa_heap_from_ddr; 
    /**< Size allocated for LPA heap from DDR */
} lpm_prop_struct_t;

#endif //LPM_DEVCFG_H


