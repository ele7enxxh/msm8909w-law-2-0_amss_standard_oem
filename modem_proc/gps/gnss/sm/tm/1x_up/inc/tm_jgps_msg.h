#ifndef TM_JGPS_MSG_H
#define TM_JGPS_MSG_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                  TM JGPS Message Translation Unit Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for Japan 
  GPS specification for KDDI.

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/1x_up/inc/tm_jgps_msg.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/06   rw      Initial Release
===========================================================================*/

/*=============================================================================

                                Include Files

=============================================================================*/
#include "tm_common.h"
#include "pdapi.h"

#ifdef FEATURE_JCDMA_GPSONE

/*----------------------------------------------------------------------------

    Header:
    ========================
    Packed Data structure which defines the common header between MS and SCU. 

    7         0 (bits)        Description
    --------------           ------------------------------------------
   0| Service ID |            An ID for service identification
   1|            |            Fixed to charcter code "ST" (0x53, 0x54)
    --------------                          
   2| ProtVer    |            Protocol Version 0x1: MS-A dedicated terminal
    --------------                            0x2: MS-B capable terminal
   3| Length     |            Data length after "cmdID" to end of "Data"
   4|            |
    -------------- 
   5| cmdID      |            Data contained in packet.
    -------------- 
    | Data       |
    | (Variable) |
       . . . 
(Bytes)

-----------------------------------------------------------------------------*/
/* INTERNAL: Data structure which defines the common header between MS and SCU. 
*/
typedef struct {
  char service_id[2];   /* ID for service Identification, "ST" fixed now */
  byte prot_version;    /* Prot version between MS and SCU = 0x1 */
  word length;          /* length of data following cmd id */
  byte cmd_id;          /* Command ID indicates the packet type */
} tm_jgps_i_hdr_s_type;

/* EXTERNAL: format for common header between MS and SCU.
*/
typedef PACKED struct PACKED_POST
{
  byte service_id_lo[8];  /* ID for service Identification, "ST" fixed now */
  byte service_id_hi[8];  /* ID for service Identification, "ST" fixed now */
  byte prot_version[8];   /* Prot version between MS and SCU */
  byte length[16];        /* length of data following cmd id */
  byte cmd_id[8];         /* Command ID indicates the packet type */
} tm_jgps_ext_hdr_s_type;


/* Service Identification 
*/
#define TM_JGPS_SERV_ID_S                0x53 /* ASCII character "S" */
#define TM_JPGS_SERV_ID_T                0x54 /* ASCII character "T" */

/* Protocol Version 
*/
#define TM_JGPS_PROT_VER_MSA_ONLY         0x01
#define TM_JGPS_PROT_VER_MSB              0x02

/* Offsets for common MS/PM SCU header.
*/
#define TM_JGPS_HDR_SERVICE_ID_OFFSET        0
#define TM_JGPS_HDR_SERVICE_ID_OFFSET_L      0
#define TM_JGPS_HDR_SERVICE_ID_OFFSET_H      1
#define TM_JGPS_HDR_PROTOCOL_VERSION_OFFSET  2
#define TM_JGPS_HDR_LENGTH_OFFSET            3
#define TM_JGPS_HDR_CMD_ID_OFFSET            5
#define TM_JGPS_HDR_IND_DATA_OFFSET          6

#define TM_JGPS_HDR_SIZE  TM_JGPS_HDR_IND_DATA_OFFSET

/* Command ID field size */
#define TM_JGPS_CMD_ID_SIZE 1

/*----------------------------------------------------------------------------

    Command Identification
    ========================
    Data sent from the MS->SCU
    
    1. MS-A POS req: cmd_id = 0x11
    2. MS-B ACQ req: cmd_id = 0x30
  
    3. MS-A POS cancel req: cmd_id = 0x92
    4. MS-B ACQ cancel req: cmd_id = 0x34

    Data sent from the SCU->MS
    
    5. MS-A POS stop req: cmd_id = 0x91
    6. MS-A ACQ stop req: cmd_id = 0x33
-----------------------------------------------------------------------------*/

/* Command Identification type
*/
typedef byte tm_jgps_scu_cmd_id_type;

