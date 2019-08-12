/*============================================================================

FILE: sns_qdsp_playback.cpp

Contains implementation of Sensors Playback Test Manager.

Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Confidential and Proprietary

============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-06-11  ae   Initial version

============================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include "simulation.h"
#include "sns_qdsp_playback.h"

extern "C" {
#include "qmi_client.h"
#include "qmi_idl_lib_internal.h"
#include "sam_qdsp_playback_clnt.h"
#include "sns_smgr_api_v01.h"
#include "sns_sam_amd_v01.h"
#include "sns_common_v01.h"
#include "sns_init.h"
#include "sns_debug.h"
extern void smr_set_qmi_service_obj (void);
extern sns_err_code_e sns_debug_test_init2(void);
extern sns_err_code_e sns_debug_test_init2(void);
extern sns_err_code_e sns_debug_test_deinit2(void);   
}



static pb_algo_attrib_s pb_algo_svc[MAX_ALGO];
static int pb_algo_count = 0;
int sns_playback_parse_config(char *fname);
void sns_plbk_simulate_algo(pb_algo_attrib_s *algo_attr);
int sns_playback_simulation_status();




static int sns_playback_get_simulation_status()
{
    FILE *status_fp = NULL;
    int status;
            
    status_fp = fopen(PLAYBACK_STATUS_FILE, "r");
    if (!status_fp) {
        printf("PLAYBACK: Cannot open %s\n", PLAYBACK_STATUS_FILE);
        return 0;
    }

    fscanf(status_fp, "%d", &status);
    fclose(status_fp);
    return status;
}


/*=========================================================================
  FUNCTION: sns_playback_simulate_algo
=========================================================================*/
/*!
    @brief
    Enable's a SAM Algorithm by sending enable message, waites until the
    simulation is done and sends disable message.
*/
/*=======================================================================*/
void sns_playback_simulate_algo(pb_algo_attrib_s *algo_attr)
{
    qmi_client_type clnt;
    int err;

    switch(algo_attr->svc_id)
    {
        case SNS_SAM_AMD_SVC_ID_V01:
            printf("PLAYBACK: AMD Simulation requested\n");
            err = sns_plbk_sam_amd_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_RMD_SVC_ID_V01:
            printf("PLAYBACK: RMD Simulation requested\n");
            err = sns_plbk_sam_rmd_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_FNS_SVC_ID_V01:
            printf("PLAYBACK: FNS Simulation requested\n");
            err = sns_plbk_sam_fns_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_BTE_SVC_ID_V01:
            printf("PLAYBACK: BTE Simulation requested\n");
            err = sns_plbk_sam_bte_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_MAG_CAL_SVC_ID_V01:
            printf("PLAYBACK: MAG_CAL Simulation requested\n");
            err = sns_plbk_sam_mag_cal_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
            printf("PLAYBACK: FILTERED_MAG Simulation requested\n");
            err = sns_plbk_sam_filtered_mag_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
            printf("PLAYBACK: ROTATION_VECTOR Simulation requested\n");
            err = sns_plbk_sam_rotation_vector_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_QUATERNION_SVC_ID_V01:
            printf("PLAYBACK: QUATERNION Simulation requested\n");
            err = sns_plbk_sam_quaternion_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
            printf("PLAYBACK: GRAVITY_VECTOR Simulation requested\n");
            err = sns_plbk_sam_gravity_vector_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_ORIENTATION_SVC_ID_V01:
            printf("PLAYBACK: ORIENTATION Simulation requested\n");
            err = sns_plbk_sam_orientation_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
            printf("PLAYBACK: BASIC_GESTURES Simulation requested\n");
            err = sns_plbk_sam_basic_gestures_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_TAP_SVC_ID_V01:
            printf("PLAYBACK: TAP Simulation requested\n");
            err = sns_plbk_sam_tap_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_FACING_SVC_ID_V01:
            printf("PLAYBACK: FACING Simulation requested\n");
            err = sns_plbk_sam_facing_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
            printf("PLAYBACK: INTEG_ANGLE Simulation requested\n");
            err = sns_plbk_sam_integ_angle_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_GYRO_TAP2_SVC_ID_V01:
            printf("PLAYBACK: GYRO_TAP2 Simulation requested\n");
            err = sns_plbk_sam_gyro_tap2_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_GYROBUF_SVC_ID_V01:
            printf("PLAYBACK: GYROBUF Simulation requested\n");
            err = sns_plbk_sam_gyrobuf_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_GYROINT_SVC_ID_V01:
            printf("PLAYBACK: GYROINT Simulation requested\n");
            err = sns_plbk_sam_gyroint_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_PED_SVC_ID_V01:
            printf("PLAYBACK: PED Simulation requested\n");
            err = sns_plbk_sam_ped_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_PAM_SVC_ID_V01:
            printf("PLAYBACK: PAM Simulation requested\n");
            err = sns_plbk_sam_pam_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_SMD_SVC_ID_V01:
            printf("PLAYBACK: SMD Simulation requested\n");
            err = sns_plbk_sam_smd_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_CMC_SVC_ID_V01:
            printf("PLAYBACK: CMC Simulation requested\n");
            err = sns_plbk_sam_cmc_enable(algo_attr->algo_config, &clnt);
            break;
        case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
            printf("PLAYBACK:  Simulation requested\n");
            err = sns_plbk_sam_game_rotation_vector_enable(algo_attr->algo_config, &clnt);
            break;
        default:
            return;
    }

    if (err !=0) {
        printf("PLAYBACK: Simulation failed\n");
        return;
    }

    while (sns_playback_get_simulation_status() != PLAYBACK_SIM_DONE) {
        Sleep(60000);
    }

    switch(algo_attr->svc_id)
    {
        case SNS_SAM_AMD_SVC_ID_V01:
            sns_plbk_sam_amd_disable(&clnt);
            break;
        case SNS_SAM_RMD_SVC_ID_V01:
            sns_plbk_sam_rmd_disable(&clnt);
            break;
        case SNS_SAM_FNS_SVC_ID_V01:
            sns_plbk_sam_fns_disable(&clnt);
            break;
        case SNS_SAM_BTE_SVC_ID_V01:
            sns_plbk_sam_bte_disable(&clnt);
            break;
        case SNS_SAM_MAG_CAL_SVC_ID_V01:
            sns_plbk_sam_mag_cal_disable(&clnt);
            break;
        case SNS_SAM_FILTERED_MAG_SVC_ID_V01:
            sns_plbk_sam_filtered_mag_disable(&clnt);
            break;
        case SNS_SAM_ROTATION_VECTOR_SVC_ID_V01:
            sns_plbk_sam_rotation_vectore_disable(&clnt);
            break;
        case SNS_SAM_QUATERNION_SVC_ID_V01:
            sns_plbk_sam_quaternion_disable(&clnt);
            break;
        case SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01:
            sns_plbk_sam_gravity_vector_disable(&clnt);
            break;
        case SNS_SAM_ORIENTATION_SVC_ID_V01:
            sns_plbk_sam_orientation_disable(&clnt);
            break;
        case SNS_SAM_BASIC_GESTURES_SVC_ID_V01:
            sns_plbk_sam_basic_gestures_disable(&clnt);
            break;
        case SNS_SAM_FACING_SVC_ID_V01:
            sns_plbk_sam_facing_disable(&clnt);
            break;
        case SNS_SAM_INTEG_ANGLE_SVC_ID_V01:
            sns_plbk_sam_integ_angle_disable(&clnt);
            break;
        case SNS_SAM_GYRO_TAP2_SVC_ID_V01:
            sns_plbk_sam_gyro_tap2_disable(&clnt);
            break;
        case SNS_SAM_GYROBUF_SVC_ID_V01:
            sns_plbk_sam_gyrobuf_disable(&clnt);
            break;
        case SNS_SAM_GYROINT_SVC_ID_V01:
            sns_plbk_sam_gyroint_disable(&clnt);
            break;
        case SNS_SAM_PED_SVC_ID_V01:
            sns_plbk_sam_ped_disable(&clnt);
            break;
        case SNS_SAM_PAM_SVC_ID_V01:
            sns_plbk_sam_pam_disable(&clnt);
            break;
        case SNS_SAM_SMD_SVC_ID_V01:
            sns_plbk_sam_smd_disable(&clnt);
            break;
        case SNS_SAM_CMC_SVC_ID_V01:
            sns_plbk_sam_cmc_disable(&clnt);
            break;
        case SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01:
            sns_plbk_sam_game_rotation_vector_disable(&clnt);
            break;
        default:
            return;
    }
    printf("PLAYBACK: Simulation ended\n");
}


