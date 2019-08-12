#ifndef TM_PAII_H
#define TM_PAII_H

/*===========================================================================

        Position Determination Internal Data Structures
                   
DESCRIPTION

  This file contains data structures and constants which represent the 
  messages transmitted between the mobile and the PDE.  This format is 
  only used internally.
  
  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/is801/inc/tm_paii.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/30/07   cl      Mainlined FEATURE_GPSONE_STANDALONE
05/29/07   rw      Remove JCDMA definitions, moved to tm_jgps_msg.h
10/11/06   br      Added proper featurization and removed inlcude pai.h file.
10/14/05   rw      Include millisecond field in struct paii_lr_data_type
05/24/05   cl      Make CLM a NV item
06/23/04   ank     Mainlined FEATURE_GPSONE_VARIABLE_DBM_PDDM.
08/14/03   anr     Added feature to send Proprietary IS801 Base Lat/Long Msg.
07/24/02   jw      Added Weeknum to paii_alm_part_var_type.
04/12/02   hxw     Added FEATURE_GPSONE_VIRIABLE_DBM_PDDM to support reverse link 
                   viriable IS-801 PDDM size.
03/29/02   ak      Mainlined FEATURE_IS801_ADD_PILOTS
11/29/01   aar     Added structures for sending MSTO response.
06/27/01   dd      Added structures for requesting/receiving GPS Almanac/Ephemeris.
06/08/01   mai     Added code to support returning pilots in additional pilots field
                   of PPM response.
02/26/01   mai     Added FEATURE_GPS_MODE_IS801 wrapper around GPS parts.
01/31/01   mai     Updated to support IS801.1 changes
01/09/00    sk     Added sa_valid field to SA data for SRCH.
10/17/00   mai     Changed structures to include general header type and PDDM header
                   info for managing Network initiated sessions.
09/15/00    SK     Changed Defines to save Memory.
09/01/00   mai     Changed AA structure to reflect DOPPLER_WIN and 
                   SV_CODE_PH_WIN being decoded in this module, also changed the 
                   sensitivity assistance structures to make interface with Database
                   and search simpler.
06/01/00   mai     Took away cack_req from tx_req element as it is not a 
                   request which goes from MS to PDE.
05/19/00   mai     Added Header info part to this comment field.
05/12/00   mai     Fixed definition of SIXTEENTH_CHIP_MEAS_CAP to be 0x04. 
05/11/00   mai     Cleaned up code.
05/03/00   mai     Created module.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "target.h"
#include "comdef.h"               /* Basic definitions */
#include "customer.h"
                   
#include "qw.h"                   /* Quadword definitions */
#include "tm_pai.h"                  /* PAI definitions */
#include "caii.h"
#ifdef FEATURE_CMI
#include "caii_v.h"
#endif

/*===========================================================================

                            Data Declarations

===========================================================================*/

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------*/
#define MAX_NUM_PILOT_MEASUREMENTS  32    /* max ppm meas per part (IS801)
                                          ** Decreased from 64 for RAM Savings */
#define MAX_NUM_PPM_PARTS           1     /* max parts for PPM msg (IS801) 
                                          ** Decreased from 8 */
#define MAX_NUM_PR_PARTS            1      /* max parts for PR msg (IS801) 
                                             ** Decreased from 8 for RAM */
#define MAX_NUM_SAT_MEASUREMENTS    32     /* max PR meas per part (IS801) 
                                             ** Decreased from 64 for RAM */

#define MAX_NUM_ALM_SATS            32
#define MAX_NUM_EPH_SATS            16

#define MAX_NUM_SATS                32       /* max sats in AA msg (IS801) is at 16 */
                                             /* max sats in Demod AA and Meas AA is at 32 */

#define MAX_DATA_RECORDS            16  /* max data recs in SA msg (IS801) */
#define MAX_NUM_SATS_IN_DATA_RECORD 16    /* max sats per data rec (IS801) */
#define MAX_NAV_BITS_IN_BYTES       64             /* max data record size */
  

/* IS801 Field Defines */
#define SIXTEENTH_CHIP_MEAS_CAP     0x04

#define GPS_NONE                    0x00
#define GPS_ACQ_ASSIST              0x01
#define GPS_SEN_ASSIST              0x02
#define GPS_ALMANAC                 0x04
#define GPS_EPHEMERIS               0x08
#define GPS_KIPLING_SRCH            0x080
#define GPS_USER_PLANE_SESSION      0x800

