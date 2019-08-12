#ifndef FTM_MEAS_H
#define FTM_MEAS_H

/*===========================================================================

               M E A S    S P E C I F I C   D E F I N I T I O N S
                         H E A D E R  F I L E

DESCRIPTION
   This file contains MEAS definitions which are also 
   contained in a file on the PC.  This keeps them in a place where they 
   can be easily modified and updated as necessary to aid in it's maintenance.

   Copyright (c) 2009 - 2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Initially, comments will be entered manually. Later, as the module is
  placed under version control, the version control tool will automatic-
  ally insert change comments into this section. [Delete this comment].

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/meas/ftm/inc/ftm_meas.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/12/13   rmb     Add FTM W2W IRAT support for Bolt.
08/13/13   pl      Added interface to log and retrieve IRAT script info 
04/23/13   gvn     Adde ABORT support
03/06/13   gvn     Change macro definitions to support L2L IRAT Half script 
11/08/12   pl      Consolidate function pointer table
10/24/12   pl      Update ftm framework interface to clarify usage
10/22/12   pl      Enable L2L FTM IRAT
10/17/12   pl      Update signature of RXLM execution interface
10/16/12   ka      Fixed data type 
10/16/12   ka      Added W2W FTM test 
10/09/12   pl      FTM IRAT framework revamp
10/09/12   pl      FTM IRAT cleanup
02/24/11   tnt     Merge to MDM9k PLF110
10/06/10   jyu     Added new APIs to support FTM IRAT test 
09/24/10   kguo    Featurized LTOW IRAT test  
07/27/10   pl      Added support for x2DO test
07/27/10   pl      Added support for x2L test
06/02/10   kguo    Added support for LTOW test 
09/15/09   jps     Initial version

=============================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "rfcom.h"
#include "ftm.h"
#include "rfmeas_types.h"

typedef boolean (*get_src_rxlm_buffer_fp_type)(rfm_meas_generic_params_type *test_param_ptr);
typedef void* (*get_enter_param_fp_type)(rfm_meas_generic_params_type *test_param_ptr);
typedef void* (*get_setup_param_fp_type)(rfm_meas_generic_params_type *test_param_ptr);
typedef void* (*get_exit_param_fp_type)(rfm_meas_generic_params_type *test_param_ptr);
typedef boolean (*enter_meas_fp_type)(rfm_meas_enter_param_type *meas_enter_param);
typedef boolean (*build_script_fp_type)(rfm_meas_setup_param_type *meas_setup_param);
typedef boolean (*meas_fw_enable_fp_type)(void);
typedef boolean (*meas_fw_disable_fp_type)(rfm_meas_generic_params_type *test_param_ptr, boolean disable_src);
typedef boolean (*meas_fw_execute_rf_script_fp_type)(void);
typedef boolean (*meas_fw_execute_rxlm_fp_type)(rfm_meas_generic_params_type *test_param_ptr, boolean execute_src_rxlm);
typedef boolean (*meas_fw_start_rxagc_fp_type)(rfm_meas_generic_params_type *test_param_ptr, boolean start_src_agc);
typedef boolean (*exit_meas_fp_type)(rfm_meas_exit_param_type *meas_exit_param);
typedef boolean (*meas_exit_tech_fp_type)(rfm_meas_generic_params_type *test_param_ptr, boolean use_src_param);
typedef boolean (*meas_enter_tech_fp_type)(rfm_meas_generic_params_type *test_param_ptr, boolean use_src_param);

typedef struct
{
  get_src_rxlm_buffer_fp_type  get_src_rxlm_buffer_fp;
  get_enter_param_fp_type      get_enter_param_fp;
  get_setup_param_fp_type      get_setup_param_fp;
  get_exit_param_fp_type       get_exit_param_fp;
  enter_meas_fp_type           enter_meas_fp;
  build_script_fp_type         build_script_fp;
  meas_exit_tech_fp_type       meas_exit_tech_fp;
  meas_enter_tech_fp_type      meas_enter_tech_fp;
  exit_meas_fp_type            exit_meas_fp;
} ftm_meas_func_tbl_type;

#define FTM_MEAS_TEST_LEVEL_RF_EXECUTE 1
#define FTM_MEAS_TEST_LEVEL_FW_EXECUTE 2
#define FTM_MEAS_TEST_LEVEL_RF_ABORT   3
#define FTM_MEAS_TEST_LEVEL_FULL_SCRIPT FTM_MEAS_TEST_LEVEL_RF_EXECUTE
#define FTM_MEAS_TEST_LEVEL_HALF_SCRIPT FTM_MEAS_TEST_LEVEL_FW_EXECUTE

/*----------------------------------------------------------------------------*/
void ftm_meas_wtow_test_wcdma(uint16 home_buffer, uint16 serv_chan, uint16 nbr_buffer, uint16 meas_chan);

/*----------------------------------------------------------------------------*/
void ftm_meas_wtow_execute_buffer(uint16 buffer_id);

/*----------------------------------------------------------------------------*/
void ftm_meas_wtow_build_scripts(uint16 serv_chan, uint16 meas_chan);

/*----------------------------------------------------------------------------*/
void ftm_meas_wtow_write_scripts(boolean tune_neighbour);

/*----------------------------------------------------------------------------*/
void ftm_meas_xtoy_test_y(rfm_meas_generic_params_type *test_param);

/*----------------------------------------------------------------------------*/
void ftm_meas_xtoy_test_x(rfm_meas_generic_params_type *test_param);

/*----------------------------------------------------------------------------*/
void ftm_meas_init(void);

/*----------------------------------------------------------------------------*/
boolean ftm_meas_register(rfm_mode_enum_type tech, ftm_meas_func_tbl_type *func_tbl_ptr);

/*----------------------------------------------------------------------------*/
boolean ftm_meas_deregister(rfm_mode_enum_type tech);

/*----------------------------------------------------------------------------*/
void ftm_meas_log_irat_script_info(rfm_meas_program_script_type script_type, uint16 num_trx, uint16 num_trx_task, uint16 num_non_trx, uint16 num_non_trx_task, uint16 exec_time_us);

/*----------------------------------------------------------------------------*/
void ftm_meas_get_irat_script_info(rfm_meas_program_script_type script_type, uint16 *num_trx, uint16 *num_trx_task, uint16 *num_non_trx, uint16 *num_non_trx_task, uint16 *exec_time_us);

#endif /* FTM_MEAS_H */
