#ifndef DALTLMM_H
#define DALTLMM_H
/*===========================================================================

                      I N T E R N A L   D A L   T L M M
                            H E A D E R   F I L E

DESCRIPTION
  This file contains definitions for structures used by the DALTLMM 

REFERENCES

===========================================================================
             Copyright (c) 2010 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/tlmm/src/DALTLMM.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/10   dcf     Restructuring for new targets.
===========================================================================*/


/*==========================================================================

                        INCLUDE FILES FOR MODULE

==========================================================================*/


#include <DALStdDef.h>
#include <DALStdErr.h>
#include <DALFramework.h>
#include <DALSys.h>
#include <DALDeviceId.h>
#include <DDITlmm.h>



/*==========================================================================

                          EXPORTED STRUCTURES 

==========================================================================*/


typedef struct TLMMDrvCtxt TLMMDrvCtxt;
typedef struct TLMMDevCtxt TLMMDevCtxt;
typedef struct TLMMClientCtxt TLMMClientCtxt;


/*
 * Declaring a private "TLMM" Vtable
 * The following defines the virtual table for use when external modules do a 
 * device attach to the TLMM module for the first time when driver init is called.
 * The driver init and driver de init function pointers reside here.
 */

typedef struct TLMMDALVtbl TLMMDALVtbl;

struct TLMMDALVtbl
{
	int (*TLMM_DriverInit)(TLMMDrvCtxt *);
	int (*TLMM_DriverDeInit)(TLMMDrvCtxt *);
};



/*
 * TLMMDevCtxt
 *
 * Data structure containing all information about an interrupt
 *
 *  dwRefs:               Reserved.
 *  DevId:                DAL tlmm device ID.
 *  dwDevCtxtRefIdx:      Number of clients using this device.
 *  pDrvCtxt:             Pointer to the driver context.
 *  Reserved:             Reserved.
 *  hTlmmSync:            TLMM Synchronization object.
 */
struct TLMMDevCtxt
{
    uint32                                 dwRefs;
    DALDEVICEID                            DevId;
    uint32                                 dwDevCtxtRefIdx; 
    TLMMDrvCtxt                            *pDrvCtxt;
    DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    uint32                                 Reserved[16];
    DALSYS_SYNC_OBJECT(hTlmmSyncObj);
    DALSYSSyncHandle                       hTlmmSync;
};


/*
 * TLMMDrvCtxt
 *
 * Data structure containing all information about an interrupt
 *
 *  TLMMDALVtbl:     Base Members.
 *  dwNumDev:        The number of devices in tlmm this is 1.
 *  dwSizeDevCtxt:   Driver structure size for DALSYS.
 *  bInit:           Flag to indicate the driver has been initialized.
 *  dwRefs:          The number of clients using the driver.
 *  TLMMDevCtxt:     The TLMM Device Context.
 */
struct TLMMDrvCtxt
{
    TLMMDALVtbl        TLMMDALVtbl;
    uint32             dwNumDev;
    uint32             dwSizeDevCtxt;
    uint32             bInit;
    uint32             dwRefs;
    TLMMDevCtxt        TLMMDevCtxt[1];
};


/*
 * TLMMClientCtxt
 *
 * Data structure containing top-level TLMM access structures base members.
 *
 *  dwRefs:          References used internally.
 *  dwAccessMode:    SHARED for TLMM.
 *  pPortCtxt:       Pointer to the remote port context.
 *  pDevCtxt:        Handle to the TLMM device context.
 *  DalTlmmHandle:   Handle to the TLMM configuration structures.
 */
struct TLMMClientCtxt
{
  uint32             dwRefs;                     
  uint32             dwAccessMode;
  void               *pPortCtxt;
  TLMMDevCtxt        *pDevCtxt;            
  DalTlmmHandle      DalTlmmHandle; 
};



/*==========================================================================

   MANDATORY DAL DEVICE APIS NOT CURRENTLY IMPLEMENTED ON THIS TARGET

==========================================================================*/

