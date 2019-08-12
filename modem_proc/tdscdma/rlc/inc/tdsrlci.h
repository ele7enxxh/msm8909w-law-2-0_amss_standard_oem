#ifndef TDSRLCI_H
#define TDSRLCI_H
/*===========================================================================
              R L C  I N T E R N A L  H E A D E R  F I L E

DESCRIPTION
  This file contains RLC internal data types and declarations that are shared 
  within RLC layer.
  
Copyright (c) 2002-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/rlc/inc/tdsrlci.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when      who     what, where, why
--------  ---     ---------------------------------------------------------- 
10/13/14  sarao   CFCM changes for RLC
11/23/10  cyx     Remove tdsmvsif.h to clean up l2 interface
10/24/10  guo     Initial release Upmerged WCDMA to TDS
===========================================================================*/


#include "tdscdma_variation.h"
#include "comdef.h"
//#include "tdsrlcul.h"


/* --------------------------------- */
/* PUBLIC APIs of tdsrlci.h             */
/* --------------------------------- */

/*--------------------------------------------------------------------------- 
  Structure to carry the config information.
---------------------------------------------------------------------------*/
typedef enum {

  TDSRLCI_ESTABLISH = 0,                    /* Initialize the protocol         */ 
                                        /* parameters as specified, and    */
                                        /* enter the data transfer ready   */
                                        /* state.                          */
  TDSRLCI_RE_ESTABLISH,                     /* Re-initialize the protocol      */ 
                                        /* parameters as specified.        */
  TDSRLCI_RELEASE,                          /* Reset all the protocol          */
                                        /* parameters, and exit the        */
                                        /* data transfer ready state.      */
  TDSRLCI_MODIFY,                           /* Modify the indicated protocol   */
                                        /* parameters as specified, and    */
                                        /* remain in the data transfer     */
                                        /* ready state.                    */
  TDSRLCI_STOP,                             /* Stop all of RX/TX for UM/AM     */
  TDSRLCI_CONTINUE,                          /* Resume back to normal after STOP*/
  TDSRLCI_NO_ACTION
} tdsrlc_channel_action_cnf_e_type; 

typedef struct {
  tdsrlc_am_config_req_type am_config_req;
  uecomdef_status_e_type result[TDSUE_MAX_UL_AM_DATA_CHANNEL];        
                                        /* Config success or fail         */
} tdsrlci_dl_am_config_req_type;

typedef struct {
  rb_id_type  rb_id; /* uint8 */
  uint8 ul_data_id;                     /* Unique identifier for the uplink
                                           RLC logical channel for 
                                           transporting data PDUs. */
  tdsrlc_channel_action_cnf_e_type action;
  uecomdef_status_e_type result;        /* Config success or fail         */
} tdsrlci_dl_config_cnf_info_type;

typedef struct {
  uint8 nchan;
  tdsrlci_dl_config_cnf_info_type am_cnf[TDSUE_MAX_UL_AM_DATA_CHANNEL];
} tdsrlci_dl_config_cnf_type;


/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

#define TDSMAX_DATA_LOGGING ((uint16)500)

#define TDSRLC_DEBUG_CHANGE_ENDIANNESS(x)  (((x&0xFF000000) >> 24) | ((x&0x00FF0000) >> 8) | ((x&0x0000FF00) << 8) | ((x&0x000000FF) << 24))
/* Global variable used for logging */
extern uint32 tdsrlc_debug_var;

/* Uplink Related debug */
#define   TDSRLC_DBG_PDU_BLDG          0x00000001
#define   TDSRLC_DBG_PDU_BLDG_1        0x00000002
#define   TDSRLC_DBG_CTRL_PDU_BLDG     0x00000004
#define   TDSRLC_DBG_STATUS            0x00000008
#define   TDSRLC_DBG_UL_PP_HW          0x00000010
#define   TDSRLC_DBG_MEM_LEAK          0x00000020
#define   TDSRLC_DBG_UL_TM             0x00000040  
#define   TDSRLC_DBG_ERR_LOG_SUBMIT    0x00000080
#define   TDSRLC_UL_PDU_SIZE           42

#define   TDSRLC_DBG_POLLING           0x00000100
#define   TDSRLC_DBG_PDU_DUMP          0x00000200
#define   TDSRLC_DBG_UL_UM_PDU         0x02000000
#define   TDSRLC_DBG_UL_AM             0x00020000

/* Downlink Related debug */
#define   TDSRLC_DBG_DL_CTRL           0x00010000

#define   TDSRLC_DBG_REASSEMBLY        0x00100000
#define   TDSRLC_DBG_REASSEMBLY_1      0x00200000

/*TCP/IP PDU corruption Debugging*/
#define TDSRLC_DBG_TCP_IP_PDU_CORRUPT  0x00400000
#define TDSRLC_DBG_PDU_SDU_DUMP        0x00800000
#define TDSRLC_DBG_INVALID_SDU         0x00000400
#define TDSRLC_DBG_INVALID_SDU_FATAL   0x00000800

#define   TDSRLC_DBG_DL_UM_PDU         0x01000000
#define   TDSRLC_DBG_DL_PROFILING      0x10000000
#define   TDSRLC_DBG_DL_AM             0x08000000
#define   TDSRLC_DBG_DL_TM             0x04000000  


/* Ciphering related */
#define   TDSRLC_DBG_UL_CIPHERING      0x10000000
#define   TDSRLC_DBG_DL_CIPHERING      0x20000000

#define   TDSRLC_DBG_CORRUPTION      0x80000000
#define   TDSRLC_DBG_RTX_CORRUPTION  0x40000000

#define   TDSRLC_DBG_UL_CTL_BLK_ID_TBL  0x00000400

#define   TDSRLC_DBG_DL_PDU_DUMP       0x00001000

/*--------------------------------------------------------------------------
  Base LC ID for UM UL channels..
--------------------------------------------------------------------------*/
#define TDSRLC_UL_UM_USR_PLANE_START_LC_ID 9

/*---------------------------------------------------------------------------
  AM data PDU
---------------------------------------------------------------------------*/
#define TDSRLC_AM_SN_SIZE                 12
#define TDSRLC_AM_DC_BIT_SIZE              1
#define TDSRLC_AM_DC_BIT_DATA_PDU_MASK    0x8000
#define TDSRLC_AM_POLL_BIT_SIZE            1
#define TDSRLC_AM_POLL_BIT_MASK           0x04
#define TDSRLC_AM_HE_SIZE                  2
#define TDSRLC_AM_PDU_HDR_SIZE_BYTES       2
#define TDSRLC_AM_PDU_HDR_SIZE_BITS        16

#ifdef FEATURE_TDSCDMA_RLC_SPECIAL_HE_VALUE
#define TDSRLC_AM_SPECIAL_HE_MASK         0x02
#endif

#define TDSRLC_MAX_PDU_SIZE               1503

/*---------------------------------------------------------------------------
  AM Status PDU
---------------------------------------------------------------------------*/
#define TDSRLC_AM_STATUS_PDU_TYPE_SIZE     3
#define TDSRLC_AM_PDU_TYPE_SIZE            3
#define TDSRLC_AM_SUFI_TYPE_SIZE           4

/*---------------------------------------------------------------------------
  MRW SUFI
---------------------------------------------------------------------------*/
#define TDSRLC_AM_MRW_LENGTH_SIZE          4
#define TDSRLC_AM_MRW_N_SIZE               4
#define TDSRLC_AM_MRW_N_LENGTH_SIZE        4

/*---------------------------------------------------------------------------
  LIST SUFI
---------------------------------------------------------------------------*/
#define TDSRLC_AM_LIST_LENGTH_SIZE         4
#define TDSRLC_AM_LIST_L_SIZE              4
#define TDSRLC_AM_MAX_LENGTH_IN_LIST_SUFI  15
#define TDSRLC_HDR_E_MASK                0x01

#define TDSRLC_AM_BITMAP_BYTE             8
#define TDSRLC_AM_RLIST_SN_SIZE           12
#define TDSRLC_AM_RLIST_CW_SIZE           4

/*---------------------------------------------------------------------------
  AM RESET PDU
---------------------------------------------------------------------------*/
#define TDSRLC_AM_RESET_PDU_SIZE          24
#define TDSRLC_AM_RSN_SIZE                 1
#define TDSRLC_AM_R1_SIZE                  3
#define TDSRLC_AM_HFN_SIZE                20

