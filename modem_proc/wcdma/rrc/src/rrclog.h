#ifndef RRCLOG_H
#define RRCLOG_H
/*===========================================================================
              R R C L O G  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with the
  functions for RRC Logging.
  
Copyright (c) 2000, 2003, 2005-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrclog.h_v   1.5   06 Dec 2000 10:21:16   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrclog.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/16   nr      Adding log packet for standalone HRB db
05/12/16   vs      Adding log packet for measurement related logging
05/09/16   nr      Adding log packet for hanging RB logging
06/09/14   db      RRC Logging Changes for CMAC and CRLC
05/08/09   ss      Updated Copyright Information
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
12/19/05   da      Added definition for rb data log packet.  prototype for fn
                   rrc_send_rb_rate_info_packet()
03/10/05   vn      Fixed 3GPP rev under FEATURE_RRC_ASN1_REL5
01/13/01   ram     Updated copyright.
12/06/00   rj      Added mapping of BCCH logical channel to both  BCH and
                   FACH transport Channels.
12/05/00   rj      Added rrclog_log_ch_e_type. The formal parameter for 
                   function rrclog_log_sig_msg is  changed from uint16 to
                   rrclog_log_ch_e_type
11/30/00   rj      Added $Header to the EDIT HISTORY 
11/30/00   rj      Added more comments and absolute values are replaced 
                   with #define constants.
11/17/00   rj      deleted rrclog_read_from_file. 
                   Changed rrclog_write_to_file function name to 
                   rrclog_log_sig_msg.
                   Changed rrclog_close_file to rrclog_close_log_file.
                   Changed rrclog_create_file to rrclog_create_log_file.
                   File Pointer is removed as formal parameter for the above 
                   functions. 
11/14/00   rj      Created file.


===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "comdef.h"
#include "log.h"
#include "uecomdef.h"
#include "wl2api.h"
#include "l1rrcif.h"

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

#define SDU_HDR_RES_LENGTH   8             /* SDU Header length in bytes */
#define TIME_STAMP_LENGTH    8             /* Time Stamp length in bytes */
#define TAG_LENGTH           4             /* File Header Tag length in bytes*/
#define FILE_HDR_RES_LENGTH  6             /* File Header length in bytes */

#define FILE_FORMAT_VER      1             /* File Format Version */

#define MAJOR_SPEC_RELEASE   6             /* Major Specification release */
#define MINOR_SPEC_RELEASE   8             /* Minor Specification release */
#define MINOR_MINOR_SPEC_RELEASE 0         /* Minor minor Spec release */


/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

/*--------------------------------------------------------------------------- 
  Type for defining the status of a logical channel entity.  
---------------------------------------------------------------------------*/

 
/*===========================================================================
**                     TYPE DECLARATIONS
**=========================================================================*/
typedef enum
{
  RRCLOG_FILE_CLOSED,                       /* The Log file is closed */
  RRCLOG_FILE_OPENED                        /* The Log File is opened */
}rrclog_file_status_e_type;

typedef enum
{
  RRCLOG_FAILURE,                           /* Failure Status */
  RRCLOG_SUCCESS                            /* Success Status */
} rrclog_status_e_type;

typedef enum
{
  RRCLOG_BCCH_BCH = 0,                      /* BCCH Logical Channel mapped
                                               to BCH transport Channel*/
  RRCLOG_BCCH_FACH,                         /* BCCH Logical Channel mapped 
                                               to FACH transport Channel*/
  RRCLOG_PCCH,                              /* PCCH Logical Channel */
  RRCLOG_CCCH,                              /* CCCH Logical Channel */
  RRCLOG_DCCH                               /* DCCH logical Channel */
}rrclog_log_ch_e_type;

typedef struct
{
  uint16 log_ch;                            /* Logical Channel type */
  uint16 rb_id;                             /* Radio Bearer Id */
  uint32 num_bytes;                         /* SDU length */
  uint8 time_stamp[TIME_STAMP_LENGTH];      /* 8 bytes are reserved for
                                               time stamp.*/

  uint8 reserved_bytes[SDU_HDR_RES_LENGTH]; /* 8 bytes are reserved for 
                                               future expansion */
} rrclog_sdu_hdr_type;

typedef struct
{
  uint8 file_hdr_tag[TAG_LENGTH];            /* This is file header tag
                                                and set to "RRC ". */
  uint16 file_format_ver;                    /* File Format Version */
  uint8 major_spec;                          /* Major Specification release.*/
  uint8 minor_spec;                          /* Minor Specification release.*/
  uint8 minor_minor_spec;                    /* Minor minor Specification
                                                release .*/
  uint8 reserved_1;                          /* Reserved for funture 
                                                expansion */
  uint8 reserved_2[FILE_HDR_RES_LENGTH];     /* Resereved for future 
                                                expansion */
} rrclog_file_hdr_type;

/********* Definition for rb data log packet start *****************/
/* At present keeping this under R7 feature should be __GNUC__ actually. But lint cribs */
typedef PACKED struct PACKED_POST
{
  uint32 rb_id;
  uint32 dl_rb_rate;
  uint32 ul_rb_rate;
} rrc_rb_rate_type;


/* Log code for the UL MAC Configuration log packet */
#ifndef WCDMA_RB_RATE_INFO_LOG_PACKET
#define WCDMA_RB_RATE_INFO_LOG_PACKET     0x413A
#else
#error redefinition of LOG command code: WCDMA_RB_RATE_INFO_LOG_PACKET
#endif

