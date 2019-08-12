/*==============================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/bearerCap_pack_unpack.c_v   1.0   10 May 2002 15:26:38   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/bearer_capability_utils.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/10/20   kwa     Initial version.
==============================================================================*/

/*==============================================================================

                     INCLUDE FILES FOR MODULE

==============================================================================*/
#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"
#include "err.h"
#include "ds_cap_v.h"
#include "bearer_capability_utils.h"
#include "mn_cm_exp_v.h"
#include "nasutils.h"
/*==============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

==============================================================================*/


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  pack_bearer_capability

Arguments: 
   Input
     cm_bearer_capability       - Pointer to the Bearer Capability struct containing the data to be packed.

   Output
     data_ptr           - Pointer to packed byte stream packed by this func(). 
                          This will be as per the 3GPP 24.008 spec defined format(10.5.4.5) 
                          The IEI will not be included. Packing will result in LV format
     length             - Pointer to the number of bytes packed by this func()

Return 
     Packing is success or fail

==============================================================================*/
boolean pack_bearer_capability(byte *data_ptr, cm_bearer_capability_T *cm_bearer_capability, int *length)
{
  /* Locals */
  byte *size_ptr;
  byte octet;

  *length = 0;

  
  /*  OCTET 2 - SIZE - FILL IN LATER  */
  size_ptr = data_ptr;
  data_ptr++;
  (*length)++;

  /*  OCTET 3  */
  octet = (byte)(cm_bearer_capability->radio_channel_requirement & 0x03);
  octet <<= 5; 
  octet |= cm_bearer_capability->information_transfer_capability & 0x07;
  octet |= ((cm_bearer_capability->coding_standard & 0x01) << 4);
  octet |= ((cm_bearer_capability->transfer_mode & 0x01) << 3);

  if(
#ifdef FEATURE_ALS
      ((cm_bearer_capability->information_transfer_capability & 0x07) == ITC_AUX_SPEECH)  ||
#endif
      ((cm_bearer_capability->information_transfer_capability & 0x07) == ITC_SPEECH)
    )
  {
    /* Fill-in the preferred speech versions, if any */
#ifdef FEATURE_GSM_AMR_WB
    if (cm_bearer_capability->gsm_speech_ver_pref_0 != GSM_INVALID_SPEECH_VER)
    {
      /*  EXTENSION BIT = 0  */
      *data_ptr = octet;
      data_ptr++;
      (*length)++;

      /*  OCTET 3a  */
      octet = NULL;  /* Initialize Octet3a  */
      octet = (byte)GSM_OCTET_FOR_EXTN_OF_ITC;
      octet <<= 6; 

      /* If FEATURE_GSM_AMR_WB is  defined, this is the 3a octect and CTM bit needs to be set here. */
      octet |= ((byte)(cm_bearer_capability->ctm) << 5);
      MSG_HIGH_1("CTM value given to NAS is %d",cm_bearer_capability->ctm);
      octet |= ((byte)(cm_bearer_capability->gsm_speech_ver_pref_0) & 0x0f);
    }
#endif

    if (cm_bearer_capability->gsm_speech_ver_pref_1 != GSM_INVALID_SPEECH_VER)
    {
      /*  EXTENSION BIT = 0  */
      *data_ptr = octet;
      data_ptr++;
      (*length)++;

      /*  OCTET 3a  */
      octet = NULL;  /* Initialize Octet3a  */
      octet = (byte)GSM_OCTET_FOR_EXTN_OF_ITC;
      octet <<= 6; 

      /* If FEATURE_GSM_AMR_WB is NOT defined, this is the 3a octect and CTM bit needs to be set here. */
#ifndef FEATURE_GSM_AMR_WB
      octet |= ((byte)(cm_bearer_capability->ctm) << 5);
      MSG_HIGH_1("CTM value given to NAS is %d",cm_bearer_capability->ctm);
#endif /*  !defined(FEATURE_GSM_AMR_WB) */

      octet |= ((byte)(cm_bearer_capability->gsm_speech_ver_pref_1) & 0x0f);

      if (cm_bearer_capability->gsm_speech_ver_pref_2 != GSM_INVALID_SPEECH_VER)
      {
        /*  EXTENSION BIT = 0  */
        *data_ptr = octet;
        data_ptr++;
        (*length)++;

        /*  OCTET 3b  */
        octet = NULL;  /* Initialize Octet3b  */
        octet = (byte)GSM_OCTET_FOR_EXTN_OF_ITC;
        octet <<= 6; 

        octet |= ((byte)(cm_bearer_capability->gsm_speech_ver_pref_2) & 0x0f);

        if (cm_bearer_capability->gsm_speech_ver_pref_3 != GSM_INVALID_SPEECH_VER)
        {
          /*  EXTENSION BIT = 0  */
          *data_ptr = octet;
          data_ptr++;
          (*length)++;

          /*  OCTET 3c  */
          octet = NULL;  /* Initialize Octet3c  */
          octet = (byte)GSM_OCTET_FOR_EXTN_OF_ITC;
          octet <<= 6; 

          octet |= (byte) ((byte)cm_bearer_capability->gsm_speech_ver_pref_3 & 0x0f);

#ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST 

          if (cm_bearer_capability->gsm_speech_ver_pref_4 != GSM_INVALID_SPEECH_VER)
          {
            /*  EXTENSION BIT = 0  */
            *data_ptr = octet;
            data_ptr++;
            (*length)++;

            /*  OCTET 3d  */
            octet = NULL;  /* Initialize Octet3c  */
            octet = (byte)GSM_OCTET_FOR_EXTN_OF_ITC;
            octet <<= 6; 

            octet |= ((byte)(cm_bearer_capability->gsm_speech_ver_pref_4) & 0x0f);

            if (cm_bearer_capability->gsm_speech_ver_pref_5 != GSM_INVALID_SPEECH_VER)
            {
              /*  EXTENSION BIT = 0  */
              *data_ptr = octet;
              data_ptr++;
              (*length)++;

              /*  OCTET 3e  */
              octet = NULL;  /* Initialize Octet3c  */
              octet = (byte)GSM_OCTET_FOR_EXTN_OF_ITC;
              octet <<= 6; 

              octet |= ((byte)(cm_bearer_capability->gsm_speech_ver_pref_5) & 0x0f);

            } /* ver_pref_5 */

          } /* ver_pref_4 */

#endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */

        } /* ver_pref_3 */

      } /* ver_pref_2 */

    } /* ver_pref_1 */

    octet |= 0x80;   /* Add the extension bit signalling no octet 3a,b,c,... etc.*/
    *data_ptr = octet;
    data_ptr++;
    (*length)++;
  }
  else   /* ...information_transfer_capability != ITC_SPEECH  */
  {

    /* Save OCTET 3 */
    octet |= 0x80;   /* Add the extension bit signalling no octet 3a, etc.*/
    *data_ptr = octet;

    data_ptr++;
    (*length)++;

    /*  SETUP BEARER CAPABILITY - OCTET 4  */

    octet = (byte) ((cm_bearer_capability->compression & 0x01) << 6);

    /* corrected compilation warning C4244 conversion form int to byte */
    octet |= (byte)((cm_bearer_capability->structure & 0x03) << 4);

    octet |= ((cm_bearer_capability->duplex_mode & 0x01) << 3);
    octet |= ((cm_bearer_capability->configuration & 0x01) << 2);
    octet |= ((cm_bearer_capability->NIRR & 0x01) << 1);
    octet |= (cm_bearer_capability->establishment & 0x01);

    /*  ADD EXTENSION BIT  */

    octet |= 0x80;

    *data_ptr = octet;

    data_ptr++;
    (*length)++;

    /*  OCTET 5  */

    octet = (byte) ((cm_bearer_capability->access_id & 0x03) << 5);

    /* corrected compile warning C4244 conversion from int to byte */
    octet |= (byte)((cm_bearer_capability->rate_adaption & 0x03) << 3);
    octet |= (cm_bearer_capability->signalling_access_protocol & 0x07);

    /* OCTET 5a */
    if( (cm_bearer_capability->rate_adaption  == OTHER_RATE_ADAPTION) ||
        (cm_bearer_capability->information_transfer_capability == ITC_OTHER) )
    {
      *data_ptr = octet;
      data_ptr++;
      (*length)++;

      octet = (byte)((cm_bearer_capability->other_ITC & 0x03) << 5);

      if(cm_bearer_capability->rate_adaption  == OTHER_RATE_ADAPTION)
      {
        octet |= (byte)((cm_bearer_capability->other_rate_adaption & 0x03) << 3);

        /* OCTET 5b , TS24.008 Section 10.5.4.5.1 */
        if(cm_bearer_capability->other_rate_adaption == V120)
        {
          *data_ptr = octet;
          data_ptr++;
          (*length)++;

          octet = (byte)((cm_bearer_capability->rate_adaption_header & 0x01) << 6);
          octet |= (byte)((cm_bearer_capability->multiple_frame_establishment & 0x01) << 5);
          octet |= (byte)((cm_bearer_capability->mode_of_operation & 0x01) << 4);
          octet |= (byte)((cm_bearer_capability->logical_link_identifier_negotiation & 0x01) << 3);
          octet |= (byte)((cm_bearer_capability->assignor_assignee & 0x01) << 2);
          octet |= (byte)((cm_bearer_capability->inband_outband_negotiation & 0x1) << 1);

          octet |= 0x80;
          *data_ptr = octet;
          data_ptr++;
          (*length)++;
        }
        else
        {
          octet |= 0x80;
          *data_ptr = octet;
          data_ptr++;
          (*length)++;
        }
      }
      else
      {
        octet |= 0x80;
        *data_ptr = octet;
        data_ptr++;
        (*length)++;
      }
    }
    else
    {
      octet |= 0x80;
      *data_ptr = octet;
      data_ptr++;
      (*length)++;
    }


    /*  OCTET 6  */

    /* corrected compile warning C4244 conversion from int to byte */
    octet = (byte)(cm_bearer_capability->sync_async & 0x01);

    octet |= ((cm_bearer_capability->user_information_layer_1_protocol & 0x0f)
                             << 1);
    octet |= ((cm_bearer_capability->layer_1_id & 0x03) << 5);
    /* octet |= 0x20;      layer 1 ID */

    /*  EXTENSION BIT = 0  */

    *data_ptr = octet;

    data_ptr++;
    (*length)++;

    /*  OCTET 6a  */

    /* corrected compile warning C4244 conversion form int to byte */
    octet = (byte)((cm_bearer_capability->number_of_data_bits & 0x01) << 4);

    octet |= ((cm_bearer_capability->negotiation & 0x01) << 5);
    octet |= ((cm_bearer_capability->number_of_stop_bits & 0x01) << 6);
    octet |= (cm_bearer_capability->user_rate & 0x0f);
    /*  EXTENSION BIT = 0  */

    *data_ptr = octet;

    data_ptr++;
    (*length)++;

    /*  OCTET 6b  */
    /* corrected compile warning C4244 conversion form int to byte */
    octet = (byte)(cm_bearer_capability->parity & 0x07);

    octet |= ((cm_bearer_capability->intermediate_rate & 0x03) << 5);
    octet |= ((cm_bearer_capability->NIC_on_Tx & 0x01) << 4);
    octet |= ((cm_bearer_capability->NIC_on_Rx & 0x01) << 3);


    /*  EXTENSION BIT = 0  */

    *data_ptr = octet;

    data_ptr++;
    (*length)++;

    /*  OCTET 6c  */
    /* corrected compile warning C4244 conversion form int to byte */
    octet = (byte)(cm_bearer_capability->modem_type & 0x1f);
    octet |= (cm_bearer_capability->connection_element & 0x03) << 5;

    /* IF Fixed Network User Rate defined then a Ph2+ rate is being used */
    if (cm_bearer_capability->fixed_network_user_rate != FNUR_UNDEFINED)
    {
      /* Extension bit = 0 */
      *data_ptr = octet;
      data_ptr++;
      (*length)++;

      /* Octet 6d: Phase2+ */
      octet  = cm_bearer_capability->fixed_network_user_rate;

      if (cm_bearer_capability->other_modem_type != OMT_UNDEFINED)
      {
        octet |= (byte)(cm_bearer_capability->other_modem_type << 5);
      }
      else
      {
        octet |= OMT_NONE << 5;
      }

      if (cm_bearer_capability->maximum_number_of_trafic_channels !=
            MAX_NUMBER_OF_TCH_UNDEFINED)
      {
        /* Extension bit = 0 */
        *data_ptr = octet;
        data_ptr++;
        (*length)++;

        /* Next Octet 6: Octet 6e */
        octet = cm_bearer_capability->maximum_number_of_trafic_channels;
        octet |= (byte)(cm_bearer_capability->acceptable_channel_codings << 3);

        /* IF no IEIs for Octet 6f then goto next octet */
        if ((cm_bearer_capability->user_initiated_modification_indication==
             UIMI_UNDEFINED) &&
            (cm_bearer_capability->wanted_air_interface_user_rate == 
             WAIUR_UNDEFINED))
        {
          /* ADD EXTENSION BIT - No more Octet 6's */
          octet |= 0x80;
          *data_ptr = octet;
          data_ptr++;
          (*length)++;
        }
        else
        {
          /* Extension bit = 0 */
          *data_ptr = octet;
          data_ptr++;
          (*length)++;

          /* Next Octet 6: Octet 6f */
          if (cm_bearer_capability->wanted_air_interface_user_rate !=
              WAIUR_UNDEFINED)
          {
            octet = cm_bearer_capability->wanted_air_interface_user_rate;
          }
          else
          {
            octet = WAIUR_NOT_APPLICABLE;
          }

          if (cm_bearer_capability->user_initiated_modification_indication
              != UIMI_UNDEFINED)
          {
            octet |= (byte)(cm_bearer_capability->
            user_initiated_modification_indication << 4);
          }
          else
          {
            octet |= UIMI_NOT_ALLOWED << 4;
          }

          /* ADD EXTENSION BIT - No more Octet 6's */
          octet |= 0x80;
          *data_ptr = octet;
          data_ptr++;
          (*length)++;
        }
      }
      else
      {
        /* ADD EXTENSION BIT - No more Octet 6's */
        octet |= 0x80;
        *data_ptr = octet;
        data_ptr++;
        (*length)++;
      }
    }
    else
    {
      /* ADD EXTENSION BIT - No more Octet 6's */
      octet |= 0x80;
      *data_ptr = octet;
      data_ptr++;
      (*length)++;
    }

    if(cm_bearer_capability->user_information_layer_2_protocol != 0)
    {
      /*  OCTET 7  */
      /* corrected compile warning C4244 conversion form int to byte */

      octet=(byte)(cm_bearer_capability->user_information_layer_2_protocol&0x1f);
      octet |= ((cm_bearer_capability->layer_2_id & 0x03) << 5);
      /* octet |= 0x40;      Layer 2 ID  */

      /*  ADD EXTENSION BIT  */
      octet |= 0x80;

      *data_ptr = octet;
      data_ptr++;
      (*length)++;
    }
  }

  *size_ptr = (byte)((*length) - 1);

  return TRUE;
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  unpack_bearer_capability

Arguments: 
   Input
     data_ptr               - Pointer byte stream to be unpacked by this func(). 
                              This has to be as per the 3GPP 24.008 spec defined format(10.5.4.5)
                              The IEI will not be considered. UnPacking will take in LV format
   Output
     cm_bearer_capability   - Pointer to the Bearer Capability struct which this func() will populate as a result of unpacking.
     length                 - Pointer to the number of bytes unpacked by this func()

Return 
     UnPacking is success or fail


==============================================================================*/
boolean unpack_bearer_capability(byte *data_ptr, cm_bearer_capability_T *cm_bearer_capability, int *length, byte *last_octet_data_ptr)
{
  int index;
  int ie_length;
  int max = 0;
#ifdef FEATURE_CCBS
  int cnt;
#ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
#ifdef FEATURE_GSM_AMR_WB
  gsm_speech_ver_e_type  *spch_ver[6];
  max=6;
#else
  gsm_speech_ver_e_type  *spch_ver[5];
  max=5;
#endif
#else
#ifdef FEATURE_GSM_AMR_WB
  gsm_speech_ver_e_type  *spch_ver[4];
  max=4;
#else
  gsm_speech_ver_e_type  *spch_ver[3];
  max=3;
#endif
#endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
#endif /* FEATURE_CCBS */

  if((data_ptr + max + 1)> last_octet_data_ptr)
  {
    MSG_ERROR_0("Error!!! Not enough space in read buffer");
    return FALSE;
  }

  /*  OCTET 1  */

  index = 0;
  ie_length = 0;
  *length = 0;

  
  cm_bearer_capability->present = TRUE;
  
  /*  OCTET 2  */

  ie_length = *data_ptr;
  data_ptr++;
  (*length)++;

  if(ie_length > last_octet_data_ptr - data_ptr + 1)
  {
    MSG_ERROR_1("Error!!! Incorrect ie_length or Buffer Overflow; ie_length = %d",ie_length);
    return FALSE;
  }

  /*  OCTET 3  */
  /* fixed compile warning C4244 
  cm_bearer_capability->radio_channel_requirement =(byte) (*data_ptr & 0x60) >> 5;
  cm_bearer_capability->coding_standard = (byte)(*data_ptr & 0x10) >> 4;
  cm_bearer_capability->transfer_mode = (byte)(*data_ptr & 0x08) >> 3;
  */
  cm_bearer_capability->radio_channel_requirement = *data_ptr;
  cm_bearer_capability->radio_channel_requirement &= 0x60;
  cm_bearer_capability->radio_channel_requirement >>= 5;

  cm_bearer_capability->coding_standard = *data_ptr;
  cm_bearer_capability->coding_standard &=0x10;
  cm_bearer_capability->coding_standard >>= 4;

  cm_bearer_capability->transfer_mode = *data_ptr;
  cm_bearer_capability->transfer_mode &= 0x08;
  cm_bearer_capability->transfer_mode >>=3;

  cm_bearer_capability->information_transfer_capability = (byte)(*data_ptr & 0x07);
  data_ptr++;
  index++;

#ifdef FEATURE_CCBS
  /* OCTET 3a, 3b, ... */
#ifdef FEATURE_GSM_AMR_WB
  spch_ver[0] = &cm_bearer_capability->gsm_speech_ver_pref_0;
  spch_ver[1] = &cm_bearer_capability->gsm_speech_ver_pref_1;
  spch_ver[2] = &cm_bearer_capability->gsm_speech_ver_pref_2;
  spch_ver[3] = &cm_bearer_capability->gsm_speech_ver_pref_3;

#ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST 
  spch_ver[4] = &cm_bearer_capability->gsm_speech_ver_pref_4;
  spch_ver[5] = &cm_bearer_capability->gsm_speech_ver_pref_5;
#endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
#else
  spch_ver[0] = &cm_bearer_capability->gsm_speech_ver_pref_1;
  spch_ver[1] = &cm_bearer_capability->gsm_speech_ver_pref_2;
  spch_ver[2] = &cm_bearer_capability->gsm_speech_ver_pref_3;

#ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST 
  spch_ver[3] = &cm_bearer_capability->gsm_speech_ver_pref_4;
  spch_ver[4] = &cm_bearer_capability->gsm_speech_ver_pref_5;
#endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
#endif      

  for (cnt=0; (*(data_ptr-1)&0x80)==0x00 && cnt<max; cnt++)
  {
    *spch_ver[cnt] = (gsm_speech_ver_e_type)(*data_ptr & 0x0f);
    data_ptr++;
    index++;
  }

  /* consume any left speech version indications */
  /*  --> won't do this because it might crash */
#endif /* FEATURE_CCBS */

  if(index < ie_length)
  {
    /*  THERE IS MORE DATA - OCTET 4  */
    /* fixed compile warning C4244 */
    /*
     cm_bearer_capability->structure = (byte)(*data_ptr & 0x30) >> 4;
     cm_bearer_capability->configuration = (byte)(*data_ptr & 0x04) >> 2;
     cm_bearer_capability->NIRR = (byte)(*data_ptr & 0x02) >> 1;
     cm_bearer_capability->duplex_mode = (byte)(*data_ptr & 0x08) >> 3;
    */
    cm_bearer_capability->compression = (byte) ((*data_ptr & 0x40) >> 6);
    cm_bearer_capability->structure = (byte)((*data_ptr & 0x30) >> 4);
    cm_bearer_capability->duplex_mode = (byte)((*data_ptr & 0x08) >> 3);
    cm_bearer_capability->configuration = (byte)((*data_ptr & 0x04) >>2);
    cm_bearer_capability->NIRR = (byte)((*data_ptr & 0x02) >> 1);
    cm_bearer_capability->establishment = (byte)(*data_ptr & 0x01);


    data_ptr++;
    index++;

    if(index < ie_length)
    {
      /*  STILL MORE DATA - OCTET 5  */

      cm_bearer_capability->access_id = (byte) ((*data_ptr & 0x60) >> 5);

      if (cm_bearer_capability->access_id != ACCESS_OCTET_ID)
      {
        MSG_ERROR_1("PROTOCOL ERROR: Unexpected value for access_id = %d\n",
        cm_bearer_capability->access_id);
      }

      /* fixed compile warning C4244 */
      cm_bearer_capability->rate_adaption = (byte)((*data_ptr & 0x18) >> 3);
      cm_bearer_capability->signalling_access_protocol = (byte)(*data_ptr & 0x07);

      if( ( (index + 1)< ie_length) && ((*data_ptr & 0x80) == 0)) /* 5a */
      {  
        data_ptr++;
        index++;

        cm_bearer_capability->other_ITC = (byte)((*data_ptr & 0x60) >> 5);
        cm_bearer_capability->other_rate_adaption = (byte)((*data_ptr & 0x18) >> 3);

        if( ( (index + 1) < ie_length) && ((*data_ptr & 0x80) == 0))  /* 5b */
        {
          data_ptr++;
          index++;

          cm_bearer_capability->rate_adaption_header = (byte)((*data_ptr & 0x40) >> 6);
          cm_bearer_capability->multiple_frame_establishment = (byte)((*data_ptr & 0x20) >> 5);
          cm_bearer_capability->mode_of_operation = (byte)((*data_ptr & 0x10) >> 4);
          cm_bearer_capability->logical_link_identifier_negotiation = (byte)((*data_ptr & 0x08) >> 3);
          cm_bearer_capability->assignor_assignee = (byte)((*data_ptr & 0x04) >> 2);
          cm_bearer_capability->inband_outband_negotiation = (byte)((*data_ptr & 0x02) >> 1);    
        }

        data_ptr++;
        index++;
      }
      else
      {
        data_ptr++;
        index++;
      }

      if(index < ie_length)
      {
        /*  STILL MORE DATA - OCTET 6  */

        cm_bearer_capability->layer_1_id = (byte) ((*data_ptr & 0x60) >> 5);

        if (cm_bearer_capability->layer_1_id != LAYER_1_OCTET_ID)
        {
          MSG_ERROR_1("PROTOCOL ERROR: Unexpected value for layer_1_id = %d\n",
                cm_bearer_capability->layer_1_id);
        }

        /* fixed compile warning C4244 */
        cm_bearer_capability->user_information_layer_1_protocol = 
         (byte)((*data_ptr & 0x1e) >> 1);
        cm_bearer_capability->sync_async = (byte)(*data_ptr & 0x01);

        if( ( (index + 1) < ie_length) && ((*data_ptr & 0x80) == 0))
        {
          /* More data - Octet 6a */

          /* fixed compile warnings C4244 */
          data_ptr++;
          index++;
          cm_bearer_capability->number_of_data_bits =
            (byte)((*data_ptr & 0x10) >> 4);
          cm_bearer_capability->number_of_stop_bits = 
            (byte)((*data_ptr & 0x40) >> 6);
          cm_bearer_capability->user_rate = 
            (byte)(*data_ptr & 0x0f);
          cm_bearer_capability->negotiation = 
            (byte)((*data_ptr & 0x20) >> 5);


          if( ( (index + 1) < ie_length) && ((*data_ptr & 0x80) == 0))
          /* More data - Octet 6b */
          {
            data_ptr++;
            index++;
            /* fixed compile warnings C4244 */
            cm_bearer_capability->parity =
              (byte)(*data_ptr & 0x07);
            cm_bearer_capability->intermediate_rate = 
              (byte)((*data_ptr & 0x60) >> 5);
            cm_bearer_capability->NIC_on_Tx = 
              (byte)((*data_ptr & 0x10) >> 4);
            cm_bearer_capability->NIC_on_Rx = 
              (byte)((*data_ptr & 0x08) >> 3);

            if( ( (index + 1) < ie_length) && ((*data_ptr & 0x80) == 0))
            /* more data - Octet 6c */
            {
              data_ptr++;
              index++;
              /* fixed compile warning C4244 */
              cm_bearer_capability->modem_type =
                (byte)(*data_ptr & 0x1f);
              cm_bearer_capability->connection_element =
                (byte)((*data_ptr & 0x60) >> 5);

              if ( (cm_bearer_capability->connection_element == TRANSPARENT_DATA)
                    && (cm_bearer_capability->modem_type == AUTO_TYPE_1) )
              {
                MSG_ERROR_0("PROTOCOL ERROR: modem_type indicates autobauding_type_1 when connection_element indicates transparent\n");
              }

              if ( (cm_bearer_capability->information_transfer_capability == ITC_FAX_G3)
                    && (cm_bearer_capability->modem_type != NO_MODEM) )
              {
                MSG_ERROR_0("PROTOCOL ERROR: When info_transfer_capability indicates fax_g3, modem_type should be no_modem\n");
              }

              /* These added for Phase2+ elements */
              if(( (index + 1) < ie_length) && ((*data_ptr & 0x80) == 0))
              /* more data - Octet 6d */
              {
                data_ptr++;
                index++;

                /* fixed compile warning C4244 */
                cm_bearer_capability->fixed_network_user_rate =
                  (byte)(*data_ptr & 0x1f);
                cm_bearer_capability->other_modem_type = 
                  (byte)((*data_ptr & 0x60) >> 5);

                if (cm_bearer_capability->other_modem_type == RESERVED_OMT)
                {
                  MSG_ERROR_0("PROTOCOL ERROR: Received a reserved value of other_modem_type\n");
                }

                if (cm_bearer_capability->fixed_network_user_rate > FNUR_57600)
                {
                  MSG_ERROR_0("PROTOCOL ERROR: Received a reserved value of fixed_network_user_rate\n");
                }


                if( ( (index + 1) < ie_length) && ((*data_ptr & 0x80) == 0))
                /* more data - Octet 6e */
                {
                  data_ptr++;
                  index++;

                  cm_bearer_capability->maximum_number_of_trafic_channels =
                    (byte)(*data_ptr & 0x07);
                  cm_bearer_capability->acceptable_channel_codings = 
                    (byte)((*data_ptr & 0x78) >> 3);

                  /* Note: 
                     In the network to MS direction, 
                     max_number_of_trafic_channels &
                     acceptable_channel_codings are spare
                  */


                  if( ( (index + 1) < ie_length) && ((*data_ptr & 0x80) == 0))
                  /* more data - Octet 6f */
                  {
                    data_ptr++;
                    index++;

                    cm_bearer_capability->wanted_air_interface_user_rate = 
                      (byte)(*data_ptr & 0x0f);

                    /* fixed compile warning C4244 */
                    cm_bearer_capability->user_initiated_modification_indication = 
                      (byte)((*data_ptr & 0x70) >> 4);

                    if( ( (index + 1) < ie_length) && ((*data_ptr & 0x80) == 0))
                    /* more data - Octet 6f */
                    {
                      data_ptr++;
                      index++;

                      cm_bearer_capability->acceptable_channel_coding_extended  = 
                        (byte)((*data_ptr & 0xf0) >> 4);

                      /* fixed compile warning C4244 */
                      cm_bearer_capability->asymmetry_indication = 
                        (byte)((*data_ptr & 0x0C) >> 2);
                    }
                  }
                }
              }
            }
          }
        }

        data_ptr++;
        index++;

        if(index < ie_length)
        {
          /*  STILL MORE DATA - OCTET 7  */
          /* fixed compile warning C4244 */
          cm_bearer_capability->user_information_layer_2_protocol =
            (byte)(*data_ptr & 0x1f);
          cm_bearer_capability->layer_2_id = 
            (byte) ((*data_ptr & 0x60) >> 5);

          if (cm_bearer_capability->layer_2_id != LAYER_2_OCTET_ID)
          {
            MSG_ERROR_1("PROTOCOL ERROR: Unexpected value for layer_2_id = %d\n",
               cm_bearer_capability->layer_2_id);
          }
        }  /* end of if (index... for octet 7 */
      }  /* end of if (index... for octet 6  */
    }  /* end of if (index... for octet 5  */
  }  /* end of if (index... for octet 4  */
  *length = (*length) + ie_length;
  return TRUE;

}
/*==============================================================================

              
FUNCTION NAME

  init_bearear_capability

Arguments: 
   Input
  
   Output
     cm_bearer_capability  - Pointer to the Bearer Capability struct which this func() will initialise.
  

Return 
     
==============================================================================*/

void init_bearear_capability(cm_bearer_capability_T *cm_bearer_capability)
{

  cm_bearer_capability->present = TRUE;

   /* octet 3 */
   cm_bearer_capability->radio_channel_requirement = 1; /* FULL_RATE_ONLY */
   cm_bearer_capability->coding_standard = 0; /* coding_standard : GSM */
   cm_bearer_capability->transfer_mode =  0; /* transfer mode : circuit mode */
   cm_bearer_capability->information_transfer_capability = 0; /* information transfer capability : speech */


   /* octet 3a */
   cm_bearer_capability->ctm = CTM_NOT_SUPPORTED;

#if defined(FEATURE_GSM_AMR_WB)
   /* octet 3a */
   cm_bearer_capability->gsm_speech_ver_pref_0 = GSM_INVALID_SPEECH_VER;
#endif /* defined(FEATURE_GSM_AMR_WB) || defined(FEATURE_MM_SUPERSET) */

   cm_bearer_capability->gsm_speech_ver_pref_1 = GSM_INVALID_SPEECH_VER;

   /* octet 3b */
   cm_bearer_capability->gsm_speech_ver_pref_2 = GSM_INVALID_SPEECH_VER;

   /* octet 3c */
   cm_bearer_capability->gsm_speech_ver_pref_3 = GSM_INVALID_SPEECH_VER;

#if defined(FEATURE_GSM_EXT_SPEECH_PREF_LIST)
   /* octet 3d */
   cm_bearer_capability->gsm_speech_ver_pref_4 = GSM_INVALID_SPEECH_VER;

   /* octet 3e */
   cm_bearer_capability->gsm_speech_ver_pref_5 = GSM_INVALID_SPEECH_VER;
#endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST || FEATURE_MM_SUPERSET */ 

   /* octet 4 */
   cm_bearer_capability->compression = 0x00; /* compression:data compression not allowed*/
   cm_bearer_capability->structure = 0x00; /* structure: service data unit integrity */
   cm_bearer_capability->duplex_mode = 0x00; /* half duplex */
   cm_bearer_capability->configuration = 0x00; /* point-to-point */
   cm_bearer_capability->NIRR = 0x00; /* No meaning is associated with this value*/
   cm_bearer_capability->establishment = 0x00; /* demand */

   /* octet 5 */
   cm_bearer_capability->access_id = 0x00;  /* octet identifier */
   cm_bearer_capability->rate_adaption = 0x00; /* no rate adaption */
   cm_bearer_capability->signalling_access_protocol = 0x00; /* I.440/450 */

   /* octet 5a */                    
   cm_bearer_capability->other_ITC = 0x00; /* restricted digital information */
   cm_bearer_capability->other_rate_adaption = 0x00; /* V.120*/

   /* octet 5b */
   cm_bearer_capability->rate_adaption_header = 0x00; /* Rate adaption header not included */
   cm_bearer_capability->multiple_frame_establishment = 0x00;
   cm_bearer_capability->mode_of_operation = 0x00;
   cm_bearer_capability->logical_link_identifier_negotiation = 0x00;
   cm_bearer_capability->assignor_assignee = 0x00;
   cm_bearer_capability->inband_outband_negotiation = 0x00;
    
   /* octet 6 */
   cm_bearer_capability->layer_1_id = 0x01;  
   cm_bearer_capability->user_information_layer_1_protocol = 0x00;
   cm_bearer_capability->sync_async = 0x00;

   /* octet 6a */
   cm_bearer_capability->number_of_stop_bits = 0x00;
   cm_bearer_capability->negotiation = 0x00;
   cm_bearer_capability->number_of_data_bits = 0x00;
   cm_bearer_capability->user_rate = 0x00;
     
   /* octet 6b */
   cm_bearer_capability->intermediate_rate = 0x00;
   cm_bearer_capability->NIC_on_Tx = 0x00;
   cm_bearer_capability->NIC_on_Rx = 0x00;
   cm_bearer_capability->parity = 0x00;

   /* octet 6c */
   cm_bearer_capability->connection_element = 0x00;
   cm_bearer_capability->modem_type = 0x00;

   /* These are used for Phase2+ (i.e.14400bd) */
   /* octet 6d */
   cm_bearer_capability->other_modem_type = OMT_UNDEFINED;
   cm_bearer_capability->fixed_network_user_rate = FNUR_UNDEFINED;
   
   /* octet 6e */
   cm_bearer_capability->acceptable_channel_codings = ACC_NONE;
   cm_bearer_capability->maximum_number_of_trafic_channels = MAX_NUMBER_OF_TCH_UNDEFINED;

   /* octet 6f */
   cm_bearer_capability->user_initiated_modification_indication = UIMI_UNDEFINED;
   cm_bearer_capability->wanted_air_interface_user_rate = WAIUR_UNDEFINED;
   
   /* octet 6g */
   cm_bearer_capability->acceptable_channel_coding_extended = 0x00;
   cm_bearer_capability->asymmetry_indication = 0x00; /* EDGE channel coding */
   
   /* octet 7 */
   cm_bearer_capability->layer_2_id = 0x02;
   cm_bearer_capability->user_information_layer_2_protocol = 0x00;

   
   /* Used only for logging!! */
   cm_bearer_capability->mt_fall_back_rate = 0;   
}
