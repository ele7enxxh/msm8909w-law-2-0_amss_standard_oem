/*****************************************************************************
***
*** TITLE
***
***   GPRS SNDCP LAYER DEFINITIONS HEADER FILE
***   
***
*** DESCRIPTION
***
***  Definitions and macros for the SNDCP layer
***
*** 
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/inc/gsndef.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz	   changes
***
*****************************************************************************/

#ifndef INC_GSNDEF_H
#define INC_GSNDEF_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "err.h"
#include "msg.h"
#include "gmutex.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/* Enable SNDCP debug information */
/* #define GSN_DEBUG */


/* Miscellaneous */
#define ONE_BYTE            1
#define XID_BYTE_SIZE       8
#define NO_COMP_ENT         0xff
#define NO_COMP_VAL         0xff
#define GSN_MAX_PDU_SIZE    1502 /* See TS 23.060 par 9.3 */

/* SN-UNITDATA_REQ primitives conveying less than 100 bytes 
** are not worth forwarding to a V42bis encoder. This value 
** was chosen empirically after testing a few commercial GPRS handsets.
*/
#define UNACK_V42BIS_MIN_COMPRESSIBLE_LOAD  100

#define XID_REQ_MAX         3    /* Max times of reinitiating XID requests */

#define GSN_INVAL_SAPI      0xff
#define GSN_INVAL_NSAPI     0xff

/* Maximum number of simultaneous N-PDUs that can be processed 
** (buffered for retransmission) per NSAPI before PDP to SNDCP 
** UL data flow is suspended.
*/
#define GPRS_MAX_RETR_NPDUS   16   

/* Start of the SM to SNDCP enumerated primitives */
#define GSN_SM_MSG          0x02000000

/* SNDCP Up-Link entity suspend causes */
#define GSN_UL_SUSP_CTS_OFF   0x01
#define GSN_UL_SUSP_XID_PEND  0x02
#define GSN_UL_SUSP_EST_PEND  0x04
#define GSN_UL_SUSP_SEQ_PEND  0x08
#define GSN_UL_SUSP_MAX_NPDU  0x10
/* When XID reset is received, UL is suspended until LL_RESET_RESP is 
   received. */
#define GSN_UL_SUSP_XID_RESET 0x20

/* When LLC receives LLGMM-SUSPEND-REQ, it suspends UL flow until
   LLGMM-RESUME-REQ is received. */
#define GSN_UL_SUSP_NAS_SUSP  0x40

/* 
** Used to mask out the assosiated suspend event
*/
#define GSN_UL_SUSP_MAX_NPDU_MASK 0xef
#define GSN_UL_SUSP_SEQ_PEND_MASK 0xf7

/* Used to mask for all suspend events */
#define GSN_UL_SUSP_ALL_REASONS_MASK   0x7F

/* DCOMP value 0 and PCOMP value 0 are reserved permanetely for no compression */
#define PCOMP0_VAL          0
#define DCOMP0_VAL          0

/* XID type parameter header byte positioning relative to the 
** beginning of an XID type in an XID block. 
*/
#define XID_TYPE_PARAM_HDR_POS  0
#define XID_LEN_PARAM_HDR_POS   1

/*------------------------------------------------------------- 
  Min, max and default values of compression field parameters 
  See TS 04.65 par 6.5 and 6.6
--------------------------------------------------------------*/

/* Applicable NSAPIs parameter. Applicable to all algorithms */
#define APP_NSAPIS_LEN     2 /* Parameter's length in Bytes */ 
#define APP_NSAPIS_MIN     0
#define APP_NSAPIS_MAX     65504
#define APP_NSAPIS_DEF     0
#define APP_NSAPIS_MASK    0xffe0

/* 
**  V.42 bis compression parameters 
*/

/* Lenght of V.42 bis parameters */
#define V42BIS_LEN_P0_MAX     6 /* Max len if p = 0 */
#define V42BIS_LEN_P1_MAX     7 /* Max len if p = 1 */

