/*! \file  pmic_sbl_gen_driver.c
 *
 *  \brief  File Contains the PMIC Set Mode Implementation
 *  \details Set Mode implementation is responsible for setting
 *  all mode settings such as Register values, memory values, etc.
 *
 *    This file contains code for Target specific settings and modes.
 *
 *  &copy; Copyright 2013 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/framework/src/pm_config_sbl.c#1 $
$DateTime: 2015/09/01 00:30:35 $  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/14   mr      Removed KW errors (CR-643316)
08/07/13   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

#include "pm_sbl_boot.h"                /* Needed for pm_clk_busy_wait and sbi_mini initialization */
#include "pm_pbs_driver.h"              /* Needed for pm_clk_busy_wait and sbi_mini initialization */
#include "pm_config_sbl.h"                          /* Contains the type definitions and static register tables */
#include "pm_target_information.h"
#include "CoreVerify.h"
#include "pm_version.h"
#include "pm_pbs_info.h"
#include "boot_logger.h" /* For settings validation test */
#include <stdio.h>   /* for snprintf */ /* For settings validation test */


//Arrays are assigned inside the function
static uint16 pbs_data_ram_index_arr[1] = {0};
static uint16 pbs_data_ram_size_arr[1] = {0};
static uint8 max_pmic_index;
static char str[SET_TEST_MAX_STR_LENGTH];

static pm_err_flag_type pm_settings_validation_flag(uint8 *flag, boolean reset);

boolean
pm_sbl_validate_reg_config( uint32                        rev_id,       //Generated from PDM
                             uint32                       rev_id_read,  //Read from PMIC RevID
                             pm_sbl_reg_operation_type    reg_operation,
                             pm_sbl_rev_id_operation_type rev_id_operation
                           )
{
   boolean valid_config_flag = FALSE;

   if(rev_id == REV_ID_COMMON)
   {
      valid_config_flag = TRUE;
   }
   else
   {
      switch(rev_id_operation)
      {
         case EQUAL:
            valid_config_flag = (rev_id_read == rev_id)?TRUE:FALSE;
            break;
         case GREATER:
            valid_config_flag = (rev_id_read > rev_id)?TRUE:FALSE;
            break;
         case GREATER_OR_EQUAL:
            valid_config_flag = (rev_id_read >= rev_id)?TRUE:FALSE;
            break;
         case LESS:
            valid_config_flag = (rev_id_read < rev_id)?TRUE:FALSE;
            break;
         case LESS_OR_EQUAL:
            valid_config_flag = (rev_id_read <= rev_id)?TRUE:FALSE;
            break;
         default:
            valid_config_flag = FALSE;
            break;
      }
   }
   return valid_config_flag;
}

   //Added by Arun only for debugging purpose - TODO remove this code
int g_failure_index = 0;
int g_failed_base = 0;

