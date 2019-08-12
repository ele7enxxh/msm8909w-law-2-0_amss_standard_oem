#ifndef DSUCSDRLP_H
#define DSUCSDRLP_H
/*===========================================================================

              GSM Circuit Switched Data XXXX Header File

DESCRIPTION
  This file contains the definitions used in the GSM Circuit switched 
  data XXXX functionality.

Copyright (c) 2002 - 2011 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/GSM/GCSD/vcs/dsgcsdrlp.h_v   1.3   21 Mar 2002 10:56:08   trebman  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdrlp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
02/11/11   TTV     Converted global variable Ns to static.
02/02/11   SS      Q6 free floating changes.
04/06/09   SS      Fixed Off Target Lint Errors
07/11/08   SA      Replaced __inline with INLINE macro for all targets.
02/09/07   DA      Fixed critical lint errors
08/14/06   AR      Change prototype ds_HardwareInitialise
05/05/06   AR      Incorporate changes from BoaApps interRAT effort.
02/09/06   AR      Lint corrections.
08/12/04   AR      Cleanup XID default macros.
07/14/04   AR      Relocate frame handling macros to header file.
06/07/04   AR      Replace define for SREJ with NV item.
05/17/04   AR      Add return code for XID processing routines.
05/12/04   AR      Added macro L2R_MAX_STATUS_BYTES.
04/07/04   AR      Relocated XIDMIN/MAX_VERSION to dsucsdnt.h
03/24/04   AR      Derive TRANSMIT_FULL_THRESHOLD values from negotiated XID.
03/10/04   AR      Assign T4 timer default based on WCDMA feature. Lint fix.
02/05/04   AR      Added PDUSTATUS_STATUS_BITS, make status constants hex
01/12/04   AR      Make RLP XID defaults dependent on WCDMA feature.
09/22/03   AR      Added ds_ucsd_UplinkPDUMgr_GetType() prototype.
07/15/03   TMR     Added support for RLP Version 2
04/21/03   AR      Add support for using DSM items inplace of static buffers
                   Lint corrections
01/23/03   TMR     Changed RLP_USE_SREJ to TRUE to turn on SREJ processing 
                   and removed RLP_USE_REJ since it isn't referenced anywhere
01/10/02   tmr     Initial release

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/


/***************************************************************************
 *  File:         /home/briers/gsmqc/include/SCCS/s.rlp.h
 *  -----
 *
 *  SCCS Version: 2.8
 *  -------------
 *
 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained herein
 *  is confidential.  The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Communications Consultants Worldwide Limited (C) 2000
 *
 *  Project:        P60
 *  --------
 *
 *  Software Unit:  GSM/MS/DS
 *  --------------
 *
 *  Description:    RLP Layer 2 Main Module - Header File
 *  ------------
 *
 *  Functions Used:
 *  ---------------
 *
 *  Modification History:
 *  ---------------------
 *  Date       Vers  Auth   Details
 *  ----       ----  ----   -------
 *  01-Jun-99  2.1   RCB    Original version
 *  17-Jun-99  2.2   RCB    Correct XID timer values
 *  21-Jun-99  2.3   RCB    Default to REJ error recovery
 *  25-Oct-99  2.4   RCB    Add #define RESET_IND/RES events
 *  13-Jun-00  2.5   RCB    Changes for Phase2+
 *  26-Jul-00  2.6   RCB    Update file header
 *  18_Aug-00  2.7   RCB    Changes associated with use of ghdi interface
 *  01_Sep-00  2.8   RCB    Pass RLP_params to rlp_initialise
 *
 *****************************************************************************/


#include "datamodem_variation.h"
#include "dsucsdnti.h"

/****************************************************************************
 * The following #defines configure the RLP implementation:
 ****************************************************************************/
#define RLP_TICK_MS    20  /* This is the period between calls to rlp_main */
                           /* (in milliseconds) */



/* These define the various characteristics of the RLP entity.              */
/* The names of these variables follow closely the model given in the ETS   */
/* Specification:  Radio Link Protocol (RLP) - GSM 04.22.                   */
/* Before changing these values, refer to the ETS specs!!                   */
/* ------------------------------------------------------------------------ */
#define RLP_M_VER_2        496  /* Number of different seq numbers (modulus) */
#define RLP_M_VER_0_1      62  



#define RLP_Nmin          0   /* Minimum Sequence number */

                           /* Maximum Sequence number */
#define RLP_Nmax           (get_current_rlp_max_frame_number())

#define RLP_N2            6   /* Retry count for retransmissions */

#define RLP_T1_12KBAUD    480  /* 480ms default value */
#define RLP_T1_6KBAUD     540  /* 540ms default value */
#define RLP_T1_HALFRATE   780  /* 780ms default value */
#define RLP_T2            70   /* 70ms default value  */



/* These are the default link parameters. These are negotiated using
 * an XID frame exchange.
 * Defaults specified in 3GPP TS24.022 Table 2                       */
