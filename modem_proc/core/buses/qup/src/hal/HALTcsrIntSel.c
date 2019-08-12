/*
===========================================================================

FILE:   HALTcsrIntSel.c

DESCRIPTION:
    This file implements a Interrupt select hal.

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/qup/src/hal/HALTcsrIntSel.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/21/13 LK     Moved interrupt selection into its own file.

===========================================================================
        Copyright c 2013 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "QupLog.h"
#include "quptcsr_hwio.h"

/**
 * This function configures QUP interrupts in MSS by
 * enabling an TCSR register
 * 
 * @param[in]  pTcsrAddr TCR address.
 * @param[in]  bitMask - Bit to be set for a QUP.
 */
int32 
HAL_qupe_EnableTcsrInterrupt
(
 uint8 *pTcsrAddr,
 uint32 uBitMask,
 uint32 uBitShift
 )
{
	HWIO_PHSS_QUP_MSS_INT_SEL_n_OUTMI(
			pTcsrAddr, 0, uBitMask, (1 <<uBitShift));
	return 0;
}
