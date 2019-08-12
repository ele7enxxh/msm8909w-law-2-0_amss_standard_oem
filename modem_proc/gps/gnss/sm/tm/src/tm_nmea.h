#ifndef LM_NMEA_H
#define LM_NMEA_H
/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          NMEA Header File

GENERAL DESCRIPTION
  This header file contains all the prototypes and definitions necessary for
  the NMEA data to be properly output.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $$
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/14   jv       Galileo support
09/14/07   gk       NMEA CONFIG set parameters support
08/07/06   mrawal   File created
===========================================================================*/

#include "gps_variation.h"
#include "customer.h"  /* Customer configuration file */
#include "sm_api.h"
#include "sio.h" 
#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"
#endif /* FEATURE_RUNTIME_DEVMAP */
#include "tm_lm_iface.h"

/*===========================================================================
  EXTERNALS
===========================================================================*/
typedef enum
{
  TM_NMEA_DIAG_OPEN_PORT_UART1,
  TM_NMEA_DIAG_OPEN_PORT_UART2,    
  TM_NMEA_DIAG_OPEN_PORT_USB,
  TM_NMEA_DIAG_OPEN_PORT_SMD,
  TM_NMEA_DIAG_OPEN_PORT_NONE, /* Close port */
} tm_nmea_diag_open_port_id_e_type;


typedef enum
{
  TM_NMEA_SYSTEM_ID_DEFAULT = 0x0, 
  TM_NMEA_SYSTEM_ID_GP   = 0x01, 
  TM_NMEA_SYSTEM_ID_GL   = 0x02,  
  TM_NMEA_SYSTEM_ID_GA   = 0x03,
  TM_NMEA_SYSTEM_ID_BDS  = 0x04,
  TM_NMEA_SYSTEM_ID_QZSS = 0x05,
  TM_NMEA_SYSTEM_ID_LAST = 0x0F
}tm_nmea_system_id_e_type;


/*===========================================================================
  FUNCTION PROTOTYPES
===========================================================================*/
/*===========================================================================

FUNCTION tm_nmea_open_cb

DESCRIPTION
  This callback is called when RDM wants NMEA to open a port.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_nmea_open_cb (sio_port_id_type port_id);


/*===========================================================================

FUNCTION tm_nmea_close_cb

DESCRIPTION
  This callback is called when RDM wants NMEA to close its port.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_nmea_close_cb (void);

/*===========================================================================

FUNCTION tm_nmea_sio_init

DESCRIPTION
  This function contains the initialization for the NMEA sio.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_nmea_sio_init (void);

/*===========================================================================

FUNCTION tm_nmea_open_handler

DESCRIPTION
  This function is used to open NMEA message output port.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_nmea_open_handler(void *p_data);


/*===========================================================================

FUNCTION tm_nmea_close_handler

DESCRIPTION
  This function is used to close NMEA message's output port .

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_nmea_close_handler(void);

/*===========================================================================
FUNCTION tm_nmea_get_gps_fix_from_refpos

This function takes a reference position of type gps_RefLocStructType and
populates a sm_GpsFixRptStructType with only the fields required to generate 
NMEA strings. This is used in case of MSA where the final fix from the server
doesn't contain all the information required to generate NMEA.
   
DESCRIPTION

DEPENDENCIES

RETURN VALUE sm_GpsFixRptStructType* 

SIDE EFFECTS
 
===========================================================================*/
extern void tm_nmea_get_gps_fix_from_refpos(const gps_RefLocStructType *p_refpos,
                                            sm_GnssFixRptStructType *p_fix);