/*---------------------------------------------------------------------------
  Un acknowledge Mode definitions
---------------------------------------------------------------------------*/
#define TDSRLC_UM_PDU_HDR_SIZE_BYTES     1

/*---------------------------------------------------------------------------
    Bits/bytes/word related.
---------------------------------------------------------------------------*/
#define   TDSCONVERT_BITS_TO_BYTES(x)      (((x) + 0x7) >> 3)
#define   TDSCONVERT_WORDS_TO_BYTES(x)     ((x) << 2)
#define   TDSCONVERT_BITS_TO_WORDS(x)      (((x) + 0x1F) >> 5)
#define   TDSCONVERT_BYTES_TO_WORDS(x)     (((x) + 0x3) >> 2)

/*---------------------------------------------------------------------------
  Length Indicator related definitions
---------------------------------------------------------------------------*/
#define TDSRLC_LI_SHORT                  1
#define TDSRLC_LI_LONG                   2

/* Reserved LI as per Rel-5. PDU shall be discarded */
#define TDSRLC_LI_RESERVED_7_BIT         0x7D
#define TDSRLC_LI_RESERVED_15_BIT        0x7FFD

#define TDSRLC_LI_PIGGYBACK_7_BIT        0x7E
#define TDSRLC_LI_PIGGYBACK_15_BIT       0x7FFE


#define TDSRLC_LI_EXACT                  0x00
#define TDSRLC_LI_OCTET_LESS             0x7FFB

#define TDSRLC_LI_START_SDU_7_BIT        0x7C
#define TDSRLC_LI_START_SDU_15_BIT       0x7FFC

#define TDSRLC_LI_PADDING_7_BIT          0x7F
#define TDSRLC_LI_PADDING_15_BIT         0x7FFF

#define TDSLI_SDU_FIT_7_BIT              0x7D
#define TDSLI_SDU_FIT_15_BIT             0x7FFD

#define TDSRLC_BITMAP_ARRAY_SIZE          128

/* Sets the status for sequence number sn, in bitmap array nak_bitmap */
#define  TDSRLCI_SET_AM_SN_STATUS_OPT_BITMAP( nak_bitmap, sn, modulus) \
      ( nak_bitmap[(sn & ( modulus - 1 ) ) >> 4]\
           = ( nak_bitmap[(sn & (modulus - 1) ) >> 4] | \
          ( 0x8000 >> ( sn & 0x000F ) ) ) )

/* resets the status for sequence number sn, in bitmap array nak_bitmap */
#define  TDSRLCI_RESET_AM_SN_STATUS_OPT_BITMAP( nak_bitmap, sn, modulus ) \
         ( nak_bitmap[(sn & ( modulus - 1 ) ) >> 4]\
           = ( nak_bitmap[(sn & ( modulus - 1 ) ) >> 4] & \
           ( ~( 0x8000 >> ( ( sn & 0x000F ) ) ) ) ) )

/* Gets the status for sequence number sn, in bitmap array nak_bitmap */
#define  TDSRLCI_GET_AM_STATUS_OPT_BITMAP( nak_bitmap, sn, modulus) \
         ( ( ( nak_bitmap[(sn & ( modulus - 1 ) ) >> 4] & \
           ( 0x8000 >> ( ( sn & 0x000F ) ) ) ) > 0 ) ? TRUE : FALSE )

/* Resets the bitmap array passed, nak_bitmap */
#define  TDSRLCI_RESET_AM_NAK_STATUS( nak_bitmap ) \
         ( memset( nak_bitmap, 0, 128 * sizeof( uint16 ) ) )

/* Resets the status for sequence number from fsn to lsn, in bitmap array */
/* nak_bitmap, note that fsn and lsn should be at the same index in array */
#define  TDSRLCI_RESET_AM_SN_RANGE_NAK_STATUS( nak_bitmap, fsn, lsn, modulus) \
         ( nak_bitmap[(fsn & ( modulus - 1 ) ) >> 4]\
           = ( nak_bitmap[(fsn & ( modulus - 1 ) ) >> 4] & \
           ( ( 0xFFFF << ( 16 - ( fsn  & 0x000F ) ) ) | \
           ( 0xFFFF >> ( ( lsn & 0x000F ) + 1 ) ) ) ) )

/* Sets the status for sequence number from fsn to lsn, in bitmap array   */
/* nak_bitmap, note that fsn and lsn should be at the same index in array */
#define  TDSRLCI_SET_AM_SN_RANGE_NAK_STATUS( nak_bitmap, fsn, lsn, modulus) \
         ( nak_bitmap[( fsn & ( modulus - 1 ) ) >> 4]\
           = ( nak_bitmap[(fsn & ( modulus - 1 ) ) >> 4] | \
           ( ~( ( 0xFFFF << ( 16 - ( fsn & 0x000F ) ) ) | \
           ( 0xFFFF >> ( ( lsn & 0x000F ) + 1 ) ) ) ) ) )

/* Gets the status for sequence numbers from fsn to lsn as a 16 bit value */
/* note that fsn and lsn should be at the same index in array             */
#define  TDSRLCI_GET_AM_SN_RANGE_NAK_STATUS( nak_bitmap, fsn, lsn, modulus) \
         ( nak_bitmap[(fsn & ( modulus - 1 ) ) >> 4] & \
           ( ~( ( 0xFFFF << ( 16 - ( fsn & 0x000F ) ) ) | \
          ( 0xFFFF >> ( ( lsn & 0x000F ) + 1 ) ) ) ) )

/*Return the last sequence number in the array element where sn is,      */
/* for eg. if sn is 0 the las sn which can be prsent in one 16 bit value */
/* (in an array of 16 bit values) is 15*/
#define  TDSRLCI_GET_AM_COUNT_STATUS_OPT_BITMAP( sn ) \
         ( 0x000F - ( sn & 0x000F ) )

/*---------------------------------------------------------------------------
  Structure to hold nak/rtx status in bitmap array.
  This structure type is used both in UL and DL.
---------------------------------------------------------------------------*/
typedef struct
{

  uint16     status_bitmap[TDSRLC_BITMAP_ARRAY_SIZE]; /* array holding nak/rtx*/
                                                   /* status.             */
  uint16     next_sn;                              /* sn from where to start*/
                                                   /* adding naks or from  */
                                                   /* where to start rtx   */
                                                   /* PDUs in case we left */
                                                   /* the process in b/w   */
  boolean    next_sn_valid;                        /*valid flag for next_sn*/

}tdsrlc_opt_status_type;

/*---------------------------------------------------------------------------
  Structure to hold a 16-bit value as individual bits.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 bit0  : 1;
  uint8 bit1  : 1;
  uint8 bit2  : 1;
  uint8 bit3  : 1;
  uint8 bit4  : 1;
  uint8 bit5  : 1;
  uint8 bit6  : 1;
  uint8 bit7  : 1;
  uint8 bit8  : 1;
  uint8 bit9  : 1;
  uint8 bit10 : 1;
  uint8 bit11 : 1;
  uint8 bit12 : 1;
  uint8 bit13 : 1; 
  uint8 bit14 : 1;
  uint8 bit15 : 1;

}tdsrlc_16bit_map_type;

/*---------------------------------------------------------------------------
  Union to hold a 16-bit value as a 16-bit uint as well as individual 16bits.
  This is used in calculating number of 1's/0's in the 16-bit value in this
  union.
---------------------------------------------------------------------------*/
typedef union
{
  tdsrlc_16bit_map_type   bit_map;
  uint16               var;
}tdsrlc_bitmap_type;

#ifdef FEATURE_TDSCDMA_RLC_QXDM_LOGGING
/*---------------------------------------------------------------------------
  Structure for UL\DL AM statistic
---------------------------------------------------------------------------*/
typedef struct
{

  uint16     statistic_timer_cnt;            /* timer count*/
 
  uint16     statistic_timer_value;          /* timer value*/
}tdsrlc_statistic_timer_type;

extern tdsrlc_statistic_timer_type ul_statistic_timer;
extern tdsrlc_statistic_timer_type dl_statistic_timer;
#endif

/* Table to store the RLC-ids during RESET-status-timer messages */
extern rlc_lc_id_type  tdsrlc_rst_satus_tmr_tbl[TDSUE_MAX_AM_ENTITY];