/* ----------------------------------------------------------------- */
#define XIDDEFAULT_VERSION         0                    /* Version number */
#define XIDDEFAULT_IWFtoMSWindow   XIDMAX_MStoIWFWindowV1  /* Window size */
#define XIDDEFAULT_MStoIWFWindow   XIDMAX_MStoIWFWindowV1  /* Window size */
#define XIDDEFAULT_T4              50                 /* Resequence Timer */
#define XIDDEFAULT_T1              (RLP_T1_12KBAUD/10)
#define XIDDEFAULT_N2              6
#define XIDDEFAULT_T2              (RLP_T2/10)

#define XIDDEFAULT_Pt              0
#define XIDDEFAULT_P0              0
#define XIDDEFAULT_P1              512
#define XIDDEFAULT_P2              6



/***********************************************************************/
/* Defining this flag will cause poll recovery to start T1 after
   the first UnAcknowledged I-Frame. The Spec 11.10 suggests that
   Poll recovery should start after the Last frame in a sequence,
   so if a Frame is lost no recovery will start until the Tx window
   fills up and I-Frames stop being sent. This results in very slow
   Poll recovery. Over a noisy link, Data throughput is about 2-3
   times faster with this flag set, but it doesn't STRICTLY pass
   the test ETS 11.10 29.3.2.6.2 */

/*#define ENHANCED_POLL_RECOVERY 1*/ /*UNCOMMENT THIS FOR BETTER THROUGHPUT!!*/
/***********************************************************************/



/****************************************************************************
 * These are status values returned from RLP functions:
 ****************************************************************************/
#define RLP_OK       0
#define RLP_ERROR    (-1)




/****************************************************************************
 * THE FOLLOWING IS OBSOLETE, LEFT FOR DOCUMENTATION ONLY
 *
 * This is the V24 status/data word  structure.
 * It is the data format buffered in the Tx and Rx buffers between the 
 * L2R/RLP and the V24 serial interface.
 *
 * It has the following structure:
 *        MS Byte                   LS Byte
 *   7                    0   7                    0
 * +------------------------+------------------------+
 * |  V24 Status n          +    Data byte n         |   
 * +------------------------+------------------------+
 *
 * Status Bits:  Bit 0:  CTS for uplink and downlink
 *               Bit 1:  DTR for uplink, DSR for downlink
 *               Bit 2:  RTS for uplink, DCD for downlink
 *               Bit 3:  Don't care
 *               Bit 4:  Don't care
 *               Bit 5:  Don't care
 *               Bit 1:  C0
 *               Bit 1:  C1
 *
 * Where:     C0  C1
 *             0   0  : Status + 8bit character
 *             0   1  : status only
 *             1   0  : break signal
 *             1   1  : reserved
 *
 ****************************************************************************
#define V24STATUS_X_BITMASK     0x01
#define V24STATUS_SA_BITMASK    0x02
#define V24STATUS_SB_BITMASK    0x04

 Co/C1 bit masks 
#define V24TYPE_STATUSANDDATA   0x0000
#define V24TYPE_STATUSONLY      0x4000
#define V24TYPE_BREAK           0x8000
#define V24TYPE_RESERVED        0xC000
 ****************************************************************************/

/* L2R Status header byte size */
#define L2R_MAX_HDR_SIZE 2 
#define L2R_MAX_STATUS_BYTES 3 

/****************************************************************************
 * PDU Status definitions
 ****************************************************************************/
#define PDUSTATUS_X_BITMASK        0x20
#define PDUSTATUS_SA_BITMASK       0x80
#define PDUSTATUS_SB_BITMASK       0x40
#define PDUSTATUS_STATUS_BITS      ( PDUSTATUS_X_BITMASK | \
                                     PDUSTATUS_SA_BITMASK | \
                                     PDUSTATUS_SB_BITMASK )
 
#define PDUSTATUS_CTS_MASK         PDUSTATUS_X_BITMASK
#define PDUSTATUS_DTR_MASK         PDUSTATUS_SA_BITMASK
#define PDUSTATUS_RTS_MASK         PDUSTATUS_SB_BITMASK
#define PDUSTATUS_DSR_MASK         PDUSTATUS_SA_BITMASK
#define PDUSTATUS_DCD_MASK         PDUSTATUS_SB_BITMASK

#define PDUSTATUS_ADDRESS_MASK              0x1F
#define PDUSTATUS_2BYTE_STATUS_ADDRESS_MASK 0x7F

#define PDUSTATUS_REMAINDER_EMPTY           0x1F
#define PDUSTATUS_REMAINDER_FULL            0x1E
#define PDUSTATUS_BREAK                     0x1D
#define PDUSTATUS_BREAK_ACK                 0x1C
#define PDUSTATUS_2BYTE_STATUS              0x1B



/*****************************************************************
 * The RLP Frame structure
 ****************************************************************/
