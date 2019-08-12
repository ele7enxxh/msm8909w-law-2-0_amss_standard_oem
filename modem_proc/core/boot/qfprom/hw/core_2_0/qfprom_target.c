/*===========================================================================

                       QFPROM  Driver Source  Code

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

$Header: //components/rel/core.mpss/3.5.c12.3/boot/qfprom/hw/core_2_0/qfprom_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/15   tj      Removed temp_bit_mask to avoid warning
05/20/14   ck      Removed QFPROM_IS_FEC_ENABLED macro which had bug
04/14/14   tj      Correct the fec enable region base address
03/27/14   ck      Updated for Bear family/8916
09/03/13   lm      Replaced banned function (memcpy) with memscpy
12/17/12   kpa     Ported to mpss image.
10/05/12   dh      Change blow timer cycle calculation to round up 
06/11/12   dh      Initial revision for 8974

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"
#include QFPROM_HWIO_REG_INCLUDE_H
#include "HALqfprom.h"
#include "qfprom.h"
#include QFPROM_TARGET_INCLUDE_H
#include "qfprom_subsys.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*---------------------------------------------------------------------------
  Used to keep track if clock and PMIC voltage regulators are on.
---------------------------------------------------------------------------*/
static boolean qfprom_write_operation_allowed = FALSE;


/*=============================================================================
                                 LOCAL FUNCTION DEFINITIONS
=============================================================================*/


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
uint32 qfprom_find_index(QFPROM_REGION_NAME qfpromRegion)
{
    uint32 return_value = QFPROM_INVALID_ENTRY;
    uint32  index = 0;

    /*
    **  Find matching qfprom ID to obtain index in table
    */
    for (index = 0; qfprom_region_data[index].region != QFPROM_LAST_REGION_DUMMY;
           index++)
    {
        if (qfprom_region_data[index].region == qfpromRegion)
        {
            return_value = index;
            break;
        }
    }

    return return_value;
}



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
uint32
qfprom_write_check_row_pre_conditions
(
    uint32   raw_row_address,
    uint32   *row_data,
    QFPROM_REGION_NAME* region_type
)
{
    uint32 qfprom_err_status = QFPROM_ERR_UNKNOWN;
    boolean is_writeable,is_readable,value = FALSE;

    do
    {
        /* Check if the row address is part of supported QFPROM Region  */
        qfprom_err_status = qfprom_get_region_name(raw_row_address,
                                        QFPROM_ADDR_SPACE_RAW,
                                        region_type);

        if (qfprom_err_status != QFPROM_NO_ERR )
        {
            break;
        }

        /*  Check if write permissions are there or not.
        **  if read permissions are there then read the region
        ** else return error value in success
        */
        is_writeable = qfprom_is_region_writeable(*region_type);

        if (is_writeable == FALSE)
        {
            qfprom_err_status = QFPROM_REGION_NOT_WRITEABLE_ERR;
            break;
        }

        /*  Check if FEC is enabled for the Row.
        **  If FEC is enabled then write is not supported.
        */
        qfprom_err_status = qfprom_is_fec_enabled(*region_type,
                                                   &value);

        if (qfprom_err_status != QFPROM_NO_ERR)
        {
            break;
        }

        if (value)
        {
            qfprom_err_status = QFPROM_REGION_FEC_ENABLED_NOT_WRITEABLE_ERR;
            break;
        }


        /*  Check if Read permissions are there or not.
        **  if read permissions are there then read the region
        ** else return error value in success
        */
        is_readable = qfprom_is_region_readable(*region_type);

        if (is_readable == FALSE)
        {
            qfprom_err_status = QFPROM_REGION_NOT_READABLE_ERR;
            break;
        }
    }while(0);

    return qfprom_err_status;
}


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
)
{
    uint32 value;

    uint32 qfprom_err_status = QFPROM_NO_ERR;

    do
    {

        /* Check if QFPROM hw is busy or not */
        /* value   0: ok to Blow
                   1:  busy blowing fuse
                   2: error occured while blowing
          */
        do
        {
            HAL_qfprom_ReadBlowStatus(&value);
        }while(value == 0x1);

        if(value != 0x0)
        {
           /*Fuse blow error occured */
           qfprom_err_status = QFPROM_WRITE_ERR ;
           break;
        }

        /* Write QFPROM LSB Row data */
        qfprom_err_status = HAL_qfprom_WriteRawAddress(raw_row_address,
                                                            raw_data_to_write[0]);

        if (qfprom_err_status != QFPROM_NO_ERR)
        {
            break;
        }

        /* Check if QFPROM write can happen or not */
        do
        {
            HAL_qfprom_ReadBlowStatus(&value);
        }while(value == 0x1);

        if(value != 0x0)
        {
           /*Fuse blow error occured */
           qfprom_err_status = QFPROM_WRITE_ERR ;
           break;
        }
        /*If reach  here, value is 0 ie blow was ok */

        /* Write Qfprom MSB Row data */
        qfprom_err_status = HAL_qfprom_WriteRawAddress((raw_row_address +4),
                                                            raw_data_to_write[1]);
        /* Check if QFPROM write can happen or not */
        do
        {
            HAL_qfprom_ReadBlowStatus(&value);
        }while(value == 0x1);

        if(value != 0x0)
        {
           /*Fuse blow error occured */
           qfprom_err_status = QFPROM_WRITE_ERR ;
           break;
        }

        }while(0);

    return qfprom_err_status;
}


