#ifndef TDSDLCCTRCH_H
#define TDSDLCCTRCH_H

/*============================================================================
                           D L D E C . H
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dldec.h_v   1.33   11 Jul 2002 22:14:10   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdlcctrch.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
03/07/11     xq     create this file
===========================================================================*/
#include "tdscdma_variation.h"
#ifdef FEATURE_MCAL_TDSCDMA
/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "tdsl1rrcif.h"
#include "tdsl1macdata.h"
#include "tdsdlcctrchlog.h"
#include "queue.h"
#include "msm.h"
//#include "mdspext.h"

#include "tdsdldeccommon.h"
#include "tdsl1m_rcinit.h"
//#include "tdsdec.h"

/* force feature define based on master feature list in cust file */
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
#define TDSL1DEC_FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* minumum TB energy matrix for TBset for Conv coded TrCh.
   If the egy matric value is lower than this CRC is forced to fail */
#define TDSL1DEC_MIN_TB_EGY_MATRIC_THRESH 256
/* Maximum symbol error on the passed block. Used in case of BTFD.
   If a TF which passes CRC has symbol error > threshold, the TF is dropped */
#define TDSL1DEC_MAX_SYMBOL_ERR_THRESHOLD    10

#define TDSL1DEC_MDSP_SF_CFN_TTI_TYPE(cfn) \
  ((((cfn + 1) % 2) != 0) ? TDSL1_TTI_10MS : \
   (((cfn + 1) % 4) != 0) ? TDSL1_TTI_20MS : \
   (((cfn + 1) % 8) != 0) ? TDSL1_TTI_40MS : TDSL1_TTI_80MS)

#define TDSDLDEC_NUM_MAX_RRC_NON_BCH_CCTRCH 1

#define TDSDLDEC_NUM_MAX_RRC_SBCH_CCTRCH    1
#define TDSDLDEC_NUM_MAX_RRC_CCTRCH         2 /*SCCPCH&PCCPCH*/
#define TDSDLDEC_NUM_MAX_L1_BCH_INIT_CCTRCH 4 /* for internal L1 BCH init setup */
#define TDSDLDEC_NUM_MAX_L1_NBCH_CCTRCH     2 /* for internal L1 NBCH SFN decode */


/* DL_DEC_MAX_CCTRCH_WITH_BTFD indicate the total number of CCTrCh that
   can be with BTFD requirement */
#define TDSDLDEC_MAX_CCTRCH_WITH_BTFD  1

/* DL_DEC_MAX_CCTRCH_BTFD_INFO indicate the number of element in the array
   of the variable/structure that are required by BTFD module. This takes
   in to account the 1 extra data structure required for reconfig request */
#define TDSDLDEC_MAX_CCTRCH_BTFD_INFO  (TDSDLDEC_MAX_CCTRCH_WITH_BTFD + 1)

#define TDSDLDEC_MAX_DATA_BITS_BTDF    600
#define TDSDLDEC_MAX_TFC_BTFD          64
#define TDSDLDEC_MAX_TRCH_W_CRC_BTFD   3
#define TDSDLDEC_MAX_TF_ALL_TRCH_W_CRC_BTFD 16

/* This Id is used by CCTrCh setup for L1 internal BCH CCTrCh setup for
   BCH init at ACQ or for NBCH setup for SFN decode */
#define TDSDLDEC_L1_INT_BCH_CCTRCH_ID 0xFF
#define TDSDLDEC_L1_INVALID_CCTRCH_ID 0xFE
#define TDSDLDEC_L1_RRC_CCTCRH_ID_SWAPPED 0xFD
#define TDSDLDEC_NUM_TRCH_BCH         1
#define TDSDLDEC_NUM_TF_BCH           1

#ifdef FEATURE_MCAL_TDSCDMA
#define TDSDLDEC_BCH_TB_SIZE_BITS               MCALTDS_DEC_BCH_TB_SIZE_BITS
#else
#define TDSDLDEC_BCH_TB_SIZE_BITS     246
#endif

/* CcTrCh index / Id that indicates the unsued Id */
#define TDSDLDEC_L1_ID_INDEX_UNUSED NO_CCTRCH_INDEX

