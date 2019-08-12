/*=============================================================================
**
**
**                        D A T A   S E R V I C E S
**
**           G S M  C I R C U I T - S W I T C H E D  D A T A
**
**                   F A X  P R O T O C O L  S T A C K
**
**                     T 3 1  H D L C  S E R V I C E S
**
** DESCRIPTION:
**
**   This file contains source code comprising the HDLC Services interface
**   used by the T31 layer for performing zero-bit insertion and deletion
**   operations on HDLC facsimile data.
**
** EXTERNALIZED FUNCTIONS:
**
**    dst31hdlci_zbi()
**      - Invoked by T31 to perform zero-bit insertion operations on 
**        HDLC facsimile data.
**
**    dst31hdlci_zbd()
**      - Invoked by T31 to perform zero-bit deletion operations on 
**        HDLC facsimile data.
**
**    dst31hdlci_zbd_init()
**      - Invoked by T31 to initialize ZBD processing at the start of
**        each initial entry into the +FRH state.

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (c) 2002 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved
                     Qualcomm Proprietary

  This software is controlled by the United States Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.


=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dst31hdlc.c#1 $

when      who   what, where, why
--------  ---   ---------------------------------------------------------------
02/09/06  AR    Lint corrections.
05/30/04  AR    Lint changes
02/10/03  RLP   Spain IOT changes.
                  - add support to limit the number of bytes processed during
                    ZBD per 20ms interval.
                  - Pushback last found flag to make it easier to find the 
                    start of the next frame during ZBD.
01/14/03  RLP   Code review changes.
11/15/02  RLP   Changes for GCF certification support.
11/04/02  RLP   Nortel IOT updates from Stage 2 testing.
09/11/02  RLP   MMO2 IOT changes.
07/12/02  RLP   Changes to maintain TREX compatibilty.
06/21/02  RLP   First pass changes to get fax working with Surf/Racal.
03/14/02  RLP   Initial release.
=============================================================================*/

/*=============================================================================
** INCLUDES
**===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_GCSD_FAX
#include "amssassert.h"
#include "err.h"
#include "dsm.h"
#include "dst31i.h"
#include "data_msg.h"
#include "dst31hdlci.h"

/*=============================================================================
** REGIONAL VARIABLES
**============================================================================*/

/* The following variables are used during ZBD processing to maintain state
** context while extracting HDLC frame data received from the lower layers of
** the FPS.  Multiple invocations of dst31hdlci_zbd() will likely be required 
** during +FRH state processing in order to build a complete HDLC frame.  Thus,
** reentrancy considerations are required to maintain context as data is added
** to the output frame buffer.  These variables are used for that purpose.
*/

/* Used to indicate the state of the current output byte */
LOCAL uint8 zbd_obyte_start    = 0;

/* Used to indicate the number of bits assigned to the current output byte */
LOCAL uint8 zbd_ofill_start    = 0;

/* Used to indicate how far to shift in the next input bit to the output byte */
LOCAL uint8 zbd_oshift_start   = 0;

/* Used to indicate how many consecutive one bits has been found so far */
LOCAL uint8 zbd_ones_cnt_start = 0;

/* Used to track the current 1bit mask value */
LOCAL uint8 zbd_curr_1bit_mask = 0;

/* Used to track the previous 1bit mask value */
LOCAL uint8 zbd_prev_1bit_mask = 0;

/* Used to track how many opening flags are received */
LOCAL uint8 zbd_flag_cnt = 0;

/* Used to indicate how many bytes are currently in the output frame buffer */
LOCAL uint16 zbd_ocnt_start     = 0;

/* Used to indicate the current frame parsing state */
LOCAL dst31_frh_data_states_enum_type zbd_state = DST31_FRH_HDLC_FIND_FLAG_STATE;

/* Used to keep track of the previous byte during ZBD processing */
LOCAL uint8 zbd_prev_byte = 0x0;

