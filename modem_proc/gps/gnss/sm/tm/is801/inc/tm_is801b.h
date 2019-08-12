#ifndef TM_IS801B_H
#define TM_IS801B_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "customer.h"
#include "gnss_common.h"
#include "custcgps.h"

#if 0 // comment out as OSS has been removed 
#ifdef FEATURE_CGPS_OSS_QDSP6
#include "is801_b_asn1_qdsp6.h"
#elif defined(T_WINNT)
#error code not present
#else
#include "is801_b_asn1.h"
#endif
#endif // #if 0


#include "tm_asn1util.h"
#include "sm_api.h"
#include "tm_common.h"

#define IS801B_FIRST_PART 1
#define IS801B_MAX_PARTS  16


/* IS801B reverse-link request bit mask type (MS Req to PDE) */
typedef uint32                        is801B_rev_link_req_mask_type;
#define IS801B_REV_LINK_REQ_MASK_NONE       0x0000
#define IS801B_REV_LINK_REQ_MASK_ADV_BSC    0x0001  /* Advanced BS Capabilities */
#define IS801B_REV_LINK_REQ_MASK_GNSS_AA    0x0002  /* Acquisition Assistance, GNSS */
#define IS801B_REV_LINK_REQ_MASK_GNSS_SA    0x0004  /* Sensitivity Assistance, GNSS */
#define IS801B_REV_LINK_REQ_MASK_GGTO       0x0008  /* GNSS-GNSS Time offset */
#define IS801B_REV_LINK_REQ_MASK_GPS_ALM    0x0010  /* GPS Almanac */
#define IS801B_REV_LINK_REQ_MASK_GPS_EPH    0x0020  /* GPS Ephemeris */
#define IS801B_REV_LINK_REQ_MASK_GLO_ALM    0x0040  /* Glonass Almanac */
#define IS801B_REV_LINK_REQ_MASK_GLO_EPH    0x0080  /* Glonass Ephemeris */
#define IS801B_REV_LINK_REQ_MASK_ADV_LR     0x0100  /* Location Response */
#define IS801B_REV_LINK_REQ_MASK_GNSS_SVHI  0x0200  /* GPS Satellite Health Information  */
#define IS801B_REV_LINK_REQ_MASK_DGNSS_ASST 0x0400  /* DGNSS Assistance */

#if 0
/* TBD probably not needed for 801-B ? */
#define IS801_REV_LINK_REQ_MASK_BSA   0x0010  /* Base Station Almanac */
#define IS801_REV_LINK_REQ_MASK_NMB   0x0080  /* Navigation Message Bits */
#define IS801_REV_LINK_REQ_MASK_AC    0x0200  /* GPS Almanac Correction */
#define IS801_REV_LINK_REQ_MASK_LA    0x0004  /* Location Assistance */
#define IS801_REV_LINK_REQ_NUM        11      /* TBD update */
#endif


/* IS801B reverse-link response bit mask type (MS Resp to PDE) */
typedef uint32                         is801B_rev_link_resp_mask_type;
#define IS801B_REV_LINK_RESP_MASK_NONE       0x0000
#define IS801B_REV_LINK_RESP_MASK_REJ        0x0001 /* Rejecting 801-1 message with PD_MSG_TYPE==0x00*/
#define IS801B_REV_LINK_RESP_MASK_EXT_REJ    0x0002 /* Rejecting 801-A message with PD_MSG_TYPE==0x01*/
#define IS801B_REV_LINK_RESP_MASK_ADV_REJ    0x0003 /* Rejecting 801-B message with PD_MSG_TYPE==0x10*/
#define IS801B_REV_LINK_RESP_MASK_MSI        0x0004 /* MS Information */
#define IS801B_REV_LINK_RESP_MASK_ADV_MSI    0x0010 /* MS Information */
#define IS801B_REV_LINK_RESP_MASK_GNSS_PRM   0x0020 /* Pseudorange Measurement  */
#define IS801B_REV_LINK_RESP_MASK_CACK       0x0030 /* Pseudorange Measurement  */
#define IS801B_REV_LINK_RESP_MASK_ADV_CACK   0x0040 /* Pseudorange Measurement  */
#define IS801B_REV_LINK_RESP_MASK_PPM        0x0100 /* Pilot Phase Measurement  */
#define IS801B_REV_LINK_RESP_MASK_REF_LOC    0x0200 /* Ref Location Response    */
#define IS801B_REV_LINK_RESP_MASK_TOM        0x0300 /* Time Offset Measurement  */
#define IS801B_REV_LINK_RESP_MASK_ADV_SPI    0x0400 /* Adv. System Param Info   */
#define IS801B_REV_LINK_RESP_MASK_ADV_LR     0x0800 /* Adv. (Ref) Location Resp */ 
#if 0
/* TBD Not required for 801B ?  */
#define IS801B_REV_LINK_RESP_MASK_CACK  0x0080 /* Cancellation Ack */
#define IS801B_REV_LINK_RESP_MASK_AMWF  0x0004 /* Autonomous Measurement Weighting Factors */ 
#define IS801B_REV_LINK_RESP_NUM            9
#endif