/*=============================================================================
                                 GLOBAL FUNCTION DEFINITIONS
=============================================================================*/


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
uint32 qfprom_write_fuse_init( void )
{
    uint32 ret_val = QFPROM_NO_ERR;

    do
    {
       /*Ensure pmic and clock settings are reset to default before
         enabling them */
        ret_val = qfprom_write_fuse_cleanup();

        if (ret_val != QFPROM_NO_ERR)
        {
            break;
        }

        ret_val = qfprom_write_set_clock_dependent_settings();

        if (ret_val != QFPROM_NO_ERR)
        {
            break;
        }

        ret_val = qfprom_write_set_voltage_dependent_settings();

    }while(0);

    if (ret_val != QFPROM_NO_ERR)
    {
       (void)qfprom_write_fuse_cleanup();
    }
    else
    {
        /* Set the write operation active flag */
        qfprom_write_operation_allowed = TRUE;
    }

    return ret_val;
}

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
  None.

===========================================================================*/
uint32 qfprom_write_fuse_cleanup( void )
{
    uint32 ret_val = QFPROM_NO_ERR;

    do
    {
        /* At any cost the LDO should be switched OFF for MDM 9k as power down
        ** has issues and could cause MSM to be in bad state if it is ON.
        ** For MSM 8660 too its necessary to switch off voltage to the qfprom
        ** Power rail, to prevent any later accidental writes to qfprom region.
        */
        ret_val = qfprom_write_reset_voltage_dependent_settings();


        ret_val = qfprom_write_reset_clock_dependent_settings();


    }while(0);

    /* Reset the write operation active flag */
    qfprom_write_operation_allowed = FALSE;

    return ret_val;
}

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
boolean qfprom_is_write_operation_allowed ( void )
{
  return qfprom_write_operation_allowed;
}


/*===========================================================================
**  Function :    : void qfprom_convert_raw_to_corr_data(uint32 *plsb, uint32 *pmsb)

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
void qfprom_convert_raw_to_corr_data(uint32 *pLSB, uint32 *pMSB)
{
  uint32 returnFuse[2] = {0};

  /* Shift the data to the correct positions */
  returnFuse[0] = ( (((*pLSB >>  0) & 0x3FF) <<  0)   /* [09:00] */
                   |(((*pLSB >> 15) & 0x3FF) << 10)   /* [19:10] */
                   |(((*pLSB >> 30) & 0x3  ) << 20)   /* [21:20] */
                   |(((*pMSB >>  0) & 0xFF ) << 22)   /* [29:22] */
                   |(((*pMSB >> 13) & 0x3  ) << 30)); /* [31:30] */
  returnFuse[1] = (  ((*pMSB >> 15) & 0xFF ) <<  0);  /* [39:32] */

  /* Return data */
  *pLSB = returnFuse[0];
  *pMSB = returnFuse[1];
}