/* These are the total number of bytes in a large and small PDU 
 * (These will not all be data bytes during operation: some will be
 * status bytes or PDU structure bytes eg. pointers). */

#define RLP_HDR_SIZE_VER_0_1        2
#define RLP_HDR_SIZE_VER_2          3 
#define RLP_SM_FRM_DATA_SIZE       25
#define RLP_LG_FRM_DATA_SIZE       67
#define RLP_FCS_SIZE                3
#define RLP_PDU_MAX_USER_DATA_SIZE RLP_LG_FRM_DATA_SIZE


typedef struct
{
   uint32            Header;
   dsm_item_type *   Data;               /* Pointer to DSM item */
   byte              FCS[RLP_FCS_SIZE];
} RLP_Frame_T;

/* These are the total number of bytes in a large and small PDU 
 * (These will not all be data bytes during operation: some will be
 * status bytes or PDU structure bytes eg. pointers). */

/* Maximum number of actual data bytes in each type of PDU */  
#define RLP_CURRENT_MAX_DATA_BYTES  get_current_rlp_pdu_payload_size()

/* Macro to return the current length of a PDU */
#define RLP_CURRENT_PDU_SIZE        get_current_rlp_pdu_payload_size()


/*****************************************************************
 * These define the various Ns values
 ****************************************************************/
#define   NS_UNNUMBEREDFRAME    63
#define   NS_SUPERVISORYFRAME   62
#define   NS_MAX                61


/*****************************************************************
 * These define the frame types
 ****************************************************************/
#define    RR                   0x00
#define    REJ                  0x02
#define    RNR                  0x01
#define    SREJ                 0x03

#define    SABM                 0x07
#define    UA                   0x0c
#define    DISC                 0x08
#define    DM                   0x03
#define    FRAME_NULL           0x0f
#define    UI                   0x00
#define    XID                  0x17
#define    _TEST                0x1c
#define    REMAP                0x11

/* These are the different Frame types */
/* ----------------------------------- */
#define     FRAMETYPE_NONE            -1
#define     FRAMETYPE_UNNUMBERED      0
#define     FRAMETYPE_SUPERVISORY     1
#define     FRAMETYPE_INFORMATION     2


/* Macros for Frame Handling */
#define SET_UNNUMBERED_FRAME(x,y)  x.Header=0x000001f8;(x.Header|=(uint32)y<<10)
#define SET_INFORMATION_FRAME(x,y) x.Header=0x00000000;(x.Header|=(uint32)y<<1)
#define SET_SUPERVISORY_FRAME(x,y) x.Header=0x000001f0;(x.Header|=(uint32)y<<1)

#define GET_SUPERVISORY_FRAME(x)   ((x->Header >> 1) & 0x00000003)
#define GET_UNNUMBERED_FRAME(x)    ((x->Header >>10) & 0x0000001f)

#define SET_CR(x,y)       if (y) x.Header|=0x00000001;
#define SET_PF(x,y)       if (y) x.Header|=0x00000200;
#define SET_NR(x,y)       (x.Header|=(uint32)y<<10)
#define SET_NS(x,y)       (x.Header|=(uint32)y<<3)

#define GET_NS(x)         ((x->Header>>3) & 0x0000003f)
#define GET_NR(x)         ((x->Header>>10) & 0x0000003f)
#define GET_CR(x)         (((x->Header) & 0x00000001) ? TRUE : FALSE)
#define GET_PF(x)         (((x->Header) & 0x00000200) ? TRUE : FALSE)

/*  RLP V2-specific Frame Handling Macros */
#define GET_SUPERVISORY_FRAME_V2(x) ((x->Header>>11) & 0x00000003)

#define GET_NS_V2(x) ((x->Header) & 0x000001ff)
#define GET_NR_V2(x) ((x->Header>>13) & 0x000001ff)
#define GET_CR_V2(x) (((x->Header) & 0x00000400) ? TRUE : FALSE)
#define GET_PF_V2(x) (((x->Header) & 0x00000200) ? TRUE : FALSE)
#define GET_S_V2(x)  (((x->Header) & 0x00400000) ? TRUE : FALSE)

#define SET_INFORMATION_FRAME_V2(x,y) x.Header=0x00000000;(x.Header|=(uint32)y<<11)
#define SET_SUPERVISORY_FRAME_V2(x,y) x.Header=0x000001f0;(x.Header|=(uint32)y<<11)

#define SET_NS_V2(x,y) (x.Header|=(uint32)y)
#define SET_NR_V2(x,y) (x.Header|=(uint32)y<<13)
#define SET_CR_V2(x,y) if (y) x.Header|=0x00000400;
#define SET_PF_V2(x,y) if (y) x.Header|=0x00000200;
#define SET_S_V2(x,y)  if (y) x.Header|=0x00400000;


