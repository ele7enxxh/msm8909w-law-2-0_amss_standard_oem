#ifndef TM_IS801_I_H
#define TM_IS801_I_H

#ifdef __cplusplus
extern "C" {
#endif

#include "queue.h"
#include "tm_paii.h"

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  IS801 Internal Data Structure Header File

GENERAL DESCRIPTION
  This module defines the internal data structure for IS801 module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/is801/inc/tm_is801_i.h#1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/07   cl      Changes to use semi-dynamic memory allocation
12/13/06   cl      Increase max client number to 3
12/07/06   cl      Remove rev link resp mask
08/01/06   cl      Initial version
===========================================================================*/

/* ------------------------------------------------------------------------
** Constant Definition Section
** ------------------------------------------------------------------------*/
#define MAX_IS801_SA_PARTS  8
#define MAX_IS801_ALM_PARTS 8
#define MAX_IS801_EPH_PARTS 32


/*============================================================================*/
//  Size (in byte) PDDM Header
/*============================================================================*/
#define IS801_PDDM_HDR_LEN     3


/*============================================================================*/
//  Size (in byte) for fixed Reverse Link Req/Response
/*============================================================================*/
#define REV_LINK_REJ_RESP_SIZE        3
#define REV_LINK_CACK_RESP_SIZE       3
#define REV_LINK_MSI_RESP_SIZE        7
#define REV_LINK_TOM_RESP_SIZE        8


/*============================================================================*/
/* Structure for PRM/PPM fragmentation information */
/*============================================================================*/
typedef struct
{
  boolean  parsed;
  uint8    total_parts;
  uint8    part_num;
  uint8    num_meas_in_this_pddm;
  uint8    sv_arr_idx_to_resume;
} is801_prm_fragment_info_s_type;

typedef struct
{
  boolean  parsed;
  uint8    total_parts;
  uint8    part_num;  
  uint8    num_pilot_in_this_pddm;
  uint8    pilot_arr_idx_to_resume;  
} is801_ppm_fragment_info_s_type;

/*============================================================================*/
//  Data structure for SA part   
/*============================================================================*/
typedef struct
{
  uint16                  ref_bit_num;           /* Reference bit number */
  byte                    num_dr_p;    /* Number of data records in part */
  byte                    dr_size;                   /* Data record size */
  byte                    part_num;               /* Number of this part */
  byte                    total_parts;          /* Total number of parts */
} is801_sa_part_fix_type;
   
typedef struct
{
  byte    nav_msg_bits[MAX_NAV_BITS_IN_BYTES];       /* Nav message bits */
  byte    num_sv_dr;     /* Number of GPS satellites in this data record */
  byte    sv_prn_num[MAX_NUM_SATS_IN_DATA_RECORD]; /* Satellite PRN nums */
} is801_sa_part_var_type;
  
typedef struct
{
  qword                   timestamp;  /* CDMA time the data was received */    
  is801_sa_part_fix_type  fix;
  is801_sa_part_var_type  var[MAX_DATA_RECORDS];
} is801_sa_part_type;

/*============================================================================*/
//  Data structure for ALM part   
/*============================================================================*/
typedef struct
{
  qword                    timestamp;  /* CDMA time the data was received */    
  is801_alm_part_fix_type  fix;
  is801_alm_part_var_type  var[MAX_NUM_ALM_SATS];
} is801_alm_part_type;

/*============================================================================*/
//  Data structure for EPH part   
/*============================================================================*/
typedef struct
{
  qword                   timestamp;  /* CDMA time the data was received */    
  is801_eph_part_fix_type  fix;
  is801_eph_part_var_type  var[MAX_NUM_EPH_SATS];
} is801_eph_part_type;


/*============================================================================*/

/* General request header */
typedef struct
{
  byte    req_type;         /* Request element type */
  byte    req_par_len;      /* Request element parameters length */
} is801_gen_req_hdr;

/* General resp header */
typedef struct
{
  byte    resp_type;               /* Response element type */
  byte    unsol_resp;              /* Unsolicited response */
  byte    resp_par_len;            /* Response element parameters length */
} is801_gen_resp_hdr;


/*---------------------------------------------------------------------------

            TM IS801 Queue Data Structure Definition

---------------------------------------------------------------------------*/
#define TM_IS801_MAX_CLIENT          3    /* Max number of protocol sub-moudles IS801 supports */

/* IS801 fwd-link request bit mask type */
typedef uint32                         is801_fwd_link_wait_mask_type;
#define IS801_FWD_LINK_WAIT_MASK_NONE  0x0000
#define IS801_FWD_LINK_WAIT_MASK_SA    0x0001
#define IS801_FWD_LINK_WAIT_MASK_ALM   0x0002
#define IS801_FWD_LINK_WAIT_MASK_EPH   0x0004

/* ------------------------------------------------------------------------
** MS to PDE Request Typedefs
** ------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST GPS ACQUISITION ASSISTANCE                                      */
/*                            (see IS-801 Section 3.2.4.1 REQ_TYPE='0100') */
typedef struct 
{
  byte    dopp_req;                /* Doppler (0th order) term requested */
  byte    add_dopp_req;            /* Additional Doppler terms requested */
  byte    code_ph_par_req;            /* Code phase parameters requested */
  byte    az_el_req;            /* Azimuth and elevation angle requested */
} is801_aa_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST LOCATION RESPONSE  (see IS-801 Section 3.2.4.1 REQ_TYPE='0001') */

typedef struct
{
  byte    height_req;                    /* Height information requested */
  byte    clk_cor_gps_req;    /* Clock correction for GPS time requested */
  byte    velocity_req;                /* Velocity information requested */
} is801_lr_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST GPS EPHEMERIS      (see IS-801 Section 3.2.4.1 REQ_TYPE='1001') */
typedef struct
{
  byte ab_par_req;                    /* Alpha/Beta Parameters Requested */
} is801_eph_req_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* REQUEST ALMANAC CORRECTION (see IS-801 Section 3.2.4.1 REQ_TYPE='1011') */
typedef struct
{
  byte toa;                                            /* Time Of Almanac */
  byte week_num;                                       /* GPS Week Number */
} is801_alm_cor_req_type;
  
/* is801 reverse link request type */
typedef union
{ 
  is801_aa_req_type       aa_req;
  is801_lr_req_type       lr_req;
  is801_eph_req_type      eph_req;
  is801_alm_cor_req_type  alm_cor_req;
} is801_rev_link_req_msg_payload_u_type;

/* is801 reverse link request type */
typedef struct
{ 
  is801_rev_link_req_mask_type          request_type;
  is801_rev_link_req_msg_payload_u_type req_msg_payload;
} is801_rev_link_req_s_type;


/* Each Reverse Link Request Occupies an Request Queue Element */
typedef struct
{
  q_link_type                link;
  is801_rev_link_req_s_type  req_msg;
} is801_rev_link_req_queue_element_s_type;

/* Each Reverse Link Response Occupies an Response Queue Element */
typedef struct
{
  q_link_type                link;
  is801_rev_link_resp_s_type resp_msg;
} is801_rev_link_resp_queue_element_s_type;


/* Union for fwd link response types */
typedef union
{
  is801_rej_resp_type    rej_resp;
  is801_aa_resp_type     aa_resp;
  is801_lr_resp_type     lr_resp;
  is801_sa_part_type     sa_part;
  is801_alm_part_type    alm_part;  
  is801_eph_part_type    eph_part;  
} is801_fwd_link_resp_part_payload_u_type;

/* is801 forward link resp type */
typedef struct
{ 
  is801_fwd_link_resp_mask_type             resp_type;
  boolean                                   unsolicited;  
  is801_fwd_link_resp_part_payload_u_type   resp_msg_payload;        /* Place holder for processed IS801 Response (unpacked) */
} is801_fwd_link_resp_part_s_type;

typedef struct
{
  is801_fwd_link_resp_s_type               *part_resp_holder_ptr;
  byte                                      part_idx;
  byte                                      part_mask;
  byte                                      curr_dr_index;
} is801_client_part_resp_process_s_type;

/* Each PDDM Occupies an PDDM Queue Element */
typedef struct
{
  q_link_type                              link;
  byte                                     req_rd_idx;
  byte                                     resp_rd_idx;  
  is801_fwd_link_wait_mask_type            wait_mask;
  is801_client_part_resp_process_s_type    sa_resp_process;
  is801_client_part_resp_process_s_type    alm_resp_process;  
  is801_client_part_resp_process_s_type    eph_resp_process;      
  is801_fwd_link_pddm_info_s_type          pddm_info;
} is801_fwd_link_pddm_info_queue_element_s_type;

/* Each PDDM Occupies an PDDM Queue Element */
typedef struct
{
  q_link_type                              link;
  is801_rev_link_pddm_info_s_type          pddm_info;
} is801_rev_link_pddm_queue_element_s_type;


/* Data Structure for Clients */
typedef struct
{
  boolean                        active;
  tm_prtl_type                   prtl_type;
  prtl_is801_msg_cb_f_type       *client_msg_cb_fp;

  /* Data structures for fwd link pddm */
  q_type fwd_link_pddm_q;
  
  /* Data structures for rev link pddm */
  q_type rev_link_req_q;

  q_type rev_link_resp_q;

  q_type rev_link_pddm_q;

} is801_client_data_s_type;

#ifdef __cplusplus
}
#endif

#endif /* TM_IS801_I_H */

