#ifndef L1_SAP_COMMON_H
#define L1_SAP_COMMON_H
/*===========================================================================
                        L 1 _ S A P _ C O M M O N . H

DESCRIPTION

This file defines types that are common to the control and data service
access point interfaces.                        

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  
Copyright (c) 2000 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1sapcommon.h_v   1.9   02 Jan 2002 17:31:06   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1sapcommon.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
05/23/13    geg     WSW decoupling (except for component of QCHAT)
02/01/12    raj     Nikel Feature cleanup.
02/01/12    gnk     Nikel Feature cleanup.
01/26/12    vs      Nikel feature cleanup. removed the feature FEATURE_SHARED_TRCH_ENABLE_DISABLE.
01/19/12    zr      Mainlined FEATURE_MCAL_WCDMA  
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
10/15/10    mr/yh   Added support for W2L Compressed mode
04/06/10    ks      Added Enhanced FDPCH support
11/25/09    ms      Moved inline functions to l1sapcommon.h
03/30/09    rm      Added new featurization for FEATURE_CMI.
02/26/09    asm     Changes to mainline feature FEATURE_LIMIT_TFCS_TO_ERAM_SIZE 
10/03/08    hk      Bringing in 7k Mailine fixes
06/18/08    ms      Added support for new ciphering algorithm - SNOW3G
10/24/07    vsr     Mainlining MBMS and Enhanced DL changes 
09/19/07    asm     Added definitions for L1 ERAM max buffer size, max num tblks 
                    and max tb size. These should be updated per target if necessary.
08/08/07    vsr     Moved in TGMP (CM related) enum from l1cmmgr.h
10/17/06    ms      Added support for FDPCH.
10/04/06    mc      Added MCAL decoder support under feature 
                    FEATURE_MCAL_WCDMA
08/14/06    yiz     Mainline HSUPA code
05/31/05    gv      Changes to remove Lint errors
08/30/04    gs      Added interface definitions for HSDPA
08/10/04    src     Added hash-defined symbols to represent the number of
                    P-CCPCH and S-CCPCH physical channels.
09/05/02    gs      1. Changed L1_MAX_CIPHER_KEY from 2 to 4
                    2. Added macro definition L1_UL_CIPHER_KEY_REQ and
                       L1_DL_CIPHER_KEY_REQ
01/02/02    gs      1. Added the definition for L1_MAX_RL moved from
                       l1dlphychancfg.h

08/21/01    gs      1. Added the enum definition for loopback mode type.

05/30/01    sk      Added L1_UL_PHYCHAN_NONE definition to UL Phychan Enum defs.

03/08/01    gs      1. Moved the definition of TrCh bf value macro from
                       l1dlcctrchcfg.h
                    2. Added the enum L1_DL_NUM_TRCH_REF_TYPE to enum
                       l1_dl_trch_ref_type_enum_type.
                    3. Changed L1_NUM_DL_TRCH to L1_NUM_DL_TRCH_REF_TYPE.

02/21/2001  mdl     reordered the l1_dl_phychan_enum_type so that 
                    L1_DL_PHYCHAN_NOCHAN appears after L1_NUM_DL_PHYCHAN
                    thus not wasting an array element in various arrays.
                    Pulled the UL phychan enum types and associated
                    cphy_setup_cnf offsets from ulcfg.h into here.
                    
12/20/00    sk      Added L1_UL_TRCH_MAX and L1_UL_NUM_SF defines.
12/12/2000  mdl     Added MAX_SCCPCH_SLOT_FORMAT, 
                    MAX_DPCH_SLOT_FORMAT, MAX_PDSCH_SLOT_FORMAT
12/01/2000  mdl     Added support for updated CPHY_SETUP_CNF packet
10/23/00    sk      Updated for uplink related common declarations.
10/22/00    gs      file created.
                    
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "mcalwcdma_dec.h"
#include "wl1api.h"

#ifndef FEATURE_CMI

/* -------------------------------------- */
/* PUBLIC API section of l1sapcommon.h    */
/* -------------------------------------- */

typedef enum
{
  E_TTI_2MS,
  E_TTI_10MS,
  NUM_E_TTI
} e_tti_enum_type;

#else

/* Public portion of the API exists in wcdmamvsif.h */
#include "wcdmamvsif.h"

#endif /* FEATURE_CMI */

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */


/*--------------------------------------------------------------------------
                 TRANSPORT BLOCK SIZE and RAM LIMITATIONS
--------------------------------------------------------------------------*/
#define L1_MAX_UL_NUM_TRBLK            24
#define L1_MAX_UL_TB_SIZE              5120
#define L1_MAX_ERAM_A_BUFFER_SIZE      14080 /* 440 words * 32 bits per word */

/*===========================================================================
FUNCTION  L1_GET_MAX_UL_NUM_TRBLK

DESCRIPTION
  This function is called by MAC to get the upper bound on the number of 
  transport blocks that can be transmitted in a TTI.

DEPENDENCIES
  L1_MAX_UL_NUM_TRBLK must be defined accurately for every new MSM.

RETURN VALUE
  The value of L1_MAX_UL_NUM_TRBLK

SIDE EFFECTS
  None
===========================================================================*/
INLINE uint16 l1_get_max_ul_num_trblk( void )
{
  return L1_MAX_UL_NUM_TRBLK;
}

/*===========================================================================
FUNCTION  L1_GET_MAX_UL_TB_SIZE

DESCRIPTION
  This function is called by MAC to get the upper bound on the size of each 
  transport block that can be transmitted in a TTI.

DEPENDENCIES
  L1_MAX_UL_TB_SIZE must be defined accurately for every new MSM.

RETURN VALUE
  The value of L1_MAX_UL_TB_SIZE (in bits)

SIDE EFFECTS
  None
===========================================================================*/
INLINE uint16 l1_get_max_ul_tb_size( void )
{
  return L1_MAX_UL_TB_SIZE;
}


/*===========================================================================
FUNCTION  L1_GET_MAX_UL_DATA_SIZE

DESCRIPTION
  This function is called by MAC to get the upper bound on the total number
  of data bits that can be transmitted in a TTI.

DEPENDENCIES
  L1_MAX_ERAM_A_BUFFER_SIZE must be defined accurately for every new MSM.

RETURN VALUE
  The value of L1_MAX_ERAM_A_BUFFER_SIZE (in bits)

SIDE EFFECTS
  None
===========================================================================*/
INLINE uint16 l1_get_max_ul_data_size( void )
{
  return L1_MAX_ERAM_A_BUFFER_SIZE;
}

/*--------------------------------------------------------------------------
                 SPREADING FACTOR ENUMS AND VALUES
--------------------------------------------------------------------------*/

/* UL SF is valid upto SF=256 only */
#define L1_UL_NUM_SF   ((uint8)L1_NUM_SF - 1)

/* Values for speading factor for each enum type */
#define L1_SF_4_VAL    4
#define L1_SF_8_VAL    8
#define L1_SF_16_VAL   16
#define L1_SF_32_VAL   32
#define L1_SF_64_VAL   64
#define L1_SF_128_VAL  128
#define L1_SF_256_VAL  256
#define L1_SF_512_VAL  512

extern uint16 l1_sf_val_table[L1_NUM_SF];

#define L1_SF_VAL(sf_enum) l1_sf_val_table[(sf_enum)]

/*--------------------------------------------------------------------------
                 TRANSMIT TIME INTERVAL ENUMS AND VALUES
--------------------------------------------------------------------------*/



/* Values for TTI in ms for each enum type */
#define L1_TTI_10MS_VAL 10
#define L1_TTI_20MS_VAL 20
#define L1_TTI_40MS_VAL 40
#define L1_TTI_80MS_VAL 80

extern uint16 l1_tti_val_table[L1_NUM_TTI];

#define L1_TTI_VAL(tti_enum) (l1_tti_val_table[(tti_enum)])

/*--------------------------------------------------------------------------
                         CRC ENUMS AND VALUES
--------------------------------------------------------------------------*/


/* Value for CRC length for each CRC enum type */
#define L1_CRC_LENGTH_0_VAL     0
#define L1_CRC_LENGTH_8_VAL     8
#define L1_CRC_LENGTH_12_VAL    12
#define L1_CRC_LENGTH_16_VAL    16
#define L1_CRC_LENGTH_24_VAL    24

extern uint8 l1_crc_length_val_table[L1_NUM_CRC_LENGTH];

#define L1_CRC_LENGTH_VAL(crc_len_enum) \
  l1_crc_length_val_table[(crc_len_enum)]
  