/*===========================================================================
**  Function :    : void qfprom_convert_corr_to_raw_data(uint32 *plsb, uint32 *pmsb)

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
void qfprom_convert_corr_to_raw_data(uint32 *pLSB, uint32 *pMSB)
{
  uint32 returnFuse[2] = {0,0};

  /* Unshift the data to the correct positions */
  returnFuse[0] = ( (((*pLSB >>  0) & 0x3FF) <<  0)   /* [09:00] */
                   |(((*pLSB >> 10) & 0x3FF) << 15)   /* [19:10] */
                   |(((*pLSB >> 20) & 0x3  ) << 30)); /* [21:20] */
  returnFuse[1] = ( (((*pLSB >> 22) & 0xFF ) <<  0)   /* [29:22] */
                   |(((*pLSB >> 30) & 0x3  ) << 13)   /* [31:30] */
                   |(((*pMSB >>  0) & 0xFF ) << 15)); /* [39:32] */

  /* Return data */
  *pLSB = returnFuse[0];
  *pMSB = returnFuse[1];
}



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
)
{
    uint32 raw_base_address_start,raw_base_address_end;
    uint32 corr_base_address_start,corr_base_address_end;
    uint32 qfprom_err_status = QFPROM_ADDRESS_INVALID_ERR;

    if (SECURITY_CONTROL_BASE == SECURITY_CONTROL_BASE_PHYS)
    {
      if( ( row_address >= QFPROM_CORRECTED_LOWEST_ADDRESS ) &&
          ( row_address <= QFPROM_CORRECTED_HIGHEST_ADDRESS ))
      {
          *active_row_address =  row_address;
          qfprom_err_status = QFPROM_NO_ERR;
      }
      else if (( row_address >= QFPROM_RAW_LOWEST_ADDRESS ) &&
            ( row_address <= QFPROM_RAW_HIGHEST_ADDRESS ))
  
      {
          *active_row_address =  row_address;
          qfprom_err_status = QFPROM_NO_ERR;
      }
    }
    else
    {
      raw_base_address_start = SECURITY_CONTROL_BASE_PHYS +
                              QFPROM_RAW_LOWEST_ADDRESS_OFFSET;
    
      raw_base_address_end = SECURITY_CONTROL_BASE_PHYS +
                            QFPROM_RAW_HIGHEST_ADDRESS_OFFSET;
    
      corr_base_address_start = raw_base_address_start +
          QFPROM_RAW_TO_CORRECTED_ADDRESS_OFFSET;
    
      corr_base_address_end = raw_base_address_end +
                      QFPROM_RAW_TO_CORRECTED_ADDRESS_OFFSET ;
    
      if( ( row_address >= corr_base_address_start ) &&
              ( row_address <= corr_base_address_end ))
      {
        *active_row_address = SECURITY_CONTROL_BASE + row_address - SECURITY_CONTROL_BASE_PHYS;
        qfprom_err_status = QFPROM_NO_ERR;
      }
      else if (( row_address >= raw_base_address_start ) &&
              ( row_address <= raw_base_address_end ))

      {
        *active_row_address = (SECURITY_CONTROL_BASE +
            QFPROM_RAW_LOWEST_ADDRESS_OFFSET +
            (row_address - raw_base_address_start));

        qfprom_err_status = QFPROM_NO_ERR;
      }
    }

  return qfprom_err_status;
}


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
)
{
    uint32 return_value = QFPROM_ROW_BOUNDARY_CONDITION_VERIFY_ERR;
    uint32  index,row_index = 0;
    uint32 local_raw_row_address = 0x0;
    uint32 local_corr_row_address = 0x0;

    /*
    **  Ensure the row address is falling in the row boundary.
    */
    for (index = 0; qfprom_region_data[index].region != QFPROM_LAST_REGION_DUMMY;
           index++)
    {
        row_index = qfprom_region_data[index].size;

        do
        {
            local_raw_row_address =  ( qfprom_region_data[index].raw_base_addr  + (row_index -1)*8 );
            local_corr_row_address =  ( qfprom_region_data[index].corr_base_addr + (row_index -1)*8 );


            if ( (row_address == local_raw_row_address) ||
                  (row_address == local_corr_row_address) )
            {
                return_value = QFPROM_NO_ERR;
                break;
            }
            row_index--;
        }while(row_index > 0);
    }

    return return_value;
}


