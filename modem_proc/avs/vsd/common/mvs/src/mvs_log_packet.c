/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S

DESCRIPTION
  This file defines function interfaces and corresponding data
  structures for invoking multi-mode voice services.

REFERENCES
  None.

Copyright (C) 2009-2014 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/src/mvs_log_packet.c#1 $
$Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/20/13   sre     Fixed the banned QPSI memcpy and memmove APIs.
06/26/13   sud     Updated Copyright.
11/09/09    jk     Fixed AMR-WB logging for onset and speech lost
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string.h>

#include "apr_comdef.h"
#include "apr_api.h"
#include "apr_errcodes.h"
#include "mvs_voc_def.h"
#include "mvs_log_packet.h"

#include "msg.h"

#include "log.h"
#include "log_codes_umts.h"
#include "mmstd.h"

LOG_RECORD_DEFINE(LOG_VOC_REV_C)
  byte data[LEGACY_CDMA_PACKET_LENGTH];  /* vocoder data */
  byte rate;                             /* rate */
  boolean revlink;                       /* is reverse link - TRUE(TX) */
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_VOC_FOR_C)
  byte data[LEGACY_CDMA_PACKET_LENGTH];  /* vocoder data */
  byte rate;                             /* rate */
  boolean revlink;                       /* is reverse link - TRUE(TX) */
LOG_RECORD_END

/* Uplink packet data logging format: DSP form */
LOG_RECORD_DEFINE(LOG_UMTS_TX_VOCODER_PACKET_DSP_C)
  byte voc_mode;
  uint16 enc_flag_word;
  uint16 tx_pkt[LEGACY_GSM_PACKET_LENGTH_WORD];
LOG_RECORD_END

/* Downlink packet data logging format: DSP form */
LOG_RECORD_DEFINE(LOG_UMTS_RX_VOCODER_PACKET_DSP_C)
  byte voc_mode;
  uint16 dec_flag_word;
  uint16 rx_pkt[LEGACY_GSM_PACKET_LENGTH_WORD];
LOG_RECORD_END

//FR bit ordering
const uint16 param_bits[76] = {
  6,6,5,5,4,4,3,3,
  7,2,2,6,3,3,3,3,3,3,3,3,3,3,3,3,3,
  7,2,2,6,3,3,3,3,3,3,3,3,3,3,3,3,3,
  7,2,2,6,3,3,3,3,3,3,3,3,3,3,3,3,3,
  7,2,2,6,3,3,3,3,3,3,3,3,3,3,3,3,3
};

void swap (
  uint8 *src_ptr,
  uint8 *dst_ptr,
  int num_words
)
{
  uint8 tbyte;

  num_words++;
  while ( --num_words )
  {
    tbyte      = *src_ptr++;
    *dst_ptr++ = *src_ptr++;
    *dst_ptr++ = tbyte;
  }
}

void doByteSwap (
  uint8 *tempPacket,
  int fsize
)
{
  uint8 tempBuf[VOICE_PACKET_LENGTH];
  memset(tempBuf, 0, VOICE_PACKET_LENGTH);
  if ( fsize > 0 )
  {
    mmstd_memcpy(tempBuf, sizeof(tempBuf), tempPacket, fsize);
    swap(tempBuf, tempPacket, (fsize + 1) / 2);
  }
}

void FRPackPacket (
  uint16 *params,
  uint16 *codesp
)
{
  int code_wrds;

  int rem_bits,temp2,num_bits,param_size;
  int leftshift=0;

  rem_bits = 16;
  temp2 = 0;
  param_size = 76;

  for ( code_wrds = 0; code_wrds < param_size; code_wrds++ )
  {
    num_bits = param_bits[code_wrds];
    if ( rem_bits > num_bits )
    {
      rem_bits = rem_bits - num_bits;
      temp2 = temp2 | (params[code_wrds] << leftshift);
    } else
    {
      temp2 = temp2 | (params[code_wrds] ) << leftshift;
      *codesp++ = temp2;
      temp2=temp2 >> 16;
      rem_bits = 16 - (num_bits - rem_bits);
    }
    leftshift=leftshift+num_bits;
    if ( leftshift >=16 )
      leftshift=leftshift-16;
  }
  *codesp++ = temp2;
}