//#ifdef FEATURE_WCDMA_L1_TRBLK_DATA_RECEIVE_AT_TTI_BDRY
#define TDSDLDEC_GET_CFN_TTI_ALIGN_TYPE(cfn) \
  ((((cfn) % 2) != 0) ? TDSL1_TTI_10MS : \
   (((cfn) % 4) != 0) ? TDSL1_TTI_20MS : \
   (((cfn) % 8) != 0) ? TDSL1_TTI_40MS : TDSL1_TTI_80MS)
//#endif

#define TDSL1DEC_DOB_HANDLE_INTLOCK()  REX_ISR_LOCK(&tdsl1dec_dob_handle_crit_sect)
#define TDSL1DEC_DOB_HANDLE_INTFREE()  REX_ISR_UNLOCK(&tdsl1dec_dob_handle_crit_sect)

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
  TDSDEC_LOOPBACK_NA,       /* Transport channel not in loopback mode */
  TDSDEC_LOOPBACK_MODE_1_1, /* Loopback mode 1 only CRC passed block */
  TDSDEC_LOOPBACK_MODE_1_2, /* Loopback mode 1 all blocks (CRC pass/fail) */
  TDSDEC_LOOPBACK_MODE_2    /* Loopback mode 2 */
} tsdl1dec_loop_back_mode_type_enum_type;

typedef enum
{
  /* status FIFO is enpty and can be used */
  TDSDEC_DOB_STATUS_FIFO_EMPTY,
  /* status FIFO belongs tp CCTrCh that need BTFD decision */
  TDSDEC_DOB_STATUS_FIFO_BTFD,
  /* status FIFO points to DOB data that need to be delivered to MAC */
  TDSDEC_DOB_STATUS_FIFO_DELIVER
} tdsl1dec_dob_status_fifo_state_enum_type;


/* Decoder call back function type.
   This function type is called after any TBset status read */
typedef void TDSL1DEC_CCTRCH_T_CB_FUNC_TYPE(uint8);

/* This enum defines the BCH or NBCH CCTrCh sub states */
typedef enum
{
  TDSDLDEC_CCTRCH_BCH_OFF_TYPE,              /* BCH or NBCH not doing anything */
  TDSDLDEC_CCTRCH_BCH_INIT_TYPE,             /* BCH used to TTI sync   */
  TDSDLDEC_CCTRCH_BCH_DATA_DL_TYPE,      /* BCH or NBCH set by RRC that is ready for data delivery */
  TDSDLDEC_CCTRCH_NOT_BCH_TYPE               /* used for all CCTrCh that are not BCH */
} tdsdldec_bch_sub_state_enum_type;

/* This enum defines the BCH usage statets that can be set by demod */
typedef enum
{
  TDSDLDEC_BCH_USE_OFF,                           /* Turn the BCH TB processing off */
  TDSDLDEC_BCH_USE_STATUS_TO_DEMOD, /* Start giving TB set status to demod */
  TDSDLDEC_BCH_USE_DELIVER_TO_MAC      /* start deliverying BCH TB to MAC */
} tdsdldec_bch_usage_enum_type;

/* This enum defines the reference type of the CCTrCh */
typedef enum
{
  TDSDLDEC_CCTRCH_SBCH_TYPE,         /* Serving cell BCH CCTrCh set by RRC */
  TDSDLDEC_CCTRCH_NBCH_TYPE,         /* neighbor BCH CCTrCh set by RRC */
  TDSDLDEC_CCTRCH_DCH_TYPE,          /* DCH CCTrCh set by RRC */
  TDSDLDEC_CCTRCH_DSCH_TYPE,         /* DSCH CCTrCh set by RRC */
  TDSDLDEC_CCTRCH_PCH_TYPE,          /* PCH CCTrCh set by RRC */
  TDSDLDEC_CCTRCH_FACH_TYPE,         /* FACH CCTrCh set by RRC */
  TDSDLDEC_CCTRCH_PCH_FACH_TYPE,     /* PCH_FACH CCTrCh set by RRC */
  TDSDLDEC_CCTRCH_INVALID,           /* Invalid CCTrCh */
  TDSDLDEC_NUM_CCTRCH_REF_TYPE       /* number of CCTrCh reference types */
} tdsdldec_cctrch_ref_type_enum_type;

extern const uint8 tdsl1dec_cctrch_ref_to_trch_bf_table[TDSDLDEC_NUM_CCTRCH_REF_TYPE];

/* macro to convert the CCTrCh reference type to TrCh BF */
#define TDSDLDEC_CCTRCH_REF_TO_TRCH_BF(x) (tdsl1dec_cctrch_ref_to_trch_bf_table[x])

