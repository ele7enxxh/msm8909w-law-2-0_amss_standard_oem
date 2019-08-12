/*****************************************************************************
***
*** TITLE  gllcxid.c
***
***  GPRS LLC Layer Exchange of Information (XID) Functions
***
***
*** DESCRIPTION
***
***  The LLC Exchange of Information functions provide for the validation and
***  generation of XID negotiation request/response information elements, and
***  for the setting of negotiated LLC parameters.
***
***  These functions are for the internal use of the LLC layer implementation,
***  and would be employed during any message processing which embodies XID
***  negotiation elements.
***
*** 
*** EXTERNALIZED FUNCTIONS
***
***  gllc_lle_validate_u_format_cr_xid_req()
***  gllc_lle_validate_u_format_cr_xid_neg()
***
***  gllc_lle_store_u_format_cr_xid_req()
***  gllc_lle_store_u_format_cr_xid_neg()
***
***  gllc_lle_generate_l2_xid_req()
***  gllc_lle_generate_l2_xid_neg()
***
***  gllc_lle_reset_l2_xid_parameters()
***  gllc_lle_set_l2_xid_parameters()
***
***  gllc_llme_reset_l2_xid_parameters()
***
***  gllc_lle_report_l2_xid_content()
*** 
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  None
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/src/gllcxid.c#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz	   changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "gllci.h"
#include "msg.h"
#include "string.h"


/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/* ------------------------------------------------------------------------ */
/* Minimum and maximum XID parameter limits                                 */
/* ------------------------------------------------------------------------ */

#define GLLC_MIN_VERSION                  0
#define GLLC_MAX_VERSION                  15

/* ------------------------------------------------------------------------ */

/* T200 retransmission timeouts in milliseconds. */

#define GLLC_MIN_T200_MS                  100
#define GLLC_MAX_T200_MS                  409500

/* ------------------------------------------------------------------------ */

/* N200 retransmission attempt limits. */

#define GLLC_MIN_N200                     1
#define GLLC_MAX_N200                     15

/* ------------------------------------------------------------------------ */

/* N201_I confirmed information frame maximum info field lengths in number of
   octets. */

#define GLLC_MIN_N201_I                   140
#define GLLC_MAX_N201_I                   1520

/* ------------------------------------------------------------------------ */

/* mD resequencing buffer window size limits in multiples of 16 octets. */

#define GLLC_MIN_MD_OCTET16               9
#define GLLC_MAX_MD_OCTET16               24320

/* ------------------------------------------------------------------------ */

/* mU retransmission buffer window size limits in multiples of 16 octets. */

#define GLLC_MIN_MU_OCTET16               9
#define GLLC_MAX_MU_OCTET16               24320

/* ------------------------------------------------------------------------ */

/* kD resequencing buffer window size limits in frames. */

#define GLLC_MIN_KD_FRAMES                1
#define GLLC_MAX_KD_FRAMES                255

/* ------------------------------------------------------------------------ */

/* kU retransmission buffer window size limits in frames. */

#define GLLC_MIN_KU_FRAMES                1
#define GLLC_MAX_KU_FRAMES                255


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/* ------------------------------------------------------------------------ */
/*  Default and required operating parameters.								*/
/* ------------------------------------------------------------------------ */

#define GLLC_SAPI_NOT_SUPPORTED           0

/* ------------------------------------------------------------------------ */

#define GLLC_DFLT_VERSION                 0
#define GLLC_PRFD_VERSION                 0

#define GLLC_DFLT_IOV_UI                  0L

/* ------------------------------------------------------------------------ */

/* Input Offset Values (IOV-I) for acknowledged operation. */

LOCAL const uint32 gllc_dflt_IOV_I[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */


  (0x08000000L *  3),     /* GLLC_LL_SAPI_3_LL3   */
  (0x08000000L *  5),     /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */


  (0x08000000L *  9),     /* GLLC_LL_SAPI_9_LL9   */
  (0x08000000L * 11)      /* GLLC_LL_SAPI_11_LL11 */
};

/* ------------------------------------------------------------------------ */

/* T200 & T201 retransmission timeouts in milliseconds. */

LOCAL const uint32 gllc_dflt_T200_ms[ GLLC_LL_NUM_SAPIS ] =
{
  5000,                   /* GLLC_LL_SAPI_1_GMM   */
  5000,                   /* GLLC_LL_SAPI_3_LL3   */
  10000,                  /* GLLC_LL_SAPI_5_LL5   */
  20000,                  /* GLLC_LL_SAPI_7_SMS   */
  20000,                  /* GLLC_LL_SAPI_9_LL9   */
  40000                   /* GLLC_LL_SAPI_11_LL11 */
};

/* ....................................................................... */

#define GLLC_MIN_ACCEPTABLE_T200_MS                                         \
        gllc_dflt_T200_ms[GLLC_LL_SAPI_3_LL3]

/* ....................................................................... */

const uint32 gllc_prfd_T200_ms[ GLLC_LL_NUM_SAPIS ] =
{
  5000,                   /* GLLC_LL_SAPI_1_GMM   */
  5000,                   /* GLLC_LL_SAPI_3_LL3   */
  5000,                   /* GLLC_LL_SAPI_5_LL5   */
  20000,                  /* GLLC_LL_SAPI_7_SMS   */
  5000,                   /* GLLC_LL_SAPI_9_LL9   */
  5000                    /* GLLC_LL_SAPI_11_LL11 */
};

/* ------------------------------------------------------------------------ */

/* N200 retransmission attempt limits. */

LOCAL const uint8 gllc_dflt_N200[ GLLC_LL_NUM_SAPIS ] =
{
  3,                      /* GLLC_LL_SAPI_1_GMM   */
  3,                      /* GLLC_LL_SAPI_3_LL3   */
  3,                      /* GLLC_LL_SAPI_5_LL5   */
  3,                      /* GLLC_LL_SAPI_7_SMS   */
  3,                      /* GLLC_LL_SAPI_9_LL9   */
  3                       /* GLLC_LL_SAPI_11_LL11 */
};

/* ....................................................................... */

const uint8 gllc_prfd_N200[ GLLC_LL_NUM_SAPIS ] =
{
  3,                      /* GLLC_LL_SAPI_1_GMM   */
  3,                      /* GLLC_LL_SAPI_3_LL3   */
  3,                      /* GLLC_LL_SAPI_5_LL5   */
  3,                      /* GLLC_LL_SAPI_7_SMS   */
  3,                      /* GLLC_LL_SAPI_9_LL9   */
  3                       /* GLLC_LL_SAPI_11_LL11 */
};

/* ------------------------------------------------------------------------ */

/* N201_U unconfirmed information frame maximum info field lengths in number
   of octets. */

const uint16 gllc_dflt_N201_U_octets[ GLLC_LL_NUM_SAPIS ] =
{
  400,                    /* GLLC_LL_SAPI_1_GMM   */
  500,                    /* GLLC_LL_SAPI_3_LL3   */
  500,                    /* GLLC_LL_SAPI_5_LL5   */
  270,                    /* GLLC_LL_SAPI_7_SMS   */
  500,                    /* GLLC_LL_SAPI_9_LL9   */
  500                     /* GLLC_LL_SAPI_11_LL11 */
};

/* ....................................................................... */

const uint16 gllc_prfd_N201_U_octets[ GLLC_LLME_NUM_GAN_STATES ][ GLLC_LL_NUM_SAPIS ] =
{
  /* Non-GAN State Preferences */
  {
    1520,                 /* GLLC_LL_SAPI_1_GMM   */
    1520,                 /* GLLC_LL_SAPI_3_LL3   */
    1520,                 /* GLLC_LL_SAPI_5_LL5   */
    270,                  /* GLLC_LL_SAPI_7_SMS   */
    1520,                 /* GLLC_LL_SAPI_9_LL9   */
    1520                  /* GLLC_LL_SAPI_11_LL11 */
  },

  /* GAN State Preferences */
  {
    1520,                 /* GLLC_LL_SAPI_1_GMM   */
#ifdef DEBUG_GSM_GPRS_USE_GAN_N201_SIZE
    #error code not present
#else
    1520,                 /* GLLC_LL_SAPI_3_LL3   */
    1520,                 /* GLLC_LL_SAPI_5_LL5   */
#endif /* DEBUG_GSM_GPRS_USE_GAN_N201_SIZE */
    270,                  /* GLLC_LL_SAPI_7_SMS   */
#ifdef DEBUG_GSM_GPRS_USE_GAN_N201_SIZE
    #error code not present
#else
    1520,                 /* GLLC_LL_SAPI_9_LL9   */
    1520                  /* GLLC_LL_SAPI_11_LL11 */
#endif /* DEBUG_GSM_GPRS_USE_GAN_N201_SIZE */
  }

};

/* ....................................................................... */

LOCAL const uint16 gllc_min_N201_U_octets[ GLLC_LL_NUM_SAPIS ] =
{
  400,                    /* GLLC_LL_SAPI_1_GMM   */
  140,                    /* GLLC_LL_SAPI_3_LL3   */
  140,                    /* GLLC_LL_SAPI_5_LL5   */
  270,                    /* GLLC_LL_SAPI_7_SMS   */
  140,                    /* GLLC_LL_SAPI_9_LL9   */
  140                     /* GLLC_LL_SAPI_11_LL11 */
};

/* ....................................................................... */

const uint16 gllc_max_N201_U_octets[ GLLC_LL_NUM_SAPIS ] =
{
  1520,                   /* GLLC_LL_SAPI_1_GMM   */
  1520,                   /* GLLC_LL_SAPI_3_LL3   */
  1520,                   /* GLLC_LL_SAPI_5_LL5   */
  1520,                   /* GLLC_LL_SAPI_7_SMS   */
  1520,                   /* GLLC_LL_SAPI_9_LL9   */
  1520                    /* GLLC_LL_SAPI_11_LL11 */
};

/* ------------------------------------------------------------------------ */

/* N201_I confirmed information frame maximum info field lengths in number of
   octets. */

const uint16 gllc_dflt_N201_I_octets[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
  1503,                   /* GLLC_LL_SAPI_3_LL3   */
  1503,                   /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
  1503,                   /* GLLC_LL_SAPI_9_LL9   */
  1503                    /* GLLC_LL_SAPI_11_LL11 */
};

/* ....................................................................... */

const uint16 gllc_prfd_N201_I_octets[ GLLC_LLME_NUM_GAN_STATES ][ GLLC_LL_NUM_SAPIS ] =
{
  /* Non-GAN State Preferences */
  {
    GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
    1520,                   /* GLLC_LL_SAPI_3_LL3   */
    1520,                   /* GLLC_LL_SAPI_5_LL5   */
    GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
    1520,                   /* GLLC_LL_SAPI_9_LL9   */
    1520                    /* GLLC_LL_SAPI_11_LL11 */
  },

  /* GAN State Preferences */
  {
    GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
#ifdef DEBUG_GSM_GPRS_USE_GAN_N201_SIZE
    #error code not present
#else
    1520,                   /* GLLC_LL_SAPI_3_LL3   */
    1520,                   /* GLLC_LL_SAPI_5_LL5   */
#endif /* DEBUG_GSM_GPRS_USE_GAN_N201_SIZE */
    GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
#ifdef DEBUG_GSM_GPRS_USE_GAN_N201_SIZE
    #error code not present
#else
    1520,                   /* GLLC_LL_SAPI_9_LL9   */
    1520                    /* GLLC_LL_SAPI_11_LL11 */
#endif /* DEBUG_GSM_GPRS_USE_GAN_N201_SIZE */
  }
};

/* ------------------------------------------------------------------------ */

/* mD resequencing buffer window size limits in multiples of 16 octets. */