/*===========================================================================

**  Function :qfprom_is_region_readable

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
boolean  qfprom_is_region_readable
(
    QFPROM_REGION_NAME  region
)
{
    boolean status = FALSE;
    uint32  index;
    uint32 read_perm = 0;
    uint32 return_value = QFPROM_ERR_UNKNOWN;


    for (index = 0; qfprom_region_data[index].region != QFPROM_LAST_REGION_DUMMY;
           index++)
    {
        if (qfprom_region_data[index].region == region)
        {

            /* Read the read permission register values */
            return_value = HAL_qfprom_ReadCorrectedAddress(
                           HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR,
                           &read_perm);

            if (return_value != QFPROM_NO_ERR )
            {
                break;
            }

            /* Mask the values with the reg type so that LSB ,MSB masks are appropriately
            ** applied.
            **
            **  0 - Allow    Read
            **  1 - Disable  Read
            */
            if ((read_perm & qfprom_region_data[index].read_perm_mask) == 0x0)
            {
                status = TRUE;
            }
            
            break;
        }
    }

   return status;
}



/*===========================================================================

**  Function :

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
)
{
    boolean status = FALSE;
    uint32  index;
    uint32 write_perm = 0;
    uint32 return_value = QFPROM_ERR_UNKNOWN;


    for (index = 0; qfprom_region_data[index].region != QFPROM_LAST_REGION_DUMMY;
           index++)
    {
        if (qfprom_region_data[index].region == region)
        {

            /* Read the read permission register values */
            return_value = HAL_qfprom_ReadCorrectedAddress(
                           HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR,
                           &write_perm);

            if (return_value != QFPROM_NO_ERR )
            {
                break;
            }

            /* Mask the values with the reg type so that LSB ,MSB masks are appropriately
            ** applied.
            **
            **  0 - Allow    write
            **  1 - Disable  write
            */
            if ((write_perm & qfprom_region_data[index].read_perm_mask) == 0x0)
            {
                status = TRUE;
            }
            
            break;
        }
    }

   return status;
}

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
*   uint32 address - Address of the Row.
*   HAL_QFPROM_ADDR_SPACE_TYPE_ENUM addr_type - Raw or Corrected address,
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
 )
{
    uint32 return_value = QFPROM_REGION_NOT_SUPPORTED_ERR;
    uint32  index = 0;
    uint32 region_start_row_addr = 0;
    uint32 region_end_row_addr = 0;

    /*
    **  Find matching qfprom ID to obtain index in table
    */
    for (index = 0; qfprom_region_data[index].region != QFPROM_LAST_REGION_DUMMY; index++)
    {

        if (addr_type == QFPROM_ADDR_SPACE_RAW)
        {
            region_start_row_addr = qfprom_region_data[index].raw_base_addr;
        }
        else if (addr_type == QFPROM_ADDR_SPACE_CORR)
        {
            region_start_row_addr = qfprom_region_data[index].corr_base_addr;
        }

        region_end_row_addr = ((region_start_row_addr) +
                                            ((qfprom_region_data[index].size) *8) );

        if (( address >=  region_start_row_addr) &&
            (address < region_end_row_addr ))
        {
            *region_type = qfprom_region_data[index].region;
            return_value = QFPROM_NO_ERR;
            break;
        }
    }

    return return_value;
}