/* Positioning Request (MS-A). MS->SCU.
*/
#define TM_JGPS_CMD_ID_START_POS_REQUEST   0x11
/* Acquisition Request (MS-B). MS->SCU.
*/
#define TM_JGPS_CMD_ID_START_ACQ_REQUEST   0x30
/* POS start response, SCU->MS
*/
#define TM_JGPS_CMD_ID_POS_RESPONSE 0x13
/* ACQ start response, SCU->MS
*/
#define TM_JGPS_CMD_ID_ACQ_RESPONSE  0x31
/* POS complete response, SCU->MS.
*/
#define TM_JGPS_CMD_ID_POS_COMPLETE 0x14
/* ACQ Complete response, MS->SCU.
*/
#define TM_JGPS_CMD_ID_ACQ_COMPLETE 0x32
/* Cancellation of position request. MS->SCU.
*/
#define TM_JGPS_CMD_ID_CANCEL_POS_REQUEST  0x92
/* Cancellation of acquisition request. MS->SCU.
*/
#define TM_JGPS_CMD_ID_CANCEL_ACQ_REQUEST  0x34
/* Stop position request message. SCU->MS.
*/
#define TM_JGPS_CMD_ID_STOP_POS_REQUEST 0x91
/* Stop acquisition reqeust message, SCU->MS.
*/
#define TM_JGPS_CMD_ID_STOP_ACQ_REQUEST 0x33
/* IS801 messages reverse link msgs
*/
#define TM_JGPS_CMD_ID_IS801_RLINK 0x10
/* IS801 messages forward link msgs
*/
#define TM_JGPS_CMD_ID_IS801_FLINK 0x20


/* Result and cause of response */
#define TM_JGPS_SCU_MS_RESULT_OK 0
#define TM_JGPS_SCU_MS_RESULT_NG 1
#define TM_JGPS_SCU_MS_CAUSE_OK  0

/* Error causes */
#define TM_JGPS_SCU_CAUSE_NG_LIS     0x01
#define TM_JGPS_SCU_CAUSE_NG_LSU     0x02
#define TM_JGPS_SCU_CAUSE_NG_MSI     0x03
#define TM_JGPS_SCU_CAUSE_NG_TO      0x21

/*----------------------------------------------------------------------------

    Request Data Format:
    ========================
    Either a positioning request (MS-A) or acquisition request (MS-B) is
    sent from the MS->SCU.  The request data is appended to the J-GPS 
    header with a specific cmd_id 

    1. MS-A POS req: cmd_id = 0x11
    2. MS-B ACQ req: cmd_id = 0x30


-----------------------------------------------------------------------------*/

/* MS Phone number max size */
#define TM_JGPS_MAX_MSNUM 6

/* URL max size */
#define TM_JGPS_MAX_URL   260

/* PN max size */
#define TM_JGPS_MAX_ASET  6

/* INTERNAL: MS info data struct for PN and ECIO values
*/
typedef struct
{
  word pn;
  int8 ecio;
} tm_jgps_msinfo_pn;

/* INTERNAL: Data structure which defines the POS req or ACQ req 
**           info sent from the MS->SCU. 
*/
typedef struct
{
  char                msnum[TM_JGPS_MAX_MSNUM*2];
  dword               ipaddr;
  dword               esn;
  int8                tx;
  byte                fer;
  int8                rssi;
  byte                num_pn;
  tm_jgps_msinfo_pn   pn[TM_JGPS_MAX_ASET];
  word                ch;
  byte                url_length;
  char                url[TM_JGPS_MAX_URL];
} tm_jgps_i_ms_req_s_type;


/* EXTERNAL: MS phone number for POS req or ACQ req (MS->SCU)
*/
typedef PACKED struct PACKED_POST
{
  byte msnum[4];
} tm_jgps_msnum_s_type;

/* EXTERNAL: RF info (esn, tx, fer, rssi) for POS req or ACQ req (MS->SCU)
*/
typedef PACKED struct PACKED_POST
{
  byte esn[32];
  byte tx[8];
  byte fer[8];
  byte rssi[8];
} tm_jgps_rf_fix_s_type;

/* EXTERNAL: RF info (PN, ECIO) for POS req or ACQ req (MS->SCU)
*/
typedef PACKED struct PACKED_POST
{
  byte pn[16];
  byte ecio[8];
} tm_jgps_rf_var_s_type;

/* EXTERNAL: RF info data type for POS req or ACQ req (MS->SCU)
*/
typedef PACKED struct PACKED_POST
{
  tm_jgps_rf_fix_s_type   fix;
  tm_jgps_rf_var_s_type   var[TM_JGPS_MAX_ASET];
  byte                    ch[16];
} tm_jgps_rf_info_s_type;

/* EXTERNAL: URL data type for POS req or ACQ req (MS->SCU)
*/
typedef PACKED struct PACKED_POST
{
  byte url[8];
} tm_jgps_url_s_type;