DALResult TLMM_DriverInit(TLMMDrvCtxt *);
DALResult TLMM_DriverDeInit(TLMMDrvCtxt *);
DALResult TLMM_DeviceInit(TLMMClientCtxt *);
DALResult TLMM_DeviceDeInit(TLMMClientCtxt *);
DALResult TLMM_Reset(TLMMClientCtxt *);
DALResult TLMM_PowerEvent(TLMMClientCtxt *, 
                          DalPowerCmd, 
                          DalPowerDomain);
DALResult TLMM_Open(TLMMClientCtxt *, 
                    uint32);
DALResult TLMM_Close(TLMMClientCtxt *);
DALResult TLMM_Info(TLMMClientCtxt *,
                    DalDeviceInfo *,
                    uint32);
DALResult TLMM_InheritObjects(TLMMClientCtxt *,
                              DALInheritSrcPram *,
                              DALInheritDestPram *);



/*==========================================================================

                     APPLICATION PROGRAMMING INTERFACE

==========================================================================*/

/*==========================================================================

  FUNCTION      TLMM_ConfigGpio

  DESCRIPTION   Configure a GPIO to active or low power state based on the
                enable parameter.

  PARAMETERS    pCtxt      - Pointer to the client context.
                eGpioConfig - The packed configuration to program.
                eEnable     - Specifies whether the GPIO should be configured
                              to active or low power.  Passing in 
                              DAL_TLMM_GPIO_ENABLE uses the 'eGpioConfig' 
                              configuration while passing in 
                              DAL_TLMM_GPIO_DISABLE uses low power 
                              configuration.

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_ConfigGpio
( 
  TLMMClientCtxt*   pCtxt,  
  DALGpioSignalType eGpioConfig,  
  DALGpioEnableType eEnable
);


/*==========================================================================

  FUNCTION      TLMM_ConfigGpioGroup

  DESCRIPTION   Configures a group of GPIO.

  PARAMETERS    pCtxt         - Pointer to the client context.
                eEnable       - See TLMM_ConfigGpio above. 
                eGpioConfigs  - Pointer to an array of GPIO configurations.
                nSize         - The size in bytes of the gpio_group array.
                                (each configuration is 4 bytes).

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_ConfigGpioGroup
( 
  TLMMClientCtxt*     pCtxt,
  DALGpioEnableType  eEnable,
  DALGpioSignalType* eGpioConfigs,
  uint32             nSize
);


/*==========================================================================

  FUNCTION      TLMM_GpioOut

  DESCRIPTION   Reads the input value (high or low) of a GPIO.

  PARAMETERS    pCtxt       - Pointer to the client context.
                eGpioConfig  - The configuration to write to.
                eValue       - The value to write to this configuration.

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_GpioOut
(
  TLMMClientCtxt*    pCtxt,
  DALGpioSignalType eGpioConfig,
  DALGpioValueType  eValue
);


/*==========================================================================

  FUNCTION      TLMM_SetPort

  DESCRIPTION   Modifies non-GPIO TLMM hardware.

  PARAMETERS    pCtxt        - Pointer to the client context.
                ePort        - A generic enumeration specifying a hardware
                               functionality to modify.
                nValue       - The value to write (1 or 0)

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult  TLMM_SetPort
( 
  TLMMClientCtxt*   pCtxt,   
  DALGpioPortType ePort,   
  uint32          nValue
);


/*==========================================================================

  FUNCTION      TLMM_SetInactiveConfig

  DESCRIPTION   Changes the inactive (low-power) configuration for a GPIO.
                This function can also be used to bypass configuring a GPIO 
                to a low-power setting.

  PARAMETERS    pCtxt        - Pointer to the client context.
                nGpioNumber  - The GPIO number to set the inactive config.
                nConfig      - The new configuration.

  DEPENDENCIES  None.

  RETURN VALUE  DAL_SUCCESS or DAL_ERROR..

  SIDE EFFECTS  None.

==========================================================================*/

DALResult TLMM_SetInactiveConfig
(
  TLMMClientCtxt*   pCtxt,
  uint32            nGpioNumber,
  DALGpioSignalType nConfig
);


DALResult TLMM_PostInit( TLMMClientCtxt *pCtxt );

#endif /*DALTLMM_H*/

