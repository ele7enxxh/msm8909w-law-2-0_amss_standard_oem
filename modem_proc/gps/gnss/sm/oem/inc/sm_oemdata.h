#ifndef OEMDATA_H
#define OEMDATA_H
/*=============================================================================

                        OEM Data Header File

GENERAL DESCRIPTION
  This file contains contains the structure definitions and function prototypes for 
  the OEM Data.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/oem/inc/sm_oemdata.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/11   rh      Initial Version: provide PRM/PPM/SA/AA data in OEM layer

=============================================================================*/


/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "sm_api.h"
#include "gnss_common.h"
#include "qw.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

/*=============================================================================

       Macros

=============================================================================*/

/*=============================================================================

       Typedefs

=============================================================================*/

/******************************************************************************
 *   GPS SENSITIVITY ASSISTANCE OEM data, originally defined in internal      *
 *   header file tm_paii.h(paii_sa_data_type) & tm_is801.h(is801_aa_fix_type) *
 *            oemdata_is801_sa_data_type MUST BE IDENTICAL !!!                *
 ******************************************************************************/

#define OEMDATA_SA_MAX_DATA_RECORDS            16  /* max data recs in SA msg (IS801) */
#define OEMDATA_SA_MAX_NUM_SATS_IN_DATA_RECORD 16  /* max sats per data rec (IS801) */
#define OEMDATA_SA_MAX_NAV_BITS_IN_BYTES       64  /* max data record size */

typedef struct
{
  uint16  ref_bit_num;                           /* Reference bit number */
  byte    dr_size;                                   /* Data record size */
  byte    nav_msg_bits[OEMDATA_SA_MAX_NAV_BITS_IN_BYTES];/* Navigation message bits */
  byte    num_sv_dr;     /* Number of GPS satellites in this data record */
  byte    sv_prn_num[OEMDATA_SA_MAX_NUM_SATS_IN_DATA_RECORD]; /* Satellite PRN nums */
} oemdata_is801_sa_dr_type;
  
typedef struct
{
  qword                     timestamp; /* CDMA time that the data was received */    
  byte                      num_data_records;            /* Number of data records */
  oemdata_is801_sa_dr_type  data[OEMDATA_SA_MAX_DATA_RECORDS];             /* SA data records */
} oemdata_is801_sa_data_type;


/******************************************************************************
 *   GPS ACQUISITION ASSISTANCE OEM data, originally defined in internal      *
 *   header file tm_paii.h(paii_aa_data_type) & tm_is801.h(is801_aa_fix_type) *
 *            oemdata_is801_aa_fix_type MUST BE IDENTICAL !!!                 *
 ******************************************************************************/

#define OEMDATA_AA_MAX_NUM_SATS    32    /* max sats in AA msg (IS801) is at 16 */
                                        /* max sats in Demod AA and Meas AA is at 32 */
typedef struct
{
  byte    num_sv;                            /* Number of GPS satellites */
  byte    time_of_app;       /* CDMA system time ref for the GPS AA info */
  byte    dopp_incl;                /* Doppler (0th order) term included */
  byte    add_dopp_incl;             /* Additional doppler info included */
  byte    code_ph_par_incl;                  /* Code phase info included */
  uint16  reference_pn;                                  /* Reference PN */
  byte    az_el_incl;            /* Azimuth and elevation angle included */
} oemdata_is801_aa_fix_type;
  
typedef struct
{
  byte    sv_prn_num;                            /* Satellite PRN number */      
  sint15  doppler0;                                 /* 0th order doppler */
  int1    doppler1;                                 /* 1st order doppler */
  uint16  doppler_win_0_5_hz;                   /* Doppler search window */
  uint16  sv_code_ph;                                  /* GPS code phase */
  byte    sv_code_ph_int;              /* Integer number of code periods 
                                             since last GPS bit boundary */
  byte    gps_bit_num; /* 2 least significant bits of bit number (within 
                              the GPS frame) currently being transmitted */
  uint16  sv_code_ph_win;                     /* Total code phase window */
  byte    azimuth;                       /* Azimuth of the GPS satellite */
  byte    elevation;             /* Elevation angle of the GPS satellite */
} oemdata_is801_aa_var_type;
  
typedef struct
{
  qword                        timestamp; /* CDMA time that the data was received */
  oemdata_is801_aa_fix_type    fix;    
  oemdata_is801_aa_var_type    var[OEMDATA_AA_MAX_NUM_SATS];    /* Satellite AA records */
} oemdata_is801_aa_data_type;


/* Type of OEMDR Data reported from SM to OEM layer */
typedef enum
{
  OEMDATA_UNKNOWN,
  OEMDATA_PPM_DATA,
  OEMDATA_PRM_DATA,
  OEMDATA_SENSTIVITY_ASSIST_DATA,
  OEMDATA_ACQUISITION_ASSIST_DATA
} oemData_dataType;

typedef struct
{
  oemData_dataType                  oemDataType;
  union {
    pdsm_srch_ppm_rpt_type      oemPPMData;
    pdsm_srch_gps_sat_rpt_type  oemPRMData;
    oemdata_is801_sa_data_type  oemSAData;
    oemdata_is801_aa_data_type  oemAAData;
  } u;
} oemData_data;

/*=============================================================================

       Functions

=============================================================================*/

/*****************************OEM Data API functions***********************/


/*===========================================================================

FUNCTION oemData_ProcessData

DESCRIPTION
  OEM should process the OEM data inside this function
  
DEPENDENCIES
  None.

PARAMETERS
  pOemData - OEM data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void oemData_ProcessData(const oemData_data* pOemData);

/*===========================================================================

FUNCTION oemData_IsDataTypeOn

DESCRIPTION
  This function returns TRUE(Enabled) or FALSE(Disabled) for an oem data type 
  
DEPENDENCIES
  None.

PARAMETERS
  None. 

RETURN VALUE
  boolean - TRUE if this data type is on/enabled, otherwise FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean oemData_IsDataTypeOn(oemData_dataType oemDataType);

/*===========================================================================

FUNCTION oemData_SendDataType

DESCRIPTION
  Send data to OEM layer (will call oemData_ProcessData internally)
  
DEPENDENCIES
  None.

PARAMETERS
  None. 

RETURN VALUE
  boolean - TRUE if this data type has been sent to OEM successfully, otherwise FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean oemData_SendDataType(oemData_dataType oemDataType, const void *oem_data);

#endif /* OEMDATA_H */