/* IS801B fwd-link request bit mask type (PDE req to MS) */
typedef uint32                        is801B_fwd_link_req_mask_type;
#define IS801B_FWD_LINK_REQ_MASK_NONE        0x0000  /* NONE */
#define IS801B_FWD_LINK_REQ_MASK_MSI         0x0001  /* MS Information (801-1) */
#define IS801B_FWD_LINK_REQ_MASK_ADV_MSI     0x0002  /* Advanced MS Information (801-B)*/
#define IS801B_FWD_LINK_REQ_MASK_PPM         0x0004  /* Pilot Phase Measurement */
#define IS801B_FWD_LINK_REQ_MASK_GNSS_PRM    0x0008  /* GNSS Pseudorange Measurement */
#define IS801B_FWD_LINK_REQ_MASK_LR          0x0010  /* Location Response */
#define IS801B_FWD_LINK_REQ_MASK_ADV_LR      0x0020  /* Location Response */
#define IS801B_FWD_LINK_REQ_MASK_CAN         0x0040  /* Cancellation */
#define IS801B_FWD_LINK_REQ_MASK_ADV_CAN     0x0080  /* Advanced Cancellation (801-B) */
#define IS801B_FWD_LINK_REQ_MASK_ADV_SPI     0x0100  /* Adv System Parameter Information */
#define IS801B_FWD_LINK_REQ_MASK_PPM_UMB     0x0200  /* PPM meas on UMB  */
#define IS801B_FWD_LINK_REQ_MASK_TOM         0x0400
#define IS801B_FWD_LINK_REQ_MASK_UMB_TOM     0x0800
#define IS801B_FWD_LINK_REQ_MASK_AMWF        0x1000 
#define IS801B_FWD_LINK_REQ_MASK_HRPD_PPM    0x2000

#if 0
/* TBD not needed for 801B */
#define IS801B_FWD_LINK_REQ_MASK_TOM   0x0020  /* Time Offset Measurement. Not supported as per ref guide */
#define IS801B_FWD_LINK_REQ_MASK_AMWF  0x0002  /* Autonomous Measurement Weighting Factors */ 
#endif

/* IS801B fwd-link response bit mask type */
typedef uint32                         is801B_fwd_link_resp_mask_type;
#define IS801B_FWD_LINK_RESP_MASK_NONE       0x0000  /* NONE */
#define IS801B_FWD_LINK_RESP_MASK_REJ        0x0001  /* Reject */
#define IS801B_FWD_LINK_RESP_MASK_EXT_REJ    0x0002  /* Reject */
#define IS801B_FWD_LINK_RESP_MASK_ADV_REJ    0x0004  /* Reject */
#define IS801B_FWD_LINK_RESP_MASK_BSC        0x0008  /* BS Capabilities */
#define IS801B_FWD_LINK_RESP_MASK_ADV_BSC    0x0010  /* BS Capabilities */
#define IS801B_FWD_LINK_RESP_MASK_GPS_ALM    0x0020  /* Almanac */
#define IS801B_FWD_LINK_RESP_MASK_GPS_EPH    0x0040  /* Ephemeris */
#define IS801B_FWD_LINK_RESP_MASK_GLO_ALM    0x0080  /* Almanac */
#define IS801B_FWD_LINK_RESP_MASK_GLO_EPH    0x0100  /* Ephemeris */
#define IS801B_FWD_LINK_RESP_MASK_GNSS_AA    0x0200  /* Acquisition Assistance */
#define IS801B_FWD_LINK_RESP_MASK_GGTO       0x0400  /* Acquisition Assistance */
#define IS801B_FWD_LINK_RESP_MASK_GPS_SA     0x0800  /* Sensitivity Assistance */
#define IS801B_FWD_LINK_RESP_MASK_ADV_LR     0x1000  /* Location Response */
#define IS801B_FWD_LINK_RESP_MASK_ADV_SVHI   0x2000  /* GPS Satellite Health Information */
#define IS801B_FWD_LINK_RESP_MASK_DGNSS_ASST 0x4000  /* GPS Satellite Health Information */


