/*****************************************************************************
***
*** TITLE
***
***  gsn_comp.h
***
***
*** DESCRIPTION
***
***  Header file for SNDCP compression
***
*** 
*** Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsn_comp.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz	   changes
***
*****************************************************************************/

#ifndef INC_GSN_COMP_H
#define INC_GSN_COMP_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#if((defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)))

#include "gsni.h"
#include "dsm.h"

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

/* ROHC related parameters */

/* 3GPP spec 44.065 section 6.5.4.1 */
#define SNDCP_ROHC_MAX_CID_DEFAULT        15
#define SNDCP_ROHC_MAX_HEADER_DEFAULT     168

/* Recommendations in ROHC HLD document number: 80-ROHC-1 */
#define SNDCP_ROHC_MRRU                   0
#define SNDCP_ROHC_MAX_JITTER_CD          0
#define SNDCP_ROHC_NUM_ALLOWED_PKT_SIZES  0
#define SNDCP_ROHC_ALLOWED_PKT_SIZES      0
#define SNDCP_ROHC_COMP_FLOW_WIN_WIDTH    6
#define SNDCP_ROHC_NUM_UPDATES_IR         4
#define SNDCP_ROHC_NUM_NONUPDATES         0
#define SNDCP_ROHC_SO_IRt                 512
#define SNDCP_ROHC_SO_FOt                 255
#define SNDCP_ROHC_FO_IRt                 255
#define SNDCP_ROHC_TIMER_BASED_COMP       FALSE
#define SNDCP_ROHC_F_MAX_TIME             5000 /* milli seconds */
#define SNDCP_ROHC_EXP_BACKOFF            0
#define SNDCP_ROHC_TIME_STRIDE            0
#define SNDCP_ROHC_TS_STRIDE_RECALC       TRUE
#define SNDCP_ROHC_LOSS_OPTION            FALSE
#define SNDCP_ROHC_SKIP_CID               FALSE
#define SNDCP_ROHC_FEEDBACK_FOR           NULL
#define SNDCP_ROHC_RD_DEPTH               0
#define SNDCP_ROHC_TIMER_BASED_DECOMP     FALSE
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


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

/*===========================================================================
===
===  FUNCTION      GSN_1144_COMP_INIT( ) 
===
===  DESCRIPTION
===
===    Initialises an RFC 1144 compression entity 
===
===  DEPENDENCIES
===
===    None
===  
===  RETURN VALUE
===
===    TRUE if available memory
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
boolean gsn_1144_comp_init
( 
   gas_id_t   gas_id,
   pcomp_e_t *pcomp_e,
   uint8 s0 /* state slots */
);

/*===========================================================================
===
===  FUNCTION      GSN_SLHC_FREE_MEM( ) 
===
===  DESCRIPTION
===
===    Frees compression entity's dynamically allocated memory  
===
===  DEPENDENCIES
===
===    None
===  
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
void gsn_slhc_free_mem( gas_id_t gas_id, slhc_handle_type *comp_e );


/*===========================================================================
===
===  FUNCTION      GSN_IPHC_ALLOC(()
===
===  DESCRIPTION
===
===   Allocates memory for an instance of RFC2507 compression algorithm.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   Pointer to iphc instance.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

#ifdef FEATURE_GSM_SNDCP_IPHC

void *gsn_iphc_alloc(iphc_config_options_s_type *ipv4_comp_opt,
                   iphc_config_options_s_type *ipv4_decomp_opt,
                   iphc_config_options_s_type *ipv6_comp_opt,
                   iphc_config_options_s_type *ipv6_decomp_opt);
#endif /* FEATURE_GSM_SNDCP_IPHC */

