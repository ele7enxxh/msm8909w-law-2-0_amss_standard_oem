#ifndef __DDIMPM_H__
#define __DDIMPM_H__
/*==============================================================================
  FILE:         DDImpm.h
  
  OVERVIEW:     This file contains the DAL device interface (DDI) declarations
                for the vMPM Device Driver
 
  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/inc/dal/DDImpm.h#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 
#include "DalDevice.h"
#include "vmpm_types.h"
#define DALMPM_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/*===========================================================================
 *                      FUNCTION DECLARATIONS                             
 *==========================================================================*/
typedef struct Dalmpm Dalmpm;
struct Dalmpm
{
  struct DalDevice DalDevice;
  DALResult (*ConfigPin)(DalDeviceHandle    *_h,
                         uint32              mpmPinNum,
                         mpm_pin_config_type *pinConfigInfo);

  DALResult (*ConfigAndEnablePin)(DalDeviceHandle     *_h,
                                  uint32              mpmPinNum,
                                  mpm_pin_config_type *pinConfigInfo);

  DALResult (*EnablePin)(DalDeviceHandle     *_h,
                         uint32              mpmPinNum);
   
  DALResult (*DisablePin)(DalDeviceHandle     *_h,
                          uint32              mpmPinNum);
   
  DALResult (*GetNumMappedIrq)(DalDeviceHandle *_h, uint32 *pnum);

  DALResult (*GetNumMappedGpio)(DalDeviceHandle *_h, uint32 *pnum);
   
  DALResult (*MapInterrupts)(DalDeviceHandle         *_h,
                             mpm_config_info_type    *irqArray,
                             uint32                   irqCount,
                             uint32                  *irqsMapped);
   
  DALResult (*SetupInterrupts)(DalDeviceHandle         *_h,
                               mpm_config_info_type    *irqArray,
                               uint32                  irqCount);

  DALResult (*QueryPinMapping)(DalDeviceHandle *_h, 
                               uint32          localPinNum, 
                               mpm_pin_type    pinType, 
                               uint32          *pMpmPinNum);

  DALResult (*QueryPinConfigInfo)(DalDeviceHandle *_h, 
                                  uint32         mpmPinNum, 
                                  mpm_pin_query_info_type *pMpmPinInfo);

  DALResult (*AddIrqPinToMappingTbl)(DalDeviceHandle *_h, 
                                     uint32         mpmPinNum, 
                                     mpm_int_pin_mapping_type *pNewMapTblEntryInfo);
};

typedef struct DalmpmHandle DalmpmHandle; 
struct DalmpmHandle 
{
  uint32 dwDalHandleId;
  const Dalmpm * pVtbl;
  void * pClientCtxt;
};

#define Dalmpm_StringDeviceAttach(DevName,hDalDevice)\
  DAL_StringDeviceAttachEx(NULL,DevName,DALMPM_INTERFACE_VERSION,hDalDevice)

/**
 * Dalmpm_ConfigPin 
 *  
 * @brief Configure the trigger type of an MPM interrupt for
 *        both IRQ & GPIO pins 
 *
 * Since fixed-function interrupts generally have only a single configuration
 * that makes sense, this driver populates each one with a default setting at
 * init time.  In the unlikely event you want to override the default you can
 * call this function--but it is not expected to be used in general.
 *
 * @param _h           Handle to MPM DAL device.
 * @param mpmPinNum    The pin number to configure.
 * @param config_pin_info* Pointer to pin config info (pin type & trigger type)
 *
 * @return   
 *  DAL_SUCCESS : Interrupt was configured successfully.
 *  DAL_ERROR   : If invalid parameters are passed
 *
 * @note If it is only a vaild wake up and is of interrupt type then configures 
 * otherwise does nothing and returns.
 *       
 * @see Dalmpm_ConfigAndEnablePin
 */
static __inline DALResult Dalmpm_ConfigPin(DalDeviceHandle     *_h,
                                           uint32               mpmPinNum,
                                           mpm_pin_config_type *pinConfigInfo)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->ConfigPin( _h, mpmPinNum, pinConfigInfo);
  }
  else
  {
    return DAL_ERROR;
  }
}

/**
 * Dalmpm_ConfigAndEnablePin 
 *  
 * @brief Configure the trigger type and Enable an MPM-routed interrupt
 *        as a wakeup source for both IRQ & GPIO pins during deep sleep.
 *
 * If the interrupt occurs during deep sleep, the MPM will wake the system and
 * this driver will soft-trigger the interrupt into the local interrupt
 * controller (that is, you do not have to handle the MPM interrupt seperate
 * from the interrupt's regular incarnation).
 *
 * @param _h         Handle to MPM DAL device.
 * @param mpmPinNum  The pin number to configure.
 * @param config_pin_info* Pointer to pin config info (pin type & trigger type)
 *
 * @note If it is a vaild wake up interrupt and is of interrupt type then configures 
 * as wakeup source otherwsie does nothing and returns.
 *
 * @return   
 *  DAL_SUCCESS : Interrupt was configured as wakeup source successfully.
 *  DAL_ERROR   : If invalid parameters are passed
 */
