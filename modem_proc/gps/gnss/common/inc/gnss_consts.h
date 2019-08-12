/*=============================================================================

                        Session Manager API Header File

GENERAL DESCRIPTION
  gnss_consts.h contains system wide GNSS constants.
 
 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/gnss_consts.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/15    am     Removed N_MAX_LUCKY_SV
02/23/09   wjm     Initial version

=============================================================================*/

#ifndef GNSS_CONSTS_H
#define GNSS_CONSTS_H

#include "customer.h"
#include "comdef.h"
#include "gps_consts.h"
#include "loc_wwan_meas_lte_api.h"

/* GPS Constants */
#define N_GPS_SV              N_SV  /* Number of available GPS satellites */
#define N_ACTIVE_GPS_CHAN     16  /* Total number of active GPS channels */
#define GPS_LOWEST_PRN        1   /* Lowest GPS PRN number for internal use */
#define GPS_L1_BW_HZ          (2046000.0) /* GPS L1 bandwidth in Hz */

/* Glonass Constants */
#define N_GLO_SV                  24    /* Number of available Glonass 
                                          satellites */
#define GLO_LOWEST_FREQ_NUMBER    (-7)  /* Lowest valid Glonass frequency 
                                          number */
#define GLO_HIGHEST_FREQ_NUMBER   6     /* Highest valid Glonass frequency 
                                          number */
/* Total number of valid Glonass frequencies */
#define N_GLO_FREQ                (GLO_HIGHEST_FREQ_NUMBER-GLO_LOWEST_FREQ_NUMBER+1)
#define N_ACTIVE_GLO_CHAN         14    /* Total number of active Glonass 
                                          channels */
#define GLO_SLOTID_OFFSET         64    /* Offset between Glonass slot ID and 
                                          the Glonass SV ID used in MGP 
                                          code. */
#define GLO_SLOTID_UNKNOWN        255   /* Glonass slot ID is currently 
                                          invalid */
#define GLO_INVALID_FREQ          (GLO_HIGHEST_FREQ_NUMBER + 1) 
#define GLO_LOWEST_ID             (GLO_SLOTID_OFFSET + 1)
#define GLO_L1_BW_HZ              (8437500.0) /* GLO L1 bandwidth in Hz */

/* BDS-related constants */
#define N_BDS_SV                30 /* Number of available BDS satellites */

/* BDS SV ID offset for internal use */
#define BDS_SV_ID_OFFSET        200
#define BDS_LOWEST_ID           (BDS_SV_ID_OFFSET + 1)

#define N_ACTIVE_BDS_CHAN       16
#define BDS_L1_BW_HZ            (4092000.0) /* BDS L1 bandwidth in Hz */

#define N_ACTIVE_QZSS_SBAS_CHAN   16 /*!< Number of active QZSS + SBAS channels */

/* GAL-related constants */
#define N_GAL_SV                36  /* Number of available GAL satellites */
#define GAL_SV_ID_OFFSET        300
#define N_ACTIVE_GAL_CHAN       16  /* Number of active GAL channels */
#define GAL_LOWEST_ID           (GAL_SV_ID_OFFSET + 1)
#define GAL_L1_BW_HZ            (4092000.0) /* GAL L1 bandwidth in Hz */

/* GNSS Constants */
#define N_MAX_VISIBLE_GNSS_SV  36 
#define GNSS_SVID_INVALID     0   /* SVID is unknown/invalid. Ref. Callisto_Svid_Mapping, Table 2. */ 

#define N_MAX_GNSS_SV         (MAX(MAX(MAX(N_GPS_SV + N_QZSS_SV, N_GLO_SV), N_BDS_SV), N_GAL_SV)) /*!< Max number of SVs of a GNSS system */
#define N_ALL_GNSS_SV         (N_GPS_SV + N_GLO_SV + N_BDS_SV + N_GAL_SV + N_QZSS_SV)  /* Total number of SVs of GNSS systems */

/* Number of TDP channels. 
Note: This will later be set to a macro defined in WWAN ME. 
Currently 7/2014 is set so that TDP pester can be run correctly. */
#define N_ACTIVE_TDP_CHAN       20

/* TDP offset for internal use. Physical IDs in the range 0-503 will
be converted to 1000-1503 for internal use within PE */
#define TDP_PHY_ID_OFFSET       1000

/* Total number of active channels including GPS LBW and Glonass */
#define N_ACTIVE_GNSS_CHAN    (N_ACTIVE_GPS_CHAN + N_ACTIVE_GLO_CHAN + N_ACTIVE_BDS_CHAN +  N_ACTIVE_GAL_CHAN + N_QZSS_SV)

#define N_ACTIVE_MEAS_CHAN    (N_ACTIVE_GNSS_CHAN + N_ACTIVE_TDP_CHAN)

