/*===========================================================================
  FILE: tracer_stp.c

  OVERVIEW: Tracer STP interface. Writes to STM to generate STP packets.

  NOTE:
    Includes workaround to a HW issue, coded for 32/64/128-bit writes to STM.

  DEPENDENCIES:

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tracer/src/tracer_stp.c#1 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "stringl/stringl.h"   // memset (not available Venus, LPASS)

#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "tracer_event_ids.h"
#include "tracer_stp.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
DALDEVICEHANDLE hSTM;  /**< Handle to STM for trace output. */

/*---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
//=========================================================================//
#if (TRACER_OST_TOKEN_START == TRACER_OST_TOKEN_STARTWA)
void get_align_info (align_info_s *ainfo, uint32 msgAddr)
{
   // Tracer's working buffer at 16-byte aligned boundary
   ainfo->pAligned = (uint32*)((uint32)(ainfo->buffer + 3) &
                             (~((uint32)0x0F)));
   // Msg buffer's 16-byte aligned boundary (at or before start of data)
   ainfo->pMsgBoundary = (uint32*)(msgAddr &
                         (~((uint32)(TRACER_WA_ALIGNBYTE - 1))));

   // Bytes from msg buffer's aligned boundary to start of data.
   ainfo->offset = msgAddr & (TRACER_WA_ALIGNBYTE - 1);

   ainfo->nonAlignBytes =
      (ainfo->offset) ? (TRACER_WA_ALIGNBYTE - ainfo->offset) : 0;

   return;
}
#endif

//=========================================================================//
tracer_return_enum_t tracer_stp_attach_stm(void)
{
   return ( (DAL_SUCCESS == DAL_STMTraceDeviceAttach(
            DALDEVICEID_STM_TRACE, &hSTM)) ?
            TRACER_RETURN_SUCCESS : TRACER_RETURN_ERROR );
}
DALDEVICEHANDLE tracer_stp_get_hStm(void)
{
   return hSTM;
}

//=========================================================================//
void tracer_stp_send_data(STMTracePortType port,
                          STMTraceDataType opt,
                          uint32 uMsgLen,
                          const uint8 *pMsgData)
{

#define STP_SEND_ONE(_nBytes, _nBits) \
   STP_SEND_DATA(_nBits, port, opt, (* (uint##_nBits *)pMsgData)); \
   pMsgData += _nBytes; \
   uMsgLen -= _nBytes;

#define STP_SEND_ALL(_nBytes,_nBits) \
   while (uMsgLen >= _nBytes)\
   { \
      STP_SEND_ONE(_nBytes, _nBits); \
   }

   if (NULL != hSTM)
   {

// Normal buffer alignment to 32-bit boundary
#if ((TRACER_OST_FORMAT == TRACER_OST_FMT_SIMPLE) || (TRACER_OST_FORMAT == TRACER_OST_FMT_BASIC))
      // 32-bit aligned msg buffer for STM _Data32 calls.
      if (((uint32)pMsgData & 0x01) && (1 <= uMsgLen))
      {
         STP_SEND_ONE(1, 8);
      }
      if (((uint32)pMsgData & 0x02) && (2 <= uMsgLen))
      {
         STP_SEND_ONE(2, 16);
      }

      STP_SEND_ALL(4, 32);
      STP_SEND_ALL(2, 16);
      STP_SEND_ALL(1, 8);

#else // Workaround:  RW align to 32/64/128-bit boundary

      int idx;
      uint32 mask[4] =
      {  0xFFFFFFFF,
         0x00FFFFFF,
         0x0000FFFF,
         0x000000FF
      };
      align_info_s ais;

      get_align_info(&ais, (uint32)pMsgData);

      if (ais.offset)   // Alignment needed.
      {
         // Read to align then send.
         // Read into a 32-bit register then output....
         while (TRACER_WA_ALIGNBYTE <= uMsgLen)
         {
            // For every n-bytes.
            for (idx = 0; idx < (TRACER_WA_ALIGNBYTE >> 2); idx++ )
            {
               ais.pAligned[idx] =
                  ((*(ais.pMsgBoundary) >> (ais.offset << 3)) & mask[ais.offset]) |
                  ((*((ais.pMsgBoundary + 1)) << ((4 - ais.offset) << 3)) & (~mask[ais.offset]));
               uMsgLen -= 4;
               ais.pMsgBoundary ++;
            }
#if (TRACER_WA_ALIGNBYTE == 16)
            STP_SEND_DATABLK(128, port, opt | TRACE_DATA_TIMESTAMPED,
                             ais.pAligned);
#elif (TRACER_WA_ALIGNBYTE == 8)
            STP_SEND_DATABLK(64, port, opt, ais.pAligned);
#else
            STP_SEND_DATA(32, port, opt, *(ais.pAligned));
#endif

         }
         pMsgData = ((uint8*)(ais.pMsgBoundary)) + ais.offset;
      }
      else
      {
         // Aligned portion of data
#if (TRACER_WA_ALIGNBYTE == 16)
         while (TRACER_WA_ALIGNBYTE <= uMsgLen)
         {
            STP_SEND_DATABLK(128, port, opt | TRACE_DATA_TIMESTAMPED,
                             (uint32*)pMsgData);
            pMsgData += TRACER_WA_ALIGNBYTE;
            uMsgLen -= TRACER_WA_ALIGNBYTE;
         }

#elif (TRACER_WA_ALIGNBYTE == 8)
         while (TRACER_WA_ALIGNBYTE <= uMsgLen)
         {
            STP_SEND_DATABLK(64, port, opt, (uint32*)pMsgData);
            pMsgData += TRACER_WA_ALIGNBYTE;
            uMsgLen -= TRACER_WA_ALIGNBYTE;
         }
#else
         STP_SEND_ALL(4, 32);
#endif
      }

      // Handle remainder client data.

      // Form 32/64/128-bit buffer with remaining bytes + padding.
      memset(ais.buffer, 0, sizeof(ais.buffer));
      while (0 < uMsgLen)
      {
         *(ais.pAligned) = 0;
         for (idx = 0; (TRACER_WA_ALIGNBYTE > idx) && (0 < uMsgLen); idx++)
         {
            *(((uint8*)(ais.pAligned)) + idx) = *pMsgData;
            pMsgData++;
            uMsgLen--;
         }
#if (TRACER_WA_ALIGNBYTE == 16)
         STP_SEND_DATABLK(128, port, opt | TRACE_DATA_TIMESTAMPED,
                          ais.pAligned);
#elif (TRACER_WA_ALIGNBYTE == 8)
         STP_SEND_DATABLK(64, port, opt, ais.pAligned);
#else
         STP_SEND_DATA(32, port, opt, *(ais.pAligned));
#endif
      }

#endif  // Workaround
   }
   return;
}

//=========================================================================//
#define PACK_EVENTID(_id) (((((uint32)(_id)) << 8 ) & 0xFFFFFF00) | \
                           (TRACER_OST_TOKEN_SWEVENT & 0x0FF) )

void tracer_stp_send_event(STMTracePortType port,
                           tracer_event_id_t event_id)
{
   STP_SEND_DATA32(port,
                   (TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED),
                   (uint32)PACK_EVENTID(event_id));
}

//=========================================================================//
void tracer_stp_send_event_wa(STMTracePortType port,
                              tracer_event_id_t event_id)
{
   uint8 buff[32];
   uint8 *alignBuff;
   uint32 code_len;

    alignBuff= (uint8*) (((uint32)buff + 15) & (~0x0F));

   *((uint32*)alignBuff) = ((event_id << 8) & 0xFFFFFF00) |
                           (TRACER_OST_TOKEN_SWEVENT & 0x0FF);
   code_len = 4;

#if (TRACER_WA_ALIGNBYTE == 8)
   *((uint32*) (alignBuff + 4)) = (TRACER_OST_TOKEN_END & 0x0FF);
   code_len = 8;
#elif (TRACER_WA_ALIGNBYTE == 16)
   *((uint32*) (alignBuff +  4)) = (TRACER_OST_TOKEN_END & 0x0FF);
   *((uint32*) (alignBuff +  8)) = (TRACER_OST_TOKEN_END & 0x0FF);
   *((uint32*) (alignBuff + 12)) = (TRACER_OST_TOKEN_END & 0x0FF);
   code_len = 16;
#endif

   tracer_stp_send_data(port,
      (TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED),
      code_len, alignBuff);
   return;
}