/*===========================================================================

**  Function : qfprom_is_fec_enabled

** ==========================================================================
*/
/*!
*
* @brief
*   This function shall check the FEC status of the given region.
*
* @par Dependencies
*   None.
*
* @param:
*   QFPROM_REGION_NAME region_type - name of the region.
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
    QFPROM_REGION_NAME  region_type,
    boolean *fec_status
)
{
    uint32 return_value = QFPROM_NO_ERR;
    uint32 index = 0;
    uint32 fec_en_msb = 0;

    do
    {
        /*
        **  Find matching qfprom ID to obtain index in table
        */
        for (index = 0; qfprom_region_data[index].region != QFPROM_LAST_REGION_DUMMY; index++)
        {
            if (qfprom_region_data[index].region == region_type)
            {
                if (qfprom_region_data[index].fec_type == QFPROM_FEC_62_56)
                {
                  /* FEC enabled bit is the MSB of the row MSB.
                     Add 4 to corrected base address to get FEC bits. */
                  return_value =
                    HAL_qfprom_ReadCorrectedAddress(qfprom_region_data[index].corr_base_addr + 4,
                                                    &fec_en_msb);
                  
                  if (return_value != QFPROM_NO_ERR)
                  {
                    break;
                  }
                  
                  *fec_status = (fec_en_msb & 0x80000000) > 0 ? TRUE : FALSE; 
                  return_value = QFPROM_NO_ERR;
                }
                /*For regions that do no support FEC , no need for FEC action */
                else if (qfprom_region_data[index].fec_type == QFPROM_FEC_NONE)
                {
                    *fec_status = FALSE;
                    return_value = QFPROM_NO_ERR;
                }
                /* Unsupported FEC type , report error */
                else 
                {
                    *fec_status = FALSE;
                    return_value = QFPROM_ERR_UNKNOWN;
                }
                
              /*Once the matching region break out of the loop*/  
              break;
            }
        } /* end for */
    }while(0);
    
    return return_value;
}


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
 )
{
  uint32 return_value = QFPROM_REGION_NOT_SUPPORTED_ERR;
  uint32  index = 0;
  uint32 region_start_row_addr = 0;
  uint32 region_end_row_addr = 0;    
  uint32 active_address;
  
  do
  {
    if(region_index == NULL)
    {
      return_value = QFPROM_DATA_PTR_NULL_ERR;
      break;
    }
    
   /* The requested address could be physical. Convert the address in
    * to a virtual address */
    return_value = qfprom_check_convert_to_active_address(
                                            address,
                                            &active_address);
    if (return_value != QFPROM_NO_ERR)
    {
        break;
    }
    
    /* 
    **  Find matching qfprom ID to obtain index in table
    */
    for (index = 0; qfprom_region_data[index].region != QFPROM_LAST_REGION_DUMMY;
         index++)
    {
      if (addr_type == QFPROM_ADDR_SPACE_RAW)
      {
        region_start_row_addr = qfprom_region_data[index].raw_base_addr;
      }
      else if (addr_type == QFPROM_ADDR_SPACE_CORR)
      {
        region_start_row_addr = qfprom_region_data[index].corr_base_addr;
      }

      region_end_row_addr = ((region_start_row_addr) + 
                                       ((qfprom_region_data[index].size) *8) );
        
      if (( active_address >=  region_start_row_addr) && 
          (active_address < region_end_row_addr ))
      {
         *region_index = qfprom_region_data[index].qfprom_region_index;
         return_value = QFPROM_NO_ERR;
         break;
      }
    }

  }while(0);

  return return_value;
} 


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
uint32 qfprom_write_set_clock_dependent_settings( void )
{
    uint32 ret_val = QFPROM_NO_ERR;
    uint32 blow_value = 0x0;

    ret_val = qfprom_clock_enable();

    if(QFPROM_NO_ERR == ret_val)
    {
      /*  QFPROM block requires a held charge for (QFPROM_FUSE_BLOW_TIME_IN_US) micro-
      **  seconds time to blow a fuse.
      **  blow_value = Number of clocks that the blow_fuses signal (round up)  ,
      */

      if (QFPROM_BLOW_TIMER_CLK_FREQ_MHZ > 0 )
      {
        blow_value = QFPROM_BLOW_TIMER_CLK_FREQ_MHZ * QFPROM_FUSE_BLOW_TIME_IN_US;
        HAL_qfprom_SetBlowTimer(blow_value);
      }
      else
      {
          ret_val =QFPROM_CLOCK_SETTINGS_INVALID_ERR;
      }

      /* 
        Configue QFPROM_ACCEL[QFPROM_ACCEL] to 0x90,
        Configure QFPROM_ACCEL[QFPROM_TRIPPT_SEL] to 0x6 for blowing 4k ohms high resistence    
        Configure QFPROM_ACCEL[QFPROM_GATELAST] to 0x1
      */
      HAL_qfprom_SetAccelator(QFPROM_ACCEL_VALUE);
    }
    return ret_val;
}


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
uint32 qfprom_write_reset_clock_dependent_settings( void )
{
    qfprom_clock_disable();
    
    /* Set blow timer to 0 to block accidental blows to QFPROM. */
    HAL_qfprom_SetBlowTimer(0);
    
    /* Restore Accelator setting for normal fuse read */
    HAL_qfprom_SetAccelator(QFPROM_ACCEL_VALUE);
    
    return QFPROM_NO_ERR;
}

/* For test purposes only */
#ifdef QFPROM_TEST
void
qfprom_test_get_qfprom_region_data(QFPROM_REGION_INFO *data_ptr)
{
    memscpy(data_ptr,sizeof(qfprom_region_data),qfprom_region_data,sizeof(qfprom_region_data));
}
#endif