/*===========================================================================

FUNCTION tdsrlc_update_nak_status

DESCRIPTION
    - This function sets/resets the nak status for a range of pdus with sn 
      starting with (fsn) and ending with sn (lsn) (fsn and lsn inclusive).   

DEPENDENCIES
  None.

INPUT ARGUMENTS
  nak_bitmap - pointer to bitmap array
  fsn        - fist sequence number for which status is to be updated.
  lsn        - last sequence number for which status is to be updated.
  update     - pdu status 
               FALSE - ACK
               TRUE  - NAK
  mod        - modulus to use..

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlc_update_nak_status
(
  uint16                *nak_bitmap,
  uint16                fsn,
  uint16                lsn,
  boolean               update,
  uint16                mod
);
/*===========================================================================

FUNCTION tdsrlc_get_num_set

DESCRIPTION
    - This function gets the nak status indicating whether we have any naks
      pending or not.

DEPENDENCIES
  None.

INPUT ARGUMENTS
  nak_bitmap - pointer to bitmap array
  fsn        - fist sequence number from where to count.
  lsn        - last sequence number till which to count.
  mod        - modulus to use..

RETURN VALUE
  number of 1's in bitmap array

SIDE EFFECTS
  None.

===========================================================================*/
uint16 tdsrlc_get_num_set
(
  uint16                *nak_bitmap,
  uint16                fsn,
  uint16                lsn,
  uint16                mod
);

/*===========================================================================

FUNCTION tdsrlc_get_retx_bitmap

DESCRIPTION
    - This function gets the retx bitmap array maintained on UL.

DEPENDENCIES
  None.

INPUT ARGUMENTS
  lc_id      - logical channel id.

RETURN VALUE
  retx bitmap - pointer to retx bitmap array.

SIDE EFFECTS
  None.

===========================================================================*/
tdsrlc_opt_status_type *tdsrlc_get_retx_bitmap
(
  byte                  lc_id  
);


typedef enum
{
  TDSRLC_AM_TIMER_UNDEF_STATUS = 0, /* Timer doesnt exist */
  TDSRLC_AM_TIMER_RUNNING,     /* Timer enabled, is running */
  TDSRLC_AM_TIMER_SUSPEND,     /* Timer suspended, LC just re-config/ RESET */
  TDSRLC_AM_TIMER_STOP         /* Timer stopped, by RRC*/
} tdsrlc_tmr_status_type;

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/
#define TDSINIT_CTL_BLK_ID       0xFF

/* Initial value of rlc_id for 10ms Timer sig control block */
#define TDSINIT_TIMER_BLK        0xFF

#define TDSRLC_UM_MODULUS        128
#define TDSRLC_UM_MOD_MASK       0x7F
#define TDSRLC_UM_HALF_MOD       64
#define TDSRLC_AM_MODULUS        4096
#define TDSRLC_AM_HALF_MOD       2048
#define TDSRLC_AM_MOD_MASK       0x0FFF


/* RESET, RESET ACK PDU size  */
#define TDSRLC_RESET_PDU_SIZE    4

#define TDSRLC_STATUS_PDU_HEADER_SIZE  4

/* MRW/MRW_ACK SUFI size in bits */
#define TDSMRW_SUFI_SIZE         24
#define TDSMRW_TDSACK_SUFI_SIZE     20

#define TDSNUM_NAK_BUF 512

/* TX_WINDOW_SIZE SUFI  */
#define TDSWINDOW_SUFI_SIZE      16
/* NAK SUFI size in a PDU in bits*/
#define TDSNAK_LIST_SUFI_SIZE      24  /* 8-bits(LIST-Type, #SL-pairs) + 16-bits(SN,Length) */
#define TDSNAK_LIST_SUFI_SL_SIZE   16 /* 16-bits(SN,Length) */
#define TDSNAK_LIST_SUFI_HDR_SIZE  8  /* 8-bits(LIST-Type, #SL-pairs) */

#define TDSNAK_BITMAP_SUFI_SIZE  28
#define TDSNAK_RLIST_SUFI_SIZE   24

#define TDSACK_SUFI_SIZE         16
#define TDSNO_MORE_SUFI_SIZE     4

#define TDSRLC_UL_TIMER_BASE     20
#define TDSRLC_DL_TIMER_BASE TDSRLC_UL_TIMER_BASE

#define TDSRLC_AM_ACK_LSN_SIZE   12
#define TDSRLC_AM_WINDOW_SN_SIZE 12

#define TDSRLC_INVALID_SN 0xFFFF

#define TDSINVALID_LI                0xFFFF

#define TDSLI_SDU_ONE_OCTECT_SHORT   0x7FFA

#define TDSLI_SDU_MIDDLE_SEG_7_BIT   0x7E
#define TDSLI_SDU_MIDDLE_SEG_15_BIT  0x7FFE


#ifdef FEATURE_TDSCDMA_USE_RLC_BACKUP_TIMER
  #define TDSRLC_BACKUP_TIMER_CNT  5
#endif
/* retreives seven LSBs of x   */
#define TDSRLC_AM_GET_VT_DAT(x)    ((x) & 0x7F)
/* Increments x by 1   */
#define TDSRLC_AM_INC_VT_DAT(x)    (++(x))

  #define TDSRLC_AM_SET_PDU_NAK(x)  ((x) |= 0x80)
  #define TDSRLC_AM_PDU_NAKD(x)      ((x) & 0x80)
  #define TDSRLC_AM_CLEAR_PDU_NAK(x) ((x) &= 0x7F)

#ifdef FEATURE_TDSCDMA_RLC_QXDM_LOGGING
/* This timer base is used to send log packets to qxdm for logging of Am   */
/* statistics information. Currently set to 3 seconds                      */
  #define TDSRLC_AM_LOG_TIMER_BASE 3000
#endif

#ifdef FEATURE_TDSCDMA_SDU_LIFETIME_VALIDATION
#define TDSRLC_AM_INVALID_TIME_FOR_SDU      0xFFFFFFFF
#endif

typedef enum {
  TDSRLCI_CLOSE_RX_WIN = 0x00,
  TDSRLCI_OPEN_RX_WIN = 0x01
} tdsrlci_win_op_e_type;

/*---------------------------------------------------------------------------
  Enumerated type that defines the RLC Ciphering mode.
---------------------------------------------------------------------------*/
typedef enum {
  TDSNO_CIPHER = 0,
  TDSCIPHER_WAIT,
  TDSCIPHER_MOD,
  TDSCIPHER_ON
} tdscipher_mode_e_type;

/*--------------------------------------------------------------------------- 
  Typedef for storing the ciphering parameters for a logical channel.
---------------------------------------------------------------------------*/
typedef struct {
  tdscipher_mode_e_type cipher_mode;      /* Ciphering mode                   */
  uecomdef_umts_cipher_algo_e_type cipher_algo;  /* Ciphering algorithn.(UEA0, UEA1) */
  uecomdef_umts_cipher_algo_e_type cipher_algo_new;  /* Ciphering algorithn.         */
  uecomdef_umts_cipher_algo_e_type cipher_algo_new1; /* Ciphering algorithn.         */
  uecomdef_umts_cipher_algo_e_type cipher_algo_old;  /* Ciphering algorithn.         */
  uecomdef_umts_cipher_algo_e_type cipher_algo_old1; /* Ciphering algorithn.         */
  uint32  cipher_key_id;               /* Ciphering Key Id                 */
  uint32  cipher_key_id_new;           /* Ciphering Key Id                 */
  uint32  cipher_key_id_new1;          /* Ciphering Key Id for 2nd pending */
                                       /* session                          */
  uint32  cipher_key_id_old;           /* Ciphering Key Id                 */
  uint32  cipher_key_id_old1;          /* Ciphering Key Id                 */
  uint32  hfn;                         /* Hyperframe number                */
  uint32  hfn_new;                     /* Hyperframe number                */
  uint32  hfn_new1;                    /* Hyperframe number for 2nd        */
                                       /* pending session.                 */
  uint32  hfn_old;                     /* Hyperframe number                */
  uint32  hfn_old1;                    /* Hyperframe number                */
  uint32  count;                       /* Count - Ciphering HFN + RLC      */
                                       /* sequence number                  */
  uint32  count_old;                   /* Count - Ciphering HFN + RLC      */
                                       /* sequence number                  */
  uint32  count_old1;                   /* Count - Ciphering HFN + RLC      */
                                       /* sequence number                  */
  uint16  rb_id;                       /* Radio Bearer Id                  */
  uint16  act_sn;                      /* SN to start ciphering            */
  uint16  act_sn_new;                  /* act_sn for 2nd pending session   */
  uint16  act_sn_new1;                 /* act_sn for 2nd pending session   */
  uint16  act_sn_old;                  /* old act_sn                       */
  uint16  act_sn_old1;                  /* old act_sn                      */
  boolean cipher_all;                  /* TRUE - cipher all PDU tx'd.      */
  boolean act_old_cfg;                 /* TRUE - old config activated.     */
  boolean act_old_cfg1;                /* TRUE - old1 config activated.    */
  boolean new_cipher_on;               /* TRUE - Just transfer to CIPHER_ON*/
                                       /* w/o receiving any data yet.      */ 
  boolean two_pending;                 /* Two ciphering pending sessions   */
#ifdef FEATURE_TDSCDMA_FUTURE_CIPHERING_ACTIVATION
  boolean wrap_valid_new;
  boolean wrap_valid_new1;                      
#endif
} tdsrlci_ciphering_type;


