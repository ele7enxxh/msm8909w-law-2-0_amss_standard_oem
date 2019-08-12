/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2007        Qualcomm Technologies Incorporated. All Rights Reserved
  Copyright (c) 2009 - 2010 Qualcomm Technologies Incorporated. All Rights Reserved

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_se_ber.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/10   ems     Factory features Rx Lvl, non-signaling events and SE/BER
08/31/10   rm      Reverting KW fix which caused SE BER to fail
04/01/10   adk     Merged branch scmm_mdm9k_merge_final
03/19/10   lcl     Pull in changes from QSC6295
02/12/10   xw      Klocwork fix 
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
10/22/07   ad      Merge SE BER support from 6260: Add new SE BER API for setting 
                   GSM loop A/B bit ordering, and  EDGE SRB expected MCS.
01-23-07   avi     Created
===========================================================================*/
#ifndef FTM_SE_BER_H
#define FTM_SE_BER_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_log.h"

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FTM_HAS_SE_BER

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                               MACROS

===========================================================================*/
#define FTM_SE_BER_MAX_REPORT_SAMPLES     64

/*===========================================================================
 
                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/




/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

typedef enum
{
    FTM_SE_PN_TYPE_PN1 = 0,
    FTM_SE_PN_TYPE_PN9,
    FTM_SE_PN_TYPE_PN11,
    FTM_SE_PN_TYPE_PN15,
    FTM_SE_PN_TYPE_MAX = 0xFF
} se_ber_pn_length_type;

typedef enum
{
    FTM_SE_MODE_ACQUIRE = 0,
    FTM_SE_MODE_ACQUIRE_RUN,
    FTM_SE_MODE_RUN,
    FTM_SE_MODE_AUTO,
    FTM_SE_MODE_MAX = 0xFF
} se_ber_mode_type;

/* This type defines the mode of operation when SE BER module is in "steady state" */
typedef enum
{
  FTM_SE_RUN_MODE_WCDMA_RMC = 0,        /* R99 */
  FTM_SE_RUN_MODE_EGPRS_SRB,            /* EDGE SRB */
  FTM_SE_RUN_MODE_GSM_LOOP_AB,          /* Loop A or B, separate bit classes */
  FTM_SE_RUN_MODE_GSM_LOOP_C            /* Loop C, burst by burst */

} ftm_se_ber_run_mode_type;

typedef enum
{
  FTM_SE_BER_BIT_ORDERING_LEFT_ALIGNED = 0,   /* earliest received bit is bit 7 of a uint8 */
  FTM_SE_BER_BIT_ORDERING_RIGHT_ALIGNED       /* earliest received bit is bit 0 of a uint8 */
} ftm_se_ber_bit_ordering_type;

typedef enum
{
  FTM_SE_BER_MCS_RESERVED = 0,
  FTM_SE_BER_MCS1,
  FTM_SE_BER_MCS2,
  FTM_SE_BER_MCS3,
  FTM_SE_BER_MCS4,
  FTM_SE_BER_MCS5,
  FTM_SE_BER_MCS6,
  FTM_SE_BER_MCS7,
  FTM_SE_BER_MCS8,
  FTM_SE_BER_MCS9
} ftm_se_ber_mcs_type;

typedef struct
{
   ftm_se_ber_run_mode_type      run_mode;
   ftm_se_ber_bit_ordering_type  bit_ordering;
   uint8                         slot_num;
   uint32                        fn;
   boolean                       bfi;

   union
   {

     uint16                      num_bits;

     struct
     {   
       uint8                     num_class1a_bits;
       uint8                     num_class1b_bits;
       uint8                     num_class2_bits;
     } num_bits_gsm_loop_ab;

   } bit_def;

} ftm_se_ber_config_run_type;


