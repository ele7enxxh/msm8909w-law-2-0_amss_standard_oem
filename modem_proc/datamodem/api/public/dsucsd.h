#ifndef DSUCSD_H
#define DSUCSD_H

/**
  @file dsucsd.h 
  @brief
  This file contains definitions used by the GSM/WCDMA** circuit-switched
  data modules.
*/
/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the Modem_Data_mainpage file. 
		 To change any of the the file/group text for the PDF, edit the 
		 Modem_Data_mainpage file, or contact Tech Pubs.

         The above description for this file is part of the 
		 "dsucsd" group description in the Modem_Data_mainpage 
		 file. 
*/
/*===========================================================================

            U M T S  C I R C U I T   S W I T C H E D   D A T A      
                            H E A D E R   F I L E

  
  Copyright (c) 2003 - 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dsucsd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/27/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
07/12/10   km      Added Doxygen tags
09/01/08   sa      Added modifications for CMI defeaturization.
08/15/08   sa      Removed complex expression from enum ds_ucsd_call_type.
03/25/08   sa      Added dsucsd_call_descriptor_type.
03/19/08   sa      Added ds_ucsd_nt_rlp_parameters_T. 
07/03/07   ar      Changes for unified UCSD API.
09/01/06   ar      Add support for multiprocessor build.
02/09/06   ar      Lint corrections.
12/16/05   ar      Added SYS_MAX_UCSD_CALLS 
11/21/05   ar      Added DS_UMTS_CSD_FRAME_SIZE_INVALID.
09/15/05   ar      Added ds_ucsd_ota_channel_params_T
11/12/04   dwp     Added User to User info types.
07/16/04   ar      Added supplementary services dial modifer types.
06/20/03   dgy     Added support for UMTS CS Data API.
02/25/03   tmr     Created

===========================================================================*/

/*===========================================================================
Header files
===========================================================================*/
#include "cm.h"
#include "sys.h"

/** @addtogroup dsucsd
@{ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/** Invalid instance ID. */
#define DSUCSD_INVALID_INST_ID  CM_CALL_ID_INVALID  
/** No call instance ID. */
#define NO_CALL_ID          DSUCSD_INVALID_INST_ID 
/** External client ID. */
#define DSUCSD_EXTERNAL_CLIENT_ID  (-2) 

/** Maximum number of UCSD call instances.
  Multiple CS RABs must be fully supported to increase this value. */
#define SYS_MAX_UCSD_CALLS   (2)  

/*---------------------------------------------------------------------------
  Supported call types
---------------------------------------------------------------------------*/

/** Call index type. */
typedef enum ds_ucsd_call_index
{
   DS_UCSD_CALL_INDEX_ASYNC,             /**< Asynchronous CS data call. */
   DS_UCSD_CALL_INDEX_SYNC,              /**< Synchronous CS data call. */
   DS_UCSD_CALL_INDEX_FAX,               /**< Facsimile call. */
   DS_UCSD_CALL_INDEX_VT,                /**< Internal videotelephony call. */
   DS_UCSD_CALL_INDEX_VOICE,             /**< Voice call (for alternating data
                                              mode features). */
   DS_UCSD_CALL_INDEX_MAX                /**< Internal use only. */
} ds_ucsd_call_index_type; 

/** Macro to set the call type based on	ds_ucsd_call_index_type.  */
#define MAKE_CALL_TYPE( type_index ) (1 << type_index)


/** 
  The values in this enumeration are numeric because they are used in the 
  externalized API, and complex enumerations are not supported in the 
  external data representation (XDR) format. */
