#ifndef TCXOMGR_CAL_I_H
#define TCXOMGR_CAL_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

             T C X O    M A N A G E R    C A L I B R A T I O N

               TCXO Manager Calibration Internal Header File

GENERAL DESCRIPTION

  This module contains the functions that are used for factory calibration
  and field calibration to determine the coefficients for the calibrated
  XO freq vs. Temp curve

EXTERNALIZED FUNCTIONS



REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS



  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/inc/tcxomgr_cal_i.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who     what, where, why
--------    ---     --------------------------------------------------------- 
01/30/15    tl      Remove obsolete featurization
05/01/14    tl      Update c2 and c3 limits for polynomial fit calibration
02/21/14    tl      Store calibration data as saved in flash
07/16/13    mn      Dynamic C0 uncertainty in coarse cal state.
07/16/13    mn      XO Cal failure when negative freq offset is used.
07/01/13    sg      Using mid-range value for default C1
06/12/13    mn      Adding CNo to the GPS XO cal interface.
04/01/13    sg      Merged fail reason enum to a single list
10/29/12    sk      Support for exposing the freq error estimation 
                      api for FTM tests 
09/13/12    sk      field_update_rgs function parameters changed 
07/09/12    sg      Support for crystal aging
04/23/12    sg      Support for factory fine cal removal
02/10/12    sr      TCXOMGR Feature Cleanup
01/27/12    ns      Support for IFC 
01/15/12    sg      support for fast field cal v3.1 
11/01/11    mn      Added min_phase_diff and max_phase_diff elements to the
                    tcxomgr_cal_data_type structure.
11/03/11    sg      Added support for TDSCDMA XO Cal 
02/03/10    ns      Fast field cal support for 2.5 degree c1 calibration 
29/11/10    hh      Initial Version. CMI phase-4 changes.
============================================================================*/
/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "mcs_variation.h"
#include "customer.h"

#include "comdef.h"
#include "tcxomgr.h"
#include "tcxomgr_cal.h"

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/*============================================================================

                   Defines

============================================================================*/ 

#define CAL_NV_DEFAULT_VAL_C0     0         /* Default value for C0         */
#define CAL_NV_DEFAULT_VAL_C1    -4194304   /* Default value for C1         */
#define CAL_NV_DEFAULT_VAL_D21    146       /* Default value for D21        */
#define CAL_NV_DEFAULT_VAL_D20    120090    /* Default value for D20        */
#define CAL_NV_DEFAULT_VAL_D31   -110       /* Default value for D31        */
#define CAL_NV_DEFAULT_VAL_D30    274277    /* Default value for D30        */
#define CAL_NV_DEFAULT_VAL_T0     32768     /* Default value for T0         */
#define CAL_NV_DEFAULT_VAL_P      16384     /* Default value for P          */
#define CAL_NV_DEFAULT_VAL_T0P    46080     /* Default value for T0P        */
#define CAL_NV_DEFAULT_VAL_FT_QUAL_IND TCXOMGR_CAL_FT_QUAL_NOT_CAL
                                            /* Default value for FT_QUAL_IND*/
#define CAL_MID_RANGE_C1         CAL_NV_DEFAULT_VAL_C1   
                                            /* Midrange value for C1        */
#define TCXOMGR_C1_HIGH_LIMIT    -1000000 	/* Upperrange value for C1 		*/
#define TCXOMGR_C1_LOW_LIMIT     -8000000	/* Lowerrange value for C1 		*/

#define TCXOMGR_FIELD_C2_LOW_THRESHOLD -134218  /* minimum c2 value =
                                                   -5e-4 * 2^28 = -134218 */
#define TCXOMGR_FIELD_C2_HIGH_THRESHOLD 134218  /* maximum c2 value =
                                                    5e-4 * 2^28 =  134218 */

#define TCXOMGR_FIELD_C3_LOW_THRESHOLD  343597  /* minimum c3 value =
                                                    8e-5 * 2^32 = 343597 */
#define TCXOMGR_FIELD_C3_HIGH_THRESHOLD 515396  /* maximum c3 value =
                                                   12e-5 * 2^32 = 515396 */

#define CAL_IQ_SAMP_SIZE          4096      /* Size of the sram capture     */


/*============================================================================

                   Debugging

============================================================================*/


