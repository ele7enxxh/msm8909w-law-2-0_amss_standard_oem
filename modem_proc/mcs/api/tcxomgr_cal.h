#ifndef TCXOMGR_CAL_H
#define TCXOMGR_CAL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

             T C X O    M A N A G E R    C A L I B R A T I O N

               TCXO Manager Calibration Header File

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

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/tcxomgr_cal.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
11/05/15   tl      Remove factory fine calibration
05/01/14   tl      Add TCXOMGR_FIELD5 state to refine calibration in FT2
01/06/14   tl      Support chipxn compensation using VSTMR
12/19/12   sk      support for field cal coefficients subscription
07/16/13   mn      Dynamic C0 uncertainty in coarse cal state.
06/14/13   mn      Added a fail reason for frequency offset bounds check.
04/01/13   sg      Merged fail reason enum to a single list
12/05/12   sk      Tcxomgr cal fail enum made compatible to old definition
11/13/12   sk      support for t0, wb_id, temp_grad_timeout via caltree added 
10/29/12   sk      Support for exposing the freq error 
                     estimation api for FTM tests 
04/23/12   sg      Support for factory fine cal removal 
04/09/12   mn      Added a new tech TECHNOLOGY_CDMA_SEC.
02/01/12   sr      TCXOMGR Feature Cleanup
01/15/12   sg      support for fast field cal v3.1
01/23/12   ag      Make cal state enum 16 bit.
11/03/11   sg      Added support for TDSCDMA XO Cal
11/29/10   hh      CMI phase-4 changes.
11/23/10   ag      Added APIs for disabling PMIC compensation in ft_eval()
11/04/10   sm      Added Support For Fast Field Cal
09/22/10   ag      Removed clk.h as its not needed anymore.
08/09/10   ns      Added function to start/stop XO compensation 
08/09/10   sm      Changed default values of t0 and t0p to 30C
06/25/10   sm      Moved includes and structure defs from hdr to c file
06/21/10   ag      Changed the header name to lower case.
05/25/10   sm      Added INO DAL support for WCDMA IQ sample collection
05/11/10   ns      Add start and stop freq error info to ftm packet 
03/02/10   ns      IQ logging support
09/18/09   ns      Making c2 and c3 constant for factory cal. Also
                     changing temp ranges for field cal
06/15/09   ns      Xo calibration changes for CDMA SCMM bringup  
04/13/09   ag      Merged from //depot
11/16/07   va      Remove TCXO Diag cal functions
10/18/07   sst     Updated calculations for nominal C2/C3
09/06/07   sst     Sync enum of ft_qual btw NV and TCXOMGR
                   Undefine TCXOMGR_CAL_DEBUG
                   Lint fixes
08/13/07   sst     Change field data member timestamp to uint64
07/27/07   sst     Added prototypes for: tcxomgr_field_switch_xo_trim
                   Reordered enum ft_qual_enum to match vco_ind
                   Moved NV defaults from _cal.c
                   Modify PMIC coef to be -37 (-1/110 in HR ppm/C)
06/29/07   cab     Added tcxomgr_field_cal
06/18/07   aps     Added prototypes tcxomgr_ft_eval( ) and
                    tcxomgr_cal_copy_fac_to_field( )
06/18/07   va      Remove undef of XO_FACTORY_CAL for XO builds
06/13/07   sst     Created tcxomgr_cal_ft_conv_type and tcxomgr_cal_fac_type
                   Misc code fixes and updates
06/08/07   sst     Moved tcxomgr_cal_temp_type to tcxomgr.h
                   Added completion callback function to Fac Cal functions
06/06/07   sst     Factory Cal merge to mainline from dev branch
03/07/07   sst     Initial Version

============================================================================*/

/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "tcxomgr.h"
#include "vstmr.h"


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/*============================================================================

                   Defines

============================================================================*/
#define TCXOMGR_CAL_NUM_SAMPLES   512       /* max number of samples to
                                               record                       */
