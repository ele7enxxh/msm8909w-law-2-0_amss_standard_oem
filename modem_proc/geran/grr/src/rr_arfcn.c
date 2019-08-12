/*==============================================================================

                             R R   A R F C N

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_arfcn.c#1 $

==============================================================================*/


/*==============================================================================

                      INCLUDE FILES FOR MODULE

==============================================================================*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"

#include "rr_seg_load.h"
#include "rr_arfcn.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "msg.h"

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/*==============================================================================
  Constants and Macros
==============================================================================*/

/**
 * Hashed values for PGSM 900 ARFCNs are in the range 0 - 123
 */
#define RR_ARFCN_HASH_PGSM_900_MIN \
  (0)

#define RR_ARFCN_HASH_PGSM_900_MAX \
  (RR_ARFCN_HASH_PGSM_900_MIN + RR_ARFCN_PGSM_900_COUNT)

/**
 * Hashed values for EGSM 900 (lower range) ARFCNs are in the range 124 -
 */
#define RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MIN \
  (RR_ARFCN_HASH_PGSM_900_MAX + 1)

#define RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MAX \
  (RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MIN + RR_ARFCN_EGSM_900_LOWER_RANGE_COUNT)

/**
 * Hashed values for Cellular 850 ARFCNs are in the following range:
 */
#define RR_ARFCN_HASH_CELL_850_MIN \
  (RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MAX + 1)

#define RR_ARFCN_HASH_CELL_850_MAX \
  (RR_ARFCN_HASH_CELL_850_MIN + RR_ARFCN_CELL_850_COUNT)

/**
 * Hashed values for DCS 1800 ARFCNs are in the following range:
 */
#define RR_ARFCN_HASH_DCS_1800_MIN \
  (RR_ARFCN_HASH_CELL_850_MAX + 1)

#define RR_ARFCN_HASH_DCS_1800_MAX \
  (RR_ARFCN_HASH_DCS_1800_MIN + RR_ARFCN_DCS_1800_COUNT)

/**
 * Hashed values for PCS 1900 ARFCNs are in the following range:
 */
#define RR_ARFCN_HASH_PCS_1900_MIN \
  (RR_ARFCN_HASH_DCS_1800_MAX + 1)

#define RR_ARFCN_HASH_PCS_1900_MAX \
  (RR_ARFCN_HASH_PCS_1900_MIN + RR_ARFCN_PCS_1900_COUNT)

/**
 * Hashed values for EGSM 900 (upper range) ARFCNs are in the following range:
 */
#define RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_MIN \
  (RR_ARFCN_HASH_PCS_1900_MAX + 1)

#define RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_MAX \
  (RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_MIN + RR_ARFCN_EGSM_900_UPPER_RANGE_COUNT)

/* Offset added to PGSM 900 channel numbers to calculate their hashed value. */
#define RR_ARFCN_HASH_PGSM_900_OFFSET \
  ((RR_ARFCN_HASH_PGSM_900_MIN - RR_ARFCN_PGSM_900_MIN))

/**
 * The following definitions are for offsets that are applied to channel
 * numbers in each band to calculate the hashed value of the ARFCN.
 *
 * Generally, the offset is calculated by taking the minimum hashed value for
 * the band (defined above) and subtracting the minimum valid channel number
 * for the band, so taking PGSM as an example, the minimum hashed value is
 * 0, and the minimum channel number is 1.  This gives an offset of -1, which
 * maps the PGSM channel numbers in to the range 0 - 123.
 */

/* Offset added to EGSM 900 (lower range) channel numbers to calculate their
hashed value. */
#define RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_OFFSET \
  ((RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MIN - RR_ARFCN_EGSM_900_LOWER_RANGE_MIN))

/* Offset added to DCS 1800 channel numbers to calculate their hashed value. */
#define RR_ARFCN_HASH_DCS_1800_OFFSET \
  ((RR_ARFCN_HASH_DCS_1800_MIN - RR_ARFCN_DCS_1800_MIN))

/* Offset added to Cellular 850 channel numbers to calculate their hashed value. */
#define RR_ARFCN_HASH_CELL_850_OFFSET \
  ((RR_ARFCN_HASH_CELL_850_MIN - RR_ARFCN_CELL_850_MIN))