/*===========================================================================
FUNCTION tm_nmea_gpgga_put

DESCRIPTION

 tm_nmea_gpgga_put() is used to build and transmit the NMEA GGA sentence. 
 
 Global Positioning System Fix Data.
 
  Time, position and fix related data for a GPS receiver.
 
  1 2   3         4       5 6        7 8 9  10 11  12 13 14 15  16   17
  | |   |         |       | |        | | |  |   |   | |   | |   |    |
 $aaccc,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>
 
 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Universal Time Coordinated (UTC) of position.
  4) Latitude  (DDmm.mm)    DD = two digits of degrees, mm.mm = minutes.
  5) N or S (North or South).
  6) Longitude (DDDmm.mm).
  7) E or W (East or West).
  8) GPS Quality Indicator,
     0 - fix not available or invalid,
     1 - GPS SPS Mode, fix valid,
     2 - Differential GPS, SPS Mode, fix valid
     3 - GPS PPS Mode, fix valid
     4 - Real Time Kinematic:  Satellite system used in RTK mode with fixed
         integers,
     5 - Float RTK:  Satellite system used in RTK mode, floating integers
     6 - Estimated (dead reckoning) Mode,
     7 - Manual Input Mode,
     8 - Simulator Mode.
  9) Number of satellites in use, 00 - 12.
 10) Horizontal Dilution of Precision (HDOP).
 11) Antenna Altitude above/below mean-sea-level (geoid), meters.
 12) Units of antenna altitude, meters.
 13) Geoidal separation, the difference between the WGS-84 earth ellipsoid and
     the mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid.
 14) Units of geoidal separation, meters.
 15) Age of differential GPS data, time in seconds since last RTCM-SC104 
     Type 1 or Type 9 update (null field when DGPS is not used).
 16) Differential reference station ID, 0000-1023.
 17) Checksum

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
extern void tm_nmea_gpgga_put( const sm_GnssFixRptStructType *p_Fix,  boolean u_no_fix  );

/*===========================================================================
FUNCTION tm_nmea_gngns_put

DESCRIPTION

 tm_nmea_gngns_put() is used to build and transmit the NMEA GGA sentence. 
 
 GNSS Fix data
 
  Time, position and fix related data for a GNSS receiver.
 
  1 2   3         4       5 6        7 8    9  10  11  12  13  14  15  
  | |   |         |       | |        | |    |  |   |   |   |   |   |   
 $aaccc,hhmmss.ss,llll.ll,a,yyyyy.yy,a,c--c,xx,x.x,x.x,x.x,x.x,x.x*hh<CR><LF>
 
 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Universal Time Coordinated (UTC) of position.
  4) Latitude  (DDmm.mm)    DD = two digits of degrees, mm.mm = minutes.
  5) N or S (North or South).
  6) Longitude (DDDmm.mm).
  7) E or W (East or West).
  8) Mode Indicator,
     N - fix not available or invalid,
     A - Autonomous. Satellite system used in non-differential mode in position fix
     D - Differential GPS, SPS Mode, fix valid
     P - GPS PPS Mode, fix valid
     R - Real Time Kinematic:  Satellite system used in RTK mode with fixed
         integers,
     F - Float RTK:  Satellite system used in RTK mode, floating integers
     E - Estimated (dead reckoning) Mode,
     M - Manual Input Mode,
     S - Simulator Mode.
  9) Number of satellites in use, 00 - 12.
 10) Horizontal Dilution of Precision (HDOP).
 11) Antenna Altitude above/below mean-sea-level (geoid), meters.
 12) Geoidal separation, the difference between the WGS-84 earth ellipsoid and
     the mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid.
 13) Age of differential GPS data, time in seconds since last RTCM-SC104 
     Type 1 or Type 9 update (null field when DGPS is not used).
 14) Differential reference station ID.
 15) Checksum

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS
 
===========================================================================*/
void tm_nmea_gngns_put( const sm_GnssFixRptStructType *p_fix ,boolean u_no_fix );