void FRUnPackPacket (
  uint16 *pkt,
  uint16 *codeword
)
{
  int code_wrds;
  int rem_bits;
  int num_bits;
  int bitmask;
  uint32 Acc1,Acc2;

  rem_bits = 16;
  Acc1 = (uint32)*pkt++;
  Acc1 &= 0xFFFF;

  for ( code_wrds=0;code_wrds < 76; code_wrds++ )
  {
    num_bits = param_bits[code_wrds];
    rem_bits = rem_bits - num_bits;
    bitmask = (1 << num_bits)-1;

    if ( rem_bits >= 0 )
    {
      codeword[code_wrds] =  ( uint16 ) (Acc1>>rem_bits) & bitmask;
      Acc1=Acc1 & ((1<<rem_bits)-1);
    } else
    {
      Acc2= *pkt++;
      Acc2 = Acc2 & 0xFFFF;
      rem_bits = 16 + rem_bits;
      Acc1 = (Acc1<<16)| Acc2 ;
      codeword[code_wrds] =  ( uint16 ) (Acc1>>rem_bits) & bitmask;
      Acc1=Acc1 & ((1<<rem_bits)-1);
    }
  }
}

int32_t GsmFRlogPacket (
  uint8* vocoderPacket,
  uint8* transcoded_packet
)
{
  uint16 codeword[100];
  uint16 temp1[17];
  int fsize = FR_MAX_PACKET_DATA_LENGTH+1;

  memset(codeword, 0, 100);
  memset(temp1, 0, fsize);
  mmstd_memcpy((void*)temp1, sizeof(temp1), (void*)vocoderPacket, FR_MAX_PACKET_DATA_LENGTH);
  doByteSwap((uint8*)temp1,fsize);

  FRUnPackPacket(&temp1[0], codeword);
  FRPackPacket(codeword,(uint16*)transcoded_packet) ;

  return APR_EOK;
}

uint8 MapToLegacyVocMode (
  mvs_mode_type mvs_mode
)
{
  uint8 legacyMode;

  switch ( mvs_mode )
  {
  case MVS_MODE_AMR:
    legacyMode = 0x03;
    break;
  case MVS_MODE_EFR:
    legacyMode = 0x04;
    break;
  case MVS_MODE_FR:
    legacyMode = 0x05;
    break;
  case MVS_MODE_4GV_NB:
    legacyMode = 0x06;
    break;
  case MVS_MODE_HR:
    legacyMode = 0x07;
    break;
  case MVS_MODE_IS127:
    legacyMode = 0x02;
    break;
  case MVS_MODE_IS733:
    legacyMode = 0x01;
    break;
  case MVS_MODE_AMR_WB:
    legacyMode = 0x0B;
    break;
  case MVS_MODE_4GV_WB:
    legacyMode = 0x0C;
    break;
  default:
    legacyMode = 0x00;
    break;
  }
  return legacyMode;
}

uint16 map_to_legacy_wb_frame (
  mvs_amr_frame_type frame
)
{
  uint16 legacy_frame;
  switch ( frame )
  {
  case MVS_AMR_SPEECH_GOOD:
    legacy_frame = 0x00;
    break;
  case MVS_AMR_SPEECH_DEGRADED:
    legacy_frame = 0x01;
    break;
  case MVS_AMR_ONSET:
    legacy_frame = 0x08;
    break;
  case MVS_AMR_SPEECH_BAD:
    legacy_frame = 0x03;
    break;
  case MVS_AMR_SID_FIRST:
    legacy_frame = 0x04;
    break;
  case MVS_AMR_SID_UPDATE:
    legacy_frame = 0x05;
    break;
  case MVS_AMR_SID_BAD:
    legacy_frame = 0x06;
    break;
  case MVS_AMR_NO_DATA:
    legacy_frame = 0x07;
    break;
  case MVS_AMR_SPEECH_LOST:
    legacy_frame = 0x02;
    break;
  default:
    legacy_frame = 0x07;
    break;
  }
  return legacy_frame;

}

