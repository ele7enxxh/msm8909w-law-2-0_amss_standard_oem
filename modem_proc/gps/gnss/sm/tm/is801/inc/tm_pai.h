#ifndef TM_PAI_H
#define TM_PAI_H

/*===========================================================================

  IS-801  P R O T O C O L   D A T A   D E C L A R A T I O N S   
                   
DESCRIPTION
  
  This file contains the data structures for messages received and
  transmitted to the PDE, and other IS-801-related constants and indicators as
  defined in IS-801.
     
  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/is801/inc/tm_pai.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/07   rw      Remove definitions under FEATURE_JCDMA_GPSONE, moved to tm_jgps_msg.h
10/11/06   br      Removed feature GPSONE and added proper featurization.
05/24/05   cl      Make CLM a NV item
08/14/03   anr     Added feature to send Proprietary IS801 Base Lat/Long Msg.
06/04/03   ar      Added translation for SCU messages
04/11/02   hxw     Mod's to support LR on R-Link.
11/29/01   aar     Added MSTO measurement response related structures/constants
                   and change the size of the MSREV's chip Id from 8 to 16.
06/27/01   dd      Added structures for requesting/receiving GPS Almanac/Ephemeris.
06/07/01   mai	   Added define for the maximum IS801 Doppler value.
03/10/01   mai     Added more code to support addition of a couple more checks.
03/05/01   mai     Added local code to support FEATURE_IS801_BIT_CHECK, which checks 
                   incoming IS801 data for bit-wise validity.
02/26/01   mai     Added FEATURE_GPS_MODE_IS801 wrapper around GPS parts.
01/31/01   mai     Updated to support IS801.1 changes
10/17/00   mai     Added MSI request type #define.
07/07/00   mai     Added length fields for various portions of IS801 response
                   elements, used for packing capsules.
06/01/00   mai     Removed more unnecessary #defines.
05/19/00   mai     Removed unnecessary #defines.
05/11/00   mai     Cleaned up code.
05/03/00   mai     Created module.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "qw.h"

/*===========================================================================

                            Data Declarations

===========================================================================*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  PDDM DEFINITIONS                                       */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#define PDDM_HDR_LEN    3                       /* Size of header in bytes */
#define REQ_HDR_LEN     2               /* Size of request header in bytes */    
#define RESP_HDR_LEN    2              /* Size of response header in bytes */


#define REQ_MASK        0x0f               /* Number of requests mask type */
#define RESP_MASK       0x0f              /* Number of responses mask type */


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  IS-801 MS RESPONSE TYPES                               */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#define REJ_RESP_CODE               0x00 
#define MS_INFO_RESP_CODE           0x02
#define PR_MEAS_RESP_CODE           0x04
#define PPM_MEAS_RESP_CODE          0x05
#define MSTO_MEAS_RESP_CODE         0x06
#define CACK_RESP_CODE              0x07

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  IS-801 MS RESPONSE LENGTHS  (in bits)                  */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define REJ_RESP_LEN        8  /* includes reserved field */
#define MSI_RESP_LEN        40
#define CACK_RESP_LEN       8  /* includes reserved field */
#define PR_RESP_VAR_LEN     57
#define PR_RESP_FIX1_LEN    39
#define PR_RESP_FIX2_LEN    23
#define PPM_RESP_VAR_LEN    31
#define PPM_RESP_FIX1_LEN   121 /* includes ADD_PILOTS_INCL */
#define PPM_RESP_FIX2_LEN   14
#define PPM_RESP_FIX3_LEN   27
#define MSTO_RESP_LEN       48
#define LOC_RESP_FIX1_LEN  83
#define LOC_RESP_FIX2_LEN  27
#define LOC_RESP_FIX3_LEN  34
#define LOC_RESP_FIX4_LEN  19

#define PPM_REQ_LEN     3   /* Length in bytes of PPM request */
#define LR_PDE_REQ_LEN  3   /* Length in bytes of LR request from PDE */
#define TOM_REQ_LEN     1   /* Length in bytes of TOM request */
#define CANCEL_REQ_LEN  1   /* Length in bytes of CANCEL request */

#define PR_REQ_LEN      3   /* Length in bytes of PR request */