typedef enum
{
	FTM_SE_BER_REPORT_COUNT_08 = 8,
	FTM_SE_BER_REPORT_COUNT_16 = 16,
	FTM_SE_BER_REPORT_COUNT_32 = 32,
	FTM_SE_BER_REPORT_COUNT_64 = 64,
	FTM_SE_BER_REPORT_COUNT_MIN = FTM_SE_BER_REPORT_COUNT_08,
	FTM_SE_BER_REPORT_COUNT_MAX = FTM_SE_BER_REPORT_COUNT_64,
	FTM_SE_BER_REPORT_COUNT_FORCE_BYTE = 0xFF
} se_ber_log_reports_count_type;

/* the following types define the PC interface */
enum
{
  FTM_SE_BER_GEN_RESPONSE = 0
};

/* This type specifies the FTM SE BER response header packet */
typedef PACK(struct)
{
    uint16                     rsp_id;
    uint16                     rsp_cid;
    uint16                     reserved;

} ftm_se_ber_rsp_header_type;

/* This type specifies the FTM SE BER response packet */
typedef PACK(struct)
{

    ftm_diagpkt_subsys_header_type     header;
    ftm_se_ber_rsp_header_type         ftm_rsp_header;
    uint16                             status;

} ftm_se_ber_gen_rsp_pkt_type;

/* This type specifies the FTM SE BER status response packet */
typedef PACK(struct)
{

    ftm_diagpkt_subsys_header_type     header;
    ftm_se_ber_rsp_header_type         ftm_rsp_header;
    uint16                             status;
    uint32                             total_bits;
    uint32                             total_bit_errors;

} ftm_se_ber_status_rsp_pkt_type;


/* This type specifies the results for the FTM SE BER synchronous status */
typedef PACK(struct)
{
    uint32                     total_bits;
    uint32                     total_bit_errors;
} ftm_se_ber_sync_status_type;

/* This type specifies the FTM SE BER response packet */
typedef PACK(struct)
{
    ftm_diagpkt_subsys_header_type     header;
    ftm_se_ber_rsp_header_type         ftm_rsp_header;
    uint16                             status;
    ftm_se_ber_sync_status_type        sync_status;
} ftm_se_ber_sync_status_rsp_pkt_type;

typedef enum
{
  FTM_SE_BER_CONFIGURE = 0,   /* configure the PN sequence */
  FTM_SE_BER_START,           /* trigger the BER logging subsystem */
  FTM_SE_BER_STOP,             /* halt the BER logging subsystem */
  FTM_SE_BER_SET_LOOP_AB_ORDER,    /* set the PN bit ordering for GSM Loops A/B */
  FTM_SE_BER_SET_EXPECTED_MCS,
  FTM_SE_BER_CLEAR_SYNC_STATUS,
  FTM_SE_BER_GET_SYNC_STATUS
} ftm_se_ber_cmd_type;

/* This type specifies the interface for the FTM SE BER Configure Command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type           header;
    ftm_cmd_header_type                  ftm_cmd_header;

    /* PN generator to use */

    se_ber_pn_length_type                pn_type;

    /* Single ended BER mode of operation */

    se_ber_mode_type                     se_mode;

    /* flag to indicate whether output sequence of pn generator should be inverted.  Typically
     * this is utilized for PN15 according to ITU-T specifications.  However, not all standard 
     * test equipment will perform the inversion.  
     */

    boolean                              invert;  

    /* Acquisition threshold -- represents number of consecutive bits necessary to declare
     * we have acquired to the PN sequence
     */

    uint16                               acq_threshold;
	/* Acquisition lost watermark -- represents the number of bits in error necessary 
	 * to reset the state of ber state machine to "not acquired"
	 */

	uint16                               acq_lost_wm;
} ftm_se_ber_config_type;

/* This type specifies the interface for the FTM SE BER Start Command */

typedef PACK(struct)
{

    diagpkt_subsys_header_type           header;
    ftm_cmd_header_type                  ftm_cmd_header;

    /* Number of SE BER reports per log packet */

    se_ber_log_reports_count_type                report_count;

} ftm_se_ber_start_type;