/* Offset added to PCS 1900 channel numbers to calculate their hashed value. */
#define RR_ARFCN_HASH_PCS_1900_OFFSET \
  ((RR_ARFCN_HASH_PCS_1900_MIN - RR_ARFCN_PCS_1900_MIN))

/* Offset added to EGSM 900 (upper range) channel numbers to calculate their hashed value. */
#define RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_OFFSET \
  ((RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_MIN - RR_ARFCN_EGSM_900_UPPER_RANGE_MIN))

/**
 * Offsets that should be added to channel numbers to calculate the frequency
 * index of a GSM ARFCN.  This is effectively a one-way hash as band information
 * (PGSM or EGSM) is lost.  This hashing allows an easy comparison of ARFCNs
 * in PGSM and EGSM to see if they correspond to the same frequency.
 *
 * The frequency indexes in the mapped space are arranged in ascending order
 * of absolute frequency.
 *
 * PGSM frequencies resolve to the same indexes as the corresponding EGSM
 * frequencies.
 *
 * These values map the supported ARFCNs as follows:
 *
 *               ARFCN RANGE     MAPPED VALUE   OFFSET
 * Cellular 850  128 -> 251      0 -> 123       -128
 * EGSM 900      975 -> 1023     124 -> 172     -851
 * PGSM 900      1 -> 124        174 -> 297     173
 * EGSM 900      0 -> 124        173 -> 297     173
 * DCS 1800      512 -> 885      298 -> 671     -214
 * PCS 1900      512 -> 810      672 -> 971     160
 */
#define RR_ARFCN_FREQ_INDEX_CELL_850_OFFSET             -128
#define RR_ARFCN_FREQ_INDEX_EGSM_900_UPPER_RANGE_OFFSET -851
#define RR_ARFCN_FREQ_INDEX_PGSM_900_OFFSET             173
#define RR_ARFCN_FREQ_INDEX_EGSM_900_LOWER_RANGE_OFFSET 173
#define RR_ARFCN_FREQ_INDEX_DCS_1800_OFFSET             -214
#define RR_ARFCN_FREQ_INDEX_PCS_1900_OFFSET             160

#define RR_MHZ                            (1000000UL)
#define RR_KHZ                            (1000UL)

/* See 3GPP 45.005 Table 2-2 */

/* Uplink base frequencies per band in Hertz. */
#define RR_UL_FREQUENCY_BASE_PGSM_900  (890000UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_EGSM_900  (890000UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_RGSM_900  (890000UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_DCS_1800 (1710200UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_PCS_1900 (1850200UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_GSM_450   (450600UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_GSM_480   (479000UL * RR_KHZ)
#define RR_UL_FREQUENCY_BASE_GSM_850   (824200UL * RR_KHZ)

/* Downlink base frequencies per band in Hertz. */
#define RR_DL_FREQUENCY_BASE_PGSM_900 (RR_UL_FREQUENCY_BASE_PGSM_900 + (45UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_EGSM_900 (RR_UL_FREQUENCY_BASE_EGSM_900 + (45UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_RGSM_900 (RR_UL_FREQUENCY_BASE_RGSM_900 + (45UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_DCS_1800 (RR_UL_FREQUENCY_BASE_DCS_1800 + (95UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_PCS_1900 (RR_UL_FREQUENCY_BASE_PCS_1900 + (80UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_GSM_450  (RR_UL_FREQUENCY_BASE_GSM_450  + (10UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_GSM_480  (RR_UL_FREQUENCY_BASE_GSM_480  + (10UL * RR_MHZ))
#define RR_DL_FREQUENCY_BASE_GSM_850  (RR_UL_FREQUENCY_BASE_GSM_850  + (45UL * RR_MHZ))

/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Variables
==============================================================================*/

#define RR_ARFCN_FREQ_BASE_ARRAY_MAX    4

#define RR_FREQ_LOWEST   ((double) 869.2)
#define RR_FREQ_HIGHEST  ((double) 1989.8)