#define TDSRLC_MAX_SRB_ID 4


#ifdef FEATURE_TDSCDMA_RLC_QXDM_LOGGING

typedef enum{
  TDSRLC_ERR_OUTSIDE_WINDOW = 0,    /* PDU received is putside rlc window */
  TDSRLC_ERR_TIMER_DISCARD,          /* SDU-DISCARD-TIMER expired */
  TDSRLC_ERR_INVALID_LI,             /* PDU with Invalid Length Indicator */
  TDSRLC_ERR_DUPLICATE_PDU,          /* Duplicate PDU is received */
  TDSRLC_ERR_BUFFER_FULL,            /* RLC buffer is full */
  TDSRLC_ERR_INVALID_PDU_SIZE        /* PDU with invalid size is received */
} tdsrlc_err_reason_type;

typedef enum  {
  TDSDOWNLINK_FIXED_SIZE,
  TDSDOWNLINK_FLEXIBLE_SIZE,
  TDSUPLINK_FIXED_SIZE,
  TDSUPLINK_FLEXIBLE_SIZE
}tdsrlci_direction_and_rlc_e_type;

/*------------------------------------------------------------------- 
  typedef for storing a structure, containing data when rlc config
  event is generated. rlc config event is used in automation, and is
  generated for DTCH UM and AM channels, whenever the channels are
  established, reconfiged and modified. Right now the event is generated
  only for donwlink only.
  -------------------------------------------------------------------*/

typedef struct  {
  uint8 rlc_id;                      /*rlc id forwhich the event is generated*/
  uint8 direction_and_rlc_type;      /*This denotes the UL or DL rlc, with 
                                       fixed or flexible rlc pdu size*/
  uint16 size;                       /*for fixed rlc pdu size, the size var
                                       denotes the pdu size. for flexible pdu
                                       siize this denotes the length of LI*/
}tdsrlci_event_tds_rlc_config_type;

#endif

/*===========================================================================

MACRO TDSRLCI_SEQ_INC

DESCRIPTION
  This macro increases the passed in sequence number by 1 and modulo 
  TDSRLC_UM_MODULUS/TDSRLC_AM_MODULUS according to the mode.

PARAMETERS
  seq : Sequence numbers.
  mode : UM or AM

DEPENDENCIES
  None

RETURN VALUE
  sequence number.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_SEQ_INC(seq,mode) ((mode == UE_MODE_UNACKNOWLEDGED ) ?  \
                           (((seq)+1) & TDSRLC_UM_MOD_MASK) :        \
                           (((seq)+1) & TDSRLC_AM_MOD_MASK))

/*===========================================================================

MACRO TDSRLCI_AM_SEQ_INC

DESCRIPTION
  This macro increases the passed in sequence number by 1 and modulo 
 TDSRLC_AM_MODULUS according to the mode.

PARAMETERS
  seq : Sequence numbers.
  mode :AM

DEPENDENCIES
  None

RETURN VALUE
  sequence number.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_AM_SEQ_INC(seq) (((seq)+1) & TDSRLC_AM_MOD_MASK)

/*===========================================================================

MACRO TDSRLCI_UM_SEQ_INC

DESCRIPTION
  This macro increases the passed in sequence number by 1 and modulo 
 TDSRLC_UM_MODULUS according to the mode.

PARAMETERS
  seq : Sequence numbers.
  mode :AM

DEPENDENCIES
  None

RETURN VALUE
  sequence number.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_UM_SEQ_INC(seq) (((seq)+1) & TDSRLC_UM_MOD_MASK)

/*===========================================================================

MACRO TDSRLCI_SEQ_DEC

DESCRIPTION
  This macro decreases the passed in sequence number by 1 and modulo 
  TDSRLC_UM_MODULUS/TDSRLC_AM_MODULUS according to the mode.

PARAMETERS
  seq : Sequence numbers.
  mode : UM or AM

DEPENDENCIES
  None

RETURN VALUE
  sequence number.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_SEQ_DEC(seq,mode) ((mode == UE_MODE_UNACKNOWLEDGED) ? \
                           (((seq)+127) & TDSRLC_UM_MOD_MASK) :    \
                           (((seq)+4095) & TDSRLC_AM_MOD_MASK)) 

/*===========================================================================

MACRO TDSRLCI_AM_SEQ_DEC

DESCRIPTION
  This macro decreases the passed in sequence number by 1 and modulo 
  TDSRLC_AM_MODULUS according to the mode.

PARAMETERS
  seq : Sequence numbers.
  mode : AM

DEPENDENCIES
  None

RETURN VALUE
  sequence number.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_AM_SEQ_DEC(seq) (((seq) + 4095) & TDSRLC_AM_MOD_MASK)

/*===========================================================================

MACRO TDSRLCI_UM_SEQ_DEC

DESCRIPTION
  This macro decreases the passed in sequence number by 1 and modulo 
  TDSRLC_UM_MODULUS according to the mode.

PARAMETERS
  seq : Sequence numbers.
  mode : UM

DEPENDENCIES
  None

RETURN VALUE
  sequence number.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_UM_SEQ_DEC(seq) (((seq) + 127) & TDSRLC_UM_MOD_MASK) 

/*===========================================================================

MACRO TDSRLCI_SEQ_GE

DESCRIPTION
  This macro will compare the 2 passed sequence number parameters and 
  return TRUE if the 1st argument is Greater than or Equal to the 2nd 
  argument (modulo the seq_modulus/2), where seq_modulus is the 3rd argument

PARAMETERS
  seq1, seq2: Sequence numbers for comparison.
  seq_modulus: 128 for UM and 4096 for AM.
  All the passed arguments must be 32bit words.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if seq_1 >= seq_2 modulo seq_modulus, else FALSE

SIDE EFFECTS
  None
===========================================================================*/

#define TDSRLCI_SEQ_GE( seq1, seq2, seq_modulus) \
  ((((seq1) - (seq2)) & (seq_modulus >> 1)) == 0)

/*===========================================================================

MACRO TDSRLCI_SEQ_GT

DESCRIPTION
  This macro will compare the 2 passed sequence number parameters and 
  return TRUE if the 1st argument is Greater than the 2nd argument (modulo 
  input seq_modulus/2).

PARAMETERS
  seq1, seq2: Sequence numbers for comparison, and modulo to use.

DEPENDENCIES
  The passed arguments must be words

RETURN VALUE
  TRUE if seq_1 > seq_2 modulo seq_modulus, else FALSE

SIDE EFFECTS
  None
===========================================================================*/

#define TDSRLCI_SEQ_GT( seq1, seq2, seq_modulus) \
  (((seq1) != (seq2)) && ((((seq1) - (seq2)) & (seq_modulus >> 1)) == 0))
  