#define PAII_RESP_PART_INFINITE     -1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                      MSID Related Structures                            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Generic Position Determination MSID Header */
typedef struct
{
  byte  msid_type;                /* Mobile station identifier field type */
  byte  msid_len;               /* Mobile station identifier field length */
  caii_msid_type msid;                       /* Mobile station identifier */
} paii_msid_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                      IS-801 MESSAGES                                    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* POSITION DETERMINATON DATA MESSAGE HEADER  (see IS-801 Section 3.2.4.2) */

typedef struct
{
  byte sess_start;                           /* Start-of-session indicator */
  byte sess_end;                               /* End-of-session indicator */
  byte sess_source;                            /* Session-source indicator */
  byte sess_tag;                                            /* Session tag */
  byte pd_msg_type;            /* Position determination data message type */
  byte num_requests;        /* Number of request element parameter records */
  byte num_responses;      /* Number of response element parameter records */
} paii_pddm_hdr_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                      General Types                                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Packing Schemes Enum type */
typedef enum paii_pddm_packing_type_e 
{
  PAII_PDDM_PACKING_TYPE_NONE = -1,
  /* FOR INTERNAL USE OF IS801 ONLY! */
  
  PAII_PDDM_PACKING_TYPE_PRM_PPM,
  /* Packing part of PPM and part of PRM in
  ** the first PDDM.
  */
  PAII_PDDM_PACKING_TYPE_PRM_ONLY,
  /* We can only pack PRM parts in the first PDDM.
  */

  PAII_PDDM_PACKING_TYPE_MAX
  /* FOR INTERNAL USE OF IS801 ONLY! */
} paii_pddm_packing_type_e_type;

/* To get general response type */
                     
typedef struct
{
  q_link_type         link;
  byte                type;
  paii_pddm_hdr_type  pddm_hdr;
} paii_gen_hdr_type;

/* General request header */

typedef struct
{
  byte            req_type;                      /* Request element type */
  byte            req_par_len;      /* Request element parameters length */
} paii_gen_req_hdr;

/* General request header */

typedef struct
{
  byte    resp_type;                            /* Response element type */
  byte    unsol_resp;                            /* Unsolicited response */
  byte    resp_par_len;            /* Response element parameters length */
} paii_gen_resp_hdr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE MS INFORMATION    (see IS-801 Section 3.2.4.2 RESP_TYPE='0010') */
                                       
typedef struct
{
  paii_gen_hdr_type hdr;                               /* General header */
  byte    unsol_resp;                            /* Unsolicited response */
  byte    ms_ls_rev;                /* Location standard revision number */
  byte    ms_mode;                             /* Digital mode indicator */
  byte    pilot_ph_cap;                        /* Pilot phase capability */
  word    gps_acq_cap;                     /* GPS acquisition capability */
  word    loc_calc_cap;               /* Location calculation capability */
} paii_msi_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE PILOT PHASE MEASUREMENT                                         */
/*                           (see IS-801 Section 3.2.4.2 RESP_TYPE='0101') */

typedef struct
{
  uint32          time_ref_ms;             /* Time validity of this part */
  byte            offset_incl;                        /* Offset included */
  sint15          mob_sys_t_offset;             /* MS system time offset */
  uint16          ref_pn;           /* Time reference PN sequence offset */
  byte            ref_pilot_strength;                  /* Pilot strength */
  byte            band_class;                              /* Band class */
  uint16          cdma_freq;                      /* Requency assignment */
  uint16          base_id;                /* Base station identification */
  uint16          sid;                          /* System identification */
  uint16          nid;                         /* Network identification */
  byte            total_rx_pwr;                  /* Total received power */
  byte            part_num;                               /* Part number */
  byte            total_parts;                        /* Number of parts */
  byte            num_pilots_p;         /* Number of pilots in this part */
  byte            add_pilots_incl;         /* Additional pilots included */
  byte            add_band_class;               /* Additional band class */
  word            add_cdma_freq;      /* Additional frequency assignment */
  byte            add_total_rx_pwr;/* Tot received power on the add freq */
  byte            num_add_pilots_p;/* Num additional pilots in this part */
} paii_ppm_part_fix_type;

typedef struct
{
  uint32  pilot_pn_phase;                        /* Pilot measured phase */
  byte    pilot_strength;                              /* Pilot strength */
  byte    rms_err_phase;               /* RMS error in phase measurement */
} paii_ppm_part_var_type;