#define TCXOMGR_COEFF_C0_BIT_MASK (uint32) 0x01
#define TCXOMGR_COEFF_C1_BIT_MASK (uint32) 0x02
#define TCXOMGR_COEFF_C2_BIT_MASK (uint32) 0x04
#define TCXOMGR_COEFF_C3_BIT_MASK (uint32) 0x08
#define TCXOMGR_STATE_BIT_MASK    (uint32) 0x10
#define TCXOMGR_QUAL_IND_BIT_MASK (uint32) 0X20

/*============================================================================

                   Macros

============================================================================*/


/*============================================================================

                   Structs, Enums

============================================================================*/


/*------------------------ General -----------------------------------------*/

/* tcxomgr factory cal state*/
typedef enum
{
  TCXOMGR_CAL_STATE_NOT_CAL,                     /* intial uncalibrated state*/
  TCXOMGR_CAL_STATE_COARSE,                      /* Coarse tuning completed  */
  TCXOMGR_CAL_STATE_SAMP,                        /* Samples Captured         */
  TCXOMGR_CAL_STATE_C1C2C3,                      /* C1C2C3 determined        */
  TCXOMGR_CAL_STATE_FINE,                        /* Fine tuning completed    */
  TCXOMGR_CAL_STATE_COMPLETED,                   /* Fac Cal completed        */
  TCXOMGR_CAL_STATE_FAILED,                      /* Fac Cal failed           */
  TCXOMGR_CAL_STATE_MAX = 0x7FFF                 /* Dummy to make enum 16bit */
} tcxomgr_cal_state_enum_type;

 
/* tcxomgr factory failure reasons 
   **NOTE** The order cannot be changed. Maintain compatability for OEMs */
typedef enum
{

  TCXOMGR_CAL_FAIL_NONE                       = 0,    /* Passed successfully            */
          
  TCXOMGR_ERR_EST_FAIL_NONE                   = 0,    /* Passed successfully            */
  TCXOMGR_ERR_EST_FAIL_UNSUPPORTED_TECH       = 1,    /* Tech is not supported          */
  TCXOMGR_ERR_EST_FAIL_DC_OFFSET_FAR          = 2,    /* DC Offset far off check offset */
  TCXOMGR_ERR_EST_FAIL_SAMPLE_XO_TEMP         = 3,    /* Xo fluctuates over IQ sample   */
  TCXOMGR_ERR_EST_FAIL_SAMPLE_PMIC_TEMP       = 4,    /* Pmic fluctuates over IQ sample */
  TCXOMGR_ERR_EST_FAIL_SAMPLE_ROTATOR         = 5,    /* Rotators are not zero          */
      
  TCXOMGR_CAL_FAIL_TRIM_SATURATED             = 6,    /* Reached the trim limits        */
  TCXOMGR_CAL_FAIL_TEMP_SPAN                  = 7,    /* Failed req degree change       */
  TCXOMGR_CAL_FAIL_TEMP_LESS_THAN_MIN         = 8,    /* XO temp less than allowed      */
  TCXOMGR_CAL_FAIL_TEMP_MORE_THAN_MAX         = 9,    /* XO temp more than allowed      */
      
  TCXOMGR_ERR_EST_FAIL_NOISY_IQ               = 10,   /* Noisy IQ samples               */
  TCXOMGR_ERR_EST_FAIL_PHASE_JUMPS            = 11,   /* Phase jumps in IQ              */
  TCXOMGR_ERR_EST_FAIL_MSGR_INIT              = 12,   /* Mesage router did not init     */
  TCXOMGR_ERR_EST_FAIL_MSGR_SEND              = 13,   /* Mesage router sending fail     */
  TCXOMGR_ERR_EST_FAIL_MSGR_RECV              = 14,   /* Mesage router reception fail   */ 
  TCXOMGR_ERR_EST_FAIL_FATAL_ERROR            = 15,   /* Other errors like NULL pointers*/
        
  TCXOMGR_CAL_FAIL_TEMP_SPAN_TIMEOUT          = 16,   /* XO temp span time out reached  */
  TCXOMGR_CAL_FAIL_TEMP_MAX_SAMPLES           = 17,   /* XO max samples reached         */
  TCXOMGR_CAL_FAIL_TEMP_SPAN_TOO_SMALL        = 18,   /* XO temp span is too small      */
        
  TCXOMGR_ERR_EST_NO_TONE                     = 19,   /* Fail because No tone is found in valid 
                                                        ranges.                              */
  TCXOMGR_ERR_EST_MULTI_TONES                 = 20,   /* Fail because 2 or more tones are 
                                                        found in valid ranges.               */
  TCXOMGR_ERR_EST_JAMMER_OUTSIDE_RANGE        = 21,   /* Fail because extremely strong jammer 
                                                        outside of valid freq range          */
  TCXOMGR_ERR_EST_DATA_COLLECT_TIMEOUT        = 22,   /* Fail because of IQ data collection 
                                                        timeout.                             */
  TCXOMGR_ERR_EST_OTHER_ERROR                 = 23,   /* Fail due to MISC error.              */
        
  TCXOMGR_CAL_FAIL_FREQ_OFFSET_LIMIT_FAILURE  = 24    /* Signifies that the frequency offset
                                                         is not within the allowable limits  */

} tcxomgr_cal_failure_enum_type;

