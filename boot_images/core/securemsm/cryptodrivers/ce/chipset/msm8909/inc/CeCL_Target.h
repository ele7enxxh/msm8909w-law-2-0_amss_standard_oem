/*===========================================================================

                    Crypto Engine Core API

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE
 
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/securemsm/cryptodrivers/ce/chipset/msm8909/inc/CeCL_Target.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/31/12   nk      Boot version
03/09/12   yk      Modification for Crypto5
04/29/10   bm      initial version
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "msmhwio.h"
#include "ce0_msmhwioreg.h"

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/

/* ########################################################################
 * ###################     APIs Provided to CeEL       ###################
 * ########################################################################
 * Shall include the following definitions and provide APIs to get values */
/* 
 *  
 *  ==== Interrupt Names ====
 *      for example: #define SEC_CRYPTO_INTR1    CRYPTO0_CRYPTO_INTR1
 *  
 *  Please note that the common names will be used by the environment library
 *  the target core library will map various names above to a specific target
 *  and a crypto core
 */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define CECL_CE_VERSION                           CRYPTO0_CRYPTO_VERSION
#define CECL_CE_STATUS_SW_ERR                     HWIO_CRYPTO0_CRYPTO_STATUS_SW_ERR_BMSK 
#define CECL_CE_STATUS_ERR_INTR                   HWIO_CRYPTO0_CRYPTO_STATUS_ERR_INTR_BMSK 
#define CECL_CE_CONFIG                            CRYPTO0_CRYPTO_CONFIG
#define CECL_CE_STATUS                            CRYPTO0_CRYPTO_STATUS
#define CECL_CE_ENGINES_AVAIL                     CRYPTO0_CRYPTO_ENGINES_AVAIL
#define CECL_CE_DATA_INn                          CRYPTO0_CRYPTO_DATA_INn
#define CECL_CE_DATA_OUTn                         CRYPTO0_CRYPTO_DATA_OUTn
#define CECL_CE_AUTH_IVn                          CRYPTO0_CRYPTO_AUTH_IVn
#define CECL_CE_AUTH_KEYn                         CRYPTO0_CRYPTO_AUTH_KEYn
#define CECL_CE_AUTH_BYTECNT0                     CRYPTO0_CRYPTO_AUTH_BYTECNT0
#define CECL_CE_AUTH_BYTECNT1                     CRYPTO0_CRYPTO_AUTH_BYTECNT1
#define CECL_CE_AUTH_SEG_CFG                      CRYPTO0_CRYPTO_AUTH_SEG_CFG
#define CECL_CE_GOPROC                            CRYPTO0_CRYPTO_GOPROC
#define CECL_CE_GOPROC_QC_KEY                     CRYPTO0_CRYPTO_GOPROC_QC_KEY
#define CECL_CE_GOPROC_OEM_KEY                    CRYPTO0_CRYPTO_GOPROC_OEM_KEY
#define CECL_CE_ENCR_SEG_CFG                      CRYPTO0_CRYPTO_ENCR_SEG_CFG
#define CECL_CE_SEG_SIZE                          CRYPTO0_CRYPTO_SEG_SIZE
#define CECL_CE_ENCR_CNTR0_IV0                    CRYPTO0_CRYPTO_ENCR_CNTR0_IV0
#define CECL_CE_ENCR_CNTR1_IV1                    CRYPTO0_CRYPTO_ENCR_CNTR1_IV1
#define CECL_CE_ENCR_CNTR2_IV2                    CRYPTO0_CRYPTO_ENCR_CNTR2_IV2
#define CECL_CE_ENCR_CNTR3_IV3                    CRYPTO0_CRYPTO_ENCR_CNTR3_IV3
#define CECL_CE_ENCR_CNTR_MASK                    CRYPTO0_CRYPTO_ENCR_CNTR_MASK
#define CECL_CE_ENCR_KEYn                         CRYPTO0_CRYPTO_ENCR_KEYn
#define CECL_CE_GOPROC_GO_BMSK                    HWIO_CRYPTO0_CRYPTO_GOPROC_GO_BMSK
#define CECL_CE_AUTH_SEG_CFG_AUTH_ALG_SHFT        HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_ALG_SHFT
#define CECL_CE_AUTH_SEG_CFG_AUTH_SIZE_SHFT       HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_AUTH_SIZE_SHFT
#define CECL_CE_AUTH_SEG_SIZE_AUTH_SIZE_SHFT      HWIO_CRYPTO0_CRYPTO_AUTH_SEG_SIZE_AUTH_SIZE_SHFT
#define CECL_CE_AUTH_SEG_SIZE                     CRYPTO0_CRYPTO_AUTH_SEG_SIZE
#define CECL_CE_AUTH_SEG_START                    CRYPTO0_CRYPTO_AUTH_SEG_START
   