/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                  IS-801 PDE RESPONSE TYPES                              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
typedef enum 
{
  /* REJ_RESP_CODE = 0x00, covered above in MS response types */
  LOC_RESP_CODE = 0x01,
  BS_CAP_RESP_CODE = 0x02,
  BS_ALM_RESP_CODE = 0x03,
  GPS_AA_RESP_CODE = 0x04,    
  GPS_SA_RESP_CODE = 0x05,
  GPS_LA_S_RESP_CODE = 0x06,
  GPS_LA_C_RESP_CODE = 0x07,
  GPS_ALM_RESP_CODE = 0x08,
  GPS_EPH_RESP_CODE = 0x09,
  GPS_NAV_MSG_RESP_CODE = 0x0a,
  GPS_ALM_COR_RESP_CODE = 0x0b,
  GPS_SAT_HLTH_RESP_CODE = 0x0c,
  
  MAX_PDE_TO_MS_RESP_CODE = GPS_SAT_HLTH_RESP_CODE
} pde_to_ms_resp_type;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* MSID Header */
typedef PACKED struct PACKED_POST
{
  byte reserved[1];   /* Reserved */
  byte msid_type[3];  /* Mobile station identifier field type */
  byte msid_len[4];   /* Mobile station identifier field length */
} pai_msid_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Position Determination Data Message                       */
/* (See IS-801 Section 3.2.4)                                               */

typedef PACKED struct PACKED_POST
{
  byte sess_start[1];   /* Start of session indicator */
  byte sess_end[1];     /* End of session indicator */
  byte sess_source[1];  /* Session source indicator */
  byte sess_tag[5];     /* Session tag */
  byte pd_msg_type[8];  /* PDDM type */
  byte num_requests[4]; /* Number of request element parameter records */
  byte num_responses[4];/* Number of response element parameter records */
} pai_pddm_hdr_type;

typedef PACKED struct PACKED_POST
{
  byte reserved[4];     /* Reserved */
  byte req_type[4];     /* Request element type */
  byte req_par_len[8];  /* Request element parameters length */
} pai_req_hdr_type;

typedef PACKED struct PACKED_POST
{
  byte reserved[3];     /* Reserved */
  byte unsol_resp[1];   /* Unsolicited response indicator */
  byte resp_type[4];    /* Response element type */
  byte resp_par_len[8]; /* Response element parameters length */
} pai_resp_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Request GPS Acquisition Assistance                        */
/* (See IS-801 Section 3.2.4.1 REQ_TYPE = '0100')                          */
  
typedef PACKED struct PACKED_POST
{
  byte dopp_req[1];         /* Doppler (0th order) term requested */
  byte add_dopp_req[1];     /* Additional doppler terms requested */
  byte code_ph_par_req[1];  /* Code phase parameters requested */
  byte az_el_req[1];        /* Azimuth and elevation angle requested */
  byte reserved[4];         /* Reserved */
} pai_aa_req_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Request Location Response                                 */
/* (See IS-801 Section 3.2.4.1 REQ_TYPE = '0001')                          */

typedef PACKED struct PACKED_POST
{
  byte height_req[1];       /* Height information requested */
  byte clk_cor_gps_req[1];  /* Clock correction for GPS time requested */
  byte velocity_req[1];     /* Velocity information requested */
  byte reserved[5];         /* Reserved */
} pai_lr_req_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Request Location Response                                 */
/* (See IS-801 Section 4.2.4.1 REQ_TYPE = '0001')                          */

typedef PACKED struct PACKED_POST
{
  byte pref_resp_qual[3];   /* Preferred response quality */
  byte num_fixes[8];        /* Number of fixes */ 
  byte t_betw_fixes[8];     /* Time between fixes */
  byte height_req[1];       /* Height information requested */
  byte clk_cor_gps_req[1];  /* Clock correction for GPS time requested */
  byte velocity_req[1];     /* Velocity information requested */
  byte reserved[2];         /* Reserved bits */
} pai_lr_pde_req_rpt_type;
#define PAI_LR_PDE_REQ_RPT_TYPE_SZ (                                       \
  FSIZ( pai_lr_pde_req_rpt_type, pref_resp_qual ) +                        \
  FSIZ( pai_lr_pde_req_rpt_type, num_fixes )      +                        \
  FSIZ( pai_lr_pde_req_rpt_type, t_betw_fixes )   +                        \
  FSIZ( pai_lr_pde_req_rpt_type, height_req )     +                        \
  FSIZ( pai_lr_pde_req_rpt_type, clk_cor_gps_req )+                        \
  FSIZ( pai_lr_pde_req_rpt_type, velocity_req )   +                        \
  FSIZ( pai_lr_pde_req_rpt_type, reserved ))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*          Request Autonomous Measurement Weighting Factors               */