#define LOW        MSG_LEGACY_LOW
#define MED        MSG_LEGACY_MED
#define HIGH       MSG_LEGACY_HIGH
#define ERROR      MSG_LEGACY_ERROR
#define FATAL      MSG_LEGACY_FATAL

#define DBG_LOW    MSG_LEGACY_LOW
#define DBG_MED    MSG_LEGACY_LOW
#define DBG_HIGH   MSG_DBG_FAC_CAL

#define MIN_MSG_LOGGED MSG_LEGACY_HIGH




#define CAL_MSG( lvl, fmt )                                                   \
          MSG( MSG_SSID_TCXOMGR, lvl, fmt );

#define CAL_MSG_1( lvl, fmt, a1 )                                             \
          MSG_1( MSG_SSID_TCXOMGR, lvl, fmt, a1 );

#define CAL_MSG_2( lvl, fmt, a1, a2 )                                         \
          MSG_2( MSG_SSID_TCXOMGR, lvl, fmt, a1, a2 );

#define CAL_MSG_3( lvl, fmt, a1, a2, a3 )                                     \
          MSG_3( MSG_SSID_TCXOMGR, lvl, fmt, a1, a2, a3 );

#define CAL_MSG_4( lvl, fmt, a1, a2, a3, a4 )                                 \
          MSG_4( MSG_SSID_TCXOMGR, lvl, fmt, a1, a2, a3, a4 );

#define CAL_MSG_5( lvl, fmt, a1, a2, a3, a4, a5 )                             \
          MSG_5( MSG_SSID_TCXOMGR, lvl, fmt, a1, a2, a3, a4, a5 );


/*============================================================================

                   Structs, Enums

============================================================================*/

/* indicator for NV values of the quality of the coefficients
   this value must stay in this order to match the NV item's enum */
typedef enum
{
  TCXOMGR_CAL_NV_FT_QUAL_NOT_CAL,                /* uncalibrated             */
  TCXOMGR_CAL_NV_FT_QUAL_FAC1,                   /* 56 PPM mode              */
  TCXOMGR_CAL_NV_FT_QUAL_COARSE,                 /* Coarse Calibrated        */
  TCXOMGR_CAL_NV_FT_QUAL_FINE,                   /* Fine   Calibrated        */
  TCXOMGR_CAL_NV_FT_QUAL_IFC,                    /* IFC Calibrated           */
  TCXOMGR_CAL_NV_FT_QUAL_FT1,                    /* 5 PPM mode               */
  TCXOMGR_CAL_NV_FT_QUAL_FT2,                    /* 3 PPM mode               */
  TCXOMGR_CAL_NV_FT_QUAL_FAILED,                 /* 56 PPM mode              */
} tcxomgr_cal_nv_ft_qual_enum_type;

typedef enum
{
  TCXOMGR_CAL_TRIM_TUNE_COARSE,                  /*  5 PPM                   */
  TCXOMGR_CAL_TRIM_TUNE_FINE,                    /*  2 PPM                   */
} tcxomgr_cal_trim_tune_type;

typedef enum
{
  TCXOMGR_FASTFIELD_C1_CALC_FAIL,
  TCXOMGR_FASTFIELD_C1_NORM_STDEV_FAIL,
  TCXOMGR_FASTFIELD_C1_FAIL_NONE
}tcxomgr_fastfield_c1_fail_type;

/*---------------------------------------------------------------------------
  TCXO Manager FT Table Types
---------------------------------------------------------------------------*/

typedef enum
{
  TCXOMGR_TABLE_WWAN,
  TCXOMGR_TABLE_GPS,
  TCXOMGR_TABLE_MAX
} tcxomgr_ft_table_type;


/* Structure to describe the coefficients of the calculated C2/C3
   conversions */
typedef struct
{
  int32                      c1toc2_0;
  int32                      c1toc2_1;
  int32                      c1toc3_0;
  int32                      c1toc3_1;
} tcxomgr_cal_ft_conv_type;


/* Structure to describe the field coefficients of the freq/temp curve
   f(t) = c3(t-t0)^3 + c2(t-t0)^2 + c1(t-t0) + c0

  Note: The order of the first 9 members in this data type must match exactly
  those of tcxomgr_cal_fac_data_type */