typedef enum ds_ucsd_call
{
   DS_UCSD_UNKNOWN_CALL         = (0x00),   /**< Unknown call type. */
   DS_UCSD_ASYNC_CSD_CALL       = (0x01),   /**< MAKE_CALL_TYPE
                                                (DS_UCSD_CALL_INDEX_ASYNC). */
   DS_UCSD_SYNC_CSD_CALL        = (0x02),   /**< MAKE_CALL_TYPE
                                                (DS_UCSD_CALL_INDEX_SYNC). */
   DS_UCSD_FAX_CALL             = (0x04),   /**< MAKE_CALL_TYPE
                                                (DS_UCSD_CALL_INDEX_FAX). */
   DS_UCSD_VIDEO_TELEPHONY_CALL = (0x08),   /**< MAKE_CALL_TYPE
                                                (DS_UCSD_CALL_INDEX_VT). */
   DS_UCSD_VOICE_CALL           = (0x10)    /**< MAKE_CALL_TYPE
                                                (DS_UCSD_CALL_INDEX_VOICE). */
} ds_ucsd_call_type; 

/** 
  Frame (SDU) sizes for the transparent and nontransparent
  asynchronous CS data protocol stacks. */
typedef enum
{
   DS_UMTS_CSD_FRAME_SIZE_INVALID = 0,  /**< Internal control value. */
  
   DS_UMTS_CSD_FRAME_SIZE_240_BIT = 30, /**< Number of bytes in a 240-bit 
                                             frame. */
                                               
   DS_UMTS_CSD_FRAME_SIZE_288_BIT = 36, /**< Number of bytes in half  
                                             a 576-bit frame. */
                                               
   DS_UMTS_CSD_FRAME_SIZE_576_BIT = 72, /**< Number of bytes in a 576-bit
                                             frame. */
                                               
   DS_UMTS_CSD_FRAME_SIZE_640_BIT = 80  /**< Number of bytes in a 640-bit 
                                             frame. */
} ds_ucsd_frame_size_T; 

/** Macro for the maximum expected frame size. */
#define DS_UMTS_CSD_FRAME_SIZE_MAX  DS_UMTS_CSD_FRAME_SIZE_640_BIT


/** 
  Data rate types. Note that these values are used 
  in packet logging and <i>cannot</i> be modified. Also note
  that these rates <i>cannot</i> go beyond 4 bits. */
typedef enum
{
   DS_UCSD_RATE_NONE     = 0x00,  /**< Data rate is none. */
   DS_UCSD_RATE_300      = 0x01,  /**< Data rate is 300. */
   DS_UCSD_RATE_600      = 0x02,  /**< Data rate is 600. */
   DS_UCSD_RATE_1200     = 0x03,  /**< Data rate is 1200. */
   DS_UCSD_RATE_1200_300 = 0x04,  /**< Data rate is 1200_300. */
   DS_UCSD_RATE_2400     = 0x05,  /**< Data rate is 2400. */
   DS_UCSD_RATE_4800     = 0x06,  /**< Data rate is 4800. */
   DS_UCSD_RATE_7200     = 0x07,  /**< Data rate is 7200. */
   DS_UCSD_RATE_9600     = 0x08,  /**< Data rate is 9600. */
   DS_UCSD_RATE_12000    = 0x09,  /**< Data rate is 12000. */
   DS_UCSD_RATE_14400    = 0x0A,  /**< Data rate is 14400. */
   DS_UCSD_RATE_28800    = 0x0B,  /**< Data rate is 28800. */
   DS_UCSD_RATE_38400    = 0x0C,  /**< Data rate is 38400. */
// DS_UCSD_RATE_43200    = 0x0C,
   DS_UCSD_RATE_48000    = 0x0D,  /**< Data rate is 48000. */
   DS_UCSD_RATE_57600    = 0x0E,  /**< Data rate is 57600. */
   DS_UCSD_RATE_64000    = 0x0F   /**< Data rate is 64000. */
} ds_ucsd_data_rate_T;

/** Dial string modifier definitions. */
typedef enum
{
  DS_UCSD_CLI_DIAL_MODIFIER    = 0x0002,  /**< Dial modifier is CLI.  */
  DS_UCSD_CUG_DIAL_MODIFIER    = 0x0004,  /**< Dial modifier is CUG.  */
  DS_UCSD_CLI_AND_CUG_MODIFIER = 0x0008   /**< Dial modifier is CLI and CUG.  */
} ds_ucsd_dial_modifier_e_type;