#define CECL_CE_ENCR_SEG_CFG_ENCODE_SHFT          HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCODE_SHFT
#define CECL_CE_ENCR_SEG_CFG_ENCR_MODE_SHFT       HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_MODE_SHFT
#define CECL_CE_ENCR_SEG_CFG_ENCR_ALG_SHFT        HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_ALG_SHFT
#define CECL_CE_ENCR_SEG_CFG_ENCR_KEY_SZ_SHFT     HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_KEY_SZ_SHFT
#define CECL_CE_ENCR_SEG_CFG_ENCR_MODE_BMSK       HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_MODE_BMSK
#define CECL_CE_ENCR_SEG_CFG_ENCR_SIZE_SHFT       HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_ENCR_SIZE_SHFT
#define CECL_CE_ENCR_SEG_SIZE_ENCR_SIZE_SHFT      HWIO_CRYPTO0_CRYPTO_ENCR_SEG_SIZE_ENCR_SIZE_SHFT
#define CECL_CE_ENCR_SEG_SIZE                     CRYPTO0_CRYPTO_ENCR_SEG_SIZE
#define CECL_CE_ENCR_SEG_START                    CRYPTO0_CRYPTO_ENCR_SEG_START
   
#define CECL_CE_STATUS_CRYPTO_STATE_BMSK          HWIO_CRYPTO0_CRYPTO_STATUS_CRYPTO_STATE_BMSK
#define CECL_CE_SEG_CFG_CLR_CNTXT_SHFT            HWIO_CRYPTO0_CRYPTO_SEG_CFG_CLR_CNTXT_SHFT
#define CECL_CE_STATUS_OPERATION_DONE_BMSK        HWIO_CRYPTO0_CRYPTO_STATUS_OPERATION_DONE_BMSK
#define CECL_CE_STATUS_DIN_RDY_BMSK               HWIO_CRYPTO0_CRYPTO_STATUS_DIN_RDY_BMSK
#define CECL_CE_STATUS_DIN_RDY_SHFT               HWIO_CRYPTO0_CRYPTO_STATUS_DIN_RDY_SHFT
#define CECL_CE_STATUS_DOUT_RDY_BMSK              HWIO_CRYPTO0_CRYPTO_STATUS_DOUT_RDY_BMSK
#define CECL_CE_STATUS_DOUT_RDY_SHFT              HWIO_CRYPTO0_CRYPTO_STATUS_DOUT_RDY_SHFT
#define CECL_CE_AUTH_SEG_CFG_USE_HW_KEY_SHFT      HWIO_CRYPTO0_CRYPTO_AUTH_SEG_CFG_USE_HW_KEY_SHFT
#define CECL_CE_ENCR_SEG_CFG_USE_HW_KEY_ENCR_SHFT HWIO_CRYPTO0_CRYPTO_ENCR_SEG_CFG_USE_HW_KEY_ENCR_SHFT
#define CECL_CE_PHYS_CRYPTO_AUTH_IV               0
#define CECL_CE_PHYS_CRYPTO_DATA_SHADOW           0

#define CECL_SYS_SEC_CRYPTO_CFG()                 HWIO_OUT(CRYPTO0_CRYPTO_ENCR_SEG_CFG, seg_cfg_val);
#define CECL_SYS_SEC_CRYPTO_GOPROC_QC_KEY()       HWIO_OUT(CRYPTO0_CRYPTO_GOPROC_QC_KEY, HWIO_CRYPTO0_CRYPTO_GOPROC_QC_KEY_GO_BMSK);
#define CECL_SYS_SEC_CRYPTO_GOPROC_OEM_KEY()      HWIO_OUT(CRYPTO0_CRYPTO_GOPROC_OEM_KEY, HWIO_CRYPTO0_CRYPTO_GOPROC_OEM_KEY_GO_BMSK);
#define CECL_SYS_SEC_CRYPTO_GOPROC()              HWIO_OUT(CRYPTO0_CRYPTO_GOPROC, HWIO_CRYPTO0_CRYPTO_GOPROC_GO_BMSK);

#define CECL_CE_CIPHER_AES_ALGO_VAL               0x2

/**
 * SHA algorithm type 
 */
//AUTH Seg CFG type 
#define CECL_CE_AUTH_SEG_CFG_AUTH_ALG_SHA         0x1
#define CECL_CE_AUTH_SEG_CFG_AUTH_SIZE_SHA1       0x0
#define CECL_CE_AUTH_SEG_CFG_AUTH_SIZE_SHA256     0x1 

#define CECL_SHA1_INIT_VECTOR_SIZE                5
#define CECL_SHA256_INIT_VECTOR_SIZE              8