typedef struct
{
  int32                      c3;                 /* units of 2^-32 ppm/C^3  */
  int32                      c2;                 /* units of 2^-28 ppm/C^2  */
  int32                      c1;                 /* units of 2^-24 ppm/C    */
  int32                      c0;                 /* units of 2^-12 ppm      */
  tcxomgr_temp_type          t0;                 /* Ref XO Temp term
                                                     units of 2^-10 C       */
  tcxomgr_temp_type          t0p;                /* XO therm at which coarse
                                                     calibration was performed
                                                     units of 2^-10 C       */
  int32                      p;                  /* C0 uncertainty to be used 
                                                     in coarse cal state     */
  tcxomgr_cal_ft_qual_enum_type ft_qual_ind;     /* quality indicator of
                                                     the coefficients       */
  uint8                      xo_trim;            /* corresponding xo_trim
                                                     val for coefficients   */
  uint64                     timestamp;          /* timestamp of last update*/
  uint32                     nv_timestamp;       /* timestamp of last nv 
                                                    update */
  uint16                     ft_sample_cnt;      /* num of freq-temp samples*/
  tcxomgr_field_state_type   state;              /* field calibration state */
} tcxomgr_cal_field_data_type;


/*---------------------------------------------------------------------------
  TCXO Manager's XO Aging values
---------------------------------------------------------------------------*/

typedef struct
{
  int32                      c0_fresh;          /* C0 Fresh value for aging */
  int32                      c0_fresh_nv;       /* C0 Fresh value for aging,
                                                   value saved in flash     */
  uint32                     timestamp;         /* C0 fresh time stamp      */
  uint32                     timestamp_nv;      /* C0 fresh time stamp,
                                                   value saved in flash     */
  uint32                     count;             /* Points used for C0 fresh */
  boolean                    valid;             /* is C0 fresh valid        */
  int32                      c1_num_points;     /* Num of pts for C1 cal    */
  int32                      c1_std_dev;        /* Std Dev of C1 cal        */
  int32                      c3_num_points;     /* Num of pts for C3 cal    */
  int32                      c3_std_dev;        /* Std Dev of C3 cal        */
} tcxomgr_cal_aging_data_type;



/* Pack structure to be stored to EFS containing Field Cal Params */
typedef PACK (struct)
{
  int32                         c3;             /* units of 2^-32 ppm/C^3  */
  int32                         c2;             /* units of 2^-28 ppm/C^2  */
  int32                         c1;             /* units of 2^-24 ppm/C    */
  int32                         c0;             /* units of 2^-12 ppm      */
  tcxomgr_temp_type             t0;             /* Ref XO Temp term        */
  tcxomgr_cal_ft_qual_enum_type ft_qual_ind;    /* quality indicator of    */
  uint8                         xo_trim;        /* trim from fac cal       */
  uint64                        timestamp;      /* timestamp of last update*/
  uint32                        nv_timestamp;   /* last nv update          */
  tcxomgr_field_state_type      state;          /* field calibration state */                                                   
} tcxomgr_efs_field_data_type;


/* Pack structure to be stored to EFS containing Field Cal Stats */
typedef PACK (struct)
{
  int32                      c0_fresh;          /* C0 Fresh value for aging */
  uint32                     timestamp;         /* C0 fresh time stamp      */
  uint32                     count;             /* Points used for C0 fresh */
  boolean                    valid;             /* is C0 fresh valid        */
  int32                      c1_num_points;     /* Num of pts for C1 cal    */
  int32                      c1_std_dev;        /* Std Dev of C1 cal        */
  int32                      c3_num_points;     /* Num of pts for C3 cal    */
  int32                      c3_std_dev;        /* Std Dev of C3 cal        */
} tcxomgr_efs_field_aging_type;

/* SRAM capture -> IQ/P -> freq est structure */
typedef struct
{
  uint16                      raw[ CAL_IQ_SAMP_SIZE * 4];
                                                  /* raw sample data          */
  int16                       i[ ( CAL_IQ_SAMP_SIZE * 2 ) ];
                                                  /* I data                   */
  int16                       q[ ( CAL_IQ_SAMP_SIZE * 2 ) ];
                                                  /* Q data                   */
  int32                       p[ ( CAL_IQ_SAMP_SIZE * 2 ) ];
                                                  /* Phase data (2^-10 radian)*/
  int32                       phase_jumps;        /* number of phase jumps    */
  tcxomgr_hz_type             freq_est_raw;       /* raw frequency estimate   */
  tcxomgr_hz_type             freq_est;           /* adj frequency estimate   */
  tcxomgr_temp_set_type       init_temp;          /* initial temp (XO,PMIC,PA)*/
  tcxomgr_temp_set_type       final_temp;         /* final temp (XO,PMIC,PA)  */
  tcxomgr_vco_type            vco;                /* VCO value                */
  tcxomgr_rotator_type        rot;                /* ROT value                */
  int32                       min_phase_diff;     /* minimum phase difference  
                                                      per sample               */
  int32                       max_phase_diff;     /* maximum phase difference  
                                                      per sample               */
} tcxomgr_cal_data_type;