static int playback_get_algo_svc_id(char *str)
{
   if (!strcmp(str, "AMD")) {
      return SNS_SAM_AMD_SVC_ID_V01;
   } else if (!strcmp(str, "RMD")) {
      return SNS_SAM_RMD_SVC_ID_V01;
   } else if (!strcmp(str, "VMD")) {
      return SNS_SAM_VMD_SVC_ID_V01;
   } else if (!strcmp(str, "FNS")) {
      return SNS_SAM_FNS_SVC_ID_V01;
   } else if (!strcmp(str, "BTE")) {
      return SNS_SAM_BTE_SVC_ID_V01;
   } else if (!strcmp(str, "QUAT")) {
      return SNS_SAM_QUAT_SVC_ID_V01;
   } else if (!strcmp(str, "GRAVITY")) {
      return SNS_SAM_GRAVITY_SVC_ID_V01;
   } else if (!strcmp(str, "MAG_CAL")) {
      return SNS_SAM_MAG_CAL_SVC_ID_V01;
   } else if (!strcmp(str, "FILTERED_MAG")) {
      return SNS_SAM_FILTERED_MAG_SVC_ID_V01;
   } else if (!strcmp(str, "ROTATION_VECTOR")) {
      return SNS_SAM_ROTATION_VECTOR_SVC_ID_V01;
   } else if (!strcmp(str, "QUATERNION")) {
      return SNS_SAM_QUATERNION_SVC_ID_V01;
   } else if (!strcmp(str, "GRAVITY_VECTOR")) {
      return SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01;
   } else if (!strcmp(str, "ORIENTATION")) {
      return SNS_SAM_ORIENTATION_SVC_ID_V01;
   } else if (!strcmp(str, "BASIC_GESTURES")) {
      return SNS_SAM_BASIC_GESTURES_SVC_ID_V01;
   } else if (!strcmp(str, "FACING")) {
      return SNS_SAM_FACING_SVC_ID_V01;
   } else if (!strcmp(str, "INTEG_ANGLE")) {
      return SNS_SAM_INTEG_ANGLE_SVC_ID_V01;
   } else if (!strcmp(str, "GYRO_TAP")) {
      return SNS_SAM_GYRO_TAP_SVC_ID_V01;
   } else if (!strcmp(str, "GYRO_TAP2")) {
      return SNS_SAM_GYRO_TAP2_SVC_ID_V01;
   } else if (!strcmp(str, "GYROBUF")) {
      return SNS_SAM_GYROBUF_SVC_ID_V01;
   } else if (!strcmp(str, "GYROINT")) {
      return SNS_SAM_GYROINT_SVC_ID_V01;
   } else if (!strcmp(str, "PED")) {
      return SNS_SAM_PED_SVC_ID_V01;
   } else if (!strcmp(str, "PAM")) {
      return SNS_SAM_PAM_SVC_ID_V01;
   } else if (!strcmp(str, "SMD")) {
      return SNS_SAM_SMD_SVC_ID_V01;
   } else if (!strcmp(str, "CMC")) {
      return SNS_SAM_CMC_SVC_ID_V01;
   } else if (!strcmp(str, "DISTANCE_BOUND")) {
      return SNS_SAM_DISTANCE_BOUND_SVC_ID_V01;
   } else if (!strcmp(str, "GAME_ROTATION_VECTOR")) {
      return SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01;
   } else {
      return -1;
   }
}