int32_t LogCDMATxPacket (
  mvs_mode_type mvs_mode,
  mvs_voc_rate_type frame_rate,
  void *vocoderPacket
)
{
  int frameSize;
  uint8  transcoded_packet[VOICE_PACKET_LENGTH];
  LOG_VOC_REV_C_type *dsp_tx_pkt_log_ptr;
  int result = APR_EOK;

  dsp_tx_pkt_log_ptr = (LOG_VOC_REV_C_type*)log_alloc(LOG_VOC_REV_C,sizeof(LOG_VOC_REV_C_type));
  if ( dsp_tx_pkt_log_ptr!=NULL )
  {
    //0xe = erasure  for both blank rate and erasure
    dsp_tx_pkt_log_ptr->rate = ((frame_rate == MVS_VOC_ERASURE) || (frame_rate == MVS_VOC_0_RATE))?
                               0xE : (byte)frame_rate;

    dsp_tx_pkt_log_ptr->revlink = FALSE;
    memset(((void*)dsp_tx_pkt_log_ptr->data), 0,LEGACY_CDMA_PACKET_LENGTH);

    switch ( mvs_mode )
    {
    case MVS_MODE_IS127:
      frameSize = EVRC_MAX_PACKET_DATA_LENGTH;
      break;
    case MVS_MODE_IS733:
      frameSize = IS733_MAX_PACKET_DATA_LENGTH;
      break;
    case MVS_MODE_4GV_NB:
    case MVS_MODE_4GV_WB:
    case MVS_MODE_4GV_NW:
      frameSize = FOURGV_MAX_PACKET_DATA_LENGTH;
      break;
    default:
      frameSize = 0;
      result = APR_EBADPARAM;
      break;
    }

    if ( APR_EOK == result )
    {
      mmstd_memcpy((void*)transcoded_packet, sizeof(transcoded_packet), (void*)((uint8*)vocoderPacket+VOICE_PACKET_HDR_LENGTH), frameSize);
      frameSize += (frameSize%2);
      doByteSwap(&transcoded_packet[0],frameSize);
      mmstd_memcpy((void*)(dsp_tx_pkt_log_ptr->data), sizeof(dsp_tx_pkt_log_ptr->data), (void*)transcoded_packet, frameSize);
      log_commit(dsp_tx_pkt_log_ptr);
    }
  } else
  {
    result = APR_EFAILED;
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"dsp_tx_pkt_log_ptr = NULL");
  }
  return result;
}

int32_t LogCDMARxPacket (
  mvs_mode_type mvs_mode,
  mvs_voc_rate_type frame_rate,
  void *vocoderPacket
)
{
  int frameSize;
  uint8  transcoded_packet[VOICE_PACKET_LENGTH];
  LOG_VOC_FOR_C_type *dsp_rx_pkt_log_ptr;
  int32_t result = APR_EOK;

  dsp_rx_pkt_log_ptr = (LOG_VOC_FOR_C_type*)log_alloc(LOG_VOC_FOR_C,sizeof(LOG_VOC_FOR_C_type));
  if ( dsp_rx_pkt_log_ptr!=NULL )
  {
    //0xe = erasure  for both blank rate and erasure
    dsp_rx_pkt_log_ptr->rate = ((frame_rate == MVS_VOC_ERASURE) || (frame_rate == MVS_VOC_0_RATE))?
                               0xE : (byte)frame_rate;

    dsp_rx_pkt_log_ptr->revlink = TRUE;
    memset(((void*)dsp_rx_pkt_log_ptr->data), 0,LEGACY_CDMA_PACKET_LENGTH);

    switch ( mvs_mode )
    {
    case MVS_MODE_IS127:
      frameSize = EVRC_MAX_PACKET_DATA_LENGTH;
      break;
    case MVS_MODE_IS733:
      frameSize = IS733_MAX_PACKET_DATA_LENGTH;
      break;
    case MVS_MODE_4GV_NB:
    case MVS_MODE_4GV_WB:
    case MVS_MODE_4GV_NW:
      frameSize = FOURGV_MAX_PACKET_DATA_LENGTH;
      break;
    default:
      frameSize = 0;
      result = APR_EBADPARAM;
      break;
    }

    if ( APR_EOK == result )
    {
      mmstd_memcpy((void*)transcoded_packet, sizeof(transcoded_packet), (void*)((uint8*)vocoderPacket+VOICE_PACKET_HDR_LENGTH), frameSize);
      frameSize += (frameSize%2);
      doByteSwap(&transcoded_packet[0],frameSize);
      mmstd_memcpy((void*)(dsp_rx_pkt_log_ptr->data), sizeof(dsp_rx_pkt_log_ptr->data), (void*)transcoded_packet, frameSize);
      log_commit(dsp_rx_pkt_log_ptr);
    }
  } else
  {
    result = APR_EFAILED;
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"dsp_rx_pkt_log_ptr = NULL");
  }
  return result;
}