#define FIELD_TO_FAC( data ) \
  /*lint -save -e740 We play this trick on purpose */ \
  ((tcxomgr_cal_fac_data_type*)((data))) \
  /*lint restore */
  
/* structure for inclusion into the global tcxomgr structure */
typedef struct
{
  /* Field Cal coefficients (running) */
  tcxomgr_cal_field_data_type     field_data;

  /* Factory Cal coefficients (static, this should ONLY be written to in
     FacCal and when copied from NV in the field) */
  tcxomgr_cal_fac_data_type       fac_data;

  /* Conversion factors for Calibration coefficients */
  tcxomgr_cal_ft_conv_type        conv;

} tcxomgr_cal_fac_type;


/*structure consisting of arguments for field_update_rgs function*/
typedef struct
{
  
  tcxomgr_ppm_type       rot;              /* rotator from layer one  */
  tcxomgr_temp_set_type  temp;             /* Last temp read          */
  uint32                 curr_time;        /* current time in ms      */
  uint32                 temp_read_time;   /* time of temp read in ms */
  tcxomgr_client_id_type id;                /* Client Id               */

}tcxomgr_update_rgs_struct_type;

typedef PACK(struct)
{
  /* Rx frequency (in kHz)          */
  uint32 rx_freq_khz;         
  
  /* Frequency offset (in Hz)       */
  int32  offset;                      
  
  /* The power level of the CW tone set by tools at the test box */
  int32  power_level;         
  
  /* XTAL package used in the device. 
     Refer enum: tcxomgr_cal_xtal_enum_type */
  tcxomgr_cal_xtal_enum_type  xtal;
  
  /* Refer enum: tcxomgr_cal_xo_cal_tech */
  uint8  tech;
                                 
} tcxomgr_coarse_dc_cal_req_data_struct_type_v1;

typedef PACK(struct)
{
  uint32                    version;
  union 
  {
    tcxomgr_coarse_dc_cal_req_data_struct_type_v1 v1;
  } u;
} tcxomgr_coarse_dc_cal_req_data_type;

typedef PACK(struct)
{
  /* pre FacCal freqerr             */
  int32       init_xo_freqerr;                    
  
  /* post Coarse DC freqerr         */
  int32       dc_xo_freqerr;    
  
  /* Initial temperature            */
  int32       init_temp;
  
  /* Final temperature              */
  int32       final_temp;

  /* Coefficients resulted from calibration */
  int32       c3;

  /* units of 2^-32 ppm/C^3  */  
  int32       c2;

  /* units of 2^-28 ppm/C^2  */  
  int32       c1; 

  /* units of 2^-24 ppm/C    */  
  int32       c0;     

  /* Ref XO Temp term units of 2^-10 C       */  
  tcxomgr_temp_type                 t0;  

  /* XO therm at which coarse calibration was performed units of 2^-10 C       */  
  tcxomgr_temp_type                 t0p;      

  /* c0 uncertainty in coarse cal state      */
  int32                             p;  

  /* CNo from GPS XO Cal            */
  uint32                            gps_cno;  
  
  /* pre FacCal xo_trim val         */
  uint8       init_xo_trim;                   

  /* post Coarse DC xo_trim         */
  uint8       dc_xo_trim; 
  
  /* Failure reason                 */
  tcxomgr_cal_failure_enum_type     fail_reason; 
  
  /* quality indicator of the coefficients       */
  tcxomgr_cal_ft_qual_enum_type     ft_qual_ind;
                                                
} tcxomgr_coarse_dc_cal_rsp_data_struct_type_v1;

typedef PACK(struct)
{
  uint32                    version;
  union 
  {
    tcxomgr_coarse_dc_cal_rsp_data_struct_type_v1 v1;
  } u;
} tcxomgr_coarse_dc_cal_rsp_data_type;