LOCAL const uint16 gllc_dflt_mD_octet16[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
  1520,                   /* GLLC_LL_SAPI_3_LL3   */
  760,                    /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
  380,                    /* GLLC_LL_SAPI_9_LL9   */
  190                     /* GLLC_LL_SAPI_11_LL11 */
};

/* ....................................................................... */

#define GLLC_MAX_ACCEPTABLE_MD_OCTET16                                      \
        gllc_dflt_mD_octet16[GLLC_LL_SAPI_3_LL3]

/* ....................................................................... */

const uint16 gllc_prfd_mD_octet16[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
  1520,                   /* GLLC_LL_SAPI_3_LL3   */
  1520,                   /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
  1520,                   /* GLLC_LL_SAPI_9_LL9   */
  1520                    /* GLLC_LL_SAPI_11_LL11 */
};

/* ------------------------------------------------------------------------ */

/* mU retransmission buffer window size limits in multiples of 16 octets. */

LOCAL const uint16 gllc_dflt_mU_octet16[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
  1520,                   /* GLLC_LL_SAPI_3_LL3   */
  760,                    /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
  380,                    /* GLLC_LL_SAPI_9_LL9   */
  190                     /* GLLC_LL_SAPI_11_LL11 */
};

/* ....................................................................... */

#define GLLC_MAX_ACCEPTABLE_MU_OCTET16                                      \
        gllc_dflt_mU_octet16[GLLC_LL_SAPI_3_LL3]

/* ....................................................................... */

const uint16 gllc_prfd_mU_octet16[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
  1520,                   /* GLLC_LL_SAPI_3_LL3   */
  1520,                   /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
  1520,                   /* GLLC_LL_SAPI_9_LL9   */
  1520                    /* GLLC_LL_SAPI_11_LL11 */
};

/* ------------------------------------------------------------------------ */

/* kD resequencing buffer window size limits in frames. */

LOCAL const uint8 gllc_dflt_kD_frames[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
  16,                     /* GLLC_LL_SAPI_3_LL3   */
  8,                      /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
  4,                      /* GLLC_LL_SAPI_9_LL9   */
  2                       /* GLLC_LL_SAPI_11_LL11 */
};

/* ....................................................................... */

#define GLLC_MAX_ACCEPTABLE_KD_FRAMES                                       \
        gllc_dflt_kD_frames[GLLC_LL_SAPI_3_LL3]

/* ....................................................................... */

const uint8 gllc_prfd_kD_frames[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
  16,                     /* GLLC_LL_SAPI_3_LL3   */
  16,                     /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
  16,                     /* GLLC_LL_SAPI_9_LL9   */
  16                      /* GLLC_LL_SAPI_11_LL11 */
};

/* ------------------------------------------------------------------------ */

/* kU retransmission buffer window size limits in frames. */

LOCAL const uint8 gllc_dflt_kU_frames[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
  16,                     /* GLLC_LL_SAPI_3_LL3   */
  8,                      /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
  4,                      /* GLLC_LL_SAPI_9_LL9   */
  2                       /* GLLC_LL_SAPI_11_LL11 */
};

/* ....................................................................... */

#define GLLC_MAX_ACCEPTABLE_KU_FRAMES                                       \
        gllc_dflt_kU_frames[GLLC_LL_SAPI_3_LL3]

/* ....................................................................... */

const uint8 gllc_prfd_kU_frames[ GLLC_LL_NUM_SAPIS ] =
{
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_1_GMM   */
  16,                     /* GLLC_LL_SAPI_3_LL3   */
  16,                     /* GLLC_LL_SAPI_5_LL5   */
  GLLC_SAPI_NOT_SUPPORTED,/* GLLC_LL_SAPI_7_SMS   */
  16,                     /* GLLC_LL_SAPI_9_LL9   */
  16                      /* GLLC_LL_SAPI_11_LL11 */
};


/* ------------------------------------------------------------------------ */
/*  State of GAN Mode at last MO XID Negotiation for the LLE	            */
/* ------------------------------------------------------------------------ */

LOCAL gllc_llme_gan_state_t gllc_lle_last_gan_state[NUM_GERAN_DATA_SPACES][GLLC_LL_NUM_SAPIS] =
                            { { GLLC_LLME_GAN_INACTIVE } };


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GLLC_LLME_RESET_L2_XID_PARAMETERS() 
===
===  DESCRIPTION
===
===   Resets all LLME XID negotiable parameters to their default values.
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

void gllc_llme_reset_l2_xid_parameters( gas_id_t gas_id )
{
  gllc_llme[gas_id].llc_version = GLLC_DFLT_VERSION;

  gllc_diag_rpt_llme_state( gas_id );

} /* end of gllc_llme_reset_l2_xid_parameters() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLME_RESET_IOV_XID_PARAMETERS() 
===
===  DESCRIPTION
===
===   Resets all XID Input Offset IOV negotiable parameters to their
===   default values.
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

void gllc_llme_reset_IOV_xid_parameters( gas_id_t gas_id )
{
  uint32 lle;
  
  MSG_GERAN_MED_2_G( "GLC ME: Info IOV-UI/IOV-I Default Reset", 0 ,0 );
  
  gllc_llme[gas_id].input_offset_IOV_UI = GLLC_DFLT_IOV_UI;
  gllc_diag_rpt_llme_state( gas_id );

  for ( lle = 0; lle <= (uint32)GLLC_LL_LAST_SAPI; lle++ )
  {
    gllc_lle[gas_id][lle].ack.input_offset_IOV_I = gllc_dflt_IOV_I[lle];
    gllc_diag_rpt_lle_xid( gas_id, (gllc_ll_sapi_t)lle );
  }

} /* end of gllc_llme_reset_IOV_xid_parameters() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_RESET_L2_XID_PARAMETERS() 
===
===  DESCRIPTION
===
===   Resets all XID negotiable parameters relating to the given LLE to
===   their default values. If the default value of a parameter differs from
===   the preferred value, then a pending XID negotiation requirement is 
===   flagged on this LLE for the associated parameter. An automatic XID
===   negotiation exchange will result when opportunity permits.
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

void gllc_lle_reset_l2_xid_parameters( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  (void)memset( &gllc_lle[gas_id][lle].unnum.xid_req, 0, sizeof(gllc_xid_info_t) );
  (void)memset( &gllc_lle[gas_id][lle].unnum.xid_neg, 0, sizeof(gllc_xid_info_t) );
  gllc_lle[gas_id][lle].unnum.xid_pending = 0;

  /* --------------------------------------------------------------------- */

  gllc_lle[gas_id][lle].ack.input_offset_IOV_I = gllc_dflt_IOV_I[lle];

  /* --------------------------------------------------------------------- */

  gllc_lle[gas_id][lle].gen.retrx_interval_T200 = gllc_dflt_T200_ms[lle];
  gllc_lle[gas_id][lle].ack.retrx_interval_T201 = gllc_dflt_T200_ms[lle];

  if ( gllc_dflt_T200_ms[lle] != gllc_prfd_T200_ms[lle] )
    gllc_lle[gas_id][lle].unnum.xid_pending |= GLLC_T200_XID;  

  /* --------------------------------------------------------------------- */

  gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200 = gllc_dflt_N200[lle];

  if ( gllc_dflt_N200[lle] != gllc_prfd_N200[lle] )
    gllc_lle[gas_id][lle].unnum.xid_pending |= GLLC_N200_XID;  

  /* --------------------------------------------------------------------- */

  gllc_lle[gas_id][lle].unack.max_info_octets_N201_U = gllc_dflt_N201_U_octets[lle];

  if ( gllc_dflt_N201_U_octets[lle] != 
         gllc_prfd_N201_U_octets[gllc_llme[gas_id].gan_state][lle] )
    gllc_lle[gas_id][lle].unnum.xid_pending |= GLLC_N201_U_XID;  

  /* --------------------------------------------------------------------- */

  gllc_lle[gas_id][lle].ack.max_info_octets_N201_I = gllc_dflt_N201_I_octets[lle];

  if ( gllc_dflt_N201_I_octets[lle] != 
         gllc_prfd_N201_I_octets[gllc_llme[gas_id].gan_state][lle] )
    gllc_lle[gas_id][lle].unnum.xid_pending |= GLLC_N201_I_XID;  

  /* --------------------------------------------------------------------- */

  gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD =
    (uint16)(gllc_dflt_mD_octet16[lle] * 16);

  if ( gllc_dflt_mD_octet16[lle] != gllc_prfd_mD_octet16[lle] )
    gllc_lle[gas_id][lle].unnum.xid_pending |= GLLC_mD_XID;  

  /* --------------------------------------------------------------------- */

  gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU =
    (uint16)(gllc_dflt_mU_octet16[lle] * 16);

  if ( gllc_dflt_mU_octet16[lle] != gllc_prfd_mU_octet16[lle] )
    gllc_lle[gas_id][lle].unnum.xid_pending |= GLLC_mU_XID;  

  /* --------------------------------------------------------------------- */

  gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD = gllc_dflt_kD_frames[lle];

  if ( gllc_dflt_kD_frames[lle] != gllc_prfd_kD_frames[lle] )
    gllc_lle[gas_id][lle].unnum.xid_pending |= GLLC_kD_XID;  

  /* --------------------------------------------------------------------- */

  gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU = gllc_dflt_kU_frames[lle];

  if ( gllc_dflt_kU_frames[lle] != gllc_prfd_kU_frames[lle] )
    gllc_lle[gas_id][lle].unnum.xid_pending |= GLLC_kU_XID;  

  /* --------------------------------------------------------------------- */

  gllc_diag_rpt_lle_xid( gas_id, lle );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_reset_l2_xid_parameters() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_VALIDATE_U_FORMAT_CR_XID_REQ() 
===
===  DESCRIPTION
===
===   Validates the XID Negotiation Request content of a received U format
===   peer command, and returns TRUE if valid in the current context.
===
===   The LLC specification requires that XID parameters are not range
===   validated here. An out of range parameter does not warrant XID command
===   rejection according to the LLC specification.
===   
===   Range validation must be performed at the negotiated response
===   generation stage, whereupon this LLE's preferences can be substituted.
===
===   NOTE: If the XID Negotiation Request is valid, then it left to the
===   caller to store in the LLE's unnumbered information store for later
===   reference during the generation of an XID Negotiation Response and
===   setting of parameters.  
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   TRUE if validated, FALSE otherwise.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