static const double rr_arfcn_frequencies_base[] =
{
  /* index 0 - 123 */
  869.200000, /* Cellular 850 ARFCN 128 */

  /* index 124 - 297 */
  925.200000, /* EGSM 900 (upper range) ARFCN 975 */

  /* index 298 - 671 */
  1805.200000, /* DCS 1800 ARFCN 512 */

  /* index 672 - 970 */
  1930.200000, /* PCS 1900 ARFCN 512 */
};

static const uint16 rr_arfcn_frequencies_index[] =
{
  0,
  124,
  298,
  672,
  RR_ARFCN_FREQ_INDEX_MAX
};

/*==============================================================================
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/


/**
 * Checks whether two ARFCNs correspond to the same GSM frequency.
 *
 * @param arfcn1 The first ARFCN.
 * @param arfcn2 The second ARFCN.
 * @return TRUE if the ARFCNs correspond to the same GSM frequency, FALSE
 *         otherwise.
 */
boolean rr_arfcn_compare_freq(const ARFCN_T arfcn1, const ARFCN_T arfcn2)
{
  rr_arfcn_freq_index_t freq_index1 = rr_arfcn_get_freq_index(arfcn1);
  rr_arfcn_freq_index_t freq_index2 = rr_arfcn_get_freq_index(arfcn2);

  if (freq_index1 == RR_ARFCN_FREQ_INDEX_MAX ||
      freq_index2 == RR_ARFCN_FREQ_INDEX_MAX)
  {
    return FALSE;
  }
  else if (freq_index1 == freq_index2)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/**
 * Compare 2 ARFCNs to see if they are identical.  Both the channel number and
 * band must be the same.
 *
 * @param arfcn1 The first ARFCN.
 * @param arfcn2 The second ARFCN.
 * @return TRUE if the channel numers and bands match, FALSE otherwise.
 */
boolean rr_arfcn_compare(const ARFCN_T arfcn1, const ARFCN_T arfcn2)
{
  if (arfcn1.num == arfcn2.num &&
      arfcn1.band == arfcn2.band)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


#if 0 // not currently used

/**
 * Hash an ARFCN in to a unique value.  This is a two-way hash.
 */
rr_arfcn_hash_t rr_arfcn_hash(const ARFCN_T arfcn)
{
  rr_arfcn_hash_t hashed_arfcn = RR_ARFCN_HASH_MAX;

  switch (arfcn.band)
  {
    case SYS_BAND_PGSM_900:
    {
      if (arfcn.num >= RR_ARFCN_PGSM_900_MIN &&
          arfcn.num <= RR_ARFCN_PGSM_900_MAX)
      {
        hashed_arfcn = (rr_arfcn_hash_t) (arfcn.num + RR_ARFCN_HASH_PGSM_900_OFFSET);
      }
      else
      {
        MSG_GERAN_ERROR_2_G("ARFCN number %d out of range for band %d", arfcn.num, arfcn.band);
      }
      break;
    }

    case SYS_BAND_EGSM_900:
    {
      if (/*arfcn.num >= RR_ARFCN_EGSM_900_LOWER_RANGE_MIN &&*/
          arfcn.num <= RR_ARFCN_EGSM_900_LOWER_RANGE_MAX)
      {
        hashed_arfcn = (rr_arfcn_hash_t) (arfcn.num + RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_OFFSET);
      }
      else if (arfcn.num >= RR_ARFCN_EGSM_900_UPPER_RANGE_MIN &&
               arfcn.num <= RR_ARFCN_EGSM_900_UPPER_RANGE_MAX)
      {
        hashed_arfcn = (rr_arfcn_hash_t) (arfcn.num + RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_OFFSET);
      }
      else
      {
        MSG_GERAN_ERROR_2_G("ARFCN number %d out of range for band %d", arfcn.num, arfcn.band);
      }
    }
    break;

    case SYS_BAND_DCS_1800:
    {
      if (arfcn.num >= RR_ARFCN_DCS_1800_MIN &&
          arfcn.num <= RR_ARFCN_DCS_1800_MAX)
      {
        hashed_arfcn = (rr_arfcn_hash_t) (arfcn.num + RR_ARFCN_HASH_DCS_1800_OFFSET);
      }
      else
      {
        MSG_GERAN_ERROR_2_G("ARFCN number %d out of range for band %d", arfcn.num, arfcn.band);
      }
    }
    break;

    case SYS_BAND_PCS_1900:
    {
      if (arfcn.num >= RR_ARFCN_PCS_1900_MIN &&
          arfcn.num <= RR_ARFCN_PCS_1900_MAX)
      {
        hashed_arfcn = (rr_arfcn_hash_t) (arfcn.num + RR_ARFCN_HASH_PCS_1900_OFFSET);
      }
      else
      {
        MSG_GERAN_ERROR_2_G("ARFCN number %d out of range for band %d", arfcn.num, arfcn.band);
      }
    }
    break;

    case SYS_BAND_CELL_850:
    {
      if (arfcn.num >= RR_ARFCN_CELL_850_MIN &&
          arfcn.num <= RR_ARFCN_CELL_850_MAX)
      {
        hashed_arfcn = (rr_arfcn_hash_t) (arfcn.num + RR_ARFCN_HASH_CELL_850_OFFSET);
      }
      else
      {
        MSG_GERAN_ERROR_2_G("ARFCN number %d out of range for band %d", arfcn.num, arfcn.band);
      }
    }
    break;

    default:
      MSG_GERAN_ERROR_2_G("ARFCN(%d,%d) band is unexpected value",
                arfcn.num,
                arfcn.band);
  }

  if (hashed_arfcn > RR_ARFCN_HASH_MAX)
  {
    MSG_GERAN_ERROR_2_G("Hashed ARFCN %d is out of range, returning %d",
              hashed_arfcn,
              RR_ARFCN_HASH_MAX);

    hashed_arfcn = RR_ARFCN_HASH_MAX;
  }

  return hashed_arfcn;
}


/**
 * Performs the reverse of rr_arfcn_hash.
 */
ARFCN_T rr_arfcn_unhash(const rr_arfcn_hash_t hashed_arfcn)
{
  ARFCN_T arfcn;

  arfcn.num = INVALID_ARFCN;
  arfcn.band = SYS_BAND_NONE;

  /* rr_arfcn_hash_t is unsigned, by definition greater than or equal to zero */
  if (/*hashed_arfcn >= RR_ARFCN_HASH_PGSM_900_MIN &&*/
      hashed_arfcn <= RR_ARFCN_HASH_PGSM_900_MAX)
  {
    arfcn.num = (uint16) (hashed_arfcn - RR_ARFCN_HASH_PGSM_900_OFFSET);
    arfcn.band = SYS_BAND_PGSM_900;
  }

  else if (hashed_arfcn >= RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MIN &&
           hashed_arfcn <= RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_MAX)
  {
    arfcn.num = (uint16) (hashed_arfcn - RR_ARFCN_HASH_EGSM_900_LOWER_RANGE_OFFSET);
    arfcn.band = SYS_BAND_EGSM_900;
  }

  else if (hashed_arfcn >= RR_ARFCN_HASH_CELL_850_MIN &&
           hashed_arfcn <= RR_ARFCN_HASH_CELL_850_MAX)
  {
    arfcn.num = (uint16) (hashed_arfcn - RR_ARFCN_HASH_CELL_850_OFFSET);
    arfcn.band = SYS_BAND_CELL_850;
  }

  else if (hashed_arfcn >= RR_ARFCN_HASH_DCS_1800_MIN &&
           hashed_arfcn <= RR_ARFCN_HASH_DCS_1800_MAX)
  {
    arfcn.num = (uint16) (hashed_arfcn - RR_ARFCN_HASH_DCS_1800_OFFSET);
    arfcn.band = SYS_BAND_DCS_1800;
  }

  else if (hashed_arfcn >= RR_ARFCN_HASH_PCS_1900_MIN &&
           hashed_arfcn <= RR_ARFCN_HASH_PCS_1900_MAX)
  {
    arfcn.num = (uint16) (hashed_arfcn - RR_ARFCN_HASH_PCS_1900_OFFSET);
    arfcn.band = SYS_BAND_PCS_1900;
  }

  else if (hashed_arfcn >= RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_MIN &&
           hashed_arfcn <= RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_MAX)
  {
    arfcn.num = (uint16) (hashed_arfcn - RR_ARFCN_HASH_EGSM_900_UPPER_RANGE_OFFSET);
    arfcn.band = SYS_BAND_EGSM_900;
  }

  else
  {
    MSG_GERAN_ERROR_1_G("Hashed arfcn %d is out of range", hashed_arfcn);
  }

  return arfcn;
}

#endif // #if 0


/**
 * See header file for description.
 */
rr_arfcn_freq_index_t rr_arfcn_get_freq_index(const ARFCN_T arfcn)
{
  rr_arfcn_freq_index_t freq_index = RR_ARFCN_FREQ_INDEX_MAX;

  switch (arfcn.band)
  {
    case SYS_BAND_CELL_850:
    {
      if (arfcn.num >= RR_ARFCN_CELL_850_MIN &&
          arfcn.num <= RR_ARFCN_CELL_850_MAX)
      {
        freq_index = (rr_arfcn_freq_index_t) (arfcn.num + RR_ARFCN_FREQ_INDEX_CELL_850_OFFSET);
      }
      else
      {
        MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
      }
      break;
    }

    case SYS_BAND_PGSM_900:
    {
      if (arfcn.num >= RR_ARFCN_PGSM_900_MIN &&
          arfcn.num <= RR_ARFCN_PGSM_900_MAX)
      {
        freq_index = (rr_arfcn_freq_index_t) (arfcn.num + RR_ARFCN_FREQ_INDEX_PGSM_900_OFFSET);
      }
      else
      {
        MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
      }
      break;
    }

    case SYS_BAND_EGSM_900:
    {
      if (/*arfcn.num >= RR_ARFCN_EGSM_900_LOWER_RANGE_MIN &&*/
          arfcn.num <= RR_ARFCN_EGSM_900_LOWER_RANGE_MAX)
      {
        freq_index = (rr_arfcn_freq_index_t) (arfcn.num + RR_ARFCN_FREQ_INDEX_EGSM_900_LOWER_RANGE_OFFSET);
      }
      else if (arfcn.num >= RR_ARFCN_EGSM_900_UPPER_RANGE_MIN &&
               arfcn.num <= RR_ARFCN_EGSM_900_UPPER_RANGE_MAX)
      {
        freq_index = (rr_arfcn_freq_index_t) (arfcn.num + RR_ARFCN_FREQ_INDEX_EGSM_900_UPPER_RANGE_OFFSET);
      }
      else
      {
        MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
      }
      break;
    }

    case SYS_BAND_DCS_1800:
    {
      if (arfcn.num >= RR_ARFCN_DCS_1800_MIN &&
          arfcn.num <= RR_ARFCN_DCS_1800_MAX)
      {
        freq_index = (rr_arfcn_freq_index_t) (arfcn.num + RR_ARFCN_FREQ_INDEX_DCS_1800_OFFSET);
      }
      else
      {
        MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
      }
      break;
    }

    case SYS_BAND_PCS_1900:
    {
      if (arfcn.num >= RR_ARFCN_PCS_1900_MIN &&
          arfcn.num <= RR_ARFCN_PCS_1900_MAX)
      {
        freq_index = (rr_arfcn_freq_index_t) (arfcn.num + RR_ARFCN_FREQ_INDEX_PCS_1900_OFFSET);
      }
      else
      {
        MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
      }
      break;
    }

    default:
    {
      MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
    }
  }

  return freq_index;
}


/**
 * See header file for description.
 */
double rr_arfcn_freq_index_to_freq(rr_arfcn_freq_index_t freq_index)
{
  double freq = 0;

  byte i;

  for (i=0; i<RR_ARFCN_FREQ_BASE_ARRAY_MAX; i++)
  {
    if (freq_index >= rr_arfcn_frequencies_index[i] && freq_index < rr_arfcn_frequencies_index[i+1])
    {
      freq = rr_arfcn_frequencies_base[i] + (freq_index - rr_arfcn_frequencies_index[i]) * 0.2;
      return freq;
    }
  }

  //MSG_ERROR("ARFCN frequency index %d invalid", freq_index, 0, 0);
  return freq;
}

rr_arfcn_freq_index_t rr_arfcn_freq_to_freq_index(double freq)
{
  rr_arfcn_freq_index_t index;
  double f;
  byte i;

  if (freq < RR_FREQ_LOWEST || freq > RR_FREQ_HIGHEST)
  {
    return RR_ARFCN_FREQ_INDEX_MAX;
  }

  for (i=0; i<RR_ARFCN_FREQ_BASE_ARRAY_MAX; i++)
  {
    for (index = rr_arfcn_frequencies_index[i]; index < rr_arfcn_frequencies_index[i+1]; index++)
    {
      f = rr_arfcn_frequencies_base[i] + 0.2 * (index - rr_arfcn_frequencies_index[i]);

      if (f > freq)
      {
        if (index > 0)
        {
          return index - 1;
        }
        else
        {
          return RR_ARFCN_FREQ_INDEX_MAX;
        }
      }
      else if (f == freq)
      {
        return index;
      }
    }
  }

  //MSG_ERROR("ARFCN frequency %d invalid", freq, 0, 0);
  return RR_ARFCN_FREQ_INDEX_MAX;
}

uint32 rr_arfcn_frequency(const ARFCN_T arfcn)
{
  uint32 frequency = 0;

  switch (arfcn.band)
  {
    case SYS_BAND_PGSM_900:
    {
      if (arfcn.num >= RR_ARFCN_PGSM_900_MIN &&
          arfcn.num <= RR_ARFCN_PGSM_900_MAX)
      {
        frequency = RR_DL_FREQUENCY_BASE_PGSM_900;
        frequency += (200 * RR_KHZ) * arfcn.num;
      }
      else
      {
        MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
      }
      break;
    } /* SYS_BAND_PGSM_900 */

    case SYS_BAND_EGSM_900:
    {
      if (arfcn.num <= RR_ARFCN_EGSM_900_LOWER_RANGE_MAX)
      {
        frequency = RR_DL_FREQUENCY_BASE_EGSM_900;
        frequency += (200 * RR_KHZ) * arfcn.num;
      }
      else if (arfcn.num >= RR_ARFCN_EGSM_900_UPPER_RANGE_MIN &&
               arfcn.num <= RR_ARFCN_EGSM_900_UPPER_RANGE_MAX)
      {
        frequency = RR_DL_FREQUENCY_BASE_EGSM_900;
        frequency += (200 * RR_KHZ) * (arfcn.num - 1024);
      }
      else
      {
        MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
      }
      break;
    } /* SYS_BAND_EGSM_900 */

    case SYS_BAND_DCS_1800:
    {
      if (arfcn.num >= RR_ARFCN_DCS_1800_MIN &&
          arfcn.num <= RR_ARFCN_DCS_1800_MAX)
      {
        frequency = RR_DL_FREQUENCY_BASE_DCS_1800;
        frequency += (200 * RR_KHZ) * (arfcn.num - 512);
      }
      else
      {
        MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
      }
      break;
    } /* SYS_BAND_DCS_1800 */

    case SYS_BAND_PCS_1900:
    {
      if (arfcn.num >= RR_ARFCN_PCS_1900_MIN &&
          arfcn.num <= RR_ARFCN_PCS_1900_MAX)
      {
        frequency = RR_DL_FREQUENCY_BASE_PCS_1900;
        frequency += (200 * RR_KHZ) * (arfcn.num - 512);
      }
      else
      {
        MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
      }
      break;
    } /* SYS_BAND_PCS_1900 */

    case SYS_BAND_CELL_850:
    {
      if (arfcn.num >= RR_ARFCN_CELL_850_MIN &&
          arfcn.num <= RR_ARFCN_CELL_850_MAX)
      {
        frequency = RR_DL_FREQUENCY_BASE_GSM_850;
        frequency += (200 * RR_KHZ) * (arfcn.num - 128);
      }
      else
      {
        MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
      }
      break;
    } /* SYS_BAND_CELL_850 */

    default:
    {
      MSG_GERAN_ERROR_2("ARFCN (%d, %d) not valid", arfcn.num, arfcn.band);
    }
  }

  return frequency;
}


// Public API

/**
  @brief Returns the frequency of the ARFCN in Hz.

  @param arfcn The ARFCN to get the frequency of.
  @return The frequency of the ARFCN in Hz.
*/
uint32 geran_api_grr_arfcn_frequency(const ARFCN_T arfcn)
{
  return rr_arfcn_frequency(arfcn);
}


// UTF support

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