typedef struct 
{
  tcxomgr_hr_ppm_type           freq_est_in_hrppb;  /* Frequency error estimate 
                                                       in HR ppb                 */
  tcxomgr_cal_failure_enum_type fail_reason;        /* TCXOMGR failure reason.   */
  uint32                        gps_cno;            /* The CNo returned by GPS   */
} tcxomgr_cal_results_info_struct;

typedef void (*trm_cal_timeout_cb) (void);

#define CAL_FT_QUAL_FROM_NV( nv, qual )                                       \
  switch ( nv )                                                               \
  {                                                                           \
    case TCXOMGR_CAL_NV_FT_QUAL_NOT_CAL:                                      \
      qual = TCXOMGR_CAL_FT_QUAL_NOT_CAL;                                     \
      break;                                                                  \
    case TCXOMGR_CAL_NV_FT_QUAL_FAC1:                                         \
      qual = TCXOMGR_CAL_FT_QUAL_FAC1;                                        \
      break;                                                                  \
    case TCXOMGR_CAL_NV_FT_QUAL_COARSE:                                       \
      qual = TCXOMGR_CAL_FT_QUAL_COARSE;                                      \
      break;                                                                  \
    case TCXOMGR_CAL_NV_FT_QUAL_FINE:                                         \
      qual = TCXOMGR_CAL_FT_QUAL_FINE;                                        \
      break;                                                                  \
    case TCXOMGR_CAL_NV_FT_QUAL_FT1:                                          \
      qual = TCXOMGR_CAL_FT_QUAL_FT1;                                         \
      break;                                                                  \
    case TCXOMGR_CAL_NV_FT_QUAL_FT2:                                          \
      qual = TCXOMGR_CAL_FT_QUAL_FT2;                                         \
      break;                                                                  \
    case TCXOMGR_CAL_NV_FT_QUAL_FAILED:                                       \
    default:                                                                  \
      qual = TCXOMGR_CAL_FT_QUAL_FAILED;                                      \
      break;                                                                  \
  }

#define CAL_FT_QUAL_TO_NV( qual, nv )                                         \
  switch ( qual )                                                             \
  {                                                                           \
    case TCXOMGR_CAL_FT_QUAL_NOT_CAL:                                         \
      nv = (nv_xo_ft_qual_enum_type) TCXOMGR_CAL_NV_FT_QUAL_NOT_CAL;          \
      break;                                                                  \
    case TCXOMGR_CAL_FT_QUAL_FAC1:                                            \
      nv = (nv_xo_ft_qual_enum_type) TCXOMGR_CAL_NV_FT_QUAL_FAC1;             \
      break;                                                                  \
    case TCXOMGR_CAL_FT_QUAL_FT1:                                             \
      nv = (nv_xo_ft_qual_enum_type) TCXOMGR_CAL_NV_FT_QUAL_FT1;              \
      break;                                                                  \
    case TCXOMGR_CAL_FT_QUAL_COARSE:                                          \
      nv = (nv_xo_ft_qual_enum_type) TCXOMGR_CAL_NV_FT_QUAL_COARSE;           \
      break;                                                                  \
    case TCXOMGR_CAL_FT_QUAL_FINE:                                            \
      nv = (nv_xo_ft_qual_enum_type) TCXOMGR_CAL_NV_FT_QUAL_FINE;             \
      break;                                                                  \
    case TCXOMGR_CAL_FT_QUAL_FT2:                                             \
      nv = (nv_xo_ft_qual_enum_type) TCXOMGR_CAL_NV_FT_QUAL_FT2;              \
      break;                                                                  \
    case TCXOMGR_CAL_FT_QUAL_FAILED:                                          \
    default:                                                                  \
      nv = (nv_xo_ft_qual_enum_type) TCXOMGR_CAL_NV_FT_QUAL_FAILED;           \
      break;                                                                  \
  }

#define TCXOMGR_TWOS_COMP_TO_INT(data, bits) if ( ((data) & (1 << ((bits)-1))) != 0) \
                                       { (data) = (int8)((data) - (1 << (bits))); }

extern rex_tcb_type    *fac_cal_tcb;
/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/
  
/*============================================================================

FUNCTION TCXOMGR_CAL_CALC_C2

DESCRIPTION
  This function uses the current "nominal" coefficients to calculate C2 based
  one the currently known/initialzed coefficients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_calc_c2
(
  tcxomgr_cal_fac_data_type  *coefs              /* current coefs            */
);  