/*=========================================================================
  FUNCTION: sns_playback_parse_config
=========================================================================*/
/*!
    @brief
    Parses playback config file.
*/
/*=======================================================================*/
static int sns_playback_parse_config(char *fname)
{
    char line[256];
    char *pch;
    char key[60], val[60];
    int algo_svc_id;
    FILE *fp_config;

    if ((fp_config = fopen(fname, "r")) == NULL) {
        printf("Error: Cannot open %s\n", fname);
        return(-1);
    }

    while (fgets(line, sizeof(line), fp_config) != NULL)
    {
        int nchar;
        /* skip comments */
        if ((pch = strstr(line, "//"))) {
            continue;
        }

        if (sscanf(line, "%s %s %n", key, val, &nchar) == 2) {
         
          if (!strcmp(key, "ALGO_SVC")) {
              algo_svc_id = playback_get_algo_svc_id(val);
            if (algo_svc_id != -1) {
               int config_param;
               int num_elem = 0;
               pb_algo_svc[pb_algo_count].svc_id = algo_svc_id;
               pb_algo_svc[pb_algo_count].num_config_elem = 0;

               /* fill in pb_algo_svc[pb_algo_count].algo_config */
               pch = line + nchar;  /* skip directly to algo_config part */
               while (sscanf(pch, "%d%n", &config_param, &nchar) == 1)
               {
                  pb_algo_svc[pb_algo_count].algo_config[num_elem] = config_param;
                  printf("PLAYBACK: algo_config[%d]: %d\n", num_elem, config_param);
                  num_elem++;
                  pch += nchar;
               }

               pb_algo_svc[pb_algo_count].num_config_elem = num_elem;
               if (num_elem < 1) {
                  printf("PLAYBACK: algo should take at least one parameter\n");
                  return(-1);
               }

               pb_algo_count++;
               if (pb_algo_count > MAX_ALGO) {
                  printf("PLAYBACK: exceeds maximum number of algorithms\n");
                  return(-1);
               }
            }
         }
      }
    }
    
    if (pb_algo_count == 0) {
        printf("Error: No Algorithm specified\n");
        return(-1);
      }
    fclose(fp_config);
    return 0;
}