boolean gllc_lle_validate_u_format_cr_xid_req
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame
)
{
  register gllc_xid_info_t *xid_req;
           boolean          xid_req_valid = TRUE;

  /* --------------------------------------------------------------------- */

  if ( u_frame -> info_field.xid_info.errors )
    xid_req_valid = FALSE;

  /* --------------------------------------------------------------------- */

  else if ( u_frame -> xid == GLLC_XID_REQ )
  {
    xid_req = &u_frame -> info_field.xid_info;

    /* ................................................................... */

    /* Version XID cannot be negotiated in ABM, unless accompanied by Reset
       XID which indicates ABM will be exited, and unless accompanying a
       received SABM which implies that the peer is not in ABM state. */

    if ( xid_req -> contents & GLLC_VERSION_XID )
      if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
        if ( !( xid_req -> contents & GLLC_RESET_XID ) )
          if ( u_frame -> cmd_res_M4_M1 == GLLC_XID )
            xid_req_valid = FALSE;

    /* ................................................................... */

    /* IOV_UI XID cannot be negotiated in ABM, unless accompanied by Reset
       XID which indicates ABM will be exited, and unless accompanying a
       received SABM which implies that the peer is not in ABM state. */

    if ( xid_req -> contents & GLLC_IOV_UI_XID )
      if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
        if ( !( xid_req -> contents & GLLC_RESET_XID ) )
          if ( u_frame -> cmd_res_M4_M1 == GLLC_XID )
            xid_req_valid = FALSE;

    /* ................................................................... */

    /* IOV_I XID cannot accompany an XID command. */

    if ( xid_req -> contents & GLLC_IOV_I_XID )
    {
      if ( !GLLC_LLE_IS_3_5_9_11(lle) )
        xid_req_valid = FALSE;
      else if ( u_frame -> cmd_res_M4_M1 == GLLC_XID )
        xid_req_valid = FALSE;
    }

    /* ................................................................... */

    /* Reset XID cannot accompany a SABM command. */

    if ( xid_req -> contents & GLLC_RESET_XID )
      if ( u_frame -> cmd_res_M4_M1 == GLLC_SABM )
        xid_req_valid = FALSE;

    /* ................................................................... */

    /* Layer 3 and layer 2 acknowledged mode parameters for LL SAPIs other
       than 3,5,9 and 11 are invalid. */
    
    if
    (
      xid_req -> contents
      &
      (
        GLLC_LAY3_XID | GLLC_N201_I_XID | GLLC_mD_XID |
        GLLC_mU_XID   | GLLC_kD_XID     | GLLC_kU_XID
      )
    )
    {
      if ( !GLLC_LLE_IS_3_5_9_11(lle) )
        xid_req_valid = FALSE;
    }

    /* ................................................................... */
  }

  /* --------------------------------------------------------------------- */

  else if ( u_frame -> xid == GLLC_XID_NEG )
    xid_req_valid = FALSE;

  /* --------------------------------------------------------------------- */

  return ( xid_req_valid );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_validate_u_format_cr_xid_req() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_VALIDATE_U_FORMAT_CR_XID_NEG() 
===
===  DESCRIPTION
===
===   Validates the XID Negotiation Response content of a received U format
===   peer response, and returns TRUE if valid in the current context.
===
===   If the XID Negotiation Response is valid then it is stored in the
===   LLE's unnumbered information store for later reference during
===   setting of parameters.
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   TRUE if validated, FALSE otherwise.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

boolean gllc_lle_validate_u_format_cr_xid_neg
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame
)
{
  register gllc_xid_info_t *xid_neg;
           boolean          xid_neg_valid = TRUE;

  /* --------------------------------------------------------------------- */

  if ( u_frame -> info_field.xid_info.errors )
    xid_neg_valid = FALSE;

  /* --------------------------------------------------------------------- */

  else if ( u_frame -> xid == GLLC_XID_NEG )
  {
    xid_neg = &u_frame -> info_field.xid_info;

    /* ................................................................... */

    if ( xid_neg -> contents & GLLC_VERSION_XID )
    {
      /* Version XID cannot be negotiated in ABM, so is the negotiated
         version different from current version? */

      if ( (gllc_lle[gas_id][lle].state    == GLLC_LLE_ABM         ) &&
           (xid_neg -> version_xid != gllc_llme[gas_id].llc_version)    )
      {
        xid_neg_valid = FALSE;
      }

      /* Is Version in valid range? */

#if ( GLLC_MIN_VERSION == 0 ) /* Avoid compiler warning when 0. */
      if ( xid_neg -> version_xid > GLLC_MAX_VERSION )
      {
        xid_neg_valid = FALSE;
      }
#else
      if ( (xid_neg -> version_xid < GLLC_MIN_VERSION) ||
           (xid_neg -> version_xid > GLLC_MAX_VERSION)    )
      {
        xid_neg_valid = FALSE;
      }
#endif

      /* If Version was included in the original request, then is the sense
         of negotiation DOWN with respect to the proposed value? */

      if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_VERSION_XID) &&
           (xid_neg -> version_xid >
            gllc_lle[gas_id][lle].unnum.xid_req.version_xid                )    )
      {
        xid_neg_valid = FALSE;
      }

      /* If Version was not included in the original request, then is the
         sense of negotiation DOWN with respect to the current value? */
      
      if (!(gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_VERSION_XID) &&
           (xid_neg -> version_xid > gllc_llme[gas_id].llc_version         )    )
      {
        xid_neg_valid = FALSE;
      }
    }

    /* ................................................................... */

    /* IOV_UI XID cannot be negotiated in ABM. */

    if ( xid_neg -> contents & GLLC_IOV_UI_XID )
      if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
        xid_neg_valid = FALSE;

    /* ................................................................... */

    /* IOV_I XID cannot accompany an XID response. */

    if ( xid_neg -> contents & GLLC_IOV_I_XID )
      if ( u_frame -> cmd_res_M4_M1 == GLLC_XID )
        xid_neg_valid = FALSE;

    /* ................................................................... */

    if ( xid_neg -> contents & GLLC_T200_XID )
    {
      /* Is T200 in valid range? */

      if ( (xid_neg -> t200_xid < (GLLC_MIN_T200_MS / 100)) ||
           (xid_neg -> t200_xid > (GLLC_MAX_T200_MS / 100))    )
      {
        xid_neg_valid = FALSE;
      }

      /* If T200 was included in the original request, then is the sense
         of negotiation UP with respect to the proposed value? */

      if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_T200_XID      ) &&
           (xid_neg -> t200_xid < gllc_lle[gas_id][lle].unnum.xid_req.t200_xid)    )
      {
        xid_neg_valid = FALSE;
      }

      /* If T200 was not included in the original request, then is the
         sense of negotiation UP with respect to the current value? */

      if (!(gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_T200_XID) &&
           (xid_neg -> t200_xid <
            gllc_lle[gas_id][lle].gen.retrx_interval_T200 / 100         )    )
      {
        xid_neg_valid = FALSE;
      }
    }

    /* ................................................................... */

    if ( xid_neg -> contents & GLLC_N200_XID )
    {
      /* Is N200 in valid range? */

      if ( (xid_neg -> n200_xid < GLLC_MIN_N200) ||
           (xid_neg -> n200_xid > GLLC_MAX_N200)    )
      {
        xid_neg_valid = FALSE;
      }

      /* If N200 was included in the original request, then is the sense
         of negotiation UP with respect to the proposed value? */

      if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N200_XID      ) &&
           (xid_neg -> n200_xid < gllc_lle[gas_id][lle].unnum.xid_req.n200_xid)    )
      {
        xid_neg_valid = FALSE;
      }

      /* If N200 was not included in the original request, then is the sense
         of negotiation UP with respect to the current value? */

      if (!(gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N200_XID      ) &&
           (xid_neg -> n200_xid < gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200)    )
      {
        xid_neg_valid = FALSE;
      }
    }

    /* ................................................................... */

    if ( xid_neg -> contents & GLLC_N201_U_XID )
    {
      /* Is N201-U in valid range? */

      if ( (xid_neg -> n201_u_xid < gllc_min_N201_U_octets[lle]) ||
           (xid_neg -> n201_u_xid > gllc_max_N201_U_octets[lle])    )
      {
        xid_neg_valid = FALSE;
      }

      /* If N201-U was included in the original request, then is the sense
         of negotiation DOWN with respect to the proposed value? */

      if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N201_U_XID) &&
           (xid_neg -> n201_u_xid >
            gllc_lle[gas_id][lle].unnum.xid_req.n201_u_xid                )    )
      {
        xid_neg_valid = FALSE;
      }

      /* If N201-U was not included in the original request, then is the
         sense of negotiation DOWN with respect to the current value? */

      if (!(gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N201_U_XID) &&
           (xid_neg -> n201_u_xid >
            gllc_lle[gas_id][lle].unack.max_info_octets_N201_U            )    )
      {
        xid_neg_valid = FALSE;
      }
    }

    /* ................................................................... */

    if ( xid_neg -> contents & GLLC_N201_I_XID )
    {
      /* Is N201-I in valid range? */

      if ( (xid_neg -> n201_i_xid < GLLC_MIN_N201_I) ||
           (xid_neg -> n201_i_xid > GLLC_MAX_N201_I)    )
      {
        xid_neg_valid = FALSE;
      }

      /* Has N201-I been negotiated to a higher value in state ABM? */

      if ( (gllc_lle[gas_id][lle].state == GLLC_LLE_ABM    ) &&
           (xid_neg -> n201_i_xid <
           gllc_lle[gas_id][lle].ack.max_info_octets_N201_I)    )
      {
        xid_neg_valid = FALSE;
      }

      /* If N201-I was included in the original request, then is the sense
         of negotiation DOWN with respect to the proposed value? */

      if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N201_I_XID) && 
           (xid_neg -> n201_i_xid >
            gllc_lle[gas_id][lle].unnum.xid_req.n201_i_xid                )    )
      {
        xid_neg_valid = FALSE;
      }

      /* If N201-I was not included in the original request, then is the
         sense of negotiation DOWN with respect to the current value? */

      if (!(gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N201_I_XID) && 
           (xid_neg -> n201_i_xid >
            gllc_lle[gas_id][lle].ack.max_info_octets_N201_I              )    )
      {
        xid_neg_valid = FALSE;
      }
    }

    /* ................................................................... */

    if ( xid_neg -> contents & GLLC_mD_XID )
    {
      /* Validate against limits and sense of negotiation. */
      
      /* Is mD in valid range? */
        
      if
      (
        (xid_neg -> md_xid > GLLC_MAX_MD_OCTET16)
        ||
        (
          (xid_neg -> md_xid  < GLLC_MIN_MD_OCTET16) &&
          (xid_neg -> md_xid != 0                  )
        )
      )
      {
        xid_neg_valid = FALSE;
      }

      /* Has mD been negotiated to a higher value in state ABM? */
       
      if ( (gllc_lle[gas_id][lle].state == GLLC_LLE_ABM           ) &&
           (xid_neg -> md_xid <
            gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD / 16)    )
      {
        xid_neg_valid = FALSE;
      }

      /* If mD was included in the original request, then is the sense of
         negotiation DOWN with respect to the proposed value? */

      if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_mD_XID    ) &&
           (xid_neg -> md_xid > gllc_lle[gas_id][lle].unnum.xid_req.md_xid)    )
      {
        xid_neg_valid = FALSE;
      }
        
      /* If mD was not included in the original request, then is the
         sense of negotiation DOWN with respect to the current value? */

      if (!(gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_mD_XID) &&
           (xid_neg -> md_xid >
            gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD / 16    )    )
      {
        xid_neg_valid = FALSE;
      }

      if ( xid_neg -> md_xid != 0 )
      {      
        /* Can mD accomodate at least one I frame of size equal to the
           negotiated N201-I? */

        if ( (xid_neg -> contents & GLLC_N201_I_XID           ) &&
             (xid_neg -> md_xid < (xid_neg -> n201_i_xid / 16))    )
        {
          xid_neg_valid = FALSE;
        }

        /* Can mD accomodate at least one I frame of size equal to the
           current N201-I? */

        if (!(xid_neg -> contents & GLLC_N201_I_XID        ) &&
             (xid_neg -> md_xid <
              gllc_lle[gas_id][lle].ack.max_info_octets_N201_I / 16)    )
        {
          xid_neg_valid = FALSE;
        }
      }
    }

    /* ................................................................... */

    if ( xid_neg -> contents & GLLC_mU_XID )
    {
      /* Validate against limits and sense of negotiation. */
      
      /* Is mU in valid range? */
        
      if
      (
        (xid_neg -> mu_xid > GLLC_MAX_MU_OCTET16)
        ||
        (
          (xid_neg -> mu_xid  < GLLC_MIN_MU_OCTET16) &&
          (xid_neg -> mu_xid != 0                  )
        )
      )
      {
        xid_neg_valid = FALSE;
      }

      /* Has mU been negotiated to a higher value in state ABM? */
       
      if ( (gllc_lle[gas_id][lle].state == GLLC_LLE_ABM           ) &&
           (xid_neg -> mu_xid <
            gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU / 16)    )
      {
        xid_neg_valid = FALSE;
      }

      /* If mU was included in the original request, then is the sense of
         negotiation DOWN with respect to the proposed value? */

      if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_mU_XID    ) &&
           (xid_neg -> mu_xid > gllc_lle[gas_id][lle].unnum.xid_req.mu_xid)    )
      {
        xid_neg_valid = FALSE;
      }
      
      /* If mU was not included in the original request, then is the
         sense of negotiation DOWN with respect to the current value? */

      if (!(gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_mU_XID) &&
           (xid_neg -> mu_xid >
            gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU / 16    )    )
      {
        xid_neg_valid = FALSE;
      }
      
      if ( xid_neg -> mu_xid != 0 )
      {
        /* Can mU accomodate at least one I frame of size equal to the
           negotiated N201-I? */

        if ( (xid_neg -> contents & GLLC_N201_I_XID           ) &&
             (xid_neg -> mu_xid < (xid_neg -> n201_i_xid / 16))    )
        {
          xid_neg_valid = FALSE;
        }

        /* Can mU accomodate at least one I frame of size equal to the
           current N201-I? */

        if (!(xid_neg -> contents & GLLC_N201_I_XID        ) &&
             (xid_neg -> mu_xid <
              gllc_lle[gas_id][lle].ack.max_info_octets_N201_I / 16)    )
        {
          xid_neg_valid = FALSE;
        }
      }
    }

    /* ................................................................... */

    if ( xid_neg -> contents & GLLC_kD_XID )
    {
      /* Is kD in valid range? Code with Lint in mind. */

#if ( GLLC_MAX_KD_FRAMES < 255 )
      if ( (xid_neg -> kd_xid < GLLC_MIN_KD_FRAMES) ||
           (xid_neg -> kd_xid > GLLC_MAX_KD_FRAMES)    )
#else
      if ( xid_neg -> kd_xid < GLLC_MIN_KD_FRAMES )
#endif
      {
        xid_neg_valid = FALSE;
      }
      
      /* Has kD been negotiated to a higher value in state ABM? */
       
      if ( (gllc_lle[gas_id][lle].state == GLLC_LLE_ABM      ) &&
           (xid_neg -> kd_xid <
            gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD)    )
      {
        xid_neg_valid = FALSE;
      }

      /* If kD was included in the original request, then is the sense of
         negotiation DOWN with respect to the proposed value? */

      if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_kD_XID    ) &&
           (xid_neg -> kd_xid > gllc_lle[gas_id][lle].unnum.xid_req.kd_xid)    )
      {
        xid_neg_valid = FALSE;
      }

      /* If kD was not included in the original request, then is the sense
         of negotiation DOWN with respect to the current value? */

      if (!(gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_kD_XID) &&
           (xid_neg -> kd_xid >
            gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD         )    )
      {
        xid_neg_valid = FALSE;
      }
    }

    /* ................................................................... */

    if ( xid_neg -> contents & GLLC_kU_XID )
    {
      /* Is kU in valid range? Code with Lint in mind. */

#if ( GLLC_MAX_KU_FRAMES < 255 )
      if ( (xid_neg -> ku_xid < GLLC_MIN_KU_FRAMES) ||
           (xid_neg -> ku_xid > GLLC_MAX_KU_FRAMES)    )
#else
      if ( xid_neg -> ku_xid < GLLC_MIN_KU_FRAMES )
#endif
      {
         xid_neg_valid = FALSE;
      }
      /* Has kU been negotiated to a higher value in state ABM? */
       
      if ( (gllc_lle[gas_id][lle].state == GLLC_LLE_ABM      ) &&
           (xid_neg -> ku_xid <
            gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU)    )
      {
        xid_neg_valid = FALSE;
      }

      /* If kU was included in the original request, then is the sense of
         negotiation DOWN with respect to the proposed value? */

      if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_kU_XID    ) &&
           (xid_neg -> ku_xid > gllc_lle[gas_id][lle].unnum.xid_req.ku_xid)    )
      {
        xid_neg_valid = FALSE;
      }

      /* If kU was not included in the original request, then is the sense
         of negotiation DOWN with respect to the current value? */

      if (!(gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_kU_XID) &&
           (xid_neg -> ku_xid >
            gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU         )    )
      {
        xid_neg_valid = FALSE;
      }
    }

    /* ................................................................... */

    /* Reset XID cannot be sent in a negotiated response. */

    if ( xid_neg -> contents & GLLC_RESET_XID )
    {
      if ( (u_frame -> cmd_res_M4_M1 == GLLC_UA ) ||
           (u_frame -> cmd_res_M4_M1 == GLLC_XID)    )
      {
        xid_neg_valid = FALSE;
      }
    }

    /* ................................................................... */

    /* Layer 3 and layer 2 acknowledged mode parameters for LL SAPIs other
       than 3,5,9 and 11 are invalid. */
    
    if
    (
      xid_neg -> contents
      &
      (
        GLLC_LAY3_XID | GLLC_N201_I_XID | GLLC_mD_XID |
        GLLC_mU_XID   | GLLC_kD_XID     | GLLC_kU_XID
      )
    )
    {
      if ( !GLLC_LLE_IS_3_5_9_11(lle) )
        xid_neg_valid = FALSE;
    }

    /* ................................................................... */
  }

  /* --------------------------------------------------------------------- */

  else if ( u_frame -> xid == GLLC_XID_REQ )
    xid_neg_valid = FALSE;

  /* --------------------------------------------------------------------- */

  return ( xid_neg_valid );

  /* --------------------------------------------------------------------- */

} /* end of gllc_lle_validate_u_format_cr_xid_neg() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_STORE_U_FORMAT_CR_XID_REQ() 
===
===  DESCRIPTION
===
===   Stores the layer 2 XID parameter content contained in the U frame's
===   XID negotiation request, to the LLE's unnumbered information store.
===
===   This permits subsequent XID processing relating to the generation of
===   a negotiated response, and the setting of negotiated parameters.
===
===  DEPENDENCIES
===
===   It is assumed that the U frame XID negotiation request has been
===   pre-validated.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   If the U frame does not contain an XID negotiation request, then
===   the LLE's unnumbered XID negotiation response information element is
===   cleared.
===  
===========================================================================*/