/* DCOMP values assigned to V42 bis comp algorithm */
#define DCOMP1_VAL            1

#define V42BIS_DCOMP_MAX      1

#define V42BIS_P0_LEN         1 /* Parameter's length in Bytes */ 
#define V42BIS_P0_MIN         0
#define V42BIS_P0_MAX         3
#define V42BIS_P0_DEF         3
#define V42BIS_P0_MASK        0x03

/* V42bis P0 parameter values */
#define GSN_V42_NO_COMPRESSION    0x00
#define GSN_V42_MS_TO_SGSN        0x01
#define GSN_V42_SGSN_TO_MS        0x02
#define GSN_V42_BOTH_DIR          0x03

#define V42BIS_P1_LEN         2 /* Parameter's length in Bytes */ 
#define V42BIS_P1_MIN         512
#define V42BIS_P1_MAX         4096 /* Memory restrictions */
#define V42BIS_P1_DEF         512 /* From IOT with Er.. */


#define V42BIS_P2_LEN         1 /* Parameter's length in Bytes */ 
#define V42BIS_P2_MIN         6
#define V42BIS_P2_MAX         250
#define V42BIS_P2_DEF         20

/*
**   RFC 1144 compression parameters 
*/  
      
/* Length of RFC1144 parameters */
#define RFC1144_LEN_P0_MAX    3 /* Max len if p = 0 */
#define RFC1144_LEN_P1_MAX    4 /* Max len if p = 1 */

#define RFC1144_PCOMP_MAX     2

#define RFC1144_S0_1_LEN      1 /* Parameter's length in Bytes */
#define RFC1144_S0_1_MIN      0
#define RFC1144_S0_1_MAX      255
#define RFC1144_S0_1_DEF      5 /* From IOT with Er... */


/* 
**  RFC 2507 compression parameters 
*/        
/* Lenght of RFC2507 parameters */
#define RFC2507_LEN_P0_MAX    9
#define RFC2507_LEN_P1_MAX    12

#define RFC2507_PCOMP_MAX     5

/* F_MAX_PERIOD parameter */
#define RFC2507_MAX_PER_MIN   1
#define RFC2507_MAX_PER_MAX   65535
#define RFC2507_MAX_PER_DEF   256
#define RFC2507_MAX_PER_LEN   2

/* F_MAX_TIME parameter */
#define RFC2507_MAX_TIME_MIN  1
#define RFC2507_MAX_TIME_MAX  255
#define RFC2507_MAX_TIME_DEF  5
#define RFC2507_MAX_TIME_LEN  1 
 
/* MAX_HEADER parameter */
#define RFC2507_MAX_HDR_MIN   60
#define RFC2507_MAX_HDR_MAX   255  /* According to RFC2507 it should be 65535. TBC */
#define RFC2507_MAX_HDR_DEF   168
#define RFC2507_MAX_HDR_LEN   1

/* TCP_SPACE parameter */
#define RFC2507_TCP_SP_MIN    3
#define RFC2507_TCP_SP_MAX    255
#define RFC2507_TCP_SP_DEF    15
#define RFC2507_TCP_SP_LEN    1

/* NON_TCP_SPACE parameter */
#define RFC2507_NTCP_SP_MIN   3
#define RFC2507_NTCP_SP_MAX   65535
#define RFC2507_NTCP_SP_DEF   15
#define RFC2507_NTCP_SP_LEN   2


/* 
**  RFC 3095 compression parameters 
*/        
/* Lenght of RFC3095 parameters. Only 3 profiles are supported initially */
#define RFC3095_LEN_P0_MAX    14
#define RFC3095_LEN_P1_MAX    15

#define RFC3095_PCOMP_MAX     2

/* MAX_CID parameter */
#define RFC3095_MAX_CID_MIN   0
#define RFC3095_MAX_CID_MAX   16383
#define RFC3095_MAX_CID_DEF   15
#define RFC3095_MAX_CID_LEN   2

