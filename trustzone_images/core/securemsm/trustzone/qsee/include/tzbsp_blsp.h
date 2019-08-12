#ifndef TZBSP_BLSP_H
#define TZBSP_BLSP_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>


#define TZBSP_BLSP_GPIOS_SUPPORTED    6
#define TZBSP_BLSP_INDICES_SUPPORTED  3

#define TZBSP_BLSP_PERIPHERAL_INFO(PROTOCOL, PERSISTENCE, GPIO_PROTECTION_TYPE, PERIPHERAL_ID, PERIPHERAL_RG_INDEX, BLSP_XPU_ID, GPIO_1, GPIO_2, GPIO_3, GPIO_4, GPIO_5, GPIO_6, NUM_GPIOS, GPIO_INDEX_1, GPIO_INDEX_2, GPIO_INDEX_3, SUBSYSTEM_ID, IS_PERIPHERAL_TZ_USED) \
{                       \
PROTOCOL,           \
PERSISTENCE,            \
GPIO_PROTECTION_TYPE,   \
PERIPHERAL_ID,          \
PERIPHERAL_RG_INDEX,    \
BLSP_XPU_ID,            \
{GPIO_1, GPIO_2, GPIO_3, GPIO_4, GPIO_5, GPIO_6},  \
NUM_GPIOS,              \
{GPIO_INDEX_1, GPIO_INDEX_2, GPIO_INDEX_3},  \
SUBSYSTEM_ID,           \
IS_PERIPHERAL_TZ_USED   \
}                       \

#ifndef TZBSP_BLSP_UNREFERENCED_PARAMETER
#define TZBSP_BLSP_UNREFERENCED_PARAMETER(p)   (p)
#endif

/* 
 * These id's are managed only by buses. Same sub-system Id's were used in 
 * qsee_blsp.h and make sure both these enums should be in sync with each 
 * other 
 */
typedef enum {
   TZBSP_NONE_ID = 0,      /* Use this SubSystem ID When the QUP instance has no use case. 
                            * This can be used only at the time of cold boot stage. Not used
                            * for dynamic switching */
   TZBSP_TZ_ID = 1,
   TZBSP_APSS_ID = 3,
   TZBSP_ADSP_ID = 6,
   TZBSP_MAX_ID = 0xFFFF
} subsystem_id_t;

typedef enum TzBspBlspGpioProtType TzBspBlspGpioProtType;
enum TzBspBlspGpioProtType
{
   PROTECTION_TYPE_MPU,
   PROTECTION_TYPE_RPU
};

typedef enum TzBspBlspProtocol TzBspBlspProtocol;
enum TzBspBlspProtocol
{
   PROTOCOL_SPI,
   PROTOCOL_I2C,
   PROTOCOL_UART_2_LINE,   /* Without HW Flow control*/
   PROTOCOL_UART_4_LINE    /* With HW Flow control */
};

typedef enum TzBspBlspIsPersistent TzBspBlspIsPersistent;
enum TzBspBlspIsPersistent
{
   PERSISTENT,
   NON_PERSISTENT
};

typedef enum TzBspBlspPeripheralId TzBspBlspPeripheralId;
enum TzBspBlspPeripheralId
{
   BLSP_QUP_1,
   BLSP_QUP_2,
   BLSP_QUP_3,
   BLSP_QUP_4,
   BLSP_QUP_5,
   BLSP_QUP_6,
   BLSP_QUP_7,
   BLSP_QUP_8,
   BLSP_QUP_9,
   BLSP_QUP_10,
   BLSP_QUP_11,
   BLSP_QUP_12,
   BLSP_UART_START,
   BLSP_UART_1 = BLSP_UART_START,
   BLSP_UART_2,
   BLSP_UART_3,
   BLSP_UART_4,
   BLSP_UART_5,
   BLSP_UART_6,
   BLSP_UART_7,
   BLSP_UART_8,
   BLSP_UART_9,
   BLSP_UART_10,
   BLSP_UART_11,
   BLSP_UART_12,
   BLSP_INVALID_ID = 0x7FFFFFFF,
};

  
typedef struct tzbsp_blsp_peripheral_info_s
{
   TzBspBlspProtocol  protocol; /* Whether this Peripheral is used for SPI/I2C/UART */
   TzBspBlspIsPersistent  isPersistent; /* Whether the protection should remain for ever starting from TZ Cold Boot and 
                                         * no dynamic switching of BLSP Peripheral b/w TZ and other sub-system. To achieve dynamic 
                                         * owner switching this field should be set to FALSE irrespective of whether cold
                                         * boot protection is enabled or not */
   TzBspBlspGpioProtType  protectionType; /* Whether the MPU/RPU protection is used of GPIO's. For Bear it is MPU and for Badger it is RPU */   
   TzBspBlspPeripheralId  peripheralId; /* Peripheral ID starting from 1 */
   uint32  uPeripheralRgIndex; /* Peripheral (QUP/UART) RG Index */
   uint32  uBlspXpuId; /* XPU Index used for QUP/UART protection */
   uint32  uGpios[TZBSP_BLSP_GPIOS_SUPPORTED]; /* GPIO numbers used for protection and configuring functionality */
   uint32  uNumGpios; /* Total number of GPIO's used */
   uint32  uGpioIndex[TZBSP_BLSP_INDICES_SUPPORTED]; /* Index used for GPIO Protection in Bear targets. 
                                                      * NOTE:: Must contact Qualcomm for index numbers.
                                                      *        Must set unused entries to 0xFFFFFFFF. */
   uint32  uSubSystemId; /* Subsystem ID, tells to which subsystem the QUP/UART and its resources has to be assigned */
   boolean  bIsPeripheralTzUsed; /* TRUE, the QUP/UART is assigned to only TZ or shared between TZ and any other subsystem.
                                  * FALSE, not assigned to TZ (even in Dual EE) */   
} TzBsp_Blsp_Peripheral_Info;

/**
 * System call to change BLSP ownership from secure world (TZ).
 *
 * @param[in] peripheralId      Blsp Peripheral Id.
 * @param[in] owner_ss          Subsystem Owner ID.
 *
 * @return \c E_SUCCESS on success, an error code otherwise.
 */
int tzbsp_blsp_modify_ownership(uint32 peripheralId, uint32 owner_ss);

/**
 * System call to change BLSP ownership from non-secure world
 *
 * @param[in] peripheralId      Blsp Peripheral Id.
 * @param[in] owner_ss          Subsystem Owner ID.
 *
 * @return \c E_SUCCESS on success, an error code otherwise.
 */
int tzbsp_blsp_modify_ownership_ex(uint32 peripheralId, uint32 owner_ss);


/**
 * @brief Protects QUP and GPIO and configures GPIO functionality.
 *
 * @return E_SUCCESS on success, an error code otherwise.
 */
int tzbsp_blsp_init(void);

/** @brief Get Default owner of the BLSP instance from the table.

    @param[in] peripheralId        BLSP Peripheral Id (QUP/UART).
    
    @param[out] defaultOwner        Default owner..

    @return E_SUCCESS is the function is success. otherwise failure.
*/
int tzbsp_blsp_get_default_owner(uint32 peripheralId, uint32 *defaultOwner);


#endif /* TZBSP_BLSP_H */


