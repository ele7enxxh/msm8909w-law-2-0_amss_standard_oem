#ifndef __SAR_EFS_H__
#define __SAR_EFS_H__
/*!
  @file lmtsmgr_efs.h

  @brief
   EFS Item support for the SAR

*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/sar/inc/sar_efs.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/12/14   rj      Added support for GPIO polarity and BootUp BackOff
05/29/13   jk      Helper fuctions to read and store the compatibility key from EFS
04/22/13   rj      mcs_limitsmgr.conf EFS file optimization
04/05/13   rj      GPIO Based SAR Control added
08/24/12   ag      EFS Restructuring
05/06/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "lmtsmgr_efs.h"
#include "lmtsmgr_i.h"

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/
#define MAX_SAR_PWR_LUT_ROWS 5
#define MAX_SAR_PWR_LUT_COLUMNS  3

/* Total number of DSIs is 9 but 0 is considered default DSI with input
   and output bias as 0. The LUTs in EFS will only contain 8 bias entries for
   DSI ranging from 1 to 8 */
#define MAX_DSI_EFS_LUT_ROWS 8
#define MAX_DSI_LUT_ROWS 9

#define SAR_GPIO_MIN_POLLING_TIME 20




/*=============================================================================

                             TYPEDEFS

=============================================================================*/

/* In GSM, the power limits are per #UL slots and per modulation type 
   Max there could be 4 UL slots in a frame and 
   2 types of modulation - GMSK and 8PSK.
   So the power limits for each of the below possibilities need to be 
   conveyed to RF
*/
typedef enum
{
   SAR_GSM_GMSK_UL_TYPE,
   SAR_GSM_8PSK_UL_TYPE,
   SAR_GSM_MAX_LIMIT_TYPE
} sar_gsm_num_limits_type;

/* For other techs, there is just one power limit required...
*/
typedef enum
{
  SAR_NOMINAL_LIMIT_TYPE,
  SAR_TECHX_MAX_LIMIT_TYPE
}sar_techx_num_limits_type;

typedef struct
{
  /* GPIO number to read */
  uint32  gpio_num;
  /* Time to poll in millisec */
  uint32  polling_time_ms;
  /* Flag to check if GPIO based DSI is active */
  boolean is_gpio_dsi_active;
  /* Flag to check whether GPIO Polarity is Active Low */
  boolean is_gpio_active_low;
  /* Flag to whether Bootup Backoff is required */
  boolean is_init_backoff;
}sar_gpio_dsi_efs_data;


typedef struct
{
  /* Tx Power limit for high priority client */
  int16   hprio_limits[SAR_GSM_MAX_LIMIT_TYPE];

  /* Number of valid high priority limits */
  uint8   num_valid_hprio_limits;

  /* Number of valid low-prio limits in terms of row & column */
  uint8   num_valid_lut_rows;
  uint8   num_valid_lut_column;

  /* Tx Power limit table for low priority client */
  int16   lprio_limits[MAX_SAR_PWR_LUT_ROWS][MAX_SAR_PWR_LUT_COLUMNS];
}sar_pwr_lut_type;

typedef struct
{
  int16   bias[MAX_DSI_LUT_ROWS][2];
}sar_dsi_lut_type;

/* All types of LUTs applicable to tech-band combination */
typedef struct
{
  sar_pwr_lut_type*      sar_pwr_lut;

  sar_dsi_lut_type*      dsi_lut;

}sar_lut_type;

typedef struct 
{
  const char *sar_pwr_fname;
  const char *dsi_bias_fname;
} sar_efs_fname_type;

typedef struct
{
  sar_efs_fname_type           files;
  /* List of all LUTs for this band combination */
  void*                        luts;
  /* Max bands for this combination */
  uint32                       num_tech1_bands;
  uint32                       num_tech2_bands;
}sar_tech_comb_lut_type;

typedef PACK (struct)
{
  uint32   hprio_band;
  uint32   lprio_band;
  boolean  apply_hpriotech_limit;
  int16    hprio_limits[SAR_GSM_MAX_LIMIT_TYPE];
  uint8    num_valid_lut_rows;
  int16    lprio_limit_lut[MAX_SAR_PWR_LUT_ROWS][2];
}sar_efs_pwr_gplusx_rec_type;