/** @brief Calling Line Identification (CLI) dial modifier. 
*/
typedef struct
{
  boolean                       cli_enabled;  /**< Whether or not CLI is 
                                                   enabled. */
} ds_ucsd_cli_modifier_type;

/** @brief Closed User Group (CUG) dial modifier. */
typedef struct
{
  boolean                       cug_enabled;        /**< Whether or not CUG is 
                                                         enabled. */
  boolean                       cug_index_enabled;  /**< Whether or not CUG
                                                         index is enabled. */
  uint8                         cug_index_val;      /**< CUG index value. */
  boolean                       suppress_pref_cug;  /**< Suppress preferred 
                                                         CUG. */
  boolean                       suppress_outgoing_access; /**< Suppress 
                                                          outgoing access. */
} ds_ucsd_cug_modifier_type;

/** @brief Structure that is sent to the mode-specific call handlers. 
*/
typedef struct 
{
  boolean modifier_present;              /**< Whether or not a modifier is 
                                              present. */ 
  ds_ucsd_dial_modifier_e_type modifier; /**< Describes the type of 
                                              modifier. */

  struct
  {
    ds_ucsd_cli_modifier_type  cli;       /**< CLI. */
    ds_ucsd_cug_modifier_type  cug;       /**< CUG. */
   } modifier_info; /**< Modifier information. */
} ds_ucsd_dial_modifier_info_type;

/** 
  User-to-User Signaling (UUS) support. This is used for videotelephony 
  two-stage dialing. The enumeration is for various H.323 destination types 
  that are supported. The values 0 to 3 are actual values used in 
  the User-to-User Information Element (UUIE). */ 
typedef enum
{
  DS_UMTS_CSD_UUS_ID_NONE        = -1, /**< No UUIE peer address. */
  DS_UMTS_CSD_UUS_ID_RESERVED    = 0,  /**< Reserved.  */
  DS_UMTS_CSD_UUS_ID_EMAIL       = 1,  /**< RFC-822-compliant email address. */
  DS_UMTS_CSD_UUS_ID_URL         = 2,  /**< URL-style address. */
  DS_UMTS_CSD_UUS_ID_H323        = 3,  /**< Basic ISO/IEC 646 address. */
  DS_UMTS_CSD_UUS_ID_FIRST_VALID = DS_UMTS_CSD_UUS_ID_EMAIL, 
                                       /**< First valid address. */
  DS_UMTS_CSD_UUS_ID_ENUM_MAX    = 4   /**< Internal use only. */
} ds_ucsd_uus_id_e_type;
  
/** @brief Used for passing User-to-User Signaling 1 (UUS1)
 data.  
*/ 
typedef struct
{
  boolean                 present;  /**< Whether or not UUS data is present. */
  ds_ucsd_uus_id_e_type   uus_id;   /**< UUS ID. */
  uint8                  *uus_data; /**< UUS data. */
  uint8                   data_len; /**< Data length. */
} ds_ucsd_uus_info_type;
/*~ FIELD ds_ucsd_uus_info_type.uus_data VARRAY LENGTH ds_ucsd_uus_info_type.data_len */

/** @brief Used for passing Over-the-Air (OTA) channel parameters. 
*/
typedef struct
{
  uint16 sdus_per_tti;            /**< SDUs per Transmission Time 
                                       Interval (TTI). */
  uint16 sdu_size;                /**< SDU size. */
  ds_ucsd_data_rate_T data_rate;  /**< Data rate. */
} ds_ucsd_ota_channel_params_T;

/** Compression request. */
typedef enum
{
   V42_NO_COMPRESSION,   /**< Compression is none. */
   V42_ENCODE_ONLY,      /**< Compression is encode only. */
   V42_DECODE_ONLY,      /**< Compression is decode only. */
   V42_BOTH_DIRECTIONS   /**< Compression is both directions. */
} v42_comp_direction_T;

