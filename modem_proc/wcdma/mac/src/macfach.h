#ifndef MACFACH_H
#define MACFACH_H
/*=====================================================================

     F A C H    C O N F I G U R A T I O N    H E A D E R   F I L E

GENERAL DESCRIPTION

  This module contains the declaration of the functions and data types
  defined in "macfachcfg.c"

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=====================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macfach.h_v   1.2   09 May 2001 18:15:14   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macfach.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------
07/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
06/02/11   kc      Added support for logical channel to transport channel mapping info
05/07/09   ssg     Updated Copyright Information
04/29/09    av     Fixed compiler warnings in L2 
01/29/08    sk     Support for MBMS under FEATURE_MODEM_MBMS
04/04/01    ps     Initial release after merging "macfachcfg.c" and "macfachhdr.c"

=====================================================================*/


/*============================================================================================
FUNCTION NAME   process_fach_tbs_hdr

DESCRIPTION     This function processes FACH TBS Header information
============================================================================================*/
void    process_fach_tbs_hdr
(
    /* First TB ID */
    uint8   tb_id,

    /* Pointer to internal FACH config info */
    fach_config_info_type       *int_fach_ptr,

    /* Pointer to FACH TBS Header Info */
    l1_dl_tb_set_hdr_struct_type    *tbs_hdr_ptr
);

#include "wcdma_variation.h"

/*==================================================================================================
FUNCTION NAME   process_dcch_dtch_fach_config

DESCRIPTION   This function processes config info of DCCH/DTCH that is maaped to FACH
==================================================================================================*/
void  process_dcch_dtch_fach_config
  (
/* Ordered FACH config data pointer */
  mac_dl_fach_trch_config_type      *oc_fach_ptr,

/* Internal FACH Config data pointer */
  fach_config_info_type             *int_fach_ptr
  );

/*============================================================================================
FUNCTION NAME   process_fach_config

DESCRIPTION   This function processes FACH configuration data

============================================================================================*/
void  process_fach_config
(
  /* Number of FACHs */
  uint8  num_fachs,

  /* Pointer to ordered FACH configuration data list */
  mac_dl_fach_trch_config_type  *oc_fach_ptr_list[]
);

/*===============================================================================================
FUNCTION NAME mac_dl_check_fach_trch

DESCRIPTION   This function checks if the input rlc id and rb id pair map to a FACH transport channel
===============================================================================================*/

log_wcdma_trch_type mac_dl_check_fach_trch
(
	/* RLC logical channel buffer identifier (0-20) */
	rlc_lc_id_type rlc_id, 

	/* Radio Bearer Identifier (0-31) */
	rb_id_type rb_id
);

#endif