/*===========================================================================

MACRO TDSRLCI_SEQ_DIFF

DESCRIPTION
  This macro finds the difference between two sequence numbers.

PARAMETERS
  seq1, seq2: Sequence numbers for comparison, 
  seq_modulus: Modulo to use.

DEPENDENCIES
  This macro assumes that seq1 is greater than seq2 (including sequence 
  modulus).
  
RETURN VALUE
  Absolute difference of the two sequence numbers.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_SEQ_DIFF(seq1, seq2, seq_modulus) \
((seq1 >= seq2) ?\
 (seq1 - seq2) :\
 ((seq_modulus - seq2) + seq1))

#define TDSRLCI_HFN_INC(tdshfn) ((tdshfn+1) & 0xfffff)
#define TDSRLCI_HFN_DEC(tdshfn) ((tdshfn+0xfffff) & 0xfffff)
#define TDSRLCI_HFN_GT(hfn1, hfn2) \
  (((hfn1) != (hfn2)) && ((((hfn1) - (hfn2)) & 0x80000) == 0))
  
/*===========================================================================

MACRO TDSRLCI_Q_GET_NEXT_ENTRY

DESCRIPTION
  Get the next entry from the Queue, given the current entry.

PARAMETERS
  q_ptr = Pointer to Queue.
  entry_ptr = Pointer to current entry.

DEPENDENCIES
  
RETURN VALUE
  Pointer to dsm item chain.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_Q_GET_NEXT_ENTRY(q_ptr, entry_ptr) \
    ((dsm_item_type *)q_next(q_ptr, &(entry_ptr->link)))

/*===========================================================================

MACRO TDSRLCI_Q_INSERT

DESCRIPTION
  Insert an item into a Queue.

PARAMETERS
  reseq_q_ptr: Pointer to the Queue.
  new_link: link of the item to insert
  old_link: Link of item already in Queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_Q_NO_SELF_QPTR
#define TDSRLCI_Q_INSERT(reseq_q_ptr, new_link, old_link) \
    q_insert(reseq_q_ptr, new_link, old_link);
#else
#define TDSRLCI_Q_INSERT(reseq_q_ptr, new_link, old_link) \
    q_insert(new_link, old_link);
#endif
    

/*===========================================================================

MACRO TDSRLCI_Q_DELETE

DESCRIPTION
  Delete an item from a Queue.

PARAMETERS
  reseq_q_ptr: Pointer to the Queue.
  item_ptr : Pointer to item to delete.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_Q_NO_SELF_QPTR
#define TDSRLCI_Q_DELETE(q_ptr, item_ptr) \
{\
  item_ptr->dup_ptr = NULL;\
  q_delete(q_ptr, &(item_ptr->link));\
}
#else
#define TDSRLCI_Q_DELETE(q_ptr, item_ptr) \
{\
  item_ptr->dup_ptr = NULL;\
  q_delete(&(item_ptr->link));\
}
#endif


/*===========================================================================

MACRO TDSRLCI_Q_GET

DESCRIPTION
  Remove an item from the head of the Queue.

PARAMETERS
  q_ptr : Pointer to the Queue.
  link : Link

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the dsm item.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_Q_GET(q_ptr) \
  (dsm_item_type *)(q_get(q_ptr))  


/*===========================================================================

MACRO TDSRLCI_Q_CHECK

DESCRIPTION
   Get a pointer to the first item in the Queue

PARAMETERS
  reseq_q_ptr: Pointer to the Queue.

DEPENDENCIES
  This MACRO is used only for DSM items.

RETURN VALUE
  Pointer to the dsm item.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_Q_CHECK(q_ptr) \
  (dsm_item_type *)(q_check(q_ptr))
    
/*===========================================================================

MACRO TDSRLCI_Q_LAST_CHECK

DESCRIPTION
   Get a pointer to the last item in the Queue

PARAMETERS
  q_ptr: Pointer to the Queue. The queue contains items of type q_type

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the q_type.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_Q_LAST_CHECK(q_ptr) \
  (q_last_check(q_ptr))

/*===========================================================================

MACRO TDSRLCI_Q_PUT
DESCRIPTION
   Insert an item at the tail of a Queue

PARAMETERS
  reseq_q_ptr: Pointer to the Queue.
  item_ptr : Item to insert

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#define TDSRLCI_Q_PUT(q_ptr, new_link) \
   (q_put(q_ptr, new_link))
    

/*===========================================================================
**                  TYPES DECLARATION
**=========================================================================*/


typedef enum
{
#ifdef FEATURE_TDSCDMA_CPU_BASED_FLOW_CONTROL
  TDSRLC_DL_FC_ENTITY_CFCM,
#endif
  TDSRLC_DL_FC_ENTITY_PS_WM,
  TDSRLC_DL_FC_ENTITY_MAX  
}tdsrlc_dl_multi_client_fc_entity_type;

typedef enum
{
#ifdef FEATURE_TDSCDMA_CPU_BASED_FLOW_CONTROL
  TDSRLC_UL_FC_ENTITY_CFCM,
#endif
  TDSRLC_UL_FC_ENTITY_MAX  
}tdsrlc_ul_multi_client_fc_entity_type;

#define TDSNUM_MAX_FC_ENTITY MAX(TDSRLC_DL_FC_ENTITY_MAX, TDSRLC_UL_FC_ENTITY_MAX)+1
typedef uint16 (*tdsstate_to_entity_flow_function_type)(uint16, uint16);
typedef void (*tdsoutput_flow_to_output_trigger_function_type) (uint16, void*);

typedef struct{
  uint16 state[TDSNUM_MAX_FC_ENTITY];
  uint16 entity_flow[TDSNUM_MAX_FC_ENTITY];
  tdsstate_to_entity_flow_function_type state_to_entity_function_array[TDSNUM_MAX_FC_ENTITY];
  uint16 num_fc_entity;
  tdsoutput_flow_to_output_trigger_function_type output_flow_to_output_trigger_function;
  uint16 configured_max_flow;
  uint16 output_flow;
  uint16 prev_output_flow;
  void *fc_object;
  rex_crit_sect_type *fc_semaphore;
}tdsmulticlient_fc_arbitarator_type;




/*--------------------------------------------------------------------------- 
  SUFI type -- Structure of a Super-Filed in status PDU.
---------------------------------------------------------------------------*/
typedef enum{
  TDSRLCI_STATUS_PDU = 0x00,
  TDSRLCI_RESET_PDU,
  TDSRLCI_RESET_ACK_PDU,
  TDSRLCI_RESERVED_PDU
} tdsrlci_pdu_e_type;

/*--------------------------------------------------------------------------- 
  SUFI type -- Structure of a Super-Filed in status PDU.
---------------------------------------------------------------------------*/
typedef enum{
  TDSRLCI_SUFI_NO_MORE = 0x00,
  TDSRLCI_SUFI_WINDOW,
  TDSRLCI_SUFI_ACK,
  TDSRLCI_SUFI_LIST,
  TDSRLCI_SUFI_BITMAP,
  TDSRLCI_SUFI_RLIST,
  TDSRLCI_SUFI_MRW,
  TDSRLCI_SUFI_MRW_ACK,
#if defined (FEATURE_TDSCDMA_HSDPA_MAC_EHS)
  TDSRLCI_POLL_SUFI,
#endif
  TDSRLCI_SUFI_RESERVED
} tdsrlci_sufi_e_type;

/*--------------------------------------------------------------------------- 
  NAK type -- List, Bitmap and Relation List. 
---------------------------------------------------------------------------*/
typedef enum{
  TDSRLCI_NAK_LIST_TYPE = 0,
  TDSRLCI_NAK_BITMAP_TYPE,
  TDSRLCI_NAK_RLIST_TYPE
} tdsrlci_nak_e_type;


/*--------------------------------------------------------------------------- 
  Data type used for the missing PDU list for RLC as well as the retransmit 
  request list from the peer entity. There is only one free missing/require 
  list type queue for all sessions to share. Every sessions will take 
  elements from this free-shred queue for their nak_q and rtx_q. 
---------------------------------------------------------------------------*/
typedef struct {
  q_link_type link;
  uint16 sn;                        /* Missing Sequence number.            */
  tdsrlci_nak_e_type nak_type;         /* NAK TYPE                            */
  union {
    uint8 distance;                  /* number of consecutive PDUs missing */
    uint8 bitmap;                    /* NAK BITMAP                         */
    uint16 rlist;                    /* code word for relation list        */
  } nak;
} tdsrlci_missing_list_type;

/*--------------------------------------------------------------------------- 
  Move Receiving Window Queue type
---------------------------------------------------------------------------*/
typedef struct 
{
  q_link_type link;
  uint16 mrw_sn;                          /* MRW sequence #           */
  uint8  mrw_n;                           /* n_length for MRW         */
  uint8  vt_mrw;                          /* VT(MRW) - # of MRW sent */
} tdsrlci_mrwq_type;

/*---------------------------------------------------------------------------
  using to handle bitmap issue when inter-rnc ho
---------------------------------------------------------------------------*/
typedef struct {
  boolean    inter_rnc_ho_start;               /*inter-rnc ho start*/
  boolean    inter_rnc_ho_end;                 /*inter-rnc ho end*/
}tdsl2_bitmap_specail_handle_type;