/* MAX_HEADER parameter */
#define RFC3095_MAX_HDR_MIN   60
#define RFC3095_MAX_HDR_MAX   255
#define RFC3095_MAX_HDR_DEF   168
#define RFC3095_MAX_HDR_LEN   2 
 
#define RFC3095_PROF_MIN      0
#define RFC3095_PROF_MAX      65535
#define RFC3095_PROF_LEN      2

/* Supported ROHC profiles */
#define RFC3095_NOCOMP_PROF   0
#define RFC3095_RTP_PROF      1
#define RFC3095_UDP_PROF      2
#define RFC3095_ESP_PROF      3


/* PCOMP values assigned to comp algorithm */
#define PCOMP1_VAL            1
#define PCOMP2_VAL            2
#define PCOMP3_VAL            3
#define PCOMP4_VAL            4
#define PCOMP5_VAL            5

/* 
** Length, in bytes, of PCOMP/DCOMP values 
*/
#define V42BIS_DCOMP_LEN      1
#define RFC1144_PCOMP_LEN     1
#define RFC2507_PCOMP_LEN     3
#define RFC3095_PCOMP_LEN     1


/* Length in bytes of an XID type header. 1st byte holds parameter type
** and the 2nd byte holds the length of the parameter
*/
#define XID_TYPE_HDR_LEN          2

/* Length in bytes of a compression field header when P = 1.
*/
#define XID_FIELD_HDR_P1_LEN      3

/* Length in bytes of a compression field header when P = 0.
*/
#define XID_FIELD_HDR_P0_LEN      2

/* Max length in bytes of type 0 XID feild 
*/
#define XID_T0_LEN_MAX            3

/* Max length in bytes of type 1 XID field. 
*/
#define XID_T1_LEN_MAX        ( V42BIS_LEN_P1_MAX + XID_FIELD_HDR_P1_LEN )

/* Max length in bytes of an type 2 XID field. 
*/
#define XID_T2_LEN_MAX        ( RFC3095_LEN_P1_MAX + XID_FIELD_HDR_P1_LEN )

/* Maximum entity number 
*/
#define ENT_NUM_MAX               31

/* Maximum DCOMP value 
*/
#define DCOMP_VAL_MAX             15

/* Maximum PCOMP value
*/
#define PCOMP_VAL_MAX             15

/* Maximum no of profiles supported by RFC3095 implementation */
#define RFC3095_MAX_NO_OF_PROF     3


/* Data compression algorithms. See TS 04.65 table 6 
*/ 
#define T1_ALG_V42BIS     0
#define T1_ALG_NONE       0xFF 
/* --- Add more Type 1 algorithms here ------ */

/* Data compression algorithm type range. 
** Only V.42 bis supported. See Table 6 TS 04.65.  
** Update T1_ALG_MAX if more data comp algorithms are added
*/
#define T1_ALG_MAX        T1_ALG_V42BIS 

/* Protocol Control Information algorithms. See Table 4 R99 TS 04.65.*/ 
#define T2_ALG_RFC1144    0
#define T2_ALG_RFC2507    1
#define T2_ALG_RFC3095    2
#define T2_ALG_NONE       0xFF
/* --- Add more Type 2 (PCI) algorithms here ------ */

/* Update T2_ALG_MAX if more algorithms are added
*/
#define T2_ALG_MAX        T2_ALG_RFC3095
    
/* SNDCP XID parameters. See TS 04.65 Table 8. */
#define XID_TYPE_0        0   /* SNDCP Vertion Number */
#define XID_TYPE_1        1   /* Data Compression */
#define XID_TYPE_2        2   /* PCI Compression */
#define XID_TYPE_MAX      XID_TYPE_2

/* Default value of SNDCP vertion number 
*/
#define GSN_VER_NUM_DEF   0
#define GSN_VER_NUM_1     1
#define GSN_VER_NUM_LEN   1


/*-------------------------
**  TS 04.65 par 7.2  
---------------------------*/

/* Size in octets of Ack SN-PDU header of the first segment of an N-PDU */
#define ACK_PDU_HDR_1ST_SEG        3 