#define MAX_ACTIVE_GNSS_CHAN  (MAX(MAX(MAX(N_ACTIVE_GPS_CHAN, N_ACTIVE_GLO_CHAN), N_ACTIVE_BDS_CHAN), N_ACTIVE_GAL_CHAN))

/* Total number of active channels for NHz Measurements */
#define N_ACTIVE_GPS_CHAN_NHZ 14
#define N_ACTIVE_GLO_CHAN_NHZ 12
#define N_ACTIVE_BDS_CHAN_NHZ 12
#define N_ACTIVE_GAL_CHAN_NHZ 14
#define N_ACTIVE_QZSS_CHAN_NHZ 3    
#define N_ACTIVE_GNSS_CHAN_NHZ   (N_ACTIVE_GPS_CHAN_NHZ + N_ACTIVE_GLO_CHAN_NHZ + N_ACTIVE_BDS_CHAN_NHZ +  N_ACTIVE_GAL_CHAN_NHZ + N_ACTIVE_QZSS_CHAN_NHZ)

/* The range of BDS PRNs supported */ 
#define C_FIRST_BDS_D2_SV_PRN 1 
#define C_LAST_BDS_D2_SV_PRN 5
 
/* Number of potential BDS D2 satellites */ 
#define N_BDS_D2_SV (C_LAST_BDS_D2_SV_PRN - C_FIRST_BDS_D2_SV_PRN + 1)
 


/* GLO Signal Conversions */
#define GPS_UTC_OFFSET        (17U)             /* The leap second offset on
                                                  July-2015 */
#define GLO_UTC_SECS          (3U*3600U)        /* # of secs GLONASS is 
                                                  ahead of UTC (3 hrs) */
#define GLO_UTC_MSECS         (GLO_UTC_SECS*1000U)  /* # of msecs GLONASS is 
                                                  ahead of UTC (3 hrs) */
#define GLO_GPS_DAYS          (5839U)           /* # of days the GPS time 0 
                                                  preceeds GLONASS time 0 */
#define FOURYEAR_DAYS         (1461U)           /* # of days in 4-year cycle*/

#define MIN_SECS              (60U)             /* # of secs in one minute */
#define HOUR_MINS             (60U)             /* # of minutes in one hour */
#define HOUR_SECS             (3600U)           /* # of secs in one hour*/
#define DAY_HOURS             (24U)             /* # of hours in day */
#define DAY_SECS              (24U*HOUR_SECS)   /* # of secs in day */
#define DAY_MSECS             (DAY_SECS*1000U)  /* # of msecs in day */
#define FOURYEAR_SECS         (FOURYEAR_DAYS*DAY_SECS)   /* # of SEC in 4-year cycle*/
#define LEAPYEAR_DAYS         366U               /* # days in a leap year */
#define YEAR_DAYS             365U               /* # days in a year */
#define GPS_MINUTES_IN_WEEK       ((86400.0 * 7)/60)
#define GPS_SECONDS_IN_DAY        86400.0
#define GPS_SECONDS_IN_HOUR       3600.0
#define GPS_SECONDS_IN_MINUTE     60.0
#define DAYS_IN_ONE_WEEK          7
#define HOURS_IN_WEEK             (7 * 24)
#define NUM_DAYS_FRST_SIX_MTHS           181
#define NUM_DAYS_FRST_SIX_MTHS_LEAPYR    182
#define NUM_DAYS_LAST_SIX_MTHS           184
#define SECS_TO_MSECS             1000
#define SECS_TO_NSECS			  (1e9)
#define MSECS_TO_SECS             0.001
#define MICRO_SEC_IN_MILLI_SEC    1000
#define CM_IN_METER               100

/* Number of milliseconds between January 1, 1970 and January 6, 1980
  (3657 * 24 * 60 * 60 * 1000) */
#define GPS_TO_UTC_DELTA_MSECS       (315964800000ULL)

#define GLO_CA_CHIPS_MSEC  511   /* # of C/A chips per msec */
#define GLO_CA_PERIOD_MSEC 1     /* chip period in milliseconds */
#define GLO_CA_FREQ           (1000.0 * CA_CHIPS_MSEC)
#define DAY_BITS   (DAY_SECS*50U)               /* # of bits in day */
#define DAY_TKCOUNT (DAY_SECS/30U)              /* # of tk count in a day */
#define GLO_FRAME_SEC (30U)                     /* # of seconds in a Glonass 
                                                  frame */
#define GLO_L1_FREQ_HZ 1602000000              /* Glonass L1 in Hz used by RF 
                                                   driver during 
                                                   downconversion */

#define C_GLO_STR_DATA_LEN          (100)   /*One Glo string can have 100 bits*/
#define C_TIMEMARK_MS               (300) /*Timemark duration is 300ms*/