/* (See IS-801 Section 4.2.4.1 REQ_TYPE = '0011')                          */
/* HEADER ONLY; NO PAYLOAD                                                 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Request Pseudorange Measurement                           */
/* (See IS-801 Section 4.2.4.1 REQ_TYPE = '0100')                          */
  
typedef PACKED struct PACKED_POST
{
  byte pref_resp_qual[3];   /* Preferred response quality */
  byte num_fixes[8];        /* Number of fixes */
  byte t_betw_fixes[8];     /* Time between fixes */
  byte offset_req[1];       /* Offset requested */
  byte reserved[4];         /* Reserved */
} pai_pr_req_rpt_type;
#define PAI_PR_REQ_RPT_TYPE_SZ (                                           \
  FSIZ( pai_pr_req_rpt_type, pref_resp_qual ) +                            \
  FSIZ( pai_pr_req_rpt_type, num_fixes )      +                            \
  FSIZ( pai_pr_req_rpt_type, t_betw_fixes )   +                            \
  FSIZ( pai_pr_req_rpt_type, offset_req )     +                            \
  FSIZ( pai_pr_req_rpt_type, reserved ))

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Request Pilot Phase Measurement                           */
/* (See IS-801 Section 4.2.4.1 REQ_TYPE = '0101')                          */

typedef PACKED struct PACKED_POST
{
  byte pref_resp_qual[3];   /* Preferred response quality */
  byte num_fixes[8];        /* Number of fixes */
  byte t_betw_fixes[8];     /* Time between fixes */
  byte offset_req[1];       /* Offset requested */
  byte desi_pil_ph_res[1];  /* Desired pilot phase resolution */
  byte reserved[3];         /* Reserved */
} pai_ppm_req_rpt_type;
#define PAI_PPM_REQ_RPT_TYPE_SZ (                                          \
  FSIZ( pai_ppm_req_rpt_type, pref_resp_qual ) +                           \
  FSIZ( pai_ppm_req_rpt_type, num_fixes )      +                           \
  FSIZ( pai_ppm_req_rpt_type, t_betw_fixes )   +                           \
  FSIZ( pai_ppm_req_rpt_type, offset_req )     +                           \
  FSIZ( pai_ppm_req_rpt_type, desi_pil_ph_res )+                           \
  FSIZ( pai_ppm_req_rpt_type, reserved ))

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Request Time Offset Measurement                           */
/* (See IS-801 Section 4.2.4.1 REQ_TYPE = '0110')                          */

typedef PACKED struct PACKED_POST
{
  byte use_time[1];         /* Use action time indicator */
  byte action_time[6];      /* Action time */
} pai_tom_req_rpt_type;
#define PAI_TOM_REQ_RPT_TYPE_SZ (                                          \
  FSIZ( pai_tom_req_rpt_type, use_time )       +                           \
  FSIZ( pai_tom_req_rpt_type, action_time))

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Request Cancellation                                      */
/* (See IS-801 Section 4.2.4.1 REQ_TYPE = '0111')                          */

typedef PACKED struct PACKED_POST
{
  byte cancel_type[4];  /* Cancellation type */
  byte reserved[4];     /* Reserved */
} pai_cancel_req_rpt_type;
#define PAI_CANCEL_REQ_RPT_TYPE_SZ (                                       \
  FSIZ( pai_cancel_req_rpt_type, cancel_type )       +                     \
  FSIZ( pai_cancel_req_rpt_type, reserved))

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Request GPS Ephemeris                                     */
/* (See IS-801 Section 3.2.4.1 REQ_TYPE = '1001')                          */

