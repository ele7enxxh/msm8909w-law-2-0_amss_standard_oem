#ifndef TDSMACL1RRCIF_H
#define TDSMACL1RRCIF_H

/*========================================================================================

                MAC L1 RRC INTERFACE HEADER FILE

DESCRIPTION
  This file contains the declaration of the interface functions and
  data types used by MAC, L1, and RRC

  Copyright (c) 2002-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

========================================================================================*/

/*========================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macl1rrcif.h_v   1.0   10 May 2002 13:24:00   yiz  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/mac/inc/tdsmacl1rrcif.h#1 $  $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------------------------
05/06/11    cdf     Remove useless parameters tf_flag in tdsmac_ul_tf_type
11/23/10    cyx     Remove tdsmvsif.h to clean up l2 interface
10/24/10    guo   Initial release Upmerged WCDMA to TDS

========================================================================================*/

/*=======================================================================================

                     INCLUDE FILES

========================================================================================*/

#include "tdscdma_variation.h"
#include "customer.h"
#include "comdef.h"

#include "tdsuecomdef.h"

#ifdef FEATURE_VOC_AMR_WB
#include "mvssup.h"
#include "mvs.h"
#endif


/*--------------------------------------------------------------------------------------*/
/* TFC Control Flag Type                                                                */
/*--------------------------------------------------------------------------------------*/
typedef enum
{
    TDSTFC_ALLOWED = 0x00,     /* TFC is allowed */
    TDSTFC_NOT_ALLOWED,        /* TFC is not allowed */
  TDSTFC_MANDATORY,          /* This TFCI is part of the minimum set and hence can't be
                             subjected to the elimination due to CM by HLS or Tx power
                             restriction */
    TDSTFC_REMOVED             /* TFC is removed */
} tdsmac_ul_tfc_control_e_type;

/*--------------------------------------------------------------------------------------*/
/* Uplink Transport Format                                                              */
/*--------------------------------------------------------------------------------------*/
typedef struct {

    /* Number of transport blocks */
    uint16              ntblks;

    /* Transport block size (includes MAC & RLC headers) in bits */
    uint16              tb_size;

} tdsmac_ul_tf_type;

/*--------------------------------------------------------------------------------------*/
/* Uplink Transport Format Set                                                          */
/*--------------------------------------------------------------------------------------*/
typedef struct {

    /* TTI type of each transport channel */
    tdsl1_tti_enum_type    tti_info;

    /* mac_header_size */
    uint8               mac_header_size;

    
    /* Number of transport formats */
    uint16              ntf;

    /* Each known Transport Format Combination */
    tdsmac_ul_tf_type      tf_info[TDSUE_MAX_TF];

} tdsmac_ul_tfs_type;

/*--------------------------------------------------------------------------------------*/
/* Uplink Transport Format Combination                                                  */
/*--------------------------------------------------------------------------------------*/
typedef struct {

    /* Flag indicating whether the TFC is allowed (TDSALLOWED) or not  */
    /* allowed NOT_ALLOWED) or removed (REMOVED) */
    tdsmac_ul_tfc_control_e_type   control_flag;

    /* CTFC value */
    uint32  ctfc;

    /* Transport format of each transport channel */
    uint8   tfi[TDSUE_MAX_TRCH];

} tdsmac_ul_tfc_type;

/*--------------------------------------------------------------------------------------*/
/* Uplink Transport Format Combination Set                                              */
/*--------------------------------------------------------------------------------------*/
typedef struct {

    /* Number of known transport format combinations */
    uint16  ntfc;

    /* Number of Transport Channels */
    uint8   nchan;

    /* Each known Transport Format Combination */
    tdsmac_ul_tfc_type     tfc_info[TDSUE_MAX_TFC];

    /* Each known Transport Format Combination */
    tdsmac_ul_tfs_type     tfs_info[TDSUE_MAX_TRCH];
#ifdef FEATURE_VOC_AMR_WB
    mvs_mode_type orderd_codec_mode;
#endif
} tdsmac_ul_tfcs_type;


/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

#endif /* TDSMACL1RRCIF_H */

