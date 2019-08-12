/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Frequency Redefinition

GENERAL DESCRIPTION
  This module handles the frequency redefinition procedure

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_freq_redef.c_v   1.2   15 Feb 2002 16:39:30   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_freq_redef.c#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
05/13/09    sjw     Start relocation of dedicated mode data to a single structure
10/10/08    sjw     Implemented new FREQUENCY_REDEFINITION_IND interface
05/07/04    gc      Enhanced Trace Buffer Code
05/08/03    he      Used separate debug_str buffer for logging.
11/12/02    JAC     added arfcn 0 fix for mobile allocation
09/25/02    rmc     Fix compilation warnings
09/13/02    JC      Added a HACK around gsm_l1_get_current_frame_number();
                    Needs to be addressed !
08/26/02    rmc     Renamed MULTIBAND_SUPPORT to FEATURE_GSM_MULTIBAND
02/08/02    rmc     QXDM Cleanup
11/30/02    RMC     Added logging of channel configuration (as it changes)
===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ms.h"
#include "rr_seg_load.h"
#include "rr_freq_redef.h"
#include "rr_decode_ie.h"
#include "rr_general.h"
#include "rr_l1_send.h"
#include "rr_l2_send.h"
#include "rr_init.h"
#include "rr_l2.h"
#include "cause.h"
#include "rr_log.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include "rr_ded_config.h"
#include "rr_multi_sim.h"
#include "stringl/stringl.h"


/*===========================================================================

FUNCTION    rr_extract_cell_alloc_freq_list

DESCRIPTION Obtains a Cell Allocation Frequency List which can be used to
            generate the list of channels to be used for Frequency Redefinition.
            The list returned is sorted in ascending order of channel, but if
            ARFCN 0 is present, it will be the last entry in the list [N-1]

DEPENDENCIES

RETURN VALUE TRUE if the Cell Allocation Frequency List was obtained

SIDE EFFECTS

===========================================================================*/

static boolean rr_extract_cell_alloc_freq_list(
  frequency_redefinition_T *frd_ptr,
  ie_descriptor_T *current_IE_ptr,
  frequency_list_T *freq_list_ptr,
  const gas_id_t gas_id
)
{
  cell_channel_description_T     cell_allocation = {0};
  ARFCN_T                       *source_ptr      = NULL;
  boolean                        cell_alloc_ok   = FALSE;
  byte                           num_arfcns      = 0;
  gprs_scell_info_t             *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  if ((current_IE_ptr->start_IE != NULL) &&
      (current_IE_ptr->IEI      == CELL_CHANNEL_DESCRIPTION_IEI))
  {
    /* NW has provided a new Cell Channel Description in this message, so */
    /* copy the encoded CCD IE for use later, and decode it for use now   */
    memscpy(frd_ptr->ccd_ie,sizeof(frd_ptr->ccd_ie),current_IE_ptr->start_IE, FREQ_REDEF_CCD_IE_SIZE);
    rr_decode_cell_channel_description(
      frd_ptr->ccd_ie,
      &cell_allocation,
      &cell_alloc_ok,
      gas_id
    );

    if (cell_alloc_ok == TRUE)
    {
      /* source of the Freq List is the supplied CCD */
      source_ptr = cell_allocation.channel_number;
      num_arfcns = cell_allocation.no_of_entries;
      frd_ptr->ccd_present = TRUE;
    }
  }
  else if (scell_info_ptr->gsm.valid_CA_received != RR_CA_NOT_RECEIVED)
  {
    /* CCD is not present, so existing Cell Allocation is source of the Freq List */
    cell_alloc_ok = TRUE;
    source_ptr    = scell_info_ptr->gsm.cell_channel_description.channel_number;
    num_arfcns    = scell_info_ptr->gsm.cell_channel_description.no_of_entries;
  }
  else
  {
    /* Cell Allocation not received OTA yet, and not provided in the Freq Redef */
    MSG_GERAN_ERROR_0_G("No Cell Allocation received OTA, unable to use Freq Redef");
  }

  if ((source_ptr != NULL) && (cell_alloc_ok == TRUE) && (num_arfcns <= MAX_MA_CHANNEL))
  {
    byte index;

    /* copy the frequency list from whichever source was available */
    /* which should already be sorted in ascending order of ARFCN  */
    for (index = 0; index < num_arfcns; index++)
    {
      freq_list_ptr->channel[index] = source_ptr[index];
    }
    freq_list_ptr->no_of_items = num_arfcns;

    /* 3GPP 44.018 Table 10.5.2.21.1 states:
     * "The cell allocation frequency list is derived from the set of frequencies
     *  defined by the reference cell channel description information element.
     *  In the cell allocation frequency list the absolute RF channel numbers are
     *  placed in increasing order of ARFCN, except that ARFCN 0, if included in
     *  the set, is put in the LAST position in the list"
     */
    if ((freq_list_ptr->channel[0].num == 0) && (num_arfcns > 1))
    {
      /* ARFCN 0 is first in the list, move everything else down one place */
      for (index = 1; index < num_arfcns; index++)
      {
        freq_list_ptr->channel[index-1] = freq_list_ptr->channel[index];
      }

      /* and then replace ARFCN 0 in the last position */
      freq_list_ptr->channel[num_arfcns-1].num = 0;
      freq_list_ptr->channel[num_arfcns-1].band = SYS_BAND_EGSM_900;
    }
  }

  return(cell_alloc_ok);
}