/* define the CCTrCh setup command type */
typedef struct
{
  tdsl1_dl_cctrchcfg_cctrch_info_struct_type   *dl_cctrch_info; /* CCTrCh info */
  tdsl1_dl_cctrchcfg_ctfc_info_struct_type     *dl_ctfc_info;   /* CTFC info */
  tdsl1_setup_ops_enum_type          dl_op_type;      /* Phychan operation ADD or CFG*/
  tdsl1_dl_phychan_enum_type         dl_phychan_type; /* ADD/CFG PhyChan type */
  tdsl1_dl_phycfg_phychan_db_struct_type    *dl_phychan_db;  /* PhyChan info */
  uint8                           drop_cctrch_idx; /* CCTrCh index being dropped */
} tdsdldec_cctrch_setup_cmd_type;

#ifdef TDSL1DEC_FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC
/* call back function type for failed block passing to higher layer info */
typedef uint16 (*TDSDLDEC_FAILED_BLK_QUERY_CB_FUNC_TYPE)(void);
#endif /* TDSL1DEC_FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* This array stores the number of TrCh of a CCTrCh */
extern uint8 tdsl1dec_dl_num_trch[TDSDLDEC_TOTAL_CCTRCH_TABLES];
/* This array stores the number of TF in the TrChs of a CCTrCh */
extern uint8 tdsl1dec_dl_num_tf[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];
/* This array holds the TrCh reference type information of the TrChs
   (Tb set for DOB data) */
extern tdsl1_dl_trch_ref_type_enum_type
tdsl1dec_dl_trch_ref_type[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];

/* This variable stores CCTrCh table availability status. mDSP has 6 CCTrCh
   tables out of which any 3 can be used as active ones. Other 3 are used
   to reconfigure the 3 currently active CCTrCh without the loss of data */
extern boolean tdsl1dec_cctrch_avail_status[TDSDLDEC_TOTAL_CCTRCH_TABLES];

/* This variable contains the CCTrCh table index for the CCTrCh Id passed
   to the setup function. Each CCTrCh has got unique Id and a new CCTrCh
   table index is associated with the new configured CCTrCh. If this is a
   modify request the previous CCTrCh table index is freed from the CCTrCh
   Id. */
extern uint8 tdsl1dec_dl_cctrch_id_table_index[TDSL1_DL_CCTRCH_ID_COUNT];
/* This variable contains the CCTrCh Ids corresponding to the CCTrCh table
   index. This is used for getting the CCTrCh table Ids from the table index
   passed in the link FIFO as the mDSP version of CCTrCh Id. */
extern uint8 tdsl1dec_dl_cctrch_table_index_id[TDSDLDEC_TOTAL_CCTRCH_TABLES];

/* This array keeps the CCTrCh table index to which the TrCh Id passed
   by RRC, belongs */
extern uint8 tdsl1dec_dl_trch_id_cctrcht_index[TDSL1_DL_MAX_TRCH_ID + 1];
/* This array keeps the TrCh table index to which the TrCh Id passed
   by RRC, belongs */
extern uint8 tdsl1dec_dl_trch_id_table_index[TDSL1_DL_MAX_TRCH_ID + 1];
/* This variable contains the TrCh Ids corresponding to the TrCh table index.
   This is used for getting the TrCh table Ids from the table index passed
   in the link FIFO as the mDSP version of TrCh Id. */
extern uint8 tdsl1dec_dl_trch_table_index_id[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];

/* This array holds the coding type information of the TrChs
   (Tb set for DOB data) */
extern tdsl1_dl_cctrchcfg_coding_enum_type
dl_trch_coding_type[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];

/* This array holds the crc length information of the TrChs */
extern uint8 tdsl1dec_dl_trch_crc_length[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];

/* this array hold the boolean values that indicate that
   1. If the CCTrCh is TFCI case: Then this TrCh has non zero CRC type.
   2. No-TFCI case: This TrCh has all the TF having non zero CRC. That means
                    no TF has 0 number of TB.
   This decision is used at number of places so it is good to calculate it
   once and use it at different places */
extern boolean tdsl1dec_non_zero_crc_criteria[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];

/* This array saves the TTI of each TrCh in all CCTrCh */
extern tdsl1_tti_enum_type tdsl1dec_dl_trch_tti[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];

/* This array stores the TF information for all CCTrCh . This is required by
   the transport block handling module to get the information about the
   transport block set size and number of transport blocks. */