uint16 map_to_legacy_enum (
  mvs_frame_info_type mvs_frame_info
)
{
  uint16 legacy_frame;

  switch ( mvs_frame_info.amr_rate.ul_info.frame )
  {
  case MVS_AMR_SPEECH_GOOD:
    legacy_frame = 0x00;
    break;
  case MVS_AMR_SID_FIRST:
    legacy_frame = 0x01;
    break;
  case MVS_AMR_SID_UPDATE:
    legacy_frame = 0x02;
    break;
  case MVS_AMR_NO_DATA:
    legacy_frame = 0x03;
    break;
  default:
    legacy_frame = 0x03;
  }

  return  legacy_frame;
}

int32_t MappingToLegacyEncWord (
  uint16 *enc_flag,
  mvs_mode_type vocoderMode,
  mvs_frame_info_type mvs_frame_info,
  int *frameSize
)
{
  uint16 legacy_frame;
  int32_t  result = APR_EOK;
  *enc_flag = 0;

  switch ( vocoderMode )
  {
  case MVS_MODE_AMR:
    if ( (MVS_AMR_MODE_MAX == mvs_frame_info.amr_rate.ul_info.mode)||
         (MVS_AMR_FRAME_TYPE_MAX == mvs_frame_info.amr_rate.ul_info.frame) )
    {
      *enc_flag = 0;
      result = APR_EBADPARAM;
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unrecognized AMR frame ");
    } else
    {
      legacy_frame = map_to_legacy_enum(mvs_frame_info);
      *enc_flag =((legacy_frame << 0x08) | ((uint16)(mvs_frame_info.amr_rate.ul_info.mode)));
    }
    *frameSize = mvs_amr_frame_len(mvs_frame_info.amr_rate.ul_info.frame,
                                   mvs_frame_info.amr_rate.ul_info.mode);
    break;

  case MVS_MODE_AMR_WB:

    if ( (MVS_AMR_MODE_MAX == mvs_frame_info.amr_rate.ul_info.mode)||
         (MVS_AMR_FRAME_TYPE_MAX == mvs_frame_info.amr_rate.ul_info.frame) )
    {
      *enc_flag = 0;
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unrecognized AMR  wb frame ");
      result = APR_EBADPARAM;
    } else
    {
      legacy_frame = map_to_legacy_enum(mvs_frame_info);
      *enc_flag =((legacy_frame << 0x08) |
                  ((uint16)(mvs_frame_info.amr_rate.ul_info.mode - MVS_AMR_MODE_0660)));
    }
    *frameSize = mvs_amr_frame_len(mvs_frame_info.amr_rate.ul_info.frame,
                                   mvs_frame_info.amr_rate.ul_info.mode);
    break;
  case MVS_MODE_EFR:
    switch ( mvs_frame_info.gsm_rate.ul_info.frame )
    {
    case MVS_GSM_SPEECH_GOOD:
      *enc_flag = 0x01;
      break;
    case MVS_GSM_FRAME_MAX:
      result = APR_EBADPARAM;
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unrecognized EFR frame ");
      *enc_flag = 0;
      break;
    default:
      *enc_flag = 0;
    }
    *frameSize = EFR_MAX_PACKET_DATA_LENGTH;
    break;
  case MVS_MODE_FR:
    switch ( mvs_frame_info.gsm_rate.ul_info.frame )
    {
    case MVS_GSM_SPEECH_GOOD:
      *enc_flag = 0x01;
      break;
    case MVS_GSM_FRAME_MAX:
      result = APR_EBADPARAM;
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unrecognized FR frame ");
      *enc_flag = 0;
      break;
    default:
      *enc_flag = 0;
    }
    *frameSize = FR_MAX_PACKET_DATA_LENGTH;
    break;
  case MVS_MODE_HR:

    switch ( mvs_frame_info.hr_rate.ul_info.frame )
    {
    case MVS_HR_SID:
      *enc_flag = 0x10;
      break;
    case MVS_HR_SPEECH_GOOD:
      *enc_flag = 0x00;
      break;
    case MVS_HR_SPEECH_BAD:
      *enc_flag = 0x0A;
      break;
    case MVS_HR_INVALID_SID:
      *enc_flag = 0x1A;
      break;
    default:
      result = APR_EBADPARAM;
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unrecognized HR frame %x",
                                             mvs_frame_info.hr_rate.ul_info.frame);
      *enc_flag = 0;
      break;
    }
    if ( mvs_frame_info.hr_rate.ul_info.enc_err_flag == TRUE )
    {
      *enc_flag |= 0x0A;
    }
    *frameSize = HR_MAX_PACKET_DATA_LENGTH;
    break;
  default:
    result = APR_EUNSUPPORTED;
    break;
  }
  return result ;
}

