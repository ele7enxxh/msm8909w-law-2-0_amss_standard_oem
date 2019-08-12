#ifndef QFPROM_TARGET_H
#define QFPROM_TARGET_H

/*===========================================================================

                        QFPROM  Driver Header  File

DESCRIPTION
 Contains target specific defintions and APIs to be used to read and write
 qfprom values for sec ctrl 3.0.2 (used by 8x10).

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright  2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/13   lm      Ported and updated for 8x10 fuse map

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "qfprom.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  QFPROM_FEC_NONE = 0,
  QFPROM_FEC_15_10,
  QFPROM_FEC_63_56,

  /* Add above this */
  QFPROM_MAX_FEC
}QFPROM_FEC_SCHEME;

typedef enum
{
  QFPROM_CONV_FORMAT_RAW_TO_CORR = 0,
  QFPROM_CONV_FORMAT_CORR_TO_RAW = 1,
}QFPROM_CONV_FORMAT;

typedef enum
{
  QFPROM_ROW_LSB = 0,
  QFPROM_ROW_MSB,
}QFPROM_ROW;

/* Blow timer clock frequency in Mhz*/
#define QFPROM_BLOW_TIMER_CLK_FREQ_MHZ 2.4

/* Amount of time required to hold charge to blow fuse in micro-seconds */
#define QFPROM_FUSE_BLOW_TIME_IN_US    4

/*
* Configure QFPROM_ACCEL[QFPROM_TRIPPT_SEL] SW field to a value of 0x1 (001) to blow at a high resistance setting of 4 Kohms.
* Configure QFPROM_ACCEL[QFPROM_ACCEL] SW field to a value of 0x04.
* Leave the value of QFPROM_ACCEL[QFPROM_GATELAST] to a default value of 0x0 for 28nm LP technology.
*/
#define QFPROM_ACCEL_VALUE             0x104

/* QFPROM_ACCEL register default value */
#define QFPROM_ACCEL_RESET_VALUE       0x104


/* Invalid entry */
#define QFPROM_INVALID_ENTRY 0xFFFFFFFF

/* Maximum no: of bits per ROW in a QFPROM region */
#define QFPROM_MAX_BITS_PER_ROW 64 

/* For MSM8660, the Maximum rows for the supported read/write regions is defined below.
** This is currently being evaluated as 37.
*/
#define QFPROM_TARGET_MSM8660_MAX_ROWS_PER_REGION 37 

/*  Row Width =64bits , Half Row Width = 32bits  
**  Currently , support for maximum no: of rows per region is set below
*/
#define QFPROM_TARGET_MAX_ROWS_PER_REGION  \
            QFPROM_TARGET_MSM8660_MAX_ROWS_PER_REGION 

#define QFPROM_TARGET_MAX_HALF_ROWS_PER_REGION \
            (QFPROM_TARGET_MAX_ROWS_PER_REGION *2) 


/* Maximum no: of ROWs possible in a QFPROM region , this is 
** being used to define the data processing limits for write operation.
*/
#define QFPROM_MAX_ROWS_POSSIBLE_FOR_REGION  \
    (QFPROM_TARGET_MAX_ROWS_PER_REGION)

/* Max value for data processing for qfprom write operations */
#define QFPROM_MAX_REGION_BITS   \
    (QFPROM_TARGET_MAX_ROWS_PER_REGION * \
    QFPROM_MAX_BITS_PER_ROW)
    
/* Max value for data processing for qfprom write operations */
#define QFPROM_DATA_MAX_VAL  \
    ((((QFPROM_MAX_REGION_BITS / 56) + 1) * QFPROM_MAX_BITS_PER_ROW)/8)


/* Raw to Corrected Address region offset  */
#define QFPROM_RAW_TO_CORRECTED_ADDRESS_OFFSET   (0x4000)

/*
** Raw Address Start and End configuration to use for read/write operations.
** The start address is designed to be at the first supported region LSB address in the 
** memory map. The end address is designed to be the last supported region MSB address
** in the memory map.
*/
#define QFPROM_RAW_LOWEST_ADDRESS_OFFSET           (0x000000A0 )
#define QFPROM_RAW_HIGHEST_ADDRESS_OFFSET          (0x000003FC )

