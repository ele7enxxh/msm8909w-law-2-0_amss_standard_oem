/*=============================================================================
  @file sensor1_route.c

  @brief Defines the types used in the sensor1 remoting library for sensor
  service communication.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "qmi_idl_lib.h"

#include "sns_common_v01.h"
#include "sns_smgr_api_v01.h"
#include "sns_sam_amd_v01.h"
#include "sns_sam_rmd_v01.h"
#include "sns_sam_bte_v01.h"
#include "sns_sam_quaternion_v01.h"
#include "sns_sam_gravity_vector_v01.h"
#include "sns_reg_api_v02.h"
#include "sns_sam_rotation_vector_v01.h"
#include "sns_sam_filtered_mag_v01.h"
#include "sns_sam_mag_cal_v01.h"
#include "sns_sam_sensor_thresh_v01.h"
#include "sns_sam_orientation_v01.h"
#include "sns_time_api_v02.h"
#include "sns_sam_sensor_thresh_v01.h"
#include "sns_sam_basic_gestures_v01.h"
#include "sns_sam_tap_v01.h"
#include "sns_sam_facing_v01.h"
#include "sns_sam_gyroint_v01.h"
#include "sns_sam_ped_v01.h"
#include "sns_sam_cmc_v02.h"
#include "sns_sam_distance_bound_v01.h"
#include "sns_sam_smd_v01.h"
#include "sns_sam_game_rotation_vector_v01.h"
#include "sns_sam_tilt_detector_v01.h"
#include "sns_smgr_restricted_api_v01.h"
#include "sns_sam_dpc_v02.h"
#include "sns_sam_event_gated_sensor_v01.h"
#include "sns_sam_qheart_v01.h"
#include "sns_sam_fast_amd_v01.h"
#include "sns_sam_tilt_w_detector_v01.h"
#include "sns_qfp_v01.h"
#include "sns_sam_iod_v01.h"
#include "sns_sam_multishake_v01.h"
#include "sns_sam_dev_ori_v01.h"
#include "sns_oem_2_v01.h"
#include "sns_sam_pmd_v01.h"

/*============================================================================
  Type Declarations
  ============================================================================*/
typedef struct libsensor_svc_accessor
{
  qmi_idl_service_object_type (*get_svc)(int32_t, int32_t, int32_t);
  int32_t maj_ver;
  int32_t min_ver;
  int32_t tool_ver;
} libsensor_svc_accessor;

/*============================================================================
  Static Variable Definitions
  ============================================================================*/
