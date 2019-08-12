#ifndef _PM_NCP_H

#define _PM_NCP_H
/*! \file 
 *  \n
 *  \brief  pm_ncp.h ---- PMIC-NCP MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
 *   the PMIC negative charge pump driver module.
 *
 *
 *  \n &copy; Copyright (c) 2009 - 2011  by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_ncp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
06/20/09   jtn     New file
===========================================================================*/

#include "pm_lib_cmd.h"
#include "pm_lib_err.h"

/* NCP switching frequency */
typedef enum
{
    PM_NCP_FREQ_9_6MHZ,
    PM_NCP_FREQ_4_8MHZ,
    PM_NCP_FREQ_3_2MHZ,  
    PM_NCP_FREQ_2_4MHZ,
    PM_NCP_FREQ_1_92MHZ,
    PM_NCP_FREQ_1_6MHZ,   /* Default */
    PM_NCP_FREQ_1_37MHZ,
    PM_NCP_FREQ_1_2MHZ, 
    PM_NCP_FREQ_OUT_OF_RANGE 
}pm_ncp_switching_freq_type;



/*===========================================================================
FUNCTION pm_vreg_ncp_sample_comp_mode_enable                    EXTERNAL FUNCTION

DESCRIPTION
  Use this API to enable the sample-comparator mode for VREG_NCP. This function will 
  enable the comparator mode and allow us to set the switching frequency.
                                          
PARAMETERS
  1) Parameter name: OnOff
     - Enable or Disable the sample comparator output.

     Parameter type: pm_switch_cmd_type (enum).
     - Valid Inputs:
     
            PM_OFF_CMD
            PM_ON_CMD

  2) Parameter name: freq
     - Select switching frequency.

     Parameter type: pm_ncp_switching_freq_type (enum).
     - Valid Inputs:
            PM_NCP_FREQ_9_6MHZ,
            PM_NCP_FREQ_4_8MHZ,
            PM_NCP_FREQ_3_2MHZ,  
            PM_NCP_FREQ_2_4MHZ,
            PM_NCP_FREQ_1_92MHZ,
            PM_NCP_FREQ_1_6MHZ, //Default
            PM_NCP_FREQ_1_37MHZ,
            PM_NCP_FREQ_1_2MHZ,
   

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
      PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.  
      PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
      PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                          version of the PMIC.          
      PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                          with the PMIC.      
      PM_ERR_FLAG__SUCCESS              = SUCCESS.
        
DEPENDENCIES
  The following function must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disable while communicating with the PMIC.
===========================================================================*/
pm_err_flag_type pm_vreg_ncp_sample_comp_mode_enable(
                                             pm_switch_cmd_type           OnOff,
                                             pm_ncp_switching_freq_type   freq);


/**
* @brief Enables/disables the NCP functionality
* 
* @details
*   
* @param on_off - PM_ON_CMD  Enables the NCP functionality; PM_OFF_CMD otherwise               
* 
* @return     pm_err_flag_type 
*/
pm_err_flag_type pm_ncp_control(pm_switch_cmd_type on_off);


#endif