#if 0
/* TBD not needed for 801B */
#define IS801B_FWD_LINK_RESP_MASK_LASC  0x0008  /* Location Assistance Spherical Coordinates */
#define IS801B_FWD_LINK_RESP_MASK_LACC  0x0010  /* Location Assistance Cartesian Coordinates */
#define IS801B_FWD_LINK_RESP_MASK_AC    0x0800  /* GPS Almanac Correction */
#define IS801B_FWD_LINK_RESP_MASK_BSA   0x0040  /* Base Station Almanac */
#define IS801B_FWD_LINK_RESP_MASK_NMB   0x0200  /* Navigation Message Bits */
#endif

/* 
 * Protocol constants defined by IS801B
 */
#define IS801B_BS_LOCATION_REV_NUMBER 2

typedef uint8 is801b_gnss_id;
#define GNSS_ID_GPS                0x1
#define GNSS_ID_SBAS               0x2
#define GNSS_ID_GLO                0x4

typedef uint8 is801b_gnss_sig_id;
#define GNSS_GPS_SIG_ID_L1CA       0x01
#define GNSS_GPS_SIG_ID_L1C        0x02
#define GNSS_GPS_SIG_ID_L2C        0x04
#define GNSS_GPS_SIG_ID_L5         0x08

#define GNSS_GLO_SIG_ID_G1         0x01
#define GNSS_GLO_SIG_ID_G2         0x02
#define GNSS_GLO_SIG_ID_G3         0x04

typedef uint16 is801b_aflt_cpbl;
#define IS801B_AFLT_CPBL_1X_MSB    0x01
#define IS801B_AFLT_CPBL_1X_MSA    0x02
#define IS801B_AFLT_CPBL_HRPD_MSB  0x04
#define IS801B_AFLT_CPBL_HRPD_MSA  0x08
#define IS801B_AFLT_CPBL_UMB_MSB   0x10
#define IS801B_AFLT_CPBL_UMB_MSA   0x20

/* consts to set ref time src when sending PRMs to PDE */
#define TIME_REFERENCE_SOURCE_CDMA 0x0
#define TIME_REFERENCE_SOURCE_GPS  0x1
#define TIME_REFERENCE_SOURCE_GLO  0x3

/* const relative to which all code phases shall be adjusted when sending PRM
 *  to PDE */
#define IS801B_MSEC_CODE_PHASE_ORIGIN      73

/* 801 protocol version supported by MS. 801B == 2 */
#define MS_LOCATION_REV       2

/* consts used to send Mobile System Information (MSI) PDDM */
#define GNSS_ACQ_CAPABILITY_MODE_ASSISTED  0x3
#define GNSS_ACQ_CAPABILITY_MODE_BASED     0xf

#define POSN_CALC_CAPABILITY_MODE_ASSISTED 0x0
#define POSN_CALC_CAPABILITY_MODE_BASED    0x18

/*
 * convenience macros
 */
#define ERROR_CHECK_3PTRS(X, Y, Z)                        \
  do                                                      \
  {                                                       \
    if(X == NULL || Y == NULL || Z == NULL)               \
    {                                                     \
      MSG_ERROR("Null ptr passed, %x, %x, %x", X, Y, Z);  \
      return FALSE;                                       \
    }                                                     \
  }while(0);                                              \

#define ERROR_CHECK_2PTRS(X, Y)                           \
  do                                                      \
  {                                                       \
    if(X == NULL || Y == NULL)                            \
    {                                                     \
      MSG_ERROR("Null ptr passed, %x, %x", X, Y, 0);      \
      return FALSE;                                       \
    }                                                     \
  }while(0);                                              \