/* indicator of the quality of the coefficients */
typedef enum
{
  TCXOMGR_CAL_FT_QUAL_FAILED  = TCXOMGR_FT_QUAL_FAILED,
                                                 /* 56 PPM mode
                                                    (Fac Cal Failed, phone
                                                     should have failed in
                                                     Factory)                */
  TCXOMGR_CAL_FT_QUAL_NOT_CAL = TCXOMGR_FT_QUAL_NOT_CAL,
                                                 /* uncalibrated             */
  TCXOMGR_CAL_FT_QUAL_FAC1    = TCXOMGR_FT_QUAL_FAC1,
                                                 /* 56 PPM mode              */
  TCXOMGR_CAL_FT_QUAL_COARSE  = TCXOMGR_FT_QUAL_COARSE,
                                                 /* 12 PPM mode              */
  TCXOMGR_CAL_FT_QUAL_FINE    = TCXOMGR_FT_QUAL_FINE,
                                                 /* 8 PPM mode               */
  TCXOMGR_CAL_FT_QUAL_IFC     = TCXOMGR_FT_QUAL_IFC,
                                                 /* 8 PPM mode               */
  TCXOMGR_CAL_FT_QUAL_FT1     = TCXOMGR_FT_QUAL_FT1,
                                                 /*  5 PPM mode              */
  TCXOMGR_CAL_FT_QUAL_FT2     = TCXOMGR_FT_QUAL_FT2,
                                                 /*  3 PPM mode              */
} tcxomgr_cal_ft_qual_enum_type;


/* tcxomgr field cal states */
typedef enum
{
  TCXOMGR_FIELD0,
  TCXOMGR_FIELD1,
  TCXOMGR_FIELD2,
  TCXOMGR_FIELD3,
  TCXOMGR_FIELD4,
  TCXOMGR_FIELD5,
} tcxomgr_field_state_type;


/* indicator wether to record Fac Cal coefficients in RAM and/or NV during
   Factory Cal */
typedef enum
{
  TCXOMGR_CAL_RECORD_NONE = 0x0,                 /* do not record            */
  TCXOMGR_CAL_RECORD_RAM  = 0x1,                 /* record to RAM            */
  TCXOMGR_CAL_RECORD_NV   = 0x3,                 /* record to RAM and NV     */
} tcxomgr_cal_record_enum_type;

