/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_multiband.h#1 $    $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
11/02/28    tjw     API content split out from GERAN internal file

===========================================================================*/
#ifndef RR_MULTIBAND_H
#define RR_MULTIBAND_H

#include "sys_type.h"
#include "sys.h"
/* rr_internal_band_T and sys_band_T should have the same definitions */
typedef enum
{
  BAND_EGSM       = SYS_BAND_EGSM_900,
  BAND_PGSM       = SYS_BAND_PGSM_900,
  BAND_PCS        = SYS_BAND_PCS_1900,
  BAND_DCS        = SYS_BAND_DCS_1800,
  BAND_GSM_850    = SYS_BAND_CELL_850,
  MAXNO_BANDS     = INVALID_BAND,
  BAND_NONE       = SYS_BAND_NONE
} rr_internal_band_T;

extern void mb_initialise(void);
extern rr_internal_band_T rr_get_current_band(void);
extern boolean rr_is_supported_band(rr_internal_band_T band);
extern byte rr_get_current_rf_power_class(void);
extern byte rr_get_band_rf_power_class(rr_internal_band_T band);
extern byte rr_get_supported_band_count(void);

extern sys_band_class_e_type rr_get_band_indicator_for_nas(boolean);
extern sys_band_class_e_type rr_ds_get_band_indicator_for_nas(boolean                in_service,
                                                              sys_modem_as_id_e_type as_id);
extern rr_internal_band_T rr_ds_get_current_band( sys_modem_as_id_e_type as_id );
extern byte rr_ds_get_supported_band_count( sys_modem_as_id_e_type as_id );
extern byte rr_ds_get_current_rf_power_class( sys_modem_as_id_e_type as_id );
extern boolean rr_ds_is_supported_band(rr_internal_band_T        band,
                                       sys_modem_as_id_e_type    as_id);
extern byte rr_ds_get_band_rf_power_class(rr_internal_band_T band,
                                          sys_modem_as_id_e_type as_id);





#endif
