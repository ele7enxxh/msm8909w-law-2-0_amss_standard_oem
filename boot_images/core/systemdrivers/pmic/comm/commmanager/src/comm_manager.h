#ifndef _COMM_MANAGER_H_
#define _COMM_MANAGER_H_

/*! \file comm_manager.h
 *  \n
 *  \brief Comm Manager related structure type and function declarations
 *  \n  
 *  \n &copy; Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/comm/commmanager/src/comm_manager.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/01/11   APU     Adding IComm in the CommManager as an independendant 
                   entity, seperate from CommInfo.
12/08/10   wra     Re-writing CommManager to reduce memory foot print 
12/07/10   wra     RPM Optimization removed DecorateMandatoryComm() and 
                   SwapMandatoryCommInterface()
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "itest_comm.h"
#include "comm_type.h"
#include "comm_info.h"
#include "icomm.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

typedef struct
{
   boolean IsInitialized;
   pmiC_IComm** PmicCommArray;
   uint32 NumberOfDevices;
}pmiC_CommManager;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pmiC_CommManager_CreateSingleton(uint32 numberOfDevices);
void pmiC_CommManager_DeleteSingleton(void);
pmiC_CommManager* pmiC_CommManager_GetSingleton(void);

void pmiC_CommManager_ResetNumberComms(pmiC_CommManager  *me, uint32  numberOfDevices);
pmiC_IComm* pmiC_CommManager_GetComm(pmiC_CommManager  *me, uint32  deviceIndex, uint32  pCommSid);
uint32 pmiC_CommManager_GetNumberOfComms(pmiC_CommManager *me);
pm_err_flag_type pmiC_CommManager_AddCommInterface(pmiC_CommManager  *me, pmiC_CommInfo *commInfo);

#ifdef PMIC_OFFTARGET_TESTING
pmiC_ITestComm* pmiC_CommManager_GetTestComm(pmiC_CommManager  *me, pmiC_CommInfo* commInfo); 
#endif /* PMIC_OFFTARGET_TESTING */

#endif /* _COMM_MANAGER_H_ */
