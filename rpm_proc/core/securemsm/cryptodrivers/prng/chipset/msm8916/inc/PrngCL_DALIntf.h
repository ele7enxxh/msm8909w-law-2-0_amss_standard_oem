#ifndef __PRNGCL_DALINTF_H__
#define __PRNGCL_DALINTF_H__

/*===========================================================================

                       P R N G E n g i n e D r i v e r

                       H e a d e r  F i l e (e x t e r n a l)

DESCRIPTION
  This header file contains HW PRNG DAL interface specific declarations.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/securemsm/cryptodrivers/prng/chipset/msm8916/inc/PrngCL_DALIntf.h#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $ 

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2011-09-05   nk      Initial version
============================================================================*/
#include "comdef.h"
#include "PrngCL.h"

/**
 * @brief   This function enables the PRNG Engine Clock 
 *          The function uses DAL interface to enable clock.
 *
 * @param None
 *
 * @return Prng_Result_Type
 *
 * @see PrngCL_DAL_Clock_Disable
 */
PrngCL_Result_Type PrngCL_DAL_Clock_Enable( void );


/**
 * @brief   This function disables the PRNG Engine Clock 
 *          The function uses DAL interface to disable clock.
 *
 * @param None
 *
 * @return Prng_Result_Type
 *
 * @see PrngCL_DAL_Clock_Enable
 */
PrngCL_Result_Type PrngCL_DAL_Clock_Disable( void );

#endif /* __PRNGCL_DALINTF_H__ */