int32_t MappingToLegacyDecWord (
  uint16 *dec_flag,
  mvs_mode_type vocoderMode,
  mvs_frame_info_type mvs_frame_info,
  int *frameSize
)
{
  int32_t result = APR_EOK;
  uint16 wb_frame;
  *dec_flag = 0;

  switch ( vocoderMode )
  {
  case MVS_MODE_AMR:

    if ( (MVS_AMR_MODE_MAX == mvs_frame_info.amr_rate.dl_info.mode)||
         (MVS_AMR_FRAME_TYPE_MAX == mvs_frame_info.amr_rate.dl_info.frame) )
    {
      *dec_flag = 0;
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unrecognized AMR frame ");
      result = APR_EBADPARAM;
    } else
    {
      *dec_flag =( (uint16)(mvs_frame_info.amr_rate.dl_info.frame)<< 0x08 ) |
                 ( (uint16)(mvs_frame_info.amr_rate.dl_info.mode));
    }
    *frameSize = mvs_amr_frame_len(mvs_frame_info.amr_rate.dl_info.frame,
                                   mvs_frame_info.amr_rate.dl_info.mode);
    break;

  case MVS_MODE_AMR_WB:

    if ( (MVS_AMR_MODE_MAX == mvs_frame_info.amr_rate.dl_info.mode)||
         (MVS_AMR_FRAME_TYPE_MAX == mvs_frame_info.amr_rate.dl_info.frame) )
    {
      *dec_flag = 0;
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unrecognized AMR frame ");
      result = APR_EBADPARAM;
    } else
    {
      //mape to wb legacy frame type
      wb_frame = map_to_legacy_wb_frame(mvs_frame_info.amr_rate.dl_info.frame);

      *dec_flag =( wb_frame << 0x08 ) |
                 ( (uint16)(mvs_frame_info.amr_rate.dl_info.mode - MVS_AMR_MODE_0660));
    }
    *frameSize = mvs_amr_frame_len(mvs_frame_info.amr_rate.dl_info.frame,
                                   mvs_frame_info.amr_rate.dl_info.mode);
    break;

  case MVS_MODE_EFR:
    switch ( mvs_frame_info.gsm_rate.dl_info.frame )
    {
    case MVS_GSM_BFI:
      *dec_flag = 0x02;
      break;
    case MVS_GSM_FRAME_MAX:
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unrecognized EFR frame ");
      result = APR_EBADPARAM;
      *dec_flag = 0;
      break;
    default:
      *dec_flag = 0;
    }
    if ( TRUE == mvs_frame_info.gsm_rate.dl_info.taf )
    {
      *dec_flag |= 0x04;
    }
    *frameSize = EFR_MAX_PACKET_DATA_LENGTH;
    break;
  case MVS_MODE_FR:
    switch ( mvs_frame_info.gsm_rate.dl_info.frame )
    {
    case MVS_GSM_BFI:
      *dec_flag = 0x02;
      break;
    case MVS_GSM_FRAME_MAX:
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unrecognized FR frame ");
      result = APR_EBADPARAM;
      *dec_flag = 0;
      break;
    default:
      *dec_flag = 0;
    }
    if ( TRUE == mvs_frame_info.gsm_rate.dl_info.taf )
    {
      *dec_flag |= 0x04;
    }
    *frameSize = FR_MAX_PACKET_DATA_LENGTH;
    break;
  case MVS_MODE_HR:
    switch ( mvs_frame_info.hr_rate.dl_info.frame )
    {
    case MVS_HR_SID:
      *dec_flag = 0x10;
      break;
    case MVS_HR_SPEECH_GOOD:
      *dec_flag = 0x00;
      break;
    case MVS_HR_SPEECH_BAD:
      *dec_flag = 0x02;
      break;
    case MVS_HR_INVALID_SID:
      *dec_flag = 0x08;
      break;
    default:
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unrecognized HR frame %x",
                                             mvs_frame_info.hr_rate.dl_info.frame);
      result = APR_EBADPARAM;
      *dec_flag = 0;
      break;
    }

    if ( TRUE == mvs_frame_info.hr_rate.dl_info.taf )
    {
      *dec_flag |= 0x04;
    }
    if ( TRUE == mvs_frame_info.hr_rate.dl_info.bfi )
    {
      *dec_flag |= 0x02;
    }
    if ( TRUE == mvs_frame_info.hr_rate.dl_info.ufi )
    {
      *dec_flag |= 0x01;
    }
    *frameSize = HR_MAX_PACKET_DATA_LENGTH;
    break;
  default:
    result = APR_EUNSUPPORTED;
    break;
  }
  return result;
}