typedef struct
{
  byte                    unsol_resp;            /* Unsolicited response */
  paii_ppm_part_fix_type  fix;
  paii_ppm_part_var_type  var[MAX_NUM_PILOT_MEASUREMENTS]; /* Pilot recs */
} paii_ppm_part_type;

typedef struct
{
  paii_gen_hdr_type   hdr;                                 /* General header */
  paii_ppm_part_type  ppm_part[MAX_NUM_PPM_PARTS];  /* PPM message parts */
} paii_ppm_resp_type;

  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE PSEUDORANGE MEASUREMENT                                         */
/*                           (see IS-801 Section 3.2.4.2 RESP_TYPE='0100') */
typedef struct
{
  byte            part_num;                               /* Part number */
  byte            total_parts;                        /* Number of parts */
  byte            num_ps_ranges_p;/* Number of Pseudoranges in this part */
  uint32          time_ref;                /* Time validity of this part */
  byte            time_ref_src;                 /* Time reference source */
  byte            offset_incl;                        /* Offset included */
  uint16          ref_pn;                  /* Time reference PN sequence */
  sint15          mob_sys_t_offset;             /* MS system time offset */
} paii_pr_part_fix_type;
  
typedef struct
{
  byte    sv_prn_num;                            /* Satellite PRN number */
  byte    sv_cno;                                      /* Satellite C/No */
  sint15  ps_doppler;                                   /* Pseudodoppler */
  uint16  sv_code_ph_wh;           /* Satellite code phase - whole chips */
  uint16  sv_code_ph_fr;      /* Satellite code phase - fractional chips */
  byte    mul_path_ind;                           /* Multipath indicator */
  byte    ps_range_rms_er;                      /* Pseudorange RMS error */
} paii_pr_part_var_type;
  
typedef struct
{
  byte                    unsol_resp;            /* Unsolicited response */
  paii_pr_part_fix_type   fix;
  paii_pr_part_var_type   var[MAX_NUM_SAT_MEASUREMENTS];     /* Sat recs */
} paii_pr_part_type;
  
typedef struct
{                                       
  paii_gen_hdr_type     hdr;                                 /* General header */
  paii_pr_part_type   pr_part[MAX_NUM_PR_PARTS];     /* GPS PR msg parts */
} paii_pr_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE TIME OFFSET MEASUREMENT                                         */
/*                         (See IS-801 Section 3.2.4.2 RESP_TYPE = '0110') */

typedef struct
{
  paii_gen_hdr_type     hdr;              /* General header */
  byte                  unsol_resp;       /* Unsolicited response indicator */
  uint32                time_ref_ms;      /* Time validity of this message */
  uint16                ref_pn;           /* Time reference PN offset */
  uint16                mob_sys_t_offset; /* MS system time offset */
} paii_msto_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE CANCELLATION ACKNOWLEDGEMENT                                    */
/*                         (See IS-801 Section 3.2.4.2 RESP_TYPE = '0111') */

typedef struct
{
  paii_gen_hdr_type   hdr;                                 /* General header */
  byte                unsol_resp;                /* Unsolicited response */
  byte                cancel_type_ack;              /* Cancellation type */
  byte                no_out_req;      /* No outstanding request element */
} paii_cack_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REJECT                  (See IS-801 Section 3.2.4.2 RESP_TYPE = '0000') */

typedef struct
{
  paii_gen_hdr_type   hdr;                                 /* General header */
  byte                unsol_resp;                /* Unsolicited response */
  byte                rej_req_type;               /* Reject request type */
  byte                rej_reason;                       /* Reject reason */
} paii_rej_resp_type;


/* ------------------------------------------------------------------------
** MS to PDE Request Typedefs
** ------------------------------------------------------------------------*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST GPS ACQUISITION ASSISTANCE                                      */
/*                            (see IS-801 Section 3.2.4.1 REQ_TYPE='0100') */
  
typedef struct 
{
  paii_gen_hdr_type hdr;                                 /* General header */
  byte    dopp_req;                /* Doppler (0th order) term requested */
  byte    add_dopp_req;            /* Additional Doppler terms requested */
  byte    code_ph_par_req;            /* Code phase parameters requested */
  byte    az_el_req;            /* Azimuth and elevation angle requested */
} paii_aa_req_type;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST GPS SENSITIVITY ASSISTANCE     (no parameters to set)           */
/*                            (see IS-801 Section 3.2.4.1 REQ_TYPE='0101') */  
typedef struct
{
  paii_gen_hdr_type hdr;                                 /* General header */
} paii_sa_req_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST LOCATION RESPONSE  (see IS-801 Section 3.2.4.1 REQ_TYPE='0001') */

