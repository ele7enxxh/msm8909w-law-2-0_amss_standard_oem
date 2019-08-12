/*===========================================================================
                      DS_ANDSF_INTERNAL_DEFS.H

DESCRIPTION
 Internal definitions header file for ANDSF.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    -------                ----------------------------------------
06/09/14    Youjunf                          First version of file
===========================================================================*/
#ifndef DS_ANDSF_INRERNAL_DEF_H
#define DS_ANDSF_INRERNAL_DEF_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF



/*===========================================================================
                   EXTERNAL DEFINITIONS AND TYPES
===========================================================================*/
#define DS_ANDSF_POLICY_FILE_ROOT_PATH    "/data"
#define DS_ANDSF_POLICY_FILE_PATH         "/data/andsf.xml"
#define DS_ANDSF_POLICY_FILE_NAME         "andsf.xml"

#define ANDSF_POLICY_CHG_ERR_NO_ERROR       0
#define ANDSF_POLICY_CHG_ERR_INVALID_INPUT  1
#define ANDSF_POLICY_CHG_ERR_FILE_READ      2
#define ANDSF_POLICY_CHG_ERR_FILE_WRITE     3
#define ANDSF_POLICY_CHG_ERR_XML            4



typedef enum
{
  DS_ANDSF_SUBS_IDX1 = 0,

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  DS_ANDSF_SUBS_IDX2,
#endif
#ifdef FEATURE_TRIPLE_SIM
  DS_ANDSF_SUBS_IDX3,
#endif
  DS_ANDSF_SUBS_IDX_MAX
} ds_andsf_subs_idx_e_type;


#endif /* FEATURE_DATA_ANDSF */
#endif /* DS_ANDSF_INRERNAL_DEF_H */