/*============================================================================

FUNCTION TCXOMGR_CAL_CALC_C3

DESCRIPTION
  This function uses the current "nominal" coefficients to calculate C3 based
  one the currently known/initialzed coefficients

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_calc_c3
(
  tcxomgr_cal_fac_data_type  *coefs              /* current coefs            */
);



/*============================================================================

FUNCTION TCXOMGR_CAL_COPY_FAC_TO_FIELD

DESCRIPTION
  This function copies xo factory cal data to field data structure and
  initializes the extra fields in field data to their default values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The fields not found in factory data are initialized to default values.

============================================================================*/
extern void tcxomgr_cal_copy_fac_to_field ( void );


/*============================================================================

FUNCTION TCXOMGR_FIELD_CAL

DESCRIPTION
  Runs the field calibration state machine

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_field_cal
(
  uint32                 time,
  tcxomgr_temp_set_type  temp,
  tcxomgr_rotator_type   rot
);

/*============================================================================

FUNCTION TCXOMGR_FIELD_VERIFY_XO_TRIM

DESCRIPTION
  This function verifies the trim value is consistent with the value in the NV

DEPENDENCIES
  This function should only be called during power up (not in service)

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_field_verify_xo_trim( void );

/*============================================================================

FUNCTION tcxomgr_set_tdm_table

DESCRIPTION
  This function configures the sample server block to serve multiple clients
  for Poseidon modem

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_set_tdm_table( void );

/*============================================================================

FUNCTION TCXOMGR_LOG_IQ_DATA

DESCRIPTION
  This function splits the raw IQ data into smaller chunks and calls the
  logging function to log the IQ data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_log_iq_data
(
  uint16                      *raw_data,
  const tcxomgr_freq_err_est_param_type	*est_param,
  tcxomgr_temp_type            temp_xo,
  tcxomgr_temp_type            temp_pmic
);

/*============================================================================

FUNCTION TCXOMGR_FAST_FIELD_CAL_INIT

DESCRIPTION
  This function initializes the data structures and timers needed for fast
  field cal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_fast_field_cal_init(void);

/*============================================================================

FUNCTION TCXOMGR_FIELD_UPDATE_RGS
 
DESCRIPTION
    This function stores the values for rgs in the ft table for use in
    fast field calibration.
 
DEPENDENCIES
  

RETURN VALUE
    None
 
SIDE EFFECTS
 This function is called in the caller's context. So MAKE SURE THAT NO TIME
 TAKING ACTIVITIES ARE DONE IN THIS FUNCTION. If some functionality is
 added in the future, consider calling this function in the context of
 tcxomgr task by setting a signal.
 
============================================================================*/
void tcxomgr_field_update_rgs
(
  tcxomgr_update_rgs_struct_type *rgs_data
);


/*============================================================================

FUNCTION TCXOMGR_FAST_FIELD_CAL

DESCRIPTION
    This function is called periodically to do the field calibration based
    on the data currently in the ft table. 

DEPENDENCIES
    this function assumes that the ft table is updated with rotator errors
    at different temperatures. 

RETURN VALUE
    None
SIDE EFFECTS
    If the time criteria are met, this function also updates nv 
    with 

============================================================================*/
extern void tcxomgr_fast_field_cal(void);


/*============================================================================

FUNCTION TCXOMGR_FLUSH_FIELD_CAL_DATA

DESCRIPTION
    This function flushes the contents of both field calibration coefficients
    and ft table to nv. This function is called on a periodic basis and after
    some criteria are met(first c1 cal is done etc..)
 
    This function checks if the values of ft table have changed above a
    threshold. NV writes of the ft table happen only when values have changed
    signicantly. 
 
 
DEPENDENCIES
    None

RETURN VALUE
    None
SIDE EFFECTS
    This function takes the values currently in memory and flushes them to nv
    by using a slow filter. Once the values are written to nv, the contents of
    the ft table in memory are also modified. The new filtered values are
    used for updating the in memory ft table.  
============================================================================*/
void  tcxomgr_flush_field_cal_data
(
  uint32                 time,
  tcxomgr_ft_table_type  table
);