typedef struct
{
  paii_gen_hdr_type hdr;                               /* General header */
  byte    height_req;                    /* Height information requested */
  byte    clk_cor_gps_req;    /* Clock correction for GPS time requested */
  byte    velocity_req;                /* Velocity information requested */
} paii_lr_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST GPS ALMANAC                    (no parameters to set)           */
/*                            (see IS-801 Section 3.2.4.1 REQ_TYPE='1000') */
typedef struct
{
  paii_gen_hdr_type hdr;                               /* General header */
} paii_alm_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST GPS EPHEMERIS      (see IS-801 Section 3.2.4.1 REQ_TYPE='1001') */  
typedef struct
{
  paii_gen_hdr_type hdr;                               /* General header */
  byte ab_par_req;                    /* Alpha/Beta Parameters Requested */
} paii_eph_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST ALMANAC CORRECTION (see IS-801 Section 3.2.4.1 REQ_TYPE='1011') */
typedef struct
{
  paii_gen_hdr_type hdr;                               /* General header  */
  byte toa;                                            /* Time Of Almanac */
  byte week_num;                                       /* GPS Week Number */
} paii_cor_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST SATELLITE HEALTH INFORMATION   (no parameters to set)           */
/*                            (see IS-801 Section 3.2.4.1 REQ_TYPE='1100') */  
typedef struct
{
  paii_gen_hdr_type hdr;                                /* General header */
} paii_svh_req_type;


/* ------------------------------------------------------------------------
** PDE to MS Response Typedefs
** ------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REJECT                                                                  */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='0000') */

/* Same as structure for the MS to PDE Reject response */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE GPS ACQUISITION ASSISTANCE                                      */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='0100') */  
typedef struct
{
  byte    num_sv;                            /* Number of GPS satellites */
  byte    time_of_app;       /* CDMA system time ref for the GPS AA info */
  byte    dopp_incl;                /* Doppler (0th order) term included */
  byte    add_dopp_incl;             /* Additional doppler info included */
  byte    code_ph_par_incl;                  /* Code phase info included */
  uint16  reference_pn;                                  /* Reference PN */
  byte    az_el_incl;            /* Azimuth and elevation angle included */
} paii_aa_fix_type;
  
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
} paii_aa_var_type;
  
typedef struct
{
  qword               timestamp; /* CDMA time that the data was received */
  paii_aa_fix_type    fix;    
  paii_aa_var_type    var[MAX_NUM_SATS];         /* Satellite AA records */
} paii_aa_data_type;
  
typedef struct
{
  paii_gen_hdr_type  hdr;                                /* General header */
  byte                resp_type;                /* Response element type */
  byte                unsol_resp;                /* Unsolicited response */
  paii_aa_data_type   aa_data;                          /*Actual AA data */
} paii_aa_resp_type;
  
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE GPS SENSITIVITY ASSISTANCE                                      */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='0101') */  
typedef struct
{
  uint16  ref_bit_num;                           /* Reference bit number */
  byte    dr_size;                                   /* Data record size */
  byte    nav_msg_bits[MAX_NAV_BITS_IN_BYTES];/* Navigation message bits */
  byte    num_sv_dr;     /* Number of GPS satellites in this data record */
  byte    sv_prn_num[MAX_NUM_SATS_IN_DATA_RECORD]; /* Satellite PRN nums */
} paii_sa_dr_type;
  
typedef struct
{
  qword           timestamp; /* CDMA time that the data was received */    
  byte            num_data_records;            /* Number of data records */
  paii_sa_dr_type data[MAX_DATA_RECORDS];             /* SA data records */
} paii_sa_data_type;
  
typedef struct
{
  paii_gen_hdr_type  hdr;                                /* General header */
  byte                resp_type;                /* Response element type */
  byte                unsol_resp;                /* Unsolicited response */
  paii_sa_data_type   sa_data;                         /* Actual SA data */
} paii_sa_resp_type;
  
typedef struct
{
  boolean           sa_valid;                    /* To indicate if SA is valid */
  boolean           sa_accompanied_with_aa;  
  paii_sa_data_type sa_data;
} paii_sa_srch_data_type;
  