#define QFPROM_RAW_LOWEST_ADDRESS          (SECURITY_CONTROL_BASE + QFPROM_RAW_LOWEST_ADDRESS_OFFSET )
#define QFPROM_RAW_HIGHEST_ADDRESS         (SECURITY_CONTROL_BASE + QFPROM_RAW_HIGHEST_ADDRESS_OFFSET )

/*
** Corrected Address Start and End configuration to use for read/write operations.
** The start address is designed to be at the first supported region LSB address in the 
** memory map. The end address is designed to be the last supported region MSB address
** in the memory map.
*/
#define QFPROM_CORRECTED_LOWEST_ADDRESS   (QFPROM_RAW_LOWEST_ADDRESS + QFPROM_RAW_TO_CORRECTED_ADDRESS_OFFSET )
#define QFPROM_CORRECTED_HIGHEST_ADDRESS  (QFPROM_RAW_HIGHEST_ADDRESS + QFPROM_RAW_TO_CORRECTED_ADDRESS_OFFSET )

/* Macro for Read/write permission of corrected address */
#define QFPROM_READ_PERM_CORRECTED_ADDR		HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR
#define QFPROM_WRITE_PERM_CORRECTED_ADDR	HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR

/*---------------------------------------------------------------------------
  QFPROM REGIONS 
---------------------------------------------------------------------------*/
typedef enum
{
  QFPROM_JTAG_ID_REGION = 0,
  QFPROM_READ_PERMISSION_REGION,
  QFPROM_WRITE_PERMISSION_REGION,
  QFPROM_PTE_REGION,
  QFPROM_AP_ANTI_ROLLBACK_REGION,
  QFPROM_MSA_ANTI_ROLLBACK_REGION,
  QFPROM_SPARE7_REGION,
  QFPROM_SPARE8_REGION,
  QFPROM_SPARE9_REGION,
  QFPROM_OEM_CONFIG_REGION,
  QFPROM_FEATURE_CONFIG_REGION,
  QFPROM_MEM_REDUN_REGION,
  QFPROM_DEBUG_KEY_REGION,
  QFPROM_MEM_ACCEL_REGION,
  QFPROM_CALIB_REGION,
  QFPROM_FEC_EN_REGION,
  QFPROM_INTAGLIO_GLUEBIT_REGION,
  QFPROM_CUST_KEY_REGION,
  QFPROM_SERIAL_NUM_REGION,
  QFPROM_SPARE20_REGION,
  QFPROM_BOOT_ROM_PATCH_REGION,
  QFPROM_PRI_KEY_DERIVATION_KEY_REGION,
  QFPROM_SEC_KEY_DERIVATION_KEY_REGION,
  QFPROM_OEM_PK_HASH_REGION,
  QFPROM_OEM_SEC_BOOT_REGION,
  QFPROM_QC_SEC_BOOT_REGION, 
  QFPROM_USB_VID_PID_REGION,
  
  /* Add above this */
  QFPROM_LAST_REGION_DUMMY,
  QFPROM_MAX_REGION_ENUM                = 0x7FFF /* To ensure it's 16 bits wide */
} QFPROM_REGION_NAME;


typedef struct
{

    /* region name; added only for readability */  
    QFPROM_REGION_NAME               region;

    /* how many rows the region takes */
    uint32                        size;

    /*  fec type of region */
    QFPROM_FEC_SCHEME   fec_type;

    /* raw address  of the region */
    uint32                        raw_base_addr;      

    /* corrected address of the region */
    uint32                        corr_base_addr;

    /* can we read this region    */
    uint32                        read_perm_mask;      

    /* can we write to this region    */
    uint32                        write_perm_mask;

    /* LSB or MSB  of the qfprom permission region */
    QFPROM_ROW  perm_reg_type;
    
    /* Region read-able/non-read-able */
    boolean                       read_allowed;	
    
    /* Region index */
    uint32                       qfprom_region_index;	
    
} QFPROM_REGION_INFO;