static __inline DALResult Dalmpm_ConfigAndEnablePin(DalDeviceHandle *_h,
                                                    uint32 mpmPinNum,
                                                    mpm_pin_config_type *pinConfigInfo)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->ConfigAndEnablePin( _h, mpmPinNum, pinConfigInfo);
  }
  else
  {
    return DAL_ERROR;
  }
}

/**
 * Dalmpm_EnablePin 
 *  
 * @brief Enable a MPM pin as a wakeup source.
 *
 * Enable pin as a wakeup source during deep sleep.
 *
 * @param _h         Handle to MPM DAL device.
 * @param mpmPinNum  The number of the GPIO to IRQ 
 *
 * @note If the pin is not supported by the MPM hardware this function does
 *       nothing and returns.
 *
 * @return   
 *  DAL_SUCCESS : Interrupt is configured as wakeup source successfully.
 *  DAL_ERROR   : If invalid parameters are passed
 */
static __inline DALResult Dalmpm_EnablePin(DalDeviceHandle *_h,
                                           uint32          mpmPinNum)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->EnablePin( _h, mpmPinNum );
  }
  else
  {
    return DAL_ERROR;
  }
}

/**
 * Dalmpm_DisablePin 
 *  
 * @brief Disable an MPM-routed interrupt as a wakeup source during deep sleep.
 *
 * @brief Disable a MPM pin as a wakeup source.
 *
 * Disable pin as a wakeup source during deep sleep.
 *
 * @note When an interrupt is disabled in the MPM, that interrupt may be
 *       completely discarded during deep sleep (not even latched for later
 *       processing).  However, it can still cause a wakeup from "shallower"
 *       forms of sleep (like simple halt, etc.).
 *
 * @param _h         Handle to MPM DAL device.
 * @param mpmPinNum  The number of the GPIO to IRQ 
 *
 * @return   
 *  DAL_SUCCESS : Interrupt was desabled as a wakeup source successfully.
 *  DAL_ERROR   : If invalid parameters are passed
 */
static __inline DALResult Dalmpm_DisablePin(DalDeviceHandle *_h,
                                            uint32          mpmPinNum)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->DisablePin( _h, mpmPinNum);
  }
  else
  {
    return DAL_ERROR;
  }
}

/**
 * Dalmpm_GetNumMappedIrq 
 *  
 * @brief Gives the number of interrupts (IRQs) that are mapped to mpm for
 *        a master it's running (i.e. modem, apps, q6).
 *
 * @param _h    Handle to MPM DAL device.
 * @param pnum  Number of interrupts mapped.
 *
 * @return   
 *  DAL_SUCCESS : If successful.
 *  DAL_ERROR   : If invalid parameters are passed
 */
static __inline DALResult Dalmpm_GetNumMappedIrq(DalDeviceHandle *_h,
                                                 uint32          *pnum)
{
  if(_h != NULL && pnum != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->GetNumMappedIrq( _h, pnum);
  }
  else
  {
    return DAL_ERROR;
  }
}

/**
 * Dalmpm_GetNumMappedGpio 
 *  
 * @brief Gives the number of interrupts (GPIOs) that are mapped to mpm for
 *        a master it's running (i.e. modem, apps, q6).
 *
 * @param _h    Handle to MPM DAL device.
 * @param pnum  Number of interrupts mapped.
 *
 * @return   
 *  DAL_SUCCESS : If successful.
 *  DAL_ERROR   : If invalid parameters are passed
 */
static __inline DALResult Dalmpm_GetNumMappedGpio(DalDeviceHandle *_h,
                                                  uint32          *pnum)
{
  if(_h != NULL && pnum != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->GetNumMappedGpio( _h, pnum);
  }
  else
  {
    return DAL_ERROR;
  }
}

/**
 * Dalmpm_MapInterrupts 
 *  
 * @brief Fills in the input arrays with the mapping of mpm interrupt id
 *        and corresponding master interrupt id.
 *
 * It should be noted that the first two fields of mpm_config_info_type
 * must not be changed by caller or it can result into configuring other
 * interrupts which are not meant to be.
 *
 * @param _h          Handle to MPM DAL device.
 * @param irqArray    Array in which above mapping will be filled in.
 * @param irqCount    Size of the array.
 * @param irqsMapped  Number of mappings actually filled in (should be
 *                    same as irqCount in normal scenario).
 *
 * @return   
 *  DAL_SUCCESS : If interrupts are mapped successfully.
 *  DAL_ERROR   : If invalid parameters are passed
 */
