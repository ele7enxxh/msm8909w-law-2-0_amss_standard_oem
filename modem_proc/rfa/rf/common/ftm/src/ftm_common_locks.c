/*!
  @file
  ftm_common_locks.c

  @brief
  This module contains implementation to acquire and release locks for critical
  section code execution
 
  @addtogroup RF_COMMON_FTM_LOCKS
  @{
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common_locks.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/09/11   sty     Added comdef.h
07/18/11   sty     Initial revision

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "ftm_common_locks.h"

#ifdef FEATURE_FACTORY_TESTMODE

/*============================================================================*/
/*!
  @name FTM Critical Section Implementation 

  @brief
  This section contains interfaces to initialize, acquire and release critical
  sections
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*! Data type to be used for critical section */
typedef rex_crit_sect_type ftm_crit_section_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Holds the critical section data needed by FTM module - this is a common  
  data structure to be used by the whole FTM module 
*/
static ftm_crit_section_data_type ftm_crit_section_data;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the data structure to enable use of critical section  
    
  @param ftm_cs_data  
  Pointer to the data structure that is to be initialized  
*/
void
ftm_common_init_critical_section
(
  void
)
{
  /* Init Critical Section */
  rex_init_crit_sect( &ftm_crit_section_data );

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Acquires a critical section and then proceeds execution  
    
  @param ftm_cs_data  
  Pointer to the data structure to be used when acquiring critical section 
*/
void
ftm_common_enter_critical_section
(
  void
)
{
  /* Enter Critical Section */
  rex_enter_crit_sect( &ftm_crit_section_data );

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Releases a critical section and then proceeds execution  
    
  @param ftm_cs_data  
  Pointer to the data structure to be used when releasing critical section 
*/
void
ftm_common_leave_critical_section
(
  void
)
{
  /* Enter Critical Section */
  rex_leave_crit_sect( &ftm_crit_section_data );

}

/*! \} */

#endif /*FEATURE_FACTORY_TESTMODE */

/*! @} */