#define ERROR_CHECK_PTR(X)                                \
  do                                                      \
  {                                                       \
    if(X == NULL)                                         \
    {                                                     \
      MSG_ERROR("Null ptr passed, %x", X, 0, 0);          \
      return FALSE;                                       \
    }                                                     \
  }while(0);                                              \


typedef struct
{
 /* set req type to is801B or is801_fwd_link_req_mask_type */
  is801B_fwd_link_req_mask_type rejReqType;
#define  IS801B_REJ_REASON_CAP_NOT_SUPPORTED  (1)
#define  IS801B_REJ_REASON_CAP_TEMP_UNAVLBL   (2)
#define  IS801B_REJ_REASON_CAP_UNAVLBL_SESS   (3)
  uint8  rejReqReason;
}rev_link_resp_rej;

typedef struct
{
  is801B_fwd_link_req_mask_type cack_response_type;
  uint8                         no_outstanding_req;
} rev_link_resp_cack;

typedef union
{
  sm_GnssFixRptStructType  *ref_loc;
  sm_GpsMeasRptStructType  *meas;
  srch_ppm_rpt_type        *ppm_rpt;
  rev_link_resp_rej        *rl_resp_rej;
  rev_link_resp_cack       *rl_resp_cack;
}u_new_rev_link_resp_params;

/******************************************************************************/
/***** is801b_flink_pddm_common HAS to be the first element of each flink *****/
/***** information element, otherwise, badness will happen                *****/
/******************************************************************************/

#define MAX_PDU_FRAGMENTS  7
typedef struct
{
  boolean   unsolicited;
  boolean   reject;
  uint8     sess_flags;
/* elements below this comment are private to tm_is801b encoder decoder layer.
 * Any changes are forbidden in any file except tm_is801b.c */
  boolean   in_use;
  uint16    part_wait_mask;
  uint8     pdu_ref[MAX_PDU_FRAGMENTS];
  int8      pdu_ref_idx;  
}is801b_flink_pddm_common;