void gllc_lle_store_u_format_cr_xid_req
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame
)
{
  if ( u_frame -> xid == GLLC_XID_REQ )
  {
    gllc_lle[gas_id][lle].unnum.xid_req = u_frame -> info_field.xid_info;

    /* Strip out any layer 3 related information. */

    gllc_lle[gas_id][lle].unnum.xid_req.contents      &= ~GLLC_LAY3_XID;
    gllc_lle[gas_id][lle].unnum.xid_req.l3_xid.length  = 0;
    gllc_lle[gas_id][lle].unnum.xid_req.l3_xid.params  = NULL;
  }
  else
    (void)memset( &gllc_lle[gas_id][lle].unnum.xid_req, 0, sizeof(gllc_xid_info_t) );

} /* end of gllc_lle_store_u_format_cr_xid_req() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_STORE_U_FORMAT_CR_XID_NEG() 
===
===  DESCRIPTION
===
===   Stores the layer 2 XID parameter content contained in the U frame's
===   XID negotiation response, to the LLE's unnumbered information store.
===
===   This permits subsequent XID processing relating to the setting of
===   negotiated parameters.
===
===  DEPENDENCIES
===
===   It is assumed that the U frame XID negotiation response has been
===   pre-validated.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   If the U frame does not contain an XID negotiation response, then
===   the LLE's unnumbered XID negotiation response information element is
===   cleared.
===  
===========================================================================*/

