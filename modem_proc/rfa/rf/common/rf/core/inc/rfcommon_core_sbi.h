#ifndef RFCOMMON_CORE_SBI_H
#define RFCOMMON_CORE_SBI_H

/*!
  @file
  Definitions of types pertaining to SBI.

  @details
  This file exports the definitions of types that are used in the RF Driver
  relating to SBI.
*/

/*==============================================================================

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

==============================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_core_sbi.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------- 
01/20/10   aro     Added SSBI_BUS_MAX
09/02/09   aro     Initial Release

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  A listing of all SSBI lines available.
*/
typedef enum
{
  SSBI_BUS_0 = 0, /*!< SBI Line 0 */
  SSBI_BUS_1 = 1, /*!< SBI Line 1 */
  SSBI_BUS_2 = 2, /*!< SBI Line 2 */
  SSBI_BUS_3 = 3, /*!< SBI Line 3 */
  SSBI_BUS_4 = 4, /*!< SBI Line 4 */
  SSBI_BUS_5 = 5, /*!< SBI Line 5 */
  SSBI_BUS_6 = 6,  /*!< SBI Line 6 */
  SSBI_BUS_MAX /*!< Max SBI Line*/
} rfcommon_sbi_num_enum_type;

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_CORE_SBI_H */