/*============================================================================

FUNCTION TCXOMGR_FT_EVAL_NO_PMIC

DESCRIPTION
  This function calculates the frequency error in PPM expected based on a
  given set of temperatures. It excludes the effect of PMIC temperature.

DEPENDENCIES
  None

RETURN VALUE
  Frequency Error in PPM Units

SIDE EFFECTS
  The fixed point implementation loss is about 2e-4 ppm on the frequency
   estimate returned

============================================================================*/
tcxomgr_ppm_type tcxomgr_ft_eval_no_pmic
(
  tcxomgr_temp_set_type      temp                /* current temperature     */
);


/*============================================================================

FUNCTION TCXOMGR_GET_FT_CORRECTION

DESCRIPTION 
  Computes the frequencies from the curve at current temp and rgs temp. Then
  returns the difference in the form of correction to be applied to RGS.
  If pmic temperature is invalid, then avoids pmic compensation while computing
  the curve values.
  
DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_ppm_type

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_ppm_type tcxomgr_get_ft_correction
(
  tcxomgr_temp_set_type  current_temp,
  tcxomgr_temp_set_type  rgs_temp
);

/*============================================================================

FUNCTION tcxomgr_cal_gsm_init_capture

DESCRIPTION
  This function does all the initialization required for capturing GSM
  IQ samples

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_gsm_init_capture 
(
  tcxomgr_cal_xo_cal_tech tech
);

/*============================================================================

FUNCTION tcxomgr_cal_gsm_capture_iq

DESCRIPTION
  This function captures the GSM IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_gsm_capture_iq
(
  tcxomgr_cal_xo_cal_tech tech, 
  tcxomgr_cal_data_type *d
);

/*============================================================================

FUNCTION tcxomgr_cal_gsm_tear_capture

DESCRIPTION
  This function tears down any setup done for capturing IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_gsm_tear_capture
(
  tcxomgr_cal_xo_cal_tech tech
);
/*============================================================================

FUNCTION TCXOMGR_CAL_WAIT

DESCRIPTION
  This function implements wait functionality in micro seconds

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful wait

SIDE EFFECTS
  None

============================================================================*/
boolean tcxomgr_cal_wait
(
  timetick_type time_us
);


/*============================================================================

FUNCTION tcxomgr_cal_cdma_init_capture

DESCRIPTION
  This function does all the initialization required for capturing CDMA
  IQ samples

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_cdma_init_capture 
(
  tcxomgr_cal_xo_cal_tech tech
);

/*============================================================================

FUNCTION tcxomgr_cal_cdma_capture_iq

DESCRIPTION
  This function captures the CDMA IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_cdma_capture_iq
(
  tcxomgr_cal_xo_cal_tech tech, 
  tcxomgr_cal_data_type *d
);

/*============================================================================

FUNCTION tcxomgr_cal_cdma_tear_capture

DESCRIPTION
  This function tears down any setup done for capturing IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_cdma_tear_capture
(
  tcxomgr_cal_xo_cal_tech tech 
);

/*============================================================================

FUNCTION tcxomgr_cal_wcdma_init_capture

DESCRIPTION
  This function does all the initialization required for capturing WCDMA
  IQ samples

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type  tcxomgr_cal_wcdma_init_capture 
(
  tcxomgr_cal_xo_cal_tech tech
);

/*============================================================================

FUNCTION tcxomgr_cal_wcdma_capture_iq

DESCRIPTION
  This function captures the WCDMA IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_wcdma_capture_iq
(
  tcxomgr_cal_xo_cal_tech tech, 
  tcxomgr_cal_data_type *d
);

/*============================================================================

FUNCTION tcxomgr_cal_wcdma_tear_capture

DESCRIPTION
  This function tears down any setup done for capturing IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_wcdma_tear_capture
(
  tcxomgr_cal_xo_cal_tech tech 
);

/*============================================================================

FUNCTION tcxomgr_cal_lte_init_capture

DESCRIPTION
  This function does all the initialization required for capturing LTE
  IQ samples

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type  tcxomgr_cal_lte_init_capture 
(
  tcxomgr_cal_xo_cal_tech tech
);

/*============================================================================

FUNCTION tcxomgr_cal_lte_capture_iq

DESCRIPTION
  This function captures the LTE IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_lte_capture_iq
(
  tcxomgr_cal_xo_cal_tech tech, 
  tcxomgr_cal_data_type *d
);

/*============================================================================

FUNCTION tcxomgr_cal_lte_tear_capture

DESCRIPTION
  This function tears down any setup done for capturing IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_lte_tear_capture
(
  tcxomgr_cal_xo_cal_tech tech
);

/*============================================================================

FUNCTION tcxomgr_cal_tdscdma_init_capture

DESCRIPTION
  This function does all the initialization required for capturing TDSCDMA
  IQ samples

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type  tcxomgr_cal_tdscdma_init_capture 
(
  tcxomgr_cal_xo_cal_tech tech
);

/*============================================================================

FUNCTION tcxomgr_cal_tdscdma_capture_iq

DESCRIPTION
  This function captures the TDSCDMA IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_tdscdma_capture_iq
(
  tcxomgr_cal_xo_cal_tech tech, 
  tcxomgr_cal_data_type *d
);

/*============================================================================

FUNCTION tcxomgr_cal_tdscdma_tear_capture

DESCRIPTION
  This function tears down any setup done for capturing IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure
  
SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_tdscdma_tear_capture
(
  tcxomgr_cal_xo_cal_tech tech 
);


/*============================================================================

FUNCTION TCXOMGR_FT_DATA_IS_VALID
 
DESCRIPTION
    This function returns of if the passed table has valid contents or not.
     
DEPENDENCIES
    None

RETURN VALUE
    Boolean
 
SIDE EFFECTS
    None 
    
============================================================================*/
boolean tcxomgr_ft_data_is_valid
(
  tcxomgr_ft_table_type table
);

