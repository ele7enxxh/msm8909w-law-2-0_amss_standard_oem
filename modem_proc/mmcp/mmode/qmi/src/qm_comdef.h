#ifndef _QM_COMDEF_H
#define _QM_COMDEF_H

/*===========================================================================

  QM_COMDEF.H

  DESCRIPTION

  QMI_MMODE common header file

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

  when        who    what, where, why
  --------    ---    ----------------------------------------------------------
  01/09/13    aa     Created file
  ===========================================================================*/

#include "comdef.h"

/*--------------------------------------------------------------------------- 
  data types
  ---------------------------------------------------------------------------*/
#define QM_NAS_SUB_ID_VALID(sub_id) ((sub_id)> QM_SUBS_MIN && (sub_id)<QM_SUBS_MAX)

enum qm_timer_e
{
  QM_TIMER_MIN      = -1,
  QM_TIMER_TIB,
  QM_TIMER_MAX
};

typedef uint32                       rat_bitmask_type;
#define QM_RAT_MASK_C 0x00000001                           // bit 0 - CDMA                   
#define QM_RAT_MASK_H 0x00000002                           // bit 1 - HDR                   
#define QM_RAT_MASK_G 0x00000004                           // bit 2 - GERAN                   
#define QM_RAT_MASK_W 0x00000008                           // bit 3 - WCDMA                   
#define QM_RAT_MASK_L 0x00000010                           // bit 4 - LTE                   
#define QM_RAT_MASK_T 0x00000020                           // bit 5 - TDSCDMA

enum qm_rat_e
{
  QM_RAT_MIN        = -1,
  QM_RAT_1X,
  QM_RAT_HDR,
  QM_RAT_GERAN,
  QM_RAT_UMTS,
  QM_RAT_LTE,
  QM_RAT_TDSCDMA,
  QM_RAT_MAX
};

enum qm_subs_e
{
  QM_SUBS_MIN      = -1,
  QM_SUBS_PRI,
#ifdef FEATURE_DUAL_SIM
  QM_SUBS_SEC,
#endif
#ifdef FEATURE_TRIPLE_SIM
  QM_SUBS_TER,
#endif
  QM_SUBS_MAX
};

enum qm_sig_type_e
{
  QM_SIG_TYPE_MIN    = -1,
  QM_SIG_TYPE_RSSI,
  QM_SIG_TYPE_ECIO,
  QM_SIG_TYPE_SINR,
  QM_SIG_TYPE_IO,
  QM_SIG_TYPE_RSRQ,
  QM_SIG_TYPE_RSRP,
  QM_SIG_TYPE_SNR,
  QM_SIG_TYPE_RSCP,
  QM_SIG_TYPE_MAX
};

enum qm_stack_index_e
{
  STACK_INDEX_0 = 0,
  STACK_INDEX_1,
  STACK_INDEX_MAX
};



#endif // !_QM_COMDEF_H