/* These bits will be used in the app_field field of the DSM 
** structure to keep information about the PDU
*/
#define RLP_DSM_APP_FIELD_INFO_FRAME      0x00000001
#define RLP_DSM_APP_FIELD_NO_STATUS_BYTE  0x00000002

/****************************************************************************
 * These parameters are the Link Control Parameters set-able using XID 
 ****************************************************************************/
typedef struct {

   /* Version >= 0: */
   byte    RLPVersion;      
   uint16  IWFtoMSWindow;   
   uint16  MStoIWFWindow;
   byte    T1;              /* T1 timout value */ 
   byte    N2;
   byte    T2;
   
   /* Version >= 1: */
   byte     Pt;  /* Type of compression (0=V42) */
   byte     P0;  /* V42 Compression direction */
   word     P1;  /* V42 Max number of codewords */
   byte     P2;  /* V42 max encodeable string length */

   /* Version >= 2: */
   byte     T4;              /* ReSequencing Timer */

} XID_Params_T;

/* These are the Max/Min legal values of the XID params for the link */
/* ----------------------------------------------------------------- */
#define XIDMIN_IWFtoMSWindowV1   0
#define XIDMAX_IWFtoMSWindowV1   61

#define XIDMIN_MStoIWFWindowV1   0
#define XIDMAX_MStoIWFWindowV1   61

#define XIDMIN_IWFtoMSWindowV2   0
#define XIDMAX_IWFtoMSWindowV2   488

/* These values are actually specified using a formula. When V2 is
   fully implemented this must be done. !!!TBD!!! */
#define XIDMIN_MStoIWFWindowV2   0
#define XIDMAX_MStoIWFWindowV2   488   

#define XIDMIN_T1_12KB           38
#define XIDMIN_T1_14KB           42   /* For Version 2 */

#define XIDMAX_T2                8

#define XIDMIN_N2                1

#define XIDMIN_COMPRESSION_P0    0
#define XIDMAX_COMPRESSION_P0    3

#define XIDMIN_COMPRESSION_P1    512
#define XIDMAX_COMPRESSION_P1    65535

#define XIDMIN_COMPRESSION_P2    6
#define XIDMAX_COMPRESSION_P2    250

#define XIDMIN_T4                25  /* For Version 2 */


/* These states are used in the transmission of the various frames */
/* --------------------------------------------------------------- */
#define     STATE_IDLE   0
#define     STATE_SEND   1
#define     STATE_WAIT   2
#define     STATE_SREJ   3
#define     STATE_RCVD   4


/* These are the main states of the RLP mechanism */
/* ---------------------------------------------- */
#define     RLPSTATE_ADM_DETACHED                   0
#define     RLPSTATE_ADM_ATTACHED                   1
#define     RLPSTATE_PENDING_CONNECTION_REQUEST     2
#define     RLPSTATE_PENDING_CONNECTION_RESPONSE    3
#define     RLPSTATE_CONNECTION_ESTABLISHED         4
#define     RLPSTATE_DISC_INITIATED                 5
#define     RLPSTATE_PENDING_RESET_REQUEST          6
#define     RLPSTATE_PENDING_RESET_INDICATION       7
#define     RLPSTATE_MS_REMAPPING                   8
#define     RLPSTATE_NETWORK_REMAPPING              9

/****************************************************************************
 * These Functions provide the interface to the RLP, from the
 * L2R (i.e. the DS-ISR code)
 ****************************************************************************/
void rlp_initialise   (ds_ucsd_nt_rlp_parameters_T *);  /* Initialise the RLP layer */
void rlp_main         (void);  /* Entry point for the RLP Layer  */
 
boolean rlp_flow_control   (void);  /* TRUE=Flow Control Cond. from RLP */ 
boolean rlp_remote_receiver_ready (void);
short   rlp_XID_send       (const XID_Params_T* NewXID);  /* Do an XID negotiation */
short   rlp_TEST_send      (void);  /* Start a TEST sequence    */

/* Functions shared between RLP modules */
void HandleTEST          (RLP_Frame_T* Frame);
void HandleUI            (RLP_Frame_T* Frame);
short SendUnNumberedFrame (word Frame, 
                           boolean cr, boolean pf,
                           dsm_item_type * PDU);
short ReadRLPFrame       (RLP_Frame_T* Frame);
short Send_TXU           (void);
void Disconnect          (void);
boolean SREJisSuitable   (void);

void XID_to_PDU          (dsm_item_type** PDU_ptr, const XID_Params_T* Xid);
boolean ProcessXIDCommand   (dsm_item_type* PDU, XID_Params_T* Current);
boolean ProcessXIDResponse  (dsm_item_type* PDU, XID_Params_T* Xid);
void MakeRemapFrame      (short Nr, XID_Params_T* pXID);

/* These routines are specific to the Hardware */
void ds_HardwareInitialise      (boolean initTx, boolean initRx);
void ds_HardwareRLPTxRx         (void);
void ds_HardwareReadModemLines  (boolean *rts,
                                 boolean *dtr);
