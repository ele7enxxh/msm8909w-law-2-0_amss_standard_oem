/*! \file
*  \n
*  \brief  SPMI_1_0_lite.cpp ---- LITE SPMI Version 2.0 RELATED DEFINITION
*  \n
*  \n This header file contains class implementation of the SPMI Version
*  \n 2.0 messaging interface
*  \n
*  \n &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/pmic/comm/spmi_lite/src/spmi_1_0_lite.c#1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
09/25/12   hw     Remove unecessary C++ to C wrapper
03/21/12   hs     Created

========================================================================== */

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "spmi_1_0_lite.h"
#include "SpmiBus.h"

#define PM_MAX_REGS     0xFFFF //TODO check if it is still needed.

/* TRUE when SPMI already initialized. */
static boolean spmi_initialized = FALSE;

/* ========================================================================
**  Function : pm_spmi_lite_read_byte
** ======================================================================*/
/*
    Description: Read a byte data buffer from SPMI device.

    @param
    uSlaveId   -- Slave device ID to read from.
    addr       -- Device address to read from.
    pData*     -- Byte pointer to the data buffer.
    priority   -- Read priority.

    @return
    PM_ERR_FLAG__SUCCESS -- Read success.
    PM_ERR_FLAG__DAL_SERVICE_FAILED -- Read failed.

    @dependencies
    None.

    @sa None
*/
pm_err_flag_type
pm_spmi_lite_read_byte
(
  uint32 uSlaveId ,
  uint16 addr,
  uint8* pData,
  uint8 priority
)
{
  pm_err_flag_type     err = PM_ERR_FLAG__SUCCESS ;
  SpmiBus_ResultType   spmi_result = SPMI_BUS_SUCCESS;
  uint32               dataReadLength = 0;

  if(FALSE == spmi_initialized)
  {
    // initialize the SPMI BUS
    spmi_result = SpmiBus_Init();

    if(SPMI_BUS_SUCCESS  == spmi_result )
    {
      spmi_initialized = TRUE;
    }
  }

  /* Read the data from the SPMI */
  if(TRUE == spmi_initialized)
  {
    /* check for out-of-bounds index */
    if ( addr > PM_MAX_REGS)
    {
      err = PM_ERR_FLAG__SPMI_OPT_ERR ;
    }
    else
    {
      spmi_result = SpmiBus_ReadLong( uSlaveId,
                                      (SpmiBus_AccessPriorityType)priority,
                                      addr,
                                      pData,
                                      1,
                                      &dataReadLength);

      /* TODO should we update the shadow register? */
      //pm_image[addr] = *data ;

      if ( spmi_result != SPMI_BUS_SUCCESS ) { err = PM_ERR_FLAG__SPMI_OPT_ERR ; }
    }
  }
  else
  {
        err = PM_ERR_FLAG__SPMI_OPT_ERR ;
  }

  return err;
}

pm_err_flag_type  pm_spmi_lite_write_byte_mask
(
  uint32 uSlaveId , 
  uint16 addr, 
  uint8 mask, 
  uint8 value, 
  uint8 priority
)
{
    pm_err_flag_type         err = PM_ERR_FLAG__SUCCESS ;
    SpmiBus_ResultType       spmi_result = SPMI_BUS_SUCCESS;
    uint32                   dataReadLength = 1;
    uint8                    data = 0 ;

    if(FALSE == spmi_initialized)
    {
        // initialize the SPMI BUS
        spmi_result = SpmiBus_Init();

        if(SPMI_BUS_SUCCESS  == spmi_result )
        {
            spmi_initialized = TRUE;
        }
    }

    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG__SPMI_OPT_ERR ;
        }
        else
        {
             /* Read the data from the SPMI and leave all the bits other than the mask to the previous state */
            spmi_result = SpmiBus_ReadLong(uSlaveId, 
                                           (SpmiBus_AccessPriorityType)priority, 
                                           addr, 
                                           &data, 
                                           1, 
                                           &dataReadLength);

            if ( spmi_result == SPMI_BUS_SUCCESS )
            {
                data &= (~mask) ; 
                data |= (value & mask) ;

                spmi_result = SpmiBus_WriteLong(uSlaveId, 
                                                (SpmiBus_AccessPriorityType)priority, 
                                                addr, 
                                                &data, 
                                                1);
         
                /* TODO should we update the shadow register? */
                //pm_image[addr] = *data ;

                if ( spmi_result != SPMI_BUS_SUCCESS ) { err = PM_ERR_FLAG__SPMI_OPT_ERR ; }
            }
        }
    }
    else
    {
        err = PM_ERR_FLAG__SPMI_OPT_ERR ;
    }


    return err ;
}


pm_err_flag_type  pm_spmi_lite_read_byte_array ( uint32 uSlaveId ,
                                                uint16 addr , 
                                                uint32 numBYtes , 
                                                uint8* data, 
                                                uint8 priority) 
{
    pm_err_flag_type     err = PM_ERR_FLAG__SUCCESS ;
    SpmiBus_ResultType   spmi_result = SPMI_BUS_SUCCESS;
    /* Read the data from the SPMI */
    uint32               dataReadLength = 0;

    if(FALSE == spmi_initialized)
    {
        // initialize the SPMI BUS
        spmi_result = SpmiBus_Init();

        if(SPMI_BUS_SUCCESS  == spmi_result )
        {
            spmi_initialized = TRUE;
        }
    }

    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG__SPMI_OPT_ERR ;
        }
        else
        {
            spmi_result = SpmiBus_ReadLong(uSlaveId, 
                                           (SpmiBus_AccessPriorityType)priority, 
                                            addr, 
                                            data, 
                                            numBYtes, 
                                           &dataReadLength);

            /* TODO should we update the shadow register? */
            //pm_image[addr] = *data ;

            if ( spmi_result != SPMI_BUS_SUCCESS ) { err = PM_ERR_FLAG__SPMI_OPT_ERR ; }
        }
    }
    else
    {
        err = PM_ERR_FLAG__SPMI_OPT_ERR ;
    }

    return err ;
}

pm_err_flag_type  pm_spmi_lite_write_byte
(
  uint32 uSlaveId, 
  uint16 addr, 
  uint8 data, 
  uint8 priority
)
{
    pm_err_flag_type     err = PM_ERR_FLAG__SUCCESS ;
    SpmiBus_ResultType   spmi_result = SPMI_BUS_SUCCESS;

    if(FALSE == spmi_initialized)
    {
        // initialize the SPMI BUS
        spmi_result = SpmiBus_Init();

        if(SPMI_BUS_SUCCESS  == spmi_result )
        {
            spmi_initialized = TRUE;
        }
    }

    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG__SPMI_OPT_ERR ;
        }
        else
        {
            spmi_result = SpmiBus_WriteLong(uSlaveId, 
                                           (SpmiBus_AccessPriorityType)priority, 
                                            addr, 
                                            &data, 
                                            1);
     
            /* TODO should we update the shadow register? */
            //pm_image[addr] = *data ;

            if ( spmi_result != SPMI_BUS_SUCCESS ) { err = PM_ERR_FLAG__SPMI_OPT_ERR ; }
        }
    }
    else
    {
        err = PM_ERR_FLAG__SPMI_OPT_ERR ;
    }

    return err ;
}