extern tdsl1_dl_cctrchcfg_tf_info_struct_type
*tdsl1dec_tf_info[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];

//#ifdef FEATURE_WCDMA_L1_TRBLK_DATA_RECEIVE_AT_TTI_BDRY
/* CFN at which CCTrCh is enabled */
extern uint8 tdsl1dec_cctrch_enable_cfn[TDSDLDEC_TOTAL_CCTRCH_TABLES];
/* TTI boundry check pending */
extern boolean tdsl1dec_trblk_tti_bdry_check_pending[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];
//#endif

#ifdef FEATURE_TEST_LOOPBACK
/* This variable maintains the loop back state of each TrCh in a CcTrCh */
extern tsdl1dec_loop_back_mode_type_enum_type
tdsl1dec_loopback_mode[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];
#endif /* FEATURE_TEST_LOOPBACK */

#ifdef TDSL1DEC_FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC
extern boolean
tdsl1dec_errored_blk_pass_thro_mode[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];
#endif

/* This array holds the TrCh enable disable information of the TrChs
   (Tb set for DOB data) */
extern boolean tdsl1dec_trch_enable_state[TDSDLDEC_TOTAL_CCTRCH_TABLES][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];

/* TDSL1DEC_FEATURE_DL_FAILED_TRBLK_QUERY_CB_FUNC */
/* This variable keep the CCTrCh enabled state. At the setup time the CCTrCh
   is initialized to enabled = FALSE. Demod calls the function in decoder
   module to enable the CCTrCh after it has setup the corresponding PhCh.
   In the disabled state no action is taken for any TB received. */
extern boolean tdsl1dec_cctrch_enabled[TDSDLDEC_TOTAL_CCTRCH_TABLES];

extern rex_timer_type tdsl1dec_bler_meas_delay_timer;

extern tdsdldec_cctrch_ref_type_enum_type tdsl1dec_cctrch_ref_type[TDSDLDEC_TOTAL_CCTRCH_TABLES];

#ifdef FEATURE_TDSCDMA_L1_TB_BASED_OLPC
/* This array has TF index of TrChs in CCTrCh that has max blocks and is the
   part of TFC table. If there is no TF index existing in that TrCh then value
   is set to -1 */

extern int8 tdsl1dec_dch_trch_max_blk_tf_idx[TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH];
#endif

/* Number of times SFN mismatch is detected */
extern uint32 tdsdl_num_times_sfn_mismatch;

/* Dirty Bitmask to indicate which of the
   CCTrCh needs to be updated in firmware. */
extern uint8 tdsl1dec_cctrch_dirty_bmsk;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION tdsl1dec_init

DESCRIPTION
  This function initializes the variables related to this module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_init(void);

#if 0
/*===========================================================================
FUNCTION tdsl1dec_set_cctrch_idx_cb_func

DESCRIPTION
  This function register the call back function for CCTtrCh index. If CCTrCh
  index is out of range it will return with no action and MSG_ERROR. If call
  back function pointer is NULL then it is equivalent to deregistration.

  The registered functions are called when any TrCh (TBset) status is read.
  These functions are passed with argument of DOB status FIFO index that has
  details about DOB location, CCTrCh's TrCh and TF index, CRC pas fail, TB size
  etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates CCTrCh table index call back function pointer
==========================================================================*/

extern void tdsl1dec_set_cctrch_idx_cb_func(
  /* CcTrCh index for which call back function need to be registered */
  uint8 cctrch_table_idx,
  /* call back function to be registered */
  TDSL1DEC_CCTRCH_T_CB_FUNC_TYPE *cb_func);
#endif
#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
/*===========================================================================
FUNCTION        DEC_SUBMIT_TBSET_STAT_LOGPKT

FILE NAME       dlcmd.c

DESCRIPTION     This function submits the tbset statistics log pkt.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
//extern void dec_submit_tbset_stat_logpkt(void);
#endif

/*===========================================================================
FUNCTION dl_dec_initialize

DESCRIPTION
  This function initialize the decoder sub modules. This is called by L1
  manager at the startup. It performs the following initializations.
  - Sets the total number of CcTrCh used to 0 and initialize the cctrch table
    index for all id to 0xFF, all CCTrCh table Ids for all table index to
    0xFF. Sets the CCTrCh table availability status to TRUE.. Sets the TFCI
    table index availabilty to TRUE and set the TFCI index for CCTrCh table
    index to 0xFF.
  - Calls all the initialization function in decoder sub modules.
  - Set the read done status for header information to TRUE
  - Set the read done status for data information to TRUE
  - Initialize the buffer pointer to the transport block data to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_dl_dec_init(void);

/*===========================================================================
FUNCTION tdsl1dec_set_ciphering_keys

DESCRIPTION
  This function validates and sets the ciphering key information for downlink
  deciphering operation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE validate and update result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsl1dec_set_ciphering_keys(
  /* DL ciphering info structure */
  tdsl1_cipher_key_info_struct_type *dl_cipher_info);

