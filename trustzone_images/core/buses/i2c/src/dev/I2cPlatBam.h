#ifndef I2CPLATBAM_H
#define I2CPLATBAM_H
/*=============================================================================

FILE:         I2cPlatBam.h

This file contains the function prototypes for SpiDevice
              core driver BAM functionality.


              Copyright (c) 2012 Qualcomm Technologies Incorporated.
              All Rights Reserved.
              QUALCOMM Confidential and  Proprietary
=============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/i2c/src/dev/I2cPlatBam.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cPlatSvc.h"



/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
/*BIT mask Status portion of the descriptors*/
#define PLAT_BAM_DESC_FLAG_INT  0x8000  /* Generate interrupt */
#define PLAT_BAM_DESC_FLAG_EOT  0x4000  /* Generate end-of-transfer indication */
#define PLAT_BAM_DESC_FLAG_EOB  0x2000  /* Generate end-of-block indication */
#define PLAT_BAM_DESC_FLAG_NWD  0x1000  /* Generate Notify when done indication */
#define PLAT_BAM_DESC_FLAG_CMD  0x0800  /* Mark as command descriptors*/


/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

typedef enum  I2CPLATBAM_EventType
{
   I2CPLATBAM_EVT_NULL,     
   I2CPLATBAM_EVT_TX_EOT, // TODO: change the names to TX events
   I2CPLATBAM_EVT_TX_DESC_DONE,
   //I2CPLATBAM_EVT_PIPE_EMPTY,
   I2CPLATBAM_EVT_RX_EOT,
   I2CPLATBAM_EVT_RX_DESC_DONE,

   I2CPLATBAM_EVT_BAM_ERROR,
} I2CPLATBAM_EventType;

typedef struct I2CPLATBAM_CallbackResultType
{
   I2CPLATBAM_EventType bamEvt;
   void                *pArg;
} I2CPLATBAM_CallbackResultType;

typedef enum  I2CPLATBAM_DirectionType
{
   I2CPLATBAM_DIR_INPUT,
   I2CPLATBAM_DIR_OUTPUT
} I2CPLATBAM_DirectionType;

typedef struct I2CPLATBAM_BamIoVecType
{
   I2CPLATBAM_DirectionType eDir;
   uint8                    *pBuff;      /**< Buffer virtual address */
   uint16                    buffSize;  /**< Buffer size in bytes */
   uint16                    flags;     /**< Flag bitmask (see bam_iovec_options_type) */
} I2CPLATBAM_BamIoVecType;

typedef void (*I2CDEVICE_PLATBAM_CB)(I2CPLATBAM_CallbackResultType);

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/**
 * @brief Reset the BAM hardware
 *
 * This function resets the BAM hardware
 * 
 *  @param[in] hPlatDev platform device handle.
 * 
 * @return I2C_RES_SUCCESS on sucess, error code on error 
 */
int32 I2CPLATDEV_ResetBAM(I2CPLATDEV_HANDLE hPlatDev);

/**
  @brief Schedule a single BAM transfer 
 
  This function schedules a single BAM transfer.
  
  @param[in]         hPlatDev platform device handle.
  @param[in] pBamIoVec   Pointer to bam io vector
  @param[out] pFreeCnt    Pointer to remaining free transfers.
  @param[out] pfnCb       Pointer to callback function
  @param[out] pArg        Pointer to arg to be passed back to callback function.
  
   @return I2C_RES_SUCCESS  on sucess, error code on error
 */
int32 I2CPLATDEV_PlatBamTransfer(I2CPLATDEV_HANDLE hPlatDev,  
			                        I2CPLATBAM_BamIoVecType *pBamIoVec,
                                 uint32                  *pFreeCnt,
                                 I2CDEVICE_PLATBAM_CB     pfnCb,
                                 void                    *pArg);


/**
   @brief Find Available free space in a BAM pipe

  @param[in]   hPlatDev platform device handle.
  @param uDir  Producer or Consumer pipe.
  
  @return Available free space for descriptors on the 
   platform pipe.
 */
uint32 I2CPLATDEV_BamGetPipeFreeSpace(I2CPLATDEV_HANDLE hPlatDev, I2CPLATBAM_DirectionType eDir);

#endif /* I2CPLATBAM_H */