/*===========================================================================

FUNCTION  rr_decode_frequency_redefinition

DESCRIPTION Extracts the parameters from the Information Elements which
            comprise the FREQUENCY REDEFINITION over-the-air message

DEPENDENCIES Message must have been parsed into a valid IE structure

RETURN VALUE TRUE if decoding was successful

SIDE EFFECTS

===========================================================================*/
static boolean rr_decode_frequency_redefinition(
  frequency_redefinition_T *frd_ptr,
  const gas_id_t gas_id
)
{
  channel_information_T  channel_description;
  mobile_allocation_T    mobile_allocation;
  frequency_list_T       cell_alloc_freq_list;
  ie_descriptor_T       *current_IE_ptr;
  boolean                decoding_ok;
  byte                   index;

  RR_ZERO_STRUCTURE(channel_description);
  RR_ZERO_STRUCTURE(mobile_allocation);
  RR_ZERO_STRUCTURE(cell_alloc_freq_list);

  /* get a pointer to the first IE of the OTA message */
  current_IE_ptr = rr_get_first_ie_of_message(FREQUENCY_REDEFINITION, gas_id);

  if ((current_IE_ptr == NULL) || (frd_ptr == NULL))
  {
    MSG_GERAN_ERROR_0_G("Unable to decode Frequency Redefinition message");
    return(FALSE);
  }

  /**********************************/
  /* Decode the Channel Description */
  /**********************************/
  if ((current_IE_ptr->start_IE != NULL) &&
      (current_IE_ptr->next_IE  != NULL))
  {
    decoding_ok = TRUE;
    rr_decode_channel_description(
      current_IE_ptr->start_IE,
      &channel_description,
      &decoding_ok,
      gas_id
    );
  }
  else
  {
    decoding_ok = FALSE;
  }

  /**********************************/
  /* Decode the Mobile Allocation   */
  /**********************************/
  if (decoding_ok == TRUE)
  {
    current_IE_ptr = current_IE_ptr->next_IE;
    if ((current_IE_ptr->start_IE != NULL) &&
        (current_IE_ptr->next_IE  != NULL))
    {
      rr_decode_mobile_allocation(
        current_IE_ptr->start_IE,
        &mobile_allocation
      );

      if (mobile_allocation.no_of_entries > CA_MAX_LEN)
      {
        decoding_ok = FALSE;
      }
    }
    else
    {
      decoding_ok = FALSE;
    }
  }

  /****************************/
  /* Decode the Starting Time */
  /****************************/
  if (decoding_ok == TRUE)
  {
    current_IE_ptr = current_IE_ptr->next_IE;

    if ((current_IE_ptr->start_IE != NULL) &&
        (current_IE_ptr->next_IE  != NULL))
    {
      frd_ptr->starting_time = 0;
      rr_decode_starting_time(
        current_IE_ptr->start_IE,
        &(frd_ptr->starting_time),
        &decoding_ok
      );
    }
    else
    {
      decoding_ok = FALSE;
    }
  }

  /******************************************/
  /* Decode the Cell Channel Description    */
  /* Optional IE - may be omitted by the NW */
  /******************************************/
  if (decoding_ok == TRUE)
  {
    current_IE_ptr = current_IE_ptr->next_IE;
    decoding_ok = rr_extract_cell_alloc_freq_list(
      frd_ptr,
      current_IE_ptr,
      &cell_alloc_freq_list,
      gas_id
    );
  }

  /* If everything has decoded OK so far up to this point, convert */
  /* the Mobile Allocation and Cell Allocation Frequency List into */
  /* a list of hopping frequencies that can be given to Layer 1    */
  if (decoding_ok == TRUE)
  {
    byte cafl_index;
    byte cafl_count;

    /* copy the decoded channel info parameters to the output structure */
    frd_ptr->channel_info.channel_type = channel_description.channel_type;
    frd_ptr->channel_info.hopping_flag = channel_description.hopping_flag;
    frd_ptr->channel_info.HSN          = channel_description.HSN;
    frd_ptr->channel_info.MAIO         = channel_description.MAIO;
    frd_ptr->channel_info.subchannel   = channel_description.subchannel;
    frd_ptr->channel_info.TN           = channel_description.TN;
    frd_ptr->channel_info.TSC          = channel_description.TSC;

    /* determine which channels in the Cell Alloc Freq List are part  */
    /* of the final Frequency List by scanning the list of MA indices */
    frd_ptr->channel_info.frequency_list.no_of_items = 0;
    cafl_count = cell_alloc_freq_list.no_of_items;

    for (index = 0; index < mobile_allocation.no_of_entries; index++)
    {
      cafl_index = mobile_allocation.MA_C[index];

      if ((cafl_index >= 1) && (cafl_index <= cafl_count))
      {
        /* MA indices should be in the range 1..MAX_MA_CHANNEL */
        /* so offset them into the Cell Alloc Freq List array  */
        cafl_index--;

        /* make sure the ARFCN is in a known/supported band */
        if (GET_ARFCN_BAND(cell_alloc_freq_list.channel[cafl_index]) != BAND_NONE)
        {
          frd_ptr->channel_info.frequency_list.channel[index] =
            cell_alloc_freq_list.channel[cafl_index];
          frd_ptr->channel_info.frequency_list.no_of_items++;
        }
        else
        {
          decoding_ok = FALSE;
          MSG_GERAN_ERROR_0_G("Unsupported ARFCN in Cell Channel Description");
          break;
        }
      }
      else
      {
        decoding_ok = FALSE;
        MSG_GERAN_ERROR_2_G("Mobile Allocation index out of range (%d,%d)",
                  (int)cafl_index, (int)cafl_count);
        break;
      }
    }

    /* final sanity check on the new frequency list */
    if ((frd_ptr->channel_info.frequency_list.no_of_items > 0) &&
        (frd_ptr->channel_info.frequency_list.no_of_items < MAX_MA_CHANNEL))
    {
      /* sort the frequency list, ready for use by Layer 1 */
      qsort_arfcn(
        frd_ptr->channel_info.frequency_list.channel,
        frd_ptr->channel_info.frequency_list.no_of_items,
        gas_id
      );
    }
    else
    {
      decoding_ok = FALSE;
      MSG_GERAN_ERROR_1_G("Invalid num of frequencies in hopping list for redefinition (%d)",
                frd_ptr->channel_info.frequency_list.no_of_items);
    }
  }

  return(decoding_ok);
}