typedef struct
{
    /* Row address */
    uint32   raw_row_address;

    /* MSB row data */
    uint32   row_data_msb;
    
    /* LSB row data */
    uint32   row_data_lsb;
} write_row_type;


extern const QFPROM_REGION_INFO qfprom_region_data[];


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

**  Function :

** ==========================================================================
*/
/*!
*   qfprom_is_region_readable
* 
* @brief
*   This function shall return if the read permission for the region passed is available or not.
* 
* @par Dependencies
*    None
* 
* @param
*   region - The QFPROM region to be checked.
* 
* @retval boolean 
*                         - TRUE - permission is available 
*                         - FALSE - permission is not available 
* 
* @par Side Effects
* 
*/
boolean     qfprom_is_region_readable
(
    QFPROM_REGION_NAME  region
);

/*===========================================================================

**  Function :    qfprom_is_region_writeable

** ==========================================================================
*/
/*!
*   qfprom_is_region_writeable
* 
* @brief
*   This function shall return if the write permission for the region passed is available or not.
* 
* @par Dependencies
*    None
* 
* @param
*   region - The QFPROM region to be checked.
* 
* @retval boolean 
*                         - TRUE - permission is available 
*                         - FALSE - permission is not available 
* 
* @par Side Effects
* 
*/
boolean     qfprom_is_region_writeable
(
    QFPROM_REGION_NAME  region
);

/*===========================================================================

**  Function : qfprom_get_region_name

** ==========================================================================
*/
/*!
* 
* @brief
*   This function gets the region of the given row address which need to be a RAW
*   address.
*  
* @par Dependencies
*   None.
*     
* @param:
*   uint32  address - Address of the Row.
*   QFPROM_ADDR_SPACE addr_type - Raw or Corrected address,
*   QFPROM_REGION_NAME* region_type - QFPROM Region of the Row address 
*                                                              passed.
* 
* @retval
* @retval: return - QFPROM_NO_ERR - if the operation is success.
*                           error_type           - if the operation is failure.                      
* @par Side Effects
*   None.
* 
*   
*/
uint32 qfprom_get_region_name
 (
    uint32 address,
    QFPROM_ADDR_SPACE addr_type,
    QFPROM_REGION_NAME* region_type    
 );

/*===========================================================================

**  Function : qfprom_is_fec_enabled

** ==========================================================================
*/
/*!
* 
* @brief
*   This function shall check the FEC status of the given Raw region row address.
*  
* @par Dependencies
*   None.
*     
* @param:
*   uint32 row_address - Address of the Row.
*   boolean *fec_status - TRUE , FEC is enabled. 
*                                   FALSE , FEC is disabled..
* 
* @retval
* @retval: return - QFPROM_NO_ERR - if the operation is success.
*                           error_type           - if the operation is failure.                      
* @par Side Effects
*   None.
* 
*/

uint32
qfprom_is_fec_enabled
(
    QFPROM_REGION_NAME    region,
    boolean *fec_status
);


/*===========================================================================

FUNCTION  qfprom_check_convert_to_active_address

DESCRIPTION
    This function shall check the address the appropriate voltage dependent settings for the qfprom block.

PARAMETERS
    uint32   row_address  - Pointer to the row address to be checked 
    uint32   *active_row_address  - Pointer to the active row address after conversion 

DEPENDENCIES
  None.
 
RETURN VALUE
  uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
  Disables the vreg being used.

===========================================================================*/

uint32
qfprom_check_convert_to_active_address
(
    uint32 row_address,
    uint32*active_row_address
);


/*===========================================================================

FUNCTION  qfprom_check_boundary_condition_for_row_address

DESCRIPTION
    This function shall check the boundary condition for the row address to be operated.

PARAMETERS
    uint32   row_address  - Row address to be checked 

DEPENDENCIES
  None.
 
RETURN VALUE
  uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
  Disables the vreg being used.

===========================================================================*/

uint32
qfprom_check_boundary_condition_for_row_address
(
    uint32 row_address
);


