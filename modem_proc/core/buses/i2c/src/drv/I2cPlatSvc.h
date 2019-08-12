#ifndef _I2CPLATSVC_H_
#define _I2CPLATSVC_H_
/*=============================================================================

  @file   I2cPlatSvc.h

   This file contains the API for the platform services.
 
   Copyright (c) 2015-2016 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary

  ===========================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cPlatSvc.h#1 $ */
//PLEASE REMOCVE ME: Adding line to fool PW, to allow reverting a change
/*=========================================================================
  EDIT HISTORY FOR MODULE

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  03/15/16 VG     Added clock Init/De-Init APIs
  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cPlatSvcTypes.h"

#include "I2cTypes.h"
#include "I2cError.h"


/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
 
 int32
I2CPLATDEV_InitClocks
(
   I2cPlat_DescType *pPlat
);


int32
I2CPLATDEV_DeInitClocks
(
   I2cPlat_DescType *pPlat
);
/** @brief Releases data structures necessary to support the target calls.

    @param[in] pPlat platform device pointer.

    @return          int32 .
  */
int32
I2cPlat_DeInitTarget
(
   I2cPlat_DescType *pPlat
);


/** @brief Initializes data structures necessary to support the target calls.

    @param[in] pPlat  pointer to platform device structure.
    @param[in] pProps pointer to platform properties.

    @return          int32 .
  */
int32
I2cPlat_InitTarget
(
   I2cPlat_DescType      *pPlat ,
   I2cPlat_PropertyType  *pProps
);
/**
    This Function registers IST, ISR callback.

    @param[in]  hPlatDev     Platform handle.
    @param[in]  IstHook      IST function pointer.
    @param[in]  pIstData     pointer to IST callback data.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32
I2cPlat_RegisterIstHooks
(
   I2cPlat_DescType  *pDev,
   ISR_HOOK                IsrHook,
   void*                   pIsrData,
   IST_HOOK                IstHook,
   void*                   pIstData
);

/**
 This Function unregisters IST and ISR callbacks.
 
 @param[in]  hPlatDev     Platform handle.
 
 @return             I2C_RES_SUCCESS is successful, otherwise
 I2cPlat_Error.
 */
int32
I2cPlat_UnRegisterIstHooks
(
   I2cPlat_DescType  *pDev
);

/** @brief Resets resources like bam pipes following a qup core reset.

    @param[in] pPlat platform device pointer.

    @return          int32 .
  */
int32
I2cPlat_ResetTarget
(
   I2cPlat_DescType *pPlat
);

/** @brief Returns the QUP virtual address.

    @param[in]  pPlat platform device pointer.

    @return          int32 .
  */
uint8*
I2cPlat_GetQupAddress
(
   I2cPlat_DescType      *pPlat
);

/** @brief Configures and enables I2C gpios.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32
I2cPlat_EnableI2cGpios
(
   I2cPlat_DescType *pPlat
);

/** @brief disables I2C gpios.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32
I2cPlat_DisableI2cGpios
(
   I2cPlat_DescType *pPlat
);

/** @brief sets the vote for PNOC.

    @param[in] pDev     Pointer to platform device structure.
    @param[in] hClnt    Npa client handle.
    @param[in] uIb      PNOC IB value.
    @param[in] uAb      PNOC AB value.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLAT_Error.
  */
int32
I2cPlat_AddPnocVote
(
   I2cPlat_DescType *pDesc
);

/** @brief unvote for PNOC.


    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLAT_Error.
  */

int32
I2cPlat_RemovePnocVote
(
   I2cPlat_DescType *pDesc
);



/** @brief Enables the I2C clocks.

    @param[in]  pPlat platform device pointer.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32 I2cPlat_VoteI2cClkOn
(
   I2cPlat_DescType      *pPlat
);

/** @brief Disables the I2C clocks.

    @param[in]  pPlat platform device pointer.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32 I2cPlat_VoteI2cClkOff
(
   I2cPlat_DescType      *pPlat
);


/** @brief Reads the properties for this device.

    @param[in] pPlat platform device pointer.
    @param[in] pDevProp       Pointer to properties structure.

    @return          int32 .
  */
//int32 I2cPlat_ReadProperties
//(
//   I2cPlat_DescType        *pPlat,
//   I2cPlat_PropertyType         *pDevProp
//);

/** @brief Checks if BAM is enabled.

    @param[in] pPlat platform device pointer.
    @return    TRUE if BAM is enabled FALSE otherwise .
  */
//boolean
//I2cPlat_IsBamEnabled
//(
//   I2cPlat_DescType *pPlat
//);


#endif /* _I2CPLATSVC_H_ */