static __inline DALResult Dalmpm_MapInterrupts(DalDeviceHandle         *_h,
                                               mpm_config_info_type    *irqArray,
                                               uint32                   irqCount,
                                               uint32                  *irqsMapped)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->MapInterrupts( _h, irqArray, irqCount, irqsMapped);
  }
  else
  {
    return DAL_ERROR;
  }
}

/**
 * Dalmpm_SetupInterrupts 
 *  
 * @brief Sets up various fields of more than one interrupts at MPM level
 *        like trigger type and status.
 *
 * @note  As of now this function is intended to use for interrupts only 
 *        (no GPIO). 
 *
 * @param _h        Handle to MPM DAL device.
 * @param irqArray  Input array containing various interrupt information 
 * @param irqCount  Number of interrupts in the above array.
 *
 *
 * @return   
 *  DAL_SUCCESS : If successful in setting up interrupts fields like trigger
 *                type and status.
 *  DAL_ERROR   : If invalid parameters are passed.
 */
static __inline DALResult Dalmpm_SetupInterrupts(DalDeviceHandle         *_h,
                                                 mpm_config_info_type    *irqArray,
                                                 uint32                  irqCount)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->SetupInterrupts( _h, irqArray, irqCount);
  }
  else
  {
    return DAL_ERROR;
  }
}

/**
 * Dalmpm_QueryPinMapping 
 * 
 * @brief Determine the MPM Pin Number from the target config mapping table
 *        using the IRQ or GPIO pin type and Pin ID number as inputs. 
 *
 * @param _h          Handle to MPM DAL device.
 * @param localPinNum Input parameter: IRQ or GPIO ID number
 * @param pinType     Input parameter: MPM_IRQ or MPM_GPIO pin type
 * @param pMpmPinNum  Output: Address to MPM Pin Number corresponding to input
 *                    Local Pin Number
 *
 *
 * @return   
 *  DAL_SUCCESS : If successful in setting up interrupts fields like trigger
 *                type and status.
 *  DAL_ERROR   : If invalid parameters are passed.
 * 
 */
static __inline DALResult Dalmpm_QueryPinMapping( DalDeviceHandle *_h, 
                                                  uint32          localPinNum, 
                                                  mpm_pin_type    pinType, 
                                                  uint32          *pMpmPinNum)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->QueryPinMapping( _h,
                                                         localPinNum, 
                                                         pinType, 
                                                         pMpmPinNum);
  }
  else
  {
    return DAL_ERROR;
  }
}

/**
 * Dalmpm_QueryPinConfigInfo 
 *  
 * @brief Determine the MPM Pin Number from the target config mapping table
 *        using the IRQ or GPIO pin type and Pin ID number as inputs. 
 *
 * @note  As of now this function is intended to use for interrupts only 
 *        (no GPIO). 
 *
 * @param _h        Handle to MPM DAL device.
 * @param mpmPinNum Input: MPM Pin Number (index into targe config
 *                         mapping table) 
 * @param pMpmPinInfo
 *                  Output: Address to data structure containing the pin info
 *                          including the following:
 *                          - pin type (MPM_IRQ or MPM_GPIO)
 *                          - pin trigger type 
 *
 * @return   
 *  DAL_SUCCESS : If successful in setting up interrupts fields like trigger
 *                type and status.
 *  DAL_ERROR   : If invalid parameters are passed.
 * 
 */
static __inline DALResult Dalmpm_QueryPinConfigInfo( DalDeviceHandle *_h, 
                                                     uint32         mpmPinNum, 
                                                     mpm_pin_query_info_type *pMpmPinInfo )
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->QueryPinConfigInfo( _h,
                                                            mpmPinNum, 
                                                            pMpmPinInfo);
  }
  else
  {
    return DAL_ERROR;
  }
}

/**
 * Dalmpm_AddIrqPinToMappingTbl 
 *  
 * @brief Add configuration of a IRQ pin for clients.
 *
 * @note  This function is intended to use for interrupts only 
 *        (no GPIO). 
 *
 * @param _h        Handle to MPM DAL device.
 * @param mpmPinNum Input: MPM Pin Number (index into targe config
 *                         mapping table) 
 * @param pNewMapTblEntryInfo
 *                  Output: Address to data structure containing the pin info
 *                          including the following:
 *                          - pin type (MPM_IRQ or MPM_GPIO)
 *                          - pin trigger type 
 *
 * @return   
 *  DAL_SUCCESS : If successful in setting up interrupts fields like trigger
 *                type and status.
 *  DAL_ERROR   : If invalid parameters are passed.
 * 
 */
static __inline DALResult Dalmpm_AddIrqPinToMappingTbl( 
   DalDeviceHandle *_h, 
   uint32         mpmPinNum, 
   mpm_int_pin_mapping_type *pNewMapTblEntryInfo )
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->AddIrqPinToMappingTbl( 
                   _h,
                  mpmPinNum, 
                  pNewMapTblEntryInfo);
  }
  else
  {
    return DAL_ERROR;
  }
}
#endif