/*===========================================================================

FUNCTION  QFPROM_WRITE_FUSE_INIT

DESCRIPTION
  Here we might need to enable appropriate volatge regulator for each target.
  Currently MDM9x00 have volatge regulators ON by default.  
PARAMETERS
  None.

DEPENDENCIES
  None.
 
RETURN VALUE
  uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
  Turns on qfprom clock and enables VREG_GP5.

===========================================================================*/
uint32 qfprom_write_fuse_init( void );


/*===========================================================================

FUNCTION  QFPROM_WRITE_FUSE_CLEANUP

DESCRIPTION
  This function is going to use PMIC library API to trun OFF the LDO.
  Currently though for MDM9x00 we do not need to do anything.
 
DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  uint32   Any Errors while doing the cleanup.

SIDE EFFECTS
  Turns off qfprom clock and disables VREG_GP5.

===========================================================================*/
uint32 qfprom_write_fuse_cleanup( void );

/*===========================================================================

FUNCTION  QFPROM_IS_WRITE_OPERATION_ALLOWED

DESCRIPTION
  This function returns whether the clocks and voltage regulators have been
  setup correctly for writing fuses. 

DEPENDENCIES
  None.

PARAMETERS
  None.
 
RETURN VALUE
  boolean  Whether writing of fuses has been enabled.

SIDE EFFECTS
  None.

===========================================================================*/
boolean qfprom_is_write_operation_allowed ( void );


/*===========================================================================

**  Function : qfprom_get_region_index

** ==========================================================================
*/
/*!
* 
* @brief
*  This function gets the index of the region to which the given row address
*  belongs
*  
* @par Dependencies
*  None.
*     
* @param:
*  uint32 address - Address of the Row.
*  QFPROM_ADDR_SPACE addr_type - Raw or Corrected address,
*  uint32 *region_index - pointer to address location which will
*                          be filled in with index value by api.
* 
* @retval
* @retval: return - QFPROM_NO_ERR - if the operation is success.
*                   error_type    - if the operation is failure.                      
* @par Side Effects
*  None.
* 
*   
*/
uint32 qfprom_get_region_index
(
    uint32 address,
    QFPROM_ADDR_SPACE addr_type,
    uint32 *region_index
);


/*===========================================================================
**  Function :    : qfprom_write

** ==========================================================================
*/
/*!
*
* @brief :  This function writes the actual raw data to the row
*
*
* @param  :
*                 uint32   raw_row_address,
*                 uint32   *raw_data_to_write     - Pointer to data.
* @par Dependencies:
*
* @retval: return - QFPROM_NO_ERR - if the operation is success.
*                    error_type           - if the operation is failure.
*
* @par Side Effects:
* algo: Write the lower 32 bits and wait for the fuse write status to be ok.
*       Then write the upper 32 bits
*       If the write operation goes ok return QFPROM_NO_ERR else the error value.
*/
uint32 qfprom_write
(
    uint32   raw_row_address,
    uint32   *raw_data_to_write
);


/*===========================================================================
**  Function :    : qfprom_write_check_row_pre_conditions

** ==========================================================================
*/
/*!
*
* @brief :  This function shall check if the row address is part of supported QFPROM Region
*            returns the status.
*
* @param  :
*                 uint32   raw_row_address,
*                 uint32   *row_data,           - Pointer to data.
*                 QFPROM_REGION_NAME* region_type
* @par Dependencies:
*
* @retval: return - QFPROM_NO_ERR - if the operation is success.
*                   error_type           - if the operation is failure.
*
*/
uint32 qfprom_write_check_row_pre_conditions
(
    uint32   raw_row_address,
    uint32   *row_data,
    QFPROM_REGION_NAME* region_type
);


/*===========================================================================

**  Function : qfprom_find_index

** ==========================================================================
*/
/*!
*
* @brief
*   This function finds the index of the fuse in the chain.
*
* @par Dependencies
*   None.
*
* @param:
*   QFPROM_REGION_NAME  This tells the region ID for which we want to find index.
*
* @retval
*   uint32  Index in qfprom table that matches qfpromId;
*           return QFPROM_INVALID_ENTRY in no match found.
*
* @par Side Effects
*   None.
*
*
*/
uint32 qfprom_find_index(QFPROM_REGION_NAME qfpromRegion);


#endif /* QFPROM_TARGET_H */