typedef PACKED struct PACKED_POST
{
  byte ab_par_req[1];       /* Alpha/Beta Parameters requested */
  byte reserved[7];         /* Reserved */
} pai_eph_req_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Provide MS Information                                    */
/* (See IS-801 Section 3.2.4.2 RESP_TYPE = '0010')                         */

typedef PACKED struct PACKED_POST
{
  byte reserved[3];     /* Reserved */
  byte unsol_resp[1];   /* Unsolicited response indicator */
  byte resp_type[4];    /* Response element type */
  byte resp_par_len[8]; /* Response element parameters length */
  byte ms_ls_rev[6];    /* MS location standard revision number */
  byte ms_mode[4];      /* Digital mode indicator */
  byte pilot_ph_cap[6]; /* Pilot phase capability */
  byte gps_acq_cap[12]; /* GPS acquisition capability */
  byte loc_calc_cap[12];/* Position calculation capability */
} pai_msi_res_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Provide Pseudorange Measurement                           */
/* (See IS-801 Section 3.2.4.2 RESP_TYPE = '0100')                         */
  
typedef PACKED struct PACKED_POST
{
  byte part_num[3];         /* Part number */
  byte total_parts[3];      /* Total number of parts */
  byte num_ps_ranges_p[6];  /* Number of pseudoranges in this part */
  byte time_ref[24];        /* Time validity of parameters reported in this part */
  byte time_ref_src[2];     /* Time reference source */
  byte offset_incl[1];      /* Offset included */
  byte ref_pn[9];           /* Time reference PN sequence offset */
  byte mob_sys_t_offset[14];/* MS system time offset */
} pai_pr_res_fix_type;
  
typedef PACKED struct PACKED_POST
{
  byte sv_prn_num[6];     /* Satellite PRN number */
  byte sv_cno[6];         /* Satellite C/No */
  byte ps_doppler[16];    /* Pseudodoppler */
  byte sv_code_ph_wh[10];/* Satellite code phase - whole chips */
  byte sv_code_ph_fr[10]; /* Satellite code phase - fractional chips */
  byte mul_path_ind[3];   /* Multipath indicator */
  byte ps_range_rms_er[6];/* Pseudorange RMS error */
} pai_pr_res_var_type;
  
#define PAI_PR_MAX 64
/* Max number of pseudorange records is constrained by the size of the 
   num_ps_ranges_p field 
*/
  
typedef PACKED struct PACKED_POST
{
  pai_pr_res_fix_type fix;
  pai_pr_res_var_type var[PAI_PR_MAX];
} pai_pr_res_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Provide Pilot Phase Measurement                           */
/* (See IS-801 Section 3.2.4.2 RESP_TYPE = '0101')                         */

typedef PACKED struct PACKED_POST
{
  byte time_ref_ms[24];     /* Time validity of parameters reported in this part */
  byte offset_incl[1];      /* Offset included */
  byte mob_sys_t_offset[14];/* MS system time offset */
  byte ref_pn[9];           /* Time reference PN sequence offset */
  byte ref_pilot_strength[6]; /* Pilot strength */
  byte band_class[5];       /* Band class */
  byte cdma_freq[11];       /* Freqeuncy assignment */
  byte base_id[16];         /* Base station Identification */
  byte sid[15];             /* System identification */
  byte nid[16];             /* Network identification */
  byte total_rx_pwr[5];       /* Total received power */
  byte part_num[3];         /* Part number */
  byte total_parts[3];      /* Total number of parts */
  byte num_pilots_p[6];     /* Number of pilots in this part */
  byte add_pilots_incl[1];  /* Additional pilots included */
  byte add_band_class[5];   /* Additional band class */
  byte add_cdma_freq[11];   /* Additional frequency assignment */
  byte add_total_rx_pwr[5];   /* Total power received on the additional frequency */
  byte num_add_pilots_p[6]; /* NUmber of additional pilots in this part */
} pai_ppm_res_fix_type;

typedef PACKED struct PACKED_POST
{
  byte pilot_pn_phase[19];  /* Pilot measured phase */
  byte pilot_strength[6];   /* Pilot strength */
  byte rms_err_phase[6];    /* RMS error in phase measurement */
} pai_ppm_res_var_type;

#define PAI_PPM_MAX 64
  /* Max number of pilot phase records is constrained by the size of the 
     num_pilots_p field */

