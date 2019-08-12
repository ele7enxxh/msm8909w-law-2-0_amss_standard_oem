#ifndef DST31HDLCI_H
#define DST31HDLCI_H

/*===========================================================================
**
**
**                        D A T A   S E R V I C E S
**
**           G S M  C I R C U I T - S W I T C H E D  D A T A
**
**                   F A X  P R O T O C O L  S T A C K
**
**                       T 3 1  H D L C  S E R V I C E S
**
**                E X T E R N A L   H E A D E R   F I L E
**
** Description:
**
**   This is the external header file which provides HDLC zero-bit insertion
**   and deletion functions for the GCSD-FPS T31 Layer.  It contains the 
**   definitions of data structures, defined and enumerated constants, and 
**   function prototypes.
**

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (c) 2002 Qualcomm Technologies Incorporated. All Rights Reserved
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dst31hdlci.h#1 $

when      who   what, where, why
--------  ---   ---------------------------------------------------------------
01/14/03  RLP   Code review changes.
11/15/02  RLP   Changes for GCF certification support.
11/04/02  RLP   Nortel IOT updates from Stage 2 testing.
09/11/02  RLP   MMO2 IOT changes.
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

/*=============================================================================
** DEFINES
**===========================================================================*/
#define DST31_MAX_CONSECUTIVE_ONE_BITS        5
#define DST31_MAX_CONSECUTIVE_FLAG_ONE_BITS   6
#define DST31_MAX_BITS_PER_BYTE               8

/* This is the number of HDLC opening flags to detect before indicating
** that carrier is detected.  This count directly impacts when the CONNECT
** response is sent to the local TE after entering a +FRH state.
*/
#ifndef ENABLE_GSM_TREX_NETWORK_INTERFACE
#define DST31_CARRIER_DETECT_CNT              10
#else
#define DST31_CARRIER_DETECT_CNT              1
#endif

/*=============================================================================
** EXTERNAL FUNCTIONS FOR USE SOLEY BY T31
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
**   '0' bit is inserted into the data stream after any sequence of five '1' 
**   bits has been detected between two flags.
**
**   A valid HDLC frame has the following format:
**
**   <flag byte(s)><address byte><control byte><data bytes><16bit-FCS><flag(s)>
**
**   Inserting a bit into the data stream will in effect nullify byte boundaries
**   for all subsequent data bytes.  The data after ZBI is performed should be
**   treated as a pure bit stream.  This has direct ramifications on the zero-bit
**   deletion operation described below (see dst31hdlci_zbd() for details).
**
** PARAMETERS:
**   ibuf == pointer to the input data buffer on which to perform ZBI
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
extern uint16 dst31hdlci_zbi
(
  uint8 *   ibuf, 
  uint16    ibuf_len, 
  uint8 *   obuf
);


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
extern dst31_frame_status_enum_type dst31hdlci_zbd
(
  dsm_item_type **  ibuf,
  uint8 *           obuf,
  uint16 *          obuf_cnt,
  boolean *         carrier_detected
);


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
extern void dst31hdlci_zbd_init
(
  void
);

#endif /* FEATURE_DATA_GCSD_FAX */
#endif // DST31HDLCI_H