typedef PACK (struct)
{
  uint32   hprio_band;
  uint32   lprio_band;
  boolean  apply_hpriotech_limit;
  int16    hprio_limit;
  uint8    num_valid_lut_rows;
  int16    lprio_limit_lut[MAX_SAR_PWR_LUT_ROWS][2];
}sar_efs_pwr_xplusx_rec_type;

typedef PACK (struct)
{
  uint32   hprio_band;
  uint32   lprio_band;
  boolean  apply_hpriotech_limit;
  int16    hprio_limits[SAR_GSM_MAX_LIMIT_TYPE];
  uint8    num_valid_lut_rows;
  int16    lprio_limit_lut[MAX_SAR_PWR_LUT_ROWS][MAX_SAR_PWR_LUT_COLUMNS];
}sar_efs_pwr_gplusg_rec_type;

typedef PACK (struct)
{
  uint32   hprio_band;
  uint32   lprio_band;
  boolean  apply_hpriotech_limit;
  int16    hprio_limit;
  uint8    num_valid_lut_rows;
  int16    lprio_limit_lut[MAX_SAR_PWR_LUT_ROWS][MAX_SAR_PWR_LUT_COLUMNS];
}sar_efs_pwr_xplusg_rec_type;

typedef PACK (struct)
{
  uint32   hprio_band;
  uint32   lprio_band;
  int16    bias[MAX_DSI_EFS_LUT_ROWS][2];
}sar_efs_dsi_record_type;

typedef PACK (struct)
{
  uint32	 gpio_num;
  uint32	 polling_time_ms;
  boolean    is_gpio_dsi_active;
  boolean    is_gpio_active_low;
  boolean    is_init_backoff;
}sar_gpio_dsi_efs_data_type;


typedef struct 
{
  cxm_tech_type hptech;
  uint32 hpband;
  cxm_tech_type lptech;
  uint32 lpband; 
  int16 tx_power; 
  int16 dsi;
} sar_efs_lookup_in;

typedef struct
{
  int16 hplimits[SAR_GSM_MAX_LIMIT_TYPE];
  uint8 num_hplimits;
  int16 lplimit[SAR_GSM_MAX_LIMIT_TYPE];
  uint8 num_lplimits;
  boolean lplimit_valid;
} sar_efs_lookup_out;


/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  sar_efs_init

=============================================================================*/
/*!
    @brief
    Initializes SAR EFS
 
    @return
    None
*/
/*===========================================================================*/
extern void sar_efs_init (void);


/*=============================================================================

  FUNCTION:  sar_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the current SAR Tx limits.
 
    @return
    None
*/
/*===========================================================================*/
boolean sar_efs_lookup
(
  sar_efs_lookup_in* input,
  sar_efs_lookup_out* output
);

/*=============================================================================

  FUNCTION:  sar_read_gpio_dsi_efs_data

=============================================================================*/
/*!
    @brief
    Reads data related to GPIO for SAR DSI
 
    @return
    GPIO DSI EFS data 
*/
/*===========================================================================*/
sar_gpio_dsi_efs_data sar_read_gpio_dsi_efs_data
( 
  const char *fname
);

/*=============================================================================

  FUNCTION:  sar_efs_check_if_key_valid

=============================================================================*/
/*!
    @brief
    Checks if the EFS in the modem has a valid compatibility key
 
    @return
    1 if key is present
    0 if key is not present
*/
/*===========================================================================*/
uint8 sar_efs_check_if_key_present
(
  void
);

/*=============================================================================

  FUNCTION:  sar_efs_get_comp_key

=============================================================================*/
/*!
    @brief
    Used to get the compatibility key stored in Modem's EFS
 
    @return
    The 32-bit compatibility key
*/
/*===========================================================================*/
uint32 sar_efs_get_comp_key
(
  void
);

/*=============================================================================

  FUNCTION:  sar_efs_deinit

=============================================================================*/
/*!
    @brief
    Frees all the memory associated with LUTs
 
    @return
    None
*/
/*===========================================================================*/
void sar_efs_deinit( void );

#endif /* __SAR_EFS_H__ */