boolean   ds_HardwareTxReady         (void);
boolean   ds_HardwareFrameAvailable  (void);
short     ds_HardwareReceiveRLPFrame (RLP_Frame_T* Frame);
void      ds_HardwareRLPTx (void);
boolean   ds_HardwareRLPRx (void);

short 
ds_HardwareSendRLPFrame 
(
   RLP_Frame_T* Frame,
   uint32       frame_hdr_size
); 

/* V42 interface functions */
void  V42_initialise(byte max_str_len, word max_codes, v42_comp_direction_T);
short V42_main       (void);
void  V42_terminate  (void);
short V42_terminated (void);


/******************************************************************
* PDU Related information
******************************************************************/


/* These are structures for the PDU list state data. */
/* Separate Tx and Rx lists are required.            */
typedef struct {
   PDU_Type Type;
   word     PDUCount;
   uint16   VA;
   uint16   VS;
   uint16   VD;
   byte *   State;
} Tx_PDU_list_T;

typedef struct {
   PDU_Type Type;
   word     PDUCount;
   uint16   VE;
   uint16   VR;
   byte *   State;
} Rx_PDU_list_T;


/* Typedef the PDU List type */
typedef dsm_item_type ** PDU_List_T;


/* These states are used in the transmission of the various frames */
/* --------------------------------------------------------------- */
#define     PDU_STATE_IDLE   0
#define     PDU_STATE_SEND   1
#define     PDU_STATE_WAIT   2
#define     PDU_STATE_SREJ   3
#define     PDU_STATE_RCVD   4

/* These are the thresholds at which the flow control starts to take effect 
   ------------------------------------------------------------------------*/
#define RECEIVE_FULL_THRESHOLD     (RLP_M_VER_0_1 - 20)
#define RECEIVE_FULL_THRESHOLD_V2  (RLP_M_VER_2 - 160 /* 8*20 */)

/* Macros for DataToTransmit, Inc/Dec Sequence Number */
#define INC_SEQ_NUM(x)     ((x==RLP_Nmax) ? 0 : (x+1))
#define DEC_SEQ_NUM(x)     ((x==0) ? RLP_Nmax : (x-1))


/* Macros used to copy a PDU sized array of bytes */
//#define COPY_LARGE_PDU_DATA(To,From) memcpy(To, From, RLP_PDU_MAX_USER_DATA_SIZE)
//#define COPY_SMALL_PDU_DATA(To,From) memcpy(To, From, PDU_SMALL_LENGTH)
//#define COPY_PDU_DATA(To,From,Len)   memcpy(To, From, Len)

/* These macros are used to obtain a pointer to the start of a PDU
 * within a List or the List address. They are used to avoid having
 * lots of "ifs" in the code to test for large or small PDU lists. */
#define PDU_LIST_PTR(pList) ((PDU_List_T*)(pList->Type==PDU_TYPE_SMALL) ? \
                                   &small_PDU_data_list : \
                                   &large_PDU_data_list )

#define PDU_PTR(pList,Index) ((dsm_item_type*)(pList->Type==PDU_TYPE_SMALL) ? \
                                   small_PDU_data_list[Index] : \
                                   large_PDU_data_list[Index] )

#define PDU_DATA_PTR(pList,Index) ((byte*)(PDU_PTR(pList,Index)->data_ptr) )


#define DSM_PUSHDOWN(item_ptr, buf, size) \
      if ( size != dsm_pushdown(item_ptr, buf, (uint16)size, DSM_ITEM_POOL(*item_ptr))) \
      { \
        ERR("DSM pushdown problem",0,0,0); \
      }

#define DSM_PUSHDOWN_TAIL(item_ptr, buf, size) \
      if ( size != dsm_pushdown_tail(item_ptr, buf, (uint16)size, DSM_ITEM_POOL(*item_ptr))) \
      { \
        ERR("DSM pushdown tail problem",0,0,0); \
      }

#define DSM_PULLUP(item_ptr, buf, size) \
      if ( size != dsm_pullup(item_ptr, buf, (uint16)size)) \
      { \
        ERR("DSM pullup problem",0,0,0); \
      }

#define DSM_PULLUP_TAIL(item_ptr, buf, size) \
      if ( size != dsm_pullup_tail(item_ptr, buf, (uint16)size)) \
      { \
        ERR("DSM pullup tail problem",0,0,0); \
      }

#define DSM_EXTRACT(item_ptr, offset, buf, size) \
      if (size != dsm_extract(item_ptr, (uint16)offset, buf, (uint16)size)) \
      {\
        ERR("DSM extract problem ",0,0,0); \
      }

#define DSM_DUP(dup_ptr, src_ptr, offset, size) \
      if (size != dsm_dup_packet(dup_ptr, src_ptr, (uint16)offset, (uint16)size)) \
      {\
        ERR("DSM dup problem ",0,0,0); \
      }

 