extern tdsl2_bitmap_specail_handle_type Bitmap_specail_handle;

#define TDSRLC_SET_DATA_ACTIVITY_FLAG(dl_data_id, user_plane_id) ((void)0)
/*===========================================================================

FUNCTION RLCI_READ_RX_STATE

DESCRIPTION
  Read the RX state(VR_R) from the Down-link side.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint16 tdsrlci_read_rx_state
(
  rlc_lc_id_type lc_id       /* Logical channel ID for data channel  */
);


/*===========================================================================

FUNCTION RLCI_READ_VR_H_RX_STATE

DESCRIPTION
  Read the RX state(VR_H) from the Down-link side.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

uint16 tdsrlci_read_vr_h_rx_state
(
  rlc_lc_id_type lc_id
);
/*===========================================================================

FUNCTION RLCI_GET_VR_H_RX_STATE

DESCRIPTION
  Read the RX state(VR_H) from the Down-link side.
  
DEPENDENCIES

INPUT ARGUMENTS
  lc_id      - logical channel id.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

uint16 tdsrlci_get_vr_h_rx_state
(
  rlc_lc_id_type lc_id
);

/*===========================================================================

FUNCTION RLCI_GET_PREV_DL_VR_H_STATUS

DESCRIPTION
  Read the RX state(dl_vr_h_status) from the Down-link side.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

uint16 tdsrlci_get_dl_vr_h_status
(
  rlc_lc_id_type lc_id
);
/*===========================================================================

FUNCTION RLCI_READ_VR_H_RX_STATE

DESCRIPTION
  Update report_vr_h, which is the last sn to be used for ACK SUFI.

DEPENDENCIES
NONE

INPUT ARGUMENTS
  lc_id      - logical channel id.
  vr_h       - report_vr_h is updated with this value.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_update_vr_h_rx_state
(
  byte    lc_id,
  uint16  vr_h
); 
/*===========================================================================

FUNCTION TDSRLCI_RESET_STATUS_PROHIBIT_CNT

DESCRIPTION
 Resets status prohibit count
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_reset_status_prohibit_cnt
(
  rlc_lc_id_type lc_id
);

/*===========================================================================

FUNCTION TDSRLCI_INITIATE_RESET_FOR_LOWER_DL_HFN

DESCRIPTION
  Trigger UL RESET if DL RESET was received with lower HFN.
  
DEPENDENCIES

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void tdsrlci_initiate_reset_for_lower_dl_hfn
(
  rlc_lc_id_type lc_id
);
/*===========================================================================

FUNCTION RLCI_UPDATE_ACK_STATUS

DESCRIPTION
  The Downlink received an ACK status from the peer side. Update the VT_A
  and VT_MS in Uplink.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_update_ack_status
(
  rlc_lc_id_type lc_id,            /* Logical channel ID for data channel  */
  uint16 ack_sn                    /* Ack SN received from ACK SUFI        */
);

/*===========================================================================

FUNCTION RLCI_RX_STATE_CHANGED

DESCRIPTION
  The Downlink side has its receive state (VR_R) changed. The Uplink needs
  to send a PDU ACK status report when polling from the peer presents.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_rx_status_changed
(
  rlc_lc_id_type lc_id,            /* Logical channel ID for data channel  */
  boolean status
);

/*===========================================================================

FUNCTION RLCI_UPDATE_RETXQ_SEMA

DESCRIPTION
  This function LOCKs/ unlocks ReTx queue semaphore

DEPENDENCIES
  None.

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_update_retxq_sema(rlc_lc_id_type rlc_id, boolean status);

/*===========================================================================

FUNCTION RLCI_GET_RETX_PDU_LEN

DESCRIPTION
  Give the given ReTx PDU length.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint16 tdsrlci_get_retx_pdu_len
(
  rlc_lc_id_type lc_id,     /* Logical channel ID for data channel  */
  uint16 sn                 /* PDU Sequencial number                */
);

/*===========================================================================

FUNCTION RLCI_GET_MRW_Q_ITEM

DESCRIPTION
  Gets the last item in the MRW Q
  
DEPENDENCIES
  None.
  
RETURN VALUE
  Pointer to MRW Q item.
  
SIDE EFFECTS
  None.

===========================================================================*/
tdsrlci_mrwq_type* tdsrlci_get_mrw_q_item
(
  uint8 lc_id     /* Logical channel ID for data channel  */
);

/*===========================================================================

FUNCTION RLCI_UPDAT_TOTAL_RETX_PDU_SIZE

DESCRIPTION
  Update the total number of bytes needed to be reXmit.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_updat_total_retx_pdu_size
(
  byte lc_id,     /* Logical channel ID for data channel  */
  uint16 retx_size       /* PDU Sequencial number                */
);


/*===========================================================================

FUNCTION RLCI_UL_AM_BASE_TIMER_HANDLER

DESCRIPTION
  Handle 10ms Base Timer timed out.
  In Uplink/ Downlink, there is only one base timer. 
  This call back will restart the base timer(s) & set needed signals for 
  handling timer expiry in L2_UL & L2_DL tasks.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_am_base_timer_cb (uint32 cb_param);

extern rex_timer_type        tdsrlc_ul_am_base_tmr;
                              /* This is base 10ms timer for RLC AM channels */
                              /* at expiry, periodic status/ control related */
                              /* procedures are triggered*/
extern boolean               tdsrlc_ul_am_base_tmr_start;

extern rex_timer_type        tdsrlc_dl_am_base_tmr;
                              /* This is base 10ms timer for RLC AM channels */
                              /* at expiry, periodic status/ control related */
                              /* procedures are triggered*/
extern boolean               tdsrlc_dl_am_base_tmr_start;

/*===========================================================================

FUNCTION RLCI_ENQ_RETX_NAK_INFO

DESCRIPTION
  Enqueue the ReXmit NAK information into nak_q for the asked RLC entity. 
  This NAK buff will be pull out later to generate NAK status PDU.
  
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
/*===========================================================================

FUNCTION RLCI_RCVD_MRW_ACK

DESCRIPTION
  Look at the MRW_Q to compare the first MRW item in the Q to the received 
  MRW ACK_SN and N. If they are matched or the MRW ACK_SN is acknowledging
  advance, recycle the first MRW item in the MRW Q. 
  If the MRW ACK_SN is acknowledging advance, process the acknowledged SN
  to the TXQ and VT(A).
  
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_rcvd_mrw_ack
(
  rlc_lc_id_type rlc_id,
  uint16  mrw_sn,
  uint8   mrw_n
);

/*===========================================================================

FUNCTION RLCI_TX_MRW_ACK

DESCRIPTION
  Enqueue MRW ACK for the next TTI to transmit to the peer side.
  
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_tx_mrw_ack
(
  rlc_lc_id_type rlc_id,
  uint16  mrw_sn,
  uint8   mrw_n
);

/*===========================================================================

FUNCTION RLCI_CHECK_RESET_ACK_FROM_PEER

DESCRIPTION
  Received an ACK from the peer side. Compare the rx'd SN to the RESET_SN
  in the state variables. If they are the same, update the RESET_SN to 
  the next SN and stop the reset timer if applicable.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  If the rx'd RESET_SN matches the RESET_SN in the Uplink, return TRUE. 
  Otherwise, a FALSE is returned.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean tdsrlci_check_reset_ack_from_peer
(
  rlc_lc_id_type  rlc_id,                /* UL RLC Data Logical Channel ID */
  uint8           reset_ack_sn
);