/* EXTERNAL: format for POS req or ACQ req info sent from the MS->SCU. 
*/
typedef PACKED struct PACKED_POST
{
  tm_jgps_msnum_s_type      msnum[TM_JGPS_MAX_MSNUM*2];
  byte                      ipaddr[32];
  tm_jgps_rf_info_s_type    rf;
  byte                      url_length[8];
  tm_jgps_url_s_type        url[255];
} tm_jgps_ext_req_s_type;


/* Offsets for common MS/PM SCU header.
*/
#define TM_JGPS_REQ_MSNUM_OFFSET       0
#define TM_JGPS_REQ_IP_ADDRESS_OFFSET  6
#define TM_JGPS_REQ_SCU_RF_OFFSET      10
#define TM_JGPS_REQ_ULENGTH_OFFSET     37
#define TM_JGPS_REQ_URL_OFFSET         38

#define TM_JGPS_REQ_ULENGTH_MAX        255
#define TM_JGPS_REQ_ULENGTH_NULL       1

/* URL parameters in Postion request structure
*/

/* Protocol Version
*/
#define TM_JGPS_URL_VER1  1

/* Positioning System
*/
#define TM_JGPS_URL_DATUM_WGS84  0
#define TM_JPGS_URL_DATUM_TOKYO  1

/* Request Format
*/
#define TM_JGPS_URL_UNIT_DMS     0
#define TM_JGPS_URL_UNIT_DEGREE  1

/* Request Accuracy
*/
#define TM_JGPS_URL_ACRY_FREE   0
#define TM_JGPS_URL_ACRY_MAX    1
#define TM_JGPS_URL_ACRY_AVG    2
#define TM_JGPS_URL_ACRY_MIN    3

/* Number of Positioning Requests
*/
#define TM_JGPS_URL_NUM_FIX_DEFAULT  0

/* System ID, HDR or 1x
*/
#define TM_JGPS_SYSTEM_ID_1X  0x00
#define TM_JGPS_SYSTEM_ID_HDR 0x01

/* Software Revision
*/
#define TM_JGPS_MS_SW_REV     0x00

/*----------------------------------------------------------------------------

      Message Format
      ========================
      Packed Data structure which defines the common header between MS and SCU. 

      7         0 (bits)        Description
      --------------           ------------------------------------------
     0| Header     |            Specific Japan-GPS header which pre-pends
       . . .                    all packet exchange between MS<->SCU
     6|            |            (6 Bytes)
      -------------- 
      | Data       |            POS req, ACQ req, POS complete, POS suspend
      |            |            etc.
      | (Variable) |
         . . . 
  (Bytes)

-----------------------------------------------------------------------------*/
/* The largest packet send out is Positioning request, so allocate memory based on that.
*/
#define TM_JGPS_MAX_DATA_SIZE   (TM_JGPS_REQ_ULENGTH_OFFSET + TM_JGPS_REQ_ULENGTH_MAX + TM_JGPS_REQ_ULENGTH_NULL)


/* Data structure defines message format between MS and SCU. For now create a PACKED structure. Sanjeev
*/
typedef struct {
  tm_jgps_i_hdr_s_type   hdr;                          /* Common SCU-MS header */
  byte                   data[TM_JGPS_MAX_DATA_SIZE];  /* payload data */
} tm_jgps_packet_s_type;


/*----------------------------------------------------------------------------

    Response Format (SCU->MS)
    ========================
    Either for POS resp or ACQ resp from MS.
    
-----------------------------------------------------------------------------*/
/* INTERNAL: POS response, SCU->MS.
*/
typedef struct {
  byte result;
  byte cause;
} tm_jgps_i_pos_resp_s_type;

/* EXTERNAL: POS response, SCU->MS.
*/
typedef PACKED struct PACKED_POST
{
  byte result[8];
  byte cause[8];
} tm_jgps_ext_pos_resp_s_type;
#define TM_JGPS_EXT_POS_RESP_S_TYPE_SZ   2


#define TM_JGPS_SCU_MS_POS_RESP_RESULT_OK   TM_JGPS_SCU_MS_RESULT_OK     /* same OK type for all responses */
#define TM_JGPS_SCU_MS_POS_RESP_RESULT_NG   TM_JGPS_SCU_MS_RESULT_NG     /* same NG type for all responses */
#define TM_JGPS_SCU_MS_POS_RESP_CAUSE_OK    TM_JGPS_SCU_MS_CAUSE_OK     /* same OK type for all responses */
/* Note no special handling for different cause codes, so we have 1 define which takes care of all
** error cases.
*/