/*****************************************************************************
 * Export PDU function prototypes.  These functions are defined in 
 * dsgcsdulpdu.c and dsgcsddlpdu.c
 *****************************************************************************/
void      ds_ucsd_UplinkPDUMgr_ColdStart              (PDU_Type Type);
void      ds_ucsd_UplinkPDUMgr_Initialise             (PDU_Type Type, boolean Allocate);
void      ds_ucsd_UplinkPDUMgr_PwrUp_Init             (void);
void      ds_ucsd_UplinkPDUMgr_Cleanup                (void);
boolean   ds_ucsd_UplinkPDUMgr_RemapDataPending       (void);
PDU_Type  ds_ucsd_UplinkPDUMgr_Retrieve               (dsm_item_type ** PDU_ptr);
boolean   ds_ucsd_UplinkPDUMgr_SendSlotsAvailable     (void);
PDU_Type  ds_ucsd_UplinkPDUMgr_Write                  (dsm_item_type ** PDU_ptr);
boolean   ds_ucsd_UplinkPDUMgr_DataToSend             (void);
dsm_item_type * ds_ucsd_UplinkPDUMgr_Read             (void);
dsm_item_type * ds_ucsd_UplinkPDUMgr_ReadOutOfSeq     (short Index);
short     ds_ucsd_UplinkPDUMgr_Remap                  (PDU_Type Type, short Index);
void      ds_ucsd_UplinkPDUMgr_DecreaseVStoNR         (short Nr);
void      ds_ucsd_UplinkPDUMgr_AdvanceVAtoNR          (short Nr);
void      ds_ucsd_UplinkPDUMgr_Empty                  (void);
boolean   ds_ucsd_UplinkPDUMgr_IsEmpty                (void);
boolean   ds_ucsd_UplinkPDUMgr_AllFramesAcknowledged  (void);
void      ds_ucsd_UplinkPDUMgr_MarkForRetransmission  (short Index);
short     ds_ucsd_UplinkPDUMgr_SREJDataToTransmit     (void);
boolean   ds_ucsd_UplinkPDUMgr_TransmitWindowOpen     (void);
uint16    ds_ucsd_UplinkPDUMgr_VS                     (void);
uint16    ds_ucsd_UplinkPDUMgr_VA                     (void);
byte      ds_ucsd_UplinkPDUMgr_State                  (short Index);
PDU_Type  ds_ucsd_UplinkPDUMgr_GetType                (void);
void      ds_ucsd_UplinkPDUMgr_Free                   (dsm_item_type ** PDU_ptr);
dsm_item_type *  ds_ucsd_UplinkPDUMgr_Release         (Tx_PDU_list_T* List, short Index);
dsm_item_type *  ds_ucsd_UplinkPDUMgr_New             (void);


void      ds_ucsd_DownlinkPDUMgr_ColdStart            (PDU_Type Type);
void      ds_ucsd_DownlinkPDUMgr_Initialise           (PDU_Type Type, boolean Allocate);
void      ds_ucsd_DownlinkPDUMgr_PwrUp_Init           (void);
void      ds_ucsd_DownlinkPDUMgr_Cleanup              (void);
boolean   ds_ucsd_DownlinkPDUMgr_DataAvailable        (void);
PDU_Type  ds_ucsd_DownlinkPDUMgr_Read                 (dsm_item_type ** PDU_ptr);
void      ds_ucsd_DownlinkPDUMgr_Write                (dsm_item_type ** PDU_ptr);
void      ds_ucsd_DownlinkPDUMgr_WriteOutOfSeq        (dsm_item_type ** PDU_ptr, short Ns);
short     ds_ucsd_DownlinkPDUMgr_Remap                (PDU_Type Type);
void      ds_ucsd_DownlinkPDUMgr_DeleteUnAcknowledgedFrames (void);
boolean   ds_ucsd_DownlinkPDUMgr_ReceiveListFull      (void);
uint16    ds_ucsd_DownlinkPDUMgr_VE                   (void);
uint16    ds_ucsd_DownlinkPDUMgr_VR                   (void);
void      ds_ucsd_DownlinkPDUMgr_IncVR                (void);
byte      ds_ucsd_DownlinkPDUMgr_GetState             (short Index);
void      ds_ucsd_DownlinkPDUMgr_SetState             (short Index, byte State);
void      ds_ucsd_DownlinkPDUMgr_Empty                (void);
boolean   ds_ucsd_DownlinkPDUMgr_IsEmpty              (void);
boolean   ds_ucsd_DownlinkPDUMgr_ReceiveWindowOpen    (short Ns);
short     ds_ucsd_DownlinkPDUMgr_CheckSREJToSend      (void);
void      ds_ucsd_DownlinkPDUMgr_Free                 (dsm_item_type ** PDU_ptr);
dsm_item_type *  ds_ucsd_DownlinkPDUMgr_Release       (Rx_PDU_list_T* List, short Index);
dsm_item_type *  ds_ucsd_DownlinkPDUMgr_New           (void);