typedef PACKED struct PACKED_POST
{
  pai_ppm_res_fix_type fix;
  pai_ppm_res_var_type var[PAI_PPM_MAX];
} pai_ppm_res_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Provide Time Offset Measurement                           */
/* (See IS-801 Section 3.2.4.2 RESP_TYPE = '0110')                         */

typedef PACKED struct PACKED_POST
{
  byte reserved[3];           /* Reserved */
  byte unsol_resp[1];         /* Unsolicited response indicator */
  byte resp_type[4];          /* Response element type */
  byte resp_par_len[8];       /* Response element parameters length */
  byte time_ref_ms[24];       /* Time validity of params reported in this msg */
  byte ref_pn[9];             /* Time reference PN sequence offset */
  byte mob_sys_t_offset[14];  /* Mobile station system time offset */
  byte reserved2[1];          /* Reserved */
} pai_msto_res_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Provide Cancellation Acknowledgement                      */
/* (See IS-801 Section 3.2.4.2 RESP_TYPE = '0111')                         */

typedef PACKED struct PACKED_POST
{
  byte reserved[3];     /* Reserved */
  byte unsol_resp[1];   /* Unsolicited response indicator */
  byte resp_type[4];    /* Response element type */
  byte resp_par_len[8]; /* Response element parameters length */
  byte cancel_type_ack[4];  /* Cancellation type */
  byte no_out_req[1];       /* No outstanding request element */
  byte reserved2[3];         /* Reserved */
} pai_cack_res_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Reject                                                    */
/* (See IS-801 Section 3.2.4.2 RESP_TYPE = '0000')                         */

#define MAX_REJ_REASON_VALUE  0x01

typedef PACKED struct PACKED_POST
{
  byte reserved[3];     /* Reserved */
  byte unsol_resp[1];   /* Unsolicited response indicator */
  byte resp_type[4];    /* Response element type */
  byte resp_par_len[8]; /* Response element parameters length */
  byte rej_req_type[4]; /* Reject request type */
  byte rej_reason[3];   /* Reject reason */
  byte reserved2[1];     /* Reserved */
} pai_rej_res_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Provide GPS Acquisition Assistance                        */
/* (See IS-801 Section 4.2.4.2 RESP_TYPE = '0100')                         */
  
#define PAI_AA_MAX 16
/* Max number of satellites that acquisition assistance will be provided for is 
   constrained by the size of the num_sv field */

#define GPS_AA_MAX_REF_PN 0x200
/* Max valid value for REF_PN field */

#define GPS_AA_MAX_DOPPLER_WIN  0x04
/* Max valud value for DOPPLER_WIN */

#define GPS_AA_MAX_SV_CODE_PH 1022
/* Max valid value for SV_CODE_PH */

#define GPS_AA_MAX_SV_CODE_PH_INT 19
/* Max valid value for SV_CODE_PH_INT */

#define GPS_MAX_SV_PRN_VALUE 37
/* Max valid value for SV_PRN_NUM */

#define GPS_AA_MAX_DOPP0_VAL  0x7ff
/* Max valid value for Doppler0 */

typedef PACKED struct PACKED_POST
{
  byte num_sv[4];           /* Number of GPS satellites */
  byte time_of_app[8];      /* CDMA system time ref for the GPS AA info */
  byte dopp_incl[1];        /* Doppler (0th order) term included */
  byte add_dopp_incl[1];    /* Additional doppler information included */
  byte code_ph_par_incl[1]; /* Code phase information included */
  byte reference_pn[10];    /* Reference PN */
  byte az_el_incl[1];       /* Azimuth and elevation angle included */
} pai_aa_res_fix_type;
#define PAI_AA_RES_FIX_TYPE_SZ (                                           \
  FSIZ( pai_aa_res_fix_type, num_sv )            +                         \
  FSIZ( pai_aa_res_fix_type, time_of_app )       +                         \
  FSIZ( pai_aa_res_fix_type, dopp_incl )         +                         \
  FSIZ( pai_aa_res_fix_type, add_dopp_incl )     +                         \
  FSIZ( pai_aa_res_fix_type, code_ph_par_incl )  +                         \
  FSIZ( pai_aa_res_fix_type, reference_pn )      +                         \
  FSIZ( pai_aa_res_fix_type, az_el_incl ))

  
