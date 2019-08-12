#ifndef NVDIAG_H
#define NVDIAG_H
/*==========================================================================

                      Diagnostic Packet Definitions

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of diag packets must be PACKED.
  
  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2001-2009,2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/nv/src/nvdiag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/28/14   sri     Max trials of SPC unlock over diag handling...
03/04/13   sri     Removed support for NV_PEEK_F / NV_POKE_F cmds
09/01/09   pc      Made changes for Dual SIM / Dual Standby
05/28/09   pc      Exported unreadable_nv_item() and unwritable_nv_item()
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/23/07   as      Enabled pragma pack support for WINCE targets
12/14/06   as      Fixed critical lint errors.
07/25/05   as      Removed FEATURE_SIRIUS and mainlined multiple nv write.
05/04/05   as      Featurized nv_stat_enum_type to ensure uint16 for
                   external API.
04/11/05   as      Moved Pseudo-ESN and Pseudo-ESN mask to MC module.
03/14/05   as      Defined Pseudo-ESN and Pseudo-ESN mask.
12/23/04   ec      Added new structure definition DIAG_NV_WRITE2_F
08/20/01   jal     Updated for core VU
06/27/01   jal     Created file.

===========================================================================*/

#include "comdef.h"

#include "diagcmd.h"
#include "diagpkt.h"
#include "diagi.h"
#include "log_codes.h"
#include "nv.h"


/* -------------------------------------------------------------------------
** Packet Definitions
** ------------------------------------------------------------------------- */                           
#if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
#endif

#if defined(FEATURE_DIAG_SPC_TIMEOUT) && defined(FEATURE_DIAG_SPC_TTL)

#ifndef NVDIAG_SPC_TIME_OUT
#define NVDIAG_SPC_TIME_OUT 0x1C20000 /* 15secs in 32.768kHz units */
#endif
#ifndef DIAG_SPC_COUNT_MAX
#define DIAG_SPC_COUNT_MAX 15
#endif

#endif /* defined(FEATURE_DIAG_SPC_TIMEOUT) && defined(FEATURE_DIAG_SPC_TTL) */


/*==========================================================================

PACKET   DIAG_NV_READ_F

PURPOSE  Sent from the DM to the DMSS to request a read of an item
         from nonvolatile storage.

============================================================================*/
#define DIAG_NV_ITEM_SIZE 128

DIAGPKT_REQ_DEFINE(DIAG_NV_READ_F)
  
 uint16 item;                         /* Which item - use nv_items_enum_type */
 uint8  item_data[DIAG_NV_ITEM_SIZE]; /* Item itself - use nv_item_type      */
 uint16 nv_stat;                      /* Status of operation
                                                     - use nv_stat_enum_type */

DIAGPKT_REQ_END

DIAGPKT_DEFINE_RSP_AS_REQ(DIAG_NV_READ_F)


/*==========================================================================

PACKET   DIAG_NV_WRITE_F

PURPOSE  Sent from the DM to the DMSS to request a write of an item
         to nonvolatile storage.

============================================================================*/
DIAGPKT_REQ_DEFINE(DIAG_NV_WRITE_F)
  
 uint16 item;                         /* Which item - use nv_items_enum_type */
 uint8  item_data[DIAG_NV_ITEM_SIZE]; /* Item itself - use nv_item_type      */
 uint16 nv_stat;                      /* Status of operation
                                                     - use nv_stat_enum_type */
DIAGPKT_REQ_END

DIAGPKT_DEFINE_RSP_AS_REQ(DIAG_NV_WRITE_F)


/*==========================================================================

MACRO    Multiple NV Write Operation Status

PURPOSE  Status definition for multiple NV write operation

============================================================================*/

#define DIAG_SUBSYS_NV_WRITE2_F            0    /* NV subsys command for
                                                   multiple NV-item write */
#define	DIAG_SUBSYS_NV_READ_EXT_F          1    /* NV ext subsys command for
                                                   NV-item read */
#define DIAG_SUBSYS_NV_WRITE_EXT_F         2    /* NV ext subsys command for
                                                   NV-item write */



#define DIAG_NV_WRITE_TOTAL_SUCCESS        0    /* NV write successful       */
#define DIAG_NV_WRITE_TOTAL_ITEM_NUM_INV   1    /* NV write - invalid item no*/
#define DIAG_NV_WRITE_TOTAL_LEN_BAD        2    /* NV write - bad total len  */
#define DIAG_NV_WRITE_ITEM_LEN_MISMATCH    3    /* NV write - mismatched len */
#define DIAG_NV_WRITE_ITEM_FAILED          4    /* NV write failed           */

#define DIAG_NV_WRITE_ITEM_LEN_BAD         11   /* NV item - bad item length */
#define DIAG_NV_WRITE_PKT_EXHAUSTED        12   /* NV item - run out of mem  */
#define DIAG_NV_WRITE_PARAM_BAD            13   /* NV item - bad parameter   */
#define DIAG_NV_WRITE_SPC_MODE_BAD         14   /* NV item - bad SPC mode    */
#define DIAG_NV_WRITE_SEC_MODE_BAD         15   /* NV item - bad SEC mode    */


