#ifndef DACCIF_H
#define DACCIF_H
/*===========================================================================
                     D A C C I F . H

DESCRIPTION
  Header file containing the API for the Data Acceleration (DACC) Feature.
 

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE


when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/02/13    ss     Add DACC Instance Handle instead of RLC ID as identifier.
11/06/13    pc     Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


#include "dsm.h"
#include "sys.h"


/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  DACC instance handle type.
---------------------------------------------------------------------------*/
typedef void* dacc_handle_type;

/*---------------------------------------------------------------------------
  DACC Compression Engine Configuration Parameters
---------------------------------------------------------------------------*/
typedef enum
{
  DACC_COMPRESS_HEADER_AND_PAYLOAD,
  DACC_COMPRESS_HEADER_ONLY
} dacc_compress_e_type;

typedef struct
{
  uint8                      rlc_id;          /* Always valid value */
  sys_modem_as_id_e_type     sub_id;          /* Subscription ID for this IPDC instance */
  boolean                    config_deconfig; /* TRUE – Config and below params are correct, FALSE – Deconfig */
  uint16                     memory_size;     /* in Kbytes */
  uint8                      hdr_length;      /* Packet Header Length in bytes – Range 20 to 100 */
  dacc_compress_e_type       comp_type;       /* 0 – Hdr + payload, 1 – Hdr only */
  uint8                      comp_gain_thld;  /* 0 to 99 range*/
  boolean                    prefill_cmem;

} dacc_comp_config_options_s_type;



/*---------------------------------------------------------------------------
  DACC Compression Engine Compress Packet Parameters
---------------------------------------------------------------------------*/
typedef enum
{
  /* Src and Dest DSM items are allocated by RLC. 
     Compression Header followed by Compressed Data are written to Target DSM. 
     Source DSM untouched and continue to have uncompressed data */
  DACC_COMPR_COPY = 0x00,    

  /* Src and Dest DSM items are allocated by RLC. No Compression is performed – means simply Header to indicate “No Compression”. 
     Compression Header followed by original content from Original DSM are written to Target DSM. 
     Source DSM untouched */
  DACC_NOCOMPR_COPY, 

  /* Src (Actual RLC SDU) and Dest DSM items (Small DSM item) are allocated by RLC. 
     No Compression is performed – means simply Header to indicate “No Compression”
     No copy of the Src DSM to target DSM. RLC will allocate small DSM item for Compression Header purposes.
     DACC will update the UL Compression info into Small DSM. 
     Dups the Original DSM item and the DUP item is chained to the Small DSM which has header.
     Small DSM item with Header info, linked to DUP DSM item (to original packet) */
  DACC_NOCOMPR_NOCOPY, 

  /* RESERVED */
  DACC_RESERVED
} dacc_pkt_comp_mode_type;

typedef struct
{
  /* RLC would directly pass in DACC Handle instead of RLC ID */
  dacc_handle_type          dacc_handle;
  dsm_item_type           **src_dsm_item;
  dsm_item_type           **target_dsm_item;
  dacc_pkt_comp_mode_type   comp_mode; //see definition above
} dacc_req_pkt_comp_info_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DACC_POWERUP_INIT

DESCRIPTION
  Initializes the DACC Module at powerup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes IPDC state variables
===========================================================================*/
boolean dacc_powerup_init
(
  void
);


/*===========================================================================
FUNCTION DACC_CONFIG_COMPRESSOR

DESCRIPTION
  Configures a DACC Compressor Instance.  
 
DEPENDENCIES
  dacc_powerup_init() must have been called prior to calling
  dacc_config_compressor()

RETURN VALUE
  DACC Handle Type - valid handle if successful. NULL otherwise

SIDE EFFECTS
  Allocates memory for the DACC Compressor Instance if an instance has
  not already been created for the RLC ID.
===========================================================================*/
dacc_handle_type dacc_config_compressor
(
  const dacc_comp_config_options_s_type       * const comp_params
);

/*===========================================================================
FUNCTION DACC_REQ_UL_COMP_PACKET

DESCRIPTION
  This function invokes the requested DACC packet compression operation. 

DEPENDENCIES
  None

RETURN VALUE
   boolean - TRUE if successful. FALSE otherwise

SIDE EFFECTS
  If requested, compresses the source packet.  
===========================================================================*/
boolean dacc_req_ul_comp_packet
(
  dacc_req_pkt_comp_info_type   *pkt_compress_info_ptr
);

/*===========================================================================
FUNCTION DACC_RESET_COMPRESSOR

DESCRIPTION
  Flushes the DACC Compression Engine's Compressor Memory and re-initializes
  it to the prefill pattern.  

DEPENDENCIES
  NONE

RETURN VALUE
   boolean - TRUE if successful. FALSE otherwise

SIDE EFFECTS
  Flushes and prefills the DACC Compression Engine's Compressor Memory  
===========================================================================*/
boolean dacc_reset_compressor
(
  dacc_handle_type  dacc_handle
);

/*===========================================================================
FUNCTION DACC_FREE_COMPRESSOR

DESCRIPTION
  Frees a DACC Compressor Instance.  
 
DEPENDENCIES
  dacc_powerup_init() must have been called prior to calling
  dacc_config_compressor()

RETURN VALUE
   boolean - TRUE if successful. FALSE otherwise

SIDE EFFECTS
  Allocates memory for the DACC Compressor Instance if an instance has
  not already been created for the RLC ID.
===========================================================================*/
boolean dacc_free_compressor
(
  dacc_handle_type dacc_handle
);

#ifdef __cplusplus
}
#endif

#endif /* RLCDACCIF_H */
