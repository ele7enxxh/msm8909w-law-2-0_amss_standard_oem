#ifndef TZBSP_BLSP_ARCH_H
#define TZBSP_BLSP_ARCH_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "tzbsp_blsp.h"


#define TZBSP_BLSP_COUNT  1 /**< Number of BLSP's supported in this target*/
#define TZBSP_QUPS_PER_BLSP  6 /**< Number of QUP instances per each BLSP*/
#define TZBSP_UARTS_PER_BLSP  2 /**< Number of UART instances per each BLSP*/
#define TZBSP_TOTAL_QUPS  (TZBSP_BLSP_COUNT * TZBSP_QUPS_PER_BLSP) /**< Total Number of QUPs combining all BLSP's*/
#define TZBSP_TOTAL_UARTS  (TZBSP_BLSP_COUNT * TZBSP_UARTS_PER_BLSP) /**< Total Number of UARTs combining all BLSP's*/

#define TZBSP_BLSP1_BAM_BASE_ADDR 0x07884000

/* 
 * These EE id's are derived from access control team. Make sure, that these
 * id's are reviewed by BAM team whenever a new target is added.
 */
typedef enum {
   TZBSP_EE_AP = 0,
   TZBSP_EE_LPASS,
   TZBSP_EE_MSS,
   TZBSP_EE_TZ,
   TZBSP_EE_INVALID,  //BAM_MAX_EES in bamtgtcfg.h
} bam_ee_id_t;

/** @brief Set BLSAP AHB clock ON/OFF

    @param[in] blsp   BLSP Id.
    @param[in] flag    TRUE  - Clock ON
                       FALSE - Clock OFF
  
    @return E_SUCCESS if success otherwise error code.
*/
int tzbsp_blsp_set_clock(uint32 blsp, boolean flag);

/** @brief Locks GPIO
  
    @param[in] pTzBspPeripheralInfo       QUP Information.
    @param[in] vmidAccessBitMap    VMID Bit map specifying the subsystem.
        
    @return E_SUCCESS if success otherwise error code.
*/
int tzbsp_blsp_gpio_lock(TzBsp_Blsp_Peripheral_Info *pTzBspPeripheralInfo, uint32 vmidAccessBitMap);

/** @brief Assign BAM Pipe to required sub-system
  
    @param[in] blsp       BLSP Id
    @param[in] coreId     BLSP Core Id
    @param[in] bamEEId    Whether to protect the GPIO or not.
                          TRUE - Protect GPIO
                          ALSE - Un-Protect GPIO
    
    @return E_SUCCESS if success otherwise error code.
*/
int tzbsp_blsp_bam_pipe_assign(uint32 blsp, uint32 coreId, uint32 bamEEId);

/** @brief Locks QUP for a sub-system

    @param[in] pTzBspPeripheralInfo       QUP Information.
    @param[in] vmidAccessBitMap    VMID Bit map specifying the subsystem.
    @param[in] lock                Whether to protect the QUP or not.
                                   TRUE - Protect QUP
                                   FALSE - Un-Protect QUP
    
    @return E_SUCCESS if success otherwise error code.
*/
int tzbsp_blsp_qup_lock (TzBsp_Blsp_Peripheral_Info *pTzBspPeripheralInfo, uint32 vmidAccessBitMap, boolean lock);

/** @brief Configure SMMU settings for BAM Pipes
  
    @param[in] blsp         BLSP Id.
    @param[in] coreId       BLSP Core Id.
    @param[in] smmuConfig   TRUE - Secure World
                            FALSE - Non Secure World	
        
    @return E_SUCCESS if success otherwise error code.
*/
int tzbsp_blsp_smmu_config(uint32 blsp, uint32 coreId, boolean smmuConfig);

#endif /* TZBSP_BLSP_ARCH_H */