/*=========================================================================
  FUNCTION:  main            
=========================================================================*/
/*!
    @brief
    Main function for co-sim, parses the config file and simulates the specified
    SAM Algorithm.
*/
/*=======================================================================*/
int main(int argc, char** argv)
{
    OAL_Signal dsp_ready;
    FILE *status_fp = NULL;
    int err;
    
    err = sns_playback_parse_config(PLAYBACK_CONFIG_FILE);
    if(err !=0) {
        return 0;
    }
    
    /* Create a status file and set status to zero */
    status_fp = fopen(PLAYBACK_STATUS_FILE, "w");
    fprintf(status_fp, "%d", 0);
    fclose(status_fp);

        
    // Only 1 simulation object!!
    // Initialize the simulation
    simulation::CreateSimulation(0,argc,argv);
    

    // Run the simulation
    // We do not want to start a new thread
    // And we DO want to run the tests
    //simulation::get_simulation_ptr()->run(true, true);
    OAL_SignalInit(&dsp_ready);
    simulation::get_simulation_ptr()->run(true, false, dsp_ready);
    OAL_SignalWait(&dsp_ready);

    smr_set_qmi_service_obj();
    sns_reg_init();
    sns_debug_test_init2();
    Sleep(SNS_DIAG_LOGMASK_DELAY_USEC/1000);

   // Simulate the Algorithm 
    sns_playback_simulate_algo(&pb_algo_svc[0]);
    
    sns_debug_test_deinit2();
    
    return 0;
}
