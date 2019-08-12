#ifndef TDSL1_FTM_MSG_H
#define TDSL1_FTM_MSG_H

/*===========================================================================

            TDSCDMA FTM Message Router R E L A T E D   D E C L A R A T I O N S

DESCRIPTION 

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

REFERENCES

Copyright (c) 2001-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/30/13   lub     Added support for FTM MSGR messages

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "msgr_umid.h"

#ifdef __cplusplus
}  // extern "C"
#endif



#define MSGR_MODULE_L1NS                   0x0E

#define MSGR_TDSCDMA_L1NS    MSGR_TECH_MODULE(MSGR_TECH_TDSCDMA, MSGR_MODULE_L1NS)


#define TDSL1_NS_CPHY_START_TDSCDMA_MODE_CNF_CODE        4
#define TDSL1_NS_CPHY_STOP_TDSCDMA_MODE_CNF_CODE         5
#define TDSL1_NS_CPHY_ACQ_CNF_CODE                       6
#define TDSL1_NS_CPHY_SETUP_CNF_CODE                     7
#define TDSL1_NS_CPHY_PHYCHAN_ESTABLISHED_IND_CODE       8 
#define TDSL1_NS_CPHY_RL_FAILURE_IND_CODE                9
#define TDSL1_NS_CPHY_ERROR_IND_CODE                     0x0A

//For DL data
#define TDSL1_NS_DL_DATA_IND_CODE   0x0B


//For DL data
typedef struct
{
  msgr_hdr_struct_type hdr;
  uint8 *tb_data_ptr;
  uint16 trblk_size;
  uint16 buffer_size_bytes;
  boolean tb_error;

} tdsl1_ns_dl_data_ind_t;


/*------------------------------------------CNF---------------------------*/

typedef struct
{
  msgr_hdr_struct_type hdr;
} tdsl1_ns_cphy_start_tdscdma_mode_cnf_t;

typedef struct
{
  msgr_hdr_struct_type hdr;
} tdsl1_ns_cphy_stop_tdscdma_mode_cnf_t;

typedef struct
{
  msgr_hdr_struct_type hdr;
  boolean status;
  uint16 freq;
} tdsl1_ns_cphy_acq_cnf_t;

typedef struct
{
  msgr_hdr_struct_type hdr;
  boolean status;
} tdsl1_ns_cphy_setup_cnf_t;

typedef struct
{
  msgr_hdr_struct_type hdr;
  boolean phychan_est_ind_status;
} tdsl1_ns_cphy_phychan_established_ind_t;

typedef struct
{
  msgr_hdr_struct_type hdr;
} tdsl1_ns_cphy_rl_failure_ind_t;

typedef struct
{
  msgr_hdr_struct_type hdr;
} tdsl1_ns_cphy_error_ind_t;

/* Union of all L1 cnf/ind */
typedef union
{
  tdsl1_ns_cphy_start_tdscdma_mode_cnf_t  start;
  tdsl1_ns_cphy_stop_tdscdma_mode_cnf_t   stop;
  tdsl1_ns_cphy_acq_cnf_t                 acq;
  tdsl1_ns_cphy_setup_cnf_t               setup;
  tdsl1_ns_cphy_phychan_established_ind_t establ;
  tdsl1_ns_cphy_rl_failure_ind_t          rlf;
  tdsl1_ns_cphy_error_ind_t               err;
} tdsl1_ns_cnf_ind_union_type;

/* These messages are defined for use with message router */
enum
{
  //For DL data
  MSGR_DEFINE_UMID(TDSCDMA, L1NS, IND, DL_DATA, TDSL1_NS_DL_DATA_IND_CODE, tdsl1_ns_dl_data_ind_t),

  MSGR_DEFINE_UMID(TDSCDMA, L1NS, CNF, CPHY_START, TDSL1_NS_CPHY_START_TDSCDMA_MODE_CNF_CODE, tdsl1_ns_cphy_start_tdscdma_mode_cnf_t), 
  MSGR_DEFINE_UMID(TDSCDMA, L1NS, CNF, CPHY_STOP, TDSL1_NS_CPHY_STOP_TDSCDMA_MODE_CNF_CODE, tdsl1_ns_cphy_stop_tdscdma_mode_cnf_t),
  MSGR_DEFINE_UMID(TDSCDMA, L1NS, CNF, CPHY_ACQ, TDSL1_NS_CPHY_ACQ_CNF_CODE, tdsl1_ns_cphy_acq_cnf_t),
  MSGR_DEFINE_UMID(TDSCDMA, L1NS, CNF, CPHY_SETUP, TDSL1_NS_CPHY_SETUP_CNF_CODE, tdsl1_ns_cphy_setup_cnf_t),
  MSGR_DEFINE_UMID(TDSCDMA, L1NS, IND, CPHY_ESTABLISHED, TDSL1_NS_CPHY_PHYCHAN_ESTABLISHED_IND_CODE, tdsl1_ns_cphy_phychan_established_ind_t),
  MSGR_DEFINE_UMID(TDSCDMA, L1NS, IND, CPHY_RL_FAILURE, TDSL1_NS_CPHY_RL_FAILURE_IND_CODE, tdsl1_ns_cphy_rl_failure_ind_t),
  MSGR_DEFINE_UMID(TDSCDMA, L1NS, IND, CPHY_ERROR, TDSL1_NS_CPHY_ERROR_IND_CODE, tdsl1_ns_cphy_error_ind_t)
};

#endif   /*TDSL1_FTM_MSG_H*/