/*==========================================================================

PACKET   DIAG_NV_WRITE1_F

PURPOSE  Sent from the DM to the DMSS to request a NV write of an item with 
         variable data length. The request is converted by padding to fixed
         128 bytes in DMSS side for each item and make each write equivelent
         to DIAG_NV_WRITE_F.

============================================================================*/
typedef PACK(struct)
{
  uint8  item_len;                    /* length of an item_data              */
  uint16 item;                        /* Which item - use nv_items_enum_type */
  uint8  nv_stat;                     /* Status of operation
                                                     - use nv_stat_enum_type */
  uint8  item_data[1];                /* Item itself - use nv_item_type
                                         This is a starting point of the item
                                         data - its storage size should be big
                                         enough to hold the data depending on
                                         the value of item_len:
                                           sizeof(DIAG_NV_WRITE1_F_type)
                                           + item_len - 1
                                         where 1 + item_len - 1 = item_len is
                                         for item_data storage               */
} DIAG_NV_WRITE1_F_req_type;

/*==========================================================================

PACKET   DIAG_NV_WRITE2_F

PURPOSE  Sent from the DM to the DMSS to request NV write of multiple item
         with variable data length stacking in a single message from DM.
         The DMSS extract and process them one item by one item and make
         make each write equivelent to DIAG_NV_WRITE_F.

NOTE     Make sure the size of DIAG_NV_WRITE2_F is identical to DIAG_NV_WRITE_F

============================================================================*/
typedef PACK(struct)
{
  uint16 item_total;                  /* total items in multiple-item msg    */
  uint16 item_total_len;              /* length of all items in next array   */
  uint16 nv_stat_total;               /* status for entire NV write          */
  DIAG_NV_WRITE1_F_req_type item_data[1];  
                                      /*  Multi-items stacked one by one
                                         with total item size up to 
                                         DIAG_NV_ITEM_SIZE, and the actual
                                         storage size including item_data is
                                           sizeof(DIAG_NV_WRITE2_F_type)
                                           + item_total_len - 1 
                                         where
                                         item_total_len <= DIAG_NV_ITEM_SIZE */
} DIAG_NV_WRITE2_F_req_type;

typedef DIAG_NV_WRITE2_F_req_type DIAG_NV_WRITE2_F_rsp_type;

typedef struct
{
  diagpkt_subsys_header_type header;        /* Sub System header */
  uint16 item;								/* NV item */
  uint16 context; 							/* Context */
  uint8 item_data[DIAG_NV_ITEM_SIZE];		/* NV item data to pass parameters*/
  uint16 nv_stat;                       /* Return Status */  
}DIAG_NV_WRITE_EXT_F_req_type;

typedef DIAG_NV_WRITE_EXT_F_req_type DIAG_NV_WRITE_EXT_F_rsp_type;

typedef struct
{
  diagpkt_subsys_header_type header;        /* Sub System header */
  uint16 item;								/* NV item */
  uint16 context; 							/* Context */
  uint8 item_data[DIAG_NV_ITEM_SIZE];		/* NV item data to pass parameters*/
  uint16 nv_stat;                       /* Return Status */
}DIAG_NV_READ_EXT_F_req_type;

typedef DIAG_NV_READ_EXT_F_req_type DIAG_NV_READ_EXT_F_rsp_type;


typedef struct
{
  diagpkt_subsys_header_type header;        /* Sub System header */
  uint16 command;                           /* Which command */
  uint16 item;								/* NV item */
  uint16 context; 							/* Context */
  uint8 item_data[DIAG_NV_ITEM_SIZE];		/* NV item data to pass parameters*/
  uint16 return_stat;                       /* Return Status */  
}NV_SERVICES_TEST_F_req_type;

typedef NV_SERVICES_TEST_F_req_type NV_SERVICES_TEST_F_rsp_type;


/*===========================================================================

PACKET   DIAG_SMS_READ_F

PURPOSE  This message requests a read of the given SMS message out of NV

RESPONSE The diag_sms_read_rsp_type will be sent in response.

===========================================================================*/
typedef PACK(struct)
{
  byte cmd_code;                               /* Command code */
  byte address;                                /* Which message to get */
} nvdiag_sms_read_req_type;

typedef PACK(struct)
{
  byte cmd_code;                           /* Command code */
#ifdef FEATURE_NV_ENUMS_ARE_WORDS
  #error code not present
#else
  uint16 nv_stat;                          /* nv_stat should always be 
                                              uint16 (as per ICD)*/
#endif
  nv_sms_type sms_message;   /* SMS msg read from the phone - variable size */
} nvdiag_sms_read_rsp_type;


/*===========================================================================

PACKET   DIAG_SMS_WRITE_F

PURPOSE  This message requests a write of the given SMS message into NV.
         It is a variable length request message - only as long as dictated
         by the length field in sms_message.

RESPONSE The diag_sms_write_rsp_type will be sent in response.

===========================================================================*/
typedef PACK(struct)
{
  byte cmd_code;                               /* Command code */
  nv_sms_type sms_message;                     /* SMS message to write */
} nvdiag_sms_write_req_type;

typedef PACK(struct)
{
  byte cmd_code;                             /* Command code */
#ifdef FEATURE_NV_ENUMS_ARE_WORDS
  #error code not present
#else
  uint16 nv_stat;                            /* nv_stat should always be 
                                                uint16 (as per ICD)*/
#endif
} nvdiag_sms_write_rsp_type;


/*===========================================================================

FUNCTION UNREADABLE_NV_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's one of
  the few items which are never allowed to be read.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is unreadable.
  FALSE if it's not.

SIDE EFFECTS
  None.

============================================================================*/
boolean unreadable_nv_item (
  nv_items_enum_type item
);


/*===========================================================================

FUNCTION UNWRITABLE_NV_ITEM

DESCRIPTION
  This procedure checks the given nv item type to see if it's one of
  the few items which are never allowed to be written. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is unreadable.
  FALSE if it's not.

SIDE EFFECTS
  None.

============================================================================*/
boolean unwritable_nv_item (
  nv_items_enum_type item
);

#if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
#endif

#endif /* NVDIAG_H */