void gllc_lle_store_u_format_cr_xid_neg
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame
)
{
  if ( u_frame -> xid == GLLC_XID_NEG )
  {
    gllc_lle[gas_id][lle].unnum.xid_neg = u_frame -> info_field.xid_info;

    /* Strip out any layer 3 related information. */

    gllc_lle[gas_id][lle].unnum.xid_neg.contents      &= ~GLLC_LAY3_XID;
    gllc_lle[gas_id][lle].unnum.xid_neg.l3_xid.length  = 0;
    gllc_lle[gas_id][lle].unnum.xid_neg.l3_xid.params  = NULL;

    /* Combine the XID response content map with that which is currently
       pending, such that layer 2 parameters which are not current subjects
       of negotiation can be explicitly negotiated at the earliest
       opportunity. */

    gllc_lle[gas_id][lle].unnum.xid_pending |= gllc_lle[gas_id][lle].unnum.xid_neg.contents;
  }
  else
    (void)memset( &gllc_lle[gas_id][lle].unnum.xid_neg, 0, sizeof(gllc_xid_info_t) );

} /* end of gllc_lle_store_u_format_cr_xid_neg() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_GENERATE_L2_XID_REQ() 
===
===  DESCRIPTION
===
===   Examines the given LLE's pending XID negotiation requirement record to
===   see if any XID parameters can be negotiated in the current context.
===   If so, then an XID Request information element is generated in the
===   LLE's unnumbered frame information store, in anticipation of an
===   impending unnumbered frame transmission requirement which can embody
===   this element.
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   TRUE if an XID Request unnumbered information element has been
===   generated, FALSE otherwise.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

boolean gllc_lle_generate_l2_xid_req( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  /* --------------------------------------------------------------------- */

  /* Firstly clean up pending record. Remove stuff that isn't layer 2
     related or can't be negotiated in the up link direction. */
  
  gllc_lle[gas_id][lle].unnum.xid_pending &=
    ~( GLLC_LAY3_XID | GLLC_IOV_UI_XID | GLLC_IOV_I_XID | GLLC_RESET_XID );
  
  /* XID negotiation of layer 2 acknowledged mode parameters for LL SAPIs
     other than 3,5,9 and 11 are invalid. */

  if ( !GLLC_LLE_IS_3_5_9_11(lle) )
  {
    gllc_lle[gas_id][lle].unnum.xid_pending &=
     ~( GLLC_N201_I_XID|GLLC_mD_XID|GLLC_mU_XID|GLLC_kD_XID|GLLC_kU_XID );
  }

  /* Prepare XID Request unnumbered information element. */

  (void)memset( &gllc_lle[gas_id][lle].unnum.xid_req, 0, sizeof(gllc_xid_info_t) );

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_pending & GLLC_VERSION_XID )
  {
    /* Can't negotiate LLC Version in the ABM state. */

    if ( gllc_lle[gas_id][lle].state != GLLC_LLE_ABM )
    {
      gllc_lle[gas_id][lle].unnum.xid_req.contents |= GLLC_VERSION_XID;

      /* If current value is equal to the default value, then propose
         preferred value, else propose the last successfully negotiated
         value. */
     
      if ( gllc_llme[gas_id].llc_version == GLLC_DFLT_VERSION )
        gllc_lle[gas_id][lle].unnum.xid_req.version_xid = GLLC_PRFD_VERSION;
      else
        gllc_lle[gas_id][lle].unnum.xid_req.version_xid = gllc_llme[gas_id].llc_version;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_pending & GLLC_T200_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_req.contents |= GLLC_T200_XID;

    /* If current value is equal to the default value, then propose preferred
       value, else propose the last successfully negotiated value. */

    if ( gllc_lle[gas_id][lle].gen.retrx_interval_T200 == gllc_dflt_T200_ms[lle] )
      gllc_lle[gas_id][lle].unnum.xid_req.t200_xid =
        (uint16)(gllc_prfd_T200_ms[lle] / 100);
    else
      gllc_lle[gas_id][lle].unnum.xid_req.t200_xid =
        (uint16)(gllc_lle[gas_id][lle].gen.retrx_interval_T200 / 100);
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_pending & GLLC_N200_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_req.contents |= GLLC_N200_XID;

    /* If current value is equal to the default value, then propose preferred
       value, else propose the last successfully negotiated value. */

    if ( gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200 == gllc_dflt_N200[lle] )
      gllc_lle[gas_id][lle].unnum.xid_req.n200_xid = gllc_prfd_N200[lle];
    else
      gllc_lle[gas_id][lle].unnum.xid_req.n200_xid =
       (uint8)gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200;
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_pending & GLLC_N201_U_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_req.contents |= GLLC_N201_U_XID;

    /* If current value is equal to the default value, then propose preferred
       value, else if GAN state has changed since last negotiation then
       propose preferred value, else propose the last successfully 
       negotiated value. */

    if ( gllc_lle[gas_id][lle].unack.max_info_octets_N201_U ==
         gllc_dflt_N201_U_octets[lle]                  )
    {
      gllc_lle[gas_id][lle].unnum.xid_req.n201_u_xid = 
        gllc_prfd_N201_U_octets[gllc_llme[gas_id].gan_state][lle];
    }
    else if ( gllc_llme[gas_id].gan_state != gllc_lle_last_gan_state[gas_id][lle] )
    {
      gllc_lle[gas_id][lle].unnum.xid_req.n201_u_xid = 
        gllc_prfd_N201_U_octets[gllc_llme[gas_id].gan_state][lle];
    }
    else
      gllc_lle[gas_id][lle].unnum.xid_req.n201_u_xid =
        gllc_lle[gas_id][lle].unack.max_info_octets_N201_U;
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_pending & GLLC_N201_I_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_req.contents |= GLLC_N201_I_XID;

    /* If current value is equal to the default value, then propose preferred
       value, else if GAN state has changed since last negotiation then
       propose preferred value, else propose the last successfully 
       negotiated value. */
    
    if ( gllc_lle[gas_id][lle].ack.max_info_octets_N201_I ==
         gllc_dflt_N201_I_octets[lle]                )
    {
      gllc_lle[gas_id][lle].unnum.xid_req.n201_i_xid = 
        gllc_prfd_N201_I_octets[gllc_llme[gas_id].gan_state][lle];
    }
    else if ( gllc_llme[gas_id].gan_state != gllc_lle_last_gan_state[gas_id][lle] )
    {
      gllc_lle[gas_id][lle].unnum.xid_req.n201_i_xid = 
        gllc_prfd_N201_I_octets[gllc_llme[gas_id].gan_state][lle];
    }
    else
    {
      gllc_lle[gas_id][lle].unnum.xid_req.n201_i_xid =
        gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;
    }

    if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
    {
      /* In ABM, N201-I can only be negotiated to the same or higher value
         than previously used. If proposed value is less than current
         value then propose current value. */

      if ( gllc_lle[gas_id][lle].unnum.xid_req.n201_i_xid <
           gllc_lle[gas_id][lle].ack.max_info_octets_N201_I )
      {
        gllc_lle[gas_id][lle].unnum.xid_req.n201_i_xid =
          gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;
      }
    }
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_pending & GLLC_mD_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_req.contents |= GLLC_mD_XID;

    /* If current value is equal to the default value, then propose preferred
       value, else propose the last successfully negotiated value. */
    
    if ( (gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD / 16) ==
         gllc_dflt_mD_octet16[lle]                           )
    {
      gllc_lle[gas_id][lle].unnum.xid_req.md_xid = gllc_prfd_mD_octet16[lle];

      if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
      {
        /* In ABM, mD can only be negotiated to the same or higher value
           than previously used. If proposed value is less than current
           value then propose current value. */

        if ( gllc_lle[gas_id][lle].unnum.xid_req.md_xid <
             gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD / 16 )
        {
          gllc_lle[gas_id][lle].unnum.xid_req.md_xid =
            gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD / 16;
        }
      }
    }
    else
    {
      gllc_lle[gas_id][lle].unnum.xid_req.md_xid =
        gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD / 16;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_pending & GLLC_mU_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_req.contents |= GLLC_mU_XID;

    /* If current value is equal to the default value, then propose preferred
       value, else propose the last successfully negotiated value. */
    
    if ( (gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU / 16) ==
         gllc_dflt_mU_octet16[lle]                           )
    {
      gllc_lle[gas_id][lle].unnum.xid_req.mu_xid = gllc_prfd_mU_octet16[lle];

      if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
      {
        /* In ABM, mU can only be negotiated to the same or higher value
           than previously used. If proposed value is less than current
           value then propose current value. */

        if ( gllc_lle[gas_id][lle].unnum.xid_req.mu_xid <
             gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU / 16 )
        {
          gllc_lle[gas_id][lle].unnum.xid_req.mu_xid =
            gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU / 16;
        }
      }
    }
    else
    {
      gllc_lle[gas_id][lle].unnum.xid_req.mu_xid =
        gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU / 16;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_pending & GLLC_kD_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_req.contents |= GLLC_kD_XID;

    /* If current value is equal to the default value, then propose preferred
       value, else propose the last successfully negotiated value. */
    
    if ( gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD ==
         gllc_dflt_kD_frames[lle]                     )
    {      
      gllc_lle[gas_id][lle].unnum.xid_req.kd_xid = gllc_prfd_kD_frames[lle];

      if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
      {
        /* In ABM, kD can only be negotiated to the same or higher value
           than previously used. If proposed value is less than current
           value then propose current value. */

        if ( gllc_lle[gas_id][lle].unnum.xid_req.kd_xid <
             gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD )
        {
          gllc_lle[gas_id][lle].unnum.xid_req.kd_xid =
            gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD;
        }
      }
    }
    else
    {
      gllc_lle[gas_id][lle].unnum.xid_req.kd_xid =
        gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_pending & GLLC_kU_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_req.contents |= GLLC_kU_XID;

    /* If current value is equal to the default value, then propose preferred
       value, else propose the last successfully negotiated value. */
    
    if ( gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU ==
         gllc_dflt_kU_frames[lle]                     )
    {
      gllc_lle[gas_id][lle].unnum.xid_req.ku_xid = gllc_prfd_kU_frames[lle];

      if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
      {
        /* In ABM, kU can only be negotiated to the same or higher value
           than previously used. If proposed value is less than current
           value then propose current value. */

        if ( gllc_lle[gas_id][lle].unnum.xid_req.ku_xid <
             gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU )
        {
          gllc_lle[gas_id][lle].unnum.xid_req.ku_xid =
            gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU;
        }
      }
    }
    else
    {
      gllc_lle[gas_id][lle].unnum.xid_req.ku_xid =
        gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU;
    }
  }

  /* --------------------------------------------------------------------- */

  /* Update last gan state for later use. */

  gllc_lle_last_gan_state[gas_id][lle] = gllc_llme[gas_id].gan_state;

  return ( gllc_lle[gas_id][lle].unnum.xid_req.contents ? TRUE:FALSE );

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_generate_l2_xid_req() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_GENERATE_L2_XID_NEG() 
===
===  DESCRIPTION
===
===   Examines the stored XID negotiation request in the LLE's unnumbered
===   information store, to determine if an XID negotiation response can be
===   generated in the current context.
===
===   The XID negotiation response is generated as an unnumbered information
===   element in the LLE's unnumbered frame information store, in
===   anticipation of an impending unnumbered frame transmission requirement
===   which can embody this element.
===
===   The XID negotiation request parameters are further validated to ensure
===   that they are in range. Where invalid, appropriate preferences are
===   substituted.
===
===   Parameter GLLC_RESET_XID is not handled here, and is handled as a
===   special case elsewhere.
===
===  DEPENDENCIES
===
===   The XID negotiation request in the LLE's unnumbered information store
===   must have been pre-validated.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

void gllc_lle_generate_l2_xid_neg( gas_id_t gas_id, gllc_ll_sapi_t lle )
{
  /* --------------------------------------------------------------------- */

  /* Prepare XID Response unnumbered information element. */

  (void)memset( &gllc_lle[gas_id][lle].unnum.xid_neg, 0, sizeof(gllc_xid_info_t) );

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_VERSION_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_neg.contents |= GLLC_VERSION_XID;

    /* Is proposed Version Number in valid range? */

#if ( GLLC_MIN_VERSION == 0 ) /* Avoid compiler warning when 0. */
    if ( gllc_lle[gas_id][lle].unnum.xid_req.version_xid <= GLLC_MAX_VERSION )
#else
    if ( (gllc_lle[gas_id][lle].unnum.xid_req.version_xid >= GLLC_MIN_VERSION) &&
         (gllc_lle[gas_id][lle].unnum.xid_req.version_xid <= GLLC_MAX_VERSION)    )
#endif
    {
      /* Version negotiation sense is down. */

#if ( GLLC_PRFD_VERSION == 0 ) /* Avoid compiler warning when 0. */
      gllc_lle[gas_id][lle].unnum.xid_neg.version_xid = GLLC_PRFD_VERSION;
#else
      if ( gllc_lle[gas_id][lle].unnum.xid_req.version_xid >= GLLC_PRFD_VERSION )
        gllc_lle[gas_id][lle].unnum.xid_neg.version_xid = GLLC_PRFD_VERSION;
      else   
        gllc_lle[gas_id][lle].unnum.xid_neg.version_xid =
          gllc_lle[gas_id][lle].unnum.xid_req.version_xid;
#endif
    }
    else
    {
      /* Substitute current value as proposed value was out of range. */
      gllc_lle[gas_id][lle].unnum.xid_neg.version_xid = gllc_llme[gas_id].llc_version;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_IOV_UI_XID )
  {
    /* NOTE: Unnumbered transmission handling will not permit this parameter
             to be transmitted from the MS in the uplink direction, but it
             needs to be included here to allow the parameter to be set. */
    
    gllc_lle[gas_id][lle].unnum.xid_neg.contents  |= GLLC_IOV_UI_XID;
    gllc_lle[gas_id][lle].unnum.xid_neg.iov_ui_xid =
      gllc_lle[gas_id][lle].unnum.xid_req.iov_ui_xid;
  }
  
  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_IOV_I_XID )
  {
    /* NOTE: Unnumbered transmission handling will not permit this parameter
             to be transmitted from the MS in the uplink direction, but it
             needs to be included here to allow the parameter to be set. */
    
    gllc_lle[gas_id][lle].unnum.xid_neg.contents |= GLLC_IOV_I_XID;
    gllc_lle[gas_id][lle].unnum.xid_neg.iov_i_xid =
      gllc_lle[gas_id][lle].unnum.xid_req.iov_i_xid;
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_T200_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_neg.contents |= GLLC_T200_XID;

    /* Is proposed T200 in valid range? */

    if ( (gllc_lle[gas_id][lle].unnum.xid_req.t200_xid >= (GLLC_MIN_T200_MS / 100)) &&
         (gllc_lle[gas_id][lle].unnum.xid_req.t200_xid <= (GLLC_MAX_T200_MS / 100))    )
    {
      /* T200 negotiation sense is up. Negotiate up to the MS acceptable
         minimum if proposed value is below this. */

      if ( gllc_lle[gas_id][lle].unnum.xid_req.t200_xid <=
           GLLC_MIN_ACCEPTABLE_T200_MS / 100       )
      {
        gllc_lle[gas_id][lle].unnum.xid_neg.t200_xid =
          (uint16)(GLLC_MIN_ACCEPTABLE_T200_MS / 100);
      }
      else   
        gllc_lle[gas_id][lle].unnum.xid_neg.t200_xid =
          gllc_lle[gas_id][lle].unnum.xid_req.t200_xid;
    }
    else
    {
      /* Substitute current value as proposed value was out of range. */
      gllc_lle[gas_id][lle].unnum.xid_neg.t200_xid =
        (uint16)(gllc_lle[gas_id][lle].gen.retrx_interval_T200 / 100);
    }
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N200_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_neg.contents |= GLLC_N200_XID;

    /* Is proposed N200 in valid range? */

    if ( (gllc_lle[gas_id][lle].unnum.xid_req.n200_xid >= GLLC_MIN_N200) &&
         (gllc_lle[gas_id][lle].unnum.xid_req.n200_xid <= GLLC_MAX_N200)    )
    {
      /* N200 negotiation sense is up. Accept proposed value as there are
         no MS imposed limits. */

      gllc_lle[gas_id][lle].unnum.xid_neg.n200_xid = 
        gllc_lle[gas_id][lle].unnum.xid_req.n200_xid;
    }
    else
    {
      /* Substitute current value as proposed value was out of range. */
      gllc_lle[gas_id][lle].unnum.xid_neg.n200_xid =
        (uint8)gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N201_U_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_neg.contents |= GLLC_N201_U_XID;

    /* Is proposed N201-U in valid range? */

    if ( (gllc_lle[gas_id][lle].unnum.xid_req.n201_u_xid >= gllc_min_N201_U_octets[lle]) &&
         (gllc_lle[gas_id][lle].unnum.xid_req.n201_u_xid <= gllc_max_N201_U_octets[lle])    )
    {
      /* N201-U negotiation sense is down. Accept proposed value as there are
         no MS maximum requirements. */
  
      gllc_lle[gas_id][lle].unnum.xid_neg.n201_u_xid =
        gllc_lle[gas_id][lle].unnum.xid_req.n201_u_xid;
    }
    else
    {
      /* Substitute current value as proposed value was out of range. */
      gllc_lle[gas_id][lle].unnum.xid_neg.n201_u_xid =
        gllc_lle[gas_id][lle].unack.max_info_octets_N201_U;
    }
  }

  /* --------------------------------------------------------------------- */

  /* Any N201-I negotiated response must be generated before any mD or mU
     negotiated response can be generated. */ 

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N201_I_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_neg.contents |= GLLC_N201_I_XID;

    /* Is proposed N201-I in valid range? */

    if ( (gllc_lle[gas_id][lle].unnum.xid_req.n201_i_xid >= GLLC_MIN_N201_I) &&
         (gllc_lle[gas_id][lle].unnum.xid_req.n201_i_xid <= GLLC_MAX_N201_I)    )
    {
      /* N201-I negotiation sense is down. Accept proposed value as there are
         no MS imposed limits. */
  
      gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid =
        gllc_lle[gas_id][lle].unnum.xid_req.n201_i_xid;
    }
    else
    {
      /* Substitute current value as proposed value was out of range. */
      gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid =
        gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;
    }

    /* If the state is ABM then the negotiated value must be greater than or
       equal to the current value. If not then change to the current value. */
    
    if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
    {
      if ( gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid <
           gllc_lle[gas_id][lle].ack.max_info_octets_N201_I )
      {
        gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid =
          gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;
      }
    }    
  }

  /* --------------------------------------------------------------------- */

  /* Any kD negotiated response must be generated before any mD negotiated
     response can be generated. */ 

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_kD_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_neg.contents |= GLLC_kD_XID;

    /* Is kD in valid range? Code with Lint in mind. */

#if ( GLLC_MAX_KD_FRAMES < 255 )
    if ( (gllc_lle[gas_id][lle].unnum.xid_req.kd_xid >= GLLC_MIN_KD_FRAMES) &&
         (gllc_lle[gas_id][lle].unnum.xid_req.kd_xid <= GLLC_MAX_KD_FRAMES)    )
#else
    if ( gllc_lle[gas_id][lle].unnum.xid_req.kd_xid >= GLLC_MIN_KD_FRAMES )
#endif
    {
      /* kD negotiation sense is down. Negotiate down to the MS acceptable
         maximum if proposed value is above this. */
  
      if ( gllc_lle[gas_id][lle].unnum.xid_req.kd_xid >=
           GLLC_MAX_ACCEPTABLE_KD_FRAMES         )
      {
        gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid = GLLC_MAX_ACCEPTABLE_KD_FRAMES;
      }
      else   
        gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid =
          gllc_lle[gas_id][lle].unnum.xid_req.kd_xid;
    }
    else
    {
      /* Substitute preference as proposed value was out of range. */
      gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid =
        gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD;
    }
    
    /* If the state is ABM then the negotiated value must be greater than or
       equal to the current value. If not then change to the current value.*/
    
    if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
    {
      if ( gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid <
           gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD )
      {
        gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid =
          gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD;
      }
    }    
  }

  /* --------------------------------------------------------------------- */

  /* Ensure a kD negotiated response and an N201-I negotiated response have
     been generated first, if required. */

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_mD_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_neg.contents |= GLLC_mD_XID;

    /* ................................................................... */

    /* Is proposed mD in valid range? */

    if
    (
      (gllc_lle[gas_id][lle].unnum.xid_req.md_xid > GLLC_MAX_MD_OCTET16)
      ||
      (
        (gllc_lle[gas_id][lle].unnum.xid_req.md_xid  < GLLC_MIN_MD_OCTET16) &&
        (gllc_lle[gas_id][lle].unnum.xid_req.md_xid != 0                  )
      )
    )
    {
      gllc_lle[gas_id][lle].unnum.xid_neg.md_xid =
        gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD / 16;
    }
    
    /* ................................................................... */

    /* The value of mD cannot be reduced if the state is ABM. */

    else if
    (
      (gllc_lle[gas_id][lle].state == GLLC_LLE_ABM)
      &&
      (
        gllc_lle[gas_id][lle].unnum.xid_req.md_xid <
        (gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD / 16)
      )
    )
    {
      gllc_lle[gas_id][lle].unnum.xid_neg.md_xid =
        gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD / 16;
    }

    /* ................................................................... */

    /* Consider whether the peer LLE does not wish to keep a count of I
       frame octets in the downlink direction. */

    else if ( gllc_lle[gas_id][lle].unnum.xid_req.md_xid == 0 )
      gllc_lle[gas_id][lle].unnum.xid_neg.md_xid = 0;

    /* ................................................................... */

    /* Consider the case where this LLE does not wish to keep a count of I
       frame octets in the downlink direction. */

    else if ( gllc_prfd_mD_octet16[lle] == 0 )
    {
      uint16 n201_i;
      uint8  kD;

      /* The negotiated response mD value shall be set to zero, and N201-I
         and kD shall be set such that their product is less than or equal
         to the proposed value of mD. */

      gllc_lle[gas_id][lle].unnum.xid_neg.md_xid = 0;

      /* If the N201-I or kD parameters are currently up for negotiation,
         then refer to the negotiation response values, otherwise the
         current values. */

      if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_I_XID )
        n201_i = gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid;
      else
        n201_i = gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;

      if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_kD_XID )
        kD = gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid;
      else
        kD = gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD;

      /* Is the product of N201-I and kD greater than the requested
         (proposed) mD parameter? */ 
      
      if ( (n201_i * kD) > (gllc_lle[gas_id][lle].unnum.xid_req.md_xid * 16) )
      {
        /* It is so start by reducing N201-I. */

        n201_i = (uint16)(gllc_lle[gas_id][lle].unnum.xid_req.md_xid * 16) / kD;
        
        if ( n201_i >= GLLC_MIN_N201_I ) 
        {
          /* Value of N201-I is above the minimum. */
          
          gllc_lle[gas_id][lle].unnum.xid_neg.contents  |= GLLC_N201_I_XID;
          gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid = n201_i;
        }
        else
        {
          /* Reducing N201-I would take it below minimum, so set N201-I at
             minimum and reduce kD as well. */
          
          n201_i = GLLC_MIN_N201_I;
          kD     = (uint8)(gllc_lle[gas_id][lle].unnum.xid_req.md_xid * 16) / n201_i;
        
          gllc_lle[gas_id][lle].unnum.xid_neg.contents |=
            ( GLLC_N201_I_XID | GLLC_kD_XID );

          gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid = n201_i;
          gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid     = kD;
        }
      }
    }

    /* ................................................................... */

    /* Consider the case where this LLE wishes to keep a count of I frame
       octets in the downlink direction. */
    
    else
    {
      uint16 n201_i;

      /* mD negotiation sense is down. Negotiate down to the MS acceptable
         maximum if proposed value is above this. */

      if ( gllc_lle[gas_id][lle].unnum.xid_req.md_xid >=
           GLLC_MAX_ACCEPTABLE_MD_OCTET16        )
      {
        gllc_lle[gas_id][lle].unnum.xid_neg.md_xid = GLLC_MAX_ACCEPTABLE_MD_OCTET16;
      }
      else   
        gllc_lle[gas_id][lle].unnum.xid_neg.md_xid =
          gllc_lle[gas_id][lle].unnum.xid_req.md_xid;

      /* If the N201-I parameter is currently up for negotiation, then refer
         to the negotiation response values, otherwise the current values. */

      if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_I_XID )
        n201_i = gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid;
      else
        n201_i = gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;
      
      /* The value of mD shall be negotiated such that at least one N201-I
         sized I frame can be accomodated. */

      if ( (gllc_lle[gas_id][lle].unnum.xid_neg.md_xid * 16) < n201_i )
        gllc_lle[gas_id][lle].unnum.xid_neg.md_xid = (n201_i / 16) + 1;
    }
  }

  /* --------------------------------------------------------------------- */

  /* Any kU negotiated response must be generated before any mU negotiated
     response can be generated. */ 

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_kU_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_neg.contents |= GLLC_kU_XID;

    /* Is kU in valid range? Code with Lint in mind. */