typedef struct
{
  uint16                  ref_bit_num;           /* Reference bit number */
  byte                    num_dr_p;    /* Number of data records in part */
  byte                    dr_size;                   /* Data record size */
  byte                    part_num;               /* Number of this part */
  byte                    total_parts;          /* Total number of parts */
} paii_sa_part_fix_type;
   
typedef struct
{
  byte    nav_msg_bits[MAX_NAV_BITS_IN_BYTES];       /* Nav message bits */
  byte    num_sv_dr;     /* Number of GPS satellites in this data record */
  byte    sv_prn_num[MAX_NUM_SATS_IN_DATA_RECORD]; /* Satellite PRN nums */
} paii_sa_part_var_type;
  
typedef struct
{
  byte                    resp_type;            /* Response element type */
  byte                    unsol_resp;            /* Unsolicited response */
  qword                   timestamp;  /* CDMA time the data was received */    
  paii_sa_part_fix_type   fix;
  paii_sa_part_var_type   var[MAX_DATA_RECORDS];
} paii_sa_part_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE LOCATION RESPONSE                                               */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='0001') */

typedef struct
{
  qword   timestamp;             /* CDMA time that the data was received */
  uint16  timestamp_msec;                     /* Time in msec resolution */
  byte    msec_incl;                 /* Time in msec resolution included */
  uint16  time_ref_cdma;   /* CDMA system time at time solution is valid */
  sint31  latitude;                    /* Latitude, referenced to WGS-84 */
  sint31  longitude;                  /* Longitude, referenced to WGS-84 */
  byte    loc_uncrtnty_ang;              /* Angle of axis wrt true north 
                                                for position uncertainty */
  byte    loc_uncrtnty_a;      /* Standard deviation of axis along angle 
                                      specified for position uncertainty */
  byte    loc_uncrtnty_p; /* Standard deviation of axis perpendicular to 
                                angle specified for position uncertainty */
  byte    fix_type;                                          /* Fix type */
  byte    velocity_incl;                /* Velocity information included */
  uint16  velocity_hor;                 /* Horizontal velocity magnitude */
  uint16  heading;                                            /* Heading */
  int1    velocity_ver;                   /* Vertical velocity magnitude */
  byte    clock_incl;                      /* Clock information included */
  sint31  clock_bias;                                      /* Clock bias */
  sint15  clock_drift;                                    /* Clock drift */
  byte    height_incl;                    /* Height information included */
  sint15  height;                        /* Height, referenced to WGS-84 */
  byte    loc_uncrtnty_v;              /* Standard deviation of vertical 
                                          error for position uncertainty */
} paii_lr_data_type;

typedef struct
{
  paii_gen_hdr_type   hdr;                                 /* General header */
  byte                resp_type;                           /* Response element type */
  byte                unsol_resp;                          /* Unsolicited response */
  paii_lr_data_type   lr_data;
} paii_lr_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE GPS ALMANAC RESPONSE                                            */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='1000') */
/* Ordering of structure elements is different than IS-801 message format  */
/* to accomodate port of MMGPS PE                                          */
typedef struct
{
  uint8   u_SatIndex;
  uint8   u_TotalSats;      /* num_sv_p - Number of data records in part */
  uint8   u_Part_Num;                             /* Number of this part */
  uint8   u_Total_Parts;                        /* Total number of parts */
  uint8   u_WeekNum;                                  /* GPS week number */
  uint8   u_Toa;                                      /* Time Of Almanac */ 
  uint8   u_Week;                                     /* GPS week number */
} paii_alm_part_fix_type;

typedef paii_alm_part_fix_type AlmHdr;

typedef struct
{
  uint8   u_Sv_Prn_Num;                          /* Satellite PRN number */
  uint8   u_Health;
  uint16  w_DeltaI;                         /* Correction to inclination */
  uint32  q_MZero;                     /* Mean anomaly at reference time */
  uint16  w_E;                                           /* Eccentricity */
  uint32  q_SqrtA;                 /* Square root of the semi-major axis */
  uint32  q_OmegaZero;     /* Longitude of ascending node of orbit plane */
  uint32  q_Omega;                                /* Argument of perigee */
  uint16  w_OmegaDot;                         /* Rate of right ascension */
  uint16  w_Af1;              /* Apparant satellite clock correction af1 */
  uint16  w_Af0;              /* Apparant satellite clock correction af0 */
  uint8   u_Toa;                                      /* Time Of Almanac */ 
  uint8   u_WeekNum;                                  /* GPS week number */
} paii_alm_part_var_type;       