/* Factory calibration parameters */
typedef enum
{
  TECHNOLOGY_CDMA     = 0,                 
  TECHNOLOGY_WCDMA    = 1,       
  TECHNOLOGY_GSM      = 2,
  TECHNOLOGY_LTE      = 3,
  TECHNOLOGY_TDSCDMA  = 4,
  TECHNOLOGY_CDMA_SEC = 5,
  TECHNOLOGY_GPS      = 6,
  TECHNOLOGY_MAX      = 7
} tcxomgr_cal_xo_cal_tech;

typedef struct
{
  tcxomgr_temp_set_type      min;                /* min temps encountered in
                                                    calibration              */
  tcxomgr_temp_set_type      max;                /* max temps encountered in
                                                    calibration              */
} tcxomgr_cal_temp_range_type;

/*structure to record data from freq error estimation api*/
typedef struct{
  tcxomgr_cal_failure_enum_type   est_failure_type;     /* reason for failure       */
  tcxomgr_hr_ppm_type             freq_err_est;         /* Frequency error estimate */
  tcxomgr_temp_set_type           avg_temp;             /* Average temperatures     */
}tcxomgr_freq_err_est_ret_type;

/*structure with data required for the frequency estimation api*/
typedef struct{
  tcxomgr_cal_xo_cal_tech   tech;
  tcxomgr_hz_type           offset;
  uint32                    rx_freq;
}tcxomgr_freq_err_est_param_type;

typedef struct
{
  tcxomgr_temp_set_type      xo_temp[ TCXOMGR_CAL_NUM_SAMPLES ];
                                                 /* 2^-10 C, XTAL temp       */
  tcxomgr_hr_ppm_type        xo_freq[ TCXOMGR_CAL_NUM_SAMPLES ];
                                                 /* 2^-12 ppm, XO frequency  */
  int32                      c1_est;             /* estimate for coef C1     */
  int32                      num_samples;        /* num samples taken        */
  tcxomgr_cal_temp_range_type temp_range;        /* temp range encountered   */
} tcxomgr_cal_ft_sample_type;

/* Structure to describe the factory coefficients of the freq/temp curve
   f(t) = c3(t-t0)^3 + c2(t-t0)^2 + c1(t-t0) + c0

  Note: The order of the members in this data type must match exactly those of
  tcxomgr_cal_field_data_type */
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
  int32                      p;                  /* c0 uncertainty in coarse 
                                                     cal state */
  tcxomgr_cal_ft_qual_enum_type ft_qual_ind;     /* quality indicator of
                                                     the coefficients       */
  uint8                      xo_trim;            /* corresponding xo_trim
                                                     val for coefficients   */
} tcxomgr_cal_fac_data_type;

typedef enum
{
  TCXOMGR_XTAL_DEFAULT,
  TCXOMGR_XTAL_2016,
  TCXOMGR_XTAL_2520,
  TCXOMGR_XTAL_MAX
} tcxomgr_cal_xtal_enum_type;

typedef struct
{
  tcxomgr_cal_xo_cal_tech    tech;               /* Technology   */
  uint32                     rx_freq_khz;        /* Carrier Frequency */
  tcxomgr_hz_type            offset;             /* Hz, single tone freq
                                                    offset relative to the
                                                    carrier freq             */
  tcxomgr_temp_type          temp_span;          /* 2^-10 C, temp span for
                                                    factory calibration      */
  uint16                     min_ft_sample;      /* min num of F-T samples   */
  uint16                     max_ft_sample;      /* max num of F-T samples   */
  uint16                     substage_time;      /* time to wait between samp
                                                    substages (ms)           */
  uint16                     timeout_time;       /* maximum time to run each
                                                    step before declaring
                                                    failure (ms)             */
  tcxomgr_cal_record_enum_type update_nv;        /* update NV items flag     */
  tcxomgr_cal_state_enum_type state;             /* current state of fac cal */
  tcxomgr_cal_fac_data_type   nom_coef;          /* nominal coefficients     */
  tcxomgr_cal_fac_data_type   cal_coef;          /* calibrated coefficients  */
  uint32                      rxlm_buffer_id;    /* RXLM Buffer id for the state */
  uint16                      chain;             /* Chain used for sample capture */
  uint16                      carrier;           /* Carrier used for capture */
  uint16                      wb_id;             /* wide band ID */
  uint16                      temp_grad_timeout; /* Timeout value to reach req temp */
  tcxomgr_cal_xtal_enum_type  xtal;              /* XTAL package used in the device */
} tcxomgr_cal_fac_param_type;

