#ifndef RR_ARFCN_H
#define RR_ARFCN_H

/*==============================================================================

                R R   A R F C N   ---   H E A D E R   F I L E

GENERAL DESCRIPTION
  This module contains definitions and functions related to handling of GSM
  ARFCNs.

EXTERNALIZED FUNCTIONS
  rr_arfcn_compare_freq
  rr_arfcn_compare
  rr_arfcn_hash
  rr_arfcn_unhash
  rr_arfcn_get_freq_index
  rr_arfcn_freq_index_to_freq

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007-2013 Qualcomm Technologies, Inc.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_arfcn.h#1 $

==============================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"


#include "sys_type.h"
#include "sys.h"
#include "sys_v.h"
#include "rr_l1.h"
#include "rr_l1_g.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

/* Maximum, minimum and total number of channel numbers used for PGSM 900
band */
#define RR_ARFCN_PGSM_900_MIN               1
#define RR_ARFCN_PGSM_900_MAX               124
#define RR_ARFCN_PGSM_900_COUNT             124

/* Maximum, minimum and total number of channel numbers used for EGSM 900
band (lower range) */
#define RR_ARFCN_EGSM_900_LOWER_RANGE_MIN   0
#define RR_ARFCN_EGSM_900_LOWER_RANGE_MAX   124
#define RR_ARFCN_EGSM_900_LOWER_RANGE_COUNT 125

/* Maximum, minimum and total number of channel numbers used for EGSM 900
band (upper range) */
#define RR_ARFCN_EGSM_900_UPPER_RANGE_MIN   975
#define RR_ARFCN_EGSM_900_UPPER_RANGE_MAX   1023
#define RR_ARFCN_EGSM_900_UPPER_RANGE_COUNT 49

/* Maximum, minimum and total number of channel numbers used for DCS 1800
band */
#define RR_ARFCN_DCS_1800_MIN               512
#define RR_ARFCN_DCS_1800_MAX               885
#define RR_ARFCN_DCS_1800_COUNT             374

/* Maximum, minimum and total number of channel numbers used for PCS 1900
band */
#define RR_ARFCN_PCS_1900_MIN               512
#define RR_ARFCN_PCS_1900_MAX               810
#define RR_ARFCN_PCS_1900_COUNT             299

/* Maximum, minimum and total number of channel numbers used for Cellular
850 band */
#define RR_ARFCN_CELL_850_MIN               128
#define RR_ARFCN_CELL_850_MAX               251
#define RR_ARFCN_CELL_850_COUNT             124


/* This defines the number of unique ARFCNs across the bands that we support.
Note that this is different to the number of supported frequencies, as there is
some overlap between PGSM and EGSM. */
#define RR_ARFCN_HASH_MAX (RR_ARFCN_PGSM_900_COUNT + \
                           RR_ARFCN_EGSM_900_LOWER_RANGE_COUNT + \
                           RR_ARFCN_EGSM_900_UPPER_RANGE_COUNT + \
                           RR_ARFCN_DCS_1800_COUNT + \
                           RR_ARFCN_PCS_1900_COUNT + \
                           RR_ARFCN_CELL_850_COUNT)

/**
 * This defines the maximum number of unique GSM frequencies that are supported.
 */
#define RR_ARFCN_FREQ_MAX       971
#define RR_ARFCN_FREQ_INDEX_MAX 971

/*==============================================================================
  Typedefs
==============================================================================*/

/**
 * This type represents an ARFCN that has been hashed to reduce it to a single
 * unique value.
 */
typedef uint16 rr_arfcn_hash_t;

/**
 * There are 971 unique frequencies used by the GSM bands supported by the UE.
 * Each ARFCN (channel & band) maps to a single frequency, but there are some
 * frequencies that map to multople ARFCNs due to the overlap of PGSM and EGSM
 * bands.
 *
 * Each of the 971 unique frequencies can be assigned a unique index.  This
 * type can be seen as a one-way hash of an ARFCN to it's associated frequency
 * index.  This is useful for comparing ARFCNs in the PGSM or EGSM bands to
 * verify whether they are indeed equal.
 */