/*===========================================================================

FUNCTION TDSRLCI_GET_RX_WIN_SIZE

DESCRIPTION
  Obtain the rx_win_size.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  rx_win_size.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint16 tdsrlci_get_rx_win_size (rlc_lc_id_type rlc_id);


/*===========================================================================

FUNCTION RLCI_FEED_PEER_RESET_SN

DESCRIPTION
  Give the peer's RESET_SN received from the Downlink to the Uplink.
  This SN will be used in the RESET_ACK PDU sending to the peer side.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_feed_peer_reset_sn
(
  rlc_lc_id_type  rlc_id, 
  uint8           sn
);

/*===========================================================================

FUNCTION RLCI_SET_POST_RRC_RESET_SIG

DESCRIPTION

DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_set_post_rrc_reset_sig
(
  tdsrlc_error_e_type  err_type,
  rlc_lc_id_type    rlc_id
);

/*===========================================================================

FUNCTION RLCI_NON_EMPTY_FUNC()

DESCRIPTION
  Callback function that deregisters the callback function non_empty_func_ptr
  on all uplink queues and calls mac fucntion tdsmac_ul_trigger_tx_req() to
  inform mac to begin transmission.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_non_empty_func
(
      dsm_watermark_type* wm_ptr,
      void*               func_data

);


/*=========================================================================

FUNCTION RLCI_CHANGE_PEER_TX_WIN

DESCRIPTION

    This function changes the peer side's transmission window size. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void tdsrlci_change_peer_tx_win
(
  rlc_lc_id_type rlc_id,
  uint16 rx_window_size
);
 
/*===========================================================================

FUNCTION RLCI_UPDATE_RX_WIN_SIZE

DESCRIPTION
  Update the Receiving Window. 
  
  RLCI_CLOSE_RX_WIN : close the Receiving Window to VR_H.
  RLCI_OPEN_RX_WIN : Open the Receiving Window to 
                    (VR_R + config RX_WINDOW) % 4096
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_update_rx_win_size
(
  rlc_lc_id_type      rlc_id, 
  tdsrlci_win_op_e_type  win_op
);

/*===========================================================================

FUNCTION RLCI_REQUEST_TX_RESET_ACK

DESCRIPTION
  By turning on the owe_reset_ack to have the UL send RESET_ACK to the 
  peer.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_request_tx_reset_ack
(
  rlc_lc_id_type  rlc_id
);
/*===========================================================================

FUNCTION TDSRLCI_SET_TX_RESET_ACK

DESCRIPTION

  Turns the owe_reset_ack flag.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_set_tx_reset_ack
(
  rlc_lc_id_type  rlc_id
);
/*===========================================================================

FUNCTION RLCI_SET_DUPLICATE_RESET_RX

DESCRIPTION

  Turns the duplicate_reset_rx flag.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_set_duplicate_reset_rx
(
  rlc_lc_id_type  rlc_id
);

/*===========================================================================

FUNCTION TDSRLCI_GET_RESET_SN

DESCRIPTION
  Returns RSN used for the RESET sent on UL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 tdsrlci_get_reset_sn
(
  rlc_lc_id_type  rlc_id
);

/*===========================================================================

FUNCTION RLCI_GET_RESET_ACK_STATUS

DESCRIPTION
  Returns RESET ACK status on DL, i.e returns TRUE if its a UE initiated 
  RESET and UE is waiting for RESET_ACK.

DEPENDENCIES
  None.

RETURN VALUE
  RESET_ACK status - TRUE if UE is expecting a RESET_ACK, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean tdsrlci_get_reset_ack_status
(
  rlc_lc_id_type  rlc_id
);
/*===========================================================================

FUNCTION RLCI_INPUT_DL_STATUS

DESCRIPTION
  Copy the NAK list from the DL NAK Q to UL NAK Q for transmitting. And 
  turn on the 
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void  tdsrlci_input_dl_status
(
  rlc_lc_id_type  rlc_id
);

/*===========================================================================

FUNCTION TDSRLCI_GET_DL_HFN

DESCRIPTION
   This function is called by RLC UL to get latest HFN's for all established
   RLC UM and AM radio-bearers.  RLC populates the * to struct 
   tdsrlc_hfn_req_type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  If HFN's for all UM and AM rb's are populated.
  FALSE:  If no UM and AM rb's are present.

SIDE EFFECTS
  None.

===========================================================================*/
boolean tdsrlci_get_dl_hfn
(
  tdsrlc_hfn_req_type* hfn_req             /*ptr to hfn request type          */
);



/*===========================================================================

FUNCTION  TDSRLCI_SUSPEND_DL_AM_TIMER

DESCRIPTION
  It suspends the DL AM 20ms timer. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_suspend_dl_am_timer
(
  byte lc_id
);

/*===========================================================================

FUNCTION  TDSRLCI_RESUME_DL_AM_TIMER

DESCRIPTION
  It resumes the DL AM 20ms timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_resume_dl_am_timer
(
 byte lc_id
);

/*===========================================================================

FUNCTION RLCI_POST_RLC_RESET_CMD

DESCRIPTION
  This function post a RLC AM RESET command to L2 UL task to ask RLC AM
  to perform RLC RESET procedure. 
  This is to bring the RESET procedure being performed in Interrupt level 
  to the task context level.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_post_rlc_reset_cmd
(
  rlc_lc_id_type  rlc_id
);


/*===========================================================================

FUNCTION RLCI_POST_RLC_DL_DISCARD_TIMEOUT_HANDLER_COMMAND

DESCRIPTION
  This function posts a DL RLC command to handle the timer time out that
  has just been triggered.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tdsrlci_post_rlc_dl_discard_timeout_handle_command
(
  uint32 cb_param
);

/*===========================================================================

FUNCTION RLC_UL_SDU_DISCARD_TIMEOUT_HANDLER

DESCRIPTION
  This function process the discard timer that just fired off. This function
  scans the transmission buffer to see which SDUs need to be removed since
  they have stayed in the queue for too long. If there are more PDUs in the
  transmission buffer, it will also find out the PDU containing the start of
  next SDU for which the timer has not expired yet and starts the timer for
  that SDU. It will alsostart the MRW procedure for any SDUs that are
  discarded.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlc_ul_sdu_discard_timeout_handler
(
  rlc_lc_id_type rlc_channel_id
);


extern void  tdsrlc_dsm_touch_exit(dsm_item_type *pkt_head_ptr);

/*===========================================================================

FUNCTION RLCI_RESET_POLL_IND

DESCRIPTION
  Calling this function with the logical channel ID would reset reset
  "rcvd_poll" indication.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_reset_poll_ind(rlc_lc_id_type rlc_id);

/*===========================================================================

FUNCTION  RLCI_CHECK_MRW_ACK_VALIDITY

DESCRIPTION
  Validate the MRW ACK.
  25.322 11.6.6.3
  The Sender shall discard the received MRW_ACK SUFI if one of the 
  following cases occurs:
- no ongoing SDU discard with explicit signalling procedure; or
- the SN_ACK field in the received MRW_ACK SUFI < the SN_MRWLENGTH field in 
  the transmitted MRW SUFI; or
- the SN_ACK field in the received MRW_ACK SUFI = the SN_MRWLENGTH field in 
  the transmitted MRW SUFI, and the N field in the received MRW_ACK SUFI is 
  not equal to the NLENGTH field in the transmitted MRW SUFI; or
- the SN_ACK field in the received MRW_ACK SUFI > the SN_MRWLENGTH field in 
  the transmitted MRW SUFI, and the N field in the received MRW_ACK SUFI is 
  not equal to "0000".

  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE - The rx'd MRW ACK is valid.
  FALSE - The rx'd MRW ACK is not valid.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean tdsrlci_check_mrw_ack_validity 
(
  rlc_lc_id_type ul_lc_id,
  uint8  nlen,                      /* Number of length Indicators      */
  uint16 sn_mrw                    /* Sequence number of ACKED PDU     */
);

/*===========================================================================

FUNCTION RLCI_GENERATE_DL_STATUS

DESCRIPTION
  Generate DL status for UL to send.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_generate_dl_status(rlc_lc_id_type rlc_id);

/*===========================================================================

FUNCTION TDSRLCI_GET_DL_LATEST_PENDING_HFN

DESCRIPTION
   This function is called by RRC to get latest pending HFN for indicated 
   logical channel ID via RLC UL.  
   RLC DL populates the info to the struct tdsrlc_pending_hfn_req_type.
   If there is a pending configuration for UL alone then 
   dl_pending_hfn_present flag will be set to FALSE and vice-versa.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE:  If either dl_pending_hfn_present or ul_pending_hfn_present
                    flag is set to TRUE then function will return SUCCESS.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_get_dl_latest_pending_hfn
(
  rlc_lc_id_type rlc_id,                /* queried RLC logical channel ID */
  tdsrlc_pending_hfn_req_type *pending_hfn
);

/*===========================================================================

FUNCTION TDSRLCI_DL_OVERWRITE_HFN

DESCRIPTION
   This function is called by RRC to overwrite the HFN of RLC currently using
   thru UL.

   This function will be called after RRC re-establish RLC to overwrite the 
   new HFN that RRC just comes up before the traffic can be resumed.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE:  Done successful / no such LC or no ciphering at all for
                    this LC.

SIDE EFFECTS
  None.

===========================================================================*/
uecomdef_status_e_type tdsrlci_dl_overwrite_hfn
(
  rlc_lc_id_type rlc_id,                /* queried RLC logical channel ID */
  uint32 hfn
);