int32_t LogUMTSRxPacket (
  mvs_mode_type mvs_mode,
  mvs_frame_info_type mvs_frame_info,
  void *vocoderPacket
)
{

  LOG_UMTS_RX_VOCODER_PACKET_DSP_C_type *dsp_rx_pkt_log_ptr;

  uint8  transcoded_packet[VOICE_PACKET_LENGTH];

  int32_t result = APR_EOK;
  int frameSize;
  uint16 dec_flag;

  dsp_rx_pkt_log_ptr = (LOG_UMTS_RX_VOCODER_PACKET_DSP_C_type*)log_alloc(LOG_UMTS_RX_VOCODER_PACKET_DSP_C, \
                                                                         sizeof(LOG_UMTS_RX_VOCODER_PACKET_DSP_C_type));

  if ( dsp_rx_pkt_log_ptr!=NULL )
  {
    dsp_rx_pkt_log_ptr->voc_mode =MapToLegacyVocMode(mvs_mode);
    result = MappingToLegacyDecWord(&dec_flag,mvs_mode,mvs_frame_info,&frameSize);
    if ( result == APR_EOK )
    {
      dsp_rx_pkt_log_ptr->dec_flag_word = dec_flag;
    }

    memset(((void*)dsp_rx_pkt_log_ptr->rx_pkt), 0,LEGACY_GSM_PACKET_LENGTH);
    memset((void*)transcoded_packet, 0,VOICE_PACKET_LENGTH);

    switch ( mvs_mode )
    {
    case MVS_MODE_AMR:
    case MVS_MODE_AMR_WB:
      //transcoded_packet if1 de-framing
      result = amrsup_audfmt_de_framing(&transcoded_packet[0],
                                        MVS_AMR_TO_AMRSUP_FRAME(mvs_frame_info.amr_rate.dl_info.frame),
                                        MVS_AMR_TO_AMRSUP_MODE(mvs_frame_info.amr_rate.dl_info.mode),
                                        (uint8*)vocoderPacket+VOICE_PACKET_HDR_LENGTH);
      break;
    case MVS_MODE_FR :
      result = GsmFRlogPacket((uint8*)vocoderPacket+VOICE_PACKET_HDR_LENGTH,&transcoded_packet[0]);

      break;
    case MVS_MODE_EFR:
    case MVS_MODE_HR:
      mmstd_memcpy((void*)transcoded_packet, sizeof(transcoded_packet), (void*)((uint8*)vocoderPacket+VOICE_PACKET_HDR_LENGTH), frameSize);
      break;
    default:
      result = APR_EBADPARAM;
    }

    if ( APR_EOK == result )
    {
      frameSize += (frameSize%2);
      if ( MVS_MODE_FR != mvs_mode )
      {
        doByteSwap(&transcoded_packet[0],frameSize);
      }
      mmstd_memcpy((void*)(dsp_rx_pkt_log_ptr->rx_pkt), sizeof(dsp_rx_pkt_log_ptr->rx_pkt), (void*)transcoded_packet, frameSize);
      log_commit(dsp_rx_pkt_log_ptr);
    }

  } else
  {
    result = APR_EFAILED;
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"dsp_rx_pkt_log_ptr = NULL");
  }
  return result;
}