#define C_MAX_GLO_SAT_SPEED_UNC C_MAX_SAT_SPEED_UNC


/* BDS Signal Conversions */
#define BDS_CA_CHIPS_MSEC      2046  /* # of C/A chips per msec */
#define BDS_CA_PERIOD_MSEC     1     /* chip period in milliseconds */
#define BDS_CA_FREQ           (1000.0 * BDS_CA_CHIPS_MSEC)
#define B1_FREQ               (763.0 * BDS_CA_FREQ)

#define B1_WAVELENGTH           (LIGHT_SEC / B1_FREQ)

/* Meters/Sec to B1 Hz and vice versa */
#define C_MsToB1Hz      (FLT)(1.0/B1_WAVELENGTH)
#define C_B1HzToMs      (FLT)(B1_WAVELENGTH)

#define C_MAX_BDS_SAT_SPEED_UNC C_MAX_SAT_SPEED_UNC

#define GPS_BDS_OFFSET_WEEK_SECS  14  /* difference in secs-in-a week between BDS and GPS
                                        GPS_BDS_OFFSET_WEEK_SECS = GPS_week_secs - BDS_week_secs */
#define GPS_BDS_DAYS              (9492U) /* number of days that GPS is ahead of BDS */
#define GPS_BDS_WEEKS             (GPS_BDS_DAYS/DAYS_IN_ONE_WEEK) /* number of weeks that
                                        GPS is ahead of BDS */
#define BDS_GPS_ADVANCE_SECS      (GPS_BDS_WEEKS * WEEK_SECS) +  GPS_BDS_OFFSET_WEEK_SECS
                                  /* number of secs that GPS is ahead of BDS */
#define GPS_GAL_WEEKS  (1024) /* number of weeks that GPS is ahead of GAL */

/* Galileo Signal */
#define GAL_CA_CHIPS_MSEC   1023 /* # of PRN chips per msec */
#define GAL_CA_PERIOD_MSEC  4    /* PRN period in milliseconds */

#define C_MAX_GAL_SAT_SPEED_UNC C_MAX_SAT_SPEED_UNC /*!< Line of sight, 1 sided m/sec */

/* The ICD-GPS-200 document provides these earth constants to convert from
 * the ECEF coordiante system into the WGS-84 lat, long, alt system.
*/
#define C_WGS84_A   6378137.000   /* Semi-major axis (m). (to equator)*/
#define C_WGS84_B   6356752.314   /* Semi-minor axis (m). (to poles)  */

/* Earth's obliquity and flatness terms. */
#define C_WGS84_F   (C_WGS84_A - C_WGS84_B) / C_WGS84_A
#define C_WGS84_1MF (1.0 - C_WGS84_F)
#define C_WGS84_R2  ((C_WGS84_B * C_WGS84_B) / (C_WGS84_A * C_WGS84_A))
#define C_WGS84_E2  (1.0 - (C_WGS84_B * C_WGS84_B) / (C_WGS84_A * C_WGS84_A))

/* World Geodetic Survey, 1984 - Earth's rotation rate (rad/sec). */		
/* Used by both cd and nf */
#define C_WGS84_ROTATION           7.2921151467e-5
#define C_CGCS2000_EARTH_ROTATION  7.292115e-5  /* defined for BDS */

/* mu, earth's gravitational constant, */
#define C_SQRT_EARTH_GRAV_CON         ((DBL)1.996498184321739e7)
#define C_CGCS200_SQRT_EARTH_GRAV_CON ((DBL)1.996498038566530e7) /* defined for BDS and GAL */

/* GPS relativistic clock correction constant. */
#define C_GPS_F_RELATIVITY       ((DBL) -4.442807633e-10)

/* some PZ90 Earth reference ellipsoide constants */
#define C_PZ90A            (DBL)6378136.0   /* the semi-major axis in meters*/
#define C_PZ90MIU          (DBL)398600.4418e+009 /* Gravitational constant in
                                                     meters^3 / second^2 */
#define C_PZ90OMEGADOTE    (DBL)7.292115e-5 /* Earth rotation rate, rad/s */
#define C_PZ90C20          ((DBL)-1.08262575e-3) /* 2nd zonal harmonic of
                                                     geopotential J20 */

/* GAL constants */
#define C_GAL_NOMINAL_SQRTA_METERS  5440.588203494177338011974 /* Nominal semi-major axis according to GAL ICD 29600 km */
#define C_GAL_NOMINAL_I0            0.977384381116825 /* Nominal inclination according to GAL ICD 56 degrees */
#define GRAVITY_MPSS_SQ             9.80665*9.80665 // accel due to gravity at sea level squared

#endif /* GNSS_CONSTS_H */