#ifdef FEATURE_TDSCDMA_RLC_QXDM_LOGGING
tdsrlc_cipher_algo_e_type tdsrlci_map_cipher_algo
(
  uecomdef_umts_cipher_algo_e_type algo
);

/*===========================================================================

FUNCTION TDSRLC_ERROR_LOG_SUBMIT

DESCRIPTION
  Function used to log the following RLC error conditions described in 
  section 11.3.4 of 25.322

PARAMETERS  
1. dl_rlc_data_id: Indicates DL rlc logical channel ID; Value starts from > 16 for AM channels
2. rlc_sn: Indicates the erroneous PDU just ignored by UE RLC; Range "0 - 4095" 
           and value of "0xFFFF" indicates that the SN is not-applicable for this
           error condition
3. rlc_err_reason: The below table gives the mapping of the error number 
to error description:
    0. PDU outside window
    1. Timer Discard timeout
    2. Invalid length
    3. PDU duplicates
    4. Buffer full
    5. Invalid size
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/

void tdsrlc_error_log_submit(rlc_lc_id_type dl_rlc_data_id, uint16 rlc_sn, 
                          tdsrlc_err_reason_type rlc_err_reason);




#endif



/*===========================================================================

FUNCTION RLCI_EMPTY_DL_CONTROL_Q

DESCRIPTION
This function will empties tx_nak_q, mrw_ack_q queues

DEPENDENCIES
None

RETURN VALUE
None
  
SIDE EFFECTS
None

===========================================================================*/
void tdsrlci_empty_dl_control_q(uint8 lc_id);


/*===========================================================================

FUNCTION  

DESCRIPTION
This function will move the UL-AM to TDSUL_CONFIG_PENDING state and initialize 
the uplink reset parameters to default values

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_ul_set_st_and_init_reset_param
(
  rlc_lc_id_type lc_id,
  boolean init_reset_param
);

/*===========================================================================

FUNCTION TDSRLCI_DL_CHANGE_STATE

DESCRIPTION
   This function will change the downlink RLC from TDSDL_RESET_PENDING state to
   its previous state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_dl_change_state
(
  rlc_lc_id_type lc_id
);

/*===========================================================================

FUNCTION RLCI_READ_VR_MR

DESCRIPTION
  Read the RX state(VR_MR) from the Down-link side.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint16 tdsrlci_read_vr_mr
(
  rlc_lc_id_type lc_id
);


/*===========================================================================

FUNCTION tdsrlci_get_config_action

DESCRIPTION
  this function checks all the bits of the bitmask and return the
  corresponding action set by RRC to RLC at the tme of channel config req
    
DEPENDENCIES
  None.
  
RETURN VALUE
  action type
  
SIDE EFFECTS
  None.

===========================================================================*/

tdsrlc_channel_action_cnf_e_type tdsrlci_get_config_action(uint16 *cfg_mask);

/*===========================================================================

FUNCTION RLCi_DUPLICATE_NAK

DESCRIPTION

  Checks if the given nak_sn is a duplicate or not
  
DEPENDENCIES

RETURN VALUE
  TRUE if its a duplicate

SIDE EFFECTS
  None.

===========================================================================*/
boolean tdsrlci_duplicate_nak
(
  rlc_lc_id_type    rlc_id,
  uint16            nak_sn
);

/*===========================================================================

FUNCTION RLC_COPY_TO_WORD

DESCRIPTION
  This function copies data from uint8 pointer into a word byte-by-byte
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 tdsrlc_copy_to_word(uint8 *src_ptr);
/*===========================================================================

FUNCTION RLCI_GET_TX_WIN_STATE_VAR

DESCRIPTION
  Gives the Tx state varibles vt_a, vt_s, vt_ms values

DEPENDENCIES

RETURN VALUE
  vt_a, vt_s, vt_ms

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_get_tx_win_state_var
(
  rlc_lc_id_type rlc_id,
  uint16 *vt_a,
  uint16 *vt_ms,
  uint16 *vt_s
);
/*===========================================================================

FUNCTION tdsrlc_get_nak_bitmap

DESCRIPTION
    - This function gets the nak bitmap array maintained on DL.

DEPENDENCIES
  None.

INPUT ARGUMENTS
  lc_id      - logical channel id.

RETURN VALUE
  nak bitmap - pointer to nak bitmap array.

SIDE EFFECTS
  None.

===========================================================================*/
tdsrlc_opt_status_type  *tdsrlc_get_nak_bitmap
(
  byte                  lc_id  
);

/*===========================================================================

FUNCTION tdsrlc_get_nak_status

DESCRIPTION
    - This function gets the nak status indicating whether we have any naks
      pending to be transmited or not.

DEPENDENCIES
  None.

INPUT ARGUMENTS
  nak_bitmap - pointer to bitmap array
  fsn        - fist sequence number for which status is needed.
  lsn        - last sequence number for which status is needed.
  mod        - modulus to use..

RETURN VALUE
  nak status - TRUE - nak is present.
               FALSE - no naks are there.

SIDE EFFECTS
  None.

===========================================================================*/
boolean tdsrlc_get_nak_status
(
  byte                  lc_id,
  uint16                fsn,
  uint16                lsn,
  uint16                mod 
);
/*===========================================================================

FUNCTION tdsrlc_get_num_nak

DESCRIPTION
    - This function returns the number number of sn_li pairs in the 
      DL nak status array.

DEPENDENCIES
  None.

INPUT ARGUMENTS
  lc_id      - logical channel id.


RETURN VALUE
  num_sn_li - number of sn_li pairs in the DL nak status array.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 tdsrlc_get_num_nak
(
  byte                  lc_id  
);

/*===========================================================================

FUNCTION tdsrlc_get_num_nak_sn_li

DESCRIPTION
    - This function calculates and returns the number number of sn_li pairs 
      in the nak status array..

DEPENDENCIES
  None.

INPUT ARGUMENTS
  lc_id      - logical channel id.
  fsn        - fist sequence number from which sn_li is counted.
  lsn        - last sequence number till which sn_li is counted.
  mod        - modulus to use..


RETURN VALUE
  num_sn_li - number number of sn_li pairs in the nak status array.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 tdsrlc_get_num_nak_sn_li
(
  byte                  lc_id,
  uint16                fsn,
  uint16                lsn,
  uint16                mod 
);

/*===========================================================================

FUNCTION tdsrlc_set_num_nak_sn_li

DESCRIPTION
    - This function sets the value of number of sn_li pairs, num_sn_li.

DEPENDENCIES
  None.

INPUT ARGUMENTS
  lc_id      - logical channel id.
  num_sn_li  - value for num_sn_li

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlc_set_num_nak_sn_li
(
  byte                  lc_id,
  uint16                num_sn_li
);


/*===========================================================================

FUNCTION FUNCTION RLCI_RESET_AM_RETX_Q

DESCRIPTION
  Resets retranmission Q of AM entity maintained by AM UL, also clears off
  retransmission bytes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_reset_am_retx_q(rlc_lc_id_type rlc_id);

/*===========================================================================

FUNCTION TDSRLCi_CHK_UL_RESET_PENDING

DESCRIPTION
  This function checks if the UL is in RESET pending state.

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating, RESET pending or not.

SIDE EFFECTS
  None
===========================================================================*/

boolean tdsrlci_chk_ul_reset_pending
(
  rlc_lc_id_type lc_id
);

/*===========================================================================

FUNCTION TDSRLCi_CHK_DL_RESET_PENDING

DESCRIPTION
  This function checks if the DL is in RESET pending state.

DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating, RESET pending or not.

SIDE EFFECTS
  None
===========================================================================*/

boolean tdsrlci_chk_dl_reset_pending
(
  uint8 lc_id
);


/*===========================================================================

FUNCTION TDSRLCI_RESET_STATUS_TIMER_CMD

DESCRIPTION
  This function would process a RLC AM RESET status timer command given to 
  L2 UL task to perform timer from R99-ISR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  tdsrlci_reset_status_timer_cmd(void);

/*===========================================================================

FUNCTION RLCI_POST_RESET_STATUS_TIMER_CMD

DESCRIPTION
  This function post a RLC AM RESET timer command to L2 UL task to perform timer 
  resetting in UL-task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsrlci_post_reset_status_timer_cmd
(
  rlc_lc_id_type  rlc_id
);


#endif