/*===========================================================================
FUNCTION tm_nmea_pqxfi_put

DESCRIPTION

tm_nmea_pqxfi_put() is used to build and transmit the NMEA 
Proprietary Qualcomm eXtended Fix Information message. 

Global Positioning System Extended Fix Data.

Extended fix related data for a GPS receiver.

1     2         3       4 5        6        7 8 9  10 11  12 13 14 15  16   17
|     |         |       | |        |        | | |  |   |   | |   | |   |    |
$PQXFI,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>

Field Number: 
1) Proprietary Qualcommm eXtended Fix Information.
2) Universal Time Coordinated (UTC) of position.
3) Latitude  (DDmm.mm)    DD = two digits of degrees, mm.mm = minutes.
4) N or S (North or South).
5) Longitude (DDDmm.mm).
6) E or W (East or West).
7) Antenna Altitude above/below mean-sea-level (geoid), meters.
8) HEPE (m, ~63% confidence, 2D horizontal error estimate = root-sum-square of 68% confidence east error estimate and north error estimate)
9) Vertical Uncertainty (m, 68% confidence, 1-sided, 1-dimensional)
10) Velocity Uncertainty (m/s, 20% confidence, 3D = root-sum-square of 68% confidence error estimates on east, north & up velocities) 
11) Checksum

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_CGPS_LBS_TASK
extern void tm_nmea_pqxfi_put( const sm_GnssFixRptStructType *p_Fix,  boolean u_no_fix  );
#endif


/*===========================================================================
FUNCTION tm_nmea_gagsv_put

DESCRIPTION
  tm_nmea_gpgsv_put() is used to transmit the GSV - Galileo Satellites in View.

     Number of satellites (SVs) in view, satellite ID numbers, elevation, 
 azimuth, and SNR value. Four satellites maximum per transmission, additional
 satellite data sent in the second or third message.

  1 2   3 4 5  6  7  8   9      10      11 12 13  14 15
  | |   | | |  |  |  |   |       |      |  |  |   |  | 
 $aaccc,x,x,xx,xx,xx,xxx,xx,.. .. .. ..,xx,xx,xxx,xx*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Total number of messages, 1 to 3.
  4) Message number, 1 to 3.
  5) Total number of satellites in view.
  6) SV1 - Satellite ID number.
  7) SV1 - Elevation, degrees, 90 degrees maximum.
  8) SV1 - Azimuth, degrees True, 000 to 359.
  9) SV1 - SNR (C/No) 00-99 dB, NULL when not tracking.
 10) SV2 to SV3.
 11) SV4 - Satellite ID number.
 12) SV4 - Elevation, degrees, 90 degrees maximum.
 13) SV4 - Azimuth, degrees True, 000 to 359.
 14) SV4 - SNR (C/No) 00-99 dB, NULL when not tracking.
 15) Checksum

     Null fields are not required for unused sets when less then four sets
 are transmitted. 

     Galileo satelites are identified by their PRN numbers, ranging 1 to 36.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_gagsv_put(const sm_GpsMeasRptStructType * p_meas);

/*===========================================================================
FUNCTION tm_nmea_gpgsv_put

DESCRIPTION
  tm_nmea_gpgsv_put() is used to transmit the GSV - GPS Satellites in View.

     Number of satellites (SVs) in view, satellite ID numbers, elevation, 
 azimuth, and SNR value. Four satellites maximum per transmission, additional
 satellite data sent in the second or third message.

  1 2   3 4 5  6  7  8   9      10      11 12 13  14 15
  | |   | | |  |  |  |   |       |      |  |  |   |  | 
 $aaccc,x,x,xx,xx,xx,xxx,xx,.. .. .. ..,xx,xx,xxx,xx*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Total number of messages, 1 to 3.
  4) Message number, 1 to 3.
  5) Total number of satellites in view.
  6) SV1 - Satellite ID number.
  7) SV1 - Elevation, degrees, 90 degrees maximum.
  8) SV1 - Azimuth, degrees True, 000 to 359.
  9) SV1 - SNR (C/No) 00-99 dB, NULL when not tracking.
 10) SV2 to SV3.
 11) SV4 - Satellite ID number.
 12) SV4 - Elevation, degrees, 90 degrees maximum.
 13) SV4 - Azimuth, degrees True, 000 to 359.
 14) SV4 - SNR (C/No) 00-99 dB, NULL when not tracking.
 15) Checksum

     Null fields are not required for unused sets when less then four sets
 are transmitted. 

     GPS satelites are identified by their PRN numbers, ranging 1 to 32.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
extern void tm_nmea_gpgsv_put(const sm_GpsMeasRptStructType * p_meas_gps, const sm_GpsMeasRptStructType * p_meas_sbas);

/*===========================================================================
FUNCTION tm_nmea_glgsv_put

DESCRIPTION
  tm_nmea_glgsv_put() is used to transmit the GSV - GPS Satellites in View.

     Number of satellites (SVs) in view, satellite ID numbers, elevation, 
 azimuth, and SNR value. Four satellites maximum per transmission, additional
 satellite data sent in the second or third message.

  1 2   3 4 5  6  7  8   9      10      11 12 13  14 15
  | |   | | |  |  |  |   |       |      |  |  |   |  | 
 $aaccc,x,x,xx,xx,xx,xxx,xx,.. .. .. ..,xx,xx,xxx,xx*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Total number of messages, 1 to 3.
  4) Message number, 1 to 3.
  5) Total number of satellites in view.
  6) SV1 - Satellite ID number.
  7) SV1 - Elevation, degrees, 90 degrees maximum.
  8) SV1 - Azimuth, degrees True, 000 to 359.
  9) SV1 - SNR (C/No) 00-99 dB, NULL when not tracking.
 10) SV2 to SV3.
 11) SV4 - Satellite ID number.
 12) SV4 - Elevation, degrees, 90 degrees maximum.
 13) SV4 - Azimuth, degrees True, 000 to 359.
 14) SV4 - SNR (C/No) 00-99 dB, NULL when not tracking.
 15) Checksum

     Null fields are not required for unused sets when less then four sets
 are transmitted. 

     GPS satelites are identified by their PRN numbers, ranging 1 to 32.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
extern void tm_nmea_glgsv_put(const sm_GpsMeasRptStructType * p_Meas);

/*===========================================================================
FUNCTION tm_nmea_pqgsv_put

DESCRIPTION
  tm_nmea_gpgsv_put() is used to transmit the GSV - GNSS Satellites in View.

     Number of satellites (SVs) in view, satellite ID numbers, elevation, 
 azimuth, and SNR value. Four satellites maximum per transmission, additional
 satellite data sent in the second or third message.

  1 2   3 4 5  6  7  8   9      10      11 12 13  14 15  16
  | |   | | |  |  |  |   |       |       |  |  |   |  |   | 
 $aaccc,x,x,xx,xx,xx,xxx,xx,.. .. .. ..,xx,xx,xxx,xx,h,h*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Total number of messages, 1 to 20.
  4) Message number, 1 to 20.
  5) Total number of satellites in view.
  6) SV1 - Satellite ID number. (3 digits, proprietary)
  7) SV1 - Elevation, degrees, 90 degrees maximum.
  8) SV1 - Azimuth, degrees True, 000 to 359.
  9) SV1 - SNR (C/No) 00-99 dB, NULL when not tracking.
 10) SV2 to SV3.
 11) SV4 - Satellite ID number.
 12) SV4 - Elevation, degrees, 90 degrees maximum.
 13) SV4 - Azimuth, degrees True, 000 to 359.
 14) SV4 - SNR (C/No) 00-99 dB, NULL when not tracking.
 15) Signal ID (0 for all signals) 
 16  GNSS System ID (BDS:4, QZSS:5)
 16) Checksum

     Null fields are not required for unused sets when less then four sets
 are transmitted. 

     BDS and QZSS satelites are identified by their PRN numbers. BDS: 1-37 (offset -200), QZSS: 1-4 (offset -192)

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_pqgsv_put(const sm_GpsMeasRptStructType * p_meas);
   
/*===========================================================================
FUNCTION tm_nmea_gpvtg_put

DESCRIPTION
  tm_nmea_gpvtg_put() is used to transmit VTG - Course Over Ground and Ground Speed.
     
    The actual course and speed relative to the ground.

  1 2   3   4 5   6 7   8 9  10 11
  | |   |   | |   | |   | |   | |
 $aaccc,x.x,T,x.x,M,x.x,N,x.x,K*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Course, degrees.
  4) T = True.
  5) Course, degrees.
  6) M = Magnetic.
  7) Speed, knots.
  8) N = Knots.
  9) Speed Kilometers Per Hour.
 10) K = Kilometers Per Hour.
 11) Checksum.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
extern void tm_nmea_gpvtg_put( const sm_GnssFixRptStructType *p_Fix );

/*===========================================================================
FUNCTION tm_nmea_gpgsa_put

DESCRIPTION
  tm_nmea_gpgsa_put() is used to transmit GSA - GPS DOP and Active Satellites.
     

  1 2   3 4 5                   6   7   8   9
  | |   | | |                                   |   |   |   |
 $aaccc,d,e,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Mode, M = Manual, A = Automatic  2D/3D.
  4) Mode, 1 = No fix, 2 = 2D, 3 = 3D.
  5) PRN used in the position solution.  Using "null" (",") for no SV for
     a total of 12 tokens.
  6) PDOP - Position Dilution Of Precision.
  7) HDOP - Horizontal Dilution Of Precision.
  8) VDOP - Vertical Dilution Of Precision.
  9) Checksum.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

 Notes: If we send a 2D position to Street Altas, we will get a blinking 
 question mark sitting on the satellite icon within the 'GPS Status' window.
 I assume the program thinks that we've lost too many SVs and we're now
 in 2D mode.  We only set the 2D mode if we only have 3 SVs with an altitude-
 hold meaurement.  In 2D mode, the positions on the map will be in yellow.
 
===========================================================================*/
extern void tm_nmea_gpgsa_put( const sm_GnssFixRptStructType *p_Fix );