/*----------------------------------------------------------------------------

    Position Completion Format (SCU->MS)
    ==========================

-----------------------------------------------------------------------------*/
/* INTERNAL: POS complete SCU->MS.
*/
typedef struct {
  byte result;                          /* Result of positioning */
  byte cause;                           /* Reason of Pos result */
  byte flength;                         /* data length of fix data */
  char data[255];                       /* pos result information .see pm_scu_ms_pos_fixdata_s_type.*/
} tm_jgps_i_pos_comp_s_type;

/* EXTERNAL: POS complete SCU->MS.
*/
typedef PACKED struct PACKED_POST
{
  byte result[8];
  byte cause[8];
  byte flength[8];
} tm_jgps_ext_pos_comp_s_type;
#define TM_JGPS_EXT_POS_COMP_S_TYPE_SZ   3


/* defines particular to pos complete response, SCU=>MS.
*/
#define PM_SCU_MS_CMD_ID_POS_COMPLETE 0x14

#define TM_JGPS_SCU_MS_POS_COMP_RESULT_OK     TM_JGPS_SCU_MS_RESULT_OK   /* Common RESULT OK for all responses */
#define TM_JGPS_SCU_MS_POS_COMP_CAUSE_OK      TM_JGPS_SCU_MS_CAUSE_OK    /* Common RESULT OK for all responses */


#define PM_SCU_MS_CMD_ID_ACQ_COMPLETE 0x32

/* POS result information data lengths */
#define TM_JGPS_MAX_VER_LEN     1
#define TM_JGPS_MAX_DATUM_LEN   1
#define TM_JGPS_MAX_UNIT_LEN    1
#define TM_JGPS_MAX_LAT_LEN     13
#define TM_JGPS_MAX_LON_LEN     13
#define TM_JGPS_MAX_ALT_LEN     4
#define TM_JGPS_MAX_TSTAMP_LEN  14
#define TM_JGPS_MAX_SMAJ_LEN    4
#define TM_JGPS_MAX_SMIN_LEN    4
#define TM_JGPS_MAX_VERT_LEN    4
#define TM_JGPS_MAX_MAJAA_LEN   3
#define TM_JGPS_MAX_FM_LEN      1


/*----------------------------------------------------------------------------

    Cancellation Format (MS->SCU)
    ====================
    Either CANCEL POS req or ACQ req from MS to SCU


    1. MS-A POS cancel req: cmd_id = 0x92
    2. MS-B ACQ cancel req: cmd_id = 0x34
-----------------------------------------------------------------------------*/
/* INTERNAL: Cancellation of Request. MS=>SCU.
*/
typedef struct {
  byte   cause;
} tm_jgps_i_cancel_req_s_type;

/* EXTERNAL: Cancellation of request MS->SCU.
*/
typedef PACKED struct PACKED_POST
{
  byte cause[8];
} tm_jgps_ext_cancel_req_s_type;
#define TM_JGPS_EXT_CANCEL_REQ_S_TYPE_SZ   1

#define TM_JGPS_CANCEL_POS_CAUSE_OFFSET         0 

/* Cause for cancellation by MS
*/
#define TM_JGPS_CANCEL_CAUSE_USR_DISCONNECT  0x20
#define TM_JGPS_CANCEL_CAUSE_TIMEOUT         0x21
#define TM_JGPS_CANCEL_CAUSE_REJECT_LR       0x22
#define TM_JGPS_CANCEL_CAUSE_REJECT_ASSIST   0x23
#define TM_JGPS_CANCEL_CAUSE_REJECT_EPH      0x24
#define TM_JGPS_CANCEL_CAUSE_REJECT_ALM      0x25
#define TM_JGPS_CANCEL_CAUSE_OTHERS          0xFF


/*----------------------------------------------------------------------------

    Stop Request Format (SCU->MS)
    ====================
    Either STOP POS or ACQ receieved from SCU


    1. MS-A POS stop req: cmd_id = 0x91
    2. MS-B ACQ stop req: cmd_id = 0x33
-----------------------------------------------------------------------------*/
/* INTERNAL: Stop request. SCU->MS
*/
typedef struct {
  byte cause;                  /* reason for Pos stop request */
} tm_jgps_i_stop_req_s_type;

/* EXTERNAL: Stop request MS->SCU.
*/
typedef PACKED struct PACKED_POST
{
  byte cause[8];
} tm_jgps_ext_stop_req_s_type;

/* Cause for suspension by SCU
*/
#define TM_JGPS_SCU_STOP_CAUSE_OK             SCU_MS_CAUSE_OK
#define TM_JGPS_SCU_STOP_CAUSE_SYS_ABN        0x01
#define TM_JGPS_SCU_STOP_CAUSE_MS_INFO_NG     0x03
#define TM_JGPS_SCU_STOP_CAUSE_TO             0x021