typedef PACKED struct PACKED_POST
{
  byte sv_prn_num[6];       /* Satellite PRN number */
  byte doppler0[12];        /* 0th order doppler */
  byte doppler1[7];         /* 1st order doppler */
  byte doppler_win[3];      /* Doppler search window */
  byte sv_code_ph[10];      /* GPS code phase */
  byte sv_code_ph_int[5];   /* Integer  number of code periods elapsed since 
                               latest GPS bit boundary */
  byte gps_bit_num[2];      /* 2 least significant bits of the bit number 
                               (within the GPS frame) being currently 
                               transmitted */
  byte sv_code_ph_win[5];   /* Total code phase window */
  byte azimuth[5];          /* Azimuth of the GPS satellite */
  byte elevation[3];        /* Elevation angle of the GPS satellite */
} pai_aa_res_var_type;
#define pai_aa_res_var_type_sz (                                           \
  FSIZ( pai_aa_res_var_type, sv_prn_num )        +                         \
  FSIZ( pai_aa_res_var_type, doppler0 )          +                         \
  FSIZ( pai_aa_res_var_type, doppler1 )          +                         \
  FSIZ( pai_aa_res_var_type, doppler_win )       +                         \
  FSIZ( pai_aa_res_var_type, sv_code_ph )        +                         \
  FSIZ( pai_aa_res_var_type, sv_code_ph_int )    +                         \
  FSIZ( pai_aa_res_var_type, gps_bit_num )       +                         \
  FSIZ( pai_aa_res_var_type, sv_code_ph_win )    +                         \
  FSIZ( pai_aa_res_var_type, azimuth )           +                         \
  FSIZ( pai_aa_res_var_type, elevation ))
  
typedef PACKED struct PACKED_POST
{
  pai_aa_res_fix_type fix;
  pai_aa_res_var_type var[PAI_AA_MAX];
} pai_aa_res_rpt_type;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Provide GPS Sensitivity Assistance                        */
/* (See IS-801 Section 4.2.4.2 RESP_TYPE = '0101')                         */
  
#define GPS_SA_MAX_REF_BIT_VALUE  1499
/* Max valid value for ref_bit_num */

typedef PACKED struct PACKED_POST
{
  byte ref_bit_num[11]; /* Reference bit number */
  byte num_dr_p[4];     /* Number of data records in this part */
  byte dr_size[8];      /* Data record size */
  byte part_num[3];     /* Part number */
  byte total_parts[3];  /* Total number of parts */
} pai_sa_res_fix_type;
  
typedef PACKED struct PACKED_POST
{
  byte nav_msg_bits[2]; /* Navigation message bits */
} pai_sa_res_var1_type;
  
typedef PACKED struct PACKED_POST
{
  byte num_sv_dr[4];   /* Number of GPS satellites in this data record */
} pai_sa_res_var2_type;
  
typedef PACKED struct PACKED_POST
{
  byte sv_prn_num[6];   /* Satellite prn number */
} pai_sa_res_var3_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Provide Location Response                                 */
/* (See IS-801 Section 4.2.4.2 RESP_TYPE = '0001')                         */

typedef PACKED struct PACKED_POST 
{
  byte time_ref_cdma[14];   /* CDMA system time when the solution is valid */
  byte latitude[25];        /* Latitude */
  byte longitude[26];       /* Longitude */
  byte loc_uncrtnty_ang[4]; /* Angle of axis wrt True North for position 
                               uncertainty */
  byte loc_uncrtnty_a[5];   /* Standard deviation of axis along angle 
                               specified for position uncertainty */
  byte loc_uncrtnty_p[5];   /* Standard deviation of axis perpendicular to 
                               angle specified for position uncertainty */
  byte fix_type[1];         /* Fix type */
  byte velocity_incl[1];    /* Velocity information included */
  byte velocity_hor[9];     /* Horizontal velocity magnitude */
  byte heading[10];         /* Heading */
  byte velocity_ver[8];     /* Vertical velocity */
  byte clock_incl[1];       /* Clock information included */
  byte clock_bias[18];      /* Clock bias */
  byte clock_drift[16];     /* Clock drift */
  byte height_incl[1];      /* Height information included */
  byte height[14];          /* Height */
  byte loc_uncrtnty_v[5];   /* Standard deviation of vertical error for 
                               position uncertainty */
} pai_lr_res_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Provide GPS Almanac Response                              */
/* (See IS-801 Section 4.2.4.2 RESP_TYPE = '1000')                         */