/*===========================================================================
FUNCTION tdsl1dec_validate_ciphering_key_req

DESCRIPTION
  This function validates the ciphering key information for downlink
  deciphering operation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE validate result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsl1dec_validate_ciphering_key_req(
  /* DL ciphering info structure to validate the information */
  tdsl1_cipher_key_info_struct_type *dl_cipher_info);

/*===========================================================================
FUNCTION tdsl1dec_cctrch_setup

DESCRIPTION
  This function configure or modify the CCTrCh information. This is called
  by L1 manager. It goes through the currently existing CCTrCh Id and if
  CCTrCh Id in this info structure is active it assumes this function call
  as modify request otherwise as new configure request. This function calls
  the functions to calculate the rate matching parameters, rate matched size,
  code block parameters (number and size) and setup mDSP tables.

DEPENDENCIES
  None

RETURN VALUE
  Returns the CCTrCh table index setup. It retuens 0xFF in case of any
  failure. The reason for the failure can be as follows:
   - No free mDSP CCTrCh table exists to update CCTrCh info to mDSP.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 tdsl1dec_cctrch_setup(
  /* DL CCTrCh setup command packet */
  tdsdldec_cctrch_setup_cmd_type *cctrch_setup_cmd_pkt
);

/*===========================================================================

FUNCTION tdsl1dec_set_cctrch_enable_state

DESCRIPTION
  This function sets the CCTrCh enable state. Before setting the state of the
  CCTrCh table index it checks if that CCTrCh table index is in use and is
  a valid CCTrCh index.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on success of state change

SIDE EFFECTS
  Updates the state in array element of array tdsl1dec_cctrch_enabled.
===========================================================================*/
extern boolean tdsl1dec_set_cctrch_enable_state(
  /* CCTrCh table index for which the enable state is to be set */
  uint8 cctrch_index,
  /* CCTrCh state to be set */
  boolean state,
  /* CFN at which corresponding H/W ch is enabled/diabled */
  uint8 en_dis_cfn);

/*===========================================================================
FUNCTION tdsl1dec_validate_cctrch_setup_req

DESCRIPTION
  This function validates the parameters for the CCTrCh set command pacaket.
  In navigates through all the paramaters and check wheather the values are
  valid and with in range.

DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE as per the validation result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsl1dec_validate_cctrch_setup_req(
  /* DL CCTrCh setup command packet */
  tdsdldec_cctrch_setup_cmd_type *cctrch_setup_cmd_pkt);

/*===========================================================================
FUNCTION tdsl1dec_btfd_validate

DESCRIPTION
  This function validates the BTFD related restriction on the CCTrCh.
  These are based on the restriction specified in 25.212 V3.5.0 2000-12
  chapter 4.3.1.

DEPENDENCIES
  None

RETURN VALUE
  TRUE of FALSE as per the validation result.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsl1dec_btfd_validate(
  /* DL CCTrCh setup command packet */
  tdsdldec_cctrch_setup_cmd_type *cctrch_setup_cmd_pkt,
  /* indicate that phychan drop pending is BTFD */
  boolean drop_chan_btfd);

/*===========================================================================
FUNCTION tdsl1dec_set_bch_cctrch_info

DESCRIPTION
  This function create the BCH CCTrCh info. The BCH CCTrCh command packet
  doesn't come with CCTrCh info from RRC. Also this information is required
  by the interbal BCH setup by L1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_set_bch_cctrch_info(
  /* CCTrCh setup information from RRC. */
  tdsl1_dl_cctrchcfg_cctrch_info_struct_type   *cctrch_info);