typedef struct
{
  #if ((defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
      (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
  #error code not present
#else
  uint8 u_dummy;
  #endif
}is801b_flink_ie_gps_eph;

typedef struct
{
  #if (  (defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
         (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
#error code not present
#else
  uint8 u_dummy;
  #endif
  
}is801b_flink_ie_gps_alm;

typedef struct
{
  #if (  (defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
         (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
#error code not present
#else
  uint8 u_dummy;
  #endif  
  
}is801b_flink_ie_glo_eph;

typedef struct
{
  #if (  (defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
         (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
#error code not present
#else
  uint8 u_dummy;
  #endif  
  
}is801b_flink_ie_glo_alm;

typedef struct
{

  #if (  (defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
         (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
#error code not present
#else
  uint8 u_dummy;
  #endif  
  
  
}is801b_flink_ie_gps_sa;

typedef struct
{

  #if (  (defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
         (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
#error code not present
#else
  uint8 u_dummy;
  #endif  
  
  
}is801b_flink_ie_adv_lr;

typedef struct
{

  #if (  (defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
         (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
#error code not present
#else
  uint8 u_dummy;
  #endif  
  
  
}is801b_flink_ie_gnss_aa;

typedef struct
{

  #if (  (defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
         (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
#error code not present
#else
  uint8 u_dummy;
  #endif  
  
  
}is801b_flink_ie_adv_bsc;

typedef struct
{
  #if (  (defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
         (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
#error code not present
#else
  uint8 u_dummy;
  #endif  
  
  
}is801b_flink_ie_ggto;

typedef struct
{

  #if (  (defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
         (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
#error code not present
#else
  uint8 u_dummy;
  #endif  
  
  
}is801b_flink_ie_svhi;

typedef struct 
{
  uint8                     prefer_resp_qual;
  uint8                     num_fixes;
  uint8                     time_bet_fix;
  boolean                   offset_requested;
  uint8                     pilot_phase_res_desired;
} is801_b_ppm_req;

typedef struct 
{
  uint8                     prefer_resp_qual;
  uint8                     num_fixes;
  uint8                     time_bet_fix;
  uint8                     gnss_ids;
  uint8                     gnss_sig_ids;
} is801_b_prm_req;

typedef struct
{
  /***** common HAS to be the first element of the struct *****/
  is801b_flink_pddm_common       common;
  is801_b_ppm_req                ppm_req;
  is801_b_prm_req                prm_req;
  is801B_fwd_link_req_mask_type  cancel_req_type;
  is801B_fwd_link_req_mask_type  adv_cancel_req_type;
}is801b_flink_ie_req;
#if 0
typedef struct
{
  boolean request;
  union
  {
    is801B_fwd_link_req_mask_type  req;
    is801b_flink_ie_struct         resp;
  }u;
}is801b_flink_resp_pddm;
#endif

typedef struct
{
  union               /* Flags for optional data */
  {
    uint8 byte;
    struct
    {
      uint8  sess_start  : 1;  /* Start-of-session indicator */
      uint8  sess_end    : 1;  /* End-of-session indicator   */
      uint8  sess_source : 1;  /* Session-source indicator   */
      uint8  sess_tag    : 5;  /* Session tag                */
    } z_Mask;  
  } u_first_byte;
  byte    pd_msg_type;         /* PDDM type (=2 since 801b)  */
  byte    pd_msg_len_msb;
  byte    pd_msg_len_lsb;
} is801b_pddm_hdr_type;

/*
 * Bit masks corresponding with is801b_pddm_hdr_type.u_first_byte.z_Mask
 */
#define  TM_1X_IS801B_SESS_START_MASK  0x80
#define  TM_1X_IS801B_SESS_END_MASK    0x40
#define  TM_1X_IS801B_SESS_SRC_MASK    0x20
#define  TM_1X_IS801B_SESS_TAG_MASK    0x1f

/*===========================================================================

FUNCTION    : is801b_get_flink_pddm

DESCRIPTION : external Function to that is invoked by the protocol submodule
              to obtain a decoded flink pddm.
              This function sends all requests and 1 response information elt 
              to the protocol. 
              The order in which information elements are sent to prtl is 
              predefined.
DEPENDENCIES

ARGUMENTS
RETURN VALUE

SIDE EFFECTS
===========================================================================*/

boolean is801b_get_flink_pddm
(
  void                            **pddm_ptr,
  is801B_fwd_link_req_mask_type    *req_mask_v1,
  is801B_fwd_link_req_mask_type    *req_mask_v3,
  is801B_fwd_link_resp_mask_type   *resp_mask,
  uint8                            *sess_flags
);

/*===========================================================================

FUNCTION    : is801b_new_rev_link_resp

DESCRIPTION : external Function to inform the encoder-decoder layer that a
              reverse link resp needs to be encoded. This new resp is 
              updated in the unpacked global reverse link PDU, and is turned
              into a PDDM, when the caller later calls 
              is801b_create_rev_link_pddm 

DEPENDENCIES

ARGUMENTS
RETURN VALUE
TRUE  : Successfully accepted new rev link req
FALSE : Error case.

SIDE EFFECTS
Modifies global rlink_pdu to reflect the new request.
===========================================================================*/
boolean is801b_new_rev_link_resp(
                             tm_prtl_type                   prtl_type,                             
                             is801B_rev_link_resp_mask_type resp_type,
//                             is801b_gnss_id                 gnss_id,
                             boolean                        unsolicited,  /* solicited/unsolicited resp */
                             boolean                        assisted,     /* server computed fix for this session? */
                             u_new_rev_link_resp_params    *rl_resp_params
                           );

/*===========================================================================

FUNCTION    : is801b_new_rev_link_req

DESCRIPTION : external Function to inform the encoder-decoder layer that a
              reverse link req needs to be encoded. This new request is 
              updated in the unpacked global reverse link PDU, and is turned
              into a PDDM, when the caller later calls 
              is801b_create_rev_link_pddm 

DEPENDENCIES

ARGUMENTS
RETURN VALUE
TRUE  : Successfully accepted new rev link req
FALSE : Error case.

SIDE EFFECTS
Modifies global rlink_pdu to reflect the new request.
===========================================================================*/

boolean is801b_new_rev_link_req
(
  tm_prtl_type                   prtl_type,                             
  is801B_rev_link_req_mask_type  req_type
//is801b_gnss_id                 gnss_id
);


/*===========================================================================
FUNCTION is801b_create_rev_link_pddm

DESCRIPTION
 When protocol sub-modules creates rev-link PDDM, it also needs to tell IS801 
 module the maximum size of PDDM, which is limited by each protocol sub-modules'
 transport layer. 

 For example, 1x cp uses DBM as its transport layer, and the maximum dbm size 
 can differ from network to network.

 The function looks at the global reverse link message that would have been 
 updated by repeated calls to create_rev_link_req and rev_link_resp messages,
 and creates the required number of PDDMs to encode these messages.

 First, all the req messages are sent, followed by all responses other than
 PPM/PRM, followed by PPM resp and then PRM resp.

 If a PDDM is not capable of taking on all the PPM and PRM responses, the 
 smallest PPM and the smallest PRM response are sent in that PDDM, and the rest
 of the data is sent in follow on PDDMs. This is done so that when the MS sends
 unsolicited PPM/PRM resp, the server knows that there are both PPMs and PRMs 
 to be read.
 
DEPENDENCIES

RETURN VALUE
  Number of PDDM created.
  -1 indicates there is error when creating PDDM.

  Client should then call is801b_get_rev_link_pddm to get packed PDDM data to send it
  over to PDE.

SIDE EFFECTS

===========================================================================*/

int8 is801b_create_rev_link_pddm
(
  tm_prtl_type                 prtl_type,
  uint8                        flags,
  uint8                        sess_tag,
  uint8                        max_pddm_size
);

/*===========================================================================

FUNCTION    : is801b_get_rev_link_pddm

DESCRIPTION : external Function to get a packed rev link pddm from the encoder
              decoder layer

DEPENDENCIES

ARGUMENTS
RETURN VALUE
TRUE  : Successfully sending rlink pddm
FALSE : Error case.

SIDE EFFECTS
Modifies global rlink_pdu to reflect the new request.
===========================================================================*/
boolean is801b_get_rev_link_pddm
(
  tm_pdu_buf_type       **rlink_pddm,
  is801b_pddm_hdr_type  **pddm_hdr
);

/*===========================================================================

FUNCTION is801b_decode_fwd_link_pddm

DESCRIPTION : This function accepts a packed PDDM and unpacks it using the oss
              decoder. If the PDDM contains fragments, it is put in a queue.

              Once all information element fragments are received, the callback
              regitered by the calling protocol is invoked and the number of
              decoded information elements is returned.

DEPENDENCIES

INPUT PARAMS:
  p_data      : pointer to packed data containing PDDMs
  w_data_len  : length of data to be decoded
  prtl_type   : which protocol (1xCP/1xUP)
OUTPUT PARAMS:

RETURN VALUE :
  FALSE       : In case of error
  TRUE        : No errors in decoding.

SIDE EFFECTS

===========================================================================*/
boolean is801b_decode_fwd_link_pddm
(
  tm_prtl_type   prtl_type,
  uint8         *p_data, 
  uint16         w_data_len,
  uint16        *bytes_consumed
);

/*===========================================================================

FUNCTION: prtl_is801b_msg_cb_f_type

DESCRIPTION:
  This function is called by IS801 module to protocol sub-module to inform
  PDDM data is available.
  
PARAMETERS:
  
===========================================================================*/
typedef boolean (prtl_is801b_msg_cb_f_type) 
(
  uint8 num_pddm
);

/*===========================================================================

FUNCTION    : is801b_prtl_reg

DESCRIPTION : external Function called by protocol submodule to register a 
callback.

DEPENDENCIES

ARGUMENTS
RETURN VALUE
TRUE  : Successfully registered
FALSE : Error case.

SIDE EFFECTS
===========================================================================*/
boolean is801b_prtl_reg
(
  tm_prtl_type              prtl_type,
  prtl_is801b_msg_cb_f_type  *msg_cb_fp
);

/*===========================================================================

FUNCTION    : is801b_free_flink_info_elt

DESCRIPTION : External function called to free up any pdu's that an information
              element might have a reference to. This is done by decrementing
              the ref count of any pdu that the information element may have a 
              reference to.
DEPENDENCIES

ARGUMENTS
RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void is801b_free_flink_info_elt
(
  is801b_flink_pddm_common         *common
);

/*===========================================================================

FUNCTION is801b_reset_data

DESCRIPTION
  This function needs to be called when client finishes a session
  for any reason.

  It ensures that all the data structures are re-initiailzed for future IS801B
  sessions.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void is801b_reset_data
(
  tm_prtl_type prtl_type
);


#ifdef __cplusplus
}
#endif

#endif 