#define SNS_GET_SVC_OBJ( svc_name, version )                            \
  {   SNS_##svc_name##_SVC_get_service_object_internal_v##version,      \
      SNS_##svc_name##_SVC_V##version##_IDL_MAJOR_VERS,                 \
      SNS_##svc_name##_SVC_V##version##_IDL_MINOR_VERS,                 \
      SNS_##svc_name##_SVC_V##version##_IDL_TOOL_VERS }


const libsensor_svc_accessor svc_map[] = {
  SNS_GET_SVC_OBJ( SMGR, 01),           /* 0 */
  { NULL,0,0,0 },                       /* 1 */
  { NULL,0,0,0 },                       /* 2 */
  { NULL,0,0,0 },                       /* 3 */
  SNS_GET_SVC_OBJ( SAM_AMD, 01),        /* 4 */
  SNS_GET_SVC_OBJ( SAM_RMD, 01),        /* 5 */
  { NULL,0,0,0 },                       /* 6 */
  { NULL,0,0,0 },                       /* 7 */
  { NULL,0,0,0 },                       /* 8 */
  { NULL,0,0,0 },                       /* 9 */
  SNS_GET_SVC_OBJ( SAM_BTE, 01),        /* 10 */
  { NULL,0,0,0 },                       /* 11 */
  { NULL,0,0,0 },                       /* 12 */
  { NULL,0,0,0 },                       /* 13 */
  { NULL,0,0,0 },                       /* 14 */
  SNS_GET_SVC_OBJ( REG2, 02),           /* 15 */
  { NULL,0,0,0 },                       /* 16 */
  SNS_GET_SVC_OBJ( SAM_FILTERED_MAG, 01),   /* 17 */
  SNS_GET_SVC_OBJ( SAM_ROTATION_VECTOR, 01),/* 18 */
  SNS_GET_SVC_OBJ( SAM_QUATERNION, 01),     /* 19 */
  SNS_GET_SVC_OBJ( SAM_GRAVITY_VECTOR, 01), /* 20 */
  SNS_GET_SVC_OBJ( SAM_SENSOR_THRESH, 01),  /* 21 */
  { NULL,0,0,0 },                           /* 22 */
  SNS_GET_SVC_OBJ( SAM_ORIENTATION, 01),    /* 23 */
  SNS_GET_SVC_OBJ( TIME2, 02),              /* 24 */
  SNS_GET_SVC_OBJ( SAM_BASIC_GESTURES, 01), /* 25 */
  { NULL,0,0,0 },                           /* 26 */
  SNS_GET_SVC_OBJ( SAM_FACING, 01),         /* 27 */
  { NULL,0,0,0 },                           /* 28 */
  { NULL,0,0,0 },                            /* 29 */
  { NULL,0,0,0 },                            /* 30 */
  { NULL,0,0,0 },                            /* 31 */
  SNS_GET_SVC_OBJ( OEM_2, 01),               /* 32 */
  { NULL,0,0,0 },                            /* 33 */
  { NULL,0,0,0 },                            /* 34 */
  SNS_GET_SVC_OBJ( SAM_GYROINT, 01),         /* 35 */
  { NULL,0,0,0 },                            /* 36 */
  SNS_GET_SVC_OBJ( SAM_PED, 01),             /* 37 */
  { NULL,0,0,0 },                            /* 38 */
  { NULL,0,0,0 },                            /* 39 */
  SNS_GET_SVC_OBJ( SAM_SMD, 01),             /* 40 */
  SNS_GET_SVC_OBJ( SAM_CMC, 02),             /* 41 */
  SNS_GET_SVC_OBJ( SAM_DISTANCE_BOUND, 01),  /* 42 */
  SNS_GET_SVC_OBJ( SAM_GAME_ROTATION_VECTOR, 01), /* 43 */
  SNS_GET_SVC_OBJ( SMGR_RESTRICTED, 01),     /* 44 */
  { NULL,0,0,0 },                            /* 45 */
  SNS_GET_SVC_OBJ( SAM_TILT_DETECTOR, 01),   /* 46 */
  { NULL,0,0,0 },                            /* 47 */
  { NULL,0,0,0 },                            /* 48 */
  SNS_GET_SVC_OBJ( SAM_EVENT_GATED_SENSOR, 01),  /* 49 */
  { NULL,0,0,0 },                            /* 50 */
  SNS_GET_SVC_OBJ( SAM_QHEART, 01),          /* 51 */
  SNS_GET_SVC_OBJ( SAM_FAST_AMD, 01),        /* 52 */
  SNS_GET_SVC_OBJ( SAM_TILT_W_DETECTOR, 01), /* 53 */
  SNS_GET_SVC_OBJ( SAM_IOD, 01),             /* 54 */
  { NULL,0,0,0 },                            /* 55 */
  SNS_GET_SVC_OBJ( QFP, 01),                 /* 56 */
  SNS_GET_SVC_OBJ( SAM_MULTISHAKE, 01),      /* 57 */
  SNS_GET_SVC_OBJ( SAM_DPC, 02),             /* 58 */
  { NULL,0,0,0 },                            /* 59 */
  { NULL,0,0,0 },                            /* 60 */
  { NULL,0,0,0 },                            /* 61 */
  SNS_GET_SVC_OBJ( SAM_DEV_ORI, 01),         /* 62 */
  SNS_GET_SVC_OBJ( SAM_PMD, 01),             /* 63 */
};

const uint32_t SVC_MAP_LEN = (sizeof(svc_map) / sizeof(libsensor_svc_accessor)) - 1;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

qmi_idl_service_object_type
get_qmi_svc_obj( uint32_t svc_num )
{
  qmi_idl_service_object_type rv;
  if( svc_num > SVC_MAP_LEN ) {
    return NULL;
  } else if( NULL == svc_map[ svc_num ].get_svc ) {
    return NULL;
  }

  rv = svc_map[ svc_num ].get_svc(
        svc_map[ svc_num ].maj_ver,
        svc_map[ svc_num ].min_ver,
        svc_map[ svc_num ].tool_ver );
  return rv;
}
