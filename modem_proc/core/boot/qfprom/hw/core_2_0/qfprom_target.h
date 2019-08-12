#ifndef QFPROM_TARGET_H
#define QFPROM_TARGET_H

/*===========================================================================

                        QFPROM  Driver Header  File

DESCRIPTION
 Contains target specific defintions and APIs to be used to read and write
 qfprom values.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright  2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/boot/qfprom/hw/core_2_0/qfprom_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/14   ck      Set blow timer clock to 2.4Mhz as DAL does not support 2.5
03/27/14   ck      Updated for Bear family/8916
10/05/12   dh      Fix Sec block frequency value
06/11/12   dh      Initial revision for 8974

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
  QFPROM_FEC_62_56,

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
* Configure QFPROM_ACCEL[QFPROM_TRIPPT_SEL] SW field to a value of 0x6 (110) to blow at a high resistance setting of 4 Kohms.
* Configure QFPROM_ACCEL[QFPROM_ACCEL] SW field to a value of 0x90.
* Leave the value of QFPROM_ACCEL[QFPROM_GATELAST] to a default value of 0x1 for 28nm HPm technology.
*/
#define QFPROM_ACCEL_VALUE             0xE90

/* QFPROM_ACCEL register default value */
#define QFPROM_ACCEL_RESET_VALUE       0xB00

/* Invalid entry */
#define QFPROM_INVALID_ENTRY 0xFFFFFFFF

#define QFPROM_FEC_15_10_EN_BIT_MASK 0xE0000000
#define QFPROM_FEC_15_10_EN_SHIFT 29


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
#define QFPROM_RAW_LOWEST_ADDRESS_OFFSET           (0x00000000)
#define QFPROM_RAW_HIGHEST_ADDRESS_OFFSET           (0x000001FC)

#define QFPROM_RAW_LOWEST_ADDRESS          (SECURITY_CONTROL_BASE + QFPROM_RAW_LOWEST_ADDRESS_OFFSET)
#define QFPROM_RAW_HIGHEST_ADDRESS         (SECURITY_CONTROL_BASE + QFPROM_RAW_HIGHEST_ADDRESS_OFFSET)

/*
** Corrected Address Start and End configuration to use for read/write operations.
** The start address is designed to be at the first supported region LSB address in the 
** memory map. The end address is designed to be the last supported region MSB address
** in the memory map.
*/
#define QFPROM_CORRECTED_LOWEST_ADDRESS   (QFPROM_RAW_LOWEST_ADDRESS + QFPROM_RAW_TO_CORRECTED_ADDRESS_OFFSET)
#define QFPROM_CORRECTED_HIGHEST_ADDRESS  (QFPROM_RAW_HIGHEST_ADDRESS + QFPROM_RAW_TO_CORRECTED_ADDRESS_OFFSET)



/*---------------------------------------------------------------------------
  QFPROM REGIONS 
---------------------------------------------------------------------------*/
typedef enum
{
  QFPROM_JTAG_ID_REGION = 0,
  QFPROM_FEATURE_ID_REGION,
  QFPROM_PTE0_REGION,
  QFPROM_PTE1_REGION,
  QFPROM_SERIAL_NUM_REGION,
  QFPROM_PTE2_REGION,
  QFPROM_READ_WRITE_PERMISSION_REGION,
  QFPROM_ANTI_ROLLBACK_1_REGION,
  QFPROM_ANTI_ROLLBACK_2_REGION,
  QFPROM_ANTI_ROLLBACK_3_REGION,
  QFPROM_OEM_CONFIG_REGION,
  QFPROM_FEATURE_CONFIG_REGION,
  QFPROM_PRI_KEY_DERIVATION_KEY_REGION,
  QFPROM_SEC_KEY_DERIVATION_KEY_REGION,
  QFPROM_OEM_SEC_BOOT_REGION,
  QFPROM_CALIB2_BOOT_REGION, 
  QFPROM_PK_HASH_REGION,
  QFPROM_CALIB_REGION,
  QFPROM_MEM_CONFIG_REGION,
  QFPROM_SPARE_REG15_REGION,
  QFPROM_SPARE_REG16_REGION,
  QFPROM_SPARE_REG17_REGION,
  QFPROM_SPARE_REG18_REGION,
  QFPROM_SPARE_REG19_REGION,

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
**  Function :    : qfprom_convert_raw_to_corr_data

** ==========================================================================
*/
/*!
* @brief : Used for rows with 15/10 FEC Scheme.
*               Converts a RAW format fuse data to CORR format by removing the
*               FEC data and compressing the remaining data to the LSB (32 bits
*               of data) and the MSB (the remaining 8 bits). 
* @param  : *pLSB - Data[21:20]X[4:0]Data[19:10]X[4:0]Data[9:0]
*               *pMSB - X[8:0]Data[39:30]X[4:0]Data[29:22]
* @par Dependencies: None
* @retval: *pLSB - Data[31:0]
*               *pMSB - Data[39:32] (Bits [7:0])
* @par Side Effects: FEC data is destroyed
*/
void qfprom_convert_raw_to_corr_data(uint32 *pLSB, uint32 *pMSB);


/*===========================================================================
**  Function :    : qfprom_convert_corr_to_raw_data

** ==========================================================================
*/
/*!
* @brief : Used for rows with 15/10 FEC Scheme.
*               Converts a CORR format fuse data to RAW format by uncompressing
*               the data in the LSB (fuseVal[0]) and lower the part of the MSB 
*               (fuseVal[1]) and inserting empty spaces for the FEC. This 
*               function will work with all sizes of data.
* @param  : *pLSB - Data[31:0]
*               *pMSB - Data[39:32] (Bits [7:0])
* @par Dependencies: None
* @retval: *pLSB - Data[21:20]X[4:0]Data[19:10]X[4:0]Data[9:0]
*               *pMSB - X[8:0]Data[39:30]X[4:0]Data[29:22]
* @par Side Effects: This will destory bits [63:40]
*/
void qfprom_convert_corr_to_raw_data(uint32 *pLSB, uint32 *pMSB);

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

FUNCTION  qfprom_write_set_clock_dependent_settings

DESCRIPTION
    This function shall set the appropriate clock dependent settings for the qfprom block.

PARAMETERS
  None.

DEPENDENCIES
  None.
 
RETURN VALUE
  uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 qfprom_write_set_clock_dependent_settings( void );

/*===========================================================================

FUNCTION  qfprom_write_reset_clock_dependent_settings

DESCRIPTION
    This function shall reset the appropriate clock dependent settings for the qfprom block.

PARAMETERS
  None.

DEPENDENCIES
  None.
 
RETURN VALUE
  uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 qfprom_write_reset_clock_dependent_settings( void );

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