typedef struct
{
  paii_gen_hdr_type       hdr;                         /* General header */
  byte                    resp_type;            /* Response element type */
  byte                    unsol_resp;            /* Unsolicited response */
  qword                   timestamp;  /* CDMA time the data was received */    
  paii_alm_part_fix_type  fix;
  paii_alm_part_var_type  var[MAX_NUM_ALM_SATS];
} paii_alm_resp_type;

typedef struct
{
  byte                    resp_type;            /* Response element type */
  byte                    unsol_resp;            /* Unsolicited response */
  qword                   timestamp;  /* CDMA time the data was received */    
  paii_alm_part_fix_type  fix;
  paii_alm_part_var_type  var[MAX_NUM_ALM_SATS];
} paii_alm_part_type;

  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE GPS EPHEMERIS RESPONSE                                          */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='1001') */

typedef struct
{
  uint8 u_SatIndex;
  uint8 u_TotalSats;                   /* Number of data records in part */
  uint8 u_Part_Num;                               /* Number of this part */
  uint8 u_Total_Parts;                          /* Total number of parts */
  uint8 u_AB_Par_Incl;             /* Alpha and beta parameters included */
  uint8 u_Alpha_0;           /* Ionospheric correction parameter alpha 0 */
  uint8 u_Alpha_1;           /* Ionospheric correction parameter alpha 1 */
  uint8 u_Alpha_2;           /* Ionospheric correction parameter alpha 2 */
  uint8 u_Alpha_3;           /* Ionospheric correction parameter alpha 3 */
  uint8 u_Beta_0;            /* Ionospheric correction parameter beta  0 */
  uint8 u_Beta_1;            /* Ionospheric correction parameter beta  1 */
  uint8 u_Beta_2;            /* Ionospheric correction parameter beta  2 */
  uint8 u_Beta_3;            /* Ionospheric correction parameter beta  3 */
  double  d_UpdateTime; 
} paii_eph_part_fix_type;

typedef struct
{
  uint8 u_Sv_Prn_Num;                            /* Satellite PRN number */
  uint8 u_Iode;                                         /* Issue of data */
  int16 x_C_Rs;        /* Amplitude of the sine harmonic correction term */
  int16 x_Delta_N;                             /* Mean Motion Difference */
  int32 l_M0;                     /* Mean anom,aly at the reference time */
  int16 x_C_Uc;      /* Amplitude of the cosine harmonic correction term */
  int32 l_Eccentricity;                                  /* Eccentricity */
  int16 x_C_Us;        /* Amplitude of the sine harmonic correction term */
  int32 l_A_Sqrt;                  /* Square root of the semi-major axis */
  int16 x_Toe;                               /* Ephemeris reference time */
  int16 x_C_Ic;      /* Amplitude of the cosine harmonic correction term */
  int32 l_Omega_0;         /* Longitude of ascending node of orbit plane */
  int16 x_C_Is;        /* Amplitude of the sine harmonic correction term */
  int32 l_Angle_Inclination;  /* Inclination angle at the reference time */
  int16 x_C_Rc;      /* Amplitude of the cosine harmonic correction term */
  int32 l_Omega;                                  /* Argument of perigee */
  int32 l_Omegadot;                           /* Rate of right ascension */
  int16 x_Idot;                             /* Rate of inclination angle */
  int16 x_Toc;                              /* Clock data reference time */
  uint8 b_Af2;                /* Apparant satellite clock correction af2 */
  int16 b_Af1;                /* Apparant satellite clock correction af1 */
  int32 l_Af0;                /* Apparant satellite clock correction af0 */
} paii_eph_part_var_type;       

typedef struct
{
  paii_gen_hdr_type       hdr;                         /* General header */
  byte                    resp_type;            /* Response element type */
  byte                    unsol_resp;            /* Unsolicited response */
  qword                   timestamp;  /* CDMA time the data was received */    
  paii_eph_part_fix_type  fix;
  paii_eph_part_var_type  var[MAX_NUM_EPH_SATS];
} paii_eph_resp_type;