#if ( GLLC_MAX_KU_FRAMES < 255 )
    if ( (gllc_lle[gas_id][lle].unnum.xid_req.ku_xid >= GLLC_MIN_KU_FRAMES) &&
         (gllc_lle[gas_id][lle].unnum.xid_req.ku_xid <= GLLC_MAX_KU_FRAMES)    )
#else
    if ( gllc_lle[gas_id][lle].unnum.xid_req.ku_xid >= GLLC_MIN_KU_FRAMES )
#endif
    {
      /* kU negotiation sense is down. Negotiate down to the MS acceptable
         maximum if proposed value is above this. */

      if ( gllc_lle[gas_id][lle].unnum.xid_req.ku_xid >=
           GLLC_MAX_ACCEPTABLE_KU_FRAMES )
      {
        gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid = GLLC_MAX_ACCEPTABLE_KU_FRAMES;
      }
      else   
        gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid =
          gllc_lle[gas_id][lle].unnum.xid_req.ku_xid;
    }
    else
    {
      /* Substitute current value as proposed value was out of range. */
      gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid =
        gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU;
    }
    
    /* If the state is ABM then the negotiated value must be greater than or
       equal to the current value. If not then change to the current value. */
    
    if ( gllc_lle[gas_id][lle].state == GLLC_LLE_ABM )
    {
      if ( gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid <
           gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU )
      {
        gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid =
          gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU;
      }
    }    
  }

  /* --------------------------------------------------------------------- */

  /* Ensure a kU negotiated response and an N201-I negotiated response have
     been generated first, if required. */

  if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_mU_XID )
  {
    gllc_lle[gas_id][lle].unnum.xid_neg.contents |= GLLC_mU_XID;

    /* ................................................................... */

    /* Is proposed mU in valid range? */

    if
    (
      (gllc_lle[gas_id][lle].unnum.xid_req.mu_xid > GLLC_MAX_MU_OCTET16)
      ||
      (
        (gllc_lle[gas_id][lle].unnum.xid_req.mu_xid  < GLLC_MIN_MU_OCTET16) &&
        (gllc_lle[gas_id][lle].unnum.xid_req.mu_xid != 0                  )
      )
    )
    {
      gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid =
        gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU / 16;
    }
    
    /* ................................................................... */

    /* The value of mU cannot be reduced if the state is ABM. */

    else if
    (
      (gllc_lle[gas_id][lle].state == GLLC_LLE_ABM)
      &&
      (
        gllc_lle[gas_id][lle].unnum.xid_req.mu_xid <
        (gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU / 16)
      )
    )
    {
      gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid =
        gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU / 16;
    }

    /* ................................................................... */

    /* Consider whether the peer LLE does not wish to keep a count of I
       frame octets in the uplink direction. */

    else if ( gllc_lle[gas_id][lle].unnum.xid_req.mu_xid == 0 )
      gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid = 0;

    /* ................................................................... */

    /* Consider the case where this LLE does not wish to keep a count of I
       frame octets in the uplink direction. */

    else if ( gllc_prfd_mU_octet16[lle] == 0 )
    {
      uint16 n201_i;
      uint8  kU;

      /* The negotiated response mU value shall be set to zero, and N201-I
         and kU shall be set such that their product is less than or equal
         to the proposed value of mU. */

      gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid = 0;

      /* If the N201-I or kU parameters are currently up for negotiation,
         then refer to the negotiation response values, otherwise the
         current values. */

      if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_I_XID )
        n201_i = gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid;
      else
        n201_i = gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;

      if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_kU_XID )
        kU = gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid;
      else
        kU = gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU;

      /* Is the product of N201-I and kU greater than the requested
         (proposed) mU parameter? */ 
      
      if ( (n201_i * kU) > (gllc_lle[gas_id][lle].unnum.xid_req.mu_xid * 16) )
      {
        /* It is so start by reducing N201-I. */

        n201_i = (uint16)(gllc_lle[gas_id][lle].unnum.xid_req.mu_xid * 16) / kU;
        
        if ( n201_i >= GLLC_MIN_N201_I ) 
        {
          /* Value of N201-I is above the minimum. */
          
          gllc_lle[gas_id][lle].unnum.xid_neg.contents  |= GLLC_N201_I_XID;
          gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid = n201_i;
        }
        else
        {
          /* Reducing N201-I would take it below minimum, so set N201-I at
             minimum and reduce kU as well. */
          
          n201_i = GLLC_MIN_N201_I;
          kU     = (uint8)(gllc_lle[gas_id][lle].unnum.xid_req.mu_xid * 16) / n201_i;
        
          gllc_lle[gas_id][lle].unnum.xid_neg.contents |=
            ( GLLC_N201_I_XID | GLLC_kU_XID );

          gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid = n201_i;
          gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid     = kU;
        }
      }
    }

    /* ................................................................... */

    /* Consider the case where this LLE wishes to keep a count of I frame
       octets in the uplink direction. */
    
    else
    {
      uint16 n201_i;

      /* mU negotiation sense is down. Negotiate down to the MS acceptable
         maximum if proposed value is above this. */
      
      if ( gllc_lle[gas_id][lle].unnum.xid_req.mu_xid >=
           GLLC_MAX_ACCEPTABLE_MU_OCTET16        )
      {
        gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid = GLLC_MAX_ACCEPTABLE_MU_OCTET16;
      }
      else   
        gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid =
          gllc_lle[gas_id][lle].unnum.xid_req.mu_xid;

      /* If the N201-I parameter is currently up for negotiation, then refer
         to the negotiation response values, otherwise the current values. */

      if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_I_XID )
        n201_i = gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid;
      else
        n201_i = gllc_lle[gas_id][lle].ack.max_info_octets_N201_I;
      
      /* The value of mU shall be negotiated such that at least one N201-I
         sized I frame can be accomodated. */

      if ( (gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid * 16) < n201_i )
        gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid = (n201_i / 16) + 1;
    }
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_generate_l2_xid_neg() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_SET_L2_XID_PARAMETERS() 
===
===  DESCRIPTION
===
===   Examines the given LLE's pre-validated/pre-generated XID negotiation
===   request and response unnumbered information elements, for layer 2
===   parameters which can now be set, in conclusion of the XID negotiation.
===
===   1. Parameters which appear in both the request and response are set as
===   per the response.
===
===   2. Parameters which appear in the request but not in the response are
===   set as per the request.
===
===   3. Parameters which don't appear in the request but appear in the
===   response are not handled here and are expected to have already been
===   added to the LLE's pending record for explicit negotiation at the next
===   opportunity. If the parameter in the response is the same as the
===   current value, then the parameter is removed from the LLE's pending
===   record.
===
===   The first exception to this last rule is IOV-I, N201-I, mD, mU, kD 
===   and kU, which may appear in the negotiated response field of a
===   UA response. The IOV-I parameter can only be sent in the downlink
===   direction and must be adopted without further explicit negotiation. The
===   other parameters cannot be negotiated to lower values once in ABM and
===   so are best adopted now without further explicit negotiation.  
===
===   The 2nd exception to this last rule is the case where the negotiated
===   values of mD and mU are zero. In such instances, new values for
===   N201-I, kD and kU may have been autonomously proposed in the
===   negotiated response, but which may not have appeared in the
===   negotiation request. In this case the new parameter values are adopted.
===
===   Parameter GLLC_RESET_XID is not handled here, and is handled as a
===   special case elsewhere.
===
===  DEPENDENCIES
===
===   The XID negotiation request and response in the LLE's unnumbered
===   information store must have been pre-validated or generated.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