/*----------------------------------------------------------------------------

    Timers
    ====================

-----------------------------------------------------------------------------*/

/* T02 timer for response from SCU to POS or ACQ request
*/
#define TM_JGPS_SCU_T02_TIMER_CNT  5000    /* 5 sec timer */


/* T04 timer for Positioning completion
*/
#define TM_JGPS_SCU_T04_TIMER_CNT  120000   /* 120 sec timer */

/* RF info request timer for POS or ACQ request
*/
#define TM_JGPS_RF_INFO_REQ_TIMER_CNT  3000   /* 3 sec timer */


/* MS IP address request timer for POS or ACQ request
*/
#define TM_JGPS_MS_IP_REQ_TIMER_CNT  3000   /* 3 sec timer */


/*----------------------------------------------------------------------------

    Monitor Mode: Arrange data 
    ==========================

-----------------------------------------------------------------------------*/ 

/* Pilot num */
#define TM_JGPS_PPM_PN_MIN 1
#define TM_JGPS_PPM_PN_MAX 16
#define TM_JGPS_PPM_PN_ALL 0xFF

/* SV num */
#define TM_JGPS_PRM_PRN_MIN 0
#define TM_JGPS_PRM_PRN_MAX 16
#define TM_JGPS_PRM_PRN_ALL 0xFF

/* Sort type */
#define TM_JGPS_SORT_LOW   0
#define TM_JGPS_SORT_HIGH  1


/*=============================================================================

                                Extern Functions

=============================================================================*/


/*===========================================================================

FUNCTION TM_JGPS_XLATE_INT_MS_SCU_HDR

DESCRIPTION
  This function converst internal SCU header to external format.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
extern void tm_jgps_xlate_int_ms_scu_hdr
(
  byte *buf_ptr,
    /* Pointer to where translated message should be placed */
  tm_jgps_i_hdr_s_type *hdr_ptr
    /* Pointer to internal message to be transmitted */
);


/*===========================================================================

FUNCTION TM_JGPS_XLATE_EXT_MS_SCU_HDR

DESCRIPTION
  This function converst external SCU header to internal format.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
extern void tm_jgps_xlate_ext_ms_scu_hdr
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  tm_jgps_i_hdr_s_type *hdr_ptr
    /* Pointer to place where translated message should be placed */
);


/*===========================================================================

FUNCTION TM_JGPS_XLATE_INT_MS_SCU_REQ

DESCRIPTION
  This function converst internal SCU RF information to external format.
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
extern void tm_jgps_xlate_int_ms_scu_req
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  tm_jgps_i_ms_req_s_type *int_ptr
    /* Pointer to place where translated message should be placed */
);


/*===========================================================================

FUNCTION TM_JPGS_XLATE_EXT_RESP_POS_RESP

DESCRIPTION
  This function converst the external Position response from external to
  internal format.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
extern void tm_jgps_xlate_ext_pos_resp
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  tm_jgps_i_pos_resp_s_type *int_ptr
    /* Pointer to place where translated message should be placed */
);


/*===========================================================================

FUNCTION TM_JGPS_XLATE_EXT_RESP_POS_COMP

DESCRIPTION
  This function translates external Position completion response from 
  external to internal format.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
extern void tm_jgps_xlate_ext_pos_comp
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  tm_jgps_i_pos_comp_s_type *int_ptr
    /* Pointer to place where translated message should be placed */
);


/*===========================================================================

FUNCTION TM_JGPS_XLATE_INT_CANCEL_REQ

DESCRIPTION
  This function converst external SCU Cancel Request message to internal format 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
extern void tm_jgps_xlate_int_cancel_req
( 
  byte *buf_ptr, 
    /* Pointer to where translated message should be placed */
  tm_jgps_i_cancel_req_s_type *int_ptr
    /* Pointer to internal message to be transmitted */ 
);


/*===========================================================================

FUNCTION TM_JGPS_XLATE_EXT_STOP_REQ

DESCRIPTION
  This function converst external SCU stop Request message to internal format 
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None 

===========================================================================*/
extern void tm_jgps_xlate_ext_stop_req
( 
  byte *buf_ptr,
    /* Pointer to where translated message should be placed */
  tm_jgps_i_stop_req_s_type *int_ptr
    /* Pointer to external message to be translated */
);


#endif /* FEATURE_JCDMA_GPSONE */
  
#ifdef __cplusplus
}
#endif

#endif /* TM_JGPS_MSG_H */