/*  Size in octets of Ack SN-PDU header of not the first segment of an N-PDU 
**  PCOMP, DCOMP and N-PDU num is not included in the header 
*/
#define ACK_PDU_HDR_NOT_1ST_SEG    1

/* Size in octets of Unck SN-PDU header of the first segment of an N-PDU */
#define UNACK_PDU_HDR_1ST_SEG      4

/*  Size in octets of Unack SN-PDU header of not the first segment of an N-PDU. 
**  PCOMP and DCOMP is not included in the header. 
*/
#define UNACK_PDU_HDR_NOT_1ST_SEG  3

/* More bit (M) values in SN-PDU */ 
#define SN_LAST_SEG           0
#define SN_MORE_SEG           1

/* SN-PDU Type (T) values in SN-PDU */
#define SN_DATA_PDU           0
#define SN_UNITDATA_PDU       1

/* First segment indicator bit (F) values in SN-PDU */
#define SN_FIRST_SEG          1
#define SN_NOT_FIRST_SEG      0

/* Maximum N-PDU number in Ack mode */
#define ACK_NPDU_NUM_MAX      256

/* Maximum N-PDU number in Unack mode */
#define UNACK_NPDU_NUM_MAX    4096

/* Maximum number of Unack N-PDU segments */
#define UNACK_SEG_NUM_MAX     16

/* Max length in bytes of an XID block. This value is derived from 
** LLC layer that has a byte variable to represent the XID block length
*/
#define XID_BLOCK_LEN_MAX     0xff

/* Position of F bit in the first byte of the SN-PDU header*/
#define F_BIT_POS             6

/* Position of M bit in the first byte of the SN-PDU header*/
#define M_BIT_POS             4 

/* Assuming default T201 timer values are used, this timer is set to 
** 160 seconds. This is derived as: (1 + N200)* (default T201 for SAPI-11))
** = ( 1 + 3 ) * ( 40 seconds) = 160 seconds.
*/
#define SGSN_INITIATED_LINK_EST_FAIL_TMR_VALUE 160000



/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/* Translates external NSAPI number to SNDCP internal NSAPI number */
#define GSN_INTERN_NSAPI( nsapi )   ((gsn_nsapi_t)((nsapi) - 5))

/* Translates internal SNDCP NSAPI number to external NSAPI number */
#define GSN_EXTERN_NSAPI( nsapi )   ((uint8) (nsapi) + 5)

/* Check if valid NSAPI number */
#define GSN_VALID_NSAPI( nsapi )  \
  ( (((nsapi) < 5) || ((nsapi) > 15)) ? (FALSE) : (TRUE) ) 

#define GSN_VALID_SAPI( sapi )    \
  ( (((sapi) == 3) || ((sapi) == 5) || ((sapi) == 9) || ((sapi) == 11)) ? (TRUE) : (FALSE) ) 

#define GSN_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR_FATAL( "Assertion " #xx_exp " failed", 0, 0, 0 ); \
     }

/* Wrapper to de-allocate memory that has been allocated
** using macro GPRS_MEM_MALLOC() 
*/
#define GSN_MEM_FREE( mem )  \
      { \
        GPRS_MEM_FREE( mem ); \
        mem = NULL;           \
      }

/* SNDCP Error massages  
*/
#define GSN_MSG_LOW( str, a, b, c )       MSG_LOW( "GSN " str, a, b, c )
#define GSN_MSG_MED( str, a, b, c )       MSG_MED( "GSN " str, a, b, c )
#define GSN_MSG_HIGH( str, a, b, c )      MSG_HIGH( "GSN " str, a, b, c )
#define GSN_MSG_ERR( str, a, b, c )       MSG_ERROR( "GSN " str, a, b, c )
#define GSN_ERR_FATAL( str, a, b, c )     ERR_FATAL( "GSN " str, a, b, c )


#endif /* INC_GSNDEF_H */

/*** EOF: don't remove! ***/