void gllc_lle_set_l2_xid_parameters
(
  gas_id_t       gas_id,
  gllc_ll_sapi_t lle,
  gllc_u_frame_t *u_frame
)
{
  /* --------------------------------------------------------------------- */

  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_VERSION_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_VERSION_XID)    )
  {
    gllc_llme[gas_id].llc_version = gllc_lle[gas_id][lle].unnum.xid_neg.version_xid;
    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_VERSION_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_VERSION_XID )
  {
    gllc_llme[gas_id].llc_version = gllc_lle[gas_id][lle].unnum.xid_req.version_xid;
    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_VERSION_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_VERSION_XID )
  {
    if ( gllc_llme[gas_id].llc_version == gllc_lle[gas_id][lle].unnum.xid_neg.version_xid )
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_VERSION_XID;
  }

  /* --------------------------------------------------------------------- */

  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_IOV_UI_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_IOV_UI_XID)    )
  {
    gllc_llme[gas_id].input_offset_IOV_UI = gllc_lle[gas_id][lle].unnum.xid_neg.iov_ui_xid;
    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_IOV_UI_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_IOV_UI_XID )
  {
    gllc_llme[gas_id].input_offset_IOV_UI = gllc_lle[gas_id][lle].unnum.xid_req.iov_ui_xid;
    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_IOV_UI_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_IOV_UI_XID )
  {
    gllc_llme[gas_id].input_offset_IOV_UI = gllc_lle[gas_id][lle].unnum.xid_neg.iov_ui_xid;
    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_IOV_UI_XID;
  }

  /* --------------------------------------------------------------------- */

  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_IOV_I_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_IOV_I_XID)    )
  {
    gllc_lle[gas_id][lle].ack.input_offset_IOV_I =
      gllc_lle[gas_id][lle].unnum.xid_neg.iov_i_xid;
    
    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_IOV_I_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_IOV_I_XID )
  {
    gllc_lle[gas_id][lle].ack.input_offset_IOV_I =
      gllc_lle[gas_id][lle].unnum.xid_req.iov_i_xid;
    
    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_IOV_I_XID;
  }
  else if ( (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_IOV_I_XID) &&
            (u_frame != NULL                                      )    )
  {
    if ( u_frame -> cmd_res_M4_M1 == GLLC_UA )
    {
      gllc_lle[gas_id][lle].ack.input_offset_IOV_I =
        gllc_lle[gas_id][lle].unnum.xid_neg.iov_i_xid;
    
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_IOV_I_XID;
    }
    else if ( gllc_lle[gas_id][lle].ack.input_offset_IOV_I ==
              gllc_lle[gas_id][lle].unnum.xid_neg.iov_i_xid   )
    {
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_IOV_I_XID;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_T200_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_T200_XID)    )
  {
    gllc_lle[gas_id][lle].gen.retrx_interval_T200 =
      gllc_lle[gas_id][lle].unnum.xid_neg.t200_xid * 100;

    gllc_lle[gas_id][lle].ack.retrx_interval_T201 =
      gllc_lle[gas_id][lle].unnum.xid_neg.t200_xid * 100;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_T200_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_T200_XID )
  {
    gllc_lle[gas_id][lle].gen.retrx_interval_T200 =
      gllc_lle[gas_id][lle].unnum.xid_req.t200_xid * 100;

    gllc_lle[gas_id][lle].ack.retrx_interval_T201 =
      gllc_lle[gas_id][lle].unnum.xid_req.t200_xid * 100;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_T200_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_T200_XID )
  {
    if ( gllc_lle[gas_id][lle].gen.retrx_interval_T200 ==
         gllc_lle[gas_id][lle].unnum.xid_neg.t200_xid * 100 )
    {
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_T200_XID;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N200_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N200_XID)    )
  {
    gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200 =
      gllc_lle[gas_id][lle].unnum.xid_neg.n200_xid;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N200_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N200_XID )
  {
    gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200 =
      gllc_lle[gas_id][lle].unnum.xid_req.n200_xid;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N200_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N200_XID ) 
  {    
    if ( gllc_lle[gas_id][lle].gen.max_retrx_cnt_N200 ==
         gllc_lle[gas_id][lle].unnum.xid_neg.n200_xid    )
    {
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N200_XID;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N201_U_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_U_XID)    )
  {
    gllc_lle[gas_id][lle].unack.max_info_octets_N201_U =
      gllc_lle[gas_id][lle].unnum.xid_neg.n201_u_xid;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N201_U_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N201_U_XID )
  {
    gllc_lle[gas_id][lle].unack.max_info_octets_N201_U =
      gllc_lle[gas_id][lle].unnum.xid_req.n201_u_xid;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N201_U_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_U_XID )
  {
    if ( gllc_lle[gas_id][lle].unack.max_info_octets_N201_U ==
         gllc_lle[gas_id][lle].unnum.xid_neg.n201_u_xid        )
    {
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N201_U_XID;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N201_I_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_I_XID)    )
  {
    gllc_lle[gas_id][lle].ack.max_info_octets_N201_I =
      gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N201_I_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_N201_I_XID )
  {
    gllc_lle[gas_id][lle].ack.max_info_octets_N201_I =
      gllc_lle[gas_id][lle].unnum.xid_req.n201_i_xid;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N201_I_XID;
  }
  else if ( (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_I_XID) &&
            (u_frame != NULL                                       )    )
  {
    if ( (gllc_lle[gas_id][lle].state      == GLLC_LLE_LOC_ESTABLISH) &&
         (u_frame -> cmd_res_M4_M1 == GLLC_UA               )    )
    {
      if ( gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid < 
           gllc_lle[gas_id][lle].ack.max_info_octets_N201_I )
      {
        /* N201-I is present in the response but absent in the request. A
           transition to the ABM state is expected and the proposed N201-I is 
           less than currently used. Make an exception and accept this now
           as N201-I can't be negotiated to a lower value once in ABM. */

        gllc_lle[gas_id][lle].ack.max_info_octets_N201_I =
          gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid;
    
        gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N201_I_XID;
      }
    }
    else if ( gllc_lle[gas_id][lle].ack.max_info_octets_N201_I ==
              gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid      )
    {
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N201_I_XID;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_kD_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_kD_XID)    )
  {
    gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD =
      gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_kD_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_kD_XID )
  {
    gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD =
      gllc_lle[gas_id][lle].unnum.xid_req.kd_xid;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_kD_XID;
  }
  else if ( (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_kD_XID) &&
            (u_frame != NULL                                   )    )
  {
    if ( (gllc_lle[gas_id][lle].state      == GLLC_LLE_LOC_ESTABLISH) &&
         (u_frame -> cmd_res_M4_M1 == GLLC_UA               )    )
    {
      if ( gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid <
           gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD )
      {
        /* kD is present in the response but absent in the request. A
           transition to the ABM state is expected and the proposed kD is 
           less than currently used. Make an exception and accept this now
           as kD can't be negotiated to a lower value once in ABM. */

        gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD =
          gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid;
    
        gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_kD_XID;
      }
    }
    else if ( gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD ==
              gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid           )
    {
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_kD_XID;
    }
  }

  /* --------------------------------------------------------------------- */

  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_kU_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_kU_XID)    )
  {
    gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU =
      gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_kU_XID;
  }
  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_kU_XID )
  {
    gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU =
      gllc_lle[gas_id][lle].unnum.xid_req.ku_xid;

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_kU_XID;
  }
  else if ( (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_kU_XID) &&
            (u_frame != NULL                                   )    )
  {
    if ( (gllc_lle[gas_id][lle].state      == GLLC_LLE_LOC_ESTABLISH) &&
         (u_frame -> cmd_res_M4_M1 == GLLC_UA               )    )
    {
      if ( gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid <
           gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU )
      {
        /* kU is present in the response but absent in the request. A
           transition to the ABM state is expected and the proposed kU is 
           less than currently used. Make an exception and accept this now
           as kU can't be negotiated to a lower value once in ABM. */

        gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU =
          gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid;
    
        gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_kU_XID;
      }
    }
    else if ( gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU ==
              gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid           )
    {
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_kU_XID;
    }
  }

  /* --------------------------------------------------------------------- */

  /* mD XID parameter must be considered after N201-I and kD. */
  
  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_mD_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_mD_XID)    )
  {
    if ( gllc_lle[gas_id][lle].unnum.xid_neg.md_xid == 0 )
    {
      /* The LLE which originated the negotiated response does not wish
         to maintain a count of I frame octets in the downlink direction.
         To compensate, the LLE may have offered new values of N201-I and
         kD consistent with the mD parameter of the original request. */

      if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_I_XID )
      {
        gllc_lle[gas_id][lle].ack.max_info_octets_N201_I =
          gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid;

        gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N201_I_XID;
      }

      if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_kD_XID )
      {
        gllc_lle[gas_id][lle].ack.max_reseq_buf_frames_kD =
          gllc_lle[gas_id][lle].unnum.xid_neg.kd_xid;

        gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_kD_XID;
      }
    }

    gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD =
      (uint16)(gllc_lle[gas_id][lle].unnum.xid_neg.md_xid * 16);

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_mD_XID;
  }

  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_mD_XID )
  {
    gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD =
      (uint16)(gllc_lle[gas_id][lle].unnum.xid_req.md_xid * 16);

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_mD_XID;
  }

  else if ( (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_mD_XID) &&
            (u_frame != NULL                                   )    )
  {
    if ( (gllc_lle[gas_id][lle].state      == GLLC_LLE_LOC_ESTABLISH) &&
         (u_frame -> cmd_res_M4_M1 == GLLC_UA               )    )
    {
      if ( (uint16)(gllc_lle[gas_id][lle].unnum.xid_neg.md_xid * 16) <
           gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD           )
      {
        /* mD is present in the response but absent in the request. A
           transition to the ABM state is expected and the proposed mD is 
           less than currently used. Make an exception and accept this now
           as mD can't be negotiated to a lower value once in ABM. */

        gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD =
          (uint16)(gllc_lle[gas_id][lle].unnum.xid_neg.md_xid * 16);

        gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_mD_XID;
      }
    }
    else if ( gllc_lle[gas_id][lle].ack.max_reseq_buf_octets_mD ==
              (uint16)(gllc_lle[gas_id][lle].unnum.xid_neg.md_xid * 16) )
    {
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_mD_XID;
    }
  }

  /* --------------------------------------------------------------------- */

  /* mU XID parameter must be considered after N201-I and kU. */
  
  if ( (gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_mU_XID) &&
       (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_mU_XID)    )
  {
    if ( gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid == 0 )
    {
      /* The LLE which originated the negotiated response does not wish
         to maintain a count of I frame octets in the uplink direction.
         To compensate, the LLE may have offered new values of N201-I and
         kU consistent with the mU parameter of the original request. */

      if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_N201_I_XID )
      {
        gllc_lle[gas_id][lle].ack.max_info_octets_N201_I =
          gllc_lle[gas_id][lle].unnum.xid_neg.n201_i_xid;

        gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_N201_I_XID;
      }

      if ( gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_kU_XID )
      {
        gllc_lle[gas_id][lle].ack.max_retrx_buf_frames_kU =
          gllc_lle[gas_id][lle].unnum.xid_neg.ku_xid;

        gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_kU_XID;
      }
    }

    gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU =
      (uint16)(gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid * 16);

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_mU_XID;
  }

  else if ( gllc_lle[gas_id][lle].unnum.xid_req.contents & GLLC_mU_XID )
  {
    gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU =
      (uint16)(gllc_lle[gas_id][lle].unnum.xid_req.mu_xid * 16);

    gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_mU_XID;
  }

  else if ( (gllc_lle[gas_id][lle].unnum.xid_neg.contents & GLLC_mU_XID) &&
            (u_frame != NULL                                   )    )
  {
    if ( (gllc_lle[gas_id][lle].state      == GLLC_LLE_LOC_ESTABLISH) &&
         (u_frame -> cmd_res_M4_M1 == GLLC_UA               )    )
    {
      if ( (uint16)(gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid * 16) <
           gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU           )
      {
        /* mU is present in the response but absent in the request. A
           transition to the ABM state is expected and the proposed mU is 
           less than currently used. Make an exception and accept this now
           as mU can't be negotiated to a lower value once in ABM. */

        gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU =
          (uint16)(gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid * 16);

        gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_mU_XID;
      }
    }
    else if ( gllc_lle[gas_id][lle].ack.max_retrx_buf_octets_mU ==
              (uint16)(gllc_lle[gas_id][lle].unnum.xid_neg.mu_xid * 16) )
    {
      gllc_lle[gas_id][lle].unnum.xid_pending &= ~GLLC_mU_XID;
    }
  }

  /* --------------------------------------------------------------------- */

  gllc_diag_rpt_llme_state( gas_id );
  gllc_diag_rpt_lle_xid( gas_id, lle );
  
  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_set_l2_xid_parameters() */


