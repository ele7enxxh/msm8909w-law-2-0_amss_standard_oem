#ifndef L1_DS_H
#define L1_DS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       G E R A N   L 1 - D A T A  S E R V I C E S   P R O C E S S I N G
                       A P I   F I L E

DESCRIPTION
   This module contains definitions for the interface between L1 and
   Data Services.


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/public/l1_ds.h#1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
17/03/15   xz       CR807490 sovle the address word aligment issue for scheduled_status of SCH
19/08/14   pjr      CR711506 Support for AMR Partial Decode and DTX detection FR19667
06/06/14   jk       CR668058:Adding EPD Log pakcet 
11/07/14   pjr      CR 692177 Added support for RFACCH 2.0
02/01/14   jk       CR595130: GL1 changes to support VAMOS-II
15/08/13   sk       CR524039 GBTA bringup changes
09-02-10   tjw      Initial revision created from l1_ds.h.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES REQUIRED FOR API

===========================================================================*/
#ifndef COMDEF_H
  #include "comdef.h"
#endif
  #include "gfw_sw_intf.h"


/* Copied from "gl1_defs.h", cross-checked in geran\gl1\inc\l1_ds.h */
#define APICOPY__GL1_DEFS_FRAMES_IN_BLK_PERIOD 4

typedef struct
{
  uint8  storeStatus     : 1;
  uint8  combStatus      : 1; 
  uint8  clearStatus     : 1;
  uint8  crcStatus       : 1;
  uint8  buffId          : 1;  
  uint32 snrStored;
  uint32 snrCurrent;
  uint16 rFacchFNL1;
  uint16 rFacchFNFW;
} APICOPY__GfwRfacchLogType;

typedef struct
{
   boolean       good_data;
   boolean       firecode_used;
   uint16        len;
   uint16        chan;      /* gl1_msg_chan_type */
   uint16        sid;
   uint16        voicing_mode;
   uint16        rx_qual;   /* hard rx qual */
   uint16        rx_qual_s; /* soft rx qual */
   uint16        bfi;       /* bad frame indicator */
   uint16        ufi;
   uint16        sfi;       /* speech frame indicator */
   uint16        onset;
   uint16        cmi;
   uint16        cmc;
   uint16        phase;
   uint16        bep[APICOPY__GL1_DEFS_FRAMES_IN_BLK_PERIOD];
   boolean       rfacch_store_status;
   boolean       rfacch_comb_status;
   boolean       rsacch_store_status;
   boolean       rsacch_comb_status;
   uint16        scheduled_status;
   boolean       vamospaired;
   APICOPY__GfwRfacchLogType rfacchLogBuff;
   GfwEpdLogPacket epdLogPacket;

   /* Metrics used for FEATURE_GSM_AMR_PD_DTX_DETECT */
   boolean       gfw_dtx_ind_dec;
   boolean       gfw_amr_pd_ind_dec;
   boolean       gfw_gl1_disable_amr_pd;
} APICOPY__gl1_defs_rx_hdr_struct;

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/* DSP stores data in words, so need 19 words to store 290 bits. */
#define GCSD_BUF_MAX_DATA_OCTETS 38   // 290 bits stored in 19 words.
/* DSP receive buffers contain a header, so to limit copies of data
   we allow for that header in the buffer GSCD passes to L1. */
#define GCSD_BUF_MAX_OFFSET sizeof(APICOPY__gl1_defs_rx_hdr_struct)

typedef enum {
      GCSD_RX_NORMAL=0,
      GCSD_BAD_DATA,
      GCSD_NO_BUFFER,
      GCSD_UNKNOWN
} gcsd_receive_status;

typedef enum {
      GCSD_TX_NORMAL=0,
      GCSD_SKIP_TX
} gcsd_transmit_status;

typedef struct {
   uint16    data_offset;
   uint8     data[GCSD_BUF_MAX_OFFSET + GCSD_BUF_MAX_DATA_OCTETS];
} gcsd_buffer;

typedef gcsd_buffer *gcsd_buffer_ptr;

#endif /* L1_DS_H */