int32_t LogUMTSTxPacket (
  mvs_mode_type mvs_mode,
  mvs_frame_info_type mvs_frame_info,
  void *vocoderPacket
)
{
  LOG_UMTS_TX_VOCODER_PACKET_DSP_C_type *dsp_tx_pkt_log_ptr;

  uint8  transcoded_packet[VOICE_PACKET_LENGTH];

  int32_t result = APR_EOK;
  int frameSize;
  uint16 enc_flag;

  dsp_tx_pkt_log_ptr = (LOG_UMTS_TX_VOCODER_PACKET_DSP_C_type*)log_alloc(LOG_UMTS_TX_VOCODER_PACKET_DSP_C, \
                                                                         sizeof(LOG_UMTS_TX_VOCODER_PACKET_DSP_C_type));

  if ( dsp_tx_pkt_log_ptr!=NULL )
  {
    dsp_tx_pkt_log_ptr->voc_mode = MapToLegacyVocMode(mvs_mode);
    result = MappingToLegacyEncWord(&enc_flag,mvs_mode,mvs_frame_info,&frameSize);
    if ( result == APR_EOK )
    {
      dsp_tx_pkt_log_ptr->enc_flag_word = enc_flag;
    }
    memset(((void*)dsp_tx_pkt_log_ptr->tx_pkt), 0,LEGACY_GSM_PACKET_LENGTH);
    memset((void*)transcoded_packet, 0,VOICE_PACKET_LENGTH);

    switch ( mvs_mode )
    {
    case MVS_MODE_AMR:
    case MVS_MODE_AMR_WB:

      result = amrsup_audfmt_de_framing(&transcoded_packet[0],
                                        MVS_AMR_TO_AMRSUP_FRAME(mvs_frame_info.amr_rate.ul_info.frame),
                                        MVS_AMR_TO_AMRSUP_MODE(mvs_frame_info.amr_rate.ul_info.mode),
                                        (uint8*)vocoderPacket+VOICE_PACKET_HDR_LENGTH);
      break;
    case MVS_MODE_FR:
      result = GsmFRlogPacket((uint8*)vocoderPacket+VOICE_PACKET_HDR_LENGTH,&transcoded_packet[0]);
      break;
    case MVS_MODE_EFR:
    case MVS_MODE_HR:
      mmstd_memcpy((void*)transcoded_packet, sizeof(transcoded_packet), (void*)((uint8*)vocoderPacket+VOICE_PACKET_HDR_LENGTH), frameSize);
      break;
    default:
      result = APR_EBADPARAM;
    }

    if ( APR_EOK == result )
    {
      frameSize += (frameSize%2);
      if ( MVS_MODE_FR != mvs_mode )
      {
        doByteSwap(&transcoded_packet[0],frameSize);
      }
      mmstd_memcpy((void*)(dsp_tx_pkt_log_ptr->tx_pkt), sizeof(dsp_tx_pkt_log_ptr->tx_pkt), (void*)transcoded_packet, frameSize);
      log_commit(dsp_tx_pkt_log_ptr);
    }

  } else
  {
    result = APR_EFAILED;
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"dsp_tx_pkt_log_ptr = NULL");
  }
  return result;
}