/*===========================================================================

FUNCTION  rr_handle_frequency_redefinition_msg

DESCRIPTION Handles a received FREQUENCY REDEFINITION message

DEPENDENCIES

RETURN VALUE RR event for calling function

SIDE EFFECTS None

===========================================================================*/
rr_event_T rr_handle_frequency_redefinition_msg(const gas_id_t gas_id)
{
  frequency_redefinition_T *frd_ptr;
  rr_dedicated_data_T      *ded_ptr;

  /* get a pointer to the global dedicated mode data */
  ded_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (ded_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("No dedicated mode data is available for Freq Redef");
    return(EV_NO_EVENT);
  }
  else
  {
    frd_ptr = &(ded_ptr->frequency_redef_details);
  }

  /* initialise local storage for a new instance of this OTA message */
  frd_ptr->ccd_present = FALSE;

  /* decode the OTA message and store details locally */
  if (rr_decode_frequency_redefinition(frd_ptr, gas_id))
  {
    /* send the channel info and starting time to Layer 1 */
    rr_send_mph_frequency_redefinition_req(
      &(frd_ptr->channel_info),
      frd_ptr->starting_time,
      gas_id
    );

    /* L1 must send a MPH_FREQUENCY_REDEFINITION_IND in response */
  }
  else
  {
    /* unable to decode the OTA message, reply with a RR STATUS */
    rr_send_status(RR_INVALID_MANDATORY_INFORMATION, gas_id);
    MSG_GERAN_ERROR_0_G("Unable to decode Frequency Redefinition");
  }

  return(EV_NO_EVENT);
}