pm_err_flag_type
pm_sbl_config()
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   pm_err_flag_type err_flag_chip_rev = PM_ERR_FLAG__SUCCESS;
   uint16 pbs_data_index = 0;
   pm_pbs_ram_data_type *pbs_ram_config_ds = NULL;
   pm_sbl_seq_type *sbl_reg_config_ds = NULL;
   pm_sbl_seq_type *rc = NULL;  //reg config data ptr
   boolean valid_sid = FALSE;
   uint32 rev_id_read;

   sbl_reg_config_ds = (pm_sbl_seq_type*)pm_target_information_get_specific_info(PM_PROP_SBL_REG_CONFIG_LUT); //get handle for sbl config data
   if (NULL == sbl_reg_config_ds)
   {
      return PM_ERR_FLAG__INVALID_POINTER;
   }

   for(rc = sbl_reg_config_ds; (rc->reg_operation != PM_SBL_OPERATION_INVALID); rc++)
   {
	   //Added by Arun only for debugging purpose - TODO remove this code
      if (PM_ERR_FLAG__SUCCESS == err_flag)
      {
       g_failure_index++;
      }
      else
      {
        g_failed_base = rc->base_address;
      }


      if (rc->reg_operation != PM_SBL_DELAY ) 
      {
          err_flag_chip_rev |= pm_resolve_chip_revision(rc->sid, &valid_sid, &rev_id_read);
          if (valid_sid == FALSE)
          {
             continue;  //This current configuation does not apply to this SID (PMIC does not exist)
          }
      }

     if( pm_sbl_validate_reg_config(rc->rev_id, rev_id_read, rc->reg_operation, rc->rev_id_operation)== TRUE )
     {
        switch(rc->reg_operation)
        {
           case PM_SBL_WRITE:
              {
                 rc->base_address += rc->offset;
                 err_flag |= pm_comm_write_byte(rc->sid, rc->base_address, rc->data, 1);
              }
              break;

           case PM_SBL_DELAY:
              {
                 pm_clk_busy_wait(rc->offset);
              }
              break;

           case PM_SBL_PBS_RAM:
              {
                pm_pbs_info_data_type **pm_pbs_data = (pm_pbs_info_data_type **)pm_target_information_get_specific_info(PM_PROP_PBS_INFO);
                if(NULL == pm_pbs_data)
                {
                    return PM_ERR_FLAG__INVALID_POINTER;
                }

                /* pm_model_type pm_model = pm_get_pmic_model(0);
                if (pm_model == PMIC_IS_PM8909)
                {
                    pbs_data_ram_index_arr[0] = 0;
                    pbs_data_ram_size_arr[0] = 128;
                }
                if (pm_model == PMIC_IS_PMD9635)
                {
                     pbs_data_ram_index_arr[0] = 0;
                     pbs_data_ram_size_arr[0] = 256;
                } */

                pbs_data_ram_index_arr[0] = 0;
                pbs_data_ram_size_arr[0] = ((pm_pbs_info_data_type *)pm_pbs_data[0])->pbs_mem_size;
                          
                 pbs_data_index = pbs_data_ram_index_arr[rc->data];
                 pbs_ram_config_ds = (pm_pbs_ram_data_type*)pm_target_information_get_specific_info( PM_PROP_PBS_RAM_CONFIG_LUT); 
                 err_flag |= pm_pbs_ram_image_load(rc->sid, &pbs_ram_config_ds[pbs_data_index], pbs_data_ram_size_arr[pbs_data_index]);
                }
              break;

           case PM_SBL_NOP:
              break;

           default:
                 err_flag |= PM_ERR_FLAG__SBL_CONFIG_REG_OPERATION_NOT_SUPPORTED;
              break;
        }
     }
   }

   return err_flag;
}

