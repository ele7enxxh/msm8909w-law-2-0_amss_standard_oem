/*! \file comm_manager.c 
*  \n
*  \brief Comm Manager related function definitions
*  \n  
*  \n &copy; Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/comm/commmanager/src/comm_manager.c#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
01/15/13   aab      Fixed KW errors 
01/04/12   mpt      Added Bat board support to GetTestComm()
01/06/10   wra      Removed MSM8660 Target Namespace
12/08/10   wra      Re-writing CommManager to reduce memory foot print 
12/07/10   wra      RPM Optimization removed DecorateMandatoryComm() and 
SwapMandatoryCommInterface() and cleaned up some comments
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "comm_manager.h"
#include "pm_processor.h"
#include "pm_utils.h" // pm_malloc

static pmiC_CommManager m_CommManager;

static pm_err_flag_type pmiC_CommManager_CreateModuleComm(pmiC_CommManager *me, pmiC_CommInfo* commInfo, pmiC_IComm** pCurrentComm, pmiC_IComm** pNewComm);
static void pmiC_CommManager_Init(pmiC_CommManager *me, uint32 numberOfDevices);

/*=========================================================================
      Function Definitions
==========================================================================*/
void pmiC_CommManager_CreateSingleton(uint32 numberOfDevices)
{
    if(FALSE == m_CommManager.IsInitialized)
    {
        if( numberOfDevices > 0 )
        {
            pmiC_CommManager_Init(&m_CommManager, numberOfDevices);
        }
        else
        {
            pmiC_CommManager_Init(&m_CommManager, 1);
        }
    }
}

void pmiC_CommManager_DeleteSingleton()
{
    if(TRUE == m_CommManager.IsInitialized)
    {
        m_CommManager.IsInitialized = FALSE;
        m_CommManager.PmicCommArray = NULL;
        m_CommManager.NumberOfDevices = 0;
    }
}

pmiC_CommManager* pmiC_CommManager_GetSingleton()
{
    return &m_CommManager;
}

void pmiC_CommManager_Init(pmiC_CommManager *me, uint32 numberOfDevices)
{
    me->IsInitialized = FALSE;
    me->NumberOfDevices = numberOfDevices;
    me->PmicCommArray = NULL;
    pmiC_CommManager_ResetNumberComms(me, numberOfDevices);
}

void pmiC_CommManager_ResetNumberComms(pmiC_CommManager *me, uint32 numberOfDevices)
{
    int i = 0;

    const int number_of_allocs = (me->NumberOfDevices * 2) + 1; 
    if(me->PmicCommArray != NULL)
    {
        for(i = 0; i < number_of_allocs; i++)
        {
            (void)DALSYS_Free(me->PmicCommArray[i]);
        }
        (void)DALSYS_Free(me->PmicCommArray);
    }

    me->NumberOfDevices = numberOfDevices;

    if( numberOfDevices > 0 )
    {
        // Create and initialize memory.
        pm_malloc(number_of_allocs*sizeof(pmiC_IComm*), (void**)&(me->PmicCommArray) );

            for(i = 0; i < number_of_allocs; i++)
            {
                me->PmicCommArray[i] = NULL;
            }
        }
    }
pmiC_IComm* pmiC_CommManager_GetComm(pmiC_CommManager *me, uint32 deviceIndex, uint32 pCommSid)
{
    // Each device has 2 internal SID (0 & 1)
    uint32 commIndex = (deviceIndex * 2) + (pCommSid % 2);
    return me->PmicCommArray[commIndex];
}

uint32 pmiC_CommManager_GetNumberOfComms(pmiC_CommManager *me)
{
    return me->NumberOfDevices * 2;
}

pm_err_flag_type pmiC_CommManager_AddCommInterface(pmiC_CommManager  *me, pmiC_CommInfo *commInfo)
{
    pm_err_flag_type err = PM_ERR_FLAG__SUCCESS; 
    pmiC_CommInfo  *temp_CommInfo = NULL;

    uint32 commIndex = (2 * commInfo->mDeviceInfo->mDeviceIndex) + (commInfo->mBusIndex % 2);
    pmiC_CommManager_CreateModuleComm( me, commInfo, NULL, &(me->PmicCommArray[commIndex]) ) ;       

    if ( NULL != me->PmicCommArray[commIndex] )
    {                
        /* Use the copy constructor and populate the IComm array */
        pm_malloc(sizeof(pmiC_CommInfo), (void**)&temp_CommInfo);
            pmiC_CommInfo_InitCopy(temp_CommInfo, commInfo);
            me->PmicCommArray[commIndex]->mpCommInfo = temp_CommInfo ;
        }
    return err;
}

pm_err_flag_type pmiC_CommManager_CreateModuleComm(pmiC_CommManager  *me, pmiC_CommInfo* commInfo, pmiC_IComm** pCurrentComm, pmiC_IComm** pNewComm)
{
    pm_err_flag_type errPowerup = PM_ERR_FLAG__SUCCESS ;

     // Check the repository for this commType
     uint32 commIndex = (2 * commInfo->mDeviceInfo->mDeviceIndex) + (commInfo->mBusIndex % 2);

     if(me->PmicCommArray[commIndex] != NULL)
     {
        *pNewComm = me->PmicCommArray[commIndex];
     }
     else
     {
        errPowerup = pm_processor_create_comm_channel(commInfo, pCurrentComm, pNewComm);

        if(*pNewComm != NULL)
        {
           me->PmicCommArray[commIndex] = *pNewComm;
        }
     }

     return errPowerup;
}

#ifdef PMIC_OFFTARGET_TESTING
pmiC_ITestComm* pmiC_CommManager_GetTestComm(pmiC_CommManager  *me, pmiC_CommInfo* commInfo)
{
     // Check that the commType that is being requested is an ITestComm object
     CommType communicationType = commInfo->mCommType ;
     uint32 commIndex = 0;

      switch ( communicationType )
     {
     case ShadowRegisterOnlyType :
     case BatBoardBaseType:
        {
            commIndex = (2*commInfo->mDeviceInfo->mDeviceIndex) + (commInfo->mBusIndex % 2);
            return (pmiC_ITestComm*)(me->PmicCommArray[commIndex]);
        }
     }
     return NULL;
}
#endif