typedef uint16 rr_arfcn_freq_index_t;

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/**
 * This function checks whether 2 ARFCNs correspond to the same absolute radio
 * frequency.  This function takes in to account the overlap of PGSM and EGSM,
 * so for example an ARFCN with band set to PGSM and channel number set to 2
 * is equal to an ARFCN with band set to EGSM and channel number set to 2.
 *
 * @param arfcn1 The first ARFCN to compare
 * @param arfcn2 The second ARFCN to compare
 * @return TRUE if arfcn1 is the same frequency as arfcn2, FALSE otherwise.
 */
extern boolean rr_arfcn_compare_freq(const ARFCN_T arfcn1,
                                     const ARFCN_T arfcn2);
/**
 * This function checks whether 2 ARFCNs correspond to the same absolute radio
 * frequency.  This function does not take in to account the overlap between
 * PGSM and EGSM bands, e.g. ARFCN 2 in PGSM is NOT considered equal to ARFCN 2
 * in EGSM band.
 *
 * @param arfcn1 The first ARFCN
 * @param arfcn2 The second ARFCN
 * @return TRUE if the ARFCNs are identical, FALSE otherwise.
 */
extern boolean rr_arfcn_compare(const ARFCN_T arfcn1, const ARFCN_T arfcn2);

#if 0 // not currently used
/**
 * Hashes the ARFCN provided and returns a unique value representing it.
 *
 * @param arfcn The ARFCN to hash.
 * @return A unique value representing the ARFCN, or RR_ARFCN_HASH_MAX if the
 *         hashing operation fails.
 */
extern rr_arfcn_hash_t rr_arfcn_hash(const ARFCN_T arfcn);

/**
 * Performs the reverse of rr_arfcn_hash.
 *
 * @param hashed_arfcn The hashed ARFCN to unhash.
 * @return The ARFCN.  The return value .num field will be set to INVALID_ARFCN
 *         if the operation fails.
 */
extern ARFCN_T rr_arfcn_unhash(const rr_arfcn_hash_t hashed_arfcn);
#endif // #if 0

/**
 * This function returns the frequency index of the ARFCN.  The frequencies for
 * all ARFCNs are mapped in to the range 0 - 970.
 *
 * @param arfcn The ARFCN to map to a frequency.
 * @return The frequency index of the ARFCN, or RR_ARFCN_FREQ_INDEX_MAX if the
 *         calculation fails (i.e. the ARFCN is not supported).
 */
extern rr_arfcn_freq_index_t rr_arfcn_get_freq_index(const ARFCN_T arfcn);

/**
 * Get the frequency (in MHz) that corresponds to the frequency index
 * specified.
 *
 * @param freq_index The frequency index to get the frequency for.
 * @return The frequency corresponding to the frequency index, or 0 if it
 *         is not a valid frequency index.
 */
extern double rr_arfcn_freq_index_to_freq(rr_arfcn_freq_index_t freq_index);

/**
 * Converts a frequency to a frequency index.  If the specified frequency does
 * not correspond exactly to a GSM frequency, the nearest frequency index will
 * be returned.
 *
 * @param freq The frequency to convert.
 * @return rr_arfcn_freq_index_t The frequency index that corresponds to the
 *                               specified frequency, RR_ARFCN_FREQ_INDEX_MAX
 *                               if an error occurs.
 */
extern rr_arfcn_freq_index_t rr_arfcn_freq_to_freq_index(double freq);

/**
  @brief Returns the frequency of the ARFCN in Hz.

  @param arfcn The ARFCN to get the frequency of.
  @return The frequency of the ARFCN in Hz.
*/
extern uint32 rr_arfcn_frequency(
  const ARFCN_T arfcn
);

#endif /* RR_ARFCN_H */