typedef struct
{
  byte                    resp_type;            /* Response element type */
  byte                    unsol_resp;            /* Unsolicited response */
  qword                   timestamp;  /* CDMA time the data was received */    
  paii_eph_part_fix_type  fix;
  paii_eph_part_var_type  var[MAX_NUM_EPH_SATS];
} paii_eph_part_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE GPS AlMANAC CORRECTIONS RESPONSE                                */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='1011') */
typedef struct
{
  byte num_sv_p;                       /* Number of data records in part */
  byte ref_time;                 /* Time of validity of these parameters */
  byte toa;                                           /* Time Of Almanac */
  byte week_num;                                      /* GPS week number */
  byte part_num;                                  /* Number of this part */
  byte total_parts;                             /* Total number of parts */
} paii_cor_part_fix_type;

typedef struct
{
  byte   sv_prn_num;                             /* Satellite PRN number */
  byte   delta_xyz_incl;                  /* Correction for XYZ included */
  sint15 delta_x;                         /* Correction for X coordinate */
  sint15 delta_y;                         /* Correction for Y coordinate */
  sint15 delta_z;                         /* Correction for Z coordinate */
  sint15 delta_x_1st;           /* 1st order correction for X coordinate */
  sint15 delta_y_1st;           /* 1st order correction for Y coordinate */
  sint15 delta_z_1st;           /* 1st order correction for Z coordinate */
  byte   delta_clock_incl;              /* Correction for clock included */
  sint31 delta_clock;                            /* Correction for clock */
  byte delta_clock_1st;                /* 1st order correction for clock */
} paii_cor_part_var_type;       

typedef struct
{
  byte                    resp_type;            /* Response element type */
  byte                    unsol_resp;            /* Unsolicited response */
  qword                   timestamp;  /* CDMA time the data was received */    
  paii_cor_part_fix_type  fix;
  paii_cor_part_var_type  var[MAX_DATA_RECORDS];
} paii_cor_part_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* PROVIDE SATELLITE HEALTH RESPONSE                                       */
/*                           (see IS-801 Section 4.2.4.2 RESP_TYPE='1100') */
typedef struct
{
  byte bad_sv_present;                     /* Bad GPS satellites present */
  byte num_bad_sv;                   /* The number of bad GPS satellites */
} paii_svh_part_fix_type;

typedef struct
{
  byte bad_sv_prn_num;                           /* Satellite PRN number */
} paii_svh_part_var_type;       

typedef struct
{
  byte                    resp_type;            /* Response element type */
  byte                    unsol_resp;            /* Unsolicited response */
  qword                   timestamp;  /* CDMA time the data was received */    
  paii_svh_part_fix_type  fix;
  paii_svh_part_var_type  var[MAX_DATA_RECORDS];
} paii_svh_part_type;


/* ------------------------------------------------------------------------
** PDE to MS Request Typedefs
** ------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST MS INFORMATION                                                  */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0010')  */
typedef paii_gen_hdr_type paii_msi_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST AUTONOMOUS MEASUREMENT WEIGHTING FACTORS                        */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0011')  */
typedef paii_gen_hdr_type paii_amwf_req_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST PSEUDORANGE MEASUREMENT                                         */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0100')  */  
typedef struct
{
  paii_gen_hdr_type hdr;                                 /* General header */
  byte    pref_resp_qual;                  /* Preferred response quality */
  byte    num_fixes;                                  /* Number of fixes */
  byte    t_betw_fixes;                            /* Time between fixes */
  byte    offset_req;                                /* Offset requested */
} paii_pr_req_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST PILOT PHASE MEASUREMENT                                         */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0101')  */
typedef struct
{
  paii_gen_hdr_type hdr;                               /* General header */
  byte    pref_resp_qual;                  /* Preferred response quality */
  byte    num_fixes;                                  /* Number of fixes */
  byte    t_betw_fixes;                            /* Time between fixes */
  byte    offset_req;                                /* Offset requested */
  byte    desi_pil_ph_res;             /* Desired pilot phase resolution */
} paii_ppm_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST LOCATION RESPONSE                                               */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0001')  */
typedef struct
{
  paii_gen_hdr_type hdr;                               /* General header */
  byte pref_resp_qual;                     /* Preferred response quality */
  byte num_fixes;                                     /* Number of fixes */
  byte t_betw_fixes;                               /* Time between fixes */
  byte height_req;                       /* Height information requested */
  byte clk_cor_gps_req;       /* Clock correction for GPS time requested */
  byte velocity_req;                   /* Velocity information requested */
} paii_lr_pde_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST TIME OFFSET MEASUREMENT                                         */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0110')  */
typedef struct
{
  paii_gen_hdr_type hdr;                               /* General header */
  byte use_time;                            /* Use action time indicator */
  byte action_time;                                       /* Action time */
} paii_tom_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST CANCELLATION                                                    */
/*                           (see IS-801 Section 4.2.4.1 REQ_TYPE='0111')  */
typedef struct
{
  paii_gen_hdr_type hdr;                               /* General header */
  byte    cancel_type;                              /* Cancellation type */
} paii_cancel_req_type;


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*         PROPRIETARY DATA MESSAGE TYPES (pd_msg_type==0xc0)              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*************************************************************************
** NOTE: The request and response header format for Proprietary data msgs
** are the same as the regular Position Determination Data Messages.
**************************************************************************/