/** @brief Contains the Nontransparent (NT) RLP parameters. */
typedef struct
{
  /* Default CRLP parameters used for RLP V0 and V1 values from ATCOP */
  uint16               rlp_iws; /**< RLP Interworking Function (IWF) to  
                                     Mobile Station (MS) window size. */
  uint16               rlp_mws; /**< RLP MS-to-IWF window size. */
  byte                 rlp_t1;  /**< Acknowledgement timer T1. */
  byte                 rlp_n2;  /**< Retransmission attempts N2. */

  /* These are the RLP V1 parameters from ATCOP, and will only be used for
     sending out XID values  */
  uint16               rlp_iws_V1; /**< RLP V1 IWS for sending out XID 
                                        values. */
  uint16               rlp_mws_V1; /**< RLP V1 MWS for sending out XID 
                                        values. */
  byte                 rlp_t1_V1;  /**< RLP V1 T1 for sending out XID 
                                        values. */
  byte                 rlp_n2_V1;  /**< RLP V1 N2 for sending out XID 
                                        values. */

  /* These are the RLP V2 parameters from ATCOP, and will only be used for
     sending out XID values  */
  uint16               rlp_iws_V2; /**< RLP V2 IWS for sending out XID 
                                        values. */
  uint16               rlp_mws_V2; /**< RLP V2 MWS for sending out XID 
                                        values. */
  byte                 rlp_t1_V2;  /**< RLP V2 T1 for sending out XID 
                                        values. */
  byte                 rlp_n2_V2;  /**< RLP V2 N2 for sending out XID 
                                        values. */

  /* Value controls the V.42bis data compression */
  v42_comp_direction_T compression;  /**< V.42bis data compression 
                                          direction. */
  byte                 rlp_Pt;       /**< RLP PT. */
  byte                 rlp_comp_neg; /**< RLP compression negotiation. */
  byte                 rlp_P0;       /**< RLP P0. */
  word                 rlp_P1;       /**< RLP P1. */
  byte                 rlp_P2;       /**< RLP P2. */
  byte                 rlp_t4;       /**< RLP timer 4; this value is only used
                                          for RLP version 2 or later. */

  byte                 rlp_ver_gsm;   /**< RLP GSM default version. */
  byte                 rlp_ver_wcdma; /**< RLP WCDMA default version. */

  boolean              rlp_uses_srej; /**< Whether or not RLP selective reject 
                                           recovery is enabled. */

  uint32               rlp_adm_idle_timer; /**< Timer for the ADM state before 
                                                the call teardown after an RLP
                                                error. */

  uint8                rlp_connect_state;  /**< Connection state at which to 
                                                send a CONNECT response. */
} ds_ucsd_nt_rlp_parameters_T;

/** Call direction. */
typedef enum ds_ucsd_call_dir
{
  DS_UCSD_CALL_DIR_UNKNOWN,       /**< Direction is unknown. */  
  DS_UCSD_CALL_DIR_ORIGINATED,    /**< Direction is call-originated. */
  DS_UCSD_CALL_DIR_TERMINATED     /**< Direction is call-terminated. */
} ds_ucsd_call_dir_e_type;

/** @brief Describes the current call configuration. */
typedef struct dsucsd_call_descriptor_s
{
  cm_call_id_type              cm_call_id;     /**< CM call ID. */
  ds_ucsd_call_type            call_type;      /**< Type of call. */
  sys_sys_mode_e_type          network_mode;   /**< WCDMA or GSM. */ 
  ds_ucsd_call_dir_e_type      call_dir;       /**< Call direction. */
  boolean                      user_initiated_termination; 
                                   /**< User-initiated termination. */
  cm_bearer_capability_s_type  bearer_cap;     /**< Current bearer. */
  ds_ucsd_nt_rlp_parameters_T  rlp_parameters; /**< Asynchronous NT call RLP
                                                    information. */
} dsucsd_call_descriptor_type;

/** @} */ /* end_addtogroup_dsucsd */

#endif /* DSUCSD_H */
