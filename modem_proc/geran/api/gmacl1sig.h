#ifndef INC_GMACL1SIG_H
#define INC_GMACL1SIG_H
/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC L1 to MAC SIGNAL DEFINITION
***
***
*** DESCRIPTION
***
***  Signal definition for use with access function GPRS_MAC_l1_ACC_FUNC()
***
***  Defines the ENUM for all the signals used from L1 to MAC  .
***
***  Defines the Union that holds all the signal struct definitions
***  used from L1 to MAC
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gmacl1sig.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 11-06-06   tjw     Split API content from original file

*****************************************************************************/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gprsdef.h"
#include "geran_dual_sim.h"


/*****************************************************************************
***
***     Message Type Definitions
***
*****************************************************************************/
typedef enum
{

  L1_MAC_FREQ_ASS_CNF         = 0x00, /* Confirms the frequency assigment*/
  L1_MAC_USF_DETECTED         = 0x01, /* Indicates detection of USF when performing Dynamic UL TBf */
  L1_MAC_QUAL_MEASURMENTS     = 0x02, /* Reports quality measurement results to MAC */
  L1_MAC_1ST_DATA_BLOCK_SENT  = 0x03, /* Only sent on l1 tx-ing the first data block */
  L1_MAC_51_SB_FAILURE        = 0x04, /* single block failure on 51 MF procedures */
  L1_MAC_TBF_REL_CONFIRM      = 0x05, /* confirmation that l1 has returned to its idle state*/
  L1_MAC_RRBP_FAILED_IND      = 0x06, /* indicates failure to send RRBP*/
  L1_MAC_SINGLE_BLOCK_SENT    = 0x07, /* Sent when single block has been transmitted */
  L1_MAC_PSHO_CNF             = 0x08,
  /**************************************************************************
  * L1 to MAC GPRS spec dependent messages (GSM 4.04)
  **************************************************************************/
  MAC_PH_CONNECT_IND          = 0x40, /* Indicates the point at which the TBF start timer has occured */
  MAC_PH_DATA_IND             = 0x41, /* Indicates the reception of and RLC/MAC control block in L1 */
  MAC_PH_RANDOM_ACCESS_CONF   = 0x42, /* Confirms that L1 has sent PRACH */
  MAC_PH_CS_CONNECT_IND = 0x43  /* Indicates that a DTM assignment has been connected */
}  l1_mac_sig_id_t;

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/
#define UPLINK_TBF_ESTABLISHED    0
#define DOWNLINK_TBF_ESTABLISHED  1
#define UPLINK_DOWNLINK_TBF_ESTABLISHED 2

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


/*************************************************************************
*   "L1_MAC_FREQ_ASS_CNF" L1 to MAC message structure
*************************************************************************/

typedef enum
{
 UL_TBF_FREQ_CONF,
 DL_TBF_FREQ_CONF,
 UL_DL_TBF_FREQ_CONF
} tbf_freq_cnf_t;

typedef struct
{
  boolean  band_supported;
  tbf_freq_cnf_t  tbf_freq_cnf;
  gas_id_t        gas_id;
} l1_mac_tbf_freq_ass_cnf_t;


/*************************************************************************
*   "MAC_PH_CONNECT_IND" L1 to MAC message structure
*************************************************************************/
typedef enum
{
  MAC_L1_CON_STAT_NO_CAUSE,
  MAC_L1_CON_STAT_TRM_LOCK_FAIL_DL
} mac_l1_connect_status_t;

typedef struct
{
  uint8 tbf_type_ind;
  mac_l1_connect_status_t connect_status;
  gas_id_t gas_id;
} mac_ph_connect_ind_t;

/*************************************************************************
*   "MAC_PH_DATA_IND" L1 to MAC message structure
*************************************************************************/
typedef struct
{
  uint8         timeslot;
  uint32        frame_no;
  ARFCN_T       bcch_arfcn;
  uint8         dl_ctrl_block[SIZE_OF_CONTROL_RADIO_BLOCK];
  boolean       gprs_l1_page_group_flag;
  gas_id_t      gas_id;
} mac_ph_data_ind_t;


/*************************************************************************
*   "L1_MAC_TBF_REL_CONFIRM " L1 to MAC message structure
*************************************************************************/
typedef enum
{
 UL_TBF_RELEASE_CONF,
 DL_TBF_RELEASE_CONF,
 UL_DL_TBF_RELEASE_CONF
} tbf_released_t;

typedef struct
{
  tbf_released_t tbf_released;
  gas_id_t       gas_id;
} l1_mac_tbf_rel_confrim_t;

#endif
