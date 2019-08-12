/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc.
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*=============================================================================

                        Session Manager API Header File

GENERAL DESCRIPTION
  gps_consts.h contains system wide GPS constants.
 
 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

All Rights Reserved. QUALCOMM Atheros Proprietary and Confidential.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gps_consts.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/29/13    bz     Added constant definition. 
01/31/07    br     Lint clean-up.
05/03/06    ld     Initial version.

=============================================================================*/

#ifndef GPS_CONSTS_H
#define GPS_CONSTS_H

#include "customer.h"
#include "comdef.h"

#define N_SV            32    /* Number of available GPS satellites */
#define N_ACTIVE_CHAN   16    /* Total number of active channels */

#define WEEK_SECS   (7U*24U*3600U)    /* # of secs in week */ 
#define WEEK_MSECS    (WEEK_SECS*1000U) /* # of msecs in week */
#define WEEK_1P25MSECS (WEEK_SECS*800U) /* # of 1.25msecs in week */
#define WEEK_BITS   (WEEK_SECS*50U)   /* # of bits in week */
#define WEEK_ZCOUNTS  (WEEK_SECS/6U)    /* # of zcounts in week */

#define C_MIN_GPS_WEEK  1738       /* default Minimum GPS week number..
                                      ..maps to Apr 28th 2013 */
#define C_MAX_GPS_WEEK  (32768)    /* Must be less than 0xFFFF..
                                      ..32768 maps to Jan 10th 2608 */

/* Speed of light from ICD */
#define LIGHT_SEC                      (299792458.0)     /* m/s */
#define LIGHT_MSEC              (299792458.0/1000.0)     /* m/s */
#define ONE_OVER_LIGHT_MSEC           (1/LIGHT_MSEC)     /* s/m */

#define C_PI      3.1415926535898   /* From ICD */
#define C_SQRT_PI_OVER_2        1.2533141373155
#define C_RAD_TO_DEG     (180.0 / C_PI)   /* Convert radians to degrees */
#define C_DEG_TO_RAD     (C_PI / 180.0)   /* Convert degrees to radians */

/* Freq error was observed to be much higher in FTM mode (allow phone to boot into FTM mode) */
#define C_MAX_CLK_FREQ_UNC_FTM (20.0e-6*LIGHT_SEC)                   /* == 20 ppm */
#define C_MAX_CLK_FREQ_UNC     (20.0e-6*LIGHT_SEC)                    /* == 20 ppm */
#define C_MAX_CLK_TIME_UNC     (100.0*365.0*24.0*60.0*60.0*1000.0)   /* 100 years */
#define C_MAX_SAT_TIME_UNC     (100.0*365.0*24.0*60.0*60.0*1000.0)   /* 100 years */

#define C_MAX_SAT_SPEED_UNC (900.0)   /* Line of sight, 1 sided m/sec */

/* GPS Signal Conversions */
#define CA_CHIPS_MSEC 1023    /* # of C/A chips per msec */
#define CA_PERIOD_MSEC 1      /* PRN period in milliseconds */
#define CA_FREQ     (1000.0 * CA_CHIPS_MSEC)
#define L1_FREQ     (1540.0 * CA_FREQ) 

#define CA_WAVELENGTH (LIGHT_SEC / CA_FREQ)
#define L1_WAVELENGTH (LIGHT_SEC / L1_FREQ)

/* Meters/Sec to L1 Hz and vice versa */
#define C_MsToL1Hz      ((FLT)(1.0/L1_WAVELENGTH))
#define C_L1HzToMs      ((FLT)(L1_WAVELENGTH))

#define PPM2MPS ((FLT)( 1.0E-6 * LIGHT_SEC ))      /* ppm to m/s conversion */


/* The range of SBAS PRNs supported */
#define C_FIRST_SBAS_SV_PRN  120
#define C_LAST_SBAS_SV_PRN   138

/* The range of QZSS PRNs supported */
#define C_FIRST_QZSS_SV_PRN  193
#define C_LAST_QZSS_SV_PRN   197


/* Number of potential SBAS satellites */
#define N_SBAS_SV  (C_LAST_SBAS_SV_PRN - C_FIRST_SBAS_SV_PRN + 1)

/* Number of potential QZSS satellites */
#define N_QZSS_SV  (C_LAST_QZSS_SV_PRN - C_FIRST_QZSS_SV_PRN + 1)

#define C_MAX_SBAS_SAT_SPEED_UNC (40.0)   /* Line of sight, 1 sided m/sec */

#define C_MAX_QZSS_SAT_SPEED_UNC (600.0)   /* QZSS Sv Unc 1 sided m/sec */

#endif /* GPS_CONSTS_H */


