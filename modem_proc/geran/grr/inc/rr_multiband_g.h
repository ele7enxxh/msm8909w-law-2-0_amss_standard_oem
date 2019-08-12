/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/rr_multiband_g.h#1 $    $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
19/10/10    sjw     Added support for FEATURE_DUAL_SIM
10-04-01   tjw     Add customer.h and comdef.h to all SU API files--
07/01/09    sjw     Moved INVALID_ARFCN and ARFCNS_EQUAL into gprsdef.h
05/01/09    sjw     Change band indicator function prototype for FTM compatibility
Mar/19/04    VT     QuadBand: Added some comments about SYS_BAND_*
09/25/02    rmc     Fix compilation warnings
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
06/05/02    rmc     Adding multiband info accessors & modifiers for shared data

===========================================================================*/
#ifndef RR_MULTIBAND_G_H
#define RR_MULTIBAND_G_H

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "environ.h"
#include "sys_stru.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "sys_type.h"

/* Define a bitmask of all possible GSM bands. */
#define RR_SYS_BAND_MASK_GSM_ALL_BANDS (SYS_BAND_MASK_GSM_DCS_1800 | \
                                        SYS_BAND_MASK_GSM_EGSM_900 | \
                                        SYS_BAND_MASK_GSM_PGSM_900 | \
                                        SYS_BAND_MASK_GSM_450 | \
                                        SYS_BAND_MASK_GSM_480 | \
                                        SYS_BAND_MASK_GSM_750 | \
                                        SYS_BAND_MASK_GSM_850 | \
                                        SYS_BAND_MASK_GSM_RGSM_900 | \
                                        SYS_BAND_MASK_GSM_PCS_1900)

/* Define a bitmask of all GSM bands that the lower layers (RR/L1/RF) support. */
#define RR_SYS_BAND_MASK_GSM_SUPPORTED_BANDS (SYS_BAND_MASK_GSM_DCS_1800 | \
                                              SYS_BAND_MASK_GSM_EGSM_900 | \
                                              SYS_BAND_MASK_GSM_PGSM_900 | \
                                              SYS_BAND_MASK_GSM_850 | \
                                              SYS_BAND_MASK_GSM_PCS_1900)
#define MAX_NUM_MONITOR_GSM 40
#define MAX_NUM_MONITOR_DCS 55
#define MAX_NUM_MONITOR_PCS 50
#define MAX_NUM_MONITOR_CELL 40

#define MAX_NUM_MONITOR_GSM_DEFAULT 35
#define MAX_NUM_MONITOR_DCS_DEFAULT 50
#define MAX_NUM_MONITOR_PCS_DEFAULT 45
#define MAX_NUM_MONITOR_CELL_DEFAULT 35

/* ARFCN_T helper macros, see also gcommon/inc/gprsdef.h */
#define ARFCN_VALID_CHANNEL(a) ((a).num != INVALID_ARFCN)

  /* ARFCN_T structure contains channel num and band info */
  #define GET_ARFCN_BAND(a) ((rr_internal_band_T) (a).band)

#define RR_MIN_VALID_BAND_VALUE BAND_EGSM
#define RR_MAX_VALID_BAND_VALUE BAND_GSM_850

/**
 * Determines if the given band value is valid or not
 */
#define RR_IS_VALID_BAND(band) ((band) <= RR_MAX_VALID_BAND_VALUE)

/**********************************************/
/* Max. Number of power classes for each band */
/**********************************************/
#define RR_BAND_NUM_POWER_CLASSES 5

/********************************************/
/* Max number of power levels for each band */
/********************************************/
#define RR_BAND_NUM_POWER_LEVELS 32

typedef struct
{

  /* TRUE if a band is indicated in band_pref parameter */
  boolean preferred;

  /* TRUE if the band is supported by UE at any given time. For each band, this is
     set to TRUE or FALSE depending on the return value of sd_misc_get_supp_band_pref()
     which is called by rr_initialise() */

  boolean     supported;

  /**
   * minimum ARFCN value for the band
   */
  word        min_arfcn;

  /**
   * max ARFCN value for the band
   */
  word        max_arfcn;

  /**
   * max power in each mobile class for the band
   */
  byte        max_power[RR_BAND_NUM_POWER_CLASSES];

  /**
   * output power for each power level in the band
   */
  byte        output_power[RR_BAND_NUM_POWER_LEVELS];

  /**
   * power class of the mobile for the given band
   */
  byte        rf_power_class;
} band_info_T;

typedef struct
{
  band_info_T band[MAXNO_BANDS];
  rr_internal_band_T  current_band;
  rr_internal_band_T  current_dedicated_band;
} multiband_info_T;

/* SI1, SI6, PSI1 band indicator */
typedef enum
{
  SI_PSI_BAND_IND_DCS1800,
  SI_PSI_BAND_IND_PCS1900
} si_psi_band_ind_T;

extern multiband_info_T * rr_get_multiband_info(const gas_id_t gas_id);
extern void rr_init_band_support_info(const gas_id_t gas_id);

extern boolean rr_is_preferred_band(rr_internal_band_T band, const gas_id_t gas_id);
extern void    rr_set_preferred_band(rr_internal_band_T band, boolean allowed, const gas_id_t gas_id);

extern void    rr_set_supported_band(rr_internal_band_T band, boolean supported, const gas_id_t gas_id);

extern boolean rr_check_in_band(rr_internal_band_T band,word  arfcn, const gas_id_t gas_id);
extern void rr_set_current_band(rr_internal_band_T band, const gas_id_t gas_id);

extern void rr_reset_current_band(const gas_id_t gas_id);

extern void rr_set_current_dedicated_band(rr_internal_band_T band, const gas_id_t gas_id);
extern rr_internal_band_T rr_get_current_dedicated_band(const gas_id_t gas_id);

extern rr_internal_band_T rr_get_band(word arfcn, const gas_id_t gas_id);

extern boolean rr_gapi_is_supported_band(sys_band_T band, const gas_id_t gas_id);
extern byte rr_gapi_get_band_rf_power_class(rr_internal_band_T band, const gas_id_t gas_id);
extern byte rr_gapi_get_supported_band_count(const gas_id_t gas_id);

#endif /* #ifndef RR_MULTIBAND_G_H */

/* EOF */