/* Used to mask off each bit during ZBD processing */
LOCAL uint8 zbd_ibyte_bit_mask[] = 
{
  0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

/*=============================================================================
** EXPORTED FUNCTIONS USED SOLELY BY T31
**===========================================================================*/

/*=============================================================================
** FUNCTION: dst31hdlci_zbi
**
** DESCRIPTION:
**   This function performs HDLC based zero-bit insertion on the specified
**   data.  ZBI is used to insure that the HDLC flag field value of 0x7E
**   (01111110) will never occur within the address, control, info, or
**   FCS fields of the frame itself.  The flag field is used to delineate
**   HDLC frame boundaries, and thus, cannot appear anywhere else in the
**   frame body.  A bit-by-bit search of the frame is performed and an extra 
**   '0' bit be inserted into the data stream after any sequence of five '1' 
**   bits has been detected between two flags.
**
**   A valid HDLC frame has the following format:
**
**   <flag byte(s)><address byte><control byte><data bytes><16bit-FCS><flag(s)>
**
**   Inserting a bit into the data stream will in affect nullify byte boundaries
**   for all subsequent data bytes.  The data after ZBI is performed should be
**   treated as a pure bit stream.  This has direct ramifications on the 0-bit
**   deletion operation described below (see dst31hdlci_zbd() for details).
**
**   NOTE: for this implementation the T31 layer will have already included
**   closing flags with the frame data.  When a flag byte is detected, it will
**   be bit-shifted into the data stream WITHOUT 0-bit insertion.  
**
** PARAMETERS:
**   ibuf == pointer to the input data buffer on which to perfrom ZBI
**   ibuf_len == number of bytes in ibuf
**   obuf == pointer to the output data buffer to return the ZBI data
**
** RETURNS:
**   numbers of bytes placed into obuf
**
** SIDE EFFECTS:
**   Must create a copy of the original data as additional bits are inserted
**   into the data. 
**===========================================================================*/
uint16 dst31hdlci_zbi
(
  uint8 *   ibuf, 
  uint16    ibuf_len, 
  uint8 *   obuf
)
{
  register  uint8     ibyte;
  register  uint8     ishift_cnt;
  register  uint8     obyte;
  register  uint8     oshift_cnt;
  boolean   zbi_ready;
  uint8     bit;
  uint8     bit_idx;
  uint8     ofill_cnt;
  uint8     ones_cnt;
  uint8     pullup_shift_cnt;
  uint8     zbi_shift_cnt;
  uint16    byte_idx;
  uint16    ocnt;


  DATA_MSG1_LOW("HDLC-ZBI on %d bytes of data", ibuf_len);

  /* Initialize the local variables */
  obyte             = 0;
  ocnt              = 0;
  ofill_cnt         = 0;
  ones_cnt          = 0;
  oshift_cnt        = 0;
  pullup_shift_cnt  = 0;
  zbi_shift_cnt     = 0;
  zbi_ready         = FALSE;
 
  /* Ok we need to loop through each input byte */
  for (byte_idx = 0; byte_idx < (MIN(DST31_MAX_IO_DATA_SZ, ibuf_len)); byte_idx++)
  {
    ibyte = ibuf[byte_idx];
    ishift_cnt = 0;

    /* Now for each input byte we extract individual bits and start
    ** counting the consecutive 1-bits.  When we detect 5 1-bits in
    ** a row we have to insert a 0-bit.  The tricky part is when we
    ** detect 5 1-bits which cross over byte boundaries.  We have to
    ** keep track of which bit is being processing for the current
    ** input byte and which bit location it needs to be placed in
    ** the output byte.  That is what all the shift counts are used
    ** for.
    */

#if 0 /* Debug use only */
    DATA_MSG2_LOW("HDLC-ZBI processing byte[%2d]=0x%x\n", byte_idx,ibyte);
#endif

    for (bit_idx = 1; bit_idx <= DST31_MAX_BITS_PER_BYTE; bit_idx++)
    {
      /* 0-bits are implied by the output byte being initialized to 0.
      ** 1-bits are literally inserted into the output byte.
      */
      ofill_cnt++;

      if ((bit = (ibyte & (0x80 >> ishift_cnt))) > 0)
      {
        /* We have found a 1-bit so we add it to the output byte */
        obyte |=  (bit >> oshift_cnt); 

        /* Now if we have 5 consecutive 1-bits we set a flag to 
        ** indicate that we must first insert a 0-bit into the
        ** bit stream...that is as long as we don't actually find
        ** a closing flag in the frame.  If a closing flag is found
        ** we simply let it get bit shifted into the output stream.
        */
        if ((ibyte != DST31_HDLC_FLAG) &&
            (++ones_cnt == DST31_MAX_CONSECUTIVE_ONE_BITS))
        {
          zbi_ready = TRUE;
          ones_cnt = 0;

          if (ofill_cnt < DST31_MAX_BITS_PER_BYTE)
          {
            /* Still room to insert the 0-bit in the current output byte */
            zbi_ready = FALSE;
            zbi_shift_cnt++;
            oshift_cnt += zbi_shift_cnt;
            ofill_cnt++;
          }
        }
      }
      else
      {
        if (ones_cnt > 0) 
        {
          /* We need to restart counting the 1 bits */
          ones_cnt = 0;
        }
      } 

      /* Check to see if the output byte is full and if so add it
      ** to the output buffer.  We then need to move up any remaining
      ** input byte bits so they can be processed as part of the 
      ** next output byte.
      */
      if (ofill_cnt == DST31_MAX_BITS_PER_BYTE)
      {
        obuf[ocnt++] = obyte;
        zbi_shift_cnt = ofill_cnt = obyte = 0;

        /* We haven't processed the whole input byte yet so shift up
        ** the remaining bits.  This is done to allow a consistent
        ** mechanism for extracting the bits and putting them into
        ** the output byte (i.e. we always process from the msb to lsb).
        */
        if (bit_idx < DST31_MAX_BITS_PER_BYTE)
        {
          ibyte <<= bit_idx;              /*lint !e734 */
          pullup_shift_cnt = DST31_MAX_BITS_PER_BYTE - bit_idx;
          oshift_cnt = ishift_cnt = 0;
        }
        else 
        {
          /* We need to make sure we shift passed the bits that 
          ** were just added.
          */
          oshift_cnt = pullup_shift_cnt;

          /* Since we will have finished off the current input
          ** byte we can reset the pullup counter.
          */
          if (pullup_shift_cnt > 0)
          {
            pullup_shift_cnt = 0;
          }
        }
      }
      else
      {
        /* Get ready to extract the next input bit */
        ishift_cnt++;
      }
  
      /* At this point if we couldn't insert the 0-bit because the buffer
      ** was full, we have to account for it on the next output byte.
      */
      if (zbi_ready == TRUE)
      {
        pullup_shift_cnt++;
        oshift_cnt = 1;
        ofill_cnt = 1;
        zbi_ready = FALSE;
      }
    } /* for bit_idx */

    /* Placement of the input bit into the output byte is determined
    ** by calculating how many bits were pulled up from the input byte
    ** and how many 0-bits needed to be inserted (2 at most) for the
    ** current output byte.  The pullup bits comes from those bits 
    ** remaining in the current input byte that couldn't be put into
    ** the current output byte because the output byte was full.  
    ** Those bits are then added to the next output byte before the
    ** next input byte is processed -- thus we need to skip over them
    ** when placing the next input bits.
    */
    oshift_cnt = zbi_shift_cnt + pullup_shift_cnt;
  } /* for i */

  /* Make sure to include the final obyte */
  if (obyte)
  {
    obuf[ocnt++] = obyte;
  }

  DATA_MSG1_LOW("HDLC-ZBI output buffer len=%d", ocnt);

  return (ocnt); 
} /* dst31hdlci_zbi */


/*=============================================================================
** FUNCTION: dst31hdlci_zbd
**
** DESCRIPTION:
**   This function performs HDLC based zero-bit deletion on the specified
**   data.  ZBD is used to remove additional '0' bits that were added to
**   the HDLC data stream as a result of zero-bit insertion.
**
**   A valid HDLC frame has the following format:
**
**   <flag byte(s)><address byte><control byte><data bytes><16bit-FCS><flag(s)>
**
**   ZBD is handled in a more detailed manner than ZBI due to the fact that
**   opening and closing flag bytes do NOT have to fall on byte boundaries.
**   Given that, the ZBD becomes more convoluted in that not only does it
**   have to detect and remove the extra 0-bits, it also has to search for
**   and detect flag bytes to be able to extract individual frames from the
**   bit stream.
**
**   While this function does perform ZBD as required by the HDLC protocol,
**   it has been adapted to work specifically for operating on facsimile HDLC
**   frames received as part of the ITU-T T.31 protocol during processing of
**   a +FRH (receive HDLC) AT command.
**
**   For the purposes of T31, OTA data is received via the TPS, placed into
**   a DSM item, and put on the FPS DL watermark.  When the FPS is invoked
**   to perform DL processing, T31 will dequeue the next DSM item and will 
**   pass it into this function.  Since it is highly unlikely that a complete
**   HDLC frame will be received in one DSM item, the function will only be
**   able to parse out a partial frame.  Thus, the function must be reentrant
**   in that it needs to continue processing multiple DSM items to completely
**   build one HDLC frame (i.e. it needs to continue processing from where it
**   left off).
**
**   When receiving HDLC frames during +FRH processing, the sender may send 
**   more than one frame in a single transaction.  However, as per T.31, only
**   one frame can be transmitted to the local DTE for a single +FRH AT command.
**   Thus if multiple frames are received, additional received frames must be
**   buffered until the local DTE issues corresponsing +FRH commands to receive
**   the data.  The required buffering is inherently handled by the FPS DL 
**   watermark and the manner in which T31 processes the received data.
**
**   To parse the data, input bytes in the DSM item are sequentially removed
**   and processed.  Assuming first time entry into the function, processing
**   starts by searching the data to find the opening flag byte indicating the
**   start of a frame.  The data byte must be examined bit-by-bit since, as
**   stated, ZBI will force the data to no longer be byte aligned and a flag
**   byte may in fact cross byte boundaries.
**
**   Any sequence of 6 1-bits in a row constitutes a flag byte.  Any sequence
**   of 5 1-bits in a row followed by a 0-bit constitutes a sequence that has
**   an extra 0-bit.  In this case the zero bit must be removed from the bit
**   stream.  Processing of an input byte consists of extracting individual
**   bits and placing them into an output byte (buffer) while at the same time
**   keeping track of the number of consecutive 1-bits encountered, the index
**   of which input bit is being processed, the bit location to place the bit
**   into the output byte, and the number of bits that have already been added
**   to the output byte.  When the output byte becomes full (all 8-bits are 
**   accounted for and it is not a flag byte), it can be appended to the output
**   buffer.  Again, there is no direct correlation between the 8-bits of the
**   current input byte and the 8-bits comprising the current output byte other
**   than some of the input byte bits will be used to construct the actual data
**   that becomes the output byte.  Note that ALL 8-bits of the current input
**   byte will be processed before the function exits.
**
**   Since the frame can begin with any number of flag bytes, the function
**   must find and remove any sequence of consecutive opening flag bytes.
**   Actual frame data begins with the first non-flag byte detected after 
**   finding the opening flag byte.  The raw data is then appended into the
**   specified output buffer.  While processing the raw data, the function
**   must continue looking for the closing flag byte.  Once the closing flag
**   is found, a complete frame is ready to be passed back to the caller.
**
**   The reentrancy issues comes into play when the frame data arrives in
**   multiple DSM items.  A single byte of data may cross byte boundaries,
**   as well as, DSM items.  Since the byte is being processed bit by bit,
**   cases arise whereby the last bit of the input byte has been processed
**   but the actual remaining bits of the true data byte reside in the next
**   input byte which may not be accessible (because there is no more data in
**   the current DSM item).  The above mentioned parameters must be saved in
**   order for processing to resume the next time the function is entered.
**
**   If an output data byte is not fully constructed (8-bits accounted for)
**   and there is no more data to process in the current DSM item, then the
**   processing state is saved until the next iteration of this function at
**   which point the state is restored and processing can resume by dequeuing
**   input data bytes from the new DSM item.
**
**   Refer to dst31hdlci_zbi() above for detail on zero-bit insertion.
**
**   NOTE:  dst31hdlci_zbd_init() must be called to initialize ZBD processing
**     prior to processing each new sequence of frames (i.e. each time T31
**     enters the DST31_FRH_STATE for the first time after receiving a +FRH
**     command).
**
** PARAMETERS:
**   ibuf == pointer to the DSM item containing the input data stream to 
**     extract frame data from
**   obuf == pointer to the output data buffer to place ZBD data into
**   obuf_cnt == pointer to return the number of byte placed into obuf
**   carrier_detected == returns TRUE if carrier is detected, FALSE
**     otherwise.
**
** RETURNS:
**   DST31_FRAME_COMPLETE if a complete frame is found, otherwise
**   DST31_FRAME_INCOMPLETE if not
**
** SIDE EFFECTS:
**   None
**===========================================================================*/
dst31_frame_status_enum_type dst31hdlci_zbd
(
  dsm_item_type **  ibuf,
  uint8 *           obuf,
  uint16 *          obuf_cnt,
  boolean *         carrier_detected
)
{
  register uint8    ibyte;
  register uint8    ishift_cnt; 
  register uint8    obyte;
  register uint8    oshift_cnt; 
  int16             tbyte;
  boolean           frame_complete;
  boolean           flag_found;
  uint8             bit;
  uint8             bit_idx;
  uint8             i_limit;
  uint8             prev_1bit_mask;
  uint8             curr_1bit_mask;
  uint8             ones_cnt;
  uint8             ofill_cnt;
  uint16            ocnt;


  /* Safety Net */
  ASSERT(ibuf);
  ASSERT(obuf);
  ASSERT(obuf_cnt);

  DATA_MSG1_LOW("HDLC-ZBD on %d bytes of data", dsm_length_packet(*ibuf));

  /* Initialize non-reentrant local variables */
  frame_complete  = FALSE;
  flag_found      = FALSE;
  i_limit         = 0;     /* We need to limit how many bytes to process so we
                           ** don't burn too much CPU on one iteration! 
                           */

  /* Initialize reentrant local variables */
  obyte           = zbd_obyte_start;
  ocnt            = zbd_ocnt_start;
  ofill_cnt       = zbd_ofill_start;
  ones_cnt        = zbd_ones_cnt_start;
  oshift_cnt      = zbd_oshift_start;
  curr_1bit_mask  = zbd_curr_1bit_mask;
  prev_1bit_mask  = zbd_prev_1bit_mask;

  /* Get the next byte from the input buffer and start processing it */
  while (frame_complete == FALSE)
  {
    /* If we have reached our processing limit or there is no more data
    ** to process it's time to kick out.
    */
    if ((i_limit++ == DST31_FRH_MAX_BYTES_PER_20MS) || 
        ((tbyte = dsm_pull8(ibuf)) == -1))
    {
      if (i_limit == DST31_FRH_MAX_BYTES_PER_20MS)
      {
        DATA_MSG1_HIGH( "HLDC-ZBD reached limit=%d", DST31_FRH_MAX_BYTES_PER_20MS);
      }
      break;
    }

    /* Coerce the data byte into the desired type */
    ibyte = (uint8) tbyte;

    /* ishift_cnt is used to shift a bit mask through each bit of the
    ** input byte to extract indidivual bits.  It must always be reset
    ** for each new input byte.
    */
    ishift_cnt = 0;

    for (bit_idx = 0; bit_idx < DST31_MAX_BITS_PER_BYTE; bit_idx++)
    {
      DATA_MSG3_LOW("ibyte=0x%02x obyte=0x%02x fc=%d",
        ibyte, obyte, ofill_cnt);

      /* KW Fix: Check for max value of ishift_cnt*/
      if (ishift_cnt >= DST31_MAX_BITS_PER_BYTE)
      {
        /* ishift_cnt used in zbd_ibyte_bit_mask[]. So limit the value of
           ishift_cnt to size of the array*/
        ishift_cnt = DST31_MAX_BITS_PER_BYTE -1;
      }
      /* Check for a 1-bit first */
      bit = ibyte & zbd_ibyte_bit_mask[ishift_cnt];

      if (bit)
      {
        /* Keep track of how many consecutive ones we have found */
        ones_cnt++;

        if (ones_cnt == DST31_MAX_CONSECUTIVE_FLAG_ONE_BITS)
        {
          /* Ok so we know we have a flag byte.  We now need to figure out
          ** what state we are in so it can be processed accordingly.
          */
          flag_found = TRUE;

          /* Reset the consecutive ones counter so we can start over with 
          ** the next data byte if this flag gets tossed.
          */
          ones_cnt = 0;

          /* So what state are we in? */
          switch (zbd_state)
          {
            case DST31_FRH_HDLC_FIND_FLAG_STATE:
              /* In this state we have found the opening flag of the frame.
              ** We now need to check for consecutive flags and toss them
              ** until we get to the actual payload.
              */
              zbd_state = DST31_FRH_HDLC_FLAG_START_STATE;
              zbd_flag_cnt = 1;
              DATA_MSG1_LOW("Found opening flag, data=0x%x", tbyte);
              break;

            case DST31_FRH_HDLC_FLAG_START_STATE:
              /* We are still processing consecutive flags and tossing
              ** them as they are received.
              */
              zbd_flag_cnt++;

              if (zbd_flag_cnt == DST31_CARRIER_DETECT_CNT)
              {
                *carrier_detected = TRUE;
                DATA_MSG1_HIGH("+FRH carrier detected, fc=%d",zbd_flag_cnt);
              }

              DATA_MSG1_LOW("Toss consecutive flags fc=%d", zbd_flag_cnt);
              break;

            case DST31_FRH_HDLC_DATA_STATE:
              /* In this state we were capturing the raw payload data and 
              ** we have now received the closing flag for the frame.  We 
              ** have a complete HDLC frame at this point and we can pass
              ** it back out to the upper layers.
              */
              DATA_MSG2_LOW("Found closing flag, fc=%d val=0x%x",
                zbd_flag_cnt,tbyte);

              DATA_MSG2_HIGH("HDLC::PUSH back 0x%02x 0x%02x\n",
                (tbyte & 0xff),(zbd_prev_byte & 0xff));
              (void)dst31i_dsm_pushdown(ibuf, &tbyte, 1,
                                        DSM_DS_SMALL_ITEM_POOL);
              (void)dst31i_dsm_pushdown(ibuf, &zbd_prev_byte, 1,
                                        DSM_DS_SMALL_ITEM_POOL);

              zbd_flag_cnt = 0;

              zbd_state = DST31_FRH_HDLC_FLAG_STOP_STATE;
              break;
        
            case DST31_FRH_HDLC_FLAG_STOP_STATE:
              /* Frame processing is complete for the current frame */
              break;

            default:
              /* Should NEVER happen */
              DATA_MSG1_ERROR( "Unknown HDLC state, val=%d", zbd_state);
          } /* switch */

          /* If we have a complete frame we need to reset to get ready
          ** to process any subsequent frames.  The completed frame is
          ** packaged up and passed back to the upper layers.
          */
          if (zbd_state == DST31_FRH_HDLC_FLAG_STOP_STATE)
          {
            /* Reset ZBD processing since we are done with a frame */
            dst31hdlci_zbd_init();

            zbd_state = DST31_FRH_HDLC_FIND_FLAG_STATE;

            /* Don't do anything if we found a NULL frame */
            if (ocnt > 0)
            {
              /* Remove any extraneous 1-bits that were added to the previous
              ** output byte that are part of the detected flag.
              */
              obuf[ocnt-1] &= ~prev_1bit_mask;

              /* Finally, there could still be some actual data remaining
              ** in the final output byte so we need to extract it out
              ** and include it in the output buffer.
              */
              if ((obyte &= ~curr_1bit_mask) > 0)
              {
                obuf[ocnt++] = obyte;
              }

              /* Reset the 1-bits masks */
              prev_1bit_mask = curr_1bit_mask = 0;
            }
            else
            {
              DATA_MSG0_LOW("<<*** NULL HDLC frame ***>>");
            }

            /* This is the end of the current frame */
            frame_complete = TRUE;

            /* We are now done processing the 'for' loop */
            break;
          }
        }
        else 
        {
          /* Account for adding the 1-bit to the output buffer */
          ofill_cnt++;

          /* Increment the input byte shifter to get the next input bit */
          ishift_cnt++;

          /* Mask in the output bit */
          obyte |= (bit >> oshift_cnt);

          /* Keep track of the 1-bit sequence which may be part of a FLAG
          ** sequence added to the current output byte.  We may need to remove
          ** the extraneous bits later if indeed a flag byte is found.
          */
          curr_1bit_mask |= (bit >> oshift_cnt);
        }
      }
      else
      {
        /* 0-bit processing is done here */
        if (ones_cnt == DST31_MAX_CONSECUTIVE_ONE_BITS)
        {
          /* We found a ZBI 0-bit so we need to remove it -- this is done 
          ** by simply shifting the input byte to the next bit.
          */
          if (ishift_cnt < (DST31_MAX_BITS_PER_BYTE - 1))
          {
            ibyte <<= 1;
          }
        }
        else
        {
          /* Account for adding the 0-bit to the output buffer */
          ofill_cnt++;
          ishift_cnt++;
        }

        /* We can restart count consecutive one-bits since we found a zero 
        ** bit which was not part of a FLAG sequence.
        */
        if (ones_cnt > 0) 
        {
          ones_cnt = 0;
        }

        /* Since we broke the sequence of consecutive ones we need to 
        ** reset the 1bits masks.
        */
        prev_1bit_mask = curr_1bit_mask = 0;
      } /* ! if (bit = (ibyte & (0x80 >> ishift_cnt))) */

      /* If our output byte is full push it into the output buffer
      ** and setup for either processing the remaining bits in the
      ** current byte or getting the next byte to process.  If we found
      ** a flag, then we either change states to start capturing the
      ** raw data or we toss the flag byte.
      */
      if ((ofill_cnt >= DST31_MAX_BITS_PER_BYTE) || flag_found)
      {
        if (zbd_state == DST31_FRH_HDLC_FLAG_START_STATE)
        {
          /* We are still in the FLAG processing state.  We need to see if we
          ** have finally received a non-flag data byte so we can move on.
          */
          if (flag_found == FALSE)
          {
            /* Okay, we can change states to start capturing raw HDLC data.
            ** We know this because we finally found a non-flag byte.
            */ 
            DATA_MSG1_HIGH("+FRH now in DATA state, val=0x%x",obyte);
            zbd_state = DST31_FRH_HDLC_DATA_STATE;
 
            /* If we have reached the closing flag and we haven't signalled
            ** carrier detect is good, make it so.  This can happen when 
            ** multiple HDLC frames are received by a series of +FRH commands.
            ** The first in the sequence will get the full HDLC preamble and
            ** the rest will be separated by just a couple flags.
            */
            if (*carrier_detected == FALSE)
            {
              *carrier_detected = TRUE;
              DATA_MSG1_HIGH( "+FRH carrier detected, fc=%d",zbd_flag_cnt);
            }
          }
          else
          {
            /* Here we found a flag and we are still processing a sequence 
            ** of flags so we need to skip past the the closing 0-obit 
            ** (i.e. 0x7e 01111110)
            */ 
            flag_found = FALSE;
            bit_idx++;
            ishift_cnt += 2;
          }
        }

        /* Now we can capture the raw HDLC data */
        if (zbd_state == DST31_FRH_HDLC_DATA_STATE)
        {
          DATA_MSG2_LOW("HDLC->add val=0x%02x cnt=%d",
            GCSD_REFLECT_BYTE(obyte),ocnt);
          obuf[ocnt++] = obyte;
          prev_1bit_mask = curr_1bit_mask;
          curr_1bit_mask = 0;
        }

        /* Reset the output byte control variables so we can start creating the
        ** next output byte to write.
        */
        obyte = 0;
        oshift_cnt = 0;
        ofill_cnt = 0;

        /* The remaining input bits are shifted up to the MSB to maintain 
        ** consistency in the processing algorithm.
        */
        if (bit_idx < DST31_MAX_BITS_PER_BYTE)
        {
          ibyte <<= ishift_cnt;   /*lint !e734 */
          ishift_cnt = 0;
        }
      } /* if ((ofill_cnt >= DST31_MAX_BITS_PER_BYTE) || flag_found) */
    } /* for bit_idx */

    /* Save the current byte if we haven't reached the end of a frame yet */
    if (zbd_state != DST31_FRH_HDLC_FIND_FLAG_STATE)
    {
      zbd_prev_byte = (uint8) tbyte;
    }

    /* If we haven't filled up the output byte yet then we need to update
    ** the output bit shift counter to keep track of where the next bit 
    ** should be placed.
    */
    if (ofill_cnt < DST31_MAX_BITS_PER_BYTE)
    { 
      oshift_cnt = ofill_cnt;
    }
  } /* while */

  /* Remove any closing zero pad byte */
  if ((frame_complete == TRUE) && (ocnt > 0))
  {
    if (obuf[ocnt-1] == 0)
    {
      ocnt--;
    }
  }

  /* Make sure to return the current output buffer count */
  *obuf_cnt = ocnt;

  /* Save the reentrancy variables so we can pick up processing where
  ** we left off.
  */
  zbd_oshift_start   = oshift_cnt; 
  zbd_ones_cnt_start = ones_cnt;
  zbd_ofill_start    = (frame_complete == TRUE) ? 0 : ofill_cnt;
  zbd_ocnt_start     = (frame_complete == TRUE) ? 0 : ocnt;
  zbd_obyte_start    = (frame_complete == TRUE) ? 0 : obyte;
  zbd_curr_1bit_mask = curr_1bit_mask;
  zbd_prev_1bit_mask = prev_1bit_mask;

#if 0 /* Debug use only */
  DATA_MSG7(MSG_LEGACY_HIGH,"HDLC-ZBD reentrant variables: zbd_oshift_start   =%d,\
                             zbd_ofill_start    =%d, zbd_ones_cnt_start =%d,\
                             zbd_ocnt_start     =%d, zbd_obyte_start    =0x%x,\
                             zbd_curr_1bit_mask =0x%x, zbd_prev_1bit_mask =0x%x",
                             zbd_oshift_start, zbd_ofill_start, zbd_ones_cnt_start,\
                             zbd_ocnt_start, zbd_obyte_start, zbd_curr_1bit_mask,\
                             zbd_prev_1bit_mask);
#endif

  if (frame_complete)
  {
    DATA_MSG1_LOW( "HDLC-ZBD frame_size=%d", ocnt);
  }

  return ((frame_complete) ? DST31_FRAME_COMPLETE : DST31_FRAME_INCOMPLETE); 
} /* dst31hdlci_zbd */


/*=============================================================================
** FUNCTION: dst31hdlci_zbd_init
**
** DESCRIPTION:
**   This function is used to initialize the ZBD reentrancy state variables.
**   It should be called once prior to calling dst31hdlci_zbd() for the first
**   time to parse a sequence of HDLC frames received during +FRH state
**   processing.
**
**   Refer to dst31hdlci_zbd() above.
**
** PARAMETERS:
**   None
**
** RETURNS:
**   void
**
** SIDE EFFECTS:
**   Can adversely affect dst31hdlci_zbd() processing if not called prior
**   to parsing the first frame of a sequence during +FRH state processing.
**===========================================================================*/
void dst31hdlci_zbd_init
(
  void
)
{
  /* Initialize all reentrant variables used during ZBD processing.
  **
  ** Note: ZBI processing is handled a bit differently so we don't have
  ** any reentrancy requirements to worry about.
  */
  zbd_state          = DST31_FRH_HDLC_FIND_FLAG_STATE;
  zbd_obyte_start    = 0;
  zbd_ocnt_start     = 0;
  zbd_ofill_start    = 0;
  zbd_ones_cnt_start = 0;
  zbd_oshift_start   = 0;
  zbd_curr_1bit_mask = 0;
  zbd_prev_1bit_mask = 0;
  zbd_flag_cnt       = 0;
} /* dst31hdlci_zbd_init() */

#endif /* FEATURE_DATA_GCSD_FAX */