/*============================================================================

FUNCTION tcxomgr_cal_gsm_init_capture

DESCRIPTION
  This function does all the initialization required for capturing GSM
  IQ samples

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_gps_init_capture 
(
  tcxomgr_cal_xo_cal_tech tech
);

/*============================================================================

FUNCTION tcxomgr_cal_gps_capture_iq

DESCRIPTION
  This function captures the gps IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_gps_capture_iq
(
  tcxomgr_cal_xo_cal_tech tech, 
  tcxomgr_cal_data_type *d
);

/*============================================================================

FUNCTION tcxomgr_cal_gps_tear_capture

DESCRIPTION
  This function tears down any setup done for capturing IQ samples.

DEPENDENCIES
  None

RETURN VALUE
  Error Code if there is a failure
  CAL_FAIL_NONE if there is no failure

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_cal_failure_enum_type tcxomgr_cal_gps_tear_capture
(
  tcxomgr_cal_xo_cal_tech tech
);

/*===========================================================================

FUNCTION TCXOMGR_CAL_SIG_REX_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type tcxomgr_cal_sig_rex_wait
(
  trm_cal_timeout_cb  timeout_cb
);

/*============================================================================

FUNCTION tcxomgr_cal_get_wait_sig

DESCRIPTION
  Returns the signal to be used to wait when performing calibration.

DEPENDENCIES
  None

RETURN VALUE
  Signal type

SIDE EFFECTS
  None

============================================================================*/
rex_sigs_type tcxomgr_cal_get_wait_sig (void);

/*============================================================================

FUNCTION TCXOMGR_CAL_GPS_ENTER_MODE

DESCRIPTION
  This function sets GPS mode on

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
================================================================================*/
void tcxomgr_cal_gps_enter_mode
(
  int32 power,
  int32 freq_offset
);

/*============================================================================

FUNCTION TCXOMGR_CAL_GPS_ENTER_MODE

DESCRIPTION
  This function sets GPS mode OFF

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
================================================================================*/
void tcxomgr_cal_gps_exit_mode( void );

/*============================================================================

FUNCTION TCXOMGR_CAL_GPS_GET_CAL_RESULTS

DESCRIPTION
  This function gets the failure reason stored during GPS XO Cal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
================================================================================*/
void tcxomgr_cal_gps_get_cal_results( tcxomgr_cal_results_info_struct* results );

/*============================================================================

FUNCTION CAL_CALC_HZ_TO_HRPPB

DESCRIPTION
  This function translates Hz into ppm units

DEPENDENCIES
  None

RETURN VALUE
  Corresponding Hertz in 2^-12 ppm units

SIDE EFFECTS
  Fixed point loss is about 1e-3 ppm worst-case for 100KHz

============================================================================*/
int32 cal_calc_hz_to_hrppb
(
  int32                      hz,                 /* Hertz value to translate*/
  uint32                     rxfreq_khz,
  tcxomgr_hz_type            cf_correction       /* correction term in Hz
                                                    due to XO error         */
);

#endif /* TCXOMGR_CAL_I_H */