/*===========================================================================
FUNCTION tm_nmea_gngsa_put

DESCRIPTION
  tm_nmea_gngsa_put() is used to transmit GSA - GPS DOP and Active Satellites.
     

  1 2   3 4 5                   6   7   8   9
  | |   | | |                                   |   |   |   |
 $aaccc,d,e,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Mode, M = Manual, A = Automatic  2D/3D.
  4) Mode, 1 = No fix, 2 = 2D, 3 = 3D.
  5) PRN used in the position solution.  Using "null" (",") for no SV for
     a total of 12 tokens.
  6) PDOP - Position Dilution Of Precision.
  7) HDOP - Horizontal Dilution Of Precision.
  8) VDOP - Vertical Dilution Of Precision.
  9) Checksum.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

 Notes: If we send a 2D position to Street Altas, we will get a blinking 
 question mark sitting on the satellite icon within the 'GPS Status' window.
 I assume the program thinks that we've lost too many SVs and we're now
 in 2D mode.  We only set the 2D mode if we only have 3 SVs with an altitude-
 hold meaurement.  In 2D mode, the positions on the map will be in yellow.
 
===========================================================================*/
extern void tm_nmea_gngsa_put( const sm_GnssFixRptStructType *p_Fix );

/*===========================================================================
FUNCTION tm_nmea_pqgsa_put

DESCRIPTION
  tm_nmea_pqgsa_put() is used to transmit proprietary GSA - BDS/QZSS DOP and Active Satellites.
     

  1 2   3 4 5                                   6   7   8    9  10
  | |   | | |                                   |   |   |    |  |
 $aaccc,d,e,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x,[h]*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type.
  3) Mode, M = Manual, A = Automatic  2D/3D.
  4) Mode, 1 = No fix, 2 = 2D, 3 = 3D.
  5) PRN used in the position solution.  Using "null" (",") for no SV for
     a total of 12 tokens.
  6) PDOP - Position Dilution Of Precision.
  7) HDOP - Horizontal Dilution Of Precision.
  8) VDOP - Vertical Dilution Of Precision.
  9) [GNSS System ID for PQGSA only. BDS:4 QZSS:5]
  10) Checksum.

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

 Notes: If we send a 2D position to Street Altas, we will get a blinking 
 question mark sitting on the satellite icon within the 'GPS Status' window.
 I assume the program thinks that we've lost too many SVs and we're now
 in 2D mode.  We only set the 2D mode if we only have 3 SVs with an altitude-
 hold meaurement.  In 2D mode, the positions on the map will be in yellow.
 
===========================================================================*/
void tm_nmea_pqgsa_put( const sm_GnssFixRptStructType *p_fix );