typedef PACKED struct PACKED_POST
{
  byte num_sv_p[6];        /* Number of data records in part */
  byte week_num[8];        /* GPS week number */
  byte toa[8];             /* Time Of Almanac */
  byte part_num[5];        /* Number of this part */
  byte total_parts[5];      /* Total number of parts */
}pai_alm_res_fix_type;

typedef PACKED struct PACKED_POST
{
   byte sv_prn_num[5];          /* Satellite PRN number */
   byte delta_inclination[16];  /* Correction to inclination */
   byte m0[24];                 /* Mean anomaly at reference time */
   byte eccentricity[16];       /* Eccentricity */
   byte a_sqrt[24];             /* Square root of the semi-major axis */
   byte omega_0[24];            /* Longitude of ascending node of orbit plane */
   byte omega[24];              /* Argument of perigee */
   byte omegadot[16];           /* Rate of right ascension */
   byte af1[11];                /* Apparant satellite clock correction af1 */
   byte af0[11];                /* Apparant satellite clock correction af0 */
} pai_alm_res_var_type;
#define PAI_ALM_RES_VAR_TYPE_SZ (                                          \
  FSIZ( pai_alm_res_var_type, sv_prn_num )        +                        \
  FSIZ( pai_alm_res_var_type, delta_inclination ) +                        \
  FSIZ( pai_alm_res_var_type, m0 )                +                        \
  FSIZ( pai_alm_res_var_type, eccentricity )      +                        \
  FSIZ( pai_alm_res_var_type, a_sqrt )            +                        \
  FSIZ( pai_alm_res_var_type, omega_0 )           +                        \
  FSIZ( pai_alm_res_var_type, omega )             +                        \
  FSIZ( pai_alm_res_var_type, omegadot )          +                        \
  FSIZ( pai_alm_res_var_type, af1 )               +                        \
  FSIZ( pai_alm_res_var_type, af0 ))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*               Provide GPS Ephemerisc Response                           */
/* (See IS-801 Section 4.2.4.2 RESP_TYPE = '1001')                         */

typedef struct
{
  byte num_sv_p[4];             /* Number of data records in part */
  byte part_num[5];             /* Number of this part */
  byte total_parts[5];          /* Total number of parts */
  byte ab_par_incl;             /* Alpha and beta parameters included */
  byte alpha_0[8];              /* Ionospheric correction parameter alpha 0 */
  byte alpha_1[8];              /* Ionospheric correction parameter alpha 1 */
  byte alpha_2[8];              /* Ionospheric correction parameter alpha 2 */
  byte alpha_3[8];              /* Ionospheric correction parameter alpha 3 */
  byte beta_0[8];               /* Ionospheric correction parameter beta  0 */
  byte beta_1[8];               /* Ionospheric correction parameter beta  1 */
  byte beta_2[8];               /* Ionospheric correction parameter beta  2 */
  byte beta_3[8];               /* Ionospheric correction parameter beta  3 */
} pai_eph_res_fix_type;