/*===========================================================================
===
===  FUNCTION      GSN_IPHC_FREE(()
===
===  DESCRIPTION
===
===   Frees memory allocated for an instance of RFC2507 compression algorithm.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gsn_iphc_free( gas_id_t gas_id, void *handle);


/*===========================================================================
===
===  FUNCTION      GSN_IPHC_COMP_INIT( ) 
===
===  DESCRIPTION
===
===    Allocates memory for an instance of RFC 2507 compression algorithm and 
===    initialises it. 
===
===  DEPENDENCIES
===
===    None
===  
===  RETURN VALUE
===
===    TRUE if available memory
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/

boolean gsn_iphc_comp_init( gas_id_t gas_id, pcomp_e_t *pcomp_ent_ptr,
                            gsn_xid_pcomp_2507_t *rfc2507_neg_params);



/*===========================================================================
===
===  FUNCTION      GSN_IPHC_IOCTL(()
===
===  DESCRIPTION
===
===   Reinitialises an instance of RFC2507 compression algorithm with new 
===   parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   0 if success, -1 if failure.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

#ifdef FEATURE_GSM_SNDCP_IPHC

int32 gsn_iphc_ioctl(gas_id_t gas_id,
                     void *handle, 
                     iphc_ioctl_e_type ioctl_type,
                     iphc_ioctl_u_type *ioctl_data);

#endif /* FEATURE_GSM_SNDCP_IPHC */

/*===========================================================================
===
===  FUNCTION      GSN_ROHC_ALLOC(()
===
===  DESCRIPTION
===
===   Allocates memory for an instance of rohc compression algorithm.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   NULL if memory allocation fails. errno gives error code. otherwise 
===   returns a valid pointer to the allocated rohc instance.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/


/*===========================================================================
===
===  FUNCTION      GSN_ROHC_FREE(()
===
===  DESCRIPTION
===
===   Frees memory allocated for an instance of rohc compression algorithm.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gsn_rohc_free(gas_id_t gas_id, void *handle);


/*===========================================================================
===
===  FUNCTION      GSN_ROHC_COMP_INIT( ) 
===
===  DESCRIPTION
===
===    Allocates memory for an instance of RFC 3095 compression algorithm and 
===    initialises it. 
===
===  DEPENDENCIES
===
===    None
===  
===  RETURN VALUE
===
===    TRUE if memory is available. FALSE otherwise.
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/

boolean gsn_rohc_comp_init( gas_id_t gas_id, pcomp_e_t *pcomp_ent_ptr,
                            gsn_xid_pcomp_3095_t *rfc3095_neg_params);


/*===========================================================================
===
===  FUNCTION      GSN_ROHC_IOCTL(()
===
===  DESCRIPTION
===
===   Reinitialises an instance of rohc compression algorithm with new 
===   parameters.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   0 if success, -1 if failure. errno gives error code.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/


/*===========================================================================
===
===  FUNCTION      GSN_PCI_COMPRESS( ) 
===
===  DESCRIPTION
===
===    Compresses the Protocol Control Information ( TCP/UDP/IP header 
===    compression) of the PDP N-PDU using RFC 11444 or RFC 2507 algorithms.
===    Zero is returned, if no compression algorithm has been assigned on the NSAPI.
===
===  DEPENDENCIES
===    
===    None 
===  
===  RETURN VALUE
===
===    PCOMP value indicating compressed packet type
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/

uint8 gsn_pci_compress
(
  gas_id_t               gas_id,
  const gsn_nsapi_e_t   *nsapi_e_ptr,    /* NSAPI which received the N-PDU */
  dsm_item_type         **npdu           /* PDP N-PDU to be compressed */
);

/*===========================================================================
===
===  FUNCTION      GSN_PCI_DECOMPRESS( ) 
===
===  DESCRIPTION
===
===    Performs PCI decompression on the DL reassembled N-PDU 
===    using RFC 1144 or RFC 2507
===
===  DEPENDENCIES
===    
===    None 
===  
===  RETURN VALUE
===
===   TRUE if N-PDU is decompressed successfully.
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
boolean gsn_pci_decompress
(
  gas_id_t               gas_id,
  const gsn_nsapi_e_t   *nsapi_e_ptr,    /* NSAPI which received the N-PDU */
  dsm_item_type         **npdu           /* PDP N-PDU to be decompressed */
);