pm_err_flag_type 
pm_sbl_config_test()
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   pm_err_flag_type err_flag_chip_rev = PM_ERR_FLAG__SUCCESS;
   pm_sbl_seq_type *sbl_reg_config_ds = NULL;
   pm_sbl_seq_type *rc = NULL;  //reg config data ptr
   boolean valid_sid = FALSE;
   uint32 rev_id_read;
   pm_sbl_seq_type *rc_copy = NULL;  
   pm_sbl_seq_type *last_write = NULL; 
   // char str[SET_TEST_MAX_STR_LENGTH]; /* made it global */
   uint8 read_data;
   uint8 flag;
   
   pm_pbs_ram_data_type *pbs_ram_config_ds = NULL;
   
   err_flag |= pm_settings_validation_flag(&flag, 0); // to verify if flag register bit is set to run this test function
   if(flag)
   { 
      
      sbl_reg_config_ds = (pm_sbl_seq_type*)pm_target_information_get_specific_info(PM_PROP_SBL_REG_CONFIG_LUT); //get handle for sbl config data
      pbs_ram_config_ds = (pm_pbs_ram_data_type*)pm_target_information_get_specific_info(PM_PROP_PBS_RAM_CONFIG_LUT); // get handle to PBS RAM configuration info
      
      boot_log_message("PM_SET_VAL:Start");
      for(rc = sbl_reg_config_ds; (rc->reg_operation != PM_SBL_OPERATION_INVALID); rc++)  
      {
         if (rc->reg_operation != PM_SBL_DELAY ) 
         {
            err_flag_chip_rev |= pm_resolve_chip_revision(rc->sid, &valid_sid, &rev_id_read);
            if (valid_sid == FALSE)
            {
               continue;  //This current configuation does not apply to this SID (PMIC does not exist)
            }
         }    
         if( pm_sbl_validate_reg_config(rc->rev_id, rev_id_read, rc->reg_operation, rc->rev_id_operation)== TRUE )
         { 
            switch(rc->reg_operation)  
            {
               case PM_SBL_WRITE:  
               {
                  for(rc_copy = rc; (rc_copy->reg_operation != PM_SBL_OPERATION_INVALID); rc_copy++)
                  {
                     if (rc_copy->sid == rc->sid && rc_copy->base_address == rc->base_address && rc_copy->offset == rc->offset)
                     {
                        last_write = rc_copy; // getting pointer to last write of same register that we are on at current stage
                     }else
                     {
                        continue;
                     }
                  }
                  // snprintf(str,SET_TEST_MAX_STR_LENGTH,"READ_DATA: sid:%d,add:0x%X,offset:0x%X,data:0x%X,lastwrite_data:0x%X",rc->sid, rc->base_address, rc->offset, rc->data, last_write->data);
                  // boot_log_message(str);

                  if ((rc->offset != OFFSET_SEC_ACCESS) && (rc->offset != OFFSET_INT_EN)) // not covering entries with with selected offset of SEC_ACCESS and INT_EN
                  { 
                     err_flag |= pm_comm_read_byte(rc->sid, rc->base_address, &read_data, 1); //address data at original location is being changed in sbl write function so not adding offset in base address

                     if (read_data == last_write->data) // comparing current read data from register and comparing it to last write data from structure 
                     {
                        snprintf(str,SET_TEST_MAX_STR_LENGTH,"PASS:%d,0x%X,set:0x%X,get:0x%X",rc->sid, rc->base_address, rc->data, read_data); /* remove to save some memory */
                     }else
                     {
                        snprintf(str,SET_TEST_MAX_STR_LENGTH,"VIOLATION:%d,0x%X,set:0x%X,get:0x%X",rc->sid, rc->base_address, rc->data, read_data);
                        // boot_log_message(str);
                     }
                      boot_log_message(str);
                  }
               }
                  break;
               case PM_SBL_PBS_RAM:
               {
                  max_pmic_index = rc->data; // reading data section for PM_SBL_PBS_RAM gives particular pmic its intended for and last pmic index stored will give idea on max pmics available.                  
                  /* (pbs_data_ram_size_arr[pmic_device_index]-1) gives PBS size -1 value which can be used to access last line of pbs_ram_config_ds to get RAM version details below */
                  snprintf(str,SET_TEST_MAX_STR_LENGTH,"PBS:%d:0x%X,0x%X",rc->data, (((&pbs_ram_config_ds[rc->data])+(pbs_data_ram_size_arr[rc->data]-1))->word2), (((&pbs_ram_config_ds[rc->data])+(pbs_data_ram_size_arr[rc->data]-1))->word1));            
                  boot_log_message(str);
               }
                  break;
               case PM_SBL_NOP:
                  break;
               default:
                  err_flag |= PM_ERR_FLAG__SBL_CONFIG_REG_OPERATION_NOT_SUPPORTED;
                  break;
            }
         }
      }
      boot_log_message("PM_SET_VAL:End");
   }else
   {
      boot_log_message("PM_SET_VAL:Skip");
   } 
   return err_flag;
}

pm_err_flag_type pm_pbs_ram_version_validation_test()
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   pm_pbs_info_type pbs_info_ptr;
   uint8 pmic_device_index=0;
   uint8 flag;
   // char str[SET_TEST_MAX_STR_LENGTH]; made it global
   
   err_flag |= pm_settings_validation_flag(&flag, 1); // to verify if flag register bit is set to run this test function
   if(flag){
      while (pmic_device_index <= max_pmic_index)
      {
         err_flag |= pm_get_pbs_info(pmic_device_index, &pbs_info_ptr); 
         snprintf(str,SET_TEST_MAX_STR_LENGTH,"PBS_GET:%d:0x%X",pmic_device_index, pbs_info_ptr.ram_version);
         boot_log_message(str);
         pmic_device_index++;
      }
   }
   return err_flag;
}

pm_err_flag_type pm_settings_validation_flag(uint8 *flag, boolean reset)
{
   pm_settings_test_flag_reg_info_type* flag_reg_ptr=NULL;
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   // uint8 sid = 0; /* removed to save some memory */
   // uint8 mask=0x01; /* removed to save some memory */
   *flag=0x00; 

   /* handle to structure where test flag register is stored */
   flag_reg_ptr = (pm_settings_test_flag_reg_info_type*)pm_target_information_get_common_info(PM_PROP_SETTINGS_TEST_FLAG_REG);

   if (flag_reg_ptr)
   { 
      err_flag |= pm_comm_read_byte_mask(0, flag_reg_ptr->flag_register, 0x01, flag, 1); // read bit 0 of register retrived to decide  whether test needs to be triggered or not
   }
   if (reset) 
   {
      err_flag |= pm_comm_write_byte_mask(0, flag_reg_ptr->flag_register, 0x01, 0x00, 1); // reset flag for validation test
   }    
   return err_flag;
}