/* this defines where the test equipment applies the PN data for GSM loops A/B.  If the box
 * applies the PN data as if it were coming directly from the output of the vocoder, then the user must
 * select FTM_SE_BER_LOOP_AB_ORDER_VOC_OUTPUT.  From the vocoder, the bits are reordered in terms of bit classes,
 * before the rest of the channel codec stage.  If the test box applies the PN data AFTER this reordering step,
 * the user must select FTM_SE_BER_LOOP_AB_ORDER_CH_CODEC_INPUT.
 */
typedef enum
{
  FTM_SE_BER_LOOP_AB_ORDER_CH_CODEC_INPUT = 0,
  FTM_SE_BER_LOOP_AB_ORDER_VOC_OUTPUT,
  FTM_SE_BER_LOOP_AB_ORDER_MAX
} ftm_se_ber_loop_ab_order_type;

typedef PACK(struct)
{

  diagpkt_subsys_header_type           header;
  ftm_cmd_header_type                  ftm_cmd_header;

  ftm_se_ber_loop_ab_order_type        order;
} ftm_se_ber_set_loop_ab_order_type;

typedef PACK(struct)
{

  diagpkt_subsys_header_type           header;
  ftm_cmd_header_type                  ftm_cmd_header;

  ftm_se_ber_mcs_type                  expected_mcs;
} ftm_se_ber_set_expected_mcs_type;

typedef enum
{

    FTM_LOG_SE_BER_GENERIC_REPORT        = 0,
    FTM_LOG_SE_BER_GSM_LOOP_AB_REPORT

} ftm_log_se_ber_id_type;

typedef enum
{
  FTM_LOG_SE_BER_PN_STATE_UNLOCKED = 0,
  FTM_LOG_SE_BER_PN_STATE_LOCKED
} ftm_log_se_ber_pn_state_type;

typedef PACK(struct)
{
  uint32                       fn;
  uint8                        slot_num;
  ftm_log_se_ber_pn_state_type pn_state;
  uint16                       num_bits;
  uint16                       num_errors;

} ftm_log_se_ber_sample_type;

typedef PACK(struct)
{

  uint32                       fn;
  uint8                        slot_num;
  ftm_log_se_ber_pn_state_type pn_state;
  boolean                      bfi;                  /* bad frame indicator */
  uint16                       num_class1a_bits;     /* error split by bit classes */
  uint16                       num_class1a_errors;
  uint16                       num_class1b_bits;
  uint16                       num_class1b_errors;
  uint16                       num_class2_bits;
  uint16                       num_class2_errors;

} ftm_log_se_ber_gsm_loop_ab_sample_type;

/* This type specifies the FTM SE BER report type */
typedef PACK(struct)
{

    ftm_log_gen_type           header;
    ftm_log_se_ber_id_type     log_id;
    uint8                      num_samples;
    ftm_log_se_ber_sample_type data[1];

} ftm_log_se_ber_report_type;

/* This type specifies the FTM SE BER report type */
typedef PACK(struct)
{

    ftm_log_gen_type                       header;
    ftm_log_se_ber_id_type                 log_id;
    uint8                                  num_samples;
    ftm_log_se_ber_gsm_loop_ab_sample_type data[1];

} ftm_log_se_ber_report_gsm_ab_type;




/*===========================================================================
FUNCTION     FTM_SE_BER_CONFIGURE

DESCRIPTION

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  Posts a WL1 message to FTM.
===========================================================================*/

extern void ftm_se_ber_configure( se_ber_pn_length_type pn_len, se_ber_mode_type ber_mode, boolean invert, uint16 acq_thresh, uint16 lost_acq_water_mark );
extern void ftm_se_ber_start ( se_ber_log_reports_count_type report_count );
extern void ftm_se_ber_stop( void );
extern void ftm_se_ber_run( uint8* rx_data, ftm_se_ber_config_run_type *config);
extern ftm_rsp_pkt_type ftm_se_ber_dispatch( ftm_common_pkt_type *cmd);

#endif /* FTM_HAS_SE_BER */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_SE_BER_H */