/* BS System Parameter Info MSG (RESP_TYPE='1110')                                      */
                                       
typedef struct
{
  paii_gen_hdr_type hdr;           /* General header */
  byte unsol_resp;                 /* Unsolicited response */
  uint16 time_ref_cdma;            /* CDMA time when sys param is received */
  byte   still_active;             /* Base Station in Active Set indicator */
  byte   band_class;               /* Base station class */
  uint16 cdma_freq;                /* CDMA frequency */
  uint16 pilot_pn;                 /* Pilot PN sequence offset index */
  uint16 sid;                      /* System identification */
  uint16 nid;                      /* Network identification */
  uint16 base_id;                  /* Base station identification */
  uint32 base_lat;                 /* Base station latitude */
  uint32 base_long;                /* Base station longitude */
}paii_prdm_bsp_resp_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* MS REV DATA MSG (RESP_TYPE='0001')                                      */
                                       
typedef struct
{
  paii_gen_hdr_type hdr;           /* General header */
  byte unsol_resp;                 /* Unsolicited response */
  uint16 chip_id;                  /* Chip ID */           
  uint32 sw_rev_id;                /* Software Revision ID */  
  uint32 dev_manufacturer;         /* Device manufacturer */
  uint32 dev_rev_id;               /* Device Revision ID */
} paii_prdm_ms_rev_resp_type;


/* ------------------------------------------------------------------------
** IS801 REQUEST and RESPONSE unions
** ------------------------------------------------------------------------*/

typedef union
{
  paii_gen_hdr_type     gen_resp;
  paii_msi_resp_type    msi_resp;
  paii_pr_resp_type     pr_resp;
  paii_ppm_resp_type    ppm_resp;
  paii_msto_resp_type   msto_resp;
  paii_cack_resp_type   cack_resp;
  paii_rej_resp_type    rej_resp;
  paii_lr_resp_type     lr_resp;
} paii_tx_resp_type;

typedef union
{
  paii_pr_part_type     pr_part;
  paii_ppm_part_type    ppm_part;
} paii_tx_part_type;

typedef union
{
  paii_gen_hdr_type     gen_req;
  paii_aa_req_type      aa_req;
  paii_sa_req_type      sa_req;
  paii_lr_req_type      lr_req;
  paii_alm_req_type     alm_req;
  paii_eph_req_type     eph_req;
  paii_cor_req_type     cor_req;
  paii_svh_req_type     svh_req;
} paii_tx_req_type;

typedef union
{
  paii_gen_hdr_type     gen_req;
  paii_msi_req_type     msi_req;
  paii_amwf_req_type    amwf_req;
  paii_pr_req_type      pr_req;
  paii_ppm_req_type     ppm_req;
  paii_lr_pde_req_type  lr_req;
  paii_tom_req_type     tom_req;
  paii_cancel_req_type  cancel_req;
} paii_rx_req_type;

typedef union
{
  paii_gen_hdr_type     gen_resp;
  paii_rej_resp_type    rej_resp;
  paii_aa_resp_type     aa_resp;
  paii_sa_part_type     sa_part;
  paii_alm_part_type    alm_part;
  paii_eph_part_type    eph_part;
  paii_lr_resp_type     lr_resp;
} paii_rx_resp_type;

typedef union
{
  paii_aa_resp_type     aa_resp;
  paii_sa_resp_type     sa_resp;
  paii_lr_resp_type     lr_resp;
  paii_alm_resp_type    alm_resp;
  paii_eph_resp_type    eph_resp;
} paii_rx_full_resp_type;


typedef union
{
  paii_gen_hdr_type          gen_resp;
  paii_prdm_ms_rev_resp_type msr_resp;
  paii_prdm_bsp_resp_type    bsp_resp;
} paii_prdm_tx_resp_type;


#endif /* TM_PAII_H */