/*===========================================================================
FUNCTION tdsl1dec_cctrch_setup_bch

DESCRIPTION
  This function setup the a CCTrCh table for initial BCH setup for getting
  the timing and SFN count of the BCH or for the neighbor BCH to get the SFN
  value. This is called demod module.

DEPENDENCIES
  None

RETURN VALUE
  Table index of neighbour BCH, 0xFF if not successful.

SIDE EFFECTS
  Set the boolean dl_send_bch_init_trblk_flag to false.
===========================================================================*/

extern uint8 tdsl1dec_cctrch_setup_bch(void);

/*===========================================================================
FUNCTION tdsl1dec_get_cctrch_ref_type

DESCRIPTION
  This function returns the CCTrCh reference type for the CCTrCh table index.

DEPENDENCIES
  None

RETURN VALUE
  CCTrCh reference type of the CCTrCh.

SIDE EFFECTS
  None
===========================================================================*/

extern tdsdldec_cctrch_ref_type_enum_type tdsl1dec_get_cctrch_ref_type(
  /* CCTrCh mDSP table index for which the ref type is to be returned */
  uint8 cctrch_table_index);

/*===========================================================================
FUNCTION tdsl1dec_gen_cctrch_ref_type

DESCRIPTION
  This function generate the CCTrCh type depending on the TrCh ref types of
  the transport channel contained in it.

DEPENDENCIES
  None

RETURN VALUE
  CCTrCh reference type of the CCTrCh.

SIDE EFFECTS
  None
===========================================================================*/

extern tdsdldec_cctrch_ref_type_enum_type tdsl1dec_gen_cctrch_ref_type(
  /* pointer to CCTrCh info strcuture for which the ref. type is to be
     generated */
  tdsl1_dl_cctrchcfg_cctrch_info_struct_type *cctrch_info);

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
/*===========================================================================
FUNCTION tdsdlcctrch_get_longest_tti_for_t2l_cm_meas

DESCRIPTION
  This function returns the longest TTI among the TrChs in a CCTrCh.
  for searcher get to use

DEPENDENCIES
  None

RETURN VALUE
  tdsl1_tti_enum_type -> TDSL1_TTI_10MS, TDSL1_TTI_20MS, TDSL1_TTI_40MS, or TDSL1_TTI_80MS.

SIDE EFFECTS
  None
===========================================================================*/

tdsl1_tti_enum_type  tdsdlcctrch_get_longest_tti_for_t2l_cm_meas(void);
#endif

/*===========================================================================
FUNCTION tdsl1dec_get_longest_tti

DESCRIPTION
  This function returns the longest TTI among the TrChs in a CCTrCh. The
  CCTrCh table index is passed as function parameter.

DEPENDENCIES
  None

RETURN VALUE
  tdsl1_tti_enum_type -> TDSL1_TTI_10MS, TDSL1_TTI_20MS, TDSL1_TTI_40MS, or TDSL1_TTI_80MS.

SIDE EFFECTS
  None
===========================================================================*/

extern tdsl1_tti_enum_type tdsl1dec_get_longest_tti(
  /* CcTrCh table index in mDSP */
  uint8 cctrch_idx);

/*===========================================================================
FUNCTION tdsl1dec_calc_max_tti//TDS

DESCRIPTION
  This function returns the max TTI in configuration cmd.

DEPENDENCIES
  None

RETURN VALUE
  tdsl1_tti_enum_type -> TDSL1_TTI_10MS, TDSL1_TTI_20MS, L1_TTI_40MS, or TDSL1_TTI_80MS.

SIDE EFFECTS
  None
===========================================================================*/
extern tdsl1_tti_enum_type tdsl1dec_calc_max_tti(tdsl1_dl_cctrchcfg_cctrch_info_struct_type *dl_cctrch_ptr);

/*===========================================================================
FUNCTION tdsl1dec_get_trch_longest_tti

DESCRIPTION
  This function returns the longest TTI among the specific TrChs reference
  type in a CCTrCh. The CCTrCh table index and TrCh reference type is passed
  as function argument. Use ORed constant L1_DL_TRCH_REFTYPE_XXXX_BF defined
  in tdsl1sapcommon.h     to pass desired TrCh ref type bit field

DEPENDENCIES
  None

RETURN VALUE
  tdsl1_tti_enum_type -> TDSL1_TTI_10MS, TDSL1_TTI_20MS, TDSL1_TTI_40MS, or TDSL1_TTI_80MS.
  If all of the TrCh BF is not in this CCTrCh then it returns TDSL1_NUM_TTI as
  invalid TTI type.

SIDE EFFECTS
  None
===========================================================================*/