typedef struct
{
  byte sv_prn_num[5];     /* Satellite PRN number */
  byte iode[8];           /* Issue of data */
  byte c_rs[16];          /* Amplitude of the sine harmonic correction term */
  byte delta_n[16];       /* Mean Motion Difference */
  byte m0[32];            /* Mean anom,aly at the reference time */
  byte c_uc[16];          /* Amplitude of the cosine harmonic correction term */
  byte eccentricity[32];  /* Eccentricity */
  byte c_us[16];          /* Amplitude of the sine harmonic correction term */
  byte a_sqrt[32];        /* Square root of the semi-major axis */
  byte toe[16];           /* Ephemeris reference time */
  byte c_ic[16];          /* Amplitude of the cosine harmonic correction term */
  byte omega_0[32];       /* Longitude of ascending node of orbit plane */
  byte c_is[16];          /* Amplitude of the sine harmonic correction term */
  byte angle_inclination[32];  /* Inclination angle at the reference time */
  byte c_rc[16];          /* Amplitude of the cosine harmonic correction term */
  byte omega[32];         /* Argument of perigee */
  byte omegadot[24];      /* Rate of right ascension */
  byte idot[14];          /* Rate of inclination angle */
  byte toc[16];           /* Clock data reference time */
  byte af2[8];            /* Apparant satellite clock correction af2 */
  byte af1[16];           /* Apparant satellite clock correction af1 */
  byte af0[22];           /* Apparant satellite clock correction af0 */
} pai_eph_res_var_type;
#define PAI_EPH_RES_VAR_TYPE_SZ (                                          \
  FSIZ( pai_eph_res_var_type, sv_prn_num )        +                        \
  FSIZ( pai_eph_res_var_type, iode )              +                        \
  FSIZ( pai_eph_res_var_type, c_rs )              +                        \
  FSIZ( pai_eph_res_var_type, delta_n )           +                        \
  FSIZ( pai_eph_res_var_type, m0 )                +                        \
  FSIZ( pai_eph_res_var_type, c_uc )              +                        \
  FSIZ( pai_eph_res_var_type, eccentricity )      +                        \
  FSIZ( pai_eph_res_var_type, c_us )              +                        \
  FSIZ( pai_eph_res_var_type, a_sqrt )            +                        \
  FSIZ( pai_eph_res_var_type, toe )               +                        \
  FSIZ( pai_eph_res_var_type, c_ic )              +                        \
  FSIZ( pai_eph_res_var_type, omega_0 )           +                        \
  FSIZ( pai_eph_res_var_type, c_is )              +                        \
  FSIZ( pai_eph_res_var_type, angle_inclination ) +                        \
  FSIZ( pai_eph_res_var_type, c_rc )              +                        \
  FSIZ( pai_eph_res_var_type, omega )             +                        \
  FSIZ( pai_eph_res_var_type, omegadot )          +                        \
  FSIZ( pai_eph_res_var_type, idot )              +                        \
  FSIZ( pai_eph_res_var_type, toc )               +                        \
  FSIZ( pai_eph_res_var_type, af2 )               +                        \
  FSIZ( pai_eph_res_var_type, af1 )               +                        \
  FSIZ( pai_eph_res_var_type, af0 ))



/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*         PROPRIETARY DATA MESSAGE TYPES (pd_msg_type==0xc0)              */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*               PRDM RESPONSE TYPES                                       */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#define PRDM_MS_REV_RESP_CODE            0x01 


#define PRDM_BSP_RESP_CODE               0x02 



/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*               PRDM MSG RESPONSE LENGTHS  (in bits)                      */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#define PRDM_BSP_RESP_LEN          136

#define PRDM_MS_REV_RESP_LEN       112

/*************************************************************************
** NOTE: The request and response header format for Proprietary data msgs
** are the same as the regular Position Determination Data Messages.
**************************************************************************/

/* BS System Parameter Info MSG (RESP_TYPE='0010')                                      */
typedef PACKED struct PACKED_POST
{
  byte time_ref_cdma[14];            /* CDMA time when sys param is received */
  byte still_active[1];              /* Base Station in Active Set indicator */
  byte band_class[5];                /* Base station class */
  byte cdma_freq[11];                /* CDMA frequency */
  byte pilot_pn[9];                  /* Pilot PN sequence offset index */
  byte sid[15];                      /* System identification */
  byte nid[16];                      /* Network identification */
  byte base_id[16];                  /* Base station identification */
  byte base_lat[22];                 /* Base station latitude */
  byte base_long[23];                /* Base station longitude */
  byte reserved[4];                 /* Reserved */
}pai_prdm_bsp_res_rpt_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  MS Revision Data Message (RESP_TYPE = '0000')                          */

typedef PACKED struct PACKED_POST
{
  byte chip_id[16];              /* Chip ID */
  byte software_rev_id[32];     /* Software revision ID */
  byte device_manufacturer[32]; /* Device manufacturer */
  byte device_rev_id[32];       /* Device revision ID */
} pai_prdm_ms_rev_res_type;


#endif /* TM_PAI_H */