/****************************************************************************
 * Function name:  ds_ucsd_get_rlp_process_uplink
 * ------------- 
 * Description:    Returns the value of Rlp_process_uplink
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_rlp_process_uplink(void);

/****************************************************************************
 * Function name:  ds_ucsd_set_rlp_process_uplink
 * ------------- 
 * Description:    Sets the value of Rlp_process_uplink
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_rlp_process_uplink(boolean value);

/****************************************************************************
 * Function name:  ds_ucsd_get_rlp_process_downlink
 * ------------- 
 * Description:    Returns the value of Rlp_process_downlink
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_rlp_process_downlink(void);

/****************************************************************************
 * Function name:  ds_ucsd_set_rlp_process_downlink
 * ------------- 
 * Description:    Sets the value of Rlp_process_downlink
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_rlp_process_downlink(boolean value);

/****************************************************************************
 * Function name:  ds_ucsd_get_rlp_error_handling
 * ------------- 
 * Description:    Returns the value of RLP_ErrorHandling
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_rlp_error_handling(void);

/****************************************************************************
 * Function name:  ds_ucsd_set_rlp_error_handling
 * ------------- 
 * Description:    Sets the value of RLP_ErrorHandling
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_rlp_error_handling(boolean value);

/****************************************************************************
 * Function name:  ds_ucsd_get_current_xid
 * ------------- 
 * Description:    Returns a pointer to the Rlp_CurrentXID
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        Pointer of type XID_Params_T
 * -------- 
 ****************************************************************************/
XID_Params_T * ds_ucsd_get_current_xid(void);

/****************************************************************************
 * Function name:  ds_ucsd_set_RlpCurrentState
 * ------------- 
 * Description:    Sets the value of RlpCurrentState
 * -----------     
 * Parameters:     short: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_RlpCurrentState(short value);

/****************************************************************************
 * Function name:  ds_ucsd_get_RxFrame
 * ------------- 
 * Description:    Returns RxFrame
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        Pointer of type RLP_Frame_T
 * -------- 
 ****************************************************************************/
RLP_Frame_T * ds_ucsd_get_RxFrame(void);

/****************************************************************************
 * Function name:  ds_ucsd_get_TxFrame
 * ------------- 
 * Description:    Returns RxFrame
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        Pointer of type RLP_Frame_T
 * -------- 
 ****************************************************************************/
RLP_Frame_T * ds_ucsd_get_TxFrame(void);

/****************************************************************************
 * Function name:  ds_ucsd_get_UA_FBit
 * ------------- 
 * Description:    Returns the value of UA_FBit
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_UA_FBit(void);

/****************************************************************************
 * Function name:  ds_ucsd_set_UA_FBit
 * ------------- 
 * Description:    Sets the value of UA_FBit
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_UA_FBit(boolean value);

/****************************************************************************
 * Function name:  ds_ucsd_get_UA_State
 * ------------- 
 * Description:    Returns the value of UA_State
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        byte
 * -------- 
 ****************************************************************************/
byte ds_ucsd_get_UA_State(void);

/****************************************************************************
 * Function name:  ds_ucsd_set_UA_State
 * ------------- 
 * Description:    Sets the value of UA_State
 * -----------     
 * Parameters:     byte: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_UA_State(byte value);

/****************************************************************************
 * Function name:  ds_ucsd_set_Negotiate_XID_ADM_Values
 * ------------- 
 * Description:    Sets the value of Negotiate_XID_ADM_Values
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_Negotiate_XID_ADM_Values(boolean value);

/****************************************************************************
 * Function name:  ds_ucsd_get_CR
 * ------------- 
 * Description:    Returns the value of CR
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_CR(void);

/****************************************************************************
 * Function name:  ds_ucsd_get_PF
 * ------------- 
 * Description:    Returns the value of PF
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_PF(void);

/****************************************************************************
 * Function name:  ds_ucsd_get_UnNumberedFrame
 * ------------- 
 * Description:    Returns the value of UnNumberedFrame
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
short ds_ucsd_get_UnNumberedFrame(void);

/****************************************************************************
 * Function name:  ds_ucsd_get_FrameType
 * ------------- 
 * Description:    Returns the value of FrameType
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
short ds_ucsd_get_FrameType(void);

/****************************************************************************
 * Function name:  ds_ucsd_get_Remap_State
 * ------------- 
 * Description:    Returns the value of Remap_State
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
short ds_ucsd_get_Remap_State(void);

/****************************************************************************
 * Function name:  ds_ucsd_set_Remap_State
 * ------------- 
 * Description:    Sets the value of Remap_State
 * -----------     
 * Parameters:     short: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_Remap_State(short value);

/****************************************************************************
 * Function name:  ds_ucsd_get_UplinkRemapped
 * ------------- 
 * Description:    Returns the value of UplinkRemapped
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        boolean
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_UplinkRemapped(void);

/****************************************************************************
 * Function name:  ds_ucsd_set_UplinkRemapped
 * ------------- 
 * Description:    Sets the value of UplinkRemapped
 * -----------     
 * Parameters:     boolean: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_UplinkRemapped(boolean value);

/****************************************************************************
 * Function name:  ds_ucsd_get_UplinkRemappedLastNr
 * ------------- 
 * Description:    Returns the value of UplinkRemappedLastNr
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        short
 * -------- 
 ****************************************************************************/