/*===========================================================================
===
===  FUNCTION      GLLC_LLE_REPORT_L2_XID_CONTENT() 
===
===  DESCRIPTION
===
===   Generates a debug messaging report with an entry for each layer 2 XID
===   parameter contained in the given information element.
===
===   The msg_level parameter must be one of: 
===
===    MSG_LEGACY_MED     1        Medium priority messages                
===    MSG_LEGACY_HIGH    2        High priority messages                  
===
===  DEPENDENCIES
===
===   None.
===  
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===  
===========================================================================*/

void gllc_lle_report_l2_xid_content
(
  gas_id_t        gas_id,
  uint32          msg_level,
  gllc_ll_sapi_t  lle,
  gllc_xid_info_t *xid_info_p
)
{
  /* --------------------------------------------------------------------- */
  
  if ( xid_info_p -> contents )
  {
    switch ( msg_level )
    {
    case MSG_LEGACY_MED:

      if ( xid_info_p -> errors )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID Errors: 0x%x",
          gllc_sapi_addr_lookup[lle], xid_info_p -> errors);
      } 
      if ( xid_info_p -> contents & GLLC_RESET_XID )
      {
        MSG_GERAN_MED_1_G( "GLC LL %d: Info XID Reset",
          gllc_sapi_addr_lookup[lle]);
      }
      if ( xid_info_p -> contents & GLLC_VERSION_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID Vers:   %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> version_xid);
      }
      if ( xid_info_p -> contents & GLLC_IOV_UI_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID IOV-UI: 0x%x",
          gllc_sapi_addr_lookup[lle], xid_info_p -> iov_ui_xid);
      }
      if ( xid_info_p -> contents & GLLC_IOV_I_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID IOV-I:  0x%x",
          gllc_sapi_addr_lookup[lle], xid_info_p -> iov_i_xid);
      }
      if ( xid_info_p -> contents & GLLC_T200_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID T200:   %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> t200_xid);
      }
      if ( xid_info_p -> contents & GLLC_N200_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID N200:   %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> n200_xid);
      }
      if ( xid_info_p -> contents & GLLC_N201_U_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID N201-U: %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> n201_u_xid);
      }
      if ( xid_info_p -> contents & GLLC_N201_I_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID N201-I: %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> n201_i_xid);
      }
      if ( xid_info_p -> contents & GLLC_mD_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID mD:     %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> md_xid);
      }
      if ( xid_info_p -> contents & GLLC_mU_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID mU:     %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> mu_xid);
      }
      if ( xid_info_p -> contents & GLLC_kD_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID kD:     %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> kd_xid);
      }
      if ( xid_info_p -> contents & GLLC_kU_XID )
      {
        MSG_GERAN_MED_2_G( "GLC LL %d: Info XID kU:     %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> ku_xid);
      }
      break;

    /* ------------------------------------------------------------------- */

    case MSG_LEGACY_HIGH:

      if ( xid_info_p -> errors )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID Errors: 0x%x",
          gllc_sapi_addr_lookup[lle], xid_info_p -> errors);
      } 
      if ( xid_info_p -> contents & GLLC_RESET_XID )
      {
        MSG_GERAN_HIGH_1_G( "GLC LL %d: Info XID Reset",
          gllc_sapi_addr_lookup[lle]);
      }
      if ( xid_info_p -> contents & GLLC_VERSION_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID Vers:   %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> version_xid);
      }
      if ( xid_info_p -> contents & GLLC_IOV_UI_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID IOV-UI: 0x%x",
          gllc_sapi_addr_lookup[lle], xid_info_p -> iov_ui_xid);
      }
      if ( xid_info_p -> contents & GLLC_IOV_I_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID IOV-I:  0x%x",
          gllc_sapi_addr_lookup[lle], xid_info_p -> iov_i_xid);
      }
      if ( xid_info_p -> contents & GLLC_T200_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID T200:   %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> t200_xid);
      }
      if ( xid_info_p -> contents & GLLC_N200_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID N200:   %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> n200_xid);
      }
      if ( xid_info_p -> contents & GLLC_N201_U_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID N201-U: %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> n201_u_xid);
      }
      if ( xid_info_p -> contents & GLLC_N201_I_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID N201-I: %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> n201_i_xid);
      }
      if ( xid_info_p -> contents & GLLC_mD_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID mD:     %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> md_xid);
      }
      if ( xid_info_p -> contents & GLLC_mU_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID mU:     %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> mu_xid);
      }
      if ( xid_info_p -> contents & GLLC_kD_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID kD:     %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> kd_xid);
      }
      if ( xid_info_p -> contents & GLLC_kU_XID )
      {
        MSG_GERAN_HIGH_2_G( "GLC LL %d: Info XID kU:     %d",
          gllc_sapi_addr_lookup[lle], xid_info_p -> ku_xid);
      }
      break;

    default:

      MSG_GERAN_ERROR_2_G( "GLC LL %d: Rprt L2 XID Lvl Err %d",
        gllc_sapi_addr_lookup[lle], msg_level);
    }
  }

  /* --------------------------------------------------------------------- */
} /* end of gllc_lle_report_l2_xid_content() */



/*** EOF: don't remove! ***/