/* Factory calibration informational structure */
typedef struct
{
  uint8        init_xo_trim;       /* pre FacCal xo_trim val   */
  uint8        dc_xo_trim;         /* post Coarse DC xo_trim   */
  uint8        fin_xo_trim;        /* post Fine DC xo_trim     */
  uint8        final_xo_trim;      /* post FacCal xo_trim val  */

  int32        init_xo_freqerr;    /* pre FacCal freqerr       */
  int32        dc_xo_freqerr;      /* post Coarse DC freqerr   */
  int32        fin_xo_freqerr;     /* post Fine DC freqerr     */
  int32        final_xo_freqerr;   /* post FacCal freqerr      */

  uint32       s_ram_cap_cnt;      /* num of SRAM captures     */
  uint32       freq_est_calc_cnt;  /* num freq est calculations*/

  tcxomgr_temp_set_type         init_temp;    /* initial temp (XO,PMIC,PA)*/
  tcxomgr_temp_set_type         final_temp;   /* final temp (XO,PMIC,PA)  */
  tcxomgr_cal_temp_range_type   temp_range;   /* temp range encountered   */
  tcxomgr_cal_failure_enum_type fail_reason;  /* Failure reason           */
} tcxomgr_cal_info_type;


/* update_flag -/ bitmask indicating changes / 
      bit 0- set if coeff C0 has changed
      bit 1- set if coeff C1 has changed
      bit 2- set if coeff C2 has changed
      bit 3- set if coeff C3 has changed
      bit 4- set if state has changed
      bit 5- set if quality indicator has changed
      bit 6/7- unused  */
typedef struct
{
  tcxomgr_cal_fac_data_type       cal_data;         /* Coeffecient data         */
  tcxomgr_field_state_type        state;            /* state */
  uint32                          update_flag;      /* bit mask to indicate what coefficients changed*/
}tcxomgr_field_coeff_type;

/* Coeffecients to be used by L1s in their internal calculations */
typedef struct
{
  tcxomgr_field_coeff_type   gps;
  tcxomgr_field_coeff_type   wwan;
}tcxomgr_external_coeff_type;

/* function pointer for subscribing to cal coefficients */
typedef void (*tcxomgr_cal_coeff_subscribe_cb) 
(
 tcxomgr_external_coeff_type *cal_coeff 
);


/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_CAL_DC_COARSE

DESCRIPTION
  This function collects initial FT samples and uses them to do initial
  coarse XO Trim tuning to match the sampled points to the nominal calibrated
  curve

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  XO Trim could be changed