#define L1_DL_NUM_PCCPCH 2
#define L1_DL_NUM_SCCPCH 2

/*--------------------------------------------------------------------------
Values for DL physical channel status offset field in CPHY_SETUP_CNF 
packet structure
--------------------------------------------------------------------------*/
#define L1_DL_PHYCHAN_PCCPCH_S_OFFSET 0
#define L1_DL_PHYCHAN_PCCPCH_N_OFFSET 4
#define L1_DL_PHYCHAN_SCCPCH0_OFFSET  8
#define L1_DL_PHYCHAN_SCCPCH1_OFFSET  12
#define L1_DL_PHYCHAN_DPCH_OFFSET     16
#define L1_DL_PHYCHAN_PDSCH_OFFSET    20

extern uint8 l1_dl_trch_ref_type_bf_val[L1_DL_NUM_TRCH_REF_TYPE];

#define L1_DL_TRCH_REF_TYPE_BF_VAL(ref_type) l1_dl_trch_ref_type_bf_val[(ref_type)]

/* Utility macro to check upper boundary value for a variable
** Valid value of val should be less than bound
*/
#define CHECK_UPPER_BOUND(val, bound) \
{ \
  if(!(val < bound)) \
  { \
    ERR_FATAL("Val %d Exceeds boundary value %d", val, bound, 0); \
  } \
}


/*--------------------------------------------------------------------------
Values of the downlink transport channel status fields in the 
CPHY_SETUP_CNF packet structure
--------------------------------------------------------------------------*/
#define L1_DL_TRCH_SBCH_TYPE_OFFSET 0
#define L1_DL_TRCH_NBCH_TYPE_OFFSET 4
#define L1_DL_TRCH_PCH_TYPE_OFFSET  8
#define L1_DL_TRCH_FACH_TYPE_OFFSET 12
#define L1_DL_TRCH_DCH_TYPE_OFFSET  16
#define L1_DL_TRCH_DSCH_TYPE_OFFSET 20


/* This enum defines the uplink transport channel types */
typedef enum {
  L1_UL_TRCH_RACH,
  L1_UL_TRCH_DCH,
  L1_UL_TRCH_CPCH,
  L1_UL_NUM_TRCH,
  L1_UL_TRCH_NONE
} l1_ul_trch_enum_type;

/*--------------------------------------------------------------------------
Values of the uplink transport channel status fields in the 
CPHY_SETUP_CNF packet structure
--------------------------------------------------------------------------*/
#define L1_UL_TRCH_RACH_OFFSET 0
#define L1_UL_TRCH_DCH_OFFSET  4
#define L1_UL_TRCH_CPCH_OFFSET 8


#define L1_UL_PHYCHAN_DPCH_OFFSET 4
#define L1_UL_PHYCHAN_PRACH_OFFSET 0
#define L1_UL_PHYCHAN_PCPCH_OFFSET 8


/*--------------------------------------------------------------------------
           Externs for CPHY_SETUP_CNF status offset lookup arrays
--------------------------------------------------------------------------*/
extern uint8 l1_cphy_setup_cnf_dl_phychan_offset[L1_NUM_DL_PHYCHAN];
extern uint8 l1_cphy_setup_cnf_dl_trch_offset[L1_DL_NUM_TRCH_REF_TYPE];
extern uint8 l1_cphy_setup_cnf_ul_phychan_offset[L1_UL_NUM_PHYCHAN];
extern uint8 l1_cphy_setup_cnf_ul_trch_offset[L1_UL_NUM_TRCH];



/*--------------------------------------------------------------------------
  Constants indicating how many slot formats for a given physical channel
--------------------------------------------------------------------------*/
#define MAX_SCCPCH_SLOT_FORMAT 18
#define MAX_DPCH_SLOT_FORMAT 17
#define MAX_PDSCH_SLOT_FORMAT 7

/*--------------------------------------------------------------------------
                            CIPHERING_KEY_REQ
--------------------------------------------------------------------------*/

/* defines for the ciphering key updates */
#define L1_NUM_BYTES_CIPHER_KEY  16
#define L1_MAX_CIPHER_KEY        4
#define L1_MAX_CIPHER_KEY_ID_VAL 3

#define L1_UL_CIPHER_KEY_REQ 0x01
#define L1_DL_CIPHER_KEY_REQ 0x02




#endif /* L1_SAP_COMMON_H */