short ds_ucsd_get_UplinkRemappedLastNr(void);

/****************************************************************************
 * Function name:  ds_ucsd_set_UplinkRemappedLastNr
 * ------------- 
 * Description:    Sets the value of UplinkRemappedLastNr
 * -----------     
 * Parameters:     short: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_UplinkRemappedLastNr(short value);

/****************************************************************************
 * Function name:  ds_ucsd_get_CurrentPDUType
 * ------------- 
 * Description:    Returns the value of CurrentPDUType
 * -----------     
 * Parameters:     None
 * ----------
 * Returns:        PDU_Type
 * -------- 
 ****************************************************************************/
PDU_Type ds_ucsd_get_CurrentPDUType(void);

/****************************************************************************
 * Function name:  ds_ucsd_set_CurrentPDUType
 * ------------- 
 * Description:    Sets the value of CurrentPDUType
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
void ds_ucsd_set_CurrentPDUType(PDU_Type value);

/****************************************************************************
 * Function name:  ds_ucsd_get_Rlp_current_version_is_2
 * ------------- 
 * Description:    Gets the value of Rlp_current_version_is_2
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
boolean ds_ucsd_get_Rlp_current_version_is_2(void);

/****************************************************************************
 * Function name:  ds_ucsd_get_Rlp_current_PDU_payload_size
 * ------------- 
 * Description:    Gets the value of Rlp_current_PDU_payload_size
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
uint16 ds_ucsd_get_Rlp_current_PDU_payload_size(void);

/****************************************************************************
 * Function name:  ds_ucsd_get_Rlp_current_header_size
 * ------------- 
 * Description:    Gets the value of Rlp_current_header_size
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
uint16 ds_ucsd_get_Rlp_current_header_size(void);

/****************************************************************************
 * Function name:  ds_ucsd_get_Rlp_current_max_window_size
 * ------------- 
 * Description:    Gets the value of Rlp_current_max_window_size
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
uint16 ds_ucsd_get_Rlp_current_max_window_size(void);

/****************************************************************************
 * Function name:  ds_ucsd_get_Rlp_current_max_frame_number
 * ------------- 
 * Description:    Gets the value of Rlp_current_max_frame_number
 * -----------     
 * Parameters:     PDU_Type: value
 * ----------
 * Returns:        None
 * -------- 
 ****************************************************************************/
uint16 ds_ucsd_get_Rlp_current_max_frame_number(void);

#ifndef BOA_APPS
INLINE boolean current_rlp_version_is_2 (void)
{
   return ds_ucsd_get_Rlp_current_version_is_2();
}

INLINE uint16 get_current_rlp_pdu_payload_size (void)
{
   return ds_ucsd_get_Rlp_current_PDU_payload_size();
}

INLINE uint16 get_current_rlp_header_size (void)
{
   return ds_ucsd_get_Rlp_current_header_size();
}

INLINE int16 get_current_rlp_max_window_size (void)
{
   return ds_ucsd_get_Rlp_current_max_window_size();
}

INLINE int16 get_current_rlp_max_frame_number (void)
{
   return ds_ucsd_get_Rlp_current_max_frame_number();
}

INLINE uint16 get_current_rlp_neg_tx_window_size (void)
{
   XID_Params_T *Rlp_CurrentXID = NULL;
   Rlp_CurrentXID = ds_ucsd_get_current_xid();
   return Rlp_CurrentXID->MStoIWFWindow;
}

#else /* BOA_APPS */
#define current_rlp_version_is_2()           (ds_ucsd_get_Rlp_current_version_is_2())
#define get_current_rlp_pdu_payload_size()   (ds_ucsd_get_Rlp_current_PDU_payload_size())
#define get_current_rlp_header_size()        (ds_ucsd_get_Rlp_current_header_size())
#define get_current_rlp_max_window_size()    (ds_ucsd_get_Rlp_current_max_window_size())
#define get_current_rlp_max_frame_number()   (ds_ucsd_get_Rlp_current_max_frame_number())
#define get_current_rlp_neg_tx_window_size()\
                                    {ds_ucsd_get_current_xid()->MStoIWFWindow}

void  ActionNetworkRemapping (DSI_Event_T Event);
short HandleREMAPCommand (RLP_Frame_T *Frame);
#endif /* BOA_APPS */

#endif /* DSUCSDRLP_H */