============================================================================*/
extern void tcxomgr_cal_dc_coarse
(
  tcxomgr_cal_fac_param_type *fac_cal_param,     /* factory cal parameters   */
  tcxomgr_cal_ft_sample_type *samples,           /* ptr to samples currently
                                                    taken                    */
  tcxomgr_cal_info_type      *info_pkt           /* cal info pkt             */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_DC_FINE

DESCRIPTION
  This function is obsolete. Its implementation has been removed, and it will
  always return failure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_dc_fine
(
  tcxomgr_cal_fac_param_type *fac_cal_param,     /* factory cal parameters   */
  tcxomgr_cal_ft_sample_type *samples,           /* ptr to samples currently
                                                    taken                    */
  tcxomgr_cal_info_type      *info_pkt           /* cal info pkt             */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_SAMP

DESCRIPTION
  This function is obsolete. Its implementation has been removed, and it will
  always return failure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_samp
(
  tcxomgr_cal_fac_param_type *fac_cal_param,     /* factory cal parameters   */
  tcxomgr_cal_ft_sample_type *samples,           /* ptr to samples currently
                                                    taken                    */
  tcxomgr_cal_info_type      *info_pkt           /* cal info pkt             */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_C1C2C3

DESCRIPTION
  This function is obsolete. Its implementation has been removed, and it will
  always return failure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_c1c2c3
(
  tcxomgr_cal_fac_param_type *fac_cal_param,     /* factory cal parameters   */
  tcxomgr_cal_ft_sample_type *samples,           /* ptr to samples currently
                                                    taken                    */
  tcxomgr_cal_info_type      *info_pkt           /* cal info pkt             */
);

/*============================================================================

FUNCTION TCXOMGR_CAL_INIT

DESCRIPTION
  This function initializes the three main structures used in factory
  calibration. It also sets some initial info_pkt information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_cal_param_init
(
  tcxomgr_cal_fac_param_type *fac_cal_param,     /* factory cal parameters   */
  tcxomgr_cal_ft_sample_type *samples,           /* ptr to samples currently
                                                    taken                    */
  tcxomgr_cal_info_type      *info_pkt           /* cal info pkt             */
);


/*============================================================================

FUNCTION TCXOMGR_TRIGGER_CHIPX8_MND_COMP

DESCRIPTION
  Interface for enabling and disabling Chipx8 compensation during RTL mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_trigger_chipx8_mnd_comp
(
  /* True = Enable False = DISABLE */
  boolean start_comp
);


/*============================================================================

FUNCTION TCXOMGR_TRIGGER_CHIPX8_MND_COMP_VSTMR

DESCRIPTION
  Interface for enabling and disabling Chipx8 compensation during RTL mode
  using VSTMR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void tcxomgr_trigger_chipx8_mnd_comp_vstmr
(
  /* True = Enable False = DISABLE */
  boolean start_comp,
  /* VSTMR client id */
  vstmr_rtc_id rtc_id
);


/*============================================================================

FUNCTION TCXOMGR_CAL_FT_EVAL

DESCRIPTION
  This function calculates the frequency error expected based on the given
  FT curve coefficients an the current temperature of the XO and/or PMIC

DEPENDENCIES
  None

RETURN VALUE
  Frequency Error in High Resolution PPM Units

SIDE EFFECTS
  The fixed point implementation loss is about 2e-4 ppm on the frequency
   estimate returned

============================================================================*/
extern tcxomgr_hr_ppm_type tcxomgr_cal_ft_eval
(
  tcxomgr_cal_fac_data_type  *ft_coef,           /* F/T equ coefficients    */
  tcxomgr_temp_type          t,                  /* temperature in 2^-10 C  */
  boolean                    pmic_comp,          /* PMIC compensation flag  */
  tcxomgr_temp_type          tp                  /* PMIC temp in 2^-10 C    */
);

/*============================================================================

FUNCTION TCXOMGR_GET_FREQ_ERR_EST

DESCRIPTION
  This function will take SRAM samples once, then from the resultant IQ samples,
  phase info, the frequency offset of the signal is calculated 

DEPENDENCIES
  None

RETURN VALUE
  freq_err_est_data_type- freq error estimate, failure reason & average temps

SIDE EFFECTS
  None
================================================================================*/
extern void tcxomgr_get_freq_err_est
(
  const tcxomgr_freq_err_est_param_type   *est_param,
  tcxomgr_freq_err_est_ret_type           *est_data
);
/*============================================================================

FUNCTION TCXOMGR_SUBSCRIBE_CAL_COEFF_UPDATE

DESCRIPTION
  This function is called to subscribe to the field cal coefficients
  argument: address to the call back function which gets called
            everytime one of the field cal coefficients changes

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

extern void tcxomgr_subscribe_cal_coeff_update
(
  tcxomgr_cal_coeff_subscribe_cb cb_func
);

#endif /* !TCXOMGR_CAL_H */