/*===========================================================================
===
===  FUNCTION      GSN_UNACK_NPDU_DROPPED( ) 
===
===  DESCRIPTION
===
===    Informs PCI decompression entity that a DL N-PDU is dropped.
===    (see TS 04.65 par 6.5.2.3 ). Note, this function is used 
===    only in UNACK LLC operation since in ACK LLC operation in-order 
===    delivery of SN-PDUs is guaranteed.
===
===  DEPENDENCIES
===    
===    None 
===  
===  RETURN VALUE
===    
===    None
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
void gsn_unack_npdu_dropped
(
  gas_id_t               gas_id,
  const gsn_nsapi_e_t   *nsapi_e_ptr  /* NSAPI dropped the N-PDU */
);


/*===========================================================================
===
===  FUNCTION      GSN_ACK_REMOVE_FBACK_NPDUS_AND_RENUMBER( ) 
===
===  DESCRIPTION
===
===    On re-establishment of LLC ACK mode link, any feedback NPDUs held in
===    re-transmission queue are discarded before UL data transfer starts. In
===    case of a re-establishment following an inter-SGSN RAU, this activity 
===    is postponsed until SNSM_SEQ_IND is received.
===    3GPP CR - CP-060267 (C1-060958): Handling of feedback N-PDU
===
===  DEPENDENCIES
===    
===    None 
===  
===  RETURN VALUE
===    
===    None
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
void gsn_ack_remove_fback_npdus_and_renumber( gas_id_t gas_id, gsn_nsapi_e_t *nsapi_e_ptr);


#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

/*===========================================================================
===
===  FUNCTION      GSN_V42BIS_COMP_INIT( ) 
===
===  DESCRIPTION
===
===    Initialises an V42 bis compression entity 
===
===  DEPENDENCIES
===
===    None
===  
===  RETURN VALUE
===
===    None
===   
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
boolean gsn_v42bis_comp_init
( 
  gas_id_t,
  dcomp_e_t * /* entity to be initialised */    
);

/*===========================================================================
===
===  FUNCTION      GSN_DATA_COMPRESS( ) 
===
===  DESCRIPTION
===
===    Performs data compression on the PDP N-PDU using V42bis algorithm.
===    Zero is returned, if no compression algorithm has been 
===    assigned on the NSAPI.
===
===  DEPENDENCIES
===    
===    None 
===  
===  RETURN VALUE
===
===    DCOMP value indicating compressed packet type
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
uint8 gsn_data_compress
(
  gas_id_t               gas_id,
  const gsn_nsapi_e_t   *nsapi_e_ptr,    /* NSAPI which received the N-PDU */
  dsm_item_type        **npdu            /* PDP N-PDU to be compressed */
);

/*===========================================================================
===
===  FUNCTION      GSN_DATA_DECOMPRESS( ) 
===
===  DESCRIPTION
===
===    Performs data compression on the PDP N-PDU using V42bis algorithm.
===    Zero is returned, if no compression algorithm has been 
===    assigned on the NSAPI.
===
===  DEPENDENCIES
===    
===    None 
===  
===  RETURN VALUE
===
===    DCOMP value indicating compressed packet type
===
===  SIDE EFFECTS
===
===    None
===  
===========================================================================*/
void gsn_data_decompress
(
  gas_id_t               gas_id,
  const gsn_nsapi_e_t   *nsapi_e_ptr,    /* NSAPI which received the N-PDU */
  dsm_item_type         **npdu           /* PDP N-PDU to be compressed */
);

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP || FEATURE_GSM_GPRS_SNDCP_PCOMP */


#endif /* INC_GSN_COMP_H */

/*** EOF: don't remove! ***/