extern tdsl1_tti_enum_type tdsl1dec_get_trch_longest_tti(
  /* CcTrCh table index in mDSP */
  uint8 cctrch_idx,
  /* TrCh reference type bit field */
  uint8 trch_ref_type_bf);

/*===========================================================================
FUNCTION tdsl1dec_get_shortest_tti

DESCRIPTION
  This function returns the shortest TTI among the TrChs in a CCTrCh. The
  CCTrCh table index is passed as function parameter.

DEPENDENCIES
  None

RETURN VALUE
  tdsl1_tti_enum_type -> TDSL1_TTI_10MS, TDSL1_TTI_20MS, TDSL1_TTI_40MS, or TDSL1_TTI_80MS.

SIDE EFFECTS
  None
===========================================================================*/

extern tdsl1_tti_enum_type tdsl1dec_get_shortest_tti(
  /* CcTrCh table index in mDSP */
  uint8 cctrch_idx);

extern tdsl1_tti_enum_type tdsl1dec_get_trch_idx_tti_val(uint8 cctrch_idx, uint8 trch_idx);
/*===========================================================================
FUNCTION tdsl1dec_cctrch_free

DESCRIPTION
  This function set the status of the CCTrCh table index to available. It
  also sets the status TFCI table index (if any) associated with this CCTrCh
  index to available. When the CCTrCh is removed or modified the currently
  associated CCTrCh table index is freed after the successful completion of
  the action taking place in the demod module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the CCTrCh has the transport channal of the type BCH_INIT or NBCH it
  sets the corresponding flags to FALSE.
===========================================================================*/

extern void tdsl1dec_cctrch_free(
  /* CCTrCh index in mDSP tables passed allocated to CCTrCh */
  uint8 cctrch_index);

/*===========================================================================
FUNCTION tdsl1dec_get_cctrch_index

DESCRIPTION
  This function gets the CCTrCh table entry index associated with CCTrCh Id.
  This function is called by demod block to get the CCTrCh Id for setting up
  or reconfiguring the channels.

DEPENDENCIES
  None

RETURN VALUE
  CCTrCh table index. The possible values are from 0 to 5. Returns 0xFF if
  no such CCTrCh Id exists.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 tdsl1dec_get_cctrch_index(
  /* CCTrCh id passed by RRC for this CCTrCh */
  uint8 cctrch_id);

/*===========================================================================

FUNCTION tdsl1dec_cctrch_setup_done

DESCRIPTION
  This function is a callback function that is to be called when the DMA
  transfer to write the CCTrCh setup related information transfer to mDSP is
  complete. This function sends a local command CCTRCH_SETUP_DONE to L1M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_cctrch_setup_done(
  /* table index of CCTrCh setup */
  uint8 cctrch_table_index
);

/*===========================================================================
FUNCTION  tdsl1dec_auto_bler_meas

DESCRIPTION
  This function is called by L1 auto bler measure timer signal from l1 task.
  THis is used to calculate the bler for various TrCh.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_auto_bler_meas(uint8 cctrch_index);

/*===========================================================================
FUNCTION tdsl1dec_mcalwcdma_dl_process_bmc_handle

DESCRIPTION
  This function processes the DOB handle to calculate the SFN of sequencer
  and also computes if the frame is wanted.

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating if the frame is wanted.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsl1dec_mcalwcdma_dl_process_bmc_handle(
  /* CTCH channel active */
  boolean *ctch_active,
  /* SFN of sequencer */
  uint16 *sfn,
  /* CCTrCh ID from DOB */
  uint8 ctch_cctrch_id,
  /* TrCh ID from DOB */
  uint8 ctch_trch_id,
  /* CFN from DOB */
  uint8 ctch_cfn_value);

/*===========================================================================
FUNCTION tdsl1dec_get_cctrch_index //TDS

DESCRIPTION
  This function gets the CCTrCh table entry index associated with CCTrCh Id.
  This function is called by demod block to get the CCTrCh Id for setting up
  or reconfiguring the channels.

DEPENDENCIES
  None

RETURN VALUE
  CCTrCh table index. The possible values are from 0 to 5. Returns 0xFF if
  no such CCTrCh Id exists.

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1dec_cctrch_update(
  /* CCTrCh id passed by RRC for this CCTrCh */
  uint8 cctrch_index);

#endif /* FEATURE_MCAL_TDSCDMA  */
#endif /* DLDEC_H */