LOG_RECORD_DEFINE(WCDMA_RB_RATE_INFO_LOG_PACKET)
    
    /* Number of RBs */
    uint8           num_rbs;   
    rrc_rb_rate_type rb_rate [1];  

LOG_RECORD_END



/* RB rate */
#define WCDMA_RB_RATE_INFO_PACKET_LEN(num_rbs)(\
  FPOS(WCDMA_RB_RATE_INFO_LOG_PACKET_type, rb_rate) +\
      (num_rbs * ( sizeof(uint32) + sizeof(uint32) + sizeof(uint32) ) ) \
  )

#define RRCLOG_UINT32(log_ptr,val)                     \
{                                                      \
  *(log_ptr++) = (uint8)(val & 0xFF);                  \
  *(log_ptr++) = (uint8)((val & 0xFF00) >> 8);         \
  *(log_ptr++) = (uint8)((val & 0xFF0000) >> 16);      \
  *(log_ptr++) = (uint8)((val & 0xFF000000) >> 24);    \
}
#define RRCLOG_UINT16(log_ptr,val)                     \
{                                                      \
  *(log_ptr++) = (uint8)(val & 0x00FF);                \
  *(log_ptr++) = (uint8)((val & 0xFF00) >> 8);         \
}

/********* Definition for rb data log packet end *****************/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION RRC_SEND_RB_RATE_INFO_PACKET

DESCRIPTION

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None. 

===========================================================================*/
void rrc_send_rb_rate_info_packet
(
void
);

/*===========================================================================

FUNCTION rrclog_create_log_file

DESCRIPTION
  This function creates and opens a new data file for write only.

DEPENDENCIES
  None

RETURN VALUE
  rrclog_status_e_type

SIDE EFFECTS
  None
===========================================================================*/

rrclog_status_e_type rrclog_create_log_file
(    
  const char *filename  
);

/*===========================================================================
FUNCTION rrclog_close_log_file

DESCRIPTION
  This function closes the file.

DEPENDENCIES
  None

RETURN VALUE
  rrclog_status_e_type

SIDE EFFECTS
  None
===========================================================================*/

rrclog_status_e_type rrclog_close_log_file
(    
  void
);

/*===========================================================================
FUNCTION rrclog_log_sig_msg

DESCRIPTION
  This function writes the SDU header first into binary file and then writes
  the SDU data into binary file.

DEPENDENCIES
  None

RETURN VALUE
  rrclog_status_e_type

SIDE EFFECTS
  None
===========================================================================*/

rrclog_status_e_type rrclog_log_sig_msg
(    
  void  *sdu,                                /* SDU data */
  uint32 sdu_size,                           /* SDU size in bytes */
  uint16 rb_id,                              /* Radio Bearer Id to which SDU
                                                belongs */
  rrclog_log_ch_e_type log_ch                /* Logical Channel type to which
                                                SDU belongs */           
);

/*===========================================================================

FUNCTION          RRCLOG_CMAC_DL_INFO

DESCRIPTION       Logs CMAC DL information passed to L2

DEPENDENCIES      NONE

RETURN VALUE      NONE
                  
SIDE EFFECTS      NONE

===========================================================================*/
void rrclog_cmac_dl_req
(
  wcdma_l2_dl_cmd_type  *l2_dl_cmd_ptr
);

/*====================================================================
FUNCTION: rrclog_crlc_am_req

DESCRIPTION:
  This function logs the acknowledged mode configuration passed to RLC.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrclog_crlc_am_req
(
  rlc_am_config_req_type *l2_cmd_ptr
);

/*====================================================================
FUNCTION: rrclog_cmac_ul_req

DESCRIPTION:
  Logs MAC uplink configuratio for R99 Channels(RACH and DCH)

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrclog_cmac_ul_req
(
  wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr
);

/*====================================================================
FUNCTION: rrclog_cmac_dl_hs_setup

DESCRIPTION:
  This function logs the HS configuration in downlink passed to MAC.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrclog_cmac_dl_hs_setup
(
  wcdma_l2_dl_cmd_type  *l2_dl_cmd_ptr
);

/*====================================================================
FUNCTION: rrclog_cmac_ul_hs_setup

DESCRIPTION:
  This function logs the HS configuration in uplink passed to MAC.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrclog_cmac_ul_hs_setup
(
  wcdma_l2_ul_cmd_type  *l2_ul_cmd_ptr
);

/*====================================================================
FUNCTION: rrc_hanging_rb_info

DESCRIPTION:
  This function logs the hanging rb information.
  Log code 0x4804

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrc_log_hanging_rb_info( void );

/*====================================================================
FUNCTION: rrc_log_standalone_hanging_rb_info

DESCRIPTION:
  This function logs the hanging rb information.
  Log code 0x4804

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrc_log_standalone_hanging_rb_info (void);

/*=========================================================================
FUNCTION: rrc_log_meas_info

DESCRIPTION:
  This function logs the measurments read from SIB11/12
  Log code 0x4805
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrc_log_meas_info
(
  l1_meas_sib_parm_struct_type* rrcmeas_sib_ptr,
  l1_meas_ctrl_parm_struct_type* rrcmeas_mcm_ptr,
  l1_meas_trans_type* rrcmeas_trans_ptr,
  l1_meas_ctrl_enum_type  meas_choice,
  l1_inter_freq_cell_list_struct_type* l1_inter_freq_sib_ptr
);
#endif /* RRCLOG_H */

/*=========================================================================*/