/*===========================================================================

FUNCTION     rr_handle_frequency_redefinition_ind

DESCRIPTION  Handles the MPH_FREQUENCY_REDEFINITION_IND from Layer 1

DEPENDENCIES

RETURN VALUE RR event for RR procedure state machine

SIDE EFFECTS May update global channel configuration and current band

===========================================================================*/
rr_event_T rr_handle_frequency_redefinition_ind(void *message_ptr, const gas_id_t gas_id)
{
  mph_frequency_redefinition_ind_T *redef_ind_ptr;
  frequency_redefinition_T         *frd_ptr;
  rr_dedicated_data_T              *ded_ptr;
  rr_event_T                        redef_event = EV_NO_EVENT;
  gprs_scell_info_t                *scell_info_ptr = rr_gprs_get_scell_info(gas_id);

  /* get a pointer to the global dedicated mode data */
  ded_ptr = rr_get_dedicated_data_ptr(gas_id);
  if (ded_ptr == NULL)
  {
    MSG_GERAN_ERROR_0_G("No dedicated mode data is available for Freq Redef");
    return(EV_NO_EVENT);
  }
  else
  {
    frd_ptr = &(ded_ptr->frequency_redef_details);
  }

  redef_ind_ptr = (mph_frequency_redefinition_ind_T *)message_ptr;

  if (redef_ind_ptr != NULL)
  {
    if (redef_ind_ptr->channels_in_same_band == FALSE)
    {
      /* failure case, determine what the response is */
      /* see 3GPP 44.018 section 3.4.5.1 for details  */
      if (redef_ind_ptr->starting_time_elapsed == TRUE)
      {
        redef_event = EV_RADIO_LINK_FAILURE;
      }
      else
      {
        rr_send_status(RR_FREQUENCY_NOT_IMPLEMENTED, gas_id);
      }
    }
    else
    {
      channel_spec_T *chan_spec_ptr;
      sys_band_T      new_band;

      /***********************************************/
      /* Copy the frequency redefinition parameters  */
      /* into the global channel_configuration store */
      /***********************************************/
      chan_spec_ptr = rr_get_channel_spec(ded_ptr, gas_id);
      if (chan_spec_ptr != NULL)
      {
        chan_spec_ptr->before_parameters_valid = TRUE;
        chan_spec_ptr->channel_info_1_before   = chan_spec_ptr->channel_info_1_after;
        chan_spec_ptr->number_of_ded_channels  = 1;
        chan_spec_ptr->channel_info_1_after    = frd_ptr->channel_info;
        chan_spec_ptr->starting_time[0]        = frd_ptr->starting_time >> 8;
        chan_spec_ptr->starting_time[1]        = frd_ptr->starting_time & 0xFF;
        chan_spec_ptr->starting_time_valid     = TRUE;
      }

      /* Set current dedicated band */
      new_band = frd_ptr->channel_info.frequency_list.channel[0].band;
      rr_set_current_dedicated_band((rr_internal_band_T)new_band, gas_id);

      /* update cell channel description in the sys info database if required */
      if (frd_ptr->ccd_present == TRUE)
      {
        cell_channel_description_T *ccd_ptr;

        ccd_ptr = &(scell_info_ptr->gsm.cell_channel_description);

        rr_decode_cell_channel_description(
          frd_ptr->ccd_ie,
          ccd_ptr,
          &(frd_ptr->ccd_present),
          gas_id
        );

        /* reuse the presence variable to set the valid_CA flag accordingly */
        if (frd_ptr->ccd_present == TRUE)
        {
          scell_info_ptr->gsm.valid_CA_received = RR_CA_RECEIVED;
        }
        else
        {
          MSG_GERAN_ERROR_0_G("CCD in Freq Redef was found to be invalid later");
        }
      }
    }
  }

  /* log the global channel configuration, whether it has changed or not */
  (void)rr_log_channel_configuration(gas_id);

  return(redef_event);
}