/*===========================================================================
FUNCTION tm_nmea_gprmc_put
 
DESCRIPTION

 tm_nmea_gprmc_put() is used to build and transmit the NMEA RMC sentence. 

 Global Positioning System Fix Data.

 Time, position and fix related data for a GPS receiver.

  1 2   3         4 5       6 7        8 9  10  11     12  13 14 15
  | |   |         | |       | |        | |   |   |      |   | |  | 
 $aaccc,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxxxx,x.x,a,a*hh<CR><LF>

 Field Number: 
  1) Talker ID.
  2) Sentence format/type. (RMC)
  3) Universal Time Coordinated (UTC) of position.
  4) Status (A = Valid, V = Invalid)
  5) Latitude  (DDmm.mm)		DD = two digits of degrees, mm.mm = minutes.
  6) N or S (North or South).
  7) Longitude (DDDmm.mm).
  8) E or W (East or West).
  9) Speed (Knots)
 10) Course. Degrees True.
 11) Date ddmmyy
 12) Magnetic variation 
 13) Mag variation direction. (E/W). E subtracts mag var from true, W adds mag var to true.
 14) Mode indicator. 
 15) Checksum

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
extern void tm_nmea_gprmc_put(const sm_GnssFixRptStructType *p_Fix );

/*===========================================================================
FUNCTION tm_nmea_pstis_put

DESCRIPTION
  tm_nmea_pstis_put() is used to transmit PSTIS - GPS session start indication
     
  12      3    
  ||      |      
  $PSTIS,*hh<CR><LF>

  Field Description: 
  1) Talker ID.
  2) PSTIS for Proprietary SnapTrack Inc. session control message.
  3) Checksum


DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
void tm_nmea_pstis_put(void);

/*===========================================================================

FUNCTION tm_nmea_diag_assign_port

DESCRIPTION
  This function opens NMEA port.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_nmea_diag_open_port(tm_nmea_diag_open_port_id_e_type port_to_open);

/*===========================================================================

FUNCTION tm_nmea_diag_close_port

DESCRIPTION
  This function closes NMEA port

DEPENDENCIES
  None.

RETURN VALUE
  None.  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_nmea_diag_close_port(void (*close_func_ptr)(void));

/*===========================================================================

FUNCTION tm_nmea_open_handler_part2

DESCRIPTION
  Handles opening the NMEA port after asynchronous close of 
  already opened port.

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_nmea_open_handler_part2(void);

/*===========================================================================

FUNCTION tm_nmea_diag_open_port_part2

DESCRIPTION
  Handles opening the NMEA diag port after asynchronous close of 
  already opened port.

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_nmea_diag_open_port_part2(void);

/*===========================================================================
FUNCTION tm_nmea_debug

DESCRIPTION
  This function sends the debug NMEA sentences to LBS and also
  on the NMEA COM port if it is open.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_nmea_debug(tm_nmea_data_s_type* pDebugNMEA);

/*===========================================================================
FUNCTION tm_nmea_set_gnss_meas_update

DESCRIPTION
  Stores the FCount from measurement report

DEPENDENCIES

RETURN VALUE none

SIDE EFFECTS

===========================================================================*/
extern void tm_nmea_set_gnss_meas_update(const sm_GpsMeasRptStructType* p_Meas);

#endif /* LM_NMEA_H */
