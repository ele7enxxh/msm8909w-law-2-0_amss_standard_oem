/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Decode IE Module

GENERAL DESCRIPTION
  Utility functions for the decoding of IEs associated with OTA messages

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_decode_ie.c_v   1.4   17 Jul 2002 11:50:32   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_decode_ie.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
06/11/13    nn      CR546531 Use explicit pointer error checking instead of ASSERT
04/04/05  DA    Added a call to rr_log_multirate_parameters() from function
          rr_decode_multirate_config_ie() to report multirate parameters (AMR).
May/18/04   VT      Changed all non-NULL ptr ASSERTs to MSG_ERRORs.
04/29/04    gc      CR42000: Fixes to handle Early Classmark Sending Restriction
Jan/28/04    VT     QuadBand: In rr_decode_channel_description() getting the
                      channel band using rr_get_band(), instead of getting from
                      SI DB, for inter-band intra-cell HO to work.
Oct/21/03    VT     In rr_decode_ia_rest_octets(), additionally checking if
                      starting_time_valid is FALSE.
02/05/02    JAC     Lint changes
01/07/02    JAC     Fixed multirate config length and number of modes check/calculation
11/21/02    JAC     Fixed format support bug in rr_decode_frequency_list_ie when in PGSM
11/18/02    RG      Fixes bugs from unit-testing; review comments for GSM to WCDMA reselection.
11/07/02    JAC     Added multirate config ie
10/21/02    RG      Added GSM to WCDMA Reselection support (SI2 Qtr decode ).
08/26/02    rmc     Removed FEATURE_MULTIMODE_GSM feature use
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
07/09/02    rmc     Use accessors to get multiband information
07/08/02    rmc     Removed CBQ2 (not supported)
04/11/02    JAC     added stored plmn support
02/08/02    rmc     QXDM Cleanup
12/21/01    RMC     Added decoding functions for more IEs
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include <stdlib.h>
#include <string.h>
#include "rr_seg_load.h"
#include "rr_decode_ie.h"
#include "rr_general.h"
#include "rr_gprs_debug.h"
#include "rr_log.h"
#include "gs.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "gprs_mem.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "gsbitstream.h"
#ifdef FEATURE_VAMOS
#include "rr_nv.h"
#endif
#include "rr_multi_sim.h"
#include "stringl/stringl.h"
#include "rr_sys_info_cache.h"

#define RR_FREQUENCY_LIST_W_ARRAY_LENGTH 64

static word rr_get_bit_field( byte   field[],int   bit_number,int   bit_field_size ,int array_field_size );

static void rr_decode_set_band(ARFCN_T *arfcn_list_ptr, byte num_entries, const gas_id_t gas_id);

static void rr_decode_frequency_list_ie
(
  byte *IE,
  ARFCN_T channel_number,
  cell_channel_description_T *frequency_list,
  boolean *decoding_ok,
  const gas_id_t gas_id
);


/*
 *   Function name:  rr_decode_neighbour_cell_description
 *   ----------------------------------------------------
 *
 *   Description: Decode the neighbour cell description IE
 *   ------------
 *
 *   Parameters: The IE
 *   -----------
 *
 *   Returns:   The IE decoded
 *   --------
 *
 */

void rr_decode_neighbour_cell_description
(
  byte                  *IE,
  byte                  *EXT_IND,
  BCCH_frequency_list_T *BCCH_frequency_list,
  boolean               *decoding_ok,
  const gas_id_t         gas_id
)
{
  cell_channel_description_T cell_channel_description;
  byte BA_IND;
  byte index;

  BA_IND   = (byte) (IE[0] & 0x10) >> 4;
  *EXT_IND = (byte) (IE[0] & 0x20) >> 5;

  rr_decode_cell_channel_description(IE, &cell_channel_description,decoding_ok, gas_id);

  if (*decoding_ok == TRUE)
  {
    // The Cell Channel Description can contain upto 64 entries.
    // This should be limited to 32 before storing it as a BA-list
    if (cell_channel_description.no_of_entries > BA_MAX_LEN)
    {
      cell_channel_description.no_of_entries = BA_MAX_LEN;
    }

    for (index = 0; index < cell_channel_description.no_of_entries; index++)
    {
      BCCH_frequency_list->BCCH_ARFCN[index] =
      cell_channel_description.channel_number[index];
    }

    BCCH_frequency_list->no_of_entries = cell_channel_description.no_of_entries;
    BCCH_frequency_list->BA_IND = BA_IND;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("Can't decode neighbour description");
    BCCH_frequency_list->no_of_entries = 0;
  }
}

/*
 *   Function name:  rr_decode_neighbour_cell_description_2
 *   ------------------------------------------------------
 *
 *   Description: Decode the neighbour cell description 2 IE
 *   ------------ This is very similar to "neighbour cell description",
 *                except that it contains multiband reporting rather
 *                than EXT_IND
 *
 *   Parameters: The IE
 *   -----------
 *
 *   Returns:   The IE decoded
 *   --------
 *
 */

void rr_decode_neighbour_cell_description_2
(
  byte                  *IE,
  byte                  *multiband_reporting,
  BCCH_frequency_list_T *BCCH_frequency_list,
  boolean               *decoding_ok,
  const gas_id_t         gas_id
)
{
  cell_channel_description_T cell_channel_description;
  byte BA_IND;
  byte index;

  /* extract the BA_IND flag from bit 125 */
  BA_IND   = (byte) (IE[0] & 0x10) >> 4;

  /*
   * Need to get the multiband_reporting value, from bits 127 and 126 and
   * then overwrite bits 127, 126 **and** 125 of the IE with zero so that
   * the IE looks like an ordinary cell channel description.
   * See 3GPP 44.018 v6.17.0 sections 10.5.2.22a and 10.5.2.1b
   */
  *multiband_reporting = (byte) (IE[0] & 0x60) >> 5;
  IE[0] &= 0x8f; /* i.e. mask is 10001111 to remove bits 127, 126 and 125 */
  rr_decode_cell_channel_description(IE, &cell_channel_description, decoding_ok, gas_id);

  /*
   * just in case it is required, replace the multiband_reporting and
   * BA_IND parameters in the first octet of the IE before returning
   */

  IE[0] |= (byte)(((*multiband_reporting) & 0x03) << 5);
  IE[0] |= (byte)((BA_IND & 0x01) << 4);

  if (*decoding_ok == TRUE)
  {
    // The Cell Channel Description can contain upto 64 entries.
    // This should be limited to 32 before storing it as a BA-list
    if (cell_channel_description.no_of_entries > BA_MAX_LEN)
    {
      cell_channel_description.no_of_entries = BA_MAX_LEN;
    }

    for (index = 0; index < cell_channel_description.no_of_entries; index++)
    {
      BCCH_frequency_list->BCCH_ARFCN[index] = cell_channel_description.channel_number[index];
    }

    BCCH_frequency_list->no_of_entries = cell_channel_description.no_of_entries;
    BCCH_frequency_list->BA_IND = BA_IND;
  }
  else
  {
    BCCH_frequency_list->no_of_entries = 0;
  }
}


/*
 *   Function name:  rr_decode_cell_description
 *   ---------------------------------------------
 *
 *   Description: Decode the Cell Description IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:  The IE decoded
 *   --------
 *
 *
 */

void rr_decode_cell_description(
  byte *IE,
  cell_description_T *cell_description,
  const gas_id_t gas_id
)
{
  word HIGH_ARFCN;
  word LOW_ARFCN;

  HIGH_ARFCN = (word) (IE[0] & 0xC0) << 2;
  LOW_ARFCN = IE[1];

  cell_description->BSIC.PLMN_colour_code = (byte) (IE[0] & 0x38) >> 3;
  cell_description->BSIC.BS_colour_code = (byte) (IE[0] & 0x07);

  cell_description->ARFCN.num = HIGH_ARFCN | LOW_ARFCN;
  // WARNING: Need to resolve band information using SI
  cell_description->ARFCN.band = (sys_band_T) rr_get_band(cell_description->ARFCN.num, gas_id);
}

/*
 *   Function name:  rr_decode_channel_description
 *   ---------------------------------------------
 *
 *   Description: Decode the channel description IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:   The decoded IE
 *   --------
 *
 *
 */

void rr_decode_channel_description(
  byte *IE,
  channel_information_T *channel_information,
  boolean *decoding_ok,
  const gas_id_t gas_id
)
{
  byte TN         = 0;
  byte channel    = 0;
  byte MAIO       = 0;
  byte MAIO_LOW   = 0;
  byte MAIO_HIGH  = 0;
  byte TSC        = 0;
  byte H          = 0;
  word ARFCN      = 0;
  word ARFCN_LOW  = 0;
  word ARFCN_HIGH = 0;
  byte HSN        = 0;
#ifdef FEATURE_VAMOS
  byte TSC_S      = 0;
#endif // FEATURE_VAMOS

  *decoding_ok = TRUE;

  TN = IE[0] & 0x07;
  channel = (byte)(IE[0] & 0xF8) >> 3;
  TSC = (byte)(IE[1] & 0xE0) >> 5;

  H = (byte)(IE[1] & 0x10) >> 4;

  if (H == 1)
  {
    MAIO_HIGH = (byte)(IE[1] & 0x0F) << 2;
    MAIO_LOW = (byte)(IE[2] & 0xC0) >> 6;

    MAIO = MAIO_LOW | MAIO_HIGH;

    HSN = IE[2] & 0x3F;
  }

  else
  {
    ARFCN_HIGH = (word)(IE[1] & 0x03) << 8;
    ARFCN_LOW = IE[2];

    ARFCN = ARFCN_LOW | ARFCN_HIGH;
  }

  /*
  from 3GPP 44.018, Table 10.5.2.5.1 Channel Description IE

  Bits
  8 7 6 5 4
  S 0 0 0 1  TCH/F
  S 0 0 1 T  TCH/H
  0 0 1 T T  SDCCH/4
  0 1 T T T  SDCCH/8

  where T indicates subchannel
        S indicates TSC set (0=Set 1, 1=Set 2)

  Note: 'S' is added for VAMOS. For Pre-VAMOS specifications, 'S' should be read as '0'
  */

  /* Note: Decoding of 'channel' above has shifted it into bits 0-4 */

  /* TCH/F */
#ifdef FEATURE_VAMOS
  if ((channel & 0x0f) == 0x01)    // 0x0f = 0 1 1 1 1   0x01 = 0 0 0 0 1
#else
  if (channel == 0x01)             //                    0x01 = 0 0 0 0 1
#endif // FEATURE_VAMOS
  {
    channel_information->channel_type = FULL_RATE_TRAFFIC;
    GS_SET_UNDEFINED(channel_information->subchannel);
#ifdef FEATURE_VAMOS
    TSC_S = (channel & 0x10) >> 4;  // 0x10 = 1 0 0 0 0  TSC_S = {0,1}
#endif // FEATURE_VAMOS
    MSG_GERAN_HIGH_0_G("channel_type=FULL_RATE_TRAFFIC, subchannel=n/a");
  }
  else
  /* TCH/H */
#ifdef FEATURE_VAMOS
  if ((channel & 0x0e) == 0x02)    // 0x0e = 0 1 1 1 0   0x02 = 0 0 0 1 0
#else
  if ((channel & 0x1e) == 0x02)    // 0x1e = 1 1 1 1 0   0x02 = 0 0 0 1 0
#endif // FEATURE_VAMOS
  {
    channel_information->channel_type = HALF_RATE_TRAFFIC;
    channel_information->subchannel = channel & 0x01;
#ifdef FEATURE_VAMOS
    TSC_S = (channel & 0x10) >> 4;  // 0x10 = 1 0 0 0 0  TSC_S = {0,1}
#endif // FEATURE_VAMOS
    MSG_GERAN_HIGH_1_G("channel_type=HALF_RATE_TRAFFIC, subchannel=%x",(int)(channel & 0x01));
  }
  else
  /* SDCCH/4 */
  if ((channel & 0x1c) == 0x04)    // 0x1c = 1 1 1 0 0   0x04 = 0 0 1 0 0
  {
    channel_information->channel_type = SDCCH_4;
    channel_information->subchannel = channel & 0x03;
    MSG_GERAN_HIGH_1_G("channel_type=SDCCH_4, subchannel=%x",(int)(channel & 0x03));
  }
  else
  /* SDCCH/8 */
  if ((channel & 0x18) == 0x08)    // 0x18 = 1 1 0 0 0   0x08 = 0 1 0 0 0
  {
    channel_information->channel_type = SDCCH_8;
    channel_information->subchannel = channel & 0x07;
    MSG_GERAN_HIGH_1_G("channel_type=SDCCH_8, subchannel=%x",(int)(channel & 0x07));
  }
  else
  {
    MSG_GERAN_ERROR_1_G("Bad channel value(%d) in channel description",(int)channel);
    *decoding_ok = FALSE;
  }

  channel_information->TN = TN;
  channel_information->TSC = TSC;
#ifdef FEATURE_VAMOS
  channel_information->TSC |= (TSC_S << 3);
  MSG_GERAN_HIGH_2_G("TSC_S=%x => TSC=%x",(int)TSC_S,(int)channel_information->TSC);
#endif // FEATURE_VAMOS
  channel_information->hopping_flag = H;

  if (H == 1)
  {
    channel_information->MAIO = MAIO;
    channel_information->HSN = HSN;
  }
  else
  {
    channel_information->frequency_list.no_of_items = 1;
    channel_information->frequency_list.channel[0].num = ARFCN;
    channel_information->frequency_list.channel[0].band = (sys_band_T) rr_get_band(ARFCN, gas_id);
  }

#ifdef FEATURE_VAMOS
  // If TSC2 is specified but VAMOS support is disabled, this channel mode is invalid
  if ((TSC_S == 1) && (!rr_get_nv_vamos_enabled(RR_GAS_ID_TO_AS_ID)))
  {
    MSG_GERAN_ERROR_0_G("TSC Set 2 specified but VAMOS disabled in NV - reject");
    *decoding_ok = FALSE;
  }
#endif // FEATURE_VAMOS
}

/*
 *   Function name:  rr_decode_channel_description_2
 *   ---------------------------------------------
 *
 *   Description: Decode the channel description 2 IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:   The decoded IE
 *   --------
 *
 *
 */

void rr_decode_channel_description_2(
  byte *IE,
  channel_information_T *channel_information,
  boolean *decoding_ok,
  const gas_id_t gas_id
)
{
  byte TN         = 0;
  byte channel    = 0;
  byte MAIO       = 0;
  byte MAIO_LOW   = 0;
  byte MAIO_HIGH  = 0;
  byte TSC        = 0;
  byte H          = 0;
  word ARFCN      = 0;
  word ARFCN_LOW  = 0;
  word ARFCN_HIGH = 0;
  byte HSN        = 0;
#ifdef FEATURE_VAMOS
  byte TSC_S      = 0;
#endif // FEATURE_VAMOS

  *decoding_ok = TRUE;

  TN = IE[0] & 0x07;
  channel = (byte)(IE[0] & 0xF8) >> 3;
  TSC = (byte)(IE[1] & 0xE0) >> 5;

  H = (byte)(IE[1] & 0x10) >> 4;

  if (H == 1)
  {
    MAIO_HIGH = (byte)(IE[1] & 0x0F) << 2;
    MAIO_LOW = (byte)(IE[2] & 0xC0) >> 6;

    MAIO = MAIO_LOW | MAIO_HIGH;

    HSN = IE[2] & 0x3F;
  }

  else
  {
    ARFCN_HIGH = (word)(IE[1] & 0x03) << 8;
    ARFCN_LOW = IE[2];

    ARFCN = ARFCN_LOW | ARFCN_HIGH;
  }

  /*
  from 3GPP 44.018, Table 10.5.2.5.1 Channel Description IE

  Bits
  8 7 6 5 4
  0 0 0 0 1  TCH/F
  0 0 0 1 T  TCH/H
  0 0 1 T T  SDCCH/4
  0 1 T T T  SDCCH/8

  1 1 0 0 0  TCH/F using TSC Set 2
  1 1 1 0 T  TCH/H using TSC Set 2

  where T indicates subchannel

  Note: TSC Set 2 is added for VAMOS. If TSC Set 2 is not directly specified, TSC Set 1 is assumed
  */

  /* Note: Decoding of 'channel' above has shifted it into bits 0-4 */

  /* TCH/F */
  if (channel == 0x01)            //                    0x01 = 0 0 0 0 1
  {
    channel_information->channel_type = FULL_RATE_TRAFFIC;
    GS_SET_UNDEFINED(channel_information->subchannel);
    MSG_GERAN_HIGH_0_G("channel_type=FULL_RATE_TRAFFIC, subchannel=n/a");
  }
  else
  /* TCH/H */
  if ((channel & 0x1e) == 0x02)    // 0x1e = 1 1 1 1 0   0x02 = 0 0 0 1 0
  {
    channel_information->channel_type = HALF_RATE_TRAFFIC;
    channel_information->subchannel = channel & 0x01;
    MSG_GERAN_HIGH_1_G("channel_type=HALF_RATE_TRAFFIC, subchannel=%x",(int)(channel & 0x01));
  }
  else
  /* SDCCH/4 */
  if ((channel & 0x1c) == 0x04)    // 0x1c = 1 1 1 0 0   0x04 = 0 0 1 0 0
  {
    channel_information->channel_type = SDCCH_4;
    channel_information->subchannel = channel & 0x03;
    MSG_GERAN_HIGH_1_G("channel_type=SDCCH_4, subchannel=%x",(int)(channel & 0x03));
  }
  else
  /* SDCCH/8 */
  if ((channel & 0x18) == 0x08)    // 0x18 = 1 1 0 0 0   0x08 = 0 1 0 0 0
  {
    channel_information->channel_type = SDCCH_8;
    channel_information->subchannel = channel & 0x07;
    MSG_GERAN_HIGH_1_G("channel_type=SDCCH_8, subchannel=%x",(int)(channel & 0x07));
  }
#ifdef FEATURE_VAMOS
  else
  /* TCH/F using TSC Set 2 */
  if (channel == 0x18)            // 0x18 = 1 1 0 0 0
  {
    channel_information->channel_type = FULL_RATE_TRAFFIC;
    GS_SET_UNDEFINED(channel_information->subchannel);
    TSC_S = 1;                     // TSC_S = 1  =>  TSC Set 2
    MSG_GERAN_HIGH_0_G("channel_type=FULL_RATE_TRAFFIC, subchannel=n/a");
  }
  else
  /* TCH/H using TSC Set 2 */
  if ((channel & 0x1e) == 0x1c)    // 0x1e = 1 1 1 1 0   0x1c = 1 1 1 0 0
  {
    channel_information->channel_type = HALF_RATE_TRAFFIC;
    channel_information->subchannel = channel & 0x01;
    TSC_S = 1;                     // TSC_S = 1  =>  TSC Set 2
  }
#endif // FEATURE_VAMOS
  else
  {
    MSG_GERAN_ERROR_1_G("Bad channel value(%d) in channel description",(int)channel);
    *decoding_ok = FALSE;
  }

  channel_information->TN = TN;
  channel_information->TSC = TSC;
#ifdef FEATURE_VAMOS
  channel_information->TSC |= (TSC_S << 3);
  MSG_GERAN_HIGH_2_G("TSC_S=%x => TSC=%x",(int)TSC_S,(int)channel_information->TSC);
#endif // FEATURE_VAMOS
  channel_information->hopping_flag = H;

  if (H == 1)
  {
    channel_information->MAIO = MAIO;
    channel_information->HSN = HSN;
  }
  else
  {
    channel_information->frequency_list.no_of_items = 1;
    channel_information->frequency_list.channel[0].num = ARFCN;
    channel_information->frequency_list.channel[0].band = (sys_band_T) rr_get_band(ARFCN, gas_id);
  }

#ifdef FEATURE_VAMOS
  // If TSC2 is specified but VAMOS support is disabled, this channel mode is invalid
  if ((TSC_S == 1) && (!rr_get_nv_vamos_enabled(RR_GAS_ID_TO_AS_ID)))
  {
    *decoding_ok = FALSE;
    MSG_GERAN_ERROR_0_G("TSC Set 2 specified but VAMOS disabled in NV - reject");
  }
#endif // FEATURE_VAMOS
}

/*
 *   Function name:  rr_decode_channel_mode
 *   --------------------------------------
 *
 *   Description: Decode the channel mode IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:  The channel mode
 *   --------
 *
 *
 */

void rr_decode_channel_mode(byte *IE, channel_mode_T *channel_mode,boolean *decoding_ok)
{
  *decoding_ok = TRUE;
  *channel_mode = IE[0];

  switch (IE[0])
  {

    /*
     * list all valid values
     */

    case SIGNALLING_ONLY:
    case SPEECH_V1:
    case SPEECH_V2:
    case SPEECH_V3:
    case DATA_14400:
    case DATA_9600:
    case DATA_4800:
    case DATA_2400:
#ifdef FEATURE_GSM_AMR_WB
    case SPEECH_V5:
#endif /*FEATURE_GSM_AMR_WB  */
 
#ifdef FEATURE_VAMOS_II
    case SPEECH_V1_VAMOS2:
    case SPEECH_V2_VAMOS2:
    case SPEECH_V3_VAMOS2:

#ifdef FEATURE_GSM_AMR_WB
    case SPEECH_V5_VAMOS2:
#endif /*FEATURE_GSM_AMR_WB  */

#endif /*FEATURE_VAMOS_II*/
      break;

    default :
    {
      MSG_GERAN_ERROR_1("Bad value(%d) in channel mode IE",(int) IE[0]);
      *decoding_ok = FALSE;
    }
  }
}

/*
 *   Function name:  greatest_power
 *   ------------------------------
 *
 *   Description:
 *   ------------
 *
 *
 *   Parameters:
 *   -----------
 *
 *
 *
 *   Returns:
 *   --------
 *
 *
 */

/*************************************************************************************/
/* Made greatest_power into a macro to speed up the decode function, which calls the */
/* function repeatedly                                                               */
/*************************************************************************************/

word greatest_power_imp(word index)
{
  unsigned int result = 0;
  word old_result = 0;

  if (index == 0)
  {
    old_result = 0;
  }
  else if (index == 1)
  {
    old_result = 1;
  }
  else
  {
    result = 1;

    while (result <= index)
    {
      old_result =  (word) result;
      result = result << 1;
    }
  }

  return old_result;
}

/*
 *   Function name:  decode
 *   ----------------------
 *
 *   Description:
 *   ------------
 *
 *
 *   Parameters:
 *   -----------
 *
 *
 *
 *   Returns:
 *   --------
 *
 *
 */
#ifdef PERLUTF
#error code not present
#else
static boolean decode(word *W, byte size, word *offset, uint8 offset_elements, word original_range)
#endif // PERLUTF
{
  int range;
  word index;
  int n;
  word k;
  int temp;
  boolean decoding_ok = TRUE;

  for (k = 1; k < (size + 1); k++)
  {
    index = k;
    range = original_range / greatest_power(index);
    n = W[index];

    while (index > 1)
    {
      range = 2 * range + 1;

      if ((2 * index) < (3 * greatest_power(index)))
      {

        /**************/
        /* Left Child */
        /**************/

        index = (word) (index - (greatest_power(index) / 2));

        temp = n + (int) (W[index]) - 1 - ((range - 1) / 2);

        while (temp < 0)
        {
          temp += range;
        }

        if (temp == range)
        {
          n = temp;
        }

        else if (temp == 0)
        {
          n = range;
        }

        else
        {
          n = temp % range;
        }
      }

      else
      {

        /***************/
        /* Right Child */
        /***************/

        index = index - greatest_power(index);

        temp = n + W[index];

        while (temp < 0)
        {
          temp += range;
        }

        if (temp == range)
        {
          n = temp;
        }

        else if (temp == 0)
        {
          n = range;
        }

        else
        {
          n = temp % range;
        }
      }
    }
    if ( n > 65535 )
    {
      MSG_GERAN_ERROR_1("Error in decode(%d)",(int) n);
      n = 0;
      decoding_ok = FALSE;
    }

    if ((k-1) == offset_elements)
    {
      MSG_GERAN_ERROR_2("RR decode overrun array, k:%d, offset_size:%d",k,offset_elements);
      decoding_ok = FALSE;
      break;
    }

    offset[k-1] = (word) n;
  }

  return decoding_ok;
}

/*
 *   Function name:  compare
 *   -----------------------
 *
 *   Description: Compare 2 parameters and returns either -1, 0 or 1
 *   ------------
 *
 *
 *   Parameters: the 2 parameters to compare
 *   -----------
 *
 *
 *
 *   Returns:  The result of the comparison
 *   --------
 *
 *
 */

int compare(const void *arg1, const void *arg2)
{
  word *num1;
  word *num2;

  num1 = (word *) arg1;
  num2 = (word *) arg2;

  if (*num1 < *num2)
  {
    return -1;
  }

  else if (*num1 == *num2)
  {
    return 0;
  }

  else
  {
    return 1;
  }

}

#ifdef FEATURE_WCDMA

static int compare_rr_rrc_fdd_cell_type_scrambling_codes(
  const void * arg1,
  const void * arg2
)
{
  uint32 sc1;
  uint32 sc2;

  sc1 = ((rr_rrc_fdd_cell_type *) arg1)->scrambling_code;
  sc2 = ((rr_rrc_fdd_cell_type *) arg2)->scrambling_code;


  if (sc1 < sc2)
  {
    return -1;
  }
  else if (sc1 > sc2)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

#endif /* FEATURE_WCDMA */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
static int compare_rr_rrc_tdd_cell_type_cpid(
  const void * arg1,
  const void * arg2
)
{
  uint32 cpid1;
  uint32 cpid2;

  cpid1 = ((tdsrrc_cell_type *) arg1)->cpid;
  cpid2 = ((tdsrrc_cell_type *) arg2)->cpid;


  if (cpid1 < cpid2)
  {
    return -1;
  }
  else if (cpid1 > cpid2)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

/*
 *   Function name:  rr_decode_cell_channel_description
 *   --------------------------------------------------
 *
 *   Description: Decode the Cell Allocation IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:     The decoded IE
 *   --------
 *
 *
 */

void rr_decode_cell_channel_description
(
  byte                        *IE,
  cell_channel_description_T  *cell_allocation,
  boolean                     *decoding_ok,
  const gas_id_t               gas_id
)
{
  byte ARFCN_mask;
  byte RRFCN_mask;
  word RRFCN;
  word CA_ARFCN;
  byte index;
  byte bit_index;
  byte ca_index = 0;
  word ORIG_ARFCN;
  byte size = 0;
  word offset[CA_MAX_LEN];
  word W[29];
  byte F0;
  boolean support = TRUE;

  *decoding_ok = TRUE;

  if (((byte)(IE[0] & 0xC0) >> 6) == 0)
  {

    /*****************************************************/
    /* The cell allocation format is a bit map format    */
    /*****************************************************/

    ARFCN_mask = 0x01;
    CA_ARFCN = 0;
    cell_allocation->no_of_entries = 0;

    index = 16;

    do
    {
      index--;

      for (bit_index = 0; bit_index < 8; bit_index++)
      {
        CA_ARFCN++;

        if ((IE[index] & ARFCN_mask) == ARFCN_mask)
        {
          if (ca_index < CA_MAX_LEN)
          {
            cell_allocation->channel_number[ca_index].num = CA_ARFCN;
            // WARNING: Need to resolve band information using SI
            cell_allocation->channel_number[ca_index].band = (sys_band_T) rr_get_band(CA_ARFCN, gas_id);
            cell_allocation->no_of_entries++;
            ca_index++;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Overflow in cell allocation detected, ignoring further entries");
          }
        }

        if ((bit_index == 3) && (index == 0))
        {
          break;
        }

        else if (ARFCN_mask != 0x80)
        {
          ARFCN_mask = (byte) (ARFCN_mask << 1);
        }

        else
        {
          ARFCN_mask = 0x01;
        }
      }

    } while (index != 0);
  }
  /*lint -save -e774  */
  else if (((IE[0] & 0xC0) == 0x80) && ((IE[0] & 0x08) == 0x00) && (support == TRUE))
  /*lint -restore */
  {

    /***************************************************/
    /* The cell allocation format is 1024 range format */
    /***************************************************/

    F0 = (word) (IE[0] & 0x04) >> 2;

    if (F0 == 1)
    {

      /**************************************/
      /* The ARFCN 0 is a member of the set */
      /**************************************/

      cell_allocation->channel_number[0].num = 0;
      // WARNING: Need to resolve band information using SI
      cell_allocation->channel_number[0].band = (sys_band_T) rr_get_band(0, gas_id);
      cell_allocation->no_of_entries = 1;
    }
    else
    {
      cell_allocation->no_of_entries = 0;
    }

    /*************************/
    /* Decode the 16 weights */
    /*************************/

    W[1] = (word) (IE[0] & 0x03) << 8;
    W[1] |= (word) IE[1];

    W[2] = (word) (IE[2] << 1);
    W[2] |= (word) (IE[3] & 0x80) >> 7;

    W[3] = ((word) (IE[3] & 0x7F) << 2);
    W[3] |= (word) (IE[4] & 0xCF) >> 6;

    W[4] = (word) (IE[4] & 0x3F) << 2;
    W[4] |= (word) (IE[5] & 0xCF) >> 6;

    W[5] = (word) (IE[5] & 0x3F) << 2;
    W[5] |= (word) (IE[6] & 0xCF) >> 6;

    W[6] = (word) (IE[6] & 0x3F) << 2;
    W[6] |= (word) (IE[7] & 0xCF) >> 6;

    W[7] = (word) (IE[7] & 0x3F) << 2;
    W[7] |= (word) (IE[8] & 0xCF) >> 6;

    W[8] = (word) (IE[8] & 0x3F) << 1;
    W[8] |= (word) (IE[9] & 0x80) >> 7;

    W[9] = IE[9] & 0x7F;

    W[10] = (word) (IE[10] & 0xFE) >> 1;

    W[11] = (word) (IE[10] & 0x01) << 6;
    W[11] |= (word) (IE[11] & 0xFC) >> 2;

    W[12] = (word) (IE[11] & 0x03) << 5;
    W[12] |= (word) (IE[12] & 0xF8) >> 3;

    W[13] = (word) (IE[12] & 0x07) << 4;
    W[13] |= (word) (IE[13] & 0xF0) >> 4;

    W[14] = (word) (IE[13] & 0x0F) << 3;
    W[14] |= (word) (IE[14] & 0xE0) >> 5;

    W[15] = (word) (IE[14] & 0x1F) << 2;
    W[15] |= (word) (IE[15] & 0xC0) >> 6;

    W[16] = (word) (IE[15] & 0x3F);

    index = 1;

    while (index < 17)
    {
      if (W[index] != 0)
      {
        index++;
        size++;
      }
      else
      {
        break;
      }
    }

    if (!decode(W, size, offset, NELEMS(offset), 1023))
    {
      *decoding_ok = FALSE;
    }

    if (cell_allocation->no_of_entries == 1)
    {
      for (index = 1; index < (size + 1); index++)
      {
        cell_allocation->channel_number[index].num = offset[index-1];
        // WARNING: Need to resolve band information using SI
        cell_allocation->channel_number[index].band = (sys_band_T) rr_get_band(offset[index-1], gas_id);
      }
      qsort_arfcn(cell_allocation->channel_number,size + 1, gas_id);
    }

    else
    {
      for (index = 0; index < size; index++)
      {
        cell_allocation->channel_number[index].num = offset[index];
        // WARNING: Need to resolve band information using SI
        cell_allocation->channel_number[index].band = (sys_band_T) rr_get_band(offset[index], gas_id);
      }

      qsort_arfcn(cell_allocation->channel_number, size, gas_id);
    }

    cell_allocation->no_of_entries += size;

  }
  /*lint -save -e774  */
  else if (((IE[0] & 0xC0) == 0x80) && ((IE[0] & 0x0E) == 0x08) && (support == TRUE))
  /*lint -restore */
  {

    /**************************************************/
    /* The cell allocation format is 512 range format */
    /**************************************************/

    ORIG_ARFCN = (word) (IE[0] & 0x01) << 9;
    ORIG_ARFCN |= (word) (IE[1]) << 1;
    ORIG_ARFCN |= (word) (IE[2] & 0x80) >> 7;

    W[1] = (word) (IE[2] & 0x7F) << 2;
    W[1] |= (word) (IE[3] & 0xC0) >> 6;

    W[2] = (word) (IE[3] & 0x3F) << 2;
    W[2] |= (word) (IE[4] & 0xC0) >> 6;

    W[3] = (word) (IE[4] & 0x3F) << 2;
    W[3] |= (word) (IE[5] & 0xC0) >> 6;

    W[4] = (word) (IE[5] & 0x3F) << 1;
    W[4] |= (word) (IE[6] & 0x80) >> 7;

    W[5] = IE[6] & 0x7F;

    W[6] = (word) (IE[7] & 0xFE) >> 1;

    W[7] = (word) (IE[7] & 0x01) << 6;
    W[7] |= (word) (IE[8] & 0xFC) >> 2;

    W[8] = (word) (IE[8] & 0x03) << 4;
    W[8] |= (word) (IE[9] & 0xF0) >> 4;

    W[9] = (word) (IE[9] & 0x0F) << 2;
    W[9] |= (word) (IE[10] & 0xC0) >> 6;

    W[10] = IE[10] & 0x3F;

    W[11] = (word) (IE[11] & 0xFC) >> 2;

    W[12] = (word) (IE[11] & 0x03) << 4;
    W[12] |= (word) (IE[12] & 0xF0) >> 4;

    W[13] = (word) (IE[12] & 0x0F) << 2;
    W[13] |= (word) (IE[13] & 0xC0) >> 6;

    W[14] = IE[13] & 0x3F;

    W[15] = (word) (IE[14] & 0xFC) >> 2;

    W[16] = (word) (IE[14] & 0x03) << 3;
    W[16] |= (word) (IE[15] & 0xE0) >> 5;

    W[17] = IE[15] & 0x1F;

    index = 1;

    while (index < 18)
    {
      if (W[index] != 0)
      {
        index++;
        size++;
      }
      else
      {
        break;
      }
    }

    if (!decode(W, size, offset, NELEMS(offset), 511))
    {
      *decoding_ok = FALSE;
    }

    cell_allocation->no_of_entries = size + 1;

    cell_allocation->channel_number[0].num = ORIG_ARFCN;
    // WARNING: Need to resolve band information using SI
    cell_allocation->channel_number[0].band = (sys_band_T) rr_get_band(ORIG_ARFCN, gas_id);

    for (index = 1; index < (size + 1); index++)
    {
      cell_allocation->channel_number[index].num =
      (ORIG_ARFCN + offset[index-1]) % 1024;
      // WARNING: Need to resolve band information using SI
      cell_allocation->channel_number[index].band = (sys_band_T) rr_get_band(
        cell_allocation->channel_number[index].num,
        gas_id
      );
    }

    qsort_arfcn(cell_allocation->channel_number,size + 1, gas_id);
  }
  /*lint -save -e774  */
  else if (((IE[0] & 0xC0) == 0x80) && ((IE[0] & 0x0E) == 0x0A) && (support == TRUE))
  /*lint -restore */
  {

    /****************************************************/
    /* The cell allocation format is a 256 range format */
    /****************************************************/

    ORIG_ARFCN = (word) (IE[0] & 0x01) << 9;
    ORIG_ARFCN |= (word) (IE[1]) << 1;
    ORIG_ARFCN |= (word) (IE[2] & 0x80) >> 7;

    W[1] = (word) (IE[2] & 0x7F) << 1;
    W[1] |= (word) (IE[3] & 0x80) >> 7;

    W[2] = IE[3] & 0x7F;

    W[3] = (word) (IE[4] & 0xFE) >> 1;

    W[4] = (word) (IE[4] & 0x01) << 5;
    W[4] |= (word) (IE[5] & 0xF8) >> 3;

    W[5] = (word) (IE[5] & 0x07) << 3;
    W[5] |= (word) (IE[6] & 0xE0) >> 5;

    W[6] = (word) (IE[6] & 0x1F) << 1;
    W[6] |= (word) (IE[7] & 0x80) >> 7;

    W[7] = (word) (IE[7] & 0x7E) >> 1;

    W[8] = (word) (IE[7] & 0x01) << 4;
    W[8] |= (word) (IE[8] & 0xF0) >> 4;

    W[9] = (word) (IE[8] & 0x0F) << 1;
    W[9] |= (word) (IE[9] & 0x80) >> 7;

    W[10] = (word) (IE[9] & 0x7C) >> 2;

    W[11] = (word) (IE[9] & 0x03) << 3;
    W[11] |= (word) (IE[10] & 0xE0) >> 5;

    W[12] = IE[10] & 0x1F;

    W[13] = (word) (IE[11] & 0xF8) >> 3;

    W[14] = (word) (IE[11] & 0x07) << 2;
    W[14] |= (word) (IE[12] & 0xC0) >> 6;

    W[15] = (word) (IE[12] & 0x3E) >> 1;

    W[16] = (word) (IE[12] & 0x01) << 3;
    W[16] |= (word) (IE[13] & 0xE0)>> 5;

    W[17] = (word) (IE[13] & 0x1E) >> 1;

    W[18] = (word) (IE[13] & 0x01) << 3;
    W[18] |= (word) (IE[14] & 0xE0)>> 5;

    W[19] = (word) (IE[14] & 0x1E) >> 1;

    W[20] = (word) (IE[14] & 0x01) << 3;
    W[20] |= (word) (IE[15] & 0xE0)>> 5;

    W[21] = (word) (IE[15] & 0x1E) >> 1;

    index = 1;

    while (index < 22)
    {
      if (W[index] != 0)
      {
        index++;
        size++;
      }
      else
      {
        break;
      }
    }

    if (!decode(W, size, offset, NELEMS(offset), 255))
    {
      *decoding_ok = FALSE;
    }

    cell_allocation->no_of_entries = size + 1;

    cell_allocation->channel_number[0].num = ORIG_ARFCN;
    // WARNING: Need to resolve band information using SI
    cell_allocation->channel_number[0].band = (sys_band_T) rr_get_band(ORIG_ARFCN, gas_id);

    for (index = 1; index < (size + 1); index++)
    {
      cell_allocation->channel_number[index].num =
      (ORIG_ARFCN + offset[index-1]) % 1024;
      // WARNING: Need to resolve band information using SI
      cell_allocation->channel_number[index].band = (sys_band_T) rr_get_band(
        cell_allocation->channel_number[index].num,
        gas_id
      );
    }

    qsort_arfcn(cell_allocation->channel_number,size + 1, gas_id);
  }
  /*lint -save -e774  */
  else if (((IE[0] & 0xC0) == 0x80) && ((IE[0] & 0x0E) == 0x0C) && (support == TRUE))
  /*lint -restore */
  {

    /****************************************************/
    /* The cell allocation format is a 128 range format */
    /****************************************************/

    ORIG_ARFCN = (word) (IE[0] & 0x01) << 9;
    ORIG_ARFCN |= (word) (IE[1]) << 1;
    ORIG_ARFCN |= (word) (IE[2] & 0x80) >> 7;

    W[1] = IE[2] & 0x7F;

    W[2] = (word) (IE[3] & 0xFC) >> 2;

    W[3] = (word) (IE[3] & 0x03) << 4;
    W[3] |= (word) (IE[4] & 0xF0) >> 4;

    W[4] = (word) (IE[4] & 0x0F) << 1;
    W[4] |= (word) (IE[5] & 0x80) >> 7;

    W[5] = (word) (IE[5] & 0x7C) >> 2;

    W[6] = (word) (IE[5] & 0x03) << 3;
    W[6] |= (word) (IE[6] & 0xE0) >> 5;

    W[7] = IE[6] & 0x1F;

    W[8] = (word) (IE[7] & 0xF0) >> 4;

    W[9] = IE[7] & 0x0F;

    W[10] = (word) (IE[8] & 0xF0) >> 4;

    W[11] = IE[8] & 0x0F;

    W[12] = (word) (IE[9] & 0xF0) >> 4;

    W[13] = IE[9] & 0x0F;

    W[14] = (word) (IE[10] & 0xF0) >> 4;

    W[15] = IE[10] & 0x0F;

    W[16] = (word) (IE[11] & 0xE0) >> 5;

    W[17] = (word) (IE[11] & 0x1C) >> 2;

    W[18] = (word) (IE[11] & 0x03) << 1;
    W[18] |= (word) (IE[12] & 0x80) >> 7;

    W[19] = (word) (IE[12] & 0x70) >> 4;

    W[20] = (word) (IE[12] & 0x0E) >> 1;

    W[21] = (word) (IE[12] & 0x01) << 2;
    W[21] |= (word) (IE[13] & 0xC0) >> 6;

    W[22] = (word) (IE[13] & 0x38) >> 3;

    W[23] = (word) (IE[13] & 0x07);

    W[24] = (word) (IE[14] & 0xE0) >> 5;

    W[25] = (word) (word) (IE[14] & 0x1C) >> 2;

    W[26] = (word) (IE[14] & 0x03) << 1;
    W[26] |= (word) (IE[15] & 0x80) >> 7;

    W[27] = (word) (IE[15] & 0x70) >> 4;

    W[28] = (word) (IE[15] & 0x0E) >> 1;

    index = 1;

    while (index < 29)
    {
      if (W[index] != 0)
      {
        index++;
        size++;
      }
      else
      {
        break;
      }
    }

    if (!decode(W, size, offset, NELEMS(offset), 127))
    {
      *decoding_ok = FALSE;
    }

    cell_allocation->no_of_entries = size + 1;

    cell_allocation->channel_number[0].num = ORIG_ARFCN;
    // WARNING: Need to resolve band information using SI
    cell_allocation->channel_number[0].band = (sys_band_T) rr_get_band(ORIG_ARFCN, gas_id);
    for (index = 1; index < (size + 1); index++)
    {
      cell_allocation->channel_number[index].num =
      (ORIG_ARFCN + offset[index-1]) % 1024;

      // WARNING: Need to resolve band information using SI
      cell_allocation->channel_number[index].band = (sys_band_T) rr_get_band(
        cell_allocation->channel_number[index].num,
        gas_id
      );
    }

    qsort_arfcn(cell_allocation->channel_number,size + 1, gas_id);
  }
  /*lint -save -e774  */
  else if (((IE[0] & 0xC0) == 0x80) && ((IE[0] & 0x0E) == 0x0E) && (support == TRUE))
  /*lint -restore  */
  {

    /***********************************************************/
    /* The cell allocation format is a variable bit map format */
    /***********************************************************/

    ORIG_ARFCN = (word) (IE[0] & 0x01) << 9;
    ORIG_ARFCN |= (word) (IE[1]) << 1;
    ORIG_ARFCN |= (word) (IE[2] & 0x80) >> 7;

    ca_index = 0;
    cell_allocation->no_of_entries = 1;
    cell_allocation->channel_number[ca_index].num = ORIG_ARFCN;
    // WARNING: Need to resolve band information using SI
    cell_allocation->channel_number[ca_index].band = (sys_band_T) rr_get_band(ORIG_ARFCN, gas_id);
    ca_index++;

    RRFCN_mask = 0x40;
    RRFCN = 1;

    for (index = 2; index < 16; index++)
    {
      while (RRFCN_mask != 0 && (ca_index < CA_MAX_LEN))
      {
        if ((IE[index] & RRFCN_mask) != 0)
        {
          cell_allocation->no_of_entries++;
          cell_allocation->channel_number[ca_index].num =
          (RRFCN + ORIG_ARFCN) % 1024;
          cell_allocation->channel_number[ca_index].band = (sys_band_T) rr_get_band(
            cell_allocation->channel_number[ca_index].num,
            gas_id
          );
          ca_index++;
        }

        RRFCN++;
        RRFCN_mask = (byte) (RRFCN_mask >> 1);
      }

      RRFCN_mask = 0x80;
    }

    qsort_arfcn(cell_allocation->channel_number, cell_allocation->no_of_entries, gas_id);
  }
  else
  {
    *decoding_ok = FALSE;

    MSG_GERAN_ERROR_2("Format not supported(%d,%d)",(int) support,(int) IE[0]);

  }
}

/*
 *   Function name:  rr_decode_cell_identity
 *   ---------------------------------------
 *
 *   Description: Decode the Cell Identity IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:     The cell identity
 *   --------
 *
 *
 */

word rr_decode_cell_identity(byte *IE)
{
  word cell_identity;

  cell_identity = ((byte)IE[0] << 8);

  cell_identity |= (word)IE[1];

  return cell_identity;
}

/*
 *   Function name:  rr_decode_cell_options_on_bcch
 *   ----------------------------------------------
 *
 *   Description: Decode the cell options IE from the BCCH
 *   ------------
 *
 *
 *   Parameters: the IE
 *   -----------
 *
 *
 *
 *   Returns:   The decoded IE
 *   --------
 *
 *
 */

void rr_decode_cell_options_on_bcch(byte *IE, cell_options_T *cell_options)
{
  byte RADIO_LINK_TIMEOUT;
  byte PWRC;
  byte DTX;

  RADIO_LINK_TIMEOUT = IE[0] & 0x0F;
  PWRC = ((byte)(IE[0] & 0x40) >> 6);
  DTX = ((byte)(IE[0] & 0x30) >> 4);

  cell_options->PWRC = PWRC;
  cell_options->DTX = DTX;
  cell_options->RADIO_LINK_TIMEOUT = (byte) ((RADIO_LINK_TIMEOUT + 1) * 4);
}

/*
 *   Function name:  rr_decode_cell_options_on_sacch
 *   -----------------------------------------------
 *
 *   Description: Decode the Cell Option IE from the SACCH
 *   ------------
 *
 *
 *   Parameters: The cell option IE
 *   -----------
 *
 *
 *
 *   Returns:  The decoded IE
 *   --------
 *
 *
 */

void rr_decode_cell_options_on_sacch(byte *IE,cell_options_T *cell_options)
{
  byte RADIO_LINK_TIMEOUT;
  byte PWRC;
  byte DTX;
  byte DTX_LOW;
  byte DTX_HIGH;

  RADIO_LINK_TIMEOUT = IE[0] & 0x0F;

  PWRC = ((byte)(IE[0] & 0x40) >> 6);

  DTX_LOW = ((byte)(IE[0] & 0x30) >> 4);
  DTX_HIGH = ((byte)(IE[0] & 0x80) >> 5);

  DTX = DTX_LOW | DTX_HIGH;

  cell_options->PWRC = PWRC;
  cell_options->DTX = DTX;
  cell_options->RADIO_LINK_TIMEOUT = (byte) ((RADIO_LINK_TIMEOUT + 1) * 4);
}

/*
 *   Function name:  rr_decode_cell_selection_parameters
 *   ---------------------------------------------------
 *
 *   Description: Decode the cell selection parameters IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:     The decoded IE
 *   --------
 *
 *
 */

void rr_decode_cell_selection_parameters
(
  byte                        *IE,
  cell_selection_parameters_T *cell_selection_parameters
)
{
  byte CELL_RESELECT_HYSTERESIS;
  byte MS_TXPWR_MAX_CCH;
  byte ACS;
  byte NECI;
  byte RXLEV_ACCESS_MIN;

  CELL_RESELECT_HYSTERESIS = ((byte)(IE[0] & 0xE0) >> 5) * 2;
  MS_TXPWR_MAX_CCH = (IE[0] & 0x1F);

  ACS = (byte)(IE[1] & 0x80) >> 7;

  NECI = (byte)(IE[1] & 0x40) >> 6;

  RXLEV_ACCESS_MIN = IE[1] & 0x3F;

  cell_selection_parameters->CELL_RESELECT_HYSTERESIS = CELL_RESELECT_HYSTERESIS;
  cell_selection_parameters->MS_TXPWR_MAX_CCH = MS_TXPWR_MAX_CCH;
  cell_selection_parameters->ACS = ACS;
  cell_selection_parameters->NECI = NECI;
  cell_selection_parameters->RXLEV_ACCESS_MIN = RXLEV_ACCESS_MIN;
}


/**
 * Decode the Control Channel Description IE (04.18 10.5.2.11)
 *
 * @param IE          byte array containing the encoded IE information
 *
 * @param control_channel_description
 *                    [out] set to the decoded control channel description information
 *
 * @param decoding_ok [out] set to TRUE if the information was decoded, FALSE otherwise
 */
void rr_decode_control_channel_description
(
  byte                          *IE,
  control_channel_description_T *control_channel_description,
  boolean                       *decoding_ok
)
{
  byte ATT                      = 0;
  byte CCCH_CONF                = 0;
  boolean BS_CCCH_SDCCH_COMB    = 0;
  byte BS_AG_BLKS_RES           = 0;
  byte BS_CC_CHANS              = 0;
  byte BS_PA_MFRMS              = 0;
  byte T3212_timeout            = 0;
  byte MSCR                     = 0;

  *decoding_ok = TRUE;

  MSCR = ((byte)(IE[0] & 0x80) >> 7);
  ATT = ((byte)(IE[0] & 0x40) >> 6);
  BS_AG_BLKS_RES = ((byte)(IE[0] & 0x38) >> 3);
  CCCH_CONF = IE[0] & 0x07;

  if (CCCH_CONF == 0)
  {
    BS_CCCH_SDCCH_COMB = FALSE;
    BS_CC_CHANS = 1;
  }
  else if (CCCH_CONF == 1)
  {
    BS_CCCH_SDCCH_COMB = TRUE;
    BS_CC_CHANS = 1;

    if (BS_AG_BLKS_RES > 2)
    {
      MSG_GERAN_ERROR_1("Bad value(%d) for BS_AG_BLKS_RES",(int) BS_AG_BLKS_RES);
      *decoding_ok = FALSE;
    }
  }
  else if (CCCH_CONF == 2)
  {
    BS_CCCH_SDCCH_COMB = FALSE;
    BS_CC_CHANS = 2;
  }
  else if (CCCH_CONF == 4)
  {
    BS_CCCH_SDCCH_COMB = FALSE;
    BS_CC_CHANS = 3;
  }
  else if (CCCH_CONF == 6)
  {
    BS_CCCH_SDCCH_COMB = FALSE;
    BS_CC_CHANS = 4;
  }
  else
  {
    *decoding_ok = FALSE;
    MSG_GERAN_ERROR_1("Bad value(%d) for CCCH_CONF",(int) CCCH_CONF);
  }

  BS_PA_MFRMS = IE[1] & 0x07;
  BS_PA_MFRMS += 2;

  T3212_timeout = IE[2];

  control_channel_description->ATT = ATT;
  control_channel_description->BS_AG_BLKS_RES = BS_AG_BLKS_RES;
  control_channel_description->BS_CC_CHANS = BS_CC_CHANS;
  control_channel_description->BS_CCCH_SDCCH_COMB = BS_CCCH_SDCCH_COMB;
  control_channel_description->BS_PA_MFRMS = BS_PA_MFRMS;
  control_channel_description->T3212_timeout = T3212_timeout;

  control_channel_description->MSCR = MSCR;
} /* rr_decode_control_channel_description */

/*
 *   Function name:  rr_decode_frequency_channel_sequence
 *   ----------------------------------------------------
 *
 *   Description: Decode the Frequency Channel Sequence IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:  The decoded IE
 *   --------
 *
 *
 */

void rr_decode_frequency_channel_sequence
(
  byte              *IE,
  frequency_list_T  *frequency_list,
  boolean           *decoding_ok,
  const gas_id_t     gas_id
)
{
  byte lowest_ARFCN;
  word ARFCN;
  byte freq_index;
  byte index;
  byte increment;

  *decoding_ok = TRUE;
  frequency_list->no_of_items = 0;

  lowest_ARFCN = IE[0] & 0x7F;

  if ((lowest_ARFCN == 0) || (lowest_ARFCN > 124))
  {
    MSG_GERAN_ERROR_1_G("Bad value(%d) for lowest_ARFCN",(int) lowest_ARFCN);
    *decoding_ok = FALSE;
  }
  else
  {
    frequency_list->channel[0].num = (word)lowest_ARFCN;
    frequency_list->no_of_items++;

    ARFCN = lowest_ARFCN;
    freq_index = 1;

    for (index = 1; index < 9; index++)
    {
      increment = (byte) (IE[index] & 0xF0) >> 4;

      if (increment != 0)
      {
        if ((ARFCN + increment) > 124)
        {
          MSG_GERAN_ERROR_3_G(
            "RR Decode freq chan descrip ARFCN out of range, ARFCN:0x%x, index:%d, IE:0x%x",
            ARFCN,
            index,
            IE[index]
          );
          *decoding_ok = FALSE;
          break;
        }

        ARFCN += increment;
        frequency_list->channel[freq_index].num = (word) ARFCN;
        frequency_list->no_of_items++;
        freq_index++;
      }
      else
      {
        ARFCN += 15;
      }

      increment = IE[index] & 0x0F;

      if (increment != 0)
      {
        if ((ARFCN + increment) > 124)
        {
          MSG_GERAN_ERROR_3("RR Decode freq chan descrip ARFCN out of range, ARFCN:0x%x, index:%d, IE:0x%x",
                    ARFCN,
                    index,
                    IE[index]);
          *decoding_ok = FALSE;
          break;
        }

        ARFCN += increment;
        frequency_list->channel[freq_index].num = (word) ARFCN;
        frequency_list->no_of_items++;
        freq_index++;
      }
      else
      {
        ARFCN += 15;
      }

    }
  }

  rr_decode_set_band(frequency_list->channel, frequency_list->no_of_items, gas_id);
}

/*
 *   Function name:  rr_decode_ia_rest_octets
 *   ----------------------------------------
 *
 *   Description: Decode the IA rest octets IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:   The decoded IE
 *   --------
 *
 *
 */

void rr_decode_ia_rest_octets
(
  byte                *IE,
  boolean             *info_valid,
  byte                *MAIO,
  mobile_allocation_T *mobile_allocation,
  boolean              starting_time_valid,
  boolean             *irat_ho_info_compressed,
  uint8               *end_of_msg_ptr,
  boolean             *decode_ok
)
{

  byte P;
  byte length;
  byte new_IE[64];        /* its 64 as length is coded in 6 bits */
  byte index;

  if ((IE == NULL) || (info_valid == NULL) || (MAIO == NULL) ||
      (mobile_allocation == NULL) )
  {
    MSG_GERAN_ERROR_0("NULL pointer parameter");
    return;
  }

  if (irat_ho_info_compressed == NULL)
  {
    MSG_GERAN_ERROR_0("NULL pointer parameter");
    return;
  }

  P = (byte) (IE[0] & 0xC0) >> 6;

  if (P == 0)
  {
    /*************************************************/
    /* The frequency parameters field is not present */
    /*************************************************/
    *info_valid = FALSE;

    /* irat_ho_info_compressed is in the 1st octet, 3rd highest bit */
    *irat_ho_info_compressed = (boolean) (((IE[0] ^ 0x2b) & 0x20) >> 5);
  }
  else if (P == 2)
  {
    length = IE[0] & 0x3F;


    /* Check that the frequency params fit into the remaining space*/
    /* IE[0] : start of reset octets addr +
       length : length of freq params +
       1 : freq param length field */
    if ( (&IE[0] + length + 1) > end_of_msg_ptr )
    {
      *info_valid = FALSE;
      *decode_ok = FALSE; /*invalid msg*/
      MSG_GERAN_ERROR_1("RR Freq param before time too long, len:%d",length);
      return;
    }

    /* 3GPP TS 04.18 Section: 9.1.18.3  If the starting time IE is not present,
     * the mobile station shall consider the frequency parameters, before time
     * construction as unnecessary in the message.                              */
    /* So, check also if starting_time_valid is FALSE                           */

    if ( (length == 0) || (starting_time_valid == FALSE) )
    {
      /*************************************************/
      /* The frequency parameters field is not present */
      /*************************************************/
      *info_valid = FALSE;
    }
    else
    {
      *info_valid = TRUE;
      *MAIO = IE[1] & 0x3F;

      /*
       * Create an IE consisting of the length byte followed by all the
       * MA C bytes from the IA rest octets. length currently includes
       * octet 3 which is not present in the new_ie, so subtract 1
       */

      new_IE[0] = length-1;

      for (index = 0; index < length; index++)
      {
        new_IE[index + 1] = IE[index + 2];
      }

      rr_decode_mobile_allocation(&new_IE[0], mobile_allocation);
    }
    /* irat_ho_info_compressed is in the octet after frequency parameters, before time,
     * 1st highest bit
     */
    if ( &IE[length+1] < end_of_msg_ptr )
    {
      *irat_ho_info_compressed = (boolean) (((IE[length+1] ^ 0x2b) & 0x80) >> 7);
    }
    else
    {
      *irat_ho_info_compressed = FALSE;
    }
  }
}


/**
 * Decode the LAI IE
 *
 * @param IE     Input IE in byte array form
 *
 * @param location_area_identification
 *               [out] Set to the decoded LAI
 */
void rr_decode_location_area_identification
(
  byte  *IE,
  LAI_T *location_area_identification
)
{
  location_area_identification->plmn_id.identity[0] = IE[0];
  location_area_identification->plmn_id.identity[1] = IE[1];
  location_area_identification->plmn_id.identity[2] = IE[2];

  location_area_identification->location_area_code = ((byte)IE[3] << 8);
  location_area_identification->location_area_code |= (word)IE[4];
} /* rr_decode_location_area_identification */

/*
 *   Function name:  rr_decode_mobile_allocation
 *   -------------------------------------------
 *
 *   Description: Decode the Mobile Allocation IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:  The decoded IE
 *   --------
 *
 *
 */

void rr_decode_mobile_allocation(byte *IE, mobile_allocation_T *mobile_allocation)
{
  byte length;
  byte MA_C_mask;
  byte index;
  byte bit_index;
  byte ma_index;
  byte MA_C;

  if (IE == NULL)
  {
    MSG_GERAN_ERROR_0("rr_decode_mobile_allocation trying to decode NULL IE");
    return;
  }

  length = IE[0];

  ma_index = 0;
  MA_C_mask = 0x01;
  MA_C = 0;
  mobile_allocation->no_of_entries = 0;

  index = length + 1;

  if (length != 0)
  {
    do
    {
      index--;

      for (bit_index = 0; bit_index < 8; bit_index++)
      {
        MA_C++;

        if ((IE[index] & MA_C_mask) != 0)
        {
          if (ma_index < CA_MAX_LEN)
          {
            mobile_allocation->MA_C[ma_index] = MA_C;
            mobile_allocation->no_of_entries++;
            ma_index++;
          }
          else
          {
            MSG_GERAN_ERROR_1("GRR ma_index out of range: %d",ma_index);
            return;
          }
        }

        if (MA_C_mask != 0x80)
        {
          MA_C_mask = (byte) (MA_C_mask << 1);
        }

        else
        {
          MA_C_mask = 0x01;
        }
      }
    } while (index != 1);
  }
}

/**
 * Decode the frequency list IE
 *
 * @param IE             Pointer to IE byte array
 *
 * @param channel_number channel number where IE was received
 *
 * @param frequency_list [out] set to the decoded IE value
 *
 * @param decoding_ok    [out] set to TRUE if decoding successful, FALSE otherwise
 */
void rr_decode_frequency_list(
  byte              *IE,
  ARFCN_T           channel_number,
  frequency_list_T  *frequency_list,
  boolean           *decoding_ok,
  const gas_id_t     gas_id
)
{
  byte index;
  cell_channel_description_T cell_channel_description;
  memset((void *) &cell_channel_description,0,sizeof(cell_channel_description));

  rr_decode_frequency_list_ie(
    IE,
    channel_number,
    &cell_channel_description,
    decoding_ok,
    gas_id
  );

  if (*decoding_ok)
  {
    frequency_list->no_of_items = cell_channel_description.no_of_entries;

    for (index = 0; index < cell_channel_description.no_of_entries; index++)
    {
      frequency_list->channel[index] = cell_channel_description.channel_number[index];
    }
  }
  else /* decoding error */
  {
    frequency_list->no_of_items = 0;
  }
}

/*
 *   Function name:  rr_decode_RACH_control_parameters
 *   --------------------------------------------------
 *
 *   Description: Decode the RACH comtrol parameters IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:  The decoded IE
 *   --------
 *
 *
 */

void rr_decode_RACH_control_parameters(byte *IE,RACH_control_parameters_T *RACH_control_parameters)
{
  static const byte tx_integer_table[16] =
  { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16 ,20, 25, 32, 50};

  byte temp;
  byte max_retrans = 0;
  byte tx_integer;
  byte CELL_BAR_ACCESS;
  byte REESTABLISHMENT_ALLOWED;
  byte access_control[2];

  /*****************************************************/
  /* Decode the maximum number of retransmission field */
  /*****************************************************/

  temp = (byte)(IE[0] & 0xC0) >> 6;

  if (temp == 0)
  {
    max_retrans = 1;
  }

  else if (temp == 1)
  {
    max_retrans = 2;
  }

  else if (temp == 2)
  {
    max_retrans = 4;
  }

  else if (temp == 3)
  {
    max_retrans = 7;
  }

  /*****************************************************/
  /* Decode the number of slots to spread transmission */
  /*****************************************************/

  temp = (byte)(IE[0] & 0x3C) >> 2;

  tx_integer = tx_integer_table[temp];

  /*******************************/
  /* Decode the CELL BAR ACCESS  */
  /*******************************/

  CELL_BAR_ACCESS = (byte)(IE[0] & 0x02) >> 1;

  /*******************************************/
  /* Decode the REESTABLISHMENT information  */
  /*******************************************/

  REESTABLISHMENT_ALLOWED = (IE[0] & 0x01);

  if (REESTABLISHMENT_ALLOWED == 0)
  {
    REESTABLISHMENT_ALLOWED = 1;
  }
  else
  {
    REESTABLISHMENT_ALLOWED = 0;
  }

  access_control[0] = IE[1];
  access_control[1] = IE[2];

  RACH_control_parameters->CELL_BAR_ACCESS = CELL_BAR_ACCESS;
  RACH_control_parameters->REESTABLISHMENT_ALLOWED = REESTABLISHMENT_ALLOWED;
  RACH_control_parameters->tx_integer = tx_integer;
  RACH_control_parameters->max_retrans = max_retrans;
  RACH_control_parameters->access_control[0] = access_control[0];
  RACH_control_parameters->access_control[1] = access_control[1];
}

/*===========================================================================

FUNCTION  rr_decode_request_reference

DESCRIPTION

Decodes the request reference IE

===========================================================================*/

void rr_decode_request_reference(
  byte *IE_ptr,
  byte *access_reference_ptr,
  word *frame_number_ptr
)
{
  int16 T1_prime;
  int16 T2;
  int16 T3;
  int32 FN;

  RR_NULL_CHECK_FATAL(IE_ptr);
  RR_NULL_CHECK_FATAL(access_reference_ptr);
  RR_NULL_CHECK_FATAL(frame_number_ptr);

  /**
   * From 3GPP 44.018 10.5.2.30 Request Reference
   *  
   * The purpose of the Request Reference information element is to provide the random access information used 
   * in the channel request and the frame number, FN modulo 42432 in which the channel request was received 
   *  
   *            Bit    7 6 5 4 3 2 1 0
   *  
   * Reference  IE[0]  7 6 5 4 3 2 1 0   => 8 bits (range 0-255)
   * T1_prime   IE[1]  4 3 2 1 0 - - -   => 5 bits (range 0-31)
   *  
   * T3         IE[1]  - - - - - 5 4 3 
   *            IE[2]  2 1 0 - - - - -   => 6 bits (range 0-63)
   *  
   * T2         IE[2]  - - - 4 3 2 1 0   => 5 bits (range 0-31) 
   *  
   * The T1' field is coded as the binary representation of (FN div 1326) mod 32. 
   * The T3 field is coded as the binary representation of FN mod 51. 
   * The T2 field is coded as the binary representation of FN mod 26. 
   *  
   * The frame number, FN modulo 42432 can be calculated as (51 x ((T3-T2) mod 26)) + T3 + (51 x 26 x T1')
   */

  *access_reference_ptr = IE_ptr[0];

  T1_prime = (IE_ptr[1] & 0xF8) >> 3;
  T3 = ((IE_ptr[1] & 0x07) << 3) | ((IE_ptr[2] & 0xE0) >> 5);
  T2 = IE_ptr[2] & 0x1F;

  FN = (51 * (((T3 - T2) + 26) % 26)) + T3 + (51 * 26 * T1_prime);

  // Result should be FN mod 42432 (0..42431)
  if ((FN >=0) && (FN <= 42431))
  {
    *frame_number_ptr = (word)FN;
  }
  else
  {
    MSG_GERAN_ERROR_3("Error decoding FN T1_prime:%d, T2:%d, T3:%d",T1_prime,T2,T3);

    // Error in the calculation, so set to an out of range value to avoid matching
    *frame_number_ptr = 0xffff;
  }

  return;

} /* rr_decode_request_reference */

/*
 *   Function name:  rr_decode_short_freq_list
 *   -----------------------------------------
 *
 *   Description: Decode the short frequency list IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:  The decoded IE
 *   --------
 *
 *
 */

void rr_decode_short_freq_list(
  byte              *IE,
  frequency_list_T  *frequency_list,
  boolean           *decoding_ok,
  const gas_id_t     gas_id
)
{
  byte index;
  byte new_IE[16];
  cell_channel_description_T cell_channel_description;


  for (index = 0; index < 16; index++)
  {
    new_IE[index] = 0;
  }

  for (index = 0; index < 9; index++)
  {
    new_IE[index] = IE[index];
  }

  rr_decode_cell_channel_description(
    new_IE,
    &cell_channel_description,
    decoding_ok,
    gas_id
  );

  frequency_list->no_of_items = cell_channel_description.no_of_entries;

  if ( (cell_channel_description.no_of_entries > NELEMS(frequency_list->channel))  ||
       (cell_channel_description.no_of_entries > NELEMS(cell_channel_description.channel_number)) )
  {
    *decoding_ok = FALSE;
    MSG_GERAN_ERROR_1_G("RR cell_channel_description.no_of_entries: 0x%x, overruns array",
              cell_channel_description.no_of_entries);
    return;
  }

  for (index = 0; index < cell_channel_description.no_of_entries; index++)
  {
    frequency_list->channel[index] = cell_channel_description.channel_number[index];
  }
}

/*
 *   Function name:  rr_decode_starting_time
 *   ---------------------------------------
 *
 *   Description: Decode the starting time IE
 *   ------------
 *
 *
 *   Parameters: The IE
 *   -----------
 *
 *
 *
 *   Returns:  The decoded IE
 *   --------
 *
 *
 */

void rr_decode_starting_time(byte *IE_ptr, word *starting_time_ptr, boolean *decode_ok_ptr)
{
  int16 T1_prime;
  int16 T2;
  int16 T3;
  int32 FN;

  RR_NULL_CHECK_FATAL(IE_ptr);
  RR_NULL_CHECK_FATAL(starting_time_ptr);
  RR_NULL_CHECK_FATAL(decode_ok_ptr);

  /** 
   * From 3GPP 44.018 10.5.2.38	Starting Time 
   * 
   *            Bit    7 6 5 4 3 2 1 0
   *  
   * T1_prime   IE[0]  4 3 2 1 0 - - -   => 5 bits (range 0-31)
   *  
   * T3         IE[0]  - - - - - 5 4 3 
   *            IE[1]  2 1 0 - - - - -   => 6 bits (range 0-63)
   *  
   * T2         IE[1]  - - - 4 3 2 1 0   => 5 bits (range 0-31) 
   *  
   * The T1' field is coded as the binary representation of (FN div 1326) mod 32. 
   * The T3 field is coded as the binary representation of FN mod 51. 
   * The T2 field is coded as the binary representation of FN mod 26.
   *  
   * The frame number, FN modulo 42432 can be calculated as 
   * (51 x ((T3 - T2) mod 26)) + T3 + (51 x 26 x T1'). 
   *  
   * The Starting Time IE can encode only an interval of time of 42 432 frames.
   */

  T1_prime = (IE_ptr[0] & 0xF8) >> 3;
  T3 = ((IE_ptr[0] & 0x07) << 3) | ((IE_ptr[1] & 0xE0) >> 5);
  T2 = IE_ptr[1] & 0x1F;

  FN = (51 * (((T3 - T2) + 26) % 26)) + T3 + (51 * 26 * T1_prime);

  // Result should be FN mod 42432 (0..42431)
  if ((FN >=0) && (FN <= 42431))
  {
    *starting_time_ptr = (word)FN;
    *decode_ok_ptr = TRUE;
  }
  else
  {
    MSG_GERAN_ERROR_3("Error decoding FN T1_prime:%d, T2:%d, T3:%d",T1_prime,T2,T3);
    *decode_ok_ptr = FALSE;
  }

  return;
}


/*
 *   Function name:  rr_decode_set_band()
 *   --------------------------------------------------
 *
 *   Description: Ensure the band field in an array of ARFCN_T structures is set
 *   ------------
 *
 *
 *   Parameters: Array of ARFCN_Ts, number of entries (0..255)
 *   -----------
 *
 *
 *
 *   Returns:     Void
 *   --------
 *
 *
 */
static void rr_decode_set_band(ARFCN_T *arfcn_list_ptr, byte num_entries, const gas_id_t gas_id)
{
  ARFCN_T *loop_ptr = arfcn_list_ptr;
  byte     index;

  /* Restore correct band information */
  for (index = 0; index < num_entries; index++, loop_ptr++)
  {
    loop_ptr->band = (sys_band_T) rr_get_band(loop_ptr->num, gas_id);
  }
}

/*
 *   Function name:  rr_decode_frequency_list_ie()
 *   --------------------------------------------------
 *
 *   Description: Decode the frequency list (4.08 10.5.2.13)
 *   ------------
 *
 *
 *   Parameters: The IE, pointing to the length part
 *   -----------
 *
 *
 *
 *   Returns:     The decoded IE
 *   --------
 *
 *
 */

#define  MAX_FREQ_LIST_SIZE  132

static void rr_decode_frequency_list_ie(
  byte                        *IE,
  ARFCN_T                     channel_number,
  cell_channel_description_T  *frequency_list,
  boolean                     *decoding_ok,
  const gas_id_t               gas_id
)
{
  word  W_value = 0;
  int   bit_number;
  byte  bit_field_size;
  word  mask;

  byte  W_count;
  byte  ARFCN_mask;
  byte  RRFCN_mask;
  word  RRFCN;
  word  CA_ARFCN;
  byte  index;
  byte  bit_index;
  byte  ca_index = 0;
  word  ORIG_ARFCN;
  byte  size = 0;
  word  W[RR_FREQUENCY_LIST_W_ARRAY_LENGTH];
  word  offset[CA_MAX_LEN];
  byte  F0;
  byte  length;
  boolean support = TRUE;
  byte IE_scr[MAX_FREQ_LIST_SIZE] ;

  /* Assume decode will fail to ensure early returns indicate an error */
  *decoding_ok = FALSE;

  /* initialize the frequency list to 0 entries in case it isn't set */
  frequency_list->no_of_entries = 0;

  if (rr_gapi_is_supported_band(SYS_BAND_PGSM_900, gas_id) && (rr_gapi_get_supported_band_count(gas_id) == 1))
  {
    /************************************************************/
    /* The mobile does not support anything but bitmap 0 format */
    /************************************************************/
    support = FALSE;
  }

  length = IE[0];

  /* Octet 0 is the 'Length of Value Part' octet - to copy the
  complete encoded IE we need to copy both the length octet and
  the value part octets - IE[0] + 1, after checking for boundary
  conditions.  Check for length > MAX_FREQ_LIST_SIZE - 1 to avoid
  integer overflow. */

  if (length > (MAX_FREQ_LIST_SIZE - 1))
  {
    MSG_GERAN_ERROR_1_G("Received invalid length %d for Frequency List IE, decode failed", length);
    return;
  }

  /*
   * copy the message into a fixed buffer area terminated with a couple
   * of nulls
   */

  // Replace memcpy with memscpy
  memscpy(&(IE_scr[0]),sizeof(IE_scr), IE, (length + 1));
  

  /* initialize everything from length+1 all the way to the end */
  for (index = length + 1; index < MAX_FREQ_LIST_SIZE; ++index)
  {
    IE_scr[index] = 0;
  }

  if ((byte)(IE_scr[1] & 0xC0) == 0)
  {
    /*****************************************************/
    /* The cell allocation format is a bit map format    */
    /*****************************************************/

    ARFCN_mask = 0x01;
    CA_ARFCN = 0;
    frequency_list->no_of_entries = 0;

    /* bitmap 0 format length is fixed at 16 bytes */
    if (length != 16)
    {
      MSG_GERAN_ERROR_1_G("Invalid length=%d in freqListIE",(int) length);
      return;
    }

    /*
     * point at the last byte in the IE
     */

    index = 16;

    do
    {

      for (bit_index = 0; bit_index < 8; bit_index++)
      {
        CA_ARFCN++;

        if ((IE_scr[index] & ARFCN_mask) == ARFCN_mask)
        {
          if (ca_index < CA_MAX_LEN)
          {
            frequency_list->channel_number[ca_index].num = CA_ARFCN;
            frequency_list->no_of_entries++;
            ca_index++;
          }
          else
          {
            MSG_GERAN_ERROR_0_G("Overflow while decoding frequency list");
            return;
          }
        }

        if ((bit_index == 3) && (index == 1))
        {

          /*
           * last ARFCN in octet 3 has been processed
           */

          break;
        }

        else if (ARFCN_mask != 0x80)
        {
          ARFCN_mask = (byte) (ARFCN_mask << 1);
        }

        else
        {
          ARFCN_mask = 0x01;
        }
      }

      index--;

    } while (index != 0);

    rr_decode_set_band(frequency_list->channel_number, frequency_list->no_of_entries, gas_id);
  }
  else if (((IE_scr[1] & 0xC0) == 0x80) && ((IE_scr[1] & 0x08) == 0x00) && (support == TRUE))
  {
    /***************************************************/
    /* The cell allocation format is 1024 range format */
    /***************************************************/
    /* range1024 format requires a minimum of 2 bytes to encode */
    if (length < 2)
    {
      MSG_GERAN_ERROR_1_G("Invalid length=%d in freqListIE",(int) length);
      return;
    }

    F0 = (word) (IE_scr[1] & 0x04) >> 2;

    if (F0 == 1)
    {
      /**************************************/
      /* The ARFCN 0 is a member of the set */
      /**************************************/

      frequency_list->channel_number[0].num = 0;
      frequency_list->no_of_entries = 1;
    }
    else
    {
      frequency_list->no_of_entries = 0;
    }

    /**********************/
    /* Decode the weights */
    /**********************/


    /*
     * first W value (W1) is non standard
     */

    W_value = (word) ((IE_scr[1] & 0x03) << 8) + IE_scr[2];
    W_count = 1;
    bit_number = 0;

    while ( W_value != 0)
    {
      if (W_count >= NELEMS(W))
      {
        MSG_GERAN_ERROR_1_G("RR array overrun W_count:0x%x",W_count);
        return;
      }

      W[W_count] = W_value;

      W_count++;

      /*
       * work out the size of the W field
       * Start as 9 bits, then get log (base 2) of W_count -
       * for exponents greater than or equal to 2, subtract (exponent - 2)
       * from the field size, as per 04.08 10.5.2.13.3
       */

      bit_field_size = 9;
      mask = 0xfffc;

      while (( W_count & mask ) != 0)
      {
        mask = (word) (mask << 1);
        bit_field_size -= 1;
      }

      /*
       * get next W value
       */

      W_value = rr_get_bit_field(&IE_scr[3],bit_number,bit_field_size,(MAX_FREQ_LIST_SIZE-3) );
      bit_number += bit_field_size;
    }

    if (W_count >= NELEMS(W))
    {
      MSG_GERAN_ERROR_1_G("RR array overrun W_count:0x%x",W_count);
      return;
    }

    W[W_count] = 0;
    size = W_count-1;

    if (!decode(W,size,offset, NELEMS(offset),1023))
    {
      MSG_GERAN_ERROR_1_G("decode failed size:%d",size);
      return;
    }

    if ( (size > NELEMS(frequency_list->channel_number)) ||
         (size > NELEMS(offset)) )
    {
      MSG_GERAN_ERROR_1_G("RR size:%d overrun array",size);
      return;
    }

    if (frequency_list->no_of_entries == 1)
    {
      /* already stored a single channel number => ARFCN 0 */
      for (index = 1; index < (size + 1); index++)
      {
        frequency_list->channel_number[index].num = offset[index-1];
      }
    }
    else
    {
      /* no entries in frequency list yet */
      for (index = 0; index < size; index++)
      {
        frequency_list->channel_number[index].num = offset[index];
      }
    }

    qsort_arfcn(frequency_list->channel_number,size + frequency_list->no_of_entries, gas_id);
    frequency_list->no_of_entries += size;
  }
  else if (((IE_scr[1] & 0xC0) == 0x80) && ((IE_scr[1] & 0x0E) == 0x08) && (support == TRUE))
  {
    /**************************************************/
    /* The cell allocation format is 512 range format */
    /**************************************************/
    /* range512 format requires a minimum of 4 bytes to encode */
    if (length < 4)
    {
      MSG_GERAN_ERROR_1_G("Invalid length=%d in freqListIE",(int) length);
      return;
    }

    ORIG_ARFCN = (word) (IE_scr[1] & 0x01) << 9;
    ORIG_ARFCN |= (word) (IE_scr[2]) << 1;
    ORIG_ARFCN |= (word) (IE_scr[3] & 0x80) >> 7;

    W_count = 1;

    /*
     * first bit of W1 is bit 7 of 4th byte of IE
     */

    bit_number = 1;

    do
    {
      /*
       * work out the size of the W field
       * Start as 9 bits, then get log (base 2) of W_count -
       * for exponents greater than or equal to 1, subtract (exponent - 1)
       * from the field size, as per 04.08 10.5.2.13.4
       */

      bit_field_size = 9;
      mask = 0xfffe;

      while (( W_count & mask ) != 0)
      {
        mask = (word) (mask << 1);
        bit_field_size -= 1;
      }

      /*
       * get next W value
       */

      W_value = rr_get_bit_field( &IE_scr[3],bit_number,bit_field_size,(MAX_FREQ_LIST_SIZE-3) );
      bit_number += bit_field_size;

      /* Only write in to the W array if we are within it's bounds */
      if (W_count < RR_FREQUENCY_LIST_W_ARRAY_LENGTH)
      {
        W[W_count] = W_value;
      }
      else
      {
        break;
      }

      W_count++;
    } while (W_value != 0);

    size = W_count-2;

    if (!decode(W, size, offset, NELEMS(offset), 511))
    {
      MSG_GERAN_ERROR_1_G("decode failed size:%d",size);
      return;
    }

    frequency_list->no_of_entries = size + 1;
    frequency_list->channel_number[0].num = ORIG_ARFCN;

    for (index = 1; index < (size + 1); index++)
    {
      frequency_list->channel_number[index].num =
        (ORIG_ARFCN + offset[index-1]) % 1024;
    }
    qsort_arfcn(frequency_list->channel_number, size + 1, gas_id);
  }
  else if (((IE_scr[1] & 0xC0) == 0x80) && ((IE_scr[1] & 0x0E) == 0x0A) && (support == TRUE))
  {
    /****************************************************/
    /* The cell allocation format is a 256 range format */
    /****************************************************/
    /* range256 format requires a minimum of 4 bytes to encode */
    if (length < 4)
    {
      MSG_GERAN_ERROR_1_G("Invalid length=%d in freqListIE",(int) length);
      return;
    }

    ORIG_ARFCN = (word) (IE_scr[1] & 0x01) << 9;
    ORIG_ARFCN |= (word) (IE_scr[2]) << 1;
    ORIG_ARFCN |= (word) (IE_scr[3] & 0x80) >> 7;

    W_count = 1;

    /*
     * first bit of W1 is bit 7 of 4th byte of IE
     */

    bit_number = 1;

    do
    {

      /*
       * work out the size of the W field
       * Start as 8 bits, then get log (base 2) of W_count -
       * for exponents greater than or equal to 1, subtract (exponent - 1)
       * from the field size, as per 04.08 10.5.2.13.4
       */

      bit_field_size = 8;
      mask = 0xfffe;

      while (( W_count & mask ) != 0)
      {
        mask = (word) (mask << 1);
        bit_field_size -= 1;
      }

      /*
       * get next W value
       */

      W_value = rr_get_bit_field( &IE_scr[3],bit_number,bit_field_size,(MAX_FREQ_LIST_SIZE-3) );
      bit_number += bit_field_size;

      /* Only write in to the W array if we are within it's bounds */
      if (W_count < RR_FREQUENCY_LIST_W_ARRAY_LENGTH)
      {
        W[W_count] = W_value;
      }
      else
      {
        break;
      }

      W_count++;
    } while (W_value != 0);

    size = W_count-2;

    if (!decode(W, size, offset, NELEMS(offset), 255))
    {
      *decoding_ok = FALSE;
      return;
    }

    frequency_list->no_of_entries = size + 1;
    frequency_list->channel_number[0].num = ORIG_ARFCN;

    for (index = 1; index < (size + 1); index++)
    {
      frequency_list->channel_number[index].num =
        (ORIG_ARFCN + offset[index-1]) % 1024;
    }

    qsort_arfcn(frequency_list->channel_number, size + 1, gas_id);
  }

  else if (((IE_scr[1] & 0xC0) == 0x80) && ((IE_scr[1] & 0x0E) == 0x0C) && (support == TRUE))
  {
    /****************************************************/
    /* The cell allocation format is a 128 range format */
    /****************************************************/

    /* range128 format requires a minimum of 3 bytes to encode */
    if (length < 3)
    {
      MSG_GERAN_ERROR_1_G("Invalid length=%d in freqListIE",(int) length);
      return;
    }

    ORIG_ARFCN = (word) (IE_scr[1] & 0x01) << 9;
    ORIG_ARFCN |= (word) (IE_scr[2]) << 1;
    ORIG_ARFCN |= (word) (IE_scr[3] & 0x80) >> 7;

    W_count = 1;

    /*
     * first bit of W1 is bit 7 of 4th byte of IE
     */

    bit_number = 1;

    do
    {

      /*
       * work out the size of the W field
       * Start as 9 bits, then get log (base 2) of W_count -
       * for exponents greater than or equal to 1, subtract (exponent - 1)
       * from the field size, as per 04.08 10.5.2.13.6
       */

      bit_field_size = 7;
      mask = 0xfffe;

      while (( W_count & mask ) != 0)
      {
        mask = (word) ( mask << 1 );
        bit_field_size -= 1;
      }

      /*
       * get next W value
       */

      W_value = rr_get_bit_field(&IE_scr[3],bit_number,bit_field_size,(MAX_FREQ_LIST_SIZE-3) );
      bit_number += bit_field_size;

      /* Only write in to the W array if we are within it's bounds */
      if (W_count < RR_FREQUENCY_LIST_W_ARRAY_LENGTH)
      {
        W[W_count] = W_value;
      }
      else
      {
        break;
      }

      W_count++;
    } while (W_value != 0);

    size = W_count-2;

    if (!decode(W, size, offset, NELEMS(offset), 127))
    {
      MSG_GERAN_ERROR_1_G("decode failed size:%d",size);
      return;
    }

    frequency_list->no_of_entries = size + 1;
    frequency_list->channel_number[0].num = ORIG_ARFCN;

    for (index = 1; index < (size + 1); index++)
    {
      frequency_list->channel_number[index].num =
        (ORIG_ARFCN + offset[index-1]) % 1024;
    }

    qsort_arfcn(frequency_list->channel_number, size + 1, gas_id);

  }

  else if (((IE_scr[1] & 0xC0) == 0x80) && ((IE_scr[1] & 0x0E) == 0x0E) && (support == TRUE))
  {
    /***********************************************************/
    /* The cell allocation format is a variable bit map format */
    /***********************************************************/
    /* variable bitmap format requires a minimum of 3 bytes to encode */
    if (length < 3)
    {
      MSG_GERAN_ERROR_1_G("Invalid length=%d in freqListIE",(int) length);
      return;
    }

    ORIG_ARFCN = (word) (IE_scr[1] & 0x01) << 9;
    ORIG_ARFCN |= (word) (IE_scr[2]) << 1;
    ORIG_ARFCN |= (word) (IE_scr[3] & 0x80) >> 7;

    ca_index = 0;
    frequency_list->no_of_entries = 1;
    frequency_list->channel_number[ca_index].num = ORIG_ARFCN;
    ca_index++;

    RRFCN_mask = 0x40;
    RRFCN = 1;

    for (index = 3; index < IE_scr[0]+1; index++)
    {
      while (RRFCN_mask != 0)
      {
        if (index < MAX_FREQ_LIST_SIZE)
        {
          if ((IE_scr[index] & RRFCN_mask) != 0)
          {
            if (ca_index < CA_MAX_LEN)
            {
              frequency_list->no_of_entries++;
              frequency_list->channel_number[ca_index].num = (RRFCN + ORIG_ARFCN) % 1024;
              ca_index++;
            }
            else
            {
              MSG_GERAN_ERROR_0_G("ca_index %d out of range");
              return;
            }
          }
        }
        else
        {
          MSG_GERAN_ERROR_0_G("index %d out of range");
          return;
        }

        RRFCN++;
        RRFCN_mask = (byte) (RRFCN_mask >> 1);
      }

      RRFCN_mask = 0x80;
    }

    qsort_arfcn(frequency_list->channel_number, frequency_list->no_of_entries, gas_id);

    rr_decode_set_band(frequency_list->channel_number, frequency_list->no_of_entries, gas_id);
  }
  else
  {
    MSG_GERAN_ERROR_2_G("Format (%d,%d) not supported",(int) support,(int) IE_scr[1]);
    return;
  }

  *decoding_ok = TRUE;

} /* rr_decode_frequency_list_ie */

/*===========================================================================

FUNCTION  rr_decode_multirate_config_ie

DESCRIPTION

Decodes the multirate configuration IE

===========================================================================*/

void rr_decode_multirate_config_ie
(
  byte                *IE_ptr,
  multirate_config_T  *pConfig,
  boolean             *decoding_ok,
  channel_mode_T       channel_mode_in_ass,
  const gas_id_t       gas_id
)
{
  byte length;
  byte MR_version;
  byte num_modes;
  gs_bit_stream_T bitstream;
  gs_bit_stream_T *bit_stream_ptr;
  RR_NULL_CHECK_FATAL(IE_ptr);
  RR_NULL_CHECK_FATAL(pConfig);
  RR_NULL_CHECK_FATAL(decoding_ok);

  (void) channel_mode_in_ass;

  pConfig->valid = FALSE;
  num_modes = 0;
  /* Initialize bit stream; Add 1 for the length octet itself since the IE starts from the
     length octet */
  gs_bit_stream_initialize(&bitstream,IE_ptr,IE_ptr[0]+1);
  bit_stream_ptr = &bitstream;


  /*Get length of IE*/
  length = (byte) gs_bit_stream_extract_field(bit_stream_ptr,8);

  /*Check length*/
  if (length < 2)
  {
    MSG_GERAN_ERROR_1_G("Multirate len%d err",length);
    *decoding_ok = FALSE;
    return;
  }

  /*Get MR version must be 001/010*/
  MR_version = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
#ifdef FEATURE_GSM_AMR_WB
/*MR version 001 and 010 are supported*/
  if((MR_version != 1)&&(MR_version != 2))
  {
    /*Multirate speech version not supported*/
    MSG_GERAN_ERROR_1_G("MR ver%d not supported",MR_version);
    *decoding_ok = FALSE;
    return;
  }
#else
  if (MR_version != 1)
  {
    /*Multirate speech version not supported*/
    MSG_GERAN_ERROR_1_G("MR ver%d not supported",MR_version);
    *decoding_ok = FALSE;
    return;
  }
#endif /*FEATURE_GSM_AMR_WB*/
  pConfig->noise_suppression_off= (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

  pConfig->start_mode_valid = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

  /* spare bit */
  (void) gs_bit_stream_extract_field(bit_stream_ptr,1);

  pConfig->start_mode = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);

  pConfig->codec_modes = (byte) gs_bit_stream_extract_field(bit_stream_ptr,8);

#ifdef FEATURE_GSM_AMR_WB
 
#ifdef FEATURE_VAMOS_II
  if(channel_mode_in_ass == SPEECH_V3 || channel_mode_in_ass == SPEECH_V3_VAMOS2)
#else
  if(channel_mode_in_ass == SPEECH_V3)
#endif /*FEATURE_VAMOS_II*/
  {
    if(MR_version == 2)
    {
      /*Wrong MR version provided in assignment*/
      MSG_GERAN_ERROR_1_G("Wrong MR version provided in assignment %d ",MR_version);
      *decoding_ok = FALSE;
      return;
    }
  }
#ifdef FEATURE_VAMOS_II
  else if(channel_mode_in_ass == SPEECH_V5 ||channel_mode_in_ass == SPEECH_V5_VAMOS2)
#else
  else if(channel_mode_in_ass == SPEECH_V5)
#endif /*FEATURE_VAMOS_II*/
  {
    if(MR_version == 1)
    {
      /*Wrong MR version provided in assignment*/
      MSG_GERAN_ERROR_1_G("Wrong MR version provided in assignment %d ",MR_version);
      *decoding_ok = FALSE;
      return;
    }
  }

  if(MR_version == 2)
  {
    if(pConfig->codec_modes & 0x10)
    {
      /*AMR_WB codec rate 23.85 is not supported */
      MSG_GERAN_ERROR_0_G("AMR_WB codec rate 23.85 is not supported ");
      *decoding_ok = FALSE;
      return;

    }

    if(pConfig->codec_modes & 0x08)
    {
      /*AMR_WB codec rate 23.85 is not supported */
      MSG_GERAN_ERROR_0_G("AMR_WB codec rate 15.85 is not supported ");
      *decoding_ok = FALSE;
      return;
    }

    num_modes = ((pConfig->codec_modes & 0x04)>>2)+((pConfig->codec_modes & 0x02)>>1)+
                 (pConfig->codec_modes & 0x01);

    if (num_modes==0)
    {
      /***********************************************************/
      /* For FR AMR WB, there should be atleast one codec modes given in the */
      /* Assignment                                              */
      /***********************************************************/
      MSG_GERAN_ERROR_1_G("Codec num mode err%d",num_modes);
      *decoding_ok = FALSE;
      return;
    }
  }
  else
#endif /* FEATURE_GSM_AMR_WB*/
  if(MR_version == 1)
  {
    /* Count number of codec modes */
    num_modes = ((IE_ptr[2] & 0x80)>>7) + ((IE_ptr[2] & 0x40)>>6) + ((IE_ptr[2] & 0x20)>>5) + ((IE_ptr[2] & 0x10)>>4) +
      ((IE_ptr[2] & 0x08)>>3) + ((IE_ptr[2] & 0x04)>>2) + ((IE_ptr[2] & 0x02)>>1) + (IE_ptr[2] & 0x01);


    /* The following consistency checks have to be done
    1)  the active set does not include any codec mode or the active set includes more than four codec modes; or
    2)  one or more codec modes of the active codec set are not supported by the assigned channel; or
    3)  the threshold and hysteresis values are not set according to requirements given in 3GPP TS 05.09.
    They are (05.09):
    3a) THR_1 <= THR_2 <= THR_3, and
    3b) THR_1 + HYST_1 <= THR_2 + HYST_2  <= THR_3 + HYST_3.
    */

    /* check 1 */
    if (num_modes>4 || num_modes==0)
    {
      /********************************************************/
      /* 4.18 section 3.4.3.3 specifies that there must be at */
      /* least 1 codec mode and no more than 4                */
      /********************************************************/
      MSG_GERAN_ERROR_1_G("Codec num mode err%d",num_modes);
      *decoding_ok = FALSE;
      return;
    }
  }

  /*Get Thresholds and hystersis*/
  if (num_modes>1)
  {

    int index;

    /*****************************************************************************/
    /* Check length to see if it is correct, the number of thresholds/hysteresis */
    /* is equal to num_modes-1. So if there was 2 modes there would be 1         */
    /* threshold/hysteresis. The combined length of the threshold/hysteresis is  */
    /* 10 bits, so some spare bits are needed. It works out that the length is   */
    /* 1 bytes for the initial header + 1 byte*(num_modes-1) + 1 byte for spare  */
    /* bits. See 4.18 10.5.2.21aa                                                */
    /*****************************************************************************/
    if (length<(1+num_modes))
    {
      MSG_GERAN_ERROR_1_G("Multirate len%d err",length);
      *decoding_ok = FALSE;
      return;
    }

    /* spare bits */
    (void) gs_bit_stream_extract_field(bit_stream_ptr,2);

    for (index=0; index<(num_modes-1); index++)
    {
      pConfig->thresholds[index] = (byte) gs_bit_stream_extract_field(bit_stream_ptr,6);
      pConfig->hysteresis[index] = (byte) gs_bit_stream_extract_field(bit_stream_ptr,4);
    }
  }

  /* check 3a */
  if (num_modes == 3)
  {
    if (
        (pConfig->thresholds[0] > pConfig->thresholds[1]) ||
        (pConfig->thresholds[0] + pConfig->hysteresis[0] > pConfig->thresholds[1] + pConfig->hysteresis[1])
       )
    {
      MSG_GERAN_ERROR_0_G("Inconsistent threshold/hysteresis");
      *decoding_ok = FALSE;
      return;
    }
  }
  else if (num_modes == 4)  /* check 3b */
  {
    if (
        (pConfig->thresholds[0] > pConfig->thresholds[1]) ||
        (pConfig->thresholds[0] + pConfig->hysteresis[0] > pConfig->thresholds[1] + pConfig->hysteresis[1]) ||
        (pConfig->thresholds[1] > pConfig->thresholds[2]) ||
        (pConfig->thresholds[1] + pConfig->hysteresis[1] > pConfig->thresholds[2] + pConfig->hysteresis[2])
       )
    {
      MSG_GERAN_ERROR_0_G("Inconsistent threshold/hysteresis");
      *decoding_ok = FALSE;
      return;
    }
  }

  rr_log_multirate_parameters(pConfig, num_modes, gas_id);

  pConfig->valid = TRUE;
  *decoding_ok = TRUE;

  return;
} /* rr_decode_multirate_config_ie */


/*
 *   Function name:  rr_get_bit_field()
 *   --------------------------------------------------
 *
 *   Description: Extract the value in a bit field, given the
 *   ------------ offset into the bit field, and the size of the
 *                item to extract.
 *
 *
 *   Parameters: The start address of the bit field
 *   ----------- The offset into the bit field to the MSB
 *               The length of the item in the bit field to extract
 *
 *
 *
 *   Returns:     The decoded value
 *   --------
 *
 *
 */

static word rr_get_bit_field( byte   field[],int   bit_number,int   bit_field_size ,int array_field_size )
{
  int         byte_offset;
  int         first_no_bits;
  int         last_no_bits;
  word        ret_val;


  byte_offset = bit_number / 8;
  first_no_bits = 8 - bit_number % 8 ;

  if ( first_no_bits == 0)
  {
    first_no_bits = 8;
  }

  last_no_bits = bit_field_size - first_no_bits;


  /*
   * get useful bits from first byte, left aligned at present, then shift up
   * by the number of bits in the second part
   */
  if (byte_offset < array_field_size)
  {
   if (last_no_bits > 0)
   {
     ret_val = (word) (((byte)((field[byte_offset] << (8-first_no_bits)) & 0xff )
                >> (8-first_no_bits)) << last_no_bits);

     ret_val |= (field[byte_offset+1]>>(8-last_no_bits));
   }
   else
   {
     last_no_bits = -last_no_bits;

     ret_val = ((byte)((byte)((field[byte_offset] <<
                               (8-first_no_bits)) & 0xff )
                       >> (8-first_no_bits))) >> (last_no_bits);
   }
  }
  else
  {
     ret_val=0;
  }
  return ret_val;
}

/**************************************************************************************
Function
rr_SI2q_MI_instance_already_received

Description
Initialize the SI2q_MI_instances array to their default values.

Parameters
[IN]
instances_T           *SI2quater:
Pointer to global storage for the instances.

byte SI2quater_INDEX:
index to the current instance

Returns
TRUE if the instance has been received already; FALSE otherwise

**********************************************************************************************/
boolean rr_SI2q_MI_instance_already_received(instances_T *SI2quater, byte SI2quater_INDEX)
{
  RR_NULL_CHECK_FATAL (SI2quater);
  /* ASSERT (SI2quater_INDEX < MAX_INSTANCES); */

  if (SI2quater_INDEX < MAX_INSTANCES)
  {
    if (SI2quater->instance[SI2quater_INDEX].valid_data == TRUE)
    {
      return TRUE;
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("SI2quater_INDEX >= MAX_INSTANCES!!");
  }
  return FALSE;
}

/**************************************************************************

Function:
rr_update_status_SI2q_MI_instance

Description:
Set the received and changed flags for the instance to the specified values

Parameters:
[IN/OUT]
instances_T *instances
Pointer to the SI2quater of MI instances
[IN]
byte index
index to the instance whose values should be set
boolean received
Value to set to
boolean changed
Value to set to

Returns:
None

***************************************************************************/
static void rr_update_status_SI2q_MI_instance (
  instances_T *instances,
  byte index,
  boolean received,
  boolean changed
)
{
  RR_NULL_CHECK_FATAL (instances);
  /* ASSERT (index < MAX_INSTANCES); */
  if (index < MAX_INSTANCES)
  {
    instances->instance[index].valid_data = received;
    instances->instance[index].changed = changed;
  }
  else
  {
    MSG_GERAN_ERROR_0("In func rr_update_status_SI2q_MI_instance, index >= MAX_INSTANCES!!");
  }
}

/**************************************************************************

Function:
rr_received_all_instances

Description:
Reset the 'changed' flag in the instances 0 till the specified array_length.

Parameters:
[IN]
instances_T *instances
Pointer to the SI2quater of MI instances
byte array_length
Number of instances received
byte COUNT
Maximum number of instances of MI or SI2quater COUNT specified in the instances.

Returns:
TRUE if all instances have been received; FALSE otherwise

***************************************************************************/
static boolean rr_received_all_instances(instances_T *instances, byte array_length, byte COUNT)
{
  // Just check if the array_length is same as SI2quater_COUNT, for now.
  // May need to check if the index_array has all the indices from 1 till SI2quater_COUNT.

  int i;
  boolean received;

  if (instances == NULL)
  {
    MSG_GERAN_ERROR_0("NULL ptr passed to rr_received_all_instances!!");
    return FALSE;
  }

  if (array_length > MAX_INSTANCES)
  {
    MSG_GERAN_ERROR_1("array_length > MAX_INSTANCES, truncating to %d", MAX_INSTANCES);
    array_length = MAX_INSTANCES;
  }

  /* ASSERT (COUNT < MAX_INSTANCES); */
  if (COUNT >= MAX_INSTANCES)
  {
    MSG_GERAN_ERROR_1("COUNT >= MAX_INSTANCES, truncating to %d", MAX_INSTANCES - 1);
    COUNT = MAX_INSTANCES - 1;
  }

  if (array_length == COUNT+1) /* For one instance SI2q/MI count = 0, hence the +1 */
  {
    received = TRUE;

    for (i = 0; i < array_length; ++i)
    {
      /* bitwise AND of the received bit of all instances */
      received &= instances->instance[i].valid_data;

      /* When received becomes FALSE return received immediately since bitwise AND will eventually return FALSE */
      if (received == FALSE)
      {
        return received;
      }
    }

    return received;
  }

  return FALSE;
}

/**************************************************************************

Function:
rr_reset_SI2q_MI_changed

Description:
Reset the 'changed' flag in the instances 0 till the specified array_length.

Parameters:
[IN/OUT]
instances_T *instances
Pointer to the SI2quater of MI instances.
[IN]
byte array_length
Number of instances

Returns:
None

***************************************************************************/

static void rr_reset_SI2q_MI_changed(instances_T *instances, byte array_length)
{
  int i;

  if (instances == NULL)
  {
    MSG_GERAN_ERROR_0("NULL ptr passed to rr_reset_SI2q_MI_changed!!");
    return;
  }

  if (array_length > MAX_INSTANCES)
  {
    MSG_GERAN_ERROR_0("array_length > MAX_INSTANCES!!");
    array_length = MAX_INSTANCES;
  }

  for (i = 0; i < array_length; ++i)
  {
    instances->instance[i].changed = FALSE;
  }
}

/**************************************************************************

Function:
rr_MI_changed_3g_wait_instances

Description:
Checks if there is a change in any of the 3G_WAIT instances that contain
the 3G Neighbor Cell Description.

Parameters:
[IN]
instances_T *MeasInfo
Pointer to the measurement information instances.
byte array_length
Number of instances

Returns:
TRUE if change in any of the 3G_WAIT instances that contain the 3G Neighbor Cell
Description; FALSE otherwise.

***************************************************************************/
static boolean rr_MI_changed_3g_wait_instances (instances_T *MeasInfo, byte array_length)
{
  int i;
  boolean changed = FALSE;

  if (MeasInfo == NULL)
  {
    MSG_GERAN_ERROR_0("NULL ptr passed to rr_MI_changed_3g_wait_instances!!");
    return FALSE;
  }

  if (array_length > MAX_INSTANCES)
  {
    MSG_GERAN_ERROR_0("array_length > MAX_INSTANCES!!");
    array_length = MAX_INSTANCES;
  }

  for (i = 0; i < array_length; ++i)
  {
    if (MeasInfo->instance[i].common_params.WCDMA_3G_NC_Desc_PI)
    {
      /* bitwise OR of the received bit of all instances */
      changed |= MeasInfo->instance[i].changed;

      /* When changed becomes TRUE return changed immediately since bitwise OR will eventually return TRUE */
      if (changed == TRUE)
      {
        return changed;
      }
    }
  }

  return changed;
}

/**************************************************************************

Function:
rr_SI2q_MI_changed

Description:
Checks if there is a change in any of the instances received (from 0 till
array_length)

Parameters:
[IN]
instances_T *instances
Pointer to the SI2quater or MI instances array

byte array_length
length of the instance array

byte COUNT
Maximum number of SI2quater or MI that is specified in the instance.

Returns
TRUE if atleast one of the instances has changed; FALSE otherwise

***************************************************************************/

boolean rr_SI2q_MI_changed (instances_T *instances)
{
  int i;
  boolean changed = FALSE;

  if (instances == NULL)
  {
    MSG_GERAN_ERROR_0("NULL ptr passed to rr_SI2q_MI_changed!!");
    return FALSE;
  }

  if (instances->num_instances > MAX_INSTANCES)
  {
    MSG_GERAN_ERROR_0("array_length > MAX_INSTANCES!!");
    instances->num_instances = MAX_INSTANCES;
  }

  for (i = 0; ( (i < MAX_INSTANCES) && (!changed) ) ; ++i)
  {
    if (instances->instance[i].valid_data)
    {
      /* bitwise OR of the received bit of all instances */
      changed |= instances->instance[i].changed;
    }
  }

  return changed;
}

#if  defined(FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE)
static boolean rr_decode_fdd_cell_info_field(byte NR_OF_FDD_CELLS, gs_bit_stream_T *bit_stream_ptr,
                                             encoded_FDD_info_T *encoded_info_ptr)
{
  if (encoded_info_ptr == NULL)
  {
    MSG_GERAN_ERROR_0("NULL pointer");
    return FALSE;
  }

  if (NR_OF_FDD_CELLS > 31)
  {
    MSG_GERAN_ERROR_1("NR_OF_FDD_CELLS(%d) > 31",(int) NR_OF_FDD_CELLS);
    return FALSE;
  }

  /* Set the num_cells in the temp storage to 0 when the NR_OF_FDD_CELLS is a
     value between 17 to 31 both inclusive */
  else if (NR_OF_FDD_CELLS >= 17)
  {
    encoded_info_ptr->num_cells = 0;
  }
  else
  {
    byte  max_cells, num_bits, num_bits_index, num_FDD_CELLS;

    /* Set the num_cells in the temp storage to NR_OF_FDD_CELLS */
    encoded_info_ptr->num_cells = NR_OF_FDD_CELLS;

    /*
      The number of bits for the encoded cells (W values) are in the following pattern:
      W(1): 10bits
      W(2) and W(3) : 9 bits
      W(4), W(5), W(6), and W(7) : 8 bits
      and so on.

      We need two loops:
      (1) from 1 to the total number of cells specified in NR_OF_FDD_CELLS
         --> max_cells and num_bits indicate the maximum number of cells of num_bits bits.
         --> max_cells will be 1, 2, 4, 8,...; multiples of 2
         --> num_bits will start from 10(Range1024 = 2power10) since
             W(1) is of 10 bits and will be decremented
             by 1 after reading max_cells with num_bits bits.
      (2) for each cell in (1) loop to extract "max_cells" cells each of "num_bits" bits
     */
    for ( num_FDD_CELLS = 1, max_cells = 1, num_bits = 10;
          num_FDD_CELLS <= NR_OF_FDD_CELLS;
          max_cells *= 2, num_bits-- )
    {
      for ( num_bits_index = 0;
            (num_bits_index < max_cells) && (num_FDD_CELLS <= NR_OF_FDD_CELLS);
            ++num_bits_index )
      {
        encoded_info_ptr->encoded_cells[num_FDD_CELLS++] =
            (word) gs_bit_stream_extract_field(bit_stream_ptr,num_bits);
      } /* loop to get the W values */
    } /* loop for NR_OF_FDD_CELLS */
  }

  if (bit_stream_ptr->overflow)
  {
    MSG_GERAN_ERROR_0("decoding fdd cells overflow");
    return FALSE;
  }

  return TRUE;

}
#endif /* FEATURE_CELL_SELECTION_AFTER_CONN_RELEASE && FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
static boolean rr_decode_tdd_cell_info_field(byte NR_OF_TDD_CELLS, gs_bit_stream_T *bit_stream_ptr,
                                             encoded_TDD_info_T *encoded_info_ptr)
{
  if (encoded_info_ptr == NULL)
  {
    MSG_GERAN_ERROR_0("NULL pointer");
    return FALSE;
  }

  if (NR_OF_TDD_CELLS > 31)
  {
    MSG_GERAN_ERROR_1("NR_OF_TDD_CELLS(%d) > 31",(int) NR_OF_TDD_CELLS);
    return FALSE;
  }

  /* Set the num_cells in the temp storage to 0 when the NR_OF_TDD_CELLS is a
     value between 21 to 31 both inclusive */
  else if (NR_OF_TDD_CELLS >= 21)
  {
    encoded_info_ptr->num_cells = 0;
  }
  else
  {
    byte  max_cells, num_bits, num_bits_index, num_TDD_CELLS;

    /* Set the num_cells in the temp storage to NR_OF_TDD_CELLS */
    encoded_info_ptr->num_cells = NR_OF_TDD_CELLS;

    /*
      The number of bits for the encoded cells (W values) are in the following pattern:
      W(1): 9bits
      W(2) and W(3) : 8 bits
      W(4), W(5), W(6), and W(7) : 7 bits
      and so on.

      We need two loops:
      (1) from 1 to the total number of cells specified in NR_OF_TDD_CELLS
         --> max_cells and num_bits indicate the maximum number of cells of num_bits bits.
         --> max_cells will be 1, 2, 4, 8,...; multiples of 2
         --> num_bits will start from 10(Range512 = 2power9) since
             W(1) is of 9 bits and will be decremented
             by 1 after reading max_cells with num_bits bits.
      (2) for each cell in (1) loop to extract "max_cells" cells each of "num_bits" bits
     */
    for ( num_TDD_CELLS = 1, max_cells = 1, num_bits = 9;
          num_TDD_CELLS <= NR_OF_TDD_CELLS;
          max_cells *= 2, num_bits-- )
    {
      for ( num_bits_index = 0;
            (num_bits_index < max_cells) && (num_TDD_CELLS <= NR_OF_TDD_CELLS);
            ++num_bits_index )
      {
        encoded_info_ptr->encoded_cells[num_TDD_CELLS++] =
            (word) gs_bit_stream_extract_field(bit_stream_ptr,num_bits);
      } /* loop to get the W values */
    } /* loop for NR_OF_TDD_CELLS */
  }

  if (bit_stream_ptr->overflow)
  {
    MSG_GERAN_ERROR_0("decoding tdd cells overflow");
    return FALSE;
  }

  return TRUE;

}
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

/**************************************************************************************
Function
rr_extract_SI2q_MI_3G_NC_Desc

Description
This is a common routine used to extract information received in SI2quater
or in Measurement Information

Parameters
[IN]
gs_bit_stream_T *bit_stream_ptr:
Pointer to the bit stream containing the SI2quater or the Measurement Information
received from the network. It is set to the position from where the
"3G Neighbor Cell Description" can be extracted.

[OUT]
SI2q_MI_params_T *common_params:
Pointer to the temporary storage into which the parameters common to both SI2quater
and Measurement Information are extracted into.

SI2q_params_T *si2q_params:
Pointer to the temporary storage into which parameters specifiec to SI2quater
are extracted into.

MI_params_T *mi_params:
Pointer to the temporary storage into which the parameters specifiec to Measurement Information
are extracted into.

encoded_FDD_info_T *encoded_info:
Pointer to temporary storage that stores the extracted encoded neighbor cells.

boolean *decoding_ok
return value to indicate whether the decoding was successfull.

boolean *overflow
[IN|OUT]
Used to indicate if the end of the bitstream has already been reach [IN] or to indicate to
calling function that the end of the bitstream was reached during this function call [OUT].

Returns
None

**********************************************************************************************/

static void rr_extract_SI2q_MI_3G_NC_Desc(
  gs_bit_stream_T *bit_stream_ptr,
  SI2q_MI_params_T *common_params,
  SI2q_params_T *si2q_params,
  MI_params_T *mi_params,
  encoded_FDD_info_T *encoded_fdd_info,
  boolean *decoding_ok,
  encoded_TDD_info_T *encoded_tdd_info
)
{
  encoded_FDD_info_T *encoded_fdd_info_ptr;
  encoded_TDD_info_T *encoded_tdd_info_ptr;
  byte  Repeated_FDD_Start,
        NR_OF_FDD_CELLS,
        max_cells,
        num_bits,
        num_bits_index,
        num_FDD_CELLS = 0,
        NR_OF_TDD_CELLS,
        num_TDD_CELLS;

  byte source = SOURCE_UNKNOWN;

  RR_NULL_CHECK_FATAL (bit_stream_ptr);
  RR_NULL_CHECK_FATAL (common_params);
  RR_NULL_CHECK_FATAL (encoded_fdd_info);
  RR_NULL_CHECK_FATAL (decoding_ok);
  ASSERT ((si2q_params != NULL) || (mi_params != NULL));
  RR_NULL_CHECK_FATAL (encoded_tdd_info);

  /* find the source from which information need to be extracted */
  if ((mi_params != NULL) && (si2q_params == NULL))
  {
    source = SOURCE_MEAS_INFO;
  }
  else if ((si2q_params != NULL) && (mi_params == NULL))
  {
    source = SOURCE_SI2_QUATER;
  }

  common_params->WCDMA_3G_NC_Desc_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

  if (common_params->WCDMA_3G_NC_Desc_PI)
  {
    /* if MI extract the 3g wait parameter also */
    if (
        (source == SOURCE_MEAS_INFO) &&
        (mi_params != NULL) /* this repeat check is to satisfy lint */
       )
    {
      mi_params->MI_3G_WAIT_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

      if (mi_params->MI_3G_WAIT_PI)
      {
        mi_params->MI_3G_WAIT = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      }
    }

    common_params->Index_Start_3G_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (common_params->Index_Start_3G_PI == 1)
    {
      common_params->Index_Start_3G = (byte) gs_bit_stream_extract_field(bit_stream_ptr,7);
    }

    common_params->Absolute_Index_Start_EMR_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (common_params->Absolute_Index_Start_EMR_PI == 1)
    {
      common_params->Absolute_Index_Start_EMR = (byte) gs_bit_stream_extract_field(bit_stream_ptr,7);
    }


    common_params->UTRAN_FDD_Desc_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

    if (common_params->UTRAN_FDD_Desc_PI)
    {
      byte BW_FDD_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
      if (BW_FDD_PI == 1)
      {
        /* Bandwidth_FDD = */ (void) gs_bit_stream_extract_field(bit_stream_ptr,3);
      }

      /* Collect the repeated UTRAN FDD Neighbor Cell Information */
      for (
            common_params->num_repeated_FDD_info = 0;
            common_params->num_repeated_FDD_info < MAX_RR_MPH_FREQ_UPDATE_LIST;
            common_params->num_repeated_FDD_info++
          )
      {

        Repeated_FDD_Start = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

        if (Repeated_FDD_Start == 0)
        {
          break;
        }

        encoded_fdd_info_ptr = &encoded_fdd_info[common_params->num_repeated_FDD_info];

        /* read bit with value 0 */
        (void) gs_bit_stream_extract_field(bit_stream_ptr,1);

        encoded_fdd_info_ptr->uarfcn
              = (word) gs_bit_stream_extract_field(bit_stream_ptr,14);

        encoded_fdd_info_ptr->FDD_Indic0 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

        encoded_fdd_info_ptr->NR_OF_FDD_CELLS = (byte) gs_bit_stream_extract_field(bit_stream_ptr,5);

        NR_OF_FDD_CELLS = encoded_fdd_info_ptr->NR_OF_FDD_CELLS;

        if (NR_OF_FDD_CELLS > 31)
        {
          MSG_GERAN_ERROR_1("NR_OF_FDD_CELLS(%d) > 31",(int) NR_OF_FDD_CELLS);
          *decoding_ok = FALSE;
          /* stop decoding further and return; If there was an existing 3g neighbor list
          it is left untouched */
          return;
        }

        /* Set the num_cells in the temp storage to 0 when the NR_OF_FDD_CELLS is a
           value between 17 to 31 both inclusive */
        if ((NR_OF_FDD_CELLS >= 17) && (NR_OF_FDD_CELLS <= 31))
        {
          encoded_fdd_info_ptr->num_cells = 0;
        }
        else
        {
          /* Set the num_cells in the temp storage to NR_OF_FDD_CELLS */
          encoded_fdd_info_ptr->num_cells = NR_OF_FDD_CELLS;
        }

        if ((NR_OF_FDD_CELLS > 0) && (NR_OF_FDD_CELLS < 17))
        {
          /*
            The number of bits for the encoded cells (W values) are in the following pattern:
            W(1): 10bits
            W(2) and W(3) : 9 bits
            W(4), W(5), W(6), and W(7) : 8 bits
            and so on.

            We need two loops:
            (1) from 1 to the total number of cells specified in NR_OF_FDD_CELLS
               --> max_cells and num_bits indicate the maximum number of cells of num_bits bits.
               --> max_cells will be 1, 2, 4, 8,...; multiples of 2
               --> num_bits will start from 10(Range1024 = 2power10) since
                   W(1) is of 10 bits and will be decremented
                   by 1 after reading max_cells with num_bits bits.
            (2) for each cell in (1) loop to extract "max_cells" cells each of "num_bits" bits
           */
          for (
                num_FDD_CELLS = 1, max_cells = 1, num_bits = 10;
                num_FDD_CELLS <= NR_OF_FDD_CELLS;
                max_cells *= 2, num_bits--
              )
          {
            for (
                  num_bits_index = 0;
                  (num_bits_index < max_cells) && (num_FDD_CELLS <= NR_OF_FDD_CELLS);
                  ++num_bits_index
                )
            {
              encoded_fdd_info_ptr->encoded_cells[num_FDD_CELLS++] =
                  (word) gs_bit_stream_extract_field(bit_stream_ptr,num_bits);
            } /* loop to get the W values */
          } /* loop for NR_OF_FDD_CELLS */
        } /* NR_OF_FDD_CELLS < 17 */
        else /* NR_OF_FDD_CELLS >= 17 */
        {
          /* the number of bits is 0; So the next bit will be the end bit with value 0 */
        }
      }/* loop for the repeated FDD information */
    } /* end if (UTRAN_FDD_Desc_PI) */


    /* TDD CELLS */

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
      {
        common_params->utran_tdd_bandwidth = (uint8)gs_bit_stream_extract_field(bit_stream_ptr,3);
      }
      else
      {
        common_params->utran_tdd_bandwidth = 0; /*Default to 3.84Mcps when missing*/
      }

      /* Collect the repeated UTRAN TDD Neighbor Cell Information */
      for (
            common_params->num_repeated_TDD_info = 0;
            common_params->num_repeated_TDD_info < MAX_UTRAN_TDD_MPH_FREQ_UPDATE_LIST;
            common_params->num_repeated_TDD_info++
          )
      {
        if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1) == 0)
        {
          break;
        }

        encoded_tdd_info_ptr = &encoded_tdd_info[common_params->num_repeated_TDD_info];

        /* read bit with value 0 */
        (void) gs_bit_stream_extract_field(bit_stream_ptr,1);

        encoded_tdd_info_ptr->uarfcn = (word)gs_bit_stream_extract_field(bit_stream_ptr,14);

        encoded_tdd_info_ptr->TDD_Indic0 = (boolean)gs_bit_stream_extract_field(bit_stream_ptr,1);

        encoded_tdd_info_ptr->NR_OF_TDD_CELLS = (byte) gs_bit_stream_extract_field(bit_stream_ptr,5);

        NR_OF_TDD_CELLS = encoded_tdd_info_ptr->NR_OF_TDD_CELLS;

        if (NR_OF_TDD_CELLS > 31)
        {
          MSG_GERAN_ERROR_1("NR_OF_TDD_CELLS(%d) > 31",(int) NR_OF_TDD_CELLS);
          *decoding_ok = FALSE;
          /* stop decoding further and return; If there was an existing 3g neighbor list
          it is left untouched */
          return;
        }

        /* Set the num_cells in the temp storage to 0 when the NR_OF_TDD_CELLS is a
           value between 21 to 31 both inclusive */
        if ((NR_OF_TDD_CELLS >= 21) && (NR_OF_TDD_CELLS <= 31))
        {
          encoded_tdd_info_ptr->num_cells = 0;
        }
        else
        {
          /* Set the num_cells in the temp storage to NR_OF_TDD_CELLS */
          encoded_tdd_info_ptr->num_cells = NR_OF_TDD_CELLS;
        }

        if (NR_OF_TDD_CELLS < 21)
        {
          /*
            The number of bits for the encoded cells (W values) are in the following pattern:
            W(1): 9bits
            W(2) and W(3) : 8 bits
            W(4), W(5), W(6), and W(7) : 7 bits
            and so on.

            We need two loops:
            (1) from 1 to the total number of cells specified in NR_OF_TDD_CELLS
               --> max_cells and num_bits indicate the maximum number of cells of num_bits bits.
               --> max_cells will be 1, 2, 4, 8,...; multiples of 2
               --> num_bits will start from 10(Range512 = 2power9) since
                   W(1) is of 9 bits and will be decremented
                   by 1 after reading max_cells with num_bits bits.
            (2) for each cell in (1) loop to extract "max_cells" cells each of "num_bits" bits
           */
          for (
                num_TDD_CELLS = 1, max_cells = 1, num_bits = 9;
                num_TDD_CELLS <= NR_OF_TDD_CELLS;
                max_cells *= 2, num_bits--
              )
          {
            for (
                  num_bits_index = 0;
                  (num_bits_index < max_cells) && (num_TDD_CELLS <= NR_OF_TDD_CELLS);
                  ++num_bits_index
                )
            {
              encoded_tdd_info_ptr->encoded_cells[num_TDD_CELLS++] =
              (word)gs_bit_stream_extract_field(bit_stream_ptr,num_bits);
            } /* loop to get the W values */
          } /* loop for NR_OF_TDD_CELLS */
        } /* NR_OF_TDD_CELLS < 21 */
        else /* NR_OF_TDD_CELLS >= 21 */
        {
          /* the number of bits is 0; So the next bit will be the end bit with value 0 */
        }
      }/* loop for the repeated TDD information */
    } /* end UTRAN_TDD_Desc_PI */

    /* Decode CDMA_2000_Desc if source is MI */
    if (source == SOURCE_MEAS_INFO)
    {
      if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1)) /* CDMA2000 Desc PI */
      {
        byte i, number_cdma2000_cells;

        (void) gs_bit_stream_extract_field(bit_stream_ptr,5); /* frequency band */

        (void) gs_bit_stream_extract_field(bit_stream_ptr,11); /* frequency */

        number_cdma2000_cells = (byte) gs_bit_stream_extract_field(bit_stream_ptr,5); /* number cdma2000 cells */

        for (i = 0; i < number_cdma2000_cells; ++i)
        {
          (void) gs_bit_stream_extract_field(bit_stream_ptr,9); /* pilot PN offset */
          if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1)) /* cdma info */
          {
            byte type_of_info_flag;

            type_of_info_flag = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);

            switch(type_of_info_flag)
            {
              case 0:
                /* TD_MODE */ (void) gs_bit_stream_extract_field(bit_stream_ptr,2);
                /* TD_POWER_LEVEL */ (void) gs_bit_stream_extract_field(bit_stream_ptr,3);
                break;
              case 1:
                {
                  byte WALSH_LEN_A;
                  /* QOF */ (void) gs_bit_stream_extract_field(bit_stream_ptr,2);
                  WALSH_LEN_A = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
                  /* AUX_PILOT_WALSH */ (void) gs_bit_stream_extract_field(bit_stream_ptr,WALSH_LEN_A+6);
                  break;
                }
              case 2:
                {
                  byte WALSH_LEN_B;
                  /* QOF */ (void) gs_bit_stream_extract_field(bit_stream_ptr,2);
                  WALSH_LEN_B = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
                  /* AUX_TD_WALSH */ (void) gs_bit_stream_extract_field(bit_stream_ptr,WALSH_LEN_B+6);
                  /* AUX_TD_POWER_LEVEL */ (void) gs_bit_stream_extract_field(bit_stream_ptr,2);
                  /* TD_MODE */ (void) gs_bit_stream_extract_field(bit_stream_ptr,2);
                  break;
                }
              case 3:
                {
                  /* SR3_PRIM_PILOT */ (void) gs_bit_stream_extract_field(bit_stream_ptr,2);
                  /* SR3_PILOT_POWER1 */ (void) gs_bit_stream_extract_field(bit_stream_ptr,3);
                  /* SR3_PILOT_POWER2 */ (void) gs_bit_stream_extract_field(bit_stream_ptr,3);
                  break;
                }
              case 6:
               {
                 byte WALSH_LEN_C;
                 /* SR3_PRIM_PILOT */ (void) gs_bit_stream_extract_field(bit_stream_ptr,2);
                 /* SR3_PILOT_POWER1 */ (void) gs_bit_stream_extract_field(bit_stream_ptr,3);
                 /* SR3_PILOT_POWER2 */ (void) gs_bit_stream_extract_field(bit_stream_ptr,3);
                 /* QOF */ (void) gs_bit_stream_extract_field(bit_stream_ptr,2);
                 WALSH_LEN_C = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
                 /* AUX_WALSH_LEN */ (void) gs_bit_stream_extract_field(bit_stream_ptr,WALSH_LEN_C+6);
                 if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1)) /* WALSH_LENGTH1 */
                 {
                   byte WALSH_LENGTH1;
                   /* QOF1 */ (void) gs_bit_stream_extract_field(bit_stream_ptr,2);
                   WALSH_LENGTH1 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
                   /* AUX_PILOT_WALSH1 */ (void) gs_bit_stream_extract_field(bit_stream_ptr,WALSH_LENGTH1+6);
                 }
                 if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1)) /* WALSH_LENGTH2 */
                 {
                   byte WALSH_LENGTH2;
                   /* QOF2 */ (void) gs_bit_stream_extract_field(bit_stream_ptr,2);
                   WALSH_LENGTH2 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
                   /* AUX_PILOT_WALSH2 */ (void) gs_bit_stream_extract_field(bit_stream_ptr,WALSH_LENGTH2+6);
                 }
                 break;
               }
             default:
               break;
            } /* end switch(type_of_info_flag) */
          }  /* if  cdma info */
        }  /* end  for (i = 0; i < number_cdma2000_cells; ++i) */
      } /* if CDMA 2000 Description present */
    } /* if source is MI */
  } /* Done reading 3G neighbor cell description struct */
}

/**************************************************************************************
Function
rr_extract_SI2quater_3G_MP_Desc

Description
Extract 3G Measurement Parameters received in SI2quater

Parameters
[IN]
gs_bit_stream_T *bit_stream_ptr:
Pointer to the bit stream containing the SI2quater
received from the network. It is set to the position from where the
"3G Measurement Parameters Description" can be extracted.

[OUT]
SI2q_params_T *si2q_params:
Pointer to the temporary storage into which the measurement parameters
are extracted into.

Returns
None

**********************************************************************************************/

static void rr_extract_SI2quater_3G_MP_Desc(
  gs_bit_stream_T *bit_stream_ptr,
  SI2q_params_T *si2q_params
)
{
  RR_NULL_CHECK_FATAL (bit_stream_ptr);
  RR_NULL_CHECK_FATAL (si2q_params);

  si2q_params->WCDMA_3G_MP_Desc_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
  if (si2q_params->WCDMA_3G_MP_Desc_PI)
  {
    si2q_params->QSearch_I = (byte) gs_bit_stream_extract_field(bit_stream_ptr,4);

    si2q_params->QSearch_C_Initial = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    si2q_params->FDD_Info_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (si2q_params->FDD_Info_PI)
    {
      si2q_params->FDD_Qoffset = (byte) gs_bit_stream_extract_field(bit_stream_ptr,4);
      si2q_params->FDD_REP_QUANT = (fdd_rep_quant_T) gs_bit_stream_extract_field(bit_stream_ptr,1);
      si2q_params->FDD_MULTIRAT_REPORTING = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);
      si2q_params->FDD_Qmin = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    si2q_params->TDD_Info_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (si2q_params->TDD_Info_PI)
    {

      si2q_params->TDD_Qoffset = (byte)gs_bit_stream_extract_field(bit_stream_ptr,4);
      si2q_params->TDD_MULTIRAT_REPORTING = (byte)gs_bit_stream_extract_field(bit_stream_ptr,2);
    }
  } /* end WCDMA_3G_MP_Desc_PI */
}

/**************************************************************************************
Function
rr_extract_SI2quater_GPRS_3G_MEASUREMENT_PARAMETERS

Description
Extract remaining parameters after the "3G Measurement Parameter Description"
 received in SI2quater

Parameters
[IN]
gs_bit_stream_T *bit_stream_ptr:
Pointer to the bit stream containing the SI2quater
received from the network. It is set to the position from where the
"GPRS 3G Measurement Parameters Description" can be extracted.

[OUT]
GPRS 3G Measurement Parameters go in si2q_params.

Returns
None

**********************************************************************************************/
static void rr_extract_SI2quater_GPRS_3G_MEASUREMENT_PARAMETERS(
  gs_bit_stream_T *bit_stream_ptr,
  SI2q_params_T *si2q_params
)
{
  gprs_3g_meas_params_mods_t *gprs_3g_meas_params = &si2q_params->gprs_3g_meas_params;
  RR_NULL_CHECK_FATAL (bit_stream_ptr);

  if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
  {
    gprs_3g_meas_params->valid = TRUE;

    gprs_3g_meas_params->Qsearch_P_valid = TRUE;
    gprs_3g_meas_params->Qsearch_P = (byte) gs_bit_stream_extract_field(bit_stream_ptr,4);

    gprs_3g_meas_params->_3G_SEARCH_PRIO_valid = TRUE;
    gprs_3g_meas_params->_3G_SEARCH_PRIO = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_3g_meas_params->FDD_reporting_valid = TRUE;

      gprs_3g_meas_params->FDD_REP_QUANT = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
      gprs_3g_meas_params->FDD_MULTIRAT_REPORTING = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_3g_meas_params->FDD_off_thresh_valid = TRUE;

      gprs_3g_meas_params->FDD_REPORTING_OFFSET = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      gprs_3g_meas_params->FDD_REPORTING_THRESHOLD = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_3g_meas_params->TDD_reporting_valid = TRUE;

      gprs_3g_meas_params->TDD_MULTIRAT_REPORTING = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_3g_meas_params->TDD_off_thresh_valid = TRUE;

      gprs_3g_meas_params->TDD_REPORTING_OFFSET = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      gprs_3g_meas_params->TDD_REPORTING_THRESHOLD = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }
  } /* end GPRS_3G_MPP_Desc_PI */
}

/**************************************************************************************
Function
rr_extract_SI2quater_fields1

Description
Extract the normal Measurement Parameters Description into the temporary storage. All
GPRS related parameters, NC Measurement Parameters and the extension length are
extracted but not stored, since they are not used. They have to be extracted in order
to set the bit_stream_ptr to start extracting the 3G neighbor cell description.
**************************************************************************************/
static void rr_extract_SI2quater_GPRS_RTD_Description(
  gs_bit_stream_T *bit_stream_ptr,
  SI2q_params_T *si2q_params
)
{
  RR_NULL_CHECK_FATAL (bit_stream_ptr);
  RR_NULL_CHECK_FATAL (si2q_params);

  /* This is the GPRS RTD struct, unused */
  if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
  {
    /* Start reading RTD 6 */
    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
      {
        /* discard the extracted value */
        (void) gs_bit_stream_extract_field(bit_stream_ptr,5);
      }

      while(!bit_stream_ptr->overflow)
      {
        if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
        {
          break;
        }
        /* discard the extracted value */
        (void) gs_bit_stream_extract_field(bit_stream_ptr,6);
      } /* end while */

      /* RTD6Struct could be present again */
      while(!bit_stream_ptr->overflow)
      {
        if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
        {
          break;
        }
        while(!bit_stream_ptr->overflow)
        {
          if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
          {
            break;
          }

          /* discard the extracted value */
          (void) gs_bit_stream_extract_field(bit_stream_ptr,6);
        } /* end while */
      } /* end while */
    } /* end GPRS_RTD6_PI */

    /* Start reading RTD 12 */

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      if ((byte)gs_bit_stream_extract_field(bit_stream_ptr,1))
      {
        /* discard the extracted value */
        (void) gs_bit_stream_extract_field(bit_stream_ptr,5);
      }

      while(!bit_stream_ptr->overflow)
      {
        if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
        {
          break;
        }

        /* discard the extracted value */
        (void) gs_bit_stream_extract_field(bit_stream_ptr,12);
      } /* end while */

      /* RTD6Struct could be present again */
      while(!bit_stream_ptr->overflow)
      {
        if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
        {
          break;
        }
        while(!bit_stream_ptr->overflow)
        {
          if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
          {
            break;
          }

          /* discard the extracted value */
          (void) gs_bit_stream_extract_field(bit_stream_ptr,12);
        } /* end while */
      } /* end while */
    } /* end GPRS_RTD6_PI */
  } /* GPRS_RTD_Desc_PI */
}

/**
  @brief Decodes GPRS BSIC Description structure from an instance of SI2quater.
*/
static void rr_extract_SI2quater_GPRS_BSIC_Description(
  gs_bit_stream_T *bit_stream_ptr,
  SI2q_params_T *si2q_params
)
{
  RR_NULL_CHECK_FATAL(bit_stream_ptr);
  RR_NULL_CHECK_FATAL(si2q_params);

  /* GPRS BSIC Description */
  if (gs_bit_stream_extract_field(bit_stream_ptr,1))
  {
    gprs_bsic_description_instance_t *gprs_bsic_description = NULL;
    gprs_bsic_description_instance_entry_t *current_entry;
    uint32 num_remaining_bsic;

#ifdef FEATURE_LTE
    gprs_bsic_description = GPRS_MEM_MALLOC(sizeof(gprs_bsic_description_instance_t));
#else
    gprs_bsic_description = &si2q_params->gprs_bsic_description;
#endif /* FEATURE_LTE */

    RR_NULL_CHECK_FATAL(gprs_bsic_description);
    memset(gprs_bsic_description, 0, sizeof(gprs_bsic_description_instance_t));

    MSG_GERAN_HIGH_0("Decoding GPRS BSIC Description");

    if (gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_bsic_description->ba_index_start_bsic = (byte) gs_bit_stream_extract_field(bit_stream_ptr,5);
    }
    else
    {
      gprs_bsic_description->ba_index_start_bsic = 0;
    }

    current_entry = &gprs_bsic_description->entries[gprs_bsic_description->entry_count++];

    /**
     * There is always at least 1 BSIC in the GPRS BSIC Description.
     */
    current_entry->bsic[0] = (byte) gs_bit_stream_extract_field(bit_stream_ptr, 6);
    current_entry->size = 1;
    gprs_bsic_description->entry_count = 1;

    MSG_GERAN_MED_1("Extracted a BSIC: %d", current_entry->bsic[0]);

    num_remaining_bsic = gs_bit_stream_extract_field(bit_stream_ptr, 7);

    while (num_remaining_bsic--)
    {
      /**
       * Check for frequency scrolling, indicating that the next BSIC is
       * for the next frequency index rather than the current frequency.
       */
      if (gs_bit_stream_extract_field(bit_stream_ptr, 1) == 1)
      {
        /* It is possible that the SI2quater message may indicate that it
        contains more BSICs than we have allocated storage for - if this is
        the case, we should continue to decode the message, but discard
        the information decoded.. */
        if (gprs_bsic_description->entry_count < MAX_GPRS_BSIC_DESCRIPTION_ENTRIES_PER_INSTANCE)
        {
          current_entry = &gprs_bsic_description->entries[gprs_bsic_description->entry_count++];
        }
        else
        {
          /* We have run out of storage entries.  We still need to continue
          decoding, but should discard the decoded values. */
          current_entry = NULL;
        }
      }

      if (current_entry != NULL && current_entry->size < MAX_BSIC_PER_FREQUENCY)
      {
        current_entry->bsic[current_entry->size] = (byte) gs_bit_stream_extract_field(bit_stream_ptr, 6);
        MSG_GERAN_MED_1("Extracted a BSIC: %d", current_entry->bsic[current_entry->size]);
        current_entry->size++;
      }
      else
      {
        /* Discard the information that would have been stored had current_entry
        not been NULL or had sufficient space.  This increments the internal
        counter in bit_stream to ensure decoder integrity for later structures. */
        (void) gs_bit_stream_extract_field(bit_stream_ptr, 6);
      }
    }

#ifdef FEATURE_LTE
    si2q_params->gprs_bsic_description = gprs_bsic_description;
#endif /* FEATURE_LTE */
  }
}

static void rr_extract_SI2quater_GPRS_Report_Priority_Description(
  gs_bit_stream_T *bit_stream_ptr,
  SI2q_params_T *si2q_params
)
{
  byte num_rep_pri;

  RR_NULL_CHECK_FATAL (bit_stream_ptr);
  RR_NULL_CHECK_FATAL (si2q_params);

  /* GPRS Report Priority Description */
  if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
  {
    byte Number_Cells = (byte) gs_bit_stream_extract_field(bit_stream_ptr,7);

    for (num_rep_pri = 0; num_rep_pri < Number_Cells; ++num_rep_pri)
    {
      /* REP_PRIORITY[] = */ (void) gs_bit_stream_extract_field(bit_stream_ptr,1);
    }
  }
}

static void rr_extract_SI2quater_GPRS_Measurement_Parameters_Description(
  gs_bit_stream_T *bit_stream_ptr,
  SI2q_params_T *si2q_params
)
{
  RR_NULL_CHECK_FATAL (bit_stream_ptr);
  RR_NULL_CHECK_FATAL (si2q_params);

  /* GPRS Measurement Parameters Description */
  if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
  {
    gprs_meas_params_mods_t *gprs_meas_params = &si2q_params->gprs_meas_params;

    gprs_meas_params->valid = TRUE;
    gprs_meas_params->report_type_valid = TRUE; /* always present in SI2quater */
    if (gs_bit_stream_extract_field(bit_stream_ptr,1) == 0)
    {
      gprs_meas_params->report_type = REPORT_TYPE_ENHANCED;
    }
    else
    {
      gprs_meas_params->report_type = REPORT_TYPE_N0RMAL;
    }

    gprs_meas_params->reporting_rate_valid = TRUE; /* always present in SI2quater */
    gprs_meas_params->reporting_rate = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

    gprs_meas_params->invalid_bsic_reporting_valid = TRUE; /* always present in SI2quater */
    gprs_meas_params->invalid_bsic_reporting = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_meas_params->multiband_reporting_valid = TRUE;
      gprs_meas_params->multiband_reporting = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_meas_params->serving_band_reporting_valid = TRUE;
      gprs_meas_params->serving_band_reporting = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);
    }

    gprs_meas_params->scale_ord_valid = TRUE; /* always present in SI2quater */
    gprs_meas_params->scale_ord = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_meas_params->reporting_900_valid = TRUE;
      gprs_meas_params->reporting_900_offset = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      gprs_meas_params->reporting_900_threshold = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_meas_params->reporting_1800_valid = TRUE;
      gprs_meas_params->reporting_1800_offset = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      gprs_meas_params->reporting_1800_threshold = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_meas_params->reporting_450_valid = TRUE;
      gprs_meas_params->reporting_450_offset = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      gprs_meas_params->reporting_450_threshold = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_meas_params->reporting_1900_valid = TRUE;
      gprs_meas_params->reporting_1900_offset = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      gprs_meas_params->reporting_1900_threshold = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      gprs_meas_params->reporting_850_valid = TRUE;
      gprs_meas_params->reporting_850_offset = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      gprs_meas_params->reporting_850_threshold = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }
    }
  }

static void rr_extract_SI2quater_NC_Measurement_Parameters_Description(
  gs_bit_stream_T *bit_stream_ptr,
  SI2q_params_T *si2q_params
)
{
  RR_NULL_CHECK_FATAL (bit_stream_ptr);
  RR_NULL_CHECK_FATAL (si2q_params);

  if (gs_bit_stream_extract_field(bit_stream_ptr,1))
  {
    si2q_params->nc_meas_params.valid = TRUE;
    si2q_params->nc_meas_params.network_control_order = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      si2q_params->nc_meas_params.nc_period_valid = TRUE;
      si2q_params->nc_meas_params.nc_non_drx_period = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      si2q_params->nc_meas_params.nc_reporting_period_i = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      si2q_params->nc_meas_params.nc_reporting_period_t = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }
  }
}

/**************************************************************************************
Function
rr_extract_SI2quater_Measurement_Parameters_Description

Description
Extract the normal Measurement Parameters Description into the temporary storage.

Parameters
[IN]
gs_bit_stream_T *bit_stream_ptr:
Pointer to the bit stream containing the SI2quater
received from the network. It is set to the position from where the
"Measurement Parameters Description" can be extracted.

[OUT]
SI2q_params_T *si2q_params:
Pointer to the temporary storage into which the measurement parameters
are extracted into.

Returns
None

**********************************************************************************************/
static void rr_extract_SI2quater_Measurement_Parameters_Description(
  gs_bit_stream_T *bit_stream_ptr,
  SI2q_params_T *si2q_params
)
{
  RR_NULL_CHECK_FATAL(bit_stream_ptr);
  RR_NULL_CHECK_FATAL(si2q_params);

  /* extract used fields into the the correct storage */
  si2q_params->MP_Desc_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
  if (si2q_params->MP_Desc_PI == 1)
  {
    si2q_params->REPORT_TYPE = (report_type_T) gs_bit_stream_extract_field(bit_stream_ptr,1);
    si2q_params->SERVING_BAND_REPORTING = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);
  }
}

/**************************************************************************************
Function
rr_extract_SI2quater_Extension_Information

Description
Extract the Extension Information into the temporary storage.

Parameters
[IN]
gs_bit_stream_T *bit_stream_ptr:
Pointer to the bit stream containing the SI2quater
received from the network. It is set to the position from where the
"Extension_Information" (including leading "presence" bit) can be extracted.

[OUT]
SI2q_params_T *si2q_params:
Pointer to the temporary storage into which the Extension_Information are extracted

Returns
None

**********************************************************************************************/
static void rr_extract_SI2quater_Extension_Information(
  gs_bit_stream_T *bit_stream_ptr,
  SI2q_params_T *si2q_params
)
{
  RR_NULL_CHECK_FATAL(bit_stream_ptr);
  RR_NULL_CHECK_FATAL(si2q_params);

  /* Extract the extension information */
  if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
  {
    uint16 extension_length = 1+(uint16)gs_bit_stream_extract_field(bit_stream_ptr,8);

    if (!bit_stream_ptr->overflow)
    {
      // should now be (extension_length) bits available

      // extract bit for CCN presence and reduce remaining bit count
      extension_length--;
      if (gs_bit_stream_extract_field(bit_stream_ptr,1))
      {
        byte num_cells;
        byte bit_mask = 1;
        byte octet    = 0;

        si2q_params->ccn_support_flag = TRUE;

        // extract number_cells and reduce count of remaining bits accordingly
        num_cells = (byte)gs_bit_stream_extract_field(bit_stream_ptr,7);
        extension_length -= (num_cells+7);

        // initialise the output structure
        si2q_params->ccn_support_desc.number_cells = num_cells;
        memset(si2q_params->ccn_support_desc.ccn_supported, 0, CCN_SUPPORT_BITMAP_LENGTH);

        // extract the bitmap
        while (num_cells)
        {
          if (gs_bit_stream_extract_field(bit_stream_ptr,1))
          {
            /* set this bit to 1 */
            si2q_params->ccn_support_desc.ccn_supported[octet] |= bit_mask;
          }
          bit_mask <<= 1;
          if (bit_mask == 0)
          {
            bit_mask = 1;
            octet++;
          }

          num_cells--;
        }
      }

      if (extension_length) // avoid trying to extract a zero-length bitstream below
      // skip spare bits up to remaining extension length
      {
        (void) gs_bit_stream_extract_field(bit_stream_ptr,extension_length);
      }
    }
  }
}

/**************************************************************************************
Function
rr_3G_BA_changed

Description
This function checks if there a change in the 3G_BA_IND.

Parameters
[IN]
instances_T           *instances:
Pointer to global storage for the already received instances.

SI2q_MI_params_T *common_params:
common_params containing the 3G_BA_IND received in the current instance.

Returns
TRUE if there is a change; FALSE otherwise.

**********************************************************************************************/

static boolean rr_3G_BA_changed(instances_T *instances, SI2q_MI_params_T *common_params)
{
  byte i;
  SI2q_MI_instance_T *inst;

  RR_NULL_CHECK_FATAL (instances);
  RR_NULL_CHECK_FATAL (common_params);

  /* need to loop through all the instances since the instances may be received
  out of order and the last received instance is not remembered */
  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    inst = &instances->instance[i];

    if (inst->valid_data)
    {
      if (inst->common_params.BA_IND_3G != common_params->BA_IND_3G)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**************************************************************************************
Function
rr_BA_changed

Description
This function checks if there a change in the BA_IND.

Parameters
[IN]
instances_T           *instances:
Pointer to global storage for the already received instances.

SI2q_MI_params_T *common_params:
common_params containing the BA_IND received in the current instance.

Returns
TRUE if there is a change; FALSE otherwise.

**********************************************************************************************/
static boolean rr_BA_changed(instances_T *instances, SI2q_MI_params_T *common_params)
{
  byte i;
  SI2q_MI_instance_T *inst;

  RR_NULL_CHECK_FATAL (instances);
  RR_NULL_CHECK_FATAL (common_params);

  /* need to loop through all the instances since the instances may be received
  out of order and the last received instance is not remembered */
  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    inst = &instances->instance[i];

    if (inst->valid_data)
    {
      if (inst->common_params.BA_IND != common_params->BA_IND)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**************************************************************************************
Function
rr_MP_changed

Description
This function checks if there a change in the the MP_CHANGE_MARK.

Parameters
[IN]
instances_T           *instances:
Pointer to global storage for the already received instances.

SI2q_MI_params_T *common_params:
common_params containing the MP_CHANGE_MARK received in the current instance.

Returns
TRUE if there is a change; FALSE otherwise.

**********************************************************************************************/
static boolean rr_MP_changed(instances_T *instances, SI2q_MI_params_T *common_params)
{
  byte i;
  SI2q_MI_instance_T *inst;

  RR_NULL_CHECK_FATAL (instances);
  RR_NULL_CHECK_FATAL (common_params);

  /* need to loop through all the instances since the instances may be received
  out of order and the last received instance is not remembered */
  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    inst = &instances->instance[i];

    if (inst->valid_data)
    {
      if (inst->common_params.MP_CHANGE_MARK != common_params->MP_CHANGE_MARK)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

#if 0  // not used at present
/**************************************************************************************
Function
rr_SI2q_reset_3g_measurement_parameters

Description
This function resets the SI2quater Measurement Parameters stored in the instances.

Parameters
[IN]
instances_T           *instances:
Pointer to global storage for the already received instances.

Returns
none

**********************************************************************************************/
void rr_SI2q_reset_3g_measurement_parameters(instances_T *instances)
{
  byte i;
  SI2q_MI_instance_T *inst;

  RR_NULL_CHECK_FATAL (instances);

  /* need to loop through all the instances since the instances may be received
  out of order and the last received instance is not remembered */
  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    inst = &instances->instance[i];

    if (inst->valid_data)
    {
      /* set all si2q parameters to 0 */
      memset((void *) &inst->si2q_mi.si2q_params, 0, sizeof(SI2q_params_T));
    }
  }
}
#endif // #if 0

#if 0  // not used at present
/**************************************************************************************
Function
rr_MI_reset_3g_measurement_parameters

Description
This function resets the MI Measurement Parameters stored in the instances.

Parameters
[IN]
instances_T           *instances:
Pointer to global storage for the already received instances.

Returns
none

**********************************************************************************************/
void rr_MI_reset_3g_measurement_parameters(instances_T *instances)
{
  byte i;
  SI2q_MI_instance_T *inst;

  RR_NULL_CHECK_FATAL (instances);

  /* need to loop through all the instances since the instances may be received
  out of order and the last received instance is not remembered */
  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    inst = &instances->instance[i];

    if (inst->valid_data)
    {
      /* set all si2q parameters to 0 */
      memset((void *) &inst->si2q_mi.mi_params, 0, sizeof(MI_params_T));
    }
  }
}
#endif // #if 0

/**************************************************************************************
Function
rr_MI_num_instances_with_3g_NC_Desc

Description
This function gets the number instances with 3G NC description.

Parameters
[IN]
instances_T           *instances:
Pointer to global storage for the already received instances.

Returns
none

**********************************************************************************************/
static byte rr_MI_num_instances_with_3g_NC_Desc(instances_T *instances)
{
  byte i;
  SI2q_MI_instance_T *inst;
  byte num_instances = 0;

  RR_NULL_CHECK_FATAL (instances);

  /* need to loop through all the instances since the instances may be received
  out of order and the last received instance is not remembered */
  for (i = 0; i < MAX_INSTANCES; ++i)
  {
    inst = &instances->instance[i];

    if (inst->valid_data)
    {
      if (inst->common_params.WCDMA_3G_NC_Desc_PI)
      {
        num_instances++;
      }
    }
  }
  return num_instances;
}

/**************************************************************************************
Function
rr_SI2q_MI_check_for_updates

Description
This function checks if an instance was received already and
whether there is any updates to the instance that was received
already.


Parameters
[IN]
gs_bit_stream_T *bit_stream_ptr:
Pointer to the bit stream containing the SI2quater or the Measurement Information
received from the network. It is set to the position from where the
"3G Neighbor Cell Description" can be extracted.

[OUT]
SI2q_MI_params_T *common_params:
Pointer to the temporary storage into which the parameters common to both SI2quater
and Measurement Information are extracted into.

instances_T           *instances:
Pointer to global storage for the instances.

boolean               *send_wcdma_neighbor_list_to_L1:
Set to TRUE if any instance has changed.

boolean               *si2quater_received:
Set to TRUE if all instances have been received

Returns
TRUE if there is an update; FALSE otherwise.

**********************************************************************************************/

static boolean rr_SI2q_MI_check_for_updates(
  gs_bit_stream_T  *bit_stream_ptr,
  SI2q_MI_params_T *common_params,
  instances_T      *instances,
  boolean          *si2q_mi_received
)
{
  RR_NULL_CHECK_FATAL(bit_stream_ptr);
  RR_NULL_CHECK_FATAL(common_params);
  RR_NULL_CHECK_FATAL(instances);
  RR_NULL_CHECK_FATAL(si2q_mi_received);

  /* Extract BA_IND. Check whether BA_IND is changed or not.
     if BA_IND is changed then we need to build the BSIC list again.*/
  common_params->BA_IND = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

  /* Read subsequent parameters that indicate if there is any change. */
  common_params->BA_IND_3G = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
  common_params->MP_CHANGE_MARK = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
  common_params->INDEX = (byte) gs_bit_stream_extract_field(bit_stream_ptr,4);
  common_params->COUNT = (byte) gs_bit_stream_extract_field(bit_stream_ptr,4);

  if (rr_SI2q_MI_instance_already_received(instances, common_params->INDEX))
  {
    /* Check if there is no change in both the 3G_BA_IND and MP_CHANGE_MARK */
    if (
        (rr_3G_BA_changed(instances, common_params) == FALSE) &&
        (rr_MP_changed(instances, common_params) == FALSE) &&
        (rr_BA_changed(instances, common_params) == FALSE)
       )
    {
      /* No Change; This instance has been received and processed already; So Ingore */
      MSG_GERAN_HIGH_2("Received inst %d again",(int) common_params->INDEX, 0 );

      /* Update the received/changed status of this instance to indicate that the
      instance was received but not changed */
      rr_update_status_SI2q_MI_instance (instances, common_params->INDEX, TRUE, FALSE);


      /* If all instances are received then the si2quater_received is set to TRUE; This is done
      in order to set the SI2_QUATER bit in the system information database or pending channel
      data */
      if (rr_received_all_instances(instances, instances->num_instances,common_params->COUNT))
      {
        *si2q_mi_received = TRUE;

        /* Determine if all the instances are received and if there was a change in any instance; in which
        case set the send_wcdma_neighbor_list_to_L1 flag to TRUE */

      }

      /* no update so return false */
      return FALSE;
    }
  } /* if (rr_SI2q_MI_instance_already_received */

  /* updated instance */
  return TRUE;
}

/**************************************************************************************
Function
rr_SI2q_MI_decode_FDD_cells

Description
This function decodes the FDD cells using the Range 1024 decoding algorithm.


Parameters
[IN]

byte num_encoded_cells
Number of encoded cells

word *encoded_cells
Array containing the encoded cells

cell_list_elements - number of elements in the array pointed to by cell_list

[OUT]
word *cell_list,
Array to store the decoded cells

boolean *decoding_ok
FALSE if the decoding algorithm fails to decode correctly; TRUE otherwise

Returns
None
**********************************************************************************************/

static void rr_SI2q_MI_decode_cells(
  byte num_encoded_cells,
  word *encoded_cells,
  word *cell_list,
  uint8 cell_list_elements,
  boolean *decoding_ok,
  rr_utran_rat_type_t utran_type
)
{
  RR_NULL_CHECK_FATAL (encoded_cells);
  RR_NULL_CHECK_FATAL (cell_list);
  RR_NULL_CHECK_FATAL (decoding_ok);
  /* ASSERT (num_encoded_cells < MAX_UTRAN_CELLS_PER_FREQUENCY+1); */
  if (num_encoded_cells >= (MAX_UTRAN_CELLS_PER_FREQUENCY+1))
  {
    MSG_GERAN_ERROR_0("num_encoded_cells >= (MAX_UTRAN_CELLS_PER_FREQUENCY+1)!!");
  }

  *decoding_ok = TRUE;

  // Initialize the decoded_cells array
  memset ((void *)cell_list, 0, sizeof (cell_list));

  if (utran_type == RR_UTRAN_FDD)
  {
    // Run the Range 1024 decoding algorithm
    *decoding_ok = decode (encoded_cells, num_encoded_cells, cell_list, cell_list_elements, 1023);
  }
  else if(utran_type == RR_UTRAN_TDD)
  {
    // Run the Range 512 decoding algorithm
    *decoding_ok = decode (encoded_cells, num_encoded_cells, cell_list, cell_list_elements, 511);
  }
  else
  {
    *decoding_ok = FALSE;
  }


  /* The Range decode returns FALSE if there is any error in the input */
  if (*decoding_ok == FALSE)
  {
    MSG_GERAN_ERROR_0("Error in Range decode");
    return;
  }
}

/**************************************************************************************
Function
rr_process_3g_BA

Description
This function decodes the cells received in the 3G Neighbor Cell Description of
the SI2quater or MI instance into the instance structure. It sorts the decoded
cells using the qsort routine.


Parameters
[IN]
encoded_FDD_info_T *encoded_cells
Array containing the encoded cells

byte num_repeated_FDD_info
Number of FDD information structs in the instance

[OUT]
SI2q_MI_instance_T *instance
Pointer to the instance

boolean *decoding_ok
FALSE if the decoding algorithm fails to decode correctly; TRUE otherwise

Returns
None
**********************************************************************************************/

static void rr_process_3g_BA(
  SI2q_MI_instance_T *instance,
  encoded_FDD_info_T *encoded_fdd_cells,
  SI2q_MI_params_T *common_params,
  boolean *decoding_ok,
  encoded_TDD_info_T *encoded_tdd_cells
)
{
  byte rep_FDD_index;
  byte rep_TDD_index;
  byte num_supported_uarfcns = 0;

  RR_NULL_CHECK_FATAL (instance);
  RR_NULL_CHECK_FATAL (encoded_fdd_cells);
  RR_NULL_CHECK_FATAL (decoding_ok);
  RR_NULL_CHECK_FATAL (encoded_tdd_cells);
  RR_NULL_CHECK_FATAL (common_params);

  /* Proceed only if there is still space left to store frequencies or cells in the utran_neighbor_list */

  /* Fill in the frequency list and the cell list of utran_neighbor_list */
  /* There could be a list of frequencies and a list of cells for each frequency */

  /**********************************************************************************************/
  /*********************************** FDD Decode ***********************************************/
  /**********************************************************************************************/
  for (rep_FDD_index = 0; rep_FDD_index < common_params->num_repeated_FDD_info; ++rep_FDD_index)
  {
    encoded_FDD_info_T *encoded_cells_ptr = &encoded_fdd_cells[rep_FDD_index];

    /* temporary alias */
    FDD_Info_T *fdd_info = &instance->FDD_Info[rep_FDD_index];

    word local_uarfcn = encoded_cells_ptr->uarfcn;

    /* count number of valid uarfcns this will be used to set
    the num FDD info for this instance later in this function */
    num_supported_uarfcns++;

    /* Store in the instance */
    fdd_info->uarfcn = local_uarfcn;

    if (encoded_cells_ptr->num_cells > 0 && encoded_cells_ptr->num_cells < MAX_UTRAN_CELLS_PER_INSTANCE)
    {
      /* Decode the FDD cell list for each of the frequencies received */
      rr_SI2q_MI_decode_cells(
        encoded_cells_ptr->num_cells,
        encoded_cells_ptr->encoded_cells,
        fdd_info->decoded_and_sorted_cell_values,
        NELEMS(fdd_info->decoded_and_sorted_cell_values),
        decoding_ok,
        RR_UTRAN_FDD
      );

      if (*decoding_ok == FALSE)
      {
        MSG_GERAN_ERROR_1("Failed to decode cell list for uarfcn",(int) encoded_cells_ptr->uarfcn);
        return;
      }

      /* Copy over the num_cells */
      fdd_info->num_cells = encoded_cells_ptr->num_cells;

      /* Sort the values */
      qsort(
        fdd_info->decoded_and_sorted_cell_values,
        fdd_info->num_cells + 1,
        sizeof(word),
        compare
      );

    } /* num_cells > 0 */

    /* copy the retrived FDD_Indic0 into the instances */
    fdd_info->FDD_Indic0 = encoded_cells_ptr->FDD_Indic0;
    fdd_info->NR_OF_FDD_CELLS = encoded_cells_ptr->NR_OF_FDD_CELLS;
  } /* for loop for num_repeated_FDD_info */

  /* copy the actual number of supported uarfcns to be updated to the
  instance.common_params.num_repeated_FDD_info and used from now on */
  common_params->num_repeated_FDD_info = num_supported_uarfcns;


  /**********************************************************************************************/
  /*********************************** TDD Decode ***********************************************/
  /**********************************************************************************************/

  num_supported_uarfcns = 0;

  for (rep_TDD_index = 0; rep_TDD_index < common_params->num_repeated_TDD_info; ++rep_TDD_index)
  {
    encoded_TDD_info_T *encoded_cells_ptr = &encoded_tdd_cells[rep_TDD_index];

    /* temporary alias */
    TDD_Info_T *tdd_info = &instance->TDD_Info[rep_TDD_index];

    word local_uarfcn = encoded_cells_ptr->uarfcn;

    /* count number of valid uarfcns this will be used to set
    the num TDD info for this instance later in this function */
    num_supported_uarfcns++;

    /* Store in the instance */
    tdd_info->uarfcn = local_uarfcn;

    if (encoded_cells_ptr->num_cells > 0 && encoded_cells_ptr->num_cells < MAX_UTRAN_CELLS_PER_INSTANCE)
    {
      /* Decode the TDD cell list for each of the frequencies received */
      rr_SI2q_MI_decode_cells(
        encoded_cells_ptr->num_cells,
        encoded_cells_ptr->encoded_cells,
        tdd_info->decoded_and_sorted_cell_values,
        NELEMS(tdd_info->decoded_and_sorted_cell_values),
        decoding_ok,
        RR_UTRAN_TDD
      );

      if (*decoding_ok == FALSE)
      {
        MSG_GERAN_ERROR_1("Failed to decode cell list for uarfcn",(int) encoded_cells_ptr->uarfcn);
        return;
      }

      /* Copy over the num_cells */
      tdd_info->num_cells = encoded_cells_ptr->num_cells;

      /* Sort the values */
      qsort(
        tdd_info->decoded_and_sorted_cell_values,
        tdd_info->num_cells + 1,
        sizeof(word),
        compare
      );

    } /* num_cells > 0 */

    /* copy the retrived FDD_Indic0 into the instances */
    tdd_info->TDD_Indic0 = encoded_cells_ptr->TDD_Indic0;
    tdd_info->NR_OF_TDD_CELLS = encoded_cells_ptr->NR_OF_TDD_CELLS;
  } /* for loop for num_repeated_FDD_info */

  /* copy the actual number of supported uarfcns to be updated to the
  instance.common_params.num_repeated_FDD_info and used from now on */
  common_params->num_repeated_TDD_info = num_supported_uarfcns;
}

/*
 *   Function name:  rr_initialize_encoded_FDD_info
 *   ----------------------------------------------------
 *
 *   Description: Initialize the SI2quater_Encoded_Info used in rr_decode_neighbour_cell_description_2quater
 *   ------------
 *
 *   Parameters: tempSI2quater_Encoded_Info, max_no_encoded_info
 *   -----------
 *
 *   Returns: none
 *   --------
 *
 */
static void rr_initialize_encoded_FDD_info(encoded_FDD_info_T *tempEncoded_FDD_Info, byte max_no_encoded_info)
{
  RR_NULL_CHECK_FATAL (tempEncoded_FDD_Info);
  memset ((void *)tempEncoded_FDD_Info, 0, (sizeof (encoded_FDD_info_T))*max_no_encoded_info);
}

boolean rr_si2ter_3G_BA_changed(si2ter_instances_T *instances, boolean si2ter_3g_change_mark)
{
  byte i;
  si2ter_rest_T *inst;

  RR_NULL_CHECK_FATAL (instances);

  /* need to loop through all the instances since the instances may be received
  out of order and the last received instance is not remembered */
  for (i = 0; i < MAX_SI2TER_INSTANCES; ++i)
  {
    inst = &instances->instance[i];

    if (inst->valid_data)
    {
      if (inst->SI2ter_3G_CHANGE_MARK != si2ter_3g_change_mark)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

boolean rr_si2ter_MP_changed(si2ter_instances_T *instances, boolean si2ter_mp_change_mark)
{
  byte i;
  si2ter_rest_T *inst;

  RR_NULL_CHECK_FATAL (instances);

  /* need to loop through all the instances since the instances may be received
  out of order and the last received instance is not remembered */
  for (i = 0; i < MAX_SI2TER_INSTANCES; ++i)
  {
    inst = &instances->instance[i];

    if (inst->valid_data)
    {
      if (inst->SI2ter_MP_CHANGE_MARK != si2ter_mp_change_mark)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}


void rr_update_status_si2ter_instance (
  si2ter_instances_T *instances,
  byte index,
  boolean received,
  boolean changed
)
{
  RR_NULL_CHECK_FATAL (instances);
  /* ASSERT (index < MAX_SI2TER_INSTANCES); */
  if (index < MAX_SI2TER_INSTANCES)
  {
    instances->instance[index].valid_data = received;
    instances->instance[index].changed = changed;
  }
  else
  {
    MSG_GERAN_ERROR_0("index >= MAX_SI2TER_INSTANCES!!");
  }

}



/**************************************************************************

Function:
rr_received_all_si2ter_instances

Description:
Reset the 'changed' flag in the instances 0 till the specified array_length.

Parameters:
[IN]
si2ter_instances_T *instances
Pointer to the SI2ter instances
byte array_length
Number of instances received
byte COUNT
Maximum number of instances of SI2ter COUNT specified in the instances.

Returns:
TRUE if all instances have been received; FALSE otherwise

***************************************************************************/
boolean rr_received_all_si2ter_instances(si2ter_instances_T *instances, byte array_length, byte COUNT)
{
  int i;
  boolean received;

  RR_NULL_CHECK_FATAL (instances);
  /* ASSERT (array_length <= MAX_SI2TER_INSTANCES); */
  if (array_length > MAX_SI2TER_INSTANCES)
  {
    MSG_GERAN_ERROR_1("array_length > MAX_SI2TER_INSTANCES, truncating to %d",
              MAX_SI2TER_INSTANCES);
    array_length = MAX_SI2TER_INSTANCES;
  }
  /* ASSERT (COUNT < MAX_SI2TER_INSTANCES); */
  if (COUNT >= MAX_SI2TER_INSTANCES)
  {
    MSG_GERAN_ERROR_1("COUNT >= MAX_SI2TER_INSTANCES, truncating to %d",
              MAX_SI2TER_INSTANCES - 1);
    COUNT = MAX_SI2TER_INSTANCES - 1;
  }

  if (array_length == COUNT+1) /* For one instance SI2q/MI count = 0, hence the +1 */
  {
    received = TRUE;

    for (i = 0; i < array_length; ++i)
    {
      /* bitwise AND of the received bit of all instances */
      received &= instances->instance[i].valid_data;

      /* When received becomes FALSE return received immediately since bitwise AND will eventually return FALSE */
      if (received == FALSE)
      {
        return received;
      }
    }
    return received;
  }

  return FALSE;
}



/**************************************************************************

Function:
rr_si2ter_changed

Description:
Checks if there is a change in any of the instances received (from 0 till
array_length)

Parameters:
[IN]
si2ter_instances_T *instances
Pointer to the SI2ter instances array

byte array_length
length of the instance array

byte COUNT
Maximum number of SI2ter that is specified in the instance.

Returns
TRUE if atleast one of the instances has changed; FALSE otherwise

***************************************************************************/

boolean rr_si2ter_changed (si2ter_instances_T *instances)
{
  int i;
  boolean changed = FALSE;

  if (instances == NULL)
  {
    MSG_GERAN_ERROR_0("NULL ptr passed to rr_SI2q_MI_changed!!");
    return FALSE;
  }

  if (instances->num_instances > MAX_SI2TER_INSTANCES)
  {
    MSG_GERAN_ERROR_0("array_length > MAX_INSTANCES!!");
    instances->num_instances = MAX_SI2TER_INSTANCES;
  }

  for (i = 0; ( (i < MAX_SI2TER_INSTANCES) && (!changed) ) ; ++i)
  {
    if (instances->instance[i].valid_data)
    {
      /* bitwise OR of the received bit of all instances */
      changed |= instances->instance[i].changed;
    }
  }

  return changed;
}

/**************************************************************************************
Function
rr_si2ter_reset_3g_measurement_parameters

Description
This function resets the MI Measurement Parameters stored in the instances.

Parameters
[IN]
si2ter_instances_T           *instances:
Pointer to global storage for the already received instances.

Returns
none

**********************************************************************************************/
static void rr_si2ter_reset_3g_measurement_parameters(si2ter_instances_T *instances)
{
  byte i;
  si2ter_rest_T *inst;

  RR_NULL_CHECK_FATAL (instances);

  /* need to loop through all the instances since the instances may be received
  out of order and the last received instance is not remembered */
  for (i = 0; i < MAX_SI2TER_INSTANCES; ++i)
  {
    inst = &instances->instance[i];

    if (inst->valid_data)
    {
      inst->FDD_Qmin = 0;
      inst->FDD_Qoffset = 8; /* 0 db */
      inst->TDD_Qoffset = 5; /* -90 db */
      inst->Qsearch_I = 15;
      inst->WCDMA_3G_MP_Desc_PI = FALSE;
      inst->WCDMA_3G_MP_FDD_Params_PI = FALSE;
      inst->FDD_additional_MPs_valid = FALSE;
      inst->FDD_Qmin_offset = DEFAULT_FDD_QMIN_OFFSET;
      inst->FDD_RSCPmin = DEFAULT_FDD_RSCPMIN;
    }
  }
}
/*
 *   Function name:  rr_decode_si2ter_rest_octets
 *   --------------------------------------------
 *
 *   Description: Decode the neighbour cell description 2ter Rest Octets IE.
 *   ------------
 *   Extract all the data from the IE into local storage while
     checking for any errors  or stoppers when decoding.
 *
 *   Parameters:
 *   -----------
 *   [IN] = IE - pointer to the SI 2ter rest octets
            IE_length - (used to initialize the bitstream with gs_bit_stream_initialize)
            stored_in_sidb - flag when FALSE means the SI's are being stored in the pending_
            channel_data; when TRUE it means the SI's are being stored in the system_information_
            database.
     [OUT] =
             decoding_ok - indicates whether the decoding process was successful.
 *
 *   Returns: none
 *   --------
 *
 */
void rr_decode_si2ter_rest_octets
(
  byte                          *IE,
  byte                           IE_length,
  si2ter_instances_T            *si2ter_rest_instances,
  boolean                       *si2ter_received,
  boolean                       *decoding_ok,
  system_information_database_T *sys_info_db_ptr,
  const gas_id_t                gas_id
)
{

  si2ter_rest_T this_si2ter_instance;

  /* Using bitstream routines to extract the bits from the rest octet */
  gs_bit_stream_T bitstream;
  gs_bit_stream_T *bit_stream_ptr;

  /* Asserts */
  RR_NULL_CHECK_FATAL (IE);
  RR_NULL_CHECK_FATAL (si2ter_rest_instances);
  RR_NULL_CHECK_FATAL (si2ter_received);
  RR_NULL_CHECK_FATAL (decoding_ok);

  /* initialize local */
  memset((void *)&this_si2ter_instance, 0, sizeof(si2ter_rest_T));

  /* Initialize output values */
  *decoding_ok = TRUE;
  *si2ter_received = FALSE;

  /* Initialize bit stream */
  gs_bit_stream_initialize(&bitstream,IE,IE_length);
  bit_stream_ptr = &bitstream;

  if (gs_bit_stream_extract_HL(bit_stream_ptr) == 0) /* L */
  {
    MSG_GERAN_MED_0("SI2ter rest not present");

    *si2ter_received = TRUE;
    return;
  }

  /*********************************************************************/
  /* Start Extracting information into local storage                   */
  /*********************************************************************/
  this_si2ter_instance.SI2ter_MP_CHANGE_MARK = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
  this_si2ter_instance.SI2ter_3G_CHANGE_MARK = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
  this_si2ter_instance.SI2ter_INDEX = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
  this_si2ter_instance.SI2ter_COUNT = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);

  /* Check if the instance number is valid */
  if (this_si2ter_instance.SI2ter_INDEX > this_si2ter_instance.SI2ter_COUNT)
  {
    MSG_GERAN_HIGH_2("SI2ter_INDEX > SI2ter_COUNT in SI 2ter(%d > %d)",(int) this_si2ter_instance.SI2ter_INDEX,
                                                               (int) this_si2ter_instance.SI2ter_COUNT );
    *decoding_ok = FALSE;
    return;
  }

  /* if the instance was already received */
  if (si2ter_rest_instances->instance[this_si2ter_instance.SI2ter_INDEX].valid_data)
  {
    /* Check if there is no change in both the 3G_BA_IND and MP_CHANGE_MARK */
    if (
        (rr_si2ter_3G_BA_changed(si2ter_rest_instances, this_si2ter_instance.SI2ter_3G_CHANGE_MARK) == FALSE) &&
        (rr_si2ter_MP_changed(si2ter_rest_instances, this_si2ter_instance.SI2ter_MP_CHANGE_MARK) == FALSE)
       )
    {
      MSG_GERAN_HIGH_2("Rx SI2ter(inst=%d, max=%d): No 3G BA or MP change", this_si2ter_instance.SI2ter_INDEX, this_si2ter_instance.SI2ter_COUNT);

      /* Update the received/changed status of this instance to indicate that the
      instance was received but not changed */
      rr_update_status_si2ter_instance (si2ter_rest_instances, this_si2ter_instance.SI2ter_INDEX, TRUE, FALSE);

      /* If all instances are received then the si2ter_received is set to TRUE; This is done
      in order to set the SI2_ter bit in the system information database or pending channel
      data */
      if (rr_received_all_si2ter_instances(si2ter_rest_instances, si2ter_rest_instances->num_instances,
                                              this_si2ter_instance.SI2ter_COUNT))
      {
        *si2ter_received = TRUE;
      }
      /* decoding_ok is TRUE */
      return;
    }
  }


  /* Continue extracting */
  if (gs_bit_stream_extract_field(bit_stream_ptr,1) == TRUE)
  {
    if (gs_bit_stream_extract_field(bit_stream_ptr,2) == 0x01)
    {
      this_si2ter_instance.FDD_INFO_PI = TRUE;
      this_si2ter_instance.FDD_arfcn = (word) gs_bit_stream_extract_field(bit_stream_ptr,14);
    }

    if (gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      /* byte bandwidth_FDD = */ (void) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }
  }

  /* extract and ignore TDD information */
  if (gs_bit_stream_extract_field(bit_stream_ptr,1))
  {
    if (gs_bit_stream_extract_field(bit_stream_ptr,2) == 0x01)
    {
      this_si2ter_instance.TDD_INFO_PI = TRUE;
      this_si2ter_instance.TDD_arfcn = (word) gs_bit_stream_extract_field(bit_stream_ptr,14);
    }

    if (gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      this_si2ter_instance.TDD_BANDWIDTH_PI = TRUE;
      this_si2ter_instance.TDD_BANDWIDTH = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }
  }

  /* Extract 3G MP */
  if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
  {
    this_si2ter_instance.WCDMA_3G_MP_Desc_PI = TRUE;
    this_si2ter_instance.Qsearch_I = (byte) gs_bit_stream_extract_field(bit_stream_ptr,4);

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      this_si2ter_instance.WCDMA_3G_MP_FDD_Params_PI = TRUE;
      this_si2ter_instance.FDD_Qoffset = (byte) gs_bit_stream_extract_field(bit_stream_ptr,4);
      this_si2ter_instance.FDD_Qmin = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      this_si2ter_instance.WCDMA_3G_MP_TDD_Params_PI = TRUE;
      this_si2ter_instance.TDD_Qoffset = (byte) gs_bit_stream_extract_field(bit_stream_ptr,4);
    }
  }

  /* Extract 3G Additional Measurement Parameters Description */
  if (gs_bit_stream_extract_HL(bit_stream_ptr))
  {
    if (gs_bit_stream_extract_field(bit_stream_ptr, 1) == 1)
    {
      if (!bit_stream_ptr->overflow)
      {
        this_si2ter_instance.FDD_additional_MPs_valid = TRUE;
        this_si2ter_instance.FDD_Qmin_offset = (byte) gs_bit_stream_extract_field(bit_stream_ptr, 3);
        this_si2ter_instance.FDD_RSCPmin = (byte) gs_bit_stream_extract_field(bit_stream_ptr, 4);
      }
    }
  }

  /************ Done extracting information from the IE into local storage ***************/

  /***************************************************************************************/
  /* Start processing the instance:
     1) Store changes to the 3G_BA_IND, MP_CHANGE_MARK
     2) Transfer the contents in temporary storage into the instances.
     3) Determine if all instances have been received
  */
  /***************************************************************************************/

  MSG_GERAN_HIGH_2("Decoding SI2ter instance %d, max=%d",(int) this_si2ter_instance.SI2ter_INDEX, (int) this_si2ter_instance.SI2ter_COUNT );

  /* Will be here only if either the 3G_BA_IND or the MP_CHANGE_MARK has changed */

  /* Check if there is a change in 3G_BA_IND */
  if (rr_si2ter_3G_BA_changed(si2ter_rest_instances, this_si2ter_instance.SI2ter_3G_CHANGE_MARK))
  {
    /* Re-read all instances of SI2ter and rebuild the 3G Neighbor list */
    /* This will set received/changed to FALSE */
    rr_reset_SI2ter_instances(si2ter_rest_instances);

    if (sys_info_db_ptr->cell_identity_valid_flag)
    {
      MSG_GERAN_HIGH_0_G("SI2ter 3G_BA_IND changed flush SI cache");
      rr_csi_flush_data(&sys_info_db_ptr->location_area_identification,
                        &sys_info_db_ptr->cell_identity,
                         CSI_FLUSH_SI_CHANGE,
                         gas_id);
    }
  }

  /* Check if there is a change in MP_CHANGE_MARK */
  if (rr_si2ter_MP_changed(si2ter_rest_instances, this_si2ter_instance.SI2ter_MP_CHANGE_MARK))
  {
    /* If the MP_CHANGE_MARK parameter is changed, the MS shall re-read the Real Time differences,
    REP_PRIORITY, Measurement Parameters and 3G Measurement Parameters in all instances. */
    /* Invalidate whatever received till now */
    rr_si2ter_reset_3g_measurement_parameters(si2ter_rest_instances);

    if (sys_info_db_ptr->cell_identity_valid_flag)
    {
      MSG_GERAN_HIGH_0_G("SI2ter MP_CHANGE_MARK changed flush SI cache");
      rr_csi_flush_data(&sys_info_db_ptr->location_area_identification,
                        &sys_info_db_ptr->cell_identity,
                         CSI_FLUSH_SI_CHANGE,
                         gas_id);
    }
  }

  /* copy the current instance information into the global instances structure */
  si2ter_rest_instances->instance[this_si2ter_instance.SI2ter_INDEX] = this_si2ter_instance;


  /*********************************************************/
  /* Fill in other fields in the global SI2ter instance */
  /*********************************************************/
  /* Check if the instance was not received already; The instance could have been received
  already and the one that is just processed now might be an update in which case the num_instances
  is not incremented */
  if (si2ter_rest_instances->instance[this_si2ter_instance.SI2ter_INDEX].valid_data == FALSE)
  {
    si2ter_rest_instances->num_instances++;
    si2ter_rest_instances->instance[this_si2ter_instance.SI2ter_INDEX].valid_data = TRUE;
  }
  /******************************************************************************************/
  /* Update the SI2ter array to indicate that this instance has been received and changed.
  This has to be done even if the instance was received already because the received and changed
  status variables are reset once a set of instances have been completely received. */
  /******************************************************************************************/
  rr_update_status_si2ter_instance (si2ter_rest_instances, this_si2ter_instance.SI2ter_INDEX, TRUE, TRUE);

  /*******************************************/
  /* Set the output parameters appropriately */
  /*******************************************/
  /* If all instances are received then the si2ter_received is set to TRUE; This is done
  in order to set the SI2_TER bit in the system information database or pending channel
  data */
  if (rr_received_all_si2ter_instances(si2ter_rest_instances,si2ter_rest_instances->num_instances,this_si2ter_instance.SI2ter_COUNT))
  {
    *si2ter_received = TRUE;
  }

  MSG_GERAN_HIGH_2("Decoded SI2ter inst=%d, max=%d", this_si2ter_instance.SI2ter_INDEX, this_si2ter_instance.SI2ter_COUNT);
}

/**
< PCID Group IE > ::=
  { 1 < PCID : bit (9) > } ** 0   -- explicit PCID coding
  { 0 | 1  < PCID_BITMAP_GROUP : bit (6) > }
  { 1 < PCID_Pattern_length : bit (3) >
    < PCID_Pattern : bit (val(PCID_Pattern_length + 1)) >
    < PCID_pattern_sense : bit (1) >
  } ** 0 ;

  As can be seen in the above encoding, information may be encoded in a number
  of different ways for this IE.  They are NOT mutually exclusive, even though
  it is difficult to come up with a use case which would require the use of
  more than one of these encodings and it is not obvious how to combine the
  results of the various encodings.
*/
static void rr_pcid_group_ie_decode(
  gs_bit_stream_T    * bitstream,
  rr_pcid_group_ie_t * dst
)
{
  RR_NULL_CHECK_FATAL(bitstream);
  RR_NULL_CHECK_FATAL(dst);

  memset(dst, 0, sizeof(rr_pcid_group_ie_t));

  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    /* Extract an individual PCID */
    uint32 pcid = gs_bit_stream_extract_field(bitstream, 9);

    if (pcid <= RR_LTE_PCID_MAX)
    {
      RR_PCID_GROUP_IE_SET(dst->bitmap, pcid);
    }
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    /*
    PCID_BITMAP_GROUP (6 bit field)
    If bit number n, where n can take the value from 1 to 6, in this field is
    set to '1', then cells with PCID values in the range
    (n-1) * 84 to (n*84)-1 inclusive are not allowed.
    */
    uint32 pcid_bitmap_group = gs_bit_stream_extract_field(bitstream, 6);
    uint32 n;

    for (n = 1; n < 7; ++n)
    {
      uint32 i;

      if (IS_BIT_NUM_SET(pcid_bitmap_group, (n - 1)))
      {
        uint32 lower = (n - 1) * 84;
        uint32 upper = (n * 84) - 1;
        MSG_GERAN_MED_3("PCID Bitmap Group %d is in PCID group (from %d to %d inclusive)", n, lower, upper);
        for (i = lower; i <= upper && i <= RR_LTE_PCID_MAX; ++i)
        {
          RR_PCID_GROUP_IE_SET(dst->bitmap, i);
        }
      }
    }
  }

  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    uint32 pcid_pattern_length = gs_bit_stream_extract_field(bitstream, 3) + 1;
    uint32 pcid_pattern = gs_bit_stream_extract_field(bitstream, pcid_pattern_length);
    uint32 pcid_pattern_sense = gs_bit_stream_extract_field(bitstream, 1);

    /* Max PCID length is 9 bits.  We have n bits of pattern.  We need to shift this
    left. */
    uint32 mask = pcid_pattern << (9 - pcid_pattern_length);
    uint32 i;

    for (i = 0; i <= RR_LTE_PCID_MAX; ++i)
    {
      if (pcid_pattern_sense == 0)
      {
        if ((i & mask) == mask)
        {
          RR_PCID_GROUP_IE_SET(dst->bitmap, i);
        }
      }
      else
      {
        if ((i & mask) != mask)
        {
          RR_PCID_GROUP_IE_SET(dst->bitmap, i);
        }
      }
    }
  }
}

static void rr_si2quater_ro_serving_cell_priority_params_desc_decode(
  gs_bit_stream_T * bitstream,
  csn_gsm_priority_params_ie_t * dst   
)
{
  RR_NULL_CHECK_FATAL(bitstream);
  RR_NULL_CHECK_FATAL(dst);

  dst->geran_priority = (uint8) gs_bit_stream_extract_field(bitstream, 3);
  dst->thresh_priority_search = (uint8) gs_bit_stream_extract_field(bitstream, 4);
  dst->thresh_gsm_low = (uint8) gs_bit_stream_extract_field(bitstream, 4);
  dst->h_prio = (uint8) gs_bit_stream_extract_field(bitstream, 2);
  dst->t_reselection = (uint8) gs_bit_stream_extract_field(bitstream, 2);
}

static void rr_si2quater_ro_utran_priority_params_decode(
  gs_bit_stream_T * bitstream,
  csn_utran_priority_params_t * dst 
)
{

  RR_NULL_CHECK_RETURN_VOID(bitstream);
  RR_NULL_CHECK_RETURN_VOID(dst);

  memset(dst, 0, sizeof(csn_utran_priority_params_t));

  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    uint8 frequency_index = (uint8) gs_bit_stream_extract_field(bitstream, 5);
    SET_BIT_NUM(dst->frequency_index_bitmap, frequency_index);
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->priority_valid = TRUE;
    dst->priority = (uint8) gs_bit_stream_extract_field(bitstream, 3);
  }

  dst->thresh_high = (uint8) gs_bit_stream_extract_field(bitstream, 5);

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->thresh_low_valid = TRUE;
    dst->thresh_low = (uint8) gs_bit_stream_extract_field(bitstream, 5);
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->qrxlevmin_valid = TRUE;
    dst->qrxlevmin = (uint8) gs_bit_stream_extract_field(bitstream, 5);
  }

}

static void rr_si2quater_ro_utran_priority_params_desc_decode(
  gs_bit_stream_T * bitstream,
  csn_utran_priority_params_ie_t * dst,
  uint8                          * utran_start,
  uint8                          * utran_stop
)
{
  uint8 i = 0;

  RR_NULL_CHECK_RETURN_VOID(bitstream);
  RR_NULL_CHECK_RETURN_VOID(dst);
  RR_NULL_CHECK_RETURN_VOID(utran_start);
  RR_NULL_CHECK_RETURN_VOID(utran_stop);

  memset(dst, 0, sizeof(csn_utran_priority_params_ie_t));

  *utran_start = (uint8) gs_bit_stream_extract_field(bitstream, 1);
  *utran_stop = (uint8) gs_bit_stream_extract_field(bitstream, 1);

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->default_params_valid = TRUE;
    dst->default_params.priority = (uint8) gs_bit_stream_extract_field(bitstream, 3);
    dst->default_params.thresh = (uint8) gs_bit_stream_extract_field(bitstream, 5);
    dst->default_params.qrxlevmin = (uint8) gs_bit_stream_extract_field(bitstream, 5);
  }
  else
  {
    dst->default_params_valid = FALSE;
  }

  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    /* Decode in to a temporary variable, then copy to our destination store
    if there is sufficient space left. */
    csn_utran_priority_params_t utran_priority_params;

    rr_si2quater_ro_utran_priority_params_decode(
      bitstream,
      &utran_priority_params
    );

    if (i < CSN_UTRAN_PRIORITY_PARAMS_DESC_MAX)
    {
      dst->utran_priority_params[i] = utran_priority_params;
      i++;
    }
  }

  dst->utran_priority_params_count = i;
}

/**
  @brief Decodes E-UTRAN Measurement Parameters received in SI2quater or
         Measurement Information messages.

  This function decodes the E-UTRAN Measurement Parameters:

  QSEARCH_C
  E-UTRAN_REP_QUANT
  E-UTRAN_MULTIRAT_REPORTING
  E-UTRAN_FDD_REPORTING_THRESHOLD
  E-UTRAN_FDD_REPORTING_THRESHOLD_2
  E-UTRAN_FDD_MEASUREMENT_REPORT_OFFSET
  E-UTRAN_FDD_REPORTING_OFFSET
  E-UTRAN_TDD_REPORTING_THRESHOLD
  E-UTRAN_TDD_REPORTING_THRESHOLD_2
  E-UTRAN_TDD_MEASUREMENT_REPORT_OFFSET
  E-UTRAN_TDD_REPORTING_OFFSET

  Any optional parameters will take their default values (which in all cases is
  zero, so no explicit setting other than initial zeroing of the structure is
  required) if not present in the encoded message.

  @param bitstream Bitstream that contains the source message.
  @param dst Pointer to the structure in which to store the decoded structure.
*/
static void rr_decode_eutran_meas_params(
  gs_bit_stream_T         * bitstream,
  rr_eutran_meas_params_t * dst
)
{
  rr_eutran_meas_params_t eutran_meas_params;
  boolean                 fdd_valid = FALSE;
  boolean                 tdd_valid = FALSE;

  RR_NULL_CHECK_RETURN_VOID(bitstream);

  memset(&eutran_meas_params, 0, sizeof(rr_eutran_meas_params_t));

  eutran_meas_params.qsearch_c = (uint8) gs_bit_stream_extract_field(bitstream, 4);
  eutran_meas_params.rep_quant = (uint8) gs_bit_stream_extract_field(bitstream, 1);
  eutran_meas_params.multirat_reporting = (uint8) gs_bit_stream_extract_field(bitstream, 2);

  if (gs_bit_stream_extract_field(bitstream, 1) == 0)
  {
    /* 6-bit reporting, so no E-UTRAN_FDD_MEASUREMENT_REPORT_OFFSET or
    E-UTRAN_TDD_MEASUREMENT_REPORT_OFFSET provided. */

    /* -- FDD 3 bit reporting parameters */
    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      fdd_valid = TRUE;

      /* -- FDD 6 bit reporting */
      eutran_meas_params.fdd.reporting_threshold = (uint8) gs_bit_stream_extract_field(bitstream, 3);
      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        eutran_meas_params.fdd.reporting_threshold_2 = (uint8) gs_bit_stream_extract_field(bitstream, 6);
      }
      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        eutran_meas_params.fdd.reporting_offset = (uint8) gs_bit_stream_extract_field(bitstream, 3);
      }
    }

    /* -- TDD 6 bit reporting parameters */
    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      tdd_valid = TRUE;

      /* -- TDD 6 bit reporting */
      eutran_meas_params.tdd.reporting_threshold = (uint8) gs_bit_stream_extract_field(bitstream, 3);
      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        eutran_meas_params.tdd.reporting_threshold_2 = (uint8) gs_bit_stream_extract_field(bitstream, 6);
      }
      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        eutran_meas_params.tdd.reporting_offset = (uint8) gs_bit_stream_extract_field(bitstream, 3);
      }
    }
  }
  else
  {
    /* 3-bit reporting parameters present, so no
    E-UTRAN_FDD_REPORTING_THRESHOLD or E-UTRAN_TDD_REPORTING_THRESHOLD
    provided. */

    /* -- FDD 3 bit reporting parameters */
    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      fdd_valid = TRUE;

      eutran_meas_params.fdd.measurement_report_offset = (uint8) gs_bit_stream_extract_field(bitstream, 6);
      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        eutran_meas_params.fdd.reporting_threshold_2 = (uint8) gs_bit_stream_extract_field(bitstream, 6);
      }
      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        eutran_meas_params.fdd.reporting_offset = (uint8) gs_bit_stream_extract_field(bitstream, 3);
      }
    }

    /* -- TDD 3 bit reporting parameters */
    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      tdd_valid = TRUE;

      eutran_meas_params.tdd.measurement_report_offset = (uint8) gs_bit_stream_extract_field(bitstream, 6);
      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        eutran_meas_params.tdd.reporting_threshold_2 = (uint8) gs_bit_stream_extract_field(bitstream, 6);
      }
      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        eutran_meas_params.tdd.reporting_offset = (uint8) gs_bit_stream_extract_field(bitstream, 3);
      }
    }

    eutran_meas_params.reporting_granularity = (uint8) gs_bit_stream_extract_field(bitstream, 1);
  }

  /* 3GPP 44.018 Section 9.1.54 "MEASUREMENT INFORMATION" states:

  "If both TDD and FDD frequencies are provided in the Repeated E-UTRAN
  Neighbour Cells IE and the E-UTRAN reporting thresholds are present for only
  one mode (i.e. TDD or FDD), then the parameter values for both modes shall
  be interpreted as having the same values." */
  if (fdd_valid && !tdd_valid)
  {
    eutran_meas_params.tdd = eutran_meas_params.fdd;
  }
  else if (tdd_valid && !fdd_valid)
  {
    eutran_meas_params.fdd = eutran_meas_params.tdd;
  }

  if (dst != NULL)
  {
    *dst = eutran_meas_params;
  }
}


/**
  @brief Decodes GPRS E-UTRAN Measurement Parameters received in SI2quater.

  This function decodes the GPRS E-UTRAN Measurement Parameters:

  QSEARCH_P
  E-UTRAN_REP_QUANT
  E-UTRAN_MULTIRAT_REPORTING
  E-UTRAN_FDD_REPORTING_THRESHOLD
  E-UTRAN_FDD_REPORTING_THRESHOLD_2
  E-UTRAN_FDD_REPORTING_OFFSET
  E-UTRAN_TDD_REPORTING_THRESHOLD
  E-UTRAN_TDD_REPORTING_THRESHOLD_2
  E-UTRAN_TDD_REPORTING_OFFSET

  Any optional parameters will take their default values (which in all cases is
  zero, so no explicit setting other than initial zeroing of the structure is
  required) if not present in the encoded message.

  @param bitstream Bitstream that contains the source message.
  @param dst Pointer to the structure in which to store the decoded structure.
*/
static void rr_decode_gprs_eutran_meas_params(
  gs_bit_stream_T              * bitstream,
  rr_gprs_eutran_meas_params_t * dst
)
{
  rr_gprs_eutran_meas_params_t gprs_eutran_meas_params;
  boolean                      fdd_valid = FALSE;
  boolean                      tdd_valid = FALSE;

  RR_NULL_CHECK_RETURN_VOID(bitstream);

  memset(&gprs_eutran_meas_params, 0, sizeof(gprs_eutran_meas_params));

  gprs_eutran_meas_params.qsearch_p = (uint8) gs_bit_stream_extract_field(bitstream, 4);
  gprs_eutran_meas_params.rep_quant = (uint8) gs_bit_stream_extract_field(bitstream, 1);
  gprs_eutran_meas_params.multirat_reporting = (uint8) gs_bit_stream_extract_field(bitstream, 2);

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    fdd_valid = TRUE;

    gprs_eutran_meas_params.fdd.reporting_threshold = (uint8) gs_bit_stream_extract_field(bitstream, 3);

    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      gprs_eutran_meas_params.fdd.reporting_threshold_2 = (uint8) gs_bit_stream_extract_field(bitstream, 6);
    }
    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      gprs_eutran_meas_params.fdd.reporting_offset = (uint8) gs_bit_stream_extract_field(bitstream, 3);
    }
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    tdd_valid = TRUE;

    gprs_eutran_meas_params.tdd.reporting_threshold = (uint8) gs_bit_stream_extract_field(bitstream, 3);
    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      gprs_eutran_meas_params.tdd.reporting_threshold_2 = (uint8) gs_bit_stream_extract_field(bitstream, 6);
    }
    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      gprs_eutran_meas_params.tdd.reporting_offset = (uint8) gs_bit_stream_extract_field(bitstream, 3);
    }
  }

  /* 3GPP 44.018 Section 10.5.2.33b "SI 2quater Rest Octets" states:

  "If both TDD and FDD frequencies are provided in the Repeated E-UTRAN
  Neighbour Cells IE and the E-UTRAN reporting thresholds are present for only
  one mode (i.e. TDD or FDD), then the parameter values for both modes shall
  be interpreted as having the same values." */
  if (fdd_valid && !tdd_valid)
  {
    gprs_eutran_meas_params.tdd = gprs_eutran_meas_params.fdd;
  }
  else if (tdd_valid && !fdd_valid)
  {
    gprs_eutran_meas_params.fdd = gprs_eutran_meas_params.tdd;
  }

  if (dst != NULL)
  {
    *dst = gprs_eutran_meas_params;
  }
}

static void rr_si2quater_ro_eutran_neighbour_cells_dump(
  rr_si2quater_ro_eutran_neighbour_cells_t * src
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(src);

  for (i = 0;
       i < src->eutran_frequency_list_count &&
       i < MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS;
       ++i)
  {
    MSG_GERAN_MED_2("  LTE Frequency: %d, Bandwidth: %d",
            src->eutran_frequency_list[i].earfcn,
            src->eutran_frequency_list[i].measurement_bandwidth);
  }

  if ((src->eutran_priority_valid)||(src->thresh_eutran_low_valid)||(src->eutran_qrxlevmin_valid))
  {
    
    MSG_GERAN_MED_4("LTE Priority: %d ,Thresh High: %d Thresh Low: %d, Qrxlevmin: %d ", 
                     src->eutran_priority,
                     src->thresh_eutran_high,
                     src->thresh_eutran_low,
                     src->eutran_qrxlevmin);
  }
  else
  {
    MSG_GERAN_MED_4("LTE Priority Valid: %d ,Thresh High: %d Thresh Low Valid: %d, Qrxlevmin valid: %d ", 
                     src->eutran_priority_valid,
                     src->thresh_eutran_high,
                     src->thresh_eutran_low_valid,
                     src->eutran_qrxlevmin_valid);  
  }
  
}

static void rr_si2quater_ro_eutran_neighbour_cells_decode(
  gs_bit_stream_T * bitstream,
  rr_si2quater_ro_eutran_neighbour_cells_t * dst
)
{
  uint8 i;

  RR_NULL_CHECK_FATAL(bitstream);
  RR_NULL_CHECK_FATAL(dst);

  i = 0;
  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    rr_lte_frequency_t eutran_frequency;
    eutran_frequency.earfcn = (lte_earfcn_t) gs_bit_stream_extract_field(bitstream, 16);
    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      eutran_frequency.measurement_bandwidth = (uint8) gs_bit_stream_extract_field(bitstream, 3);
    }
    else
    {
      eutran_frequency.measurement_bandwidth = RR_DEFAULT_LTE_MEASUREMENT_BANDWIDTH;
    }

    if (i < MAX_EUTRAN_NEIGHBOUR_CELLS_EARFCNS)
    {
      dst->eutran_frequency_list[i] = eutran_frequency;
      i++;
    }
  }
  dst->eutran_frequency_list_count = i;

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->eutran_priority_valid = TRUE;
    dst->eutran_priority = (uint8) gs_bit_stream_extract_field(bitstream, 3);
  }
  else
  {
    dst->eutran_priority_valid = FALSE;
  }

  dst->thresh_eutran_high = (uint8) gs_bit_stream_extract_field(bitstream, 5);

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->thresh_eutran_low_valid = TRUE;
    dst->thresh_eutran_low = (uint8) gs_bit_stream_extract_field(bitstream, 5);
  }
  else
  {
    dst->thresh_eutran_low_valid = FALSE;
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->eutran_qrxlevmin_valid = TRUE;
    dst->eutran_qrxlevmin = (uint8) gs_bit_stream_extract_field(bitstream, 5);
  }
  else
  {
    dst->eutran_qrxlevmin_valid = FALSE;
  }

  rr_si2quater_ro_eutran_neighbour_cells_dump(dst);
}

static void rr_si2quater_ro_eutran_not_allowed_cells_decode(
  gs_bit_stream_T * bitstream,
  rr_si2quater_ro_eutran_not_allowed_cells_t * dst
)
{
  uint8 i;

  RR_NULL_CHECK_FATAL(bitstream);
  RR_NULL_CHECK_FATAL(dst);

  rr_pcid_group_ie_decode(
    bitstream,
    &dst->not_allowed_cells
  );

  i = 0;
  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    uint8 eutran_frequency_index = (uint8) gs_bit_stream_extract_field(bitstream, 3);

    if (i < MAX_EUTRAN_NOT_ALLOWED_CELLS_EUTRAN_FREQUENCY_INDICES)
    {
      dst->eutran_frequency_indices[i] = eutran_frequency_index;
      i++;
    }
  }
  dst->eutran_frequency_indices_count = i;
}

/**
  @brief Decodes E-UTRAN PCID to TA mapping struct from SI2quater Rest Octets.
*/
static void rr_si2quater_ro_eutran_pcid_to_ta_mapping_decode(
  gs_bit_stream_T * bitstream,
  rr_si2quater_ro_eutran_pcid_to_ta_mapping_t * dst
)
{
  uint8 i;

  RR_NULL_CHECK_FATAL(bitstream);
  RR_NULL_CHECK_FATAL(dst);

  i = 0;
  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    rr_pcid_group_ie_t pcid_to_ta_mapping_group = {{0}};

    rr_pcid_group_ie_decode(
      bitstream,
      &pcid_to_ta_mapping_group
    );

    if (i < MAX_PCID_TO_TA_MAPPING_PCID_TO_TA_MAPPING_GROUPS)
    {
      dst->pcid_to_ta_mapping.pcid_to_ta_mapping_group[i] = pcid_to_ta_mapping_group;
      i++;
    }
  }
  dst->pcid_to_ta_mapping.pcid_to_ta_mapping_group_count = i;

  i = 0;
  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    uint8 eutran_frequency_index = (uint8) gs_bit_stream_extract_field(bitstream, 3);

    if (i < MAX_EUTRAN_PCID_TO_TA_MAPPING_EUTRAN_FREQUENCY_INDICES)
    {
      dst->eutran_frequency_indices[i] = eutran_frequency_index;
      i++;
    }
  }
  dst->eutran_frequency_indices_count = i;
}

/**
  @brief Decodes E-UTRAN Parameters Description struct.
*/
static void rr_si2quater_ro_eutran_params_desc_decode(
  gs_bit_stream_T * bitstream,
  rr_si2quater_ro_eutran_params_desc_t * dst
)
{
  uint8 i;
  boolean eutran_params = FALSE;
  boolean geutran_params = FALSE;

  RR_NULL_CHECK_FATAL(bitstream);
  RR_NULL_CHECK_FATAL(dst);

  memset(dst, 0, sizeof(rr_si2quater_ro_eutran_params_desc_t));

  dst->eutran_ccn_active =        (uint8) gs_bit_stream_extract_field(bitstream, 1);
  dst->eutran_start =             (uint8) gs_bit_stream_extract_field(bitstream, 1);
  dst->eutran_stop =              (uint8) gs_bit_stream_extract_field(bitstream, 1);

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    eutran_params= TRUE;
    dst->eutran_meas_params = GPRS_MEM_MALLOC(sizeof(rr_eutran_meas_params_t));
    RR_ASSERT_FATAL(dst->eutran_meas_params != NULL);

    rr_decode_eutran_meas_params(
      bitstream,
      dst->eutran_meas_params
    );
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    geutran_params= TRUE;
    dst->gprs_eutran_meas_params = GPRS_MEM_MALLOC(sizeof(rr_gprs_eutran_meas_params_t));
    RR_ASSERT_FATAL(dst->gprs_eutran_meas_params != NULL);

    rr_decode_gprs_eutran_meas_params(
      bitstream,
      dst->gprs_eutran_meas_params
    );
  }
  if ((eutran_params) || (geutran_params))
  {
    MSG_GERAN_MED_2("E-UTRAN present = %d and GPRS E-UTRAN present =%d Measurement Parameters Description ",eutran_params, geutran_params);
  } 
  /* Repeated E-UTRAN Neighbour Cells. */
  i = 0;
  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    rr_si2quater_ro_eutran_neighbour_cells_t * eutran_neighbour_cells = GPRS_MEM_MALLOC(sizeof(rr_si2quater_ro_eutran_neighbour_cells_t));
    RR_ASSERT_FATAL(eutran_neighbour_cells != NULL);
    memset(eutran_neighbour_cells, 0, sizeof(rr_si2quater_ro_eutran_neighbour_cells_t));

    MSG_GERAN_MED_0("Repeated E-UTRAN Neighbour Cells present");

    rr_si2quater_ro_eutran_neighbour_cells_decode(
      bitstream,
      eutran_neighbour_cells
    );

    if (i < MAX_REPEATED_EUTRAN_NEIGHBOUR_CELLS)
    {
      dst->eutran_neighbour_cells[i] = eutran_neighbour_cells;
      i++;
    }
  }
  dst->eutran_neighbour_cells_count = i;

  if (i == 0)
  {
    MSG_GERAN_MED_0("Repeated E-UTRAN Neighbour Cells not present");
  }

  i = 0;
  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    rr_si2quater_ro_eutran_not_allowed_cells_t * eutran_not_allowed_cells = GPRS_MEM_MALLOC(sizeof(rr_si2quater_ro_eutran_not_allowed_cells_t));
    RR_ASSERT_FATAL(eutran_not_allowed_cells != NULL);
    memset(eutran_not_allowed_cells, 0, sizeof(rr_si2quater_ro_eutran_not_allowed_cells_t));

    rr_si2quater_ro_eutran_not_allowed_cells_decode(
      bitstream,
      eutran_not_allowed_cells
    );

    if (i < MAX_REPEATED_EUTRAN_NEIGHBOUR_CELLS)
    {
      dst->eutran_not_allowed_cells[i] = eutran_not_allowed_cells;
      i++;
    }
  }
  dst->eutran_not_allowed_cells_count = i;

  i = 0;
  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    rr_si2quater_ro_eutran_pcid_to_ta_mapping_t * eutran_pcid_to_ta_mapping = GPRS_MEM_MALLOC(sizeof(rr_si2quater_ro_eutran_pcid_to_ta_mapping_t));
    RR_ASSERT_FATAL(eutran_pcid_to_ta_mapping != NULL);
    memset(eutran_pcid_to_ta_mapping, 0, sizeof(rr_si2quater_ro_eutran_pcid_to_ta_mapping_t));

    rr_si2quater_ro_eutran_pcid_to_ta_mapping_decode(
      bitstream,
      eutran_pcid_to_ta_mapping
    );

    if (i < MAX_REPEATED_EUTRAN_NEIGHBOUR_CELLS)
    {
      dst->eutran_pcid_to_ta_mapping[i] = eutran_pcid_to_ta_mapping;
      i++;
    }
  }
  dst->eutran_pcid_to_ta_mapping_count = i;
}

static void rr_si2quater_ro_priority_and_eutran_params_desc_decode(
  gs_bit_stream_T * bitstream,
  rr_si2quater_ro_priority_and_eutran_params_desc_t * dst
)
{
  RR_NULL_CHECK_FATAL(bitstream);
  RR_NULL_CHECK_FATAL(dst);

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->serving_cell_priority_params = GPRS_MEM_CALLOC(sizeof(csn_gsm_priority_params_ie_t));
    rr_si2quater_ro_serving_cell_priority_params_desc_decode(
      bitstream,
      dst->serving_cell_priority_params
    );
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->utran_priority_params = GPRS_MEM_CALLOC(sizeof(csn_utran_priority_params_ie_t));
    rr_si2quater_ro_utran_priority_params_desc_decode(
      bitstream,
      dst->utran_priority_params,
      &dst->utran_start,
      &dst->utran_stop
    );
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->eutran_params_desc = GPRS_MEM_MALLOC(sizeof(rr_si2quater_ro_eutran_params_desc_t));
    RR_ASSERT_FATAL(dst->eutran_params_desc != NULL);
    memset(dst->eutran_params_desc, 0, sizeof(rr_si2quater_ro_eutran_params_desc_t));

    rr_si2quater_ro_eutran_params_desc_decode(
      bitstream,
      dst->eutran_params_desc
    );
  }
}

static void rr_si2quater_ro_utran_csg_desc_decode(
  gs_bit_stream_T * bitstream,
  rr_si2quater_ro_utran_csg_desc_t * dst
)
{
  return;
}

static void rr_si2quater_ro_eutran_csg_desc_decode(
  gs_bit_stream_T * bitstream,
  rr_si2quater_ro_eutran_csg_desc_t * dst
)
{
  return;
}

static void rr_si2quater_ro_eutran_params_desc_free_children(
  rr_si2quater_ro_eutran_params_desc_t * item
)
{
  uint32 i;

  if (item->eutran_meas_params != NULL)
  {
    GPRS_MEM_FREE(item->eutran_meas_params);
    item->eutran_meas_params = NULL;
  }

  if (item->gprs_eutran_meas_params != NULL)
  {
    GPRS_MEM_FREE(item->gprs_eutran_meas_params);
    item->gprs_eutran_meas_params = NULL;
  }

  for (i = 0; i < item->eutran_neighbour_cells_count; i++)
  {
    GPRS_MEM_FREE(item->eutran_neighbour_cells[i]);
    item->eutran_neighbour_cells[i] = NULL;
  }

  for (i = 0; i < item->eutran_not_allowed_cells_count; i++)
  {
    GPRS_MEM_FREE(item->eutran_not_allowed_cells[i]);
    item->eutran_not_allowed_cells[i] = NULL;
  }

  for (i = 0; i < item->eutran_pcid_to_ta_mapping_count; i++)
  {
    GPRS_MEM_FREE(item->eutran_pcid_to_ta_mapping[i]);
    item->eutran_pcid_to_ta_mapping[i] = NULL;
  }
}

static void rr_si2quater_ro_priority_and_eutran_params_desc_free_children(
  rr_si2quater_ro_priority_and_eutran_params_desc_t * item
)
{

  if (item->serving_cell_priority_params != NULL)
  {
    GPRS_MEM_FREE(item->serving_cell_priority_params);
    item->serving_cell_priority_params = NULL;
  }

  if (item->utran_priority_params != NULL)
  {
    GPRS_MEM_FREE(item->utran_priority_params);
    item->utran_priority_params = NULL;
  }
 
  if (item->eutran_params_desc != NULL)
  {
    rr_si2quater_ro_eutran_params_desc_free_children(
      item->eutran_params_desc
    );

    GPRS_MEM_FREE(item->eutran_params_desc);
    item->eutran_params_desc = NULL;
  }
}

static void rr_si2quater_ro_utran_csg_desc_free_children(
  rr_si2quater_ro_utran_csg_desc_t * item
)
{
}

static void rr_si2quater_ro_eutran_csg_desc_free_children(
  rr_si2quater_ro_eutran_csg_desc_t * item
)
{
}

void rr_si2q_params_free_children(
  SI2q_params_T * item
)
{
#ifdef FEATURE_LTE
  if (item->gprs_bsic_description != NULL)
  {
    GPRS_MEM_FREE(item->gprs_bsic_description);
  }
#endif /* FEATURE_LTE */

  if (item->priority_and_eutran_params_desc != NULL)
  {
    rr_si2quater_ro_priority_and_eutran_params_desc_free_children(
      item->priority_and_eutran_params_desc
    );
    GPRS_MEM_FREE(item->priority_and_eutran_params_desc);
    item->priority_and_eutran_params_desc = NULL;
  }

  if (item->utran_csg_desc)
  {
    rr_si2quater_ro_utran_csg_desc_free_children(item->utran_csg_desc);
    GPRS_MEM_FREE(item->utran_csg_desc);
    item->utran_csg_desc = NULL;
  }

  if (item->eutran_csg_desc)
  {
    rr_si2quater_ro_eutran_csg_desc_free_children(item->eutran_csg_desc);
    GPRS_MEM_FREE(item->eutran_csg_desc);
    item->eutran_csg_desc = NULL;
  }
}


void rr_si2quater_ro_enh_cell_resel_par_des_decode(
  gs_bit_stream_T * bitstream,
  rr_si2quater_ro_enh_cell_resel_par_des_t  * dst
)
{
  uint8 i = 0;

  RR_NULL_CHECK_FATAL(bitstream);
  RR_NULL_CHECK_FATAL(dst);

  /*Repeated E-UTRAN Enhanced Cell Reselection Parameters*/
  while (gs_bit_stream_extract_field(bitstream, 1))
  {
      /*Repeated E-UTRAN Enhanced Cell Reselection Parameters struct*/
      while (gs_bit_stream_extract_field(bitstream, 1))
      {
        uint8 frequency_index = (uint8) gs_bit_stream_extract_field(bitstream, 3);
        if (i < MAX_EUTRAN_FREQUENCY_INDEX_COUNT)
        {
          dst->eutran_frequency_index[i] = frequency_index;
          i++;
        }
      }

    dst->eutran_frequency_index_count = i;

    dst->RSRP_valid = FALSE;
    dst->RSRQ_valid = FALSE;

    if (gs_bit_stream_extract_field(bitstream, 1) == 0)
    {
      //reselection based on target cell RSRP
      dst->RSRP_valid = TRUE;
      dst->eutran_qmin = (uint8) gs_bit_stream_extract_field(bitstream, 4);
    }
    else
    {
      //reselection based on target cell RSRQ
      dst->RSRQ_valid = TRUE;

      dst->thresh_eutran_high = (uint8) gs_bit_stream_extract_field(bitstream, 5);

      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        dst->thresh_eutran_low_q_valid = TRUE;
        dst->thresh_eutran_low_q = (uint8) gs_bit_stream_extract_field(bitstream, 5);
      }
      else
      {
        dst->thresh_eutran_low_q_valid = FALSE;
      }

      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        dst->eutran_qqualmin_valid = TRUE;
        dst->eutran_qqualmin = (uint8) gs_bit_stream_extract_field(bitstream, 4);
      }
      else
      {
        dst->eutran_qqualmin_valid  = FALSE;
      }


      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        dst->eutran_rsrpmin_valid = TRUE;
        dst->eutran_rsrpmin = (uint8) gs_bit_stream_extract_field(bitstream, 5);
      }
      else
      {
        dst->eutran_rsrpmin_valid  = FALSE;
      }
    }

  }

}


void rr_si2quater_ro_utran_csg_cell_reporting_des_decode(
  gs_bit_stream_T * bitstream,
  rr_si2quater_ro_utran_csg_cells_reporting_des_t  * dst
)
{
  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->utran_csg_fdd_reporting_threshold_valid = TRUE;
    dst->utran_csg_fdd_reporting_threshold =  (uint8) gs_bit_stream_extract_field(bitstream, 3);
    dst->utran_csg_fdd_reporting_threshold_2 = (uint8) gs_bit_stream_extract_field(bitstream, 6);
  }
  else
  {
    dst->utran_csg_fdd_reporting_threshold_valid = FALSE;
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->utran_csg_tdd_reporting_threshold_valid = TRUE;
    dst->utran_csg_tdd_reporting_threshold= (uint8) gs_bit_stream_extract_field(bitstream, 3);
  }
  else
  {
    dst->utran_csg_tdd_reporting_threshold_valid = FALSE;
  }
}

void rr_si2quater_ro_eutran_csg_cell_reporting_des_decode(
  gs_bit_stream_T * bitstream,
  rr_si2quater_ro_eutran_csg_cells_reporting_des_t  * dst
)
{
  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->eutran_csg_fdd_reporting_threshold_valid = TRUE;
    dst->eutran_csg_fdd_reporting_threshold = (uint8) gs_bit_stream_extract_field(bitstream, 3);
    dst->eutran_csg_fdd_reporting_threshold_2 = (uint8) gs_bit_stream_extract_field(bitstream, 6);
  }
  else
  {
    dst->eutran_csg_fdd_reporting_threshold_valid = FALSE;
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->eutran_csg_tdd_reporting_threshold_valid = TRUE;
    dst->eutran_csg_tdd_reporting_threshold = (uint8) gs_bit_stream_extract_field(bitstream, 3);
    dst->eutran_csg_tdd_reporting_threshold_2 = (uint8) gs_bit_stream_extract_field(bitstream, 6);
  }
  else
  {
    dst->eutran_csg_tdd_reporting_threshold_valid = FALSE;
  }

}



/**************************************************************************************
Function
rr_decode_si2quater_rest_octets

Description
This function extracts the bit stream information in the IE into
a temporary local storage. If the contents are valid, they are transferred
to the instances array

Parameters
[IN]
byte                  *IE:
Pointer to the bit stream containing the SI2quater
received from the network.

byte                  IE_length:
Length of the IE.

[OUT]
instances_T           *SI2quater:
Pointer to global storage for the instances.

boolean               *si2quater_received:
Set to TRUE if all instances have been received

boolean               *decoding_ok:
Set to TRUE if the decoding is successfull.

Returns
None

**********************************************************************************************/

void rr_decode_si2quater_rest_octets(
  byte        * IE,
  byte          IE_length,
  instances_T * SI2quater,
  boolean     * si2quater_received,
  boolean     * decoding_ok,
  ARFCN_T     * channel_number,
  system_information_database_T * cell_info_ptr,
  const gas_id_t  gas_id
)
{
  /* alias */
  SI2q_MI_params_T *common_params;
  SI2q_params_T    *si2q_params;

  /* temporary data structures */
  SI2q_MI_params_T temp_common_params;
  SI2q_params_T temp_si2q_params;
  /* data structure to store the encoded WCDMA neighbor cell information */
  encoded_FDD_info_T SI2quater_FDD_Encoded_Info[MAX_UTRAN_FREQ];
  encoded_TDD_info_T SI2quater_TDD_Encoded_Info[MAX_UTRAN_TDD_FREQ];


  /* Using bitstream routines to extract the bits from the rest octet */
  gs_bit_stream_T bitstream;
  gs_bit_stream_T *bit_stream_ptr;

  /* Asserts */
  RR_NULL_CHECK_FATAL (IE);
  RR_NULL_CHECK_FATAL (SI2quater);
  RR_NULL_CHECK_FATAL (si2quater_received);
  RR_NULL_CHECK_FATAL (decoding_ok);

  /* Initialize bit stream */
  gs_bit_stream_initialize(&bitstream,IE,IE_length);
  bit_stream_ptr = &bitstream;

  /* Initialize the temporary data structures */
  /* Initialize data structure to stored the encoded 3G neighbor cell information */
  rr_initialize_encoded_FDD_info (SI2quater_FDD_Encoded_Info, MAX_RR_MPH_FREQ_UPDATE_LIST);
  memset (&temp_common_params, 0, sizeof (SI2q_MI_params_T));
  memset (&temp_si2q_params, 0, sizeof(SI2q_params_T));
  memset (SI2quater_TDD_Encoded_Info, 0, sizeof (SI2quater_TDD_Encoded_Info));

  /* Initialize output values */
  *decoding_ok = TRUE;
  *si2quater_received = FALSE;

  /********************/
  /* Start Extracting */
  /*****************************************************************************************/
  /* Check if this instance is a new instance or an update to an already received instance */
  /*****************************************************************************************/
  if (rr_SI2q_MI_check_for_updates(
        bit_stream_ptr,
        &temp_common_params,
        SI2quater,
        si2quater_received) == FALSE)
  {
    /* No Change; This instance has been received and processed already; So Ingore */
    MSG_GERAN_HIGH_2("Repeat SI2quater instance %d",(int) temp_common_params.INDEX, 0 );
    MSG_GERAN_MED_2("Received SI2q(Index=%d, Count=%d)",(int) temp_common_params.INDEX, (int) temp_common_params.COUNT );
    return;
  }

  MSG_GERAN_HIGH_2("Received SI2q(Index=%d, Count=%d)",(int) temp_common_params.INDEX, (int) temp_common_params.COUNT );

  /*****************************************/
  /* Check if the instance number is valid */
  /*****************************************/
  if (temp_common_params.INDEX > temp_common_params.COUNT)
  {
    MSG_GERAN_HIGH_2("INDEX > COUNT in SI 2quater(%d > %d)",(int) temp_common_params.INDEX, (int) temp_common_params.COUNT );
    *decoding_ok = FALSE;
    return;
  }

  /* Extract the rest of the fields;
  Note: the following order of calling should be strictly maintained for proper extraction */
  rr_extract_SI2quater_Measurement_Parameters_Description(bit_stream_ptr, &temp_si2q_params);

  if (!bit_stream_ptr->overflow) rr_extract_SI2quater_GPRS_RTD_Description(bit_stream_ptr, &temp_si2q_params);
  if (!bit_stream_ptr->overflow) rr_extract_SI2quater_GPRS_BSIC_Description(bit_stream_ptr, &temp_si2q_params);
  if (!bit_stream_ptr->overflow) rr_extract_SI2quater_GPRS_Report_Priority_Description(bit_stream_ptr, &temp_si2q_params);
  if (!bit_stream_ptr->overflow) rr_extract_SI2quater_GPRS_Measurement_Parameters_Description(bit_stream_ptr, &temp_si2q_params);
  if (!bit_stream_ptr->overflow) rr_extract_SI2quater_NC_Measurement_Parameters_Description(bit_stream_ptr, &temp_si2q_params);
  if (!bit_stream_ptr->overflow) rr_extract_SI2quater_Extension_Information(bit_stream_ptr, &temp_si2q_params);

  if (!bit_stream_ptr->overflow)
  {
    rr_extract_SI2q_MI_3G_NC_Desc (
      bit_stream_ptr,
      &temp_common_params,
      &temp_si2q_params,
      NULL,
      SI2quater_FDD_Encoded_Info,
      decoding_ok,
      SI2quater_TDD_Encoded_Info
    );
  }

  if (*decoding_ok == FALSE)
  {
    MSG_GERAN_HIGH_2("rr_extract_SI2q_MI_3G_NC_Desc failed for SI2q(%d, %d)",(int) temp_common_params.INDEX, (int) temp_common_params.COUNT );
    return;
  }

  if (!bit_stream_ptr->overflow) rr_extract_SI2quater_3G_MP_Desc (bit_stream_ptr, &temp_si2q_params);
  if (!bit_stream_ptr->overflow) rr_extract_SI2quater_GPRS_3G_MEASUREMENT_PARAMETERS (bit_stream_ptr, &temp_si2q_params);
  if (!bit_stream_ptr->overflow)
  {
    /* { null  | L  --  Receiver compatible with earlier release
     *         | H  --  Additions in Release 5:
     */
    if (gs_bit_stream_extract_HL(bit_stream_ptr))
    {
      /* 0 | 1  < 3G Additional Measurement Parameters Description : */
      if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
      {
        if (!bit_stream_ptr->overflow)
        {
          temp_si2q_params.FDD_additional_MPs_PI = TRUE;
          temp_si2q_params.FDD_Qmin_offset = (byte) gs_bit_stream_extract_field(bit_stream_ptr, 3);
          temp_si2q_params.FDD_RSCPmin = (byte) gs_bit_stream_extract_field(bit_stream_ptr, 4);
        }

        if (bit_stream_ptr->overflow)
        {
          MSG_GERAN_ERROR_0("3G Additional Measurement Parameters indicated but message overflow detected!");
          temp_si2q_params.FDD_additional_MPs_PI = FALSE;
        }
      }

      /* 0 | 1  < 3G ADDITIONAL MEASUREMENT Parameters Description 2 : */
      if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
      {
        if (!bit_stream_ptr->overflow)
        {
          temp_si2q_params.FDD_additional_MPs_2_PI = TRUE;

          if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
          {
            temp_si2q_params.fdd_reporting_threshold_2_valid = TRUE;
            temp_si2q_params.FDD_REPORTING_THRESHOLD_2 = (byte) gs_bit_stream_extract_field(bit_stream_ptr, 6);
          }
          else
          {
            temp_si2q_params.fdd_reporting_threshold_2_valid = FALSE;
          }
        }

        /* Now check again for overflow - if overflow occurred during decoding
        of the 3G_ADDITIONAL_MEASUREMENT_PARAMETERS_DESCRIPTION_2 struct then
        flag the structure as not-present. */
        if (bit_stream_ptr->overflow)
        {
          MSG_GERAN_ERROR_0("3G Additional Measurement Parameters 2 indicated but message overflow detected!");
          temp_si2q_params.FDD_additional_MPs_2_PI = FALSE;
        }
      }

      if (!bit_stream_ptr->overflow)
      {
        /* { null  | L  -- Receiver compatible with earlier release
         *         | H  -- Additions in Release 6:
         */
        if (gs_bit_stream_extract_HL(bit_stream_ptr))
        {
          /* < 3G_CCN_ACTIVE : bit (1) > */
          if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
          {
            if (!bit_stream_ptr->overflow)
            {
              temp_si2q_params.three_g_ccn_active_valid = TRUE;
              temp_si2q_params.three_g_ccn_active       = 1;
            }
          }
        }

        if (!bit_stream_ptr->overflow)
        {
          /* null | L -- Receiver compatible with earlier release
                  | H -- Additions in Rel-7 */
          if (gs_bit_stream_extract_HL(bit_stream_ptr))
          {
            MSG_GERAN_MED_0("SI2quater Rel-7 Additions present");
            if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
            {
              temp_si2q_params.gsm_700_reporting_offset = (uint8) gs_bit_stream_extract_field(bit_stream_ptr, 3);
              temp_si2q_params.gsm_700_reporting_threshold = (uint8) gs_bit_stream_extract_field(bit_stream_ptr, 3);

              if (!bit_stream_ptr->overflow)
              {
                temp_si2q_params.gsm_700_reporting_params_valid = TRUE;
              }
            }
            else
            {
              temp_si2q_params.gsm_700_reporting_params_valid = FALSE;
            }

            if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
            {
              temp_si2q_params.gsm_810_reporting_offset = (uint8) gs_bit_stream_extract_field(bit_stream_ptr, 3);
              temp_si2q_params.gsm_810_reporting_threshold = (uint8) gs_bit_stream_extract_field(bit_stream_ptr, 3);

              if (!bit_stream_ptr->overflow)
              {
                temp_si2q_params.gsm_810_reporting_params_valid = TRUE;
              }
            }
            else
            {
              temp_si2q_params.gsm_810_reporting_params_valid = FALSE;
            }
          }

          if (bit_stream_ptr->overflow)
          {
            /* Information received in Rel-7 additions is inconsistent (truncated)
            so invalidate it all. */
            temp_si2q_params.gsm_700_reporting_params_valid = FALSE;
            temp_si2q_params.gsm_810_reporting_params_valid = FALSE;
          }

          if (!bit_stream_ptr->overflow)
          {
            /* null | L -- Receiver compatible with earlier release
                    | H -- Additions in Rel-8 */
            if (gs_bit_stream_extract_HL(bit_stream_ptr))
            {
              MSG_GERAN_MED_0("SI2quater Rel-8 Additions present");

              /* Decode Priority and E-UTRAN Parameters Description */
              if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
              {
                temp_si2q_params.priority_and_eutran_params_desc = GPRS_MEM_MALLOC(sizeof(rr_si2quater_ro_priority_and_eutran_params_desc_t));
                RR_ASSERT_FATAL(temp_si2q_params.priority_and_eutran_params_desc != NULL);
                memset(temp_si2q_params.priority_and_eutran_params_desc, 0, sizeof(rr_si2quater_ro_priority_and_eutran_params_desc_t));

                rr_si2quater_ro_priority_and_eutran_params_desc_decode(
                  bit_stream_ptr,
                  temp_si2q_params.priority_and_eutran_params_desc
                );
              }

              if (bit_stream_ptr->overflow)
              {
                /* Priority and E-UTRAN Parameters Description is inconsistent (truncated)
                so invalidate it. */
                if (temp_si2q_params.priority_and_eutran_params_desc != NULL)
                {
                  rr_si2quater_ro_priority_and_eutran_params_desc_free_children(
                    temp_si2q_params.priority_and_eutran_params_desc
                  );
                  GPRS_MEM_FREE(temp_si2q_params.priority_and_eutran_params_desc);
                  temp_si2q_params.priority_and_eutran_params_desc = NULL;
                }
                MSG_GERAN_ERROR_0("Error in network encoding of priority and eutran params, non-fatal");
              }

              /* Decode 3G CSG Description */
              if (bit_stream_ptr->overflow == FALSE &&
                  gs_bit_stream_extract_field(bit_stream_ptr, 1))
              {
                temp_si2q_params.utran_csg_desc = GPRS_MEM_MALLOC(sizeof(rr_si2quater_ro_utran_csg_desc_t));
                RR_ASSERT_FATAL(temp_si2q_params.utran_csg_desc != NULL);
                memset(temp_si2q_params.utran_csg_desc, 0, sizeof(rr_si2quater_ro_utran_csg_desc_t));

                rr_si2quater_ro_utran_csg_desc_decode(
                  bit_stream_ptr,
                  temp_si2q_params.utran_csg_desc
                );
              }

              if (bit_stream_ptr->overflow)
              {
                /* 3G CSG Description is inconsistent (truncated) so invalidate it. */
                
                if (temp_si2q_params.utran_csg_desc != NULL)
                {
                  rr_si2quater_ro_utran_csg_desc_free_children(temp_si2q_params.utran_csg_desc);
                  GPRS_MEM_FREE(temp_si2q_params.utran_csg_desc);
                  temp_si2q_params.utran_csg_desc = NULL;
                }
                MSG_GERAN_ERROR_0("Error in network encoding of utran csg, non-fatal");
              }

              /* Decode E-UTRAN CSG Description */
              if (bit_stream_ptr->overflow == FALSE &&
                  gs_bit_stream_extract_field(bit_stream_ptr, 1))
              {
                temp_si2q_params.eutran_csg_desc = GPRS_MEM_MALLOC(sizeof(rr_si2quater_ro_eutran_csg_desc_t));
                RR_ASSERT_FATAL(temp_si2q_params.eutran_csg_desc != NULL);
                memset(temp_si2q_params.eutran_csg_desc, 0, sizeof(rr_si2quater_ro_eutran_csg_desc_t));

                rr_si2quater_ro_eutran_csg_desc_decode(
                  bit_stream_ptr,
                  temp_si2q_params.eutran_csg_desc
                );
              }

              if (bit_stream_ptr->overflow)
              {
                /* E-UTRAN CSG Description is inconsistent (truncated) so invalidate it. */
                if (temp_si2q_params.eutran_csg_desc != NULL)
                {
                  rr_si2quater_ro_eutran_csg_desc_free_children(temp_si2q_params.eutran_csg_desc);
                  GPRS_MEM_FREE(temp_si2q_params.eutran_csg_desc);
                  temp_si2q_params.eutran_csg_desc = NULL;
                }
                MSG_GERAN_ERROR_0("Error in network encoding of eutran csg, non-fatal");
              }
            }
          }

          if (!bit_stream_ptr->overflow)
          {
            /* null | L -- Receiver compatible with earlier release
                    | H -- Additions in Rel-9 */
            if (gs_bit_stream_extract_HL(bit_stream_ptr))
            {
              MSG_MED("SI2quater Rel-9 Additions present", 0, 0, 0);

              /*Enhanced Cell Reselection Parameters IE described in 3GPP TS 44.060. */
              if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
              {
                rr_si2quater_ro_enh_cell_resel_par_des_decode(bit_stream_ptr,
                                                              &(temp_si2q_params.enh_cell_resel_par_des));

                if (!bit_stream_ptr->overflow)
                {
                  temp_si2q_params.enh_cell_resel_par_des_present = TRUE;
                }

              }
              else
              {
                temp_si2q_params.enh_cell_resel_par_des_present = FALSE;
              }

              /*CSG Cells Reporting Description struct */
              if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
              {

                if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
                {
                  rr_si2quater_ro_utran_csg_cell_reporting_des_decode(bit_stream_ptr,
                                                                       &(temp_si2q_params.csg_cell_reporting_des.utran_csg_cells_reporting_des));
                }
                else
                {
                  temp_si2q_params.csg_cell_reporting_des.utran_csg_cells_reporting_des_vaild = FALSE;
                }


                if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
                {
                  rr_si2quater_ro_eutran_csg_cell_reporting_des_decode(bit_stream_ptr,
                                                                       &(temp_si2q_params.csg_cell_reporting_des.eutran_csg_cells_reporting_des));
                }
                else
                {
                  temp_si2q_params.csg_cell_reporting_des.eutran_csg_cells_reporting_des_vaild = FALSE;
                }

                if (!bit_stream_ptr->overflow)
                {
                  temp_si2q_params.csg_cell_reporting_des_present = TRUE;
                }
              }
              else
              {
                temp_si2q_params.csg_cell_reporting_des_present = FALSE;
              }

              if (bit_stream_ptr->overflow)
              {
                /* Information received in Rel-9 additions is inconsistent (truncated)
                so invalidate it all. */
                temp_si2q_params.enh_cell_resel_par_des_present = FALSE;
                temp_si2q_params.csg_cell_reporting_des_present = FALSE;
                temp_si2q_params.csg_cell_reporting_des.utran_csg_cells_reporting_des_vaild = FALSE;
                temp_si2q_params.csg_cell_reporting_des.eutran_csg_cells_reporting_des_vaild = FALSE;
              }

              if (!bit_stream_ptr->overflow)
              {
                /* null | L -- Receiver compatible with earlier release
                        | H -- Additions in Rel-10 */
                if (gs_bit_stream_extract_HL(bit_stream_ptr))
                {
                  MSG_MED("SI2quater Rel-10 Additions present", 0, 0, 0);

                  if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
                  {
                    temp_si2q_params.int_pwr_red = (uint8) gs_bit_stream_extract_field(bit_stream_ptr, 1);
                    temp_si2q_params.int_pwr_red_present = TRUE;
                  }
                  else
                  {
                    temp_si2q_params.int_pwr_red_present = FALSE;
                  }

                  temp_si2q_params.nc2_csg_pccn_permitted = (uint8) gs_bit_stream_extract_field(bit_stream_ptr, 1);

                  MSG_MED("int_pwr_red_present %d int_pwr_red %d",
                           temp_si2q_params.int_pwr_red_present,temp_si2q_params.int_pwr_red,0);

                  if (bit_stream_ptr->overflow)
                  {
                    /* Information received in Rel-10 additions is inconsistent (truncated)
                    so invalidate it all. */
                    temp_si2q_params.int_pwr_red_present = FALSE;
                    temp_si2q_params.nc2_csg_pccn_permitted = FALSE;
                  }

                }
              } /*end of rel 10*/
            }
          }/*end of rel 9*/

        }
      }
    }
  }

  if (bit_stream_ptr->overflow)
  {
#ifdef PERLUTF
    #error code not present
#else
    MSG_GERAN_ERROR_0("Error in network encoding of SI2quater, non-fatal");
#endif
  }
  /************ Done extracting information from the IE into local storage ***************/

  /***************************************************************************************/
  /* Start processing the instance:
     1) Store changes to the 3G_BA_IND, MP_CHANGE_MARK
     2) Decode and store WCDMA FDD frequencies
     3) Transfer the contents in temporary storage into the instances.
     3) Determine if all instances have been received
  */
  /***************************************************************************************/


  /* Will be here only if either the 3G_BA_IND or the MP_CHANGE_MARK has changed */
  /* alias */
  common_params = &SI2quater->instance[temp_common_params.INDEX].common_params;
  si2q_params   = &SI2quater->instance[temp_common_params.INDEX].si2q_mi.si2q_params;


  /*******************************************/
  /* Check if there is a change in 3G_BA_IND */
  /*******************************************/
  if ( rr_3G_BA_changed(SI2quater, &temp_common_params) ||
       rr_MP_changed(SI2quater, &temp_common_params) )
  {
#ifdef FEATURE_NONBLIND_HANDOVER_DEBUG
    MSG_GERAN_HIGH_2("3G_BA_IND      old: <%d>, new: <%d> in SI2quater", \
      (int) common_params->BA_IND_3G, (int) temp_common_params.BA_IND_3G);
    MSG_GERAN_HIGH_2("MP_CHANGE_MARK old: <%d>, new: <%d> in SI2quater", \
      (int) common_params->MP_CHANGE_MARK, (int) temp_common_params.MP_CHANGE_MARK);
#endif
    /* Re-read all instances of SI2quater and rebuild the 3G Neighbor list */
    /* This will set valid_data =FALSE and received/changed to FALSE */
    rr_initialize_SI2q_MI_instances(SI2quater, SI2Q_INSTANCE);

    if (cell_info_ptr->cell_identity_valid_flag)
    {
      MSG_GERAN_HIGH_0("SI2Q MP changed flush SI cache");
      rr_csi_flush_data(&(cell_info_ptr->location_area_identification),
                        &(cell_info_ptr->cell_identity),
                        CSI_FLUSH_SI_CHANGE,
                        gas_id);
    }
  }

  if (!rr_SI2q_MI_instance_already_received(SI2quater, temp_common_params.INDEX))
  {
    rr_process_3g_BA(
      &SI2quater->instance[temp_common_params.INDEX],
      SI2quater_FDD_Encoded_Info,
      &temp_common_params,
      decoding_ok,
      SI2quater_TDD_Encoded_Info
    );

    if (*decoding_ok == FALSE)
    {
      /* reset this instance ? */
      return;
    }
  }

  /* copy information from the temporary structure that holds the MP and
   * common parameters to the global SI2quater instances
   */
  /* Make sure that we clear out the old si2q_params structure so any
  dynamically allocated memory associated with it is freed. */
  rr_si2q_params_free_children(si2q_params);

  *si2q_params = temp_si2q_params;
  *common_params = temp_common_params;

  /*********************************************************/
  /* Fill in other fields in the global SI2quater instance */
  /*********************************************************/
  /* Check if the instance was not received already; The instance could have been received
  already and the one that is just processed now might be an update in which case the num_instances
  is not incremented */
  if (rr_SI2q_MI_instance_already_received(SI2quater, temp_common_params.INDEX) == FALSE)
  {
    SI2quater->num_instances++;
    rr_update_status_SI2q_MI_instance (SI2quater, temp_common_params.INDEX, TRUE, TRUE);
  }
  else
  {
    /******************************************************************************************/
    /* Update the SI2quater array to indicate that this instance has been received            */
    /******************************************************************************************/
    rr_update_status_SI2q_MI_instance (SI2quater, temp_common_params.INDEX, TRUE, FALSE);
  }

  /*******************************************/
  /* Set the output parameters appropriately */
  /*******************************************/
  /* If all instances are received then the si2quater_received is set to TRUE; This is done
  in order to set the SI2_QUATER bit in the system information database or pending channel
  data */
  if (rr_received_all_instances(SI2quater,SI2quater->num_instances,temp_common_params.COUNT))
  {
#ifdef FEATURE_NONBLIND_HANDOVER_DEBUG
    MSG_GERAN_HIGH_0("Received all instances of SI2quater");
#endif
    *si2quater_received = TRUE;
  }
}

/**************************************************************************************
Function
rr_extract_MI_fields1

Description
Extract the normal Measurement Parameters Description into the temporary storage. All
GPRS related parameters, NC Measurement Parameters and the extension length are
extracted but not stored, since they are not used. They have to be extracted in order
to set the bit_stream_ptr to start extracting the 3G neighbor cell description.

Parameters
[IN]
gs_bit_stream_T *bit_stream_ptr:
Pointer to the bit stream containing the Measurement Information
received from the network.

[OUT]
MI_params_T *mi_params:
Pointer to the temporary storage into which the measurement parameters
are extracted into. Other parameters are ignored.

Returns
None

**********************************************************************************************/

static void rr_extract_MI_fields1(
  gs_bit_stream_T *bit_stream_ptr,
  MI_params_T *mi_params
)
{
  byte i;

  /* alias */
  BSIC_Desc_T      *bsic_desc = &mi_params->BSIC_Desc;
  MP_Desc_T        *mp_desc = &mi_params->MP_Desc;

  RR_NULL_CHECK_FATAL (bit_stream_ptr);
  RR_NULL_CHECK_FATAL (mi_params);

  /* extract subsequent fields */

  mi_params->PWRC = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

  mi_params->REPORT_TYPE = (report_type_T) gs_bit_stream_extract_field(bit_stream_ptr,1);

  mi_params->REPORTING_RATE = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

  mi_params->INVALID_BSIC_REPORTING = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

  if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1)) /* RTD_ Desc */
  {
    if (/* RTD6_PI */ (byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
      {
        (void) gs_bit_stream_extract_field(bit_stream_ptr,5);
      }

      while(!bit_stream_ptr->overflow)
      {
        if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
        {
          break;
        }

        /* discard the extracted value */
        (void) gs_bit_stream_extract_field(bit_stream_ptr,6);
      } /* end while */

      /* RTD6Struct could be present again */
      while(!bit_stream_ptr->overflow)
      {
        if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
        {
          break;
        }
        while(!bit_stream_ptr->overflow)
        {
          if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
          {
            break;
          }

          /* discard the extracted value */
          (void) gs_bit_stream_extract_field(bit_stream_ptr,6);
        } /* end while */
      } /* end while */
    } /* end RTD6_PI */

    /* Start reading RTD 12 */

    if (/* RTD12_PI */  (byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
      {
        /* discard the extracted value */
        (void) gs_bit_stream_extract_field(bit_stream_ptr,5);
      }


      while(!bit_stream_ptr->overflow)
      {
        if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
        {
          break;
        }

        /* discard the extracted value */
        (void) gs_bit_stream_extract_field(bit_stream_ptr,12);
      } /* end while */

      /* RTD6Struct could be present again */
      while(!bit_stream_ptr->overflow)
      {
        if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
        {
          break;
        }
        while(!bit_stream_ptr->overflow)
        {
          if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
          {
            break;
          }
          /* discard the extracted value */
          (void) gs_bit_stream_extract_field(bit_stream_ptr,12);
        } /* end while */
      } /* end while */
    } /* end RTD12_PI */
  } /* RTD_ Desc */

  mi_params->BSIC_Desc_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

  if (mi_params->BSIC_Desc_PI)
  {
    bsic_desc->BA_Index_Start_BSIC_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

    if (bsic_desc->BA_Index_Start_BSIC_PI)
    {
      bsic_desc->BA_Index_Start_BSIC = (byte) gs_bit_stream_extract_field(bit_stream_ptr,5);
    }
    bsic_desc->BSIC = (byte) gs_bit_stream_extract_field(bit_stream_ptr,6);
    bsic_desc->Number_Remaining_BSIC = (byte) gs_bit_stream_extract_field(bit_stream_ptr,7);

    for (i = 0; i < bsic_desc->Number_Remaining_BSIC; ++i)
    {
      bsic_desc->mi_bsic[i].Frequency_Scrolling = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
      bsic_desc->mi_bsic[i].BSIC = (byte) gs_bit_stream_extract_field(bit_stream_ptr,6);
    }
  }

  mi_params->REPORT_PRIORITY_Desc_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

  if (mi_params->REPORT_PRIORITY_Desc_PI)
  {
    byte Number_Cells = (byte) gs_bit_stream_extract_field(bit_stream_ptr,7);

    byte num_parts = Number_Cells / 32;
    byte last_part = Number_Cells % 32;

    for (i = 0; i < num_parts; ++i)
    {
      mi_params->REP_PRIORITY[i] = (byte) gs_bit_stream_extract_field(bit_stream_ptr, 32);
    }
    mi_params->REP_PRIORITY[i] = (byte) gs_bit_stream_extract_field(bit_stream_ptr, last_part);
  }

  mi_params->MP_Desc_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
  if (mi_params->MP_Desc_PI)
  {
    mp_desc->MULTIBAND_REPORTING_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (mp_desc->MULTIBAND_REPORTING_PI)
    {
      mp_desc->MULTIBAND_REPORTING = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);
    }

    mp_desc->SERVING_BAND_REPORTING_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (mp_desc->SERVING_BAND_REPORTING_PI)
    {
      mp_desc->SERVING_BAND_REPORTING = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);
    }

    mp_desc->SCALE_ORD = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);

    mp_desc->REPORTING_900_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (mp_desc->REPORTING_900_PI)
    {
      mp_desc->REPORTING_OFFSET_900 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      mp_desc->REPORTING_THRESHOLD_900 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    mp_desc->REPORTING_1800_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (mp_desc->REPORTING_1800_PI)
    {
      mp_desc->REPORTING_OFFSET_1800 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      mp_desc->REPORTING_THRESHOLD_1800 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    mp_desc->REPORTING_400_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (mp_desc->REPORTING_400_PI)
    {
      mp_desc->REPORTING_OFFSET_400 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      mp_desc->REPORTING_THRESHOLD_400 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    mp_desc->REPORTING_1900_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (mp_desc->REPORTING_1900_PI)
    {
      mp_desc->REPORTING_OFFSET_1900 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      mp_desc->REPORTING_THRESHOLD_1900 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    mp_desc->REPORTING_850_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (mp_desc->REPORTING_850_PI)
    {
      mp_desc->REPORTING_OFFSET_850 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      mp_desc->REPORTING_THRESHOLD_850 = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }
  }

  if (gs_bit_stream_extract_field(bit_stream_ptr,1))
  {
    byte extension_length = (byte) gs_bit_stream_extract_field(bit_stream_ptr,8);
    /* skip */ (void) gs_bit_stream_extract_field(bit_stream_ptr, extension_length+1);
  }
}

/**************************************************************************************
Function
rr_extract_MI_3G_MP_Desc

Description
Extract the 3G Measurement Parameters Description into the temporary storage.

Parameters
[IN]
gs_bit_stream_T *bit_stream_ptr:
Pointer to the bit stream containing the Measurement Information
received from the network.

[OUT]
MI_params_T *mi_params
Pointer to the temporary storage into which the 3G measurement parameters
are extracted into.

Returns
None

**********************************************************************************************/
static void rr_extract_MI_3G_MP_Desc(
  gs_bit_stream_T *bit_stream_ptr,
  MI_params_T *mi_params
)
{
  RR_NULL_CHECK_FATAL (bit_stream_ptr);
  RR_NULL_CHECK_FATAL (mi_params);

  mi_params->MI_3G_MP_Desc_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

  if (mi_params->MI_3G_MP_Desc_PI)
  {
    mi_params->QSearch_C = (byte) gs_bit_stream_extract_field(bit_stream_ptr,4);
 
    (void) gs_bit_stream_extract_field(bit_stream_ptr, 1); /* spare bit */
    mi_params->FDD_REP_QUANT = (fdd_rep_quant_T) gs_bit_stream_extract_field(bit_stream_ptr,1);

    mi_params->FDD_MULTIRAT_REPORTING_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (mi_params->FDD_MULTIRAT_REPORTING_PI)
    {
      mi_params->FDD_MULTIRAT_REPORTING = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);
    }

    mi_params->FDD_REP_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);

    if (mi_params->FDD_REP_PI)
    {
      mi_params->FDD_REP_OFFSET = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      mi_params->FDD_REP_THRESHOLD = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    mi_params->TDD_MULTIRAT_REPORTING_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (mi_params->TDD_MULTIRAT_REPORTING_PI)
    {
      mi_params->TDD_MULTIRAT_REPORTING = (byte) gs_bit_stream_extract_field(bit_stream_ptr,2);
    }

    mi_params->TDD_REP_PI = (byte) gs_bit_stream_extract_field(bit_stream_ptr,1);
    if (mi_params->TDD_REP_PI)
    {
      mi_params->TDD_REP_OFFSET = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
      mi_params->TDD_REP_THRESHOLD = (byte) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      /* CDMA2000_MULTIRAT_REPORTING = */(void) gs_bit_stream_extract_field(bit_stream_ptr,2);
    }

    if ((byte) gs_bit_stream_extract_field(bit_stream_ptr,1))
    {
      /* CDMA2000_REPORTING_OFFSET = */(void) gs_bit_stream_extract_field(bit_stream_ptr,3);
      /* CDMA2000_REPORTING_THRESHOLD = */(void) gs_bit_stream_extract_field(bit_stream_ptr,3);
    }
  } /* end WCDMA_3G_MP_Desc_PI */
}


/**
 * Extracts the 3G Additional Measurements Parameters Description 2 structure
 * from a Measurement Information message.
 */
static void rr_extract_3g_addl_meas_params_desc_2(gs_bit_stream_T *bit_stream_ptr,
                                                  MI_params_T *mi_params)
{
  if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
  {
    mi_params->fdd_reporting_threshold_2_valid = TRUE;
    mi_params->fdd_reporting_threshold_2 = (uint8) gs_bit_stream_extract_field(bit_stream_ptr, 6);
  }
  else
  {
    mi_params->fdd_reporting_threshold_2_valid = FALSE;
  }
}
 
static void rr_decode_mi_utran_suppl_params(
  gs_bit_stream_T         * bitstream,
  rr_mi_utran_suppl_params_t * dst
)
{
  rr_mi_utran_suppl_params_t utran_suppl_params;

  RR_NULL_CHECK_RETURN_VOID(bitstream);

  memset(&utran_suppl_params, 0, sizeof(rr_mi_utran_suppl_params_t));

  utran_suppl_params.utran_start = (boolean) gs_bit_stream_extract_field(bitstream, 1);
  utran_suppl_params.utran_stop = (boolean) gs_bit_stream_extract_field(bitstream, 1);

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    utran_suppl_params.utran_meas_control_params_valid = TRUE;

    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      utran_suppl_params.utran_meas_control_params.default_meas_control_valid = TRUE;
      utran_suppl_params.utran_meas_control_params.default_meas_control = (uint8) gs_bit_stream_extract_field(bitstream, 1);
    }

    /* Repeated UTRAN Measurement Control Parameters */
    while (gs_bit_stream_extract_field(bitstream, 1))
    {
      /* Each Repeated UTRAN Measurement Control Parameters structure
      providies a Measurement_Control_UTRAN value for between 0 and 31 UTRAN
      Frequencies, references by their index in the UTRAN Frequency List. */
      uint32 validity_bitmap = 0;

      while (gs_bit_stream_extract_field(bitstream, 1))
      {
        uint32 utran_frequency_index = gs_bit_stream_extract_field(bitstream, 5);

        if (utran_frequency_index < 32)
        {
          validity_bitmap |= (1 << utran_frequency_index);
        }
        else
        {
          MSG_GERAN_ERROR_1("Invalid utran_frequency_index: %d", utran_frequency_index);
        }
      }

      if (gs_bit_stream_extract_field(bitstream, 1))
      {
        SET_BIT(validity_bitmap, utran_suppl_params.utran_meas_control_params.validity_bitmap);
        SET_BIT(validity_bitmap, utran_suppl_params.utran_meas_control_params.value_bitmap);
      }
      else
      {
        SET_BIT(validity_bitmap, utran_suppl_params.utran_meas_control_params.validity_bitmap);
        CLEAR_BIT(validity_bitmap, utran_suppl_params.utran_meas_control_params.value_bitmap);
      }
    }
  }

  if (dst != NULL)
  {
    *dst = utran_suppl_params;
  }
}

static void rr_decode_mi_eutran_meas_control_params(
  gs_bit_stream_T                 * bitstream,
  rr_eutran_meas_control_params_t * dst
)
{
  rr_eutran_meas_control_params_t meas_control_params;

  RR_NULL_CHECK_RETURN_VOID(bitstream);

  memset(&meas_control_params, 0, sizeof(rr_eutran_meas_control_params_t));

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    meas_control_params.default_meas_control_valid = TRUE;
    meas_control_params.default_meas_control = (uint8) gs_bit_stream_extract_field(bitstream, 1);
  }

  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    uint8 validity_bitmap = 0;

    while (gs_bit_stream_extract_field(bitstream, 1))
    {
      uint32 eutran_frequency_index = gs_bit_stream_extract_field(bitstream, 3);

      if (eutran_frequency_index < 8)
      {
        validity_bitmap |= (1 << eutran_frequency_index);
      }
      else
      {
        MSG_GERAN_ERROR_1("Invalid eutran_frequency_index: %d", eutran_frequency_index);
      }
    }

    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      SET_BIT(validity_bitmap, meas_control_params.validity_bitmap);
      SET_BIT(validity_bitmap, meas_control_params.value_bitmap);
    }
    else
    {
      SET_BIT(validity_bitmap, meas_control_params.validity_bitmap);
      CLEAR_BIT(validity_bitmap, meas_control_params.value_bitmap);
    }
  }

  if (dst != NULL)
  {
    *dst = meas_control_params;
  }
}

static void rr_decode_mi_eutran_params(
  gs_bit_stream_T       * bitstream,
  rr_mi_eutran_params_t * dst
)
{
  rr_mi_eutran_params_t eutran_params;

  RR_NULL_CHECK_RETURN_VOID(bitstream);

  memset(&eutran_params, 0, sizeof(rr_mi_eutran_params_t));

  eutran_params.eutran_start = (boolean) gs_bit_stream_extract_field(bitstream, 1);
  eutran_params.eutran_stop = (boolean) gs_bit_stream_extract_field(bitstream, 1);

  /* E-UTRAN Measurement Parameters Description */
  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    eutran_params.meas_params_valid = TRUE;
    rr_decode_eutran_meas_params(bitstream, &eutran_params.meas_params);
  }

  /* Repeated E-UTRAN Neighbour Cells */
  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    uint32 earfcn_count = 0;
    uint16 earfcns[MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE];
    uint8  measurement_bandwidth = 0;
    uint32 i;

    earfcns[earfcn_count++] = (uint16) gs_bit_stream_extract_field(bitstream, 16);

    while (gs_bit_stream_extract_field(bitstream, 1))
    {
      uint16 earfcn = (uint16) gs_bit_stream_extract_field(bitstream, 16);

      if (earfcn_count < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE)
      {
        earfcns[earfcn_count++] = earfcn;
      }
    }

    if (gs_bit_stream_extract_field(bitstream, 1))
    {
      measurement_bandwidth = (uint8) gs_bit_stream_extract_field(bitstream, 3);
    }

    /* Now merge in to the destination structure. */
    for (i = 0; i < earfcn_count && i < MAX_RR_EUTRAN_NEIGHBOUR_LIST_SIZE; ++i)
    {
      if (eutran_params.neighbour_cells_count < ARR_SIZE(eutran_params.neighbour_cells))
      {
        eutran_params.neighbour_cells[eutran_params.neighbour_cells_count].earfcn = (lte_earfcn_t)earfcns[i];
        eutran_params.neighbour_cells[eutran_params.neighbour_cells_count].measurement_bandwidth = measurement_bandwidth;
        ++eutran_params.neighbour_cells_count;
      }
    }
  }

  /* Repeated E-UTRAN Not Allowed Cells */
  while (gs_bit_stream_extract_field(bitstream, 1))
  {
    rr_si2quater_ro_eutran_not_allowed_cells_t not_allowed_cells;

    rr_si2quater_ro_eutran_not_allowed_cells_decode(bitstream, &not_allowed_cells);

    if (eutran_params.not_allowed_cells_count < RR_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS)
    {
      eutran_params.not_allowed_cells[eutran_params.not_allowed_cells_count] = not_allowed_cells;
      ++eutran_params.not_allowed_cells_count;
    }
  }

  /* E-UTRAN Measurement Control Parameters Description */
  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    eutran_params.meas_control_params_valid = TRUE;
    rr_decode_mi_eutran_meas_control_params(bitstream, &eutran_params.meas_control_params);
  }

  if (dst != NULL)
  {
    *dst = eutran_params;
  }
}

/**************************************************************************************
Function
rr_decode_neighbour_cell_description_MI

Description
This function extracts the bit stream information in the IE into
a temporary local storage. If the contents are valid, they are transferred
to the instances array

Parameters
[IN]
byte                  *IE:
Pointer to the bit stream containing the Measurement Information
received from the network.

byte                  IE_length:
Length of the IE.

[OUT]
instances_T           *MeasInfo:
Pointer to global storage for the instances.

boolean               *send_wcdma_neighbor_list_to_L1:
Set to TRUE if any instance has changed.

boolean               *mi_received:
Set to TRUE if all instances have been received

boolean               *decoding_ok:
Set to TRUE if the decoding is successfull.

Returns
None

**********************************************************************************************/
void rr_decode_neighbour_cell_description_MI
(
  byte                  *IE,
  byte                  IE_length,
  instances_T           *MeasInfo,
  boolean               *send_wcdma_neighbor_list_to_L1,
  boolean               *mi_received,
  boolean               *decoding_ok
)
{
  boolean      process_3G_BA = TRUE;

  /* alias */
  SI2q_MI_params_T *common_params;
  MI_params_T    *mi_params;

  /* temporary data structures */
  SI2q_MI_params_T temp_common_params;
  MI_params_T temp_mi_params;

  /* data structure to store the encoded WCDMA neighbor cell information */
  encoded_FDD_info_T MI_Encoded_Info[MAX_UTRAN_FREQ_WITH_CELLS+MAX_UTRAN_FREQ_ONLY];
  encoded_TDD_info_T MI_Encoded_TDD_Info[MAX_UTRAN_TDD_FREQ]; /*9 frequencies required by TDS*/

  /* Using bitstream routines to extract the bits from the rest octet */
  gs_bit_stream_T bitstream;
  gs_bit_stream_T *bit_stream_ptr;

  /* Assert */
  RR_NULL_CHECK_FATAL (IE);
  RR_NULL_CHECK_FATAL (MeasInfo);
  RR_NULL_CHECK_FATAL (send_wcdma_neighbor_list_to_L1);
  RR_NULL_CHECK_FATAL (mi_received);
  RR_NULL_CHECK_FATAL (decoding_ok);

  /* Initialize bit stream */
  gs_bit_stream_initialize(&bitstream,IE,IE_length);
  bit_stream_ptr = &bitstream;

  /* Initialize the temporary data structures */
  /* Initialize data structure to stored the encoded 3G neighbor cell information */
  rr_initialize_encoded_FDD_info (MI_Encoded_Info, MAX_UTRAN_FREQ_WITH_CELLS+MAX_UTRAN_FREQ_ONLY);
  memset (MI_Encoded_TDD_Info, 0, sizeof MI_Encoded_TDD_Info);
  memset (&temp_common_params, 0, sizeof (SI2q_MI_params_T));
  memset (&temp_mi_params, 0, sizeof(MI_params_T));

  /* Initialize output values */
  *decoding_ok = TRUE;
  *send_wcdma_neighbor_list_to_L1 = FALSE;
  *mi_received = FALSE;


  /*********************************************************************/
  /* Start Extracting information into local storage                   */
  /*********************************************************************/

  /* extract the first octet which is the short header and ignore it */
  /* short_header */(void) gs_bit_stream_extract_field(bit_stream_ptr,8);

  if (rr_SI2q_MI_check_for_updates(
        bit_stream_ptr,
        &temp_common_params,
        MeasInfo,
        mi_received) == FALSE)
  {
    /* No Change; This instance has been received and processed already; So Ingore */
    MSG_GERAN_HIGH_2("Repeat MI instance %d",(int) temp_common_params.INDEX, 0 );
    /* log the index and count */
    MSG_GERAN_MED_2("Received MI(Index=%d, Count=%d)",(int) temp_common_params.INDEX, (int) temp_common_params.COUNT );
    /* decoding_ok is TRUE already */
    return;
  }

  MSG_GERAN_MED_2("Received MI(Index=%d, Count=%d)",(int) temp_common_params.INDEX, (int) temp_common_params.COUNT );

  /* Check if the instance number is valid */
  if (temp_common_params.INDEX > temp_common_params.COUNT)
  {
    MSG_GERAN_HIGH_2("INDEX > COUNT in MI (%d > %d)",(int) temp_common_params.INDEX, (int) temp_common_params.COUNT );
    *decoding_ok = FALSE;
    return;
  }

  /* Extract the rest of the fields; Note the following order of calling should be strictly maintained for proper extraction */
  rr_extract_MI_fields1 (bit_stream_ptr, &temp_mi_params);

  rr_extract_SI2q_MI_3G_NC_Desc (
    bit_stream_ptr,
    &temp_common_params,
    NULL,
    &temp_mi_params,
    MI_Encoded_Info,
    decoding_ok,
    MI_Encoded_TDD_Info
  );

  if (*decoding_ok == FALSE)
  {
    MSG_GERAN_HIGH_2("rr_extract_SI2q_MI_3G_NC_Desc failed for MI(%d,%d)",(int) temp_common_params.INDEX, (int) temp_common_params.COUNT );
    return;
  }

  if (!bit_stream_ptr->overflow) rr_extract_MI_3G_MP_Desc (bit_stream_ptr, &temp_mi_params);

  if (bit_stream_ptr->overflow)
  {
#ifdef PERLUTF
    #error code not present
#else
    MSG_GERAN_ERROR_0("Error in network encoding of Measurement Information, ignoring");
#endif
    *decoding_ok = FALSE;
    return;
  }

  if (gs_bit_stream_extract_HL(bit_stream_ptr))
  {
    MSG_GERAN_MED_0("Release 5 Additions present");
    if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
    {
      MSG_GERAN_MED_0("3G Additional Measurement Parameters Description 2 present");
      rr_extract_3g_addl_meas_params_desc_2(bit_stream_ptr, &temp_mi_params);
    }
    else
    {
      MSG_GERAN_MED_0("3G Additional Measurement Parameters Description 2 not present");
    }

    /*
    { null | L -- Receiver compatible with earlier release
           | H -- Additions in Rel-7
    */
    if (gs_bit_stream_extract_HL(bit_stream_ptr))
    {
      MSG_GERAN_MED_0("Additions in Rel-7 present");

      /*
      { 0 | 1  < 700_REPORTING_OFFSET : bit (3) >
               < 700_REPORTING_THRESHOLD : bit (3) > }
      { 0 | 1  < 810_REPORTING_OFFSET : bit (3) >
               < 810_REPORTING_THRESHOLD : bit (3) > }
      */
      if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
      {
        (void) gs_bit_stream_extract_field(bit_stream_ptr, 3);
        (void) gs_bit_stream_extract_field(bit_stream_ptr, 3);
      }
      if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
      {
        (void) gs_bit_stream_extract_field(bit_stream_ptr, 3);
        (void) gs_bit_stream_extract_field(bit_stream_ptr, 3);
      }

      /*
      { null | L -- Receiver compatible with earlier release
             | H -- Additions in Rel-8
      */
      if (gs_bit_stream_extract_HL(bit_stream_ptr))
      {
        MSG_GERAN_MED_0("Additions in Rel-8 present");

        /*
        { 0 | 1 < 3G Supplementary Parameters Description :
                < 3G Supplementary Parameters Description struct >> }
        */
        if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
        {
          rr_decode_mi_utran_suppl_params(bit_stream_ptr, &temp_mi_params.utran_suppl_params);
        }

        /*
        { 0 | 1 < E-UTRAN Parameters Description :
                < E-UTRAN Parameters Description struct >> }
        */
        if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
        {
          rr_decode_mi_eutran_params(bit_stream_ptr, &temp_mi_params.eutran_params);

          if (bit_stream_ptr->overflow)
          {
            MSG_GERAN_ERROR_0("Truncation detected during E-UTRAN Paramters decode, discarding IE");
          }
        }

        /*
        { 0 | 1  < E-UTRAN CSG Description :
                 < E-UTRAN CSG Description struct >> }
        */
        if (gs_bit_stream_extract_field(bit_stream_ptr, 1))
        {
          //rr_decode_mi_eutran_csg_description(bit_stream_ptr, &temp_mi_params);
        }

        /*
        { null | L -- Receiver compatible with earlier release
               | H -- Additions in Rel-9
        */
        if (gs_bit_stream_extract_HL(bit_stream_ptr))
        {
          MSG_GERAN_MED_0("Additions in Rel-9 present");
        }
        else
        {
          MSG_GERAN_MED_0("Additions in Rel-9 not present");
        }
      }
      else
      {
        MSG_GERAN_MED_0("Additions in Rel-7 not present");
        /* Check for truncation here? */
      }
    }
    else
    {
      MSG_GERAN_MED_0("Additions in Rel-7 not present");
      /* Check for truncation here? */
    }
  }
  else
  {
    MSG_GERAN_MED_0("Release 5 Additions not present");
    if (bit_stream_ptr->overflow)
    {
      /* It is permissable for the Rel5 additions to be truncated from the
      message for pre-Rel5 implementations, so ignore any overflow if there
      was no space for the indicator flag in the message. */
      bit_stream_ptr->overflow = FALSE;
    }
  }

  if (bit_stream_ptr->overflow)
  {
#ifdef PERLUTF
    #error code not present
#else
    MSG_GERAN_ERROR_0("Error in network encoding of Measurement Information, ignoring");
#endif
    *decoding_ok = FALSE;
    return;
  }

  /************** Done extracting information from the IE into local storage *************/


  /***************************************************************************************/
  /* Start processing the instance:
     1) Store changes to the 3G_BA_IND, MP_CHANGE_MARK
     2) Decode and store WCDMA FDD frequencies
     3) Transfer the contents in temporary storage into the instances.
     3) Determine if all instances have been received
  */
  /***************************************************************************************/


  MSG_GERAN_HIGH_2("Decoding Measurement Information instance %d, max=%d",(int) temp_common_params.INDEX, (int) temp_common_params.COUNT );

  /* alias */
  common_params = &MeasInfo->instance[temp_common_params.INDEX].common_params;
  mi_params   = &MeasInfo->instance[temp_common_params.INDEX].si2q_mi.mi_params;

  /* Check if there is a change in 3G_BA_IND */
  /* If its first time just store the received data. */
  if (MeasInfo->num_instances == 0)
  {
    common_params->BA_IND_3G = temp_common_params.BA_IND_3G;
    common_params->MP_CHANGE_MARK = temp_common_params.MP_CHANGE_MARK;
  }
  else if (rr_3G_BA_changed(MeasInfo, &temp_common_params) ||
           rr_MP_changed(MeasInfo, &temp_common_params) )
  {
#ifdef FEATURE_NONBLIND_HANDOVER_DEBUG
    MSG_GERAN_HIGH_2("3G_BA_IND in MI:      old <%d>, new <%d> ", (int)common_params->BA_IND_3G,
      (int) temp_common_params.BA_IND_3G);
    MSG_GERAN_HIGH_2("MP_CHANGE_MARK in MI: old <%d>, new <%d> ", (int)common_params->MP_CHANGE_MARK,
      (int) temp_common_params.MP_CHANGE_MARK);
#endif
    /* Re-read all instances of MeasInfo and rebuild the 3G Neighbor list */
    /* This will set valid_data =FALSE and received/changed to FALSE */
    rr_initialize_SI2q_MI_instances(MeasInfo, MI_INSTANCE);

    /* When the 3G_BA_IND parameter is changed in idle mode, the MS shall re-read all
    instances and rebuild the 3G Neighbor Cell list.*/
    common_params->BA_IND_3G = temp_common_params.BA_IND_3G;

    common_params->MP_CHANGE_MARK = temp_common_params.MP_CHANGE_MARK;

    /* reset index_array_length */
    MeasInfo->num_instances = 0;

    /* Start re-reading from this instance onwards */
  }
  else /* no change in 3G_BA_IND/MP_CHANGE_MARK */
  {
    /* If this instance was already received, then no need to process 3G Neighbor cell list */
    if (rr_SI2q_MI_instance_already_received(MeasInfo, temp_common_params.INDEX))
    {
      process_3G_BA = FALSE;
    }
  }

  if (process_3G_BA == TRUE)
  {
    rr_process_3g_BA(
      &MeasInfo->instance[temp_common_params.INDEX],
      MI_Encoded_Info,
      &temp_common_params,
      decoding_ok,
      MI_Encoded_TDD_Info
    );

    if (*decoding_ok == FALSE)
    {
      /* reset this instance ? */
      return;
    }
  } /* process_3G_BA == TRUE */

  *mi_params = temp_mi_params;

  /* decoding_ok should be true at this point; all the false cases are returned already */
  /* Copy the common parameters from the temporary local storage into this instance */
  *common_params = temp_common_params;

  /* Check if the instance was not received already; The instance could have been received
  already and the one that is processed now might be an update in which case the num_instances
  is not incremented */
  if (rr_SI2q_MI_instance_already_received(MeasInfo, temp_common_params.INDEX) == FALSE)
  {
    MeasInfo->num_instances++;
    /* set valid_data to TRUE */
    MeasInfo->instance[temp_common_params.INDEX].valid_data = TRUE;
  }

  /* Update the MeasInfo array to indicate that this instance has been received and changed.
  This has to be done even if the instance was received already because the received and changed
  status variables are reset once a set of instances have been completely received. */
  rr_update_status_SI2q_MI_instance (MeasInfo, temp_common_params.INDEX, TRUE, TRUE);


  /* If 3G_WAIT instances have been received send the new cell list to L1 */
  /* Assumptions: (if these assumptions broken there will be changes here)
  1) 3G_WAIT will be either present (or not present) in all the instances;
  2) 3G_WAIT number of instances with 3G NC Description
  can be considered to send the list to L1; i.e it need not be exactly
  instance #0 till instance #3G_WAIT in that order. (the spec is not explicit
  about this)
  */
  if (temp_mi_params.MI_3G_WAIT_PI)
  {
    if (temp_mi_params.MI_3G_WAIT < temp_common_params.COUNT)
    {
#ifdef FEATURE_NONBLIND_HANDOVER_DEBUG
      MSG_GERAN_HIGH_2("3G_WAIT(%d)<MI_COUNT(%d)",(int)temp_mi_params.MI_3G_WAIT,(int)temp_common_params.COUNT);
#endif

      if (rr_MI_num_instances_with_3g_NC_Desc(MeasInfo) == temp_mi_params.MI_3G_WAIT)
      {
        /* Proceed if this current instance had 3G NC Description; Otherwise,
        the above check may be satified more than once. Once when they get equal and then
        whenever the subsequent instances (after the instance for which the above
        check was satisfied) do not have 3G NC description */
        if (temp_common_params.WCDMA_3G_NC_Desc_PI)
        {
          /* If any change in the first 3G_WAIT instances that have the 3G NC Description */
          if (rr_MI_changed_3g_wait_instances (MeasInfo, MeasInfo->num_instances))
          {
            /* Update L1 with the list information in 3G_WAIT instances */
            *send_wcdma_neighbor_list_to_L1 = TRUE;
            /* reset changed bits in all the instances */
            rr_reset_SI2q_MI_changed (MeasInfo, MAX_INSTANCES);
          }
        }
      }
    }
  }

  /* If all instances are received then the MI_received is set to TRUE; */
  if (rr_received_all_instances(MeasInfo,MeasInfo->num_instances,temp_common_params.COUNT))
  {
    *mi_received = TRUE;
#ifdef FEATURE_NONBLIND_HANDOVER_DEBUG
    MSG_GERAN_HIGH_0("Received all instances of Measurement Information");
#endif

    /* Determine if all the instances are received and if there was a change in any instance; in which
    case set the send_wcdma_neighbor_list_to_L1 flag and the valid_data flag in the utran_neighbor_list
    to TRUE */
    /* Note: The valid_data flag in the utran_neighbor_list may transition from TRUE to TRUE when updates
       are received */
    /* if 3G WAIT was present and nothing changed after 3G_WAIT instances then there will
    be no update to L1 */
    if (rr_SI2q_MI_changed(MeasInfo))
    {
      *send_wcdma_neighbor_list_to_L1 = TRUE;
#ifdef FEATURE_NONBLIND_HANDOVER_DEBUG
    MSG_GERAN_HIGH_0("Sending WCDMA NCELL List to L1");
#endif
      /* reset changed flags; */
      rr_reset_SI2q_MI_changed (MeasInfo, MeasInfo->num_instances);
    } /* if (rr_SI2q_MI_changed */
  }
}


#ifdef FEATURE_LTE
static void rr_csi_eutran_description_decode(
  gs_bit_stream_T * bitstream,
  rr_csi_eutran_description_t * dst
)
{
  dst->earfcn = (lte_earfcn_t) gs_bit_stream_extract_field(bitstream, 16);

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->measurement_bandwidth_valid = TRUE;
    dst->measurement_bandwidth = (uint8) gs_bit_stream_extract_field(bitstream, 3);
  }
  else
  {
    dst->measurement_bandwidth_valid = FALSE;
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->not_allowed_cells_valid = TRUE;
    rr_pcid_group_ie_decode(bitstream, &dst->not_allowed_cells);
  }
  else
  {
    dst->not_allowed_cells_valid = FALSE;
  }

  if (gs_bit_stream_extract_field(bitstream, 1))
  {
    dst->target_pcid_valid = TRUE;
    dst->target_pcid = (uint16) gs_bit_stream_extract_field(bitstream, 9);
  }
  else
  {
    dst->target_pcid_valid = FALSE;
  }
}
#endif /* FEATURE_LTE */

static void rr_cell_selection_indicator_ie_dump(
  rr_chan_rel_cell_selection_indicator_t * src
)
{
  RR_NULL_CHECK_RETURN_VOID(src);

  if (src->valid)
  {
    MSG_GERAN_MED_0("Cell Selection Indicator IE:");
    switch (src->type)
    {
      case GSM_TARGET:
      {
        uint32 i;

        MSG_GERAN_MED_0("  GSM Description:");
        for (i = 0; i < src->target.gsm_target.num_of_cells && i < MAX_GSM_CELLS_IN_CELL_SELECTION_INDICATOR; ++i)
        {
          MSG_GERAN_MED_3("    ARFCN: %d, Band: %d, BSIC: %d",
                  src->target.gsm_target.arfcn[i].num,
                  src->target.gsm_target.arfcn[i].band,
                  CONVERT_FROM_BSIC_T(src->target.gsm_target.bsic[i]));
        }
        break;
      } /* GSM_TARGET */

#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE

      case FDD_TARGET:
      {
        uint32 i;

        MSG_GERAN_MED_0("  UTRAN FDD Description:");
        for (i = 0; i < src->target.fdd_target.num_of_frequencies && i < MAX_UTRAN_FDD_FREQUNECIES; ++i)
        {
          uint32 j;

          MSG_GERAN_MED_1("    UARFCN: %d", src->target.fdd_target.fdd_cell_list[i].uarfcn);
          for (j = 0; j < src->target.fdd_target.fdd_cell_list[i].num_of_cells && j < MAX_UTRAN_FDD_CELLS; ++j)
          {
            MSG_GERAN_MED_2("      Scrambling Code: %d, Diversity: %d",
                    src->target.fdd_target.fdd_cell_list[i].cell_list[j].scrambling_code,
                    src->target.fdd_target.fdd_cell_list[i].cell_list[j].diversity);
          }
        }
        break;
      } /* FDD_TARGET */

#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
      case TDD_TARGET:
      {
        uint32 i;

        MSG_GERAN_MED_0("  UTRAN TDD Description:");
        for (i = 0; i < src->target.tdd_target.target.num_of_frequencies && i < MAX_UTRAN_TDS_FREQUNECIES; ++i)
        {
          uint32 j;

          MSG_GERAN_MED_1("    UARFCN: %d", src->target.tdd_target.target.fdd_cell_list[i].uarfcn);
          for (j = 0; j < src->target.tdd_target.target.fdd_cell_list[i].num_of_cells && j < MAX_UTRAN_TDS_CELLS; ++j)
          {
            MSG_GERAN_MED_2("      CPID: %d, Diversity: %d",
                    src->target.tdd_target.target.fdd_cell_list[i].cell_list[j].cpid,
                    src->target.tdd_target.target.fdd_cell_list[i].cell_list[j].diversity);
          }
        }
        break;
      } /* FDD_TARGET */
#else
      case TDD_TARGET:
      {
        MSG_GERAN_MED_0("  UTRAN TDD Description (not supported)");
        break;
      } /* TDD_TARGET */
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/

#ifdef FEATURE_LTE

      case LTE_TARGET:
      {
        uint32 i;

        MSG_GERAN_MED_0("  LTE Description:");
        for (i = 0; i < src->target.lte_target.eutran_descriptions_count && i < MAX_CSI_EUTRAN_DESCRIPTIONS_COUNT; ++i)
        {
          if (src->target.lte_target.eutran_descriptions[i].measurement_bandwidth_valid)
          {
            if (src->target.lte_target.eutran_descriptions[i].target_pcid_valid)
            {
              MSG_GERAN_MED_3("    E-ARFCN: %d, Measurement Bandwidth: %d, Target PCID: %d",
                      src->target.lte_target.eutran_descriptions[i].earfcn,
                      src->target.lte_target.eutran_descriptions[i].measurement_bandwidth,
                      src->target.lte_target.eutran_descriptions[i].target_pcid);
            }
            else
            {
              MSG_GERAN_MED_2("    E-ARFCN: %d, Measurement Bandwidth: %d, Target PCID: not present",
                      src->target.lte_target.eutran_descriptions[i].earfcn,
                      src->target.lte_target.eutran_descriptions[i].measurement_bandwidth);
            }
          }
          else
          {
            if (src->target.lte_target.eutran_descriptions[i].target_pcid_valid)
            {
              MSG_GERAN_MED_2("    E-ARFCN: %d, Measurement Bandwidth: not present, Target PCID: %d",
                      src->target.lte_target.eutran_descriptions[i].earfcn,
                      src->target.lte_target.eutran_descriptions[i].target_pcid);
            }
            else
            {
              MSG_GERAN_MED_1("    E-ARFCN: %d, Measurement Bandwidth: not present, Target PCID: not present",
                      src->target.lte_target.eutran_descriptions[i].earfcn);
            }
          }

          if (src->target.lte_target.eutran_descriptions[i].not_allowed_cells_valid)
          {
            uint32 j;
            uint32 low, high;

            MSG_GERAN_MED_0("    Not Allowed Cells:");

            /* Attempt to print out ranges here as we might otherwise generate
            hundreds of calls to MSG_MED. */
            j = 0;

            /* Outer loop iterates over the PCID group searching for the 'first'
            included index.  Inner loop then iterates from that index, searching for
            the next not included index.  The range is then from first to next - 1.

            e.g. for the following pattern:

            Index: 0 1 2 3 4 5 6 7 8 9
            Value: 0 1 1 1 0 0 1 1 1 1

            The first range would be 1 - 3
            The next range would be  6 - 9

            */

            while (j <= RR_LTE_PCID_MAX)
            {
              if (RR_PCID_GROUP_IE_GET(src->target.lte_target.eutran_descriptions[i].not_allowed_cells.bitmap, j))
              {
                low = j;
                high = low;

                ++j;

                while (j <= RR_LTE_PCID_MAX)
                {
                  if (!RR_PCID_GROUP_IE_GET(src->target.lte_target.eutran_descriptions[i].not_allowed_cells.bitmap, j))
                  {
                    /* We have found an index that is not included, so the range goes as far
                    as the previous index. */
                    high = j - 1;

                    ++j;
                    break;
                  }
                  else
                  {
                    ++j;
                  }
                }

                if (j > RR_LTE_PCID_MAX)
                {
                  /* We reached the end of the array without finding an unset entry,
                  so set 'high' to the last valid index. */
                  high = RR_LTE_PCID_MAX;
                }

                if (high == low)
                {
                  MSG_GERAN_MED_1("PCID: %d", low);
                }
                else if (high > low)
                {
                  MSG_GERAN_MED_2("PCID Range: %d -> %d", low, high);
                }
              }
              else
              {
                ++j;
              }
            }
          }
          else
          {
            MSG_GERAN_MED_0("    Not Allowed Cells: not present");
          }
        }

        break;
      } /* LTE_TARGET */

#endif /* FEATURE_LTE */

      default:
      {
        MSG_GERAN_ERROR_1("Unexpected type: %d", src->type);
      }
    }
  }
  else
  {
    MSG_GERAN_MED_0("Cell Selection Indicator IE: not valid");
  }
}

boolean rr_decode_cell_selection_indicator(
  byte *IE, byte IE_length,
  rr_chan_rel_cell_selection_indicator_t *cell_sel_indicator,
  const gas_id_t gas_id
)
{
  gs_bit_stream_T bitstream;
  gs_bit_stream_T *bit_stream_ptr;
  rr_cell_selection_target_e target_type = UNKNOWN_TARGET;

  if ((IE == NULL) || (cell_sel_indicator == NULL))
  {
    MSG_GERAN_ERROR_0_G("NULL pointer parameter");
    return FALSE;
  }

  /* initialize local */
  memset((void *)cell_sel_indicator, 0, sizeof(rr_chan_rel_cell_selection_indicator_t));
  cell_sel_indicator->type = UNKNOWN_TARGET;

  /* Initialize bit stream, skip the IE length byte */
  gs_bit_stream_initialize(&bitstream,&IE[1],IE_length);
  bit_stream_ptr = &bitstream;

  target_type = (rr_cell_selection_target_e) gs_bit_stream_extract_field(bit_stream_ptr, 3);

  /*** 0 - gsm target ***/
  if (target_type == GSM_TARGET)
  {
    byte band_ind;
    byte num_of_cells = 0;
    rr_gsm_target_info_type *gsm_target;

    MSG_GERAN_MED_0_G("cell selection indicator contains GSM description ");

    cell_sel_indicator->type = GSM_TARGET;
    gsm_target = &cell_sel_indicator->target.gsm_target;

    while ( (gs_bit_stream_extract_field(bit_stream_ptr, 1) == 1) &&
            (num_of_cells < MAX_GSM_CELLS_IN_CELL_SELECTION_INDICATOR) )
    {
      byte bsic;

      band_ind = (byte)gs_bit_stream_extract_field(bit_stream_ptr, 1);
      gsm_target->arfcn[num_of_cells].num = (word)gs_bit_stream_extract_field(bit_stream_ptr, 10);
      gsm_target->arfcn[num_of_cells].band =
        (sys_band_T) rr_get_band_using_band_indicator(
          gsm_target->arfcn[num_of_cells].num,
          (si_psi_band_ind_T)band_ind,
          gas_id
        );
      bsic = (byte)gs_bit_stream_extract_field(bit_stream_ptr, 6);
      gsm_target->bsic[num_of_cells].PLMN_colour_code = (byte) (bsic & 0x38) >> 3;
      gsm_target->bsic[num_of_cells].BS_colour_code   = (byte) (bsic & 0x07);
      num_of_cells++;
    }

    gsm_target->num_of_cells = num_of_cells;
    cell_sel_indicator->valid = TRUE;

  }
#ifdef FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE

  /*** 1 - fdd target ***/
  else if (target_type == FDD_TARGET)
  {
    byte num_of_freq = 0;
    boolean decode_ok = FALSE;

    rr_rrc_fdd_target_info_type *fdd_target;

    MSG_GERAN_MED_0_G("cell selection indicator contains FDD description ");

    cell_sel_indicator->type = FDD_TARGET;
    fdd_target = &cell_sel_indicator->target.fdd_target;

    while ( (gs_bit_stream_extract_field(bit_stream_ptr, 1) == 1) &&
            (num_of_freq < MAX_UTRAN_FDD_FREQUNECIES) )
    {
      encoded_FDD_info_T encoded_fdd_cells;
      byte NR_OF_FDD_CELLS;
      word cell_list[MAX_UTRAN_FDD_CELLS];

      if (gs_bit_stream_extract_field(bit_stream_ptr, 1) == 1)
      {
        /* bandwidth_FDD */
        (void)gs_bit_stream_extract_field(bit_stream_ptr, 3);
      }

      fdd_target->fdd_cell_list[num_of_freq].uarfcn =
        (word)gs_bit_stream_extract_field(bit_stream_ptr, 14);

      fdd_target->fdd_cell_list[num_of_freq].num_of_cells = 0;

      if (gs_bit_stream_extract_field(bit_stream_ptr, 1) == 1)
      {
        /* FDD_indic0 */
        boolean fdd_indic0 = (boolean) gs_bit_stream_extract_field(bit_stream_ptr, 1);

        /* NR_OF_FDD_CELLS */
        NR_OF_FDD_CELLS = (byte)gs_bit_stream_extract_field(bit_stream_ptr, 5);

        /* below codes extract the FDD_CELL_INFORMATION Field
         * at first the field is extracted as encoded cells (W values in 1024 format)
         * then extracted as individual cells (real scrambling codes + diversity)
         * and then convert to fdd_target format
         */
        if (rr_decode_fdd_cell_info_field(NR_OF_FDD_CELLS, bit_stream_ptr, &encoded_fdd_cells))
        {
          if(encoded_fdd_cells.num_cells >= (MAX_UTRAN_CELLS_PER_FREQUENCY+1))
          {
            MSG_GERAN_ERROR_0("num_encoded_cells >= (MAX_UTRAN_CELLS_PER_FREQUENCY+1)!!");
          }
          else
          {
            decode_ok = decode (encoded_fdd_cells.encoded_cells,
                                encoded_fdd_cells.num_cells,
                                cell_list, NELEMS(cell_list), 1023);
          }
          if (decode_ok)
          {
            byte cell_count = 0;
            byte i = 0;

            if (fdd_indic0)
            {
              fdd_target->fdd_cell_list[num_of_freq].cell_list[0].scrambling_code = 0;
              fdd_target->fdd_cell_list[num_of_freq].cell_list[0].diversity = 0;
              ++cell_count;
            }

            for (i = 0; i < encoded_fdd_cells.num_cells; ++i)
            {
              fdd_target->fdd_cell_list[num_of_freq].cell_list[cell_count].scrambling_code =
                (cell_list[i] & 0x1ff) << 4;
              fdd_target->fdd_cell_list[num_of_freq].cell_list[cell_count].diversity =
                (cell_list[i] & 0x200) != 0;

              ++cell_count;
            }
            fdd_target->fdd_cell_list[num_of_freq].num_of_cells = cell_count;
          }
          else /* Decoding may have failed, but handle FDD_Indic0 anyway. */
          {
            if (fdd_indic0)
            {
              fdd_target->fdd_cell_list[num_of_freq].cell_list[0].scrambling_code = 0;
              fdd_target->fdd_cell_list[num_of_freq].cell_list[0].diversity = 0;
              fdd_target->fdd_cell_list[num_of_freq].num_of_cells = 1;
            }
            else
            {
              fdd_target->fdd_cell_list[num_of_freq].num_of_cells = 0;
            }
          }

          if (fdd_target->fdd_cell_list[num_of_freq].num_of_cells > 1)
          {
            if (fdd_target->fdd_cell_list[num_of_freq].num_of_cells > MAX_UTRAN_FDD_CELLS)
            {
              fdd_target->fdd_cell_list[num_of_freq].num_of_cells = MAX_UTRAN_FDD_CELLS;
            }

            qsort(
              fdd_target->fdd_cell_list[num_of_freq].cell_list,
              fdd_target->fdd_cell_list[num_of_freq].num_of_cells,
              sizeof(rr_rrc_fdd_cell_type),
              compare_rr_rrc_fdd_cell_type_scrambling_codes
            );
          }
        }
        else
        {
          if (fdd_indic0)
          {
            fdd_target->fdd_cell_list[num_of_freq].cell_list[0].scrambling_code = 0;
            fdd_target->fdd_cell_list[num_of_freq].cell_list[0].diversity = 0;
            fdd_target->fdd_cell_list[num_of_freq].num_of_cells = 1;
          }
          else
          {
            fdd_target->fdd_cell_list[num_of_freq].num_of_cells = 0;
          }
        }
      }

      num_of_freq++;

    }
    fdd_target->num_of_frequencies = num_of_freq;

    cell_sel_indicator->valid = TRUE;
  }
#endif /* FEATURE_GTOW_REDIRECTION_AFTER_CONN_RELEASE */
  /*
   * 2 - tdd target
   */
  else if (target_type == TDD_TARGET)
  {
    MSG_GERAN_MED_0_G("cell selection indicator contains TDD description ");

    cell_sel_indicator->type = TDD_TARGET;

#ifdef FEATURE_GSM_TDS_G2T_REDIRECTION
    {
      byte num_of_freq = 0;
      boolean decode_ok = FALSE;

      tdsrrc_target_info_type *tdd_target;

#ifdef FEATURE_GSM_TDS_PSEUDO_FAST_RETURN
      {
        rr_dedicated_data_T *dedic_ptr = rr_get_dedicated_data_ptr(gas_id);

        cell_sel_indicator->target.tdd_target.pseudo_fr_active = FALSE;

        if (dedic_ptr->tds_pseudo_fr.active == TRUE)
        {
          MSG_GERAN_ERROR_0_G("RR Pseudo FR unexpected G->T Redir for TDS during PFR mode");
          rr_tds_pseudo_fr_reset_dedicated_info(gas_id);
        }
      }
#endif /*FEATURE_GSM_TDS_PSEUDO_FAST_RETURN*/

      tdd_target = &cell_sel_indicator->target.tdd_target.target;

      while ( (gs_bit_stream_extract_field(bit_stream_ptr, 1) == 1) &&
              (num_of_freq < MAX_UTRAN_TDS_FREQUNECIES) )
      {
        encoded_TDD_info_T encoded_tdd_cells;
        byte NR_OF_TDD_CELLS;
        word cell_list[MAX_UTRAN_TDS_CELLS];

        if (gs_bit_stream_extract_field(bit_stream_ptr, 1) == 1)
        {
          /* bandwidth_TDD */
          (void)gs_bit_stream_extract_field(bit_stream_ptr, 3);
        }

        tdd_target->fdd_cell_list[num_of_freq].uarfcn =
          (word)gs_bit_stream_extract_field(bit_stream_ptr, 14);

        tdd_target->fdd_cell_list[num_of_freq].num_of_cells = 0;

        if (gs_bit_stream_extract_field(bit_stream_ptr, 1) == 1)
        {
          /* TDD_indic0 */
          boolean tdd_indic0 = (boolean) gs_bit_stream_extract_field(bit_stream_ptr, 1);

          /* NR_OF_TDD_CELLS */
          NR_OF_TDD_CELLS = (byte)gs_bit_stream_extract_field(bit_stream_ptr, 5);

          /* below codes extract the TDD_CELL_INFORMATION Field
           * at first the field is extracted as encoded cells (W values in 512 format)
           * then extracted as individual cells (real cell parameter id + diversity)
           * and then convert to tdd_target format
           */
          if (rr_decode_tdd_cell_info_field(NR_OF_TDD_CELLS, bit_stream_ptr, &encoded_tdd_cells))
          {
            if(encoded_tdd_cells.num_cells >= (MAX_UTRAN_CELLS_PER_FREQUENCY+1))
            {
              MSG_GERAN_ERROR_0("num_encoded_cells >= (MAX_UTRAN_CELLS_PER_FREQUENCY+1)!!");
            }
            else
            {
              decode_ok = decode (encoded_tdd_cells.encoded_cells,
                                  encoded_tdd_cells.num_cells,
                                  cell_list, NELEMS(cell_list), 511);
            }
            if (decode_ok)
            {
              byte cell_count = 0;
              byte i = 0;

              if (tdd_indic0)
              {
                tdd_target->fdd_cell_list[num_of_freq].cell_list[0].cpid = 0;
                tdd_target->fdd_cell_list[num_of_freq].cell_list[0].diversity = 0;
                ++cell_count;
              }

              for (i = 0; i < encoded_tdd_cells.num_cells; ++i)
              {
                tdd_target->fdd_cell_list[num_of_freq].cell_list[cell_count].cpid = cell_list[i] & 0x007f;

                tdd_target->fdd_cell_list[num_of_freq].cell_list[cell_count].diversity = (boolean)(cell_list[i] & 0x0100);

                ++cell_count;
              }
              tdd_target->fdd_cell_list[num_of_freq].num_of_cells = cell_count;
            }
            else /* Decoding may have failed, but handle TDD_Indic0 anyway. */
            {
              if (tdd_indic0)
              {
                tdd_target->fdd_cell_list[num_of_freq].cell_list[0].cpid = 0;
                tdd_target->fdd_cell_list[num_of_freq].cell_list[0].diversity = 0;
                tdd_target->fdd_cell_list[num_of_freq].num_of_cells = 1;
              }
              else
              {
                tdd_target->fdd_cell_list[num_of_freq].num_of_cells = 0;
              }
            }

            if (tdd_target->fdd_cell_list[num_of_freq].num_of_cells > 1)
            {
              if (tdd_target->fdd_cell_list[num_of_freq].num_of_cells > MAX_UTRAN_TDS_CELLS)
              {
                tdd_target->fdd_cell_list[num_of_freq].num_of_cells = MAX_UTRAN_TDS_CELLS;
              }

              qsort(
                tdd_target->fdd_cell_list[num_of_freq].cell_list,
                tdd_target->fdd_cell_list[num_of_freq].num_of_cells,
                sizeof(tdsrrc_cell_type),
                compare_rr_rrc_tdd_cell_type_cpid
              );
            }
          }
          else
          {
            if (tdd_indic0)
            {
              tdd_target->fdd_cell_list[num_of_freq].cell_list[0].cpid = 0;
              tdd_target->fdd_cell_list[num_of_freq].cell_list[0].diversity = 0;
              tdd_target->fdd_cell_list[num_of_freq].num_of_cells = 1;
            }
            else
            {
              tdd_target->fdd_cell_list[num_of_freq].num_of_cells = 0;
            }
          }
        }

        num_of_freq++;

      }
      tdd_target->num_of_frequencies = num_of_freq;

      cell_sel_indicator->valid = TRUE;
    }
#else
    /* we don't decode TDD at this time */
    cell_sel_indicator->valid = FALSE;
#endif /*FEATURE_GSM_TDS_G2T_REDIRECTION*/
  }
#ifdef FEATURE_LTE
  else if (target_type == LTE_TARGET)
  {
    rr_lte_target_info_type * lte_target = &cell_sel_indicator->target.lte_target;

    cell_sel_indicator->type = LTE_TARGET;
    cell_sel_indicator->valid = TRUE;

    while (gs_bit_stream_extract_field(bit_stream_ptr, 1))
    {
      rr_csi_eutran_description_t eutran_description;

      rr_csi_eutran_description_decode(bit_stream_ptr, &eutran_description);

      if (lte_target->eutran_descriptions_count < MAX_CSI_EUTRAN_DESCRIPTIONS_COUNT)
      {
        lte_target->eutran_descriptions[lte_target->eutran_descriptions_count] = eutran_description;
        ++lte_target->eutran_descriptions_count;
      }
      else
      {
        MSG_GERAN_ERROR_0_G("Overflow while extracting CSI EUTRAN Descriptions");
      }
    }
  }
#endif /* FEATURE_LTE */
  else
  {
    MSG_GERAN_MED_0_G("cell selection indicator contains UNKNOWN description ");

    cell_sel_indicator->type = UNKNOWN_TARGET;
    cell_sel_indicator->valid = FALSE;
  }

  rr_cell_selection_indicator_ie_dump(cell_sel_indicator);

  return TRUE;
}




/**  Functions for managing IEs in OTA messages  **/

/*
* @brief Returns a pointer to the first IE of the specified message
*
* @param msg_type    one of the defined OTA message types
*        index_ptr   (optional) output pointer to index where IE was found
*        length_ptr  (optional) output pointer to the message's imperative length
*
* @return Returns a pointer to the first IE of the specified message
*/
ie_descriptor_T *rr_get_first_ie_and_length(
  byte msg_type,
  uint8 *length_ptr,
  const gas_id_t gas_id
)
{
  uint8 max_index;
  uint8 index = 0;
  message_list_T *message_list_ptr = rr_get_message_list_ptr(gas_id);
  byte *max_message_index_ptr = rr_get_max_message_index_ptr(gas_id);

  RR_NULL_CHECK_FATAL(message_list_ptr);
  RR_NULL_CHECK_FATAL(max_message_index_ptr);

  max_index = (uint8)(*max_message_index_ptr);

  /* search the message list for the specified type */
  while ((message_list_ptr[index].message_type != msg_type) &&
         (index < max_index))
  {
    index++;
  }

  if (index >= max_index)
  {
    MSG_GERAN_ERROR_1_G("Could not find message_list entry for message type: %d",(int)msg_type);
    return(NULL);
  }

  /* if the caller wanted the imperative length, set it here */
  if (length_ptr != NULL)
  {
    *length_ptr = message_list_ptr[index].imperative_length;
  }

  return(message_list_ptr[index].first_IE);
} /* rr_get_first_ie_and_length() */


/*
* @brief Returns a pointer to the first IE of the specified message
*
* Same as rr_get_first_ie_and_length, but with the length parameter omitted
*
* @param msg_type   one of the defined OTA message types
*        index_ptr  (optional) output pointer to index where IE was found
*
* @return Returns a pointer to the first IE of the specified message
*/
ie_descriptor_T *rr_get_first_ie_of_message(byte msg_type, const gas_id_t gas_id)
{
  return (rr_get_first_ie_and_length(msg_type, NULL, gas_id));
} /* rr_get_first_ie_of_message() */


/*
* @brief Returns a pointer to the start of a particular IE within a given message
*
* @param search_ptr  starting IE for search
*        IEI         IE to search for
*
* @return Returns a pointer to the first IE of the specified message
*/
byte * rr_find_start_of_IE(ie_descriptor_T *search_ptr, byte IEI)
{
  ie_descriptor_T *current_ptr = search_ptr;

  while (current_ptr != NULL)
  {
    if (current_ptr->IEI == IEI)
    {
      return(current_ptr->start_IE);
    }
    current_ptr = current_ptr->next_IE;
  }

  return(NULL);
}


//**************************************************************************
/*
* @brief  determines the length in octets from the current CSN1 encoded IE
*         to the end of the l3 GSM OTA msg
*
* CSN.1 encoded Rest octets RCVD in GSM OTA msg that have rest octets as the last octets of the MSG
* which are not variable in length in the msg
*
* si 1    size = 1
* si 3    size = 4
* si 6    size = 7
* si 7    size = 20
* si 8    size = 20
* si 9    size = 17  not used
* si 13   size = 20
* page request 3   size = 3  not used in code
* pkt channel discription (rcvd in IA only) size = 4
* IAR  size = 3
*
*
* CSN.1 encoded Rest octets RCVD in GSM OTA msg that have rest octets as the last octets of the MSG
* which are variable in length in the msg and have NO length indicator
*
* page request 1   size = 0 to 17
* page request 2   size = 0 to 11
* IA               size = 0 to 11
* si 4             size = 0 to 10
*
* OTA msg that have variable IE octets that don't occur at the end of the msg
* but have a legth  indicator
*
* DTM ass command  :
* rr pkt ul assignment IE size = 3 to n
* rr pkt dl assignment IE size = 3 to n
* gprs broadcast info  IE size = 7 to n
*
* DTM information :
* DTM information details IE  size = 4 to n
*
*
* @param msg_type             GSM OTA msg type that containing CSN1 encoded IE
*
*        csn1_start_ie_ptr    Points to the given CSN1 encoded IE rcvd in the
*                             OTA msg type
*
* @return Returns length in octets to the end of the L3 OTA buffer
*/

uint8 rr_length_to_end_of_ota_msg(
  uint8 msg_type,
  uint8 *csn1_start_ie_ptr,
  csn_rr_decoder_msg_id_t csn_ie_type,
  const gas_id_t gas_id
)
{
  uint8  length = 0;
  uint8  index = 0;

  uint8  max_buff_size = 0;


  uint8 max_index;
  message_list_T *message_list_ptr = rr_get_message_list_ptr(gas_id);
  byte *max_message_index_ptr = rr_get_max_message_index_ptr(gas_id);

  RR_NULL_CHECK_FATAL(message_list_ptr);
  RR_NULL_CHECK_FATAL(max_message_index_ptr);

  max_index = (uint8)(*max_message_index_ptr);

  if(csn1_start_ie_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("NULL pointer");
    return(length);
  }

  /* search the message list for the specified type */
  while ((message_list_ptr[index].message_type != msg_type) &&
         (index < max_index))
  {
    index++;
  }

  /*Guard against erroneous input data */
  if (index >= max_index)
  {
    MSG_GERAN_ERROR_1_G("Could not find message_list entry for message type: %d",(int)msg_type);
    return(length);
  }

  /*check csn1_start_ie_ptr actual points to a location after the start of l3 buffer*/
  if(csn1_start_ie_ptr < message_list_ptr[index].start_l3_msg)
  {
    return(length);
  }


  /*only valid vaule of l3_msg_len are N201_MAX and MAX_OCTETS_L3_MESSAGE */
  if((message_list_ptr[index].l3_msg_len == N201_MAX) ||
     (message_list_ptr[index].l3_msg_len == MAX_OCTETS_L3_MESSAGE))
  {
    /*check csn1_start_ie_ptr does not go beyond the end of the l3 buffer*/
    if(csn1_start_ie_ptr >= ((message_list_ptr[index].start_l3_msg) + message_list_ptr[index].l3_msg_len))
    {
      return(length);
    }
    else
    {
      switch (csn_ie_type)
      {
        /*CSN.1 encoded Rest octets RCVD in GSM OTA msg that have rest octets  *
        *  as the last part MSG which are not variable in length in the msg    */

        case SI1_REST_OCTETS  : length = 1;
          break;

        case SI3_REST_OCTETS  : length = 4;
          break;

        case SI6_REST_OCTETS  : length = 7;
          break;

        case SI7_REST_OCTETS  : length = 20;
          break;

        case SI8_REST_OCTETS  : length = 20;
          break;

        case SI13_REST_OCTETS : length = 20;
          break;

        case PACKET_CHANNEL_DESCRIPTION : length = 4;
          break;

        case IAR_REST_OCTETS  : length = 3;
          break;

        /*CSN.1 encoded Rest octets RCVD in GSM OTA msg that have rest octets as the   *
        * last part MSG which are variable in length in the msg and have NO length ind */

        case P1_REST_OCTETS  :
        case P2_REST_OCTETS  :
        case IA_REST_OCTETS  :
        case SI4_REST_OCTETS :
          length =  message_list_ptr[index].l3_msg_len - (uint8)((csn1_start_ie_ptr - message_list_ptr[index].start_l3_msg));
          break;

        /*OTA msg that have variable IE octets that don't occur at the end of the  msg  *
        * but have a length indicator as part of the IE. Use this to determine the len  *
        *                                                                               *
        * NOte in message_list[].xxxx.start_IE for TLV and LV IE formats this is the   .*
        * length indicator octet of the IE.                                            */

        case RR_PACKET_UPLINK_ASSIGNMENT:
        case RR_PACKET_DOWNLINK_ASSIGNMENT:
          /*note csn decoder expects the IEI length ind for these msg as part of the   *
          *payload that it has t decode.                                              */
          length = *csn1_start_ie_ptr;

          /*add 1 becasue length ind is included*/
          length++;
          break;

        case GPRS_BROADCAST_INFORMATION :
        case DTM_INFORMATION_DETAILS:
          /*note csn decoder does NOT expect the IEI length ind for these msg's as    *
          * part of the payload that it has to decode.                                */
          length = *csn1_start_ie_ptr;
          break;

        default :
          MSG_GERAN_ERROR_1_G("unknown csn_ie_type %d", csn_ie_type);
          length = 0;
          break;
      }

      /*determine max space to end of buffer so all calculated lengths can be checked against it*/
      max_buff_size = message_list_ptr[index].l3_msg_len - (uint8)((csn1_start_ie_ptr - message_list_ptr[index].start_l3_msg));

      if (length > max_buff_size)
      {
        return(length = 0);
      }

    }
  }
  else
  {
    return(length);
  }


  return(length);
}

//******************************************************************************


static void reset_ota_message_at_index(byte message_index, const gas_id_t gas_id)
{
  ie_descriptor_T *current_IE;
  message_list_T *message_list_ptr = rr_get_message_list_ptr(gas_id);

  RR_NULL_CHECK_FATAL(message_list_ptr);

  current_IE  = message_list_ptr[message_index].first_IE;

  while (current_IE != NULL)
  {
    current_IE->start_IE = NULL;
    current_IE = current_IE->next_IE;
  }
} /* end reset_ota_message_at_index */


static boolean rr_l2_channel_valid(l2_channel_type_T allowed, l2_channel_type_T actual)
{
  if (allowed == actual)
  {
    return TRUE;
  }

  /* Messages that are normally sent on the CCCH may actually have an L2
  channel type of BCCH due to the way in which L1 sets the channel (e.g if
  RR has requested L1 to read all extended BCCH TCs then L1 will set the
  channel type for these TCs to BCCH instead of CCCH).

  The same goes for BCCH messages - if they are on the Extended BCCH L1
  may end up setting the channel type to CCCH.
  */
  if ((allowed == CCCH && actual == BCCH) ||
      (allowed == BCCH && actual == CCCH))
  {
    return TRUE;
  }


  return FALSE;
}


/*
* @brief Sanity-check an OTA message
*
* @param layer3_message  The L3 message as received OTA
*        message_length  number of octets
*        l2_channel_type  channel message received on
*        short_header     indication of the message has a short header
*        message_ok       (output) success indication
*        error_type       (output) error cause in cause of failure
*/
void rr_check_for_protocol_errors
(
  byte              *layer3_message,
  byte              message_length,
  l2_channel_type_T l2_channel_type,
  boolean           short_header,
  boolean           *message_ok,
  RR_cause_T        *error_type,
  const gas_id_t     gas_id
)
{
  ie_descriptor_T *current_IE;
  ie_descriptor_T *left_IE;
  byte             message_index;
  byte             message_type;
  uint16           index;
  message_list_T  *message_list_ptr = rr_get_message_list_ptr(gas_id);
  byte            *max_message_index_ptr = rr_get_max_message_index_ptr(gas_id);

  /***********************************************************************/
  /* In this module, we only check the presence or non presence of IEIs  */
  /* The syntactical and semantical checks on the contents of the IEIs   */
  /* will be done at a later stage                                       */
  /***********************************************************************/
  message_index = 0;
  *message_ok   = TRUE;
  left_IE       = NULL;


  if ((l2_channel_type == BCCH) || (l2_channel_type == CCCH))
  {
    if ( message_length > N201_MAX)
    {
      MSG_GERAN_HIGH_0_G("Message length exceeds message payload size");
      *error_type = RR_SEMANTICALLY_INCORRECT_MESSAGE;
      *message_ok = FALSE;
      return;
    }
  }
  else
  {
    if ( message_length > MAX_OCTETS_L3_MESSAGE)
    {
      MSG_GERAN_HIGH_0_G("Message length exceeds message payload size");
      *error_type = RR_SEMANTICALLY_INCORRECT_MESSAGE;
      *message_ok = FALSE;
      return;
    }
  }

  /* determine index offset to message type */
  if ((l2_channel_type == BCCH) || (l2_channel_type == CCCH))
  {
    index = 2;
  }
  else
  {
    index = 1;
  }

  /* extract the 6-bit message type from the appropriate position */
  message_type = layer3_message[index]; // & 0x3F;

  /* check for short header format - see 3GPP 24.007, section 11.3.2.1 */
  if ((l2_channel_type == SACCH) && (short_header == TRUE))
  {
    /* message type for short headers is only 5 bits */
    message_type = layer3_message[0] & 0x7C;

    /* shift right by 2 bits */
    message_type >>= 2;

    if (message_type == MEASUREMENT_INFORMATION)
    {
      /* MEASUREMENT_INFORMATION is 0x5 which is also SYSTEM_INFORMATION_TYPE_5_BIS */
      /* So, change it to a different unused value */
      message_type = MEASUREMENT_INFORMATION_TYPE;
    }
    else
    {
      /* for details of further unsupported short format messages */
      /* (Si10 for example) see table 10.4.2 in 3GPP TS 04.18     */
      MSG_GERAN_MED_1_G("Ignoring short msg (type=%d) on SACCH", message_type);

      /* flag the message type as unknown to avoid matching */
      message_type = UNKNOWN_SHORT_HEADER_MESSAGE_TYPE;

      /* avoid unnecessary message checking below */
      message_index = *max_message_index_ptr - 1;
    }
  }

  /******************************************************************/
  /* Try to find a matching entry in the message list, i.e.         */
  /* a structural description corresponding to the received message */
  /******************************************************************/
  while ((message_list_ptr[message_index].message_type != message_type) &&
         (message_index < *max_message_index_ptr))
  {
    message_index++;
  }

  /* determine whether a matching message structure was found */
  if (message_index == *max_message_index_ptr)
  {
    if (message_type != UNKNOWN_SHORT_HEADER_MESSAGE_TYPE)
    {
      MSG_GERAN_ERROR_1_G("Bad value(%d) for message type (not RR)",message_type);
    }
    /*************************************************************/
    /* The mobile has received a message with a message type not */
    /* defined in RR. We have to return an error "message type   */
    /* non existent or not implemented"                          */
    /*************************************************************/
    *error_type = RR_MESSAGE_TYPE_NON_EXISTENT;
    *message_ok = FALSE;
    (void) rr_log_protocol_error(message_type, RR_PROTOCOL_ERROR_UNKNOWN_TYPE, gas_id);
  }
  else
  {
    /* Message index matched, reset message pointers before decoding.
       Possibility that a previous decode exited before the message was reset */
    reset_ota_message_at_index(message_index, gas_id);

    /**
     * Check that the message was received on a valid L2 channel - discard it
     * if not.
     */
    if (rr_l2_channel_valid(message_list_ptr[message_index].l2_channel, l2_channel_type))
    {
      /****************************************************************/
      /* Check if the received message is sufficiently long, ie if it */
      /* is able to contain all the imperative part of the message    */
      /****************************************************************/
      if (message_length < message_list_ptr[message_index].imperative_length)
      {
        /*********************************************/
        /* There is an imperative message part error */
        /* The message has to be ignored             */
        /*********************************************/
        if (message_list_ptr[message_index].message_type != CHANNEL_RELEASE)
        {

          *error_type = RR_INVALID_MANDATORY_INFORMATION;
          *message_ok = FALSE;
          MSG_GERAN_HIGH_2_G("Length(%d) of message(%d) is too short",(int) message_length,(int) message_type);
          (void) rr_log_protocol_error(message_type, RR_PROTOCOL_ERROR_MESSAGE_TOO_SHORT, gas_id);
        }
        else
        {
          /*****************************************/
          /* Fill the first IE of the message with */
          /* Normal Release IE                     */
          /*****************************************/
          MSG_GERAN_HIGH_0_G("Attempt to handle channel release with incorrect length");
          *message_ok = TRUE;
        }
      }
      else if (message_list_ptr[message_index].message_type == MEASUREMENT_INFORMATION_TYPE)
      {
        current_IE = message_list_ptr[message_index].first_IE;
        current_IE->start_IE = &layer3_message[0];
        *message_ok = TRUE;
      }
      else
      {
        index++;
        current_IE = message_list_ptr[message_index].first_IE;

        message_list_ptr[message_index].start_l3_msg = layer3_message;

        if ((l2_channel_type == BCCH) || (l2_channel_type == CCCH))
        {
          message_list_ptr[message_index].l3_msg_len = N201_MAX;
        }
        else
        {
          message_list_ptr[message_index].l3_msg_len = MAX_OCTETS_L3_MESSAGE;
        }


        /**********************************************************/
        /* Skip all the IE which are part of the imperative part. */
        /* The imperative part contains only V and LV IEs         */
        /**********************************************************/
        while (current_IE != NULL)
        {
          if (current_IE->presence == MANDATORY_IE)
          {
            if (current_IE->IE_format == V)
            {
              current_IE->start_IE = &layer3_message[index];
              index += current_IE->IE_length;

              if (index > message_length)
              {
                /*
                This is a non-ideal situation where SI6 does not conform to the specifications.
                The specifications indicate SI6 rest octets are mandatory, but in the 'real world' there have been
                examples where older networks have broadcast SI6 without rest octets.
                This is because in Phase I GSM they were not mandatory, but when Phase II GSM was introduced, rest
                octets were added to SI6, but made mandatory, which is non-backward-compatible with Phase I.
                There is no way to tell whether a network is Phase I or Phase II.
                */
                if(message_type  == SYSTEM_INFORMATION_TYPE_6)
                {
                  MSG_GERAN_HIGH_1_G("SI6 with no RO, msg length(%d)",(int) message_length);
                }
                else
                {
                  MSG_GERAN_ERROR_2_G("Indexing(%d,%d) beyond end of message",(int) index,(int) message_length);
                  *message_ok = FALSE;
                  return;
                }
                current_IE->start_IE = NULL;
                break;
              }

              /*********************************************************/
              /* The last IE in the message has a Value format and has */
              /* a variable length, depending on the previous IEs.     */
              /* This is the case or SI 4 for example                  */
              /*********************************************************/
              if ((current_IE->next_IE == NULL) && (current_IE->IE_length == 0))
              {
                index = message_list_ptr[message_index].imperative_length;
              }
            }
            else if (current_IE->IE_format == LV)
            {
              current_IE->start_IE = &layer3_message[index];

              /* length field of LV : layer3_message[index] */
              if ( (layer3_message[index] > current_IE->IE_length) &&
                   (current_IE->IE_length != 0) )
              {
                MSG_GERAN_ERROR_3_G("RR Mandatory IEI out of range length:%d, max length:%d, index:0x%x",layer3_message[index],current_IE->IE_length,index);
                *message_ok = FALSE; /* Variable length is wrong, reject message */
                return;
              }

              /* advance index by value of L field + 1 (for length field itself) */
              index += layer3_message[index] + 1;

              if (index > message_length)
              {
                MSG_GERAN_ERROR_2_G("Indexing(%d,%d) beyond end of message",(int) index,(int) message_length);
                *message_ok = FALSE;
                return;
              }
            }
            else if (current_IE->IE_format == TV)
            {
              if (layer3_message[index] == current_IE->IEI)
              {
                index += current_IE->IE_length;

                if (index > message_length)
                {
                  MSG_GERAN_ERROR_2_G("Indexing(%d,%d) beyond end of message",(int) index,(int) message_length);
                  *message_ok = FALSE;
                  return;
                }
              }
            }
            else if (current_IE->IE_format == TLV)
            {
              if (layer3_message[index] == current_IE->IEI)
              {
                current_IE->start_IE = &layer3_message[index + 1];
                index += layer3_message[index + 1] + 2;

                if (index > message_length)
                {
                  MSG_GERAN_ERROR_2_G("Indexing(%d,%d) beyond end of message",(int) index,(int) message_length);
                  *message_ok = FALSE;
                  return;
                }
              }
            }
          }
          else if ((current_IE->presence == OPTIONAL_IE) ||
                   (current_IE->presence == CONDITIONAL_IE))
          {
            /*
            Optional or Conditional IEs are found in the non-imperative part of the message.
            They are of type T, TV or TLV.

            The IE configurations possible here are:

            Type 1 TV
            8  7  6  5  4  3  2  1
            |   IEI  |   value   |

            Type 2 T
            8  7  6  5  4  3  2  1
            |        IEI         |

            Type 3 TV
            8  7  6  5  4  3  2  1
            |        IEI         |
            |       value        |

            Type 4 TLV
            8  7  6  5  4  3  2  1
            |        IEI         |
            |       length       |
            |       value        |
            */

            // Test for Type 1 TV
            if (((layer3_message[index] & 0xf0) == current_IE->IEI) &&
                (current_IE->IE_length == 1))
            {
              current_IE->start_IE = &layer3_message[index];

              // IE is only one octet in length
              index++;
            }
            else

            // Test for Type 2 T
            if ((layer3_message[index] == current_IE->IEI) &&
                (current_IE->IE_length == 1))
            {
              current_IE->start_IE = NULL;

              // IE is only one octet in length
              index++;
            }
            else

            // Test for Type 3 TV or Type 4 TLV
            // Note: IE length is not checked here as it may be defined as zero for some IEs
            //       e.g. RR PACKET UPLINK ASSIGNMENT IE defines length as 3-n
            if (layer3_message[index] == current_IE->IEI)
            {
              /* Check Length (L) byte is not past the end of the message*/
              if ( (index + 1) >= message_length)
              {
                MSG_GERAN_ERROR_3_G("RR IEI=0x%x Length byte past end of msg, index=%d, msg length=%d",
                              (int) current_IE->IEI,
                              (int) index,
                              (int) current_IE->IE_length);

                *message_ok = FALSE;
                return;
              }


              current_IE->start_IE = &layer3_message[index + 1];

              if (current_IE->IE_format == TLV)
              {
                if (current_IE->IE_length != 0)
                {
                  /* length field of TLV : layer3_message[index + 1] */
                  if (layer3_message[index + 1] > current_IE->IE_length)
                  {
                    MSG_GERAN_ERROR_3_G("RR IEI=0x%x out of range length=%d, max length=%d",
                              (int) current_IE->IEI,
                              (int) layer3_message[index + 1],
                              (int) current_IE->IE_length);

                    *message_ok = FALSE;
                    return;
                  }
                }

                index += layer3_message[index + 1] + 2;
              }
              else
              if (current_IE->IE_format == TV)
              {
                index += current_IE->IE_length;
              }
              else
              {
                MSG_GERAN_ERROR_3_G("IE expected to be TV or TLV (index=%d, IEI=0x%x)",
                          (int) index,
                          (int) current_IE->IEI,
                          0);
                *message_ok = FALSE;
                return;
              }
            }
            else
            {
              if (left_IE == NULL)
              {
                left_IE = current_IE;
              }
            }
          }

          // End of message reached if index equals message length
          if (index == message_length)
          {
            break;
          }
          else
          if (index > message_length)
          {
            MSG_GERAN_ERROR_2_G("Indexing(%d,%d) beyond end of message",(int) index,(int) message_length);
            *message_ok = FALSE;
            return;
          }

          current_IE = current_IE->next_IE;

          if ((current_IE == NULL) && (index < message_length))
          {

            /****************************************************************/
            /* output a warning if the length looks suspect: this sometimes */
            /* occurs when receiving SI6 with length=18 on the Agilent 8960 */
            /****************************************************************/
            if (index == message_list_ptr[message_index].imperative_length)
            {
              MSG_GERAN_MED_3_G("Msg type 0x%02X (length=%d) ends at index %d, no more IEs.", message_type, message_length, index);
            }

            /**************************************************************/
            /* The information element received in the message is unknown */
            /* Skip it if it is a comprehension required message          */
            /**************************************************************/

            if (((byte)(layer3_message[index] & 0x80) >> 7) == 1)
            {

              /*************************************/
              /* Skip this type 1 or 2 IE          */
              /* This is an IE of one octet length */
              /*************************************/

              index++;

              // End of message reached if index equals message length
              if (index == message_length)
              {
                break;
              }
              else
              if (index > message_length)
              {
                MSG_GERAN_ERROR_2_G("Indexing(%d,%d) beyond end of message",(int) index,(int) message_length);
                *message_ok = FALSE;
                return;
              }

            }
            else
            {
              boolean check_padding = FALSE;

              /********************************/
              /* Skip this type 4 element     */
              /* The next octet is the length */
              /********************************/

              if (((byte)(layer3_message[index] & 0x70) == 0))
              {
                MSG_GERAN_ERROR_2_G("IE[%d]=%d comprehension required",(int) index,(int) layer3_message[index]);
                /***********************************************/
                /* The IE is encoded as comprehension required */
                /***********************************************/

                *error_type = RR_INVALID_MANDATORY_INFORMATION;
                *message_ok = FALSE;
                (void) rr_log_protocol_error(
                  message_type,
                  RR_PROTOCOL_ERROR_INVALID_MANDATORY_INFORMATION,
                  gas_id
                );
                return;
              }

              /* Check current octet is not padding */
              if (layer3_message[index] == 0x2B)
              {
                /* possibly a padding octet */
                check_padding = TRUE;
              }

              /* advance to what should be a length octet */
              index++;

              // End of message reached if index equals message length
              if (index == message_length)
              {
                break;
              }

              if (layer3_message[index] == 0x2B)
              {
                /* possibly a padding octet */
                check_padding = TRUE;
              }

              index += layer3_message[index];
              index++;

              // End of message reached if index equals message length
              if (index == message_length)
              {
                break;
              }
              else
              if (index > message_length)
              {
                if (check_padding)
                {
                  MSG_GERAN_MED_0_G("Additional padding found at end of message, decoding complete");
                }
                else
                {
                  MSG_GERAN_ERROR_2_G("Indexing(%d,%d) beyond end of message",(int) index,(int) message_length);
                  *message_ok = FALSE;
                  return;
                }
                break;
              }
            }

            current_IE = left_IE;
            left_IE = NULL;
          } /* if ((current_IE == NULL) && (index < message_length)) */
        } /* while (current_IE != NULL) */
      } /* else from the imperative_length test */
    } /* else from the message type test */
    else
    {
#ifdef PERLUTF
      #error code not present
#else
      MSG_GERAN_ERROR_3_G("Ignoring message %d received on invalid channel %s (0x%02x)",
        message_type,
        rr_gprs_l2_channel_name(l2_channel_type),
        l2_channel_type
      );
#endif /* PERLUTF */
      *error_type = RR_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE;
      *message_ok = FALSE;
      (void) rr_log_protocol_error(message_type, RR_PROTOCOL_ERROR_OTHER, gas_id);
    }
  }
}

#ifdef FEATURE_LTE

void rr_individual_priorities_ie_dump(
  rr_individual_priorities_ie_t * ie
)
{
  uint32 i;

  RR_NULL_CHECK_RETURN_VOID(ie);

  MSG_GERAN_MED_0("Individual Priorities IE:");
  MSG_GERAN_MED_1("  GERAN_PRIORITY: %d", ie->geran_priority);

  if (ie->utran_individual_priority_parameters_description != NULL)
  {
    MSG_GERAN_MED_0("  3G Individual Priority Parameters Description Present:");

    if (ie->utran_individual_priority_parameters_description->default_utran_priority_valid)
    {
      MSG_GERAN_MED_1("    DEFAULT_UTRAN_PRIORITY: %d", ie->utran_individual_priority_parameters_description->default_utran_priority);
    }
    else
    {
      MSG_GERAN_MED_0("    DEFAULT_UTRAN_PRIORITY: Not Present");
    }

    MSG_GERAN_MED_1("    UTRAN FDD (%d):",
                    ie->utran_individual_priority_parameters_description->num_utra_fdd_freq);

    for (i = 0;
         i < ie->utran_individual_priority_parameters_description->num_utra_fdd_freq &&
         i < LTE_IRAT_MAX_UTRAN_FREQ;
         ++i)
    {
      MSG_GERAN_MED_3("      [%02d]: UARFCN %d, Priority: %d",
              i,
              ie->utran_individual_priority_parameters_description->utra_fdd_freq_list[i].arfcn,
              ie->utran_individual_priority_parameters_description->utra_fdd_freq_list[i].priority);

    }

    MSG_GERAN_MED_1("    UTRAN TDD (%d):",
            ie->utran_individual_priority_parameters_description->num_utra_tdd_freq);

    for (i = 0;
         i < ie->utran_individual_priority_parameters_description->num_utra_tdd_freq &&
         i < LTE_IRAT_MAX_UTRAN_FREQ;
         ++i)
    {
      MSG_GERAN_MED_3("      [%02d]: UARFCN %d, Priority: %d",
              i,
              ie->utran_individual_priority_parameters_description->utra_tdd_freq_list[i].arfcn,
              ie->utran_individual_priority_parameters_description->utra_tdd_freq_list[i].priority);
    }
  }
  else
  {
    MSG_GERAN_MED_0("  3G Individual Priority Parameters Description Not Present");
  }

  if (ie->eutran_individual_priority_parameters_description != NULL)
  {
    MSG_GERAN_MED_0("  E-UTRAN Individual Priority Parameters Description Present:");

    if (ie->eutran_individual_priority_parameters_description->default_eutran_priority_valid)
    {
      MSG_GERAN_MED_1("    DEFAULT_E-UTRAN_PRIORITY: %d",
              ie->eutran_individual_priority_parameters_description->default_eutran_priority);
    }
    else
    {
      MSG_GERAN_MED_0("    DEFAULT_E-UTRAN_PRIORITY: Not Present");
    }

    MSG_GERAN_MED_1("    EUTRAN (%d):", ie->eutran_individual_priority_parameters_description->num_eutra_freq);

    for (i = 0;
         i < ie->eutran_individual_priority_parameters_description->num_eutra_freq &&
         i < LTE_IRAT_MAX_EUTRAN_FREQ;
         ++i)
    {
      MSG_GERAN_MED_3("      [%02d]: UARFCN %d, Priority: %d",
              i,
              ie->eutran_individual_priority_parameters_description->eutra_freq_list[i].arfcn,
              ie->eutran_individual_priority_parameters_description->eutra_freq_list[i].priority);
    }
  }
  else
  {
    MSG_GERAN_MED_0("  E-UTRAN Individual Priority Parameters Description Not Present");
  }

  if (ie->t3230_timeout_value_valid)
  {
    MSG_GERAN_MED_1("T3230 Timeout Value: %d", ie->t3230_timeout_value);
  }
  else
  {
    MSG_GERAN_MED_0("T3230 Timeout Value not present");
  }

}

boolean rr_individual_priorities_ie_decode(
  uint8 *ie,
  uint8 length,
  rr_individual_priorities_ie_t * dst
)
{
  gs_bit_stream_T bitstream;

  RR_NULL_CHECK_RETURN_PARAM(ie, FALSE);
  RR_NULL_CHECK_RETURN_PARAM(dst, FALSE);

  MSG_GERAN_MED_1("Decoding Individual Priorities IE (length: %d)", length);

  memset(dst, 0, sizeof(rr_individual_priorities_ie_t));

  gs_bit_stream_initialize(&bitstream, &ie[1], length);

  if (gs_bit_stream_extract_field(&bitstream, 1))
  {
    dst->geran_priority = (uint8) gs_bit_stream_extract_field(&bitstream, 3);

    /* 3G Individual Priority Parameters Description */
    if (gs_bit_stream_extract_field(&bitstream, 1))
    {
      dst->utran_individual_priority_parameters_description = GPRS_MEM_CALLOC(sizeof(rr_utran_individual_priority_parameters_description_t));
      RR_NULL_CHECK_FATAL(dst->utran_individual_priority_parameters_description);

      if (gs_bit_stream_extract_field(&bitstream, 1))
      {
        dst->utran_individual_priority_parameters_description->default_utran_priority_valid = TRUE;
        dst->utran_individual_priority_parameters_description->default_utran_priority = (uint8) gs_bit_stream_extract_field(&bitstream, 3);
      }

      /* Repeated Individual UTRAN Priority Parameters Description */
      while (gs_bit_stream_extract_field(&bitstream, 1))
      {
        if (gs_bit_stream_extract_field(&bitstream, 1) == 0)
        {
          /* FDD */

          uint32 start_index;
          uint8  utran_priority;

          start_index = dst->utran_individual_priority_parameters_description->num_utra_fdd_freq;

          while (gs_bit_stream_extract_field(&bitstream, 1))
          {
            uint16 arfcn = (uint16) gs_bit_stream_extract_field(&bitstream, 14);

            if (dst->utran_individual_priority_parameters_description->num_utra_fdd_freq < LTE_IRAT_MAX_UTRAN_FREQ)
            {
              dst->utran_individual_priority_parameters_description->utra_fdd_freq_list[
                dst->utran_individual_priority_parameters_description->num_utra_fdd_freq].arfcn = arfcn;

              ++dst->utran_individual_priority_parameters_description->num_utra_fdd_freq;
            }
          }

          utran_priority = (uint8) gs_bit_stream_extract_field(&bitstream, 3);

          for (; start_index < dst->utran_individual_priority_parameters_description->num_utra_fdd_freq; ++start_index)
          {
            dst->utran_individual_priority_parameters_description->utra_fdd_freq_list[start_index].priority = utran_priority;
          }
        }
        else
        {
          /* TDD */

          uint32 start_index;
          uint8  utran_priority;

          start_index = dst->utran_individual_priority_parameters_description->num_utra_tdd_freq;

          while (gs_bit_stream_extract_field(&bitstream, 1))
          {
            uint16 arfcn = (uint16) gs_bit_stream_extract_field(&bitstream, 14);

            if (dst->utran_individual_priority_parameters_description->num_utra_tdd_freq < LTE_IRAT_MAX_UTRAN_FREQ)
            {
              dst->utran_individual_priority_parameters_description->utra_tdd_freq_list[
                dst->utran_individual_priority_parameters_description->num_utra_tdd_freq].arfcn = arfcn;

              ++dst->utran_individual_priority_parameters_description->num_utra_tdd_freq;
            }
          }

          utran_priority = (uint8) gs_bit_stream_extract_field(&bitstream, 3);

          for (; start_index < dst->utran_individual_priority_parameters_description->num_utra_tdd_freq; ++start_index)
          {
            dst->utran_individual_priority_parameters_description->utra_tdd_freq_list[start_index].priority = utran_priority;
          }
        }
      }
    }

    /* E-UTRAN Individual Priority Parameters Description */
    if (gs_bit_stream_extract_field(&bitstream, 1))
    {


      dst->eutran_individual_priority_parameters_description = GPRS_MEM_MALLOC(sizeof(rr_eutran_individual_priority_parameters_description_t));
      RR_NULL_CHECK_FATAL(dst->eutran_individual_priority_parameters_description);
      memset(dst->eutran_individual_priority_parameters_description, 0, sizeof(rr_eutran_individual_priority_parameters_description_t));

      if (gs_bit_stream_extract_field(&bitstream, 1))
      {
        dst->eutran_individual_priority_parameters_description->default_eutran_priority_valid = TRUE;
        dst->eutran_individual_priority_parameters_description->default_eutran_priority = (uint8) gs_bit_stream_extract_field(&bitstream, 3);
      }

      /* Repeated Individual UTRAN Priority Parameters Description */
      while (gs_bit_stream_extract_field(&bitstream, 1))
      {
        uint32 start_index;
        uint8  eutran_priority;

        start_index = dst->eutran_individual_priority_parameters_description->num_eutra_freq;

        while (gs_bit_stream_extract_field(&bitstream, 1))
        {
          uint16 earfcn = (uint16) gs_bit_stream_extract_field(&bitstream, 16);
          if (dst->eutran_individual_priority_parameters_description->num_eutra_freq < LTE_IRAT_MAX_EUTRAN_FREQ)
          {
            dst->eutran_individual_priority_parameters_description->eutra_freq_list[
              dst->eutran_individual_priority_parameters_description->num_eutra_freq].arfcn = earfcn;

            ++dst->eutran_individual_priority_parameters_description->num_eutra_freq;
          }
        }

        eutran_priority = (uint8) gs_bit_stream_extract_field(&bitstream, 3);

        for (; start_index < LTE_IRAT_MAX_EUTRAN_FREQ; ++start_index)
        {
          dst->eutran_individual_priority_parameters_description->eutra_freq_list[start_index].priority = eutran_priority;
        }
      }
    }

    /* T3230 Timeout Value */
    if (gs_bit_stream_extract_field(&bitstream, 1))
    {
      dst->t3230_timeout_value_valid = TRUE;
      dst->t3230_timeout_value = (uint8) gs_bit_stream_extract_field(&bitstream, 3);
    }

    rr_individual_priorities_ie_dump(dst);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void rr_individual_priorities_ie_free_children(
  rr_individual_priorities_ie_t * item
)
{
  if (item)
  {
    if (item->utran_individual_priority_parameters_description)
    {
      GPRS_MEM_FREE(item->utran_individual_priority_parameters_description);
    }

    if (item->eutran_individual_priority_parameters_description)
    {
      GPRS_MEM_FREE(item->eutran_individual_priority_parameters_description);
    }
  }
}

#endif /* FEATURE_LTE */

/* EOF */
