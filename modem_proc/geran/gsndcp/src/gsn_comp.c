/*****************************************************************************
***
*** TITLE
***
***  gsn_comp.c
***
***
*** DESCRIPTION
***
***  Handles SNDCP Protocol Control Information (PCI) and data compression
***
***
*** EXTERNALIZED FUNCTIONS
***
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  None
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsn_comp.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#if((defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)))

#define FEATURE_DSM_WM_CB
#if(defined(FEATURE_GSM_SNDCP_IPHC) )
#include "stdio.h"
#endif /* FEATURE_GSM_SNDCP_IPHC || FEATURE_GSM_SNDCP_ROHC */
#ifdef FEATURE_GSM_SNDCP_IPHC
#include "ps_iphc.h"
#endif /* FEATURE_GSM_SNDCP_IPHC */
#include "gsn_comp.h"
#include "gsn_hdlr.h"
#include "gsndef.h"
#include "gprs_pdu.h"
#include "gprs_mem.h"
#include "gsn_util.h"
#include "memory.h"

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

uint8 gsn_1144_compress( gas_id_t , pcomp_e_t *, dsm_item_type ** );

uint8 gsn_iphc_compress(gas_id_t gas_id, pcomp_e_t *pci_e, dsm_item_type **npdu);
uint8 gsn_rohc_compress(gas_id_t gas_id, pcomp_e_t *pci_e, dsm_item_type **npdu);

#endif /* #ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP */

/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/




/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/


#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

/*===========================================================================
===
===  FUNCTION      GSN_1144_COMPRESS( )
===
===  DESCRIPTION
===
===    Compresses a TCP/IP packet using RFC 1144 compression algorithm
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    Compressed packet type
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
uint8 gsn_1144_compress
(
  gas_id_t         gas_id,
  pcomp_e_t       *pci_e,    /* PCI compresion entity */
  dsm_item_type   **npdu      /* IP packet to be compressed */
)
{
  /* compress_cid is negotiated during IPCP header compression options
  ** Is not used in SNDCP therfore it should be set to 1  */
  uint8                   compress_cid = 1;
  uint8                   pcomp_val    = 0;
  slhc_packet_type_e_type pkt_type;

  if ( slhc_compress( (slhc_handle_type *)pci_e->comp_e, 
                       npdu, 
                       compress_cid, 
                       &pkt_type ) == SLHC_SUCCESS )
  {
    switch(pkt_type )
    {
      case SLHC_PACKET_TYPE_IP: /* IP packet */
        pcomp_val = 0;
        break;

      case SLHC_PACKET_TYPE_COMPRESSED_TCP: /* compressed packet */
        pcomp_val = pci_e->alg.xid_rfc1144.pcomp2;
        break;

      case SLHC_PACKET_TYPE_UNCOMPRESSED_TCP: /* uncompressed packet */
        pcomp_val = pci_e->alg.xid_rfc1144.pcomp1;
        break;

      default:
        /* slhc_compress releases the dsm_item (npdu) in case of an error */
        MSG_GERAN_HIGH_3_G("GSN ""GSN 1144_comp: Bad IP Packet, pkt type %d", pkt_type,
                                                                  0, 
                                                                  0);
        pcomp_val = 0;
        break;
    }
  }

  return( pcomp_val );
}

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */


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
  pcomp_e_t *pcomp_e,       /* pointer to a pcomp entity */
  uint8 s0 /* state slots */
)
{
  boolean init_succ = FALSE;

  /* Allocate memory and initialise RFC1144 comp entity */
  pcomp_e->comp_e = (slhc_handle_type *) slhc_alloc( s0, s0 );

  if ( pcomp_e->comp_e != NULL ) 
  {
    init_succ = TRUE;
    MSG_GERAN_HIGH_3_G("GSN ""1144 instance created for pcomp entity %d",pcomp_e->ent_num,
                                                             0,
                                                             0);
  }
  else
  {
    MSG_GERAN_HIGH_1_G("GSN ""1144 instance not created for entity %d",pcomp_e->ent_num);
  }

  return( init_succ );

}

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
void gsn_slhc_free_mem( gas_id_t gas_id, slhc_handle_type *comp_e )
{

  if( comp_e != NULL )
  {
    slhc_free( (slhc_handle_type *)comp_e );
    MSG_GERAN_HIGH_0_G("GSN ""1144 instance freed");
  }
  else
  {
    MSG_GERAN_HIGH_0_G("GSN " "1144 entity do not exist, cannot be freed");
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_IPHC_COMPRESS( )
===
===  DESCRIPTION
===
===    Compresses TCP/IP or UDP/IP packet using RFC2507 compression algorithm
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    Compressed packet type
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
uint8 gsn_iphc_compress
(
  gas_id_t         gas_id,
  pcomp_e_t       *pci_e,     /* PCI compresion entity */
  dsm_item_type   **npdu      /* IP packet to be compressed */
)
{
#ifdef FEATURE_GSM_SNDCP_IPHC
  uint8 pcomp_val;
  int32 result;
  iphc_packet_type_e_type  pkt_type;

  result = iphc_compress(pci_e->comp_e, npdu, &pkt_type);

  if(result != 0)
  {
    MSG_GERAN_MED_1_G( "iphc_compress() failed %d",result);
  }

  switch(pkt_type)
  {
    case IPHC_PACKET_TYPE_NORMAL_IPV4:
    case IPHC_PACKET_TYPE_NORMAL_IPV6:
      pcomp_val = 0;
      break;

    case IPHC_PACKET_TYPE_FULL_HEADER:
      pcomp_val = pci_e->alg.xid_rfc2507.pcomp1;
      break;

    case IPHC_PACKET_TYPE_COMPRESSED_TCP:
      pcomp_val = pci_e->alg.xid_rfc2507.pcomp2;
      break;

    case IPHC_PACKET_TYPE_COMPRESSED_TCP_NODELTA:
      pcomp_val = pci_e->alg.xid_rfc2507.pcomp3;
      break;

    case IPHC_PACKET_TYPE_COMPRESSED_NON_TCP:
      pcomp_val = pci_e->alg.xid_rfc2507.pcomp4;
      break;

    case IPHC_PACKET_TYPE_CONTEXT_STATE:
      pcomp_val = pci_e->alg.xid_rfc2507.pcomp5;
      break;

    case IPHC_PACKET_TYPE_COMPRESSED_UDP_8:
      pcomp_val = pci_e->alg.xid_rfc2507.pcomp4;
      break;

    case IPHC_PACKET_TYPE_COMPRESSED_UDP_16:
      pcomp_val = pci_e->alg.xid_rfc2507.pcomp4;
      break;

    case IPHC_PACKET_TYPE_COMPRESSED_RTP_8:
      pcomp_val = pci_e->alg.xid_rfc2507.pcomp4;
      break;

    case IPHC_PACKET_TYPE_COMPRESSED_RTP_16:
      pcomp_val = pci_e->alg.xid_rfc2507.pcomp4;
      break;

    default:
      /* iphc_compress releases the dsm_item (npdu) in case of an error */
      MSG_GERAN_HIGH_1_G("GSN ""GSN 2507_comp: Bad Packet %d", pkt_type);
      pcomp_val = 0;
      break;
  }

  return( pcomp_val );
#else
  MSG_GERAN_HIGH_0_G("GSN ""IPHC comp disabled");
  return( 0 );
#endif /* FEATURE_GSM_SNDCP_IPHC */
}


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
                   iphc_config_options_s_type *ipv6_decomp_opt)
{
  return iphc_alloc(ipv4_comp_opt,
                    ipv4_decomp_opt,
                    ipv6_comp_opt,
                    ipv6_decomp_opt);
}

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

void gsn_iphc_free( gas_id_t gas_id, void *handle)
{
#ifdef FEATURE_GSM_SNDCP_IPHC
  iphc_free(handle);
  MSG_GERAN_HIGH_0_G("GSN ""IPHC instance freed");
#else
  MSG_GERAN_HIGH_0_G("GSN ""IPHC comp disbled ");
#endif /* FEATURE_GSM_SNDCP_IPHC */
}


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
                     iphc_ioctl_u_type *ioctl_data)
{
  MSG_GERAN_HIGH_1_G("GSN ""IPHC ioctl called: %d", ioctl_type);

  return iphc_ioctl(handle,
                    ioctl_type,
                    ioctl_data);
}

#endif /* FEATURE_GSM_SNDCP_IPHC */

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

boolean gsn_iphc_comp_init
(
  gas_id_t   gas_id,
  pcomp_e_t *pcomp_ent_ptr,
  gsn_xid_pcomp_2507_t *rfc2507_neg_params
)
{

#ifdef FEATURE_GSM_SNDCP_IPHC

  iphc_config_options_s_type ipv4_ipv6_comp_decomp_params;


  ipv4_ipv6_comp_decomp_params.tcp_space         = rfc2507_neg_params->tcp_sp;
  ipv4_ipv6_comp_decomp_params.non_tcp_space     = rfc2507_neg_params->ntcp_sp;
  ipv4_ipv6_comp_decomp_params.f_max_period      = rfc2507_neg_params->max_per;
  ipv4_ipv6_comp_decomp_params.f_max_time        = rfc2507_neg_params->max_time;
  ipv4_ipv6_comp_decomp_params.max_header        = rfc2507_neg_params->max_hdr;
  ipv4_ipv6_comp_decomp_params.rtp_enabled       = FALSE;
  ipv4_ipv6_comp_decomp_params.expect_reordering = FALSE;

  pcomp_ent_ptr->comp_e = gsn_iphc_alloc(&ipv4_ipv6_comp_decomp_params,
                                         &ipv4_ipv6_comp_decomp_params,
                                         &ipv4_ipv6_comp_decomp_params,
                                         &ipv4_ipv6_comp_decomp_params);

  if(pcomp_ent_ptr->comp_e == NULL)
  {
    MSG_GERAN_ERROR_0_G("GSN ""iphc memory alloc failed ");
    return FALSE;
  }

  MSG_GERAN_HIGH_3_G("GSN ""IPHC instance created for pcomp entity %d",
                                                  pcomp_ent_ptr->ent_num,
                                                  0,
                                                  0);
  return TRUE;

#else
  MSG_GERAN_HIGH_0_G("GSN ""IPHC comp disbled");
  return FALSE;
#endif /* FEATURE_GSM_SNDCP_IPHC */

}/* gsn_iphc_comp_init() */



/*===========================================================================
===
===  FUNCTION      GSN_ROHC_COMPRESS( )
===
===  DESCRIPTION
===
===    Compresses a RTP/UDP/IP, UDP/IP, or ESP/IP packet using RFC3095
===    compression algorithm
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
===    Compressed packet type
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
uint8 gsn_rohc_compress
(
  gas_id_t         gas_id,
  pcomp_e_t       *pci_e,    /* PCI compresion entity */
  dsm_item_type   **npdu      /* IP packet to be compressed */
)
{
  uint8 pcomp_val = 0;


  MSG_GERAN_HIGH_0_G("GSN ""ROHC comp disbled");


  return( pcomp_val );
}



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

void gsn_rohc_free(gas_id_t gas_id, void *handle)
{
  MSG_GERAN_HIGH_0_G("GSN ""ROHC comp disbled ");
}


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

boolean gsn_rohc_comp_init
(
  gas_id_t   gas_id,
  pcomp_e_t *pcomp_ent_ptr,
  gsn_xid_pcomp_3095_t *rfc3095_neg_params
)
{


  MSG_GERAN_HIGH_0_G("GSN ""ROHC comp disbled");
  return FALSE;


} /* gsn_rohc_comp_init() */


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
)
{

  uint8     pcomp_val = 0;
  uint8     comp_ent_num;
  pcomp_e_t *pcomp_e_ptr;

  /* Check if the NSAPI has an assigned PCI compression entity
  */
#ifdef FEATURE_GSM_SNDCP_IPHC
  if( nsapi_e_ptr->pcomp_e_num[T2_ALG_RFC2507] != NO_COMP_ENT )
  {
    /* Use RFC 2507 for PCI compression
    */
    comp_ent_num = nsapi_e_ptr->pcomp_e_num[T2_ALG_RFC2507];
    pcomp_e_ptr = nsapi_e_ptr->sapi_ptr->pcomp_e_pool[comp_ent_num];

    if(pcomp_e_ptr != NULL)
    {
      if((pcomp_e_ptr->state != ASSIGNED)       ||
         (!pcomp_e_ptr->alg.xid_rfc2507.pcomp1) ||
         (!pcomp_e_ptr->alg.xid_rfc2507.pcomp2) ||
         (!pcomp_e_ptr->alg.xid_rfc2507.pcomp3) ||
         (!pcomp_e_ptr->alg.xid_rfc2507.pcomp4) ||
         (!pcomp_e_ptr->alg.xid_rfc2507.pcomp5))
      {
        if((pcomp_e_ptr->alg.xid_rfc2507.pcomp1) &&
           (pcomp_e_ptr->alg.xid_rfc2507.pcomp2) &&
           (pcomp_e_ptr->alg.xid_rfc2507.pcomp3) &&
           (pcomp_e_ptr->alg.xid_rfc2507.pcomp4) &&
           (pcomp_e_ptr->alg.xid_rfc2507.pcomp5))
        {
          MSG_GERAN_ERROR_2_G("GSN ""LL %d: Invalid PCOMP entity %d",
            gsn_extern_sapi[nsapi_e_ptr->sapi_ptr->sapi],comp_ent_num);
        }
        else
        {
          MSG_GERAN_MED_3_G("GSN ""LL %d: ",
                      gsn_extern_sapi[nsapi_e_ptr->sapi_ptr->sapi],
                      0,
                      0);
          MSG_GERAN_MED_3_G("GSN ""No Hdr comp PCOMP vals = %d/%d/%d",
                      pcomp_e_ptr->alg.xid_rfc2507.pcomp1,
                      pcomp_e_ptr->alg.xid_rfc2507.pcomp2,
                      pcomp_e_ptr->alg.xid_rfc2507.pcomp3);
          MSG_GERAN_MED_3_G("GSN ""No Hdr comp PCOMP vals = %d/%d",
                      pcomp_e_ptr->alg.xid_rfc2507.pcomp4,
                      pcomp_e_ptr->alg.xid_rfc2507.pcomp5,
                      0);
        }
      }
      else
      {
        /* Compress N-PDU */
        pcomp_val = gsn_iphc_compress( gas_id, pcomp_e_ptr, npdu );

      }
    }
  }
  else
#endif /* FEATURE_GSM_SNDCP_IPHC */
  if(nsapi_e_ptr->pcomp_e_num[T2_ALG_RFC1144] != NO_COMP_ENT )
  {
    /* Use RFC 1144 for PCI compression */
    comp_ent_num = nsapi_e_ptr->pcomp_e_num[T2_ALG_RFC1144];
    pcomp_e_ptr = nsapi_e_ptr->sapi_ptr->pcomp_e_pool[comp_ent_num];

    if ( pcomp_e_ptr != NULL )
    {
      if ( (pcomp_e_ptr->state != ASSIGNED      ) ||
           (!pcomp_e_ptr->alg.xid_rfc1144.pcomp1) ||
           (!pcomp_e_ptr->alg.xid_rfc1144.pcomp2)    )
      {
        if( (pcomp_e_ptr->alg.xid_rfc1144.pcomp1) &&
            (pcomp_e_ptr->alg.xid_rfc1144.pcomp2)
          )
        {
          MSG_GERAN_ERROR_2_G("GSN ""LL %d: Invalid PCOMP entity %d",
          gsn_extern_sapi[nsapi_e_ptr->sapi_ptr->sapi],comp_ent_num);
        }
        else
        {
          MSG_GERAN_MED_3_G("GSN ""LL %d: No Hdr comp PCOMP vals = %d/%d",
            gsn_extern_sapi[nsapi_e_ptr->sapi_ptr->sapi],
              pcomp_e_ptr->alg.xid_rfc1144.pcomp1,
                pcomp_e_ptr->alg.xid_rfc1144.pcomp2);

        }
      }
      else
      {
        /* Compress N-PDU */
        pcomp_val = gsn_1144_compress( gas_id, pcomp_e_ptr, npdu );
      }
    }
  }
  else
  {
    /* MSG_GERAN_ERROR_0_G("GSN ""pcomp entity not available"); */
  }

  return(pcomp_val);
}

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
===    TRUE if N-PDU is decompressed successfully.
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
)
{
  pcomp_t              *pcomp_ptr;
  pcomp_e_t            *pcomp_e_ptr;
  uint8                comp_ent_num;
  boolean              decomp_succ  = FALSE;
  geran_pdu_priority_t pdu_priority = GPDU_LOW_PRIORITY;

  /* Find the data compression algorithm that PCOMP value has
  ** been assigned to. Then, use the compression entity that
  ** is mapped into the algorithm and decompress the packet.
  */
  pcomp_ptr = nsapi_e_ptr->sapi_ptr->
                  pcomp_pool[nsapi_e_ptr->dle.first_seg_info.pcomp];

  /* Check if the PCOMP value and the PCOMP entity are valid */
  if( pcomp_ptr != NULL )
  {
    if( pcomp_ptr->alg > T2_ALG_MAX )
    {
      MSG_GERAN_ERROR_1_G("GSN ""Invalid T2 algorthm %",pcomp_ptr->alg);
    }
    else
    {
      /* Get compression entity */
      comp_ent_num = nsapi_e_ptr->pcomp_e_num[ pcomp_ptr->alg ];
      pcomp_e_ptr= nsapi_e_ptr->sapi_ptr->pcomp_e_pool[comp_ent_num];

      if( ( pcomp_e_ptr == NULL ) ||
        ( pcomp_e_ptr->state != ASSIGNED) )
      {
        MSG_GERAN_ERROR_0_G("GSN ""Invalid PCOMP entity");
      }
      else
      {
        /* Switch on algorithm type */
        switch( pcomp_ptr->alg )
        {
          case T2_ALG_RFC1144:
          {
            if( nsapi_e_ptr->dle.first_seg_info.pcomp ==
                        pcomp_e_ptr->alg.xid_rfc1144.pcomp1 )
            {          
              /* Packet type is "Uncompressed TCP" */
              if( slhc_decompress( ( slhc_handle_type *)pcomp_e_ptr->comp_e,
                                     npdu,
                                     SLHC_PACKET_TYPE_UNCOMPRESSED_TCP
                                    ) == SLHC_FAILED )
              {
                MSG_GERAN_HIGH_1_G("GSN " "RFC 1144:Err Uncompressing Packet,pkt type %d", 
                               SLHC_PACKET_TYPE_UNCOMPRESSED_TCP);
              }
              else
                decomp_succ = TRUE;
            }
            else if( nsapi_e_ptr->dle.first_seg_info.pcomp ==
                        pcomp_e_ptr->alg.xid_rfc1144.pcomp2 )
            {
              /* Packet type is "Compressed TCP" */
              if( slhc_decompress( ( slhc_handle_type *)pcomp_e_ptr->comp_e,
                                     npdu,
                                     SLHC_PACKET_TYPE_COMPRESSED_TCP
                                    ) == SLHC_FAILED )
              {
                MSG_GERAN_HIGH_1_G("GSN " "RFC 1144:Err Uncompressing Packet,pkt type %d", 
                               SLHC_PACKET_TYPE_COMPRESSED_TCP);
              }
              else
                decomp_succ = TRUE;
            }
            else
            {
              MSG_GERAN_ERROR_1_G("GSN ""Inval PCOMP value %d for RFC 1144 ",
                            nsapi_e_ptr->dle.first_seg_info.pcomp);
            }
            break;
          }

#ifdef FEATURE_GSM_SNDCP_IPHC

          case T2_ALG_RFC2507:
          {
            iphc_packet_type_e_type pkt_type;
            dsm_item_type *snd_pkt = NULL;

            if( nsapi_e_ptr->dle.first_seg_info.pcomp ==
                        pcomp_e_ptr->alg.xid_rfc2507.pcomp1 )
            {
              pkt_type = IPHC_PACKET_TYPE_FULL_HEADER;

              /* Packet type is "Full Header" */
              if(iphc_decompress(pcomp_e_ptr->comp_e,
                                 npdu,
                                 &pkt_type,
                                 &snd_pkt) < 0)
              {
                MSG_GERAN_HIGH_0_G("GSN " "RFC 2507:Err Decompressing Packet");
              }
              else
                decomp_succ = TRUE;
            }
            else if( nsapi_e_ptr->dle.first_seg_info.pcomp ==
                        pcomp_e_ptr->alg.xid_rfc2507.pcomp2 )
            {
              pkt_type = IPHC_PACKET_TYPE_COMPRESSED_TCP;

              /* Packet type is "Compressed TCP" */
              if(iphc_decompress(pcomp_e_ptr->comp_e,
                                 npdu,
                                 &pkt_type,
                                 &snd_pkt) < 0)
              {
                MSG_GERAN_HIGH_0_G("GSN " "RFC 2507:Err Decompressing Packet");
              }
              else
                decomp_succ = TRUE;
            }
            else if( nsapi_e_ptr->dle.first_seg_info.pcomp ==
                        pcomp_e_ptr->alg.xid_rfc2507.pcomp3 )
            {
              pkt_type = IPHC_PACKET_TYPE_COMPRESSED_TCP_NODELTA;

              /* Packet type is "Compressed TCP" non-delta */
              if(iphc_decompress(pcomp_e_ptr->comp_e,
                                 npdu,
                                 &pkt_type,
                                 &snd_pkt) < 0)
              {
                MSG_GERAN_HIGH_0_G("GSN " "RFC 2507:Err Decompressing Packet");
              }
              else
                decomp_succ = TRUE;
            }
            else if( nsapi_e_ptr->dle.first_seg_info.pcomp ==
                        pcomp_e_ptr->alg.xid_rfc2507.pcomp4 )
            {

              pkt_type = IPHC_PACKET_TYPE_COMPRESSED_NON_TCP;

              /* Packet type is "Compressed non-TCP" */
              if(iphc_decompress(pcomp_e_ptr->comp_e,
                                 npdu,
                                 &pkt_type,
                                 &snd_pkt) < 0)
              {
                MSG_GERAN_HIGH_0_G("GSN " "RFC 2507:Err Decompressing Packet");
              }
              else
                decomp_succ = TRUE;
            }
            else if( nsapi_e_ptr->dle.first_seg_info.pcomp ==
                        pcomp_e_ptr->alg.xid_rfc2507.pcomp5 )
            {

              pkt_type = IPHC_PACKET_TYPE_CONTEXT_STATE;

              /* Packet type is "Compressed non-TCP" */
              if(iphc_decompress(pcomp_e_ptr->comp_e,
                                 npdu,
                                 &pkt_type,
                                 &snd_pkt) < 0)
              {
                MSG_GERAN_HIGH_0_G("GSN " "RFC 2507:Err Decompressing Packet");
              }
              else
                decomp_succ = TRUE;
            }
            else
            {
              MSG_GERAN_ERROR_1_G("GSN ""Inval PCOMP value % for RCF 2507 ",
                            nsapi_e_ptr->dle.first_seg_info.pcomp);
            }

            /* Send decompressor feedback packet to peer compressor */
            if( snd_pkt != NULL )
            {
              if ( nsapi_e_ptr->op_mode == ACK) 
              {
                gsn_process_sn_data_req( gas_id, nsapi_e_ptr->nsapi, &snd_pkt, TRUE, pdu_priority );
              }
              else if ( nsapi_e_ptr->op_mode == UNACK ) 
              {
                gsn_process_sn_udata_req( gas_id, nsapi_e_ptr->nsapi, &snd_pkt, TRUE, NULL, pdu_priority );
              }
              else
              {
                MSG_GERAN_ERROR_1_G("GSN ""Inval op mode %d ",nsapi_e_ptr->op_mode);
              }
            }
            break;
          }
#endif /* FEATURE_GSM_SNDCP_IPHC */
          default:

            MSG_GERAN_ERROR_0_G("GSN ""Invalid T2/PCI algorthm");
            break;

        } /* Switch on algorithm type */
      }
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN ""Unallocated PCOMP value %",
                  nsapi_e_ptr->dle.first_seg_info.pcomp);
  }

  return( decomp_succ );
}


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
  const gsn_nsapi_e_t   *nsapi_e_ptr    /* NSAPI dropped the N-PDU */
)
{
  pcomp_t   *pcomp_ptr;
  pcomp_e_t *pcomp_e_ptr;
  uint8      ent_num=0;

  /* Find the data compression algorithm that DCOMP value has
  ** been assigned to. Then, use the compression entity that
  ** is mapped into the algorithm and decompress the packet.
  */
  pcomp_ptr = nsapi_e_ptr->sapi_ptr->
                  pcomp_pool[nsapi_e_ptr->dle.first_seg_info.pcomp];

  /* Check if the PCOMP value and the PCOMP entity are valid */
  if( pcomp_ptr != NULL )
  {
    if( pcomp_ptr->alg > T2_ALG_MAX )
    {
      MSG_GERAN_ERROR_1_G("GSN ""Invalid T2 algorthm %",pcomp_ptr->alg);
      return;
    }
    else
     {
       ent_num = nsapi_e_ptr->pcomp_e_num[pcomp_ptr->alg];
       pcomp_e_ptr = nsapi_e_ptr->sapi_ptr->pcomp_e_pool[ent_num];
     }
    if( ( pcomp_e_ptr == NULL ) ||
        ( pcomp_e_ptr->state != ASSIGNED) )
    {
      MSG_GERAN_ERROR_1_G("GSN ""Invalid PCOMP entity: %d",ent_num);
      return;
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN ""Unallocated PCOMP value %",
                  nsapi_e_ptr->dle.first_seg_info.pcomp);
    return;
  }

  /* Switch on algorithm type */
  switch( pcomp_ptr->alg )
  {
    case T2_ALG_RFC1144:

      /* Inform decompressor about the dropped frame */
      (void)slhc_ioctl( (slhc_handle_type *)pcomp_e_ptr->comp_e,
                         SLHC_IOCTL_SET_TOSS,
                         NULL );
      break;

    case T2_ALG_RFC2507:

      /* SNDCP need not do anything as RFC2507 algorithm recovers by itself */
      MSG_GERAN_HIGH_0_G("GSN ""RFC2507 decompression recovers from dropped PDUs");
      break;

    case T2_ALG_RFC3095:

      /* SNDCP need not do anything as RFC3095 algorithm recovers by itself  */
      MSG_GERAN_HIGH_0_G("GSN ""RFC3095 decompression recovers from dropped PDUs");
      break;

    default:

      MSG_GERAN_ERROR_0_G("GSN ""Invalid T2/PCI algorthm");
      break;

   } /* Switch on algorithm type */
}


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
void gsn_ack_remove_fback_npdus_and_renumber( gas_id_t gas_id, gsn_nsapi_e_t *nsapi_e_ptr)
{
  gsn_npdu_item_t     *item_ptr;
  gsn_seg_ref_item_t  *ref_item_ptr;
  uint8                seq_pend_susp      = 0;
  uint8                first_npdu_no      = 0;
  uint8                prev_npdu_no       = 0;
  uint8                curr_npdu_no       = 0;
  boolean              is_first_pdu_fback = FALSE;

  if ( nsapi_e_ptr == NULL )
  {
    MSG_GERAN_ERROR_0_G("GSN ""nsapi entity ptr is NULL");
    return;
  }

  /* If NSAPI is in ACK mode, resume data transfer */
  if ( (nsapi_e_ptr->op_mode == ACK) && nsapi_e_ptr->pci_comp )
  {             
    /* Suspend LLC task to prevent processing of LL_DATA_CNF 
    ** that could modify the contents of the npdu_buff.
    */
    GSN_LOCK();

    /* Feedback NPDU removal is only allowed after reception of 
    ** SNSM_SEQUENCE_IND in case of a inter-SGSN RAU. 
    */
    seq_pend_susp = (nsapi_e_ptr->ule.susp_flag & GSN_UL_SUSP_SEQ_PEND);

    /* If buffered N-PDU is confirmed by the Receive N-PDU Number, 
    ** the N-PDU shall be deleted from the buffer.
    */

    if ( !seq_pend_susp )
    {
      item_ptr = q_check( &(nsapi_e_ptr->ule.npdu_buff) );

      if ( item_ptr != NULL )
      {
        first_npdu_no = item_ptr->ack_send_npdu;

        if( item_ptr->fback_pdu_flag)
        {
          is_first_pdu_fback = TRUE;
        }
      }

      /* Traverse re-transmission buffer and remove feedback PDUs */
      while( item_ptr != NULL )
      {
        /* Check for feed back PDUs and remove from re-transmission buffer */
        if( ( item_ptr->fback_pdu_flag     == TRUE ) &&
            ( item_ptr->seg_ref_buff_valid == TRUE )    )
        {
          ref_item_ptr = q_get( &(item_ptr->seg_ref_buff) );
          while( ref_item_ptr )
          {
            GSN_MEM_FREE( ref_item_ptr );   /* free memory */
            ref_item_ptr = q_get( &(item_ptr->seg_ref_buff) );
          }
   
          /* Set flag to indicate seg_ref_buff is invalid */   
          item_ptr->seg_ref_buff_valid = FALSE;
             
          /* Free memory, mutex etc. used by NPDU segment queue attached to
          ** NPDU.
          */ 
          q_destroy ( &item_ptr->seg_ref_buff );
             
          /* Delete N-PDU. Free memory back to DSM mem pool */
          GPRS_PDU_FREE( &item_ptr->npdu_ptr );

          /* Remove item from the queue */
          item_ptr = q_get( &(nsapi_e_ptr->ule.npdu_buff) );

          if( item_ptr != NULL )
          {

            MSG_GERAN_MED_1_G("GSN ""Fback PDU removed, NPDU no: %d",
              item_ptr->ack_send_npdu);

            /* free memory */
            GSN_MEM_FREE( item_ptr );
          }

          /* Check next item */
          item_ptr = q_check( &(nsapi_e_ptr->ule.npdu_buff) );
        }
        else
        {
          /* Get next item */
          item_ptr = q_next( &(nsapi_e_ptr->ule.npdu_buff), &(item_ptr->link) );
        }
      } /* End traversing the queue */ 


      /* Traverse re-transmission buffer and re-number NPDUs following removal 
      ** of feedback PDUs.
      */

      item_ptr = q_check( &(nsapi_e_ptr->ule.npdu_buff) );

      if (item_ptr != NULL)
      {
        prev_npdu_no = item_ptr->ack_send_npdu;
      }

      while( item_ptr != NULL )
      {

        /* There should not be any feedback PDUs in the buffer. */
        if ( item_ptr->fback_pdu_flag )
        {
          MSG_GERAN_ERROR_0_G("GSN ""Unexpected fback PDU in re-trans buffer");
        }
        /* If first PDUs is a feedback one, then assign that PDU no to the
        ** first non-feedback PDU found in the queue.
        */
        else if ( is_first_pdu_fback )
        {

          MSG_GERAN_MED_2_G("GSN ""NPDU renumbered: %d -> %d", 
            item_ptr->ack_send_npdu, first_npdu_no);

          item_ptr->ack_send_npdu = first_npdu_no;

          if( item_ptr->seg_ref_buff_valid == TRUE)
          {
            ref_item_ptr = q_check( &(item_ptr->seg_ref_buff) );
            while( ref_item_ptr )
            {
              ref_item_ptr->npdu_num = first_npdu_no;
              ref_item_ptr = q_check( &(item_ptr->seg_ref_buff) );
            }
          }
          else
          {
            MSG_GERAN_MED_0_G("GSN ""No seg ref buff");
          }
          first_npdu_no = ((first_npdu_no + 1) % ACK_NPDU_NUM_MAX);
        }
        /* First PDU in the queue was not a feedback one before PDU removal.*/
        else
        {
          curr_npdu_no = item_ptr->ack_send_npdu;
          
          /* Go through queue and check for any gaps in NPDU numbers and 
          ** renumber them.
          */

          if ( curr_npdu_no > ((prev_npdu_no + 1 )% ACK_NPDU_NUM_MAX))
          {
            item_ptr->ack_send_npdu = (prev_npdu_no + 1 )% ACK_NPDU_NUM_MAX;

            MSG_GERAN_MED_2_G("GSN ""NPDU renumbered: %d -> %d", 
              curr_npdu_no, item_ptr->ack_send_npdu);

            curr_npdu_no = item_ptr->ack_send_npdu;

            if( item_ptr->seg_ref_buff_valid == TRUE )
            {
              ref_item_ptr = q_check( &(item_ptr->seg_ref_buff) );
              while( ref_item_ptr )
              {
                ref_item_ptr->npdu_num = item_ptr->ack_send_npdu;
                ref_item_ptr = q_check( &(item_ptr->seg_ref_buff) );
              }
            }
            else
            {
              MSG_GERAN_MED_0_G("GSN ""No seg ref buff");
            }
          }
          prev_npdu_no = curr_npdu_no;
        }

        /* Check next item */
        item_ptr = q_next( &(nsapi_e_ptr->ule.npdu_buff), &(item_ptr->link) );

      } /* End while traversing the queue */ 
    }

    /* Resume LLC task */
    GSN_UNLOCK();
  }
  else
  {
    MSG_GERAN_MED_2_G("GSN ""Fback PDU proc not done: LLC mode %d, Hdr Comp %d ",
	  nsapi_e_ptr->op_mode, nsapi_e_ptr->pci_comp);
  }
}

#endif /* #ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP */


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
  gas_id_t      gas_id,
  dcomp_e_t    *dcomp_e_ptr /* entity to be initialised */
)
{
  boolean init_succ = FALSE;

  if( gsn_v42_alloc_tables
      (
        &dcomp_e_ptr->comp_e,
        dcomp_e_ptr->alg.xid_v42bis.p0,
        dcomp_e_ptr->alg.xid_v42bis.p1
      )
    )
  {
    gsn_V42_initialise
      (
    &dcomp_e_ptr->comp_e,
    dcomp_e_ptr->alg.xid_v42bis.p2,
    dcomp_e_ptr->alg.xid_v42bis.p1,
        dcomp_e_ptr->alg.xid_v42bis.p0,
        dcomp_e_ptr->alg.xid_v42bis.p0
      );

    init_succ = TRUE;
  }
  else
  {
    MSG_GERAN_ERROR_0_G("GSN ""Heap Exhaustion.");
  }

  return( init_succ );

}

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
  dsm_item_type         **npdu           /* PDP N-PDU to be compressed */
)
{
  uint8     dcomp_val = 0;
  uint8     comp_ent_num;
  dcomp_e_t *dcomp_e_ptr;
  uint16    len_comp,len_decomp;
  boolean   init_flag = FALSE;


  /* Check if the NSAPI has an assigned data compression entity
  */
  if( nsapi_e_ptr->dcomp_e_num[T1_ALG_V42BIS] != NO_COMP_ENT )
  {
    /* Use V42bis for data compression
    */
    comp_ent_num = nsapi_e_ptr->dcomp_e_num[T1_ALG_V42BIS];
    dcomp_e_ptr = nsapi_e_ptr->sapi_ptr->dcomp_e_pool[comp_ent_num];

    if ( dcomp_e_ptr != NULL )
    {
      if ( (dcomp_e_ptr->state != ASSIGNED     ) ||
           (!dcomp_e_ptr->alg.xid_v42bis.dcomp1)
         )
      {
        if( dcomp_e_ptr->alg.xid_v42bis.dcomp1 )
        {
          MSG_GERAN_ERROR_2_G("GSN ""LL %d: Invalid DCOMP entity %d",
          gsn_extern_sapi[nsapi_e_ptr->sapi_ptr->sapi],comp_ent_num);
        }
        else
        {
          MSG_GERAN_MED_2_G("GSN ""LL %d: No Data Comp DCOMP val = %d",
            gsn_extern_sapi[nsapi_e_ptr->sapi_ptr->sapi],
              dcomp_e_ptr->alg.xid_v42bis.dcomp1);
        }
      }
      else
      {
        /* Check if MS=>SGSN comression has been agreed */
        if( (dcomp_e_ptr->alg.xid_v42bis.p0 == GSN_V42_MS_TO_SGSN) ||
            (dcomp_e_ptr->alg.xid_v42bis.p0 == GSN_V42_BOTH_DIR) )
        {
          /* Do not V42bis compression use If data load less than 100 bytes.
          ** This is because in UNACK mode the algorithm's dictionary is
          ** reset in a packet by packet bases Therefore, building up the
          ** dictionary for less than 100 bytes it won't have any benefits.
          */
          len_decomp = GPRS_PDU_LENGTH( *npdu );
          if( nsapi_e_ptr->op_mode == UNACK )
          {
            if( len_decomp < UNACK_V42BIS_MIN_COMPRESSIBLE_LOAD )
              return ( dcomp_val );
          }

          /* Indicate to gsn_v42_main() that compression is required */
          dcomp_e_ptr->comp_e.process_uplink    = TRUE;
          dcomp_e_ptr->comp_e.process_downlink  = FALSE;

          /* Extract dsm item to a linear buffer */
          if( !gprs_pdu_strip_head
                ( npdu, dcomp_e_ptr->comp_e.tx_data_buffer.tx_data, len_decomp ))
          {
            if( *npdu )
            {
              GPRS_PDU_FREE( npdu );
            }

            MSG_GERAN_HIGH_0_G("GSN ""gprs_pdu_strip_head() failed");

            return ( 0);
          }

          /* New NPDU, initialise input buffer  */
          dcomp_e_ptr->comp_e.tx_data_buffer.read_p   = 0;
          dcomp_e_ptr->comp_e.tx_data_buffer.write_p  = len_decomp;

          if ( V42_OK == gsn_v42_main( &dcomp_e_ptr->comp_e )  )
          {
            gsn_v42_terminate( &dcomp_e_ptr->comp_e );  /* Flush entity */
            (void)gsn_v42_main( &dcomp_e_ptr->comp_e );       /* Do a V42bis cycle */
            len_comp = gsn_check_V42_tx_buffer(&dcomp_e_ptr->comp_e);

            if( len_comp > len_decomp )
            {
              MSG_GERAN_MED_2_G("GSN ""V42bis produced negative gain len_comp %d len_uncomp %d",
                              len_comp,len_decomp);
            }

            if( (len_comp >= len_decomp) && (nsapi_e_ptr->op_mode == UNACK) )
            {
              /* Compression did not produce any gain. In SNDCP UNACK mode
              ** Send the original NPDU and set DCOMP value to 0 to indicate
              ** no compression. In ACK mode, we have to send the compressed N-PDU
              ** whatever the size.
              */
              dcomp_val = 0;
              *npdu = gsn_pdu_construct( &dcomp_e_ptr->comp_e.tx_data_buffer.tx_data[0], len_decomp );

              if(*npdu == NULL)
              {
                MSG_GERAN_HIGH_1_G("GSN ""gsn_pdu_construct() failed for len %d",len_decomp);
                return ( 0);
              }

              MSG_GERAN_MED_0_G("GSN "" V42bis produced no comp gain. Send uncomp N-PDU");
            }
            else
            {
              dcomp_val = dcomp_e_ptr->alg.xid_v42bis.dcomp1;

              *npdu = gsn_pdu_construct( dcomp_e_ptr->comp_e.TxBuffer.buf, len_comp );

              if(*npdu == NULL)
              {
                MSG_GERAN_HIGH_1_G("GSN ""gsn_pdu_construct() failed for len %d",len_comp);
                return ( 0);
              }
            }
          }
          else
          {
            /* Compression failed. Send the original NPDU
            ** and set DCOMP value to 0 to indicate no Compression.
            */
            dcomp_val = 0;
            *npdu = gsn_pdu_construct( &dcomp_e_ptr->comp_e.tx_data_buffer.tx_data[0], len_decomp );

            if(*npdu == NULL)
            {
              MSG_GERAN_HIGH_1_G("GSN ""gsn_pdu_construct() failed for len %d",len_decomp);
              return ( 0);
            }

            MSG_GERAN_MED_0_G("GSN "" V42bis comp failed. Send uncomp N-PDU");

            /* Force reinitialization */
            init_flag = TRUE;
          }

          if( nsapi_e_ptr->op_mode == UNACK || init_flag == TRUE )
          {
            /* SN_UNITDATA primitive or compression faild.
            ** Reset the compression entity before processing next N-PDU.
            */
            gsn_V42_initialise(
                                &dcomp_e_ptr->comp_e,
                                dcomp_e_ptr->alg.xid_v42bis.p2,
                                dcomp_e_ptr->alg.xid_v42bis.p1,
                                dcomp_e_ptr->alg.xid_v42bis.p0,
                                GSN_V42_MS_TO_SGSN
                              );

            init_flag = FALSE;

            /* If ACK mode, re-establish LLC SAPI */
            if(  nsapi_e_ptr->op_mode == ACK )
            {
              gsn_send_ll_est_req( gas_id, nsapi_e_ptr->sapi_ptr, NULL );
              MSG_GERAN_MED_0_G("GSN "" V42bis comp failed in ACK mode. Re-EST SAPI");
            }
          }

          /* Set output buffer ready for next N-PDU */
          dcomp_e_ptr->comp_e.TxBuffer.read_p  = 0;
          dcomp_e_ptr->comp_e.TxBuffer.write_p = 0;
        }
      }
    }
  }

  return( dcomp_val );
}

/*===========================================================================
===
===  FUNCTION      GSN_DATA_DECOMPRESS( )
===
===  DESCRIPTION
===
===    Performs data decompression on the PDP N-PDU using V42bis algorithm.
===
===  DEPENDENCIES
===
===    None
===
===  RETURN VALUE
===
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
  dsm_item_type         **npdu           /* PDP N-PDU to be decompressed */
)
{

  dcomp_t   *dcomp_ptr;
  dcomp_e_t *dcomp_e_ptr;
  uint8     comp_ent_num;
  uint16    len;
  boolean   init_flag = FALSE;

  /* Find the data compression algorithm that DCOMP value has
  ** been assigned to. Then, use the compression entity that
  ** is mapped into the algorithm and decompress the packet.
  */
  dcomp_ptr = nsapi_e_ptr->sapi_ptr->
                  dcomp_pool[nsapi_e_ptr->dle.first_seg_info.dcomp];

  /* Check if the DCOMP value and the DCOMP entity are valid */
  if( dcomp_ptr != NULL )
  {
    if( dcomp_ptr->alg > T1_ALG_MAX )
    {
      MSG_GERAN_ERROR_1_G("GSN ""Invalid T1 algorthm %",dcomp_ptr->alg);
      return;
    }
    else
    {
      comp_ent_num = nsapi_e_ptr->dcomp_e_num[dcomp_ptr->alg];
      dcomp_e_ptr = nsapi_e_ptr->sapi_ptr->dcomp_e_pool[comp_ent_num];
    }

    if( ( dcomp_e_ptr == NULL ) ||
        ( dcomp_e_ptr->state != ASSIGNED) )
    {
      MSG_GERAN_ERROR_0_G("GSN ""Invalid DCOMP entity");
      return;
    }
  }
  else
  {
    MSG_GERAN_ERROR_1_G("GSN ""Unallocated DCOMP value %",
                  nsapi_e_ptr->dle.first_seg_info.dcomp);
    return;
  }


  /* Check if SGSN to MS comression has been agreed */
  if( (dcomp_e_ptr->alg.xid_v42bis.p0 == GSN_V42_SGSN_TO_MS) ||
      (dcomp_e_ptr->alg.xid_v42bis.p0 == GSN_V42_BOTH_DIR) )
  {
    /* Indicate to gsn_v42_main() that decompression is required */
    dcomp_e_ptr->comp_e.process_uplink    = FALSE;
    dcomp_e_ptr->comp_e.process_downlink  = TRUE;

    /* Extract dsm item to a linear buffer */
    len = GPRS_PDU_LENGTH( *npdu );

    if( len > GSN_MAX_PDU_SIZE )
    {
      MSG_GERAN_MED_1_G("GSN ""V42bis uncomp_len=%d > GSN_MAX_PDU_SIZE",len);
    }

    if( len > V42COMP_BUF_LEN )
    {  
	  len = V42COMP_BUF_LEN;
      MSG_GERAN_HIGH_1_G("GSN ""Truncated length to %d",len);
    }

    if( !gprs_pdu_strip_head( npdu, dcomp_e_ptr->comp_e.RxBuffer.buf, (len) ) )
    {
      if( *npdu )
      {
        GPRS_PDU_FREE( npdu );
      }
      MSG_GERAN_HIGH_0_G("GSN ""gprs_pdu_strip_head() failed");
      return;
    }
    dcomp_e_ptr->comp_e.RxBuffer.read_p = 0;
    dcomp_e_ptr->comp_e.RxBuffer.write_p = len;


    if ( V42_OK != gsn_v42_main( &dcomp_e_ptr->comp_e )  )
    {
      init_flag = TRUE;
      MSG_GERAN_MED_0_G("GSN "" V42bis decomp failed. ");
    }

    len = gsn_v42_check_rx_buffer(&dcomp_e_ptr->comp_e);
    *npdu = gsn_pdu_construct( dcomp_e_ptr->comp_e.rx_data_buffer.rx_data, len );

    if(*npdu == NULL)
    {
      MSG_GERAN_HIGH_1_G("GSN ""gsn_pdu_construct() failed for len %d",len);
    }

    if( len > GSN_MAX_PDU_SIZE )
    {
      MSG_GERAN_HIGH_1_G("GSN ""V42bis dcomp_len=%d > GSN_MAX_PDU_SIZE",len);
    }

    /* Set buffer ready for next N-PDU */
    dcomp_e_ptr->comp_e.rx_data_buffer.read_p  = 0;
    dcomp_e_ptr->comp_e.rx_data_buffer.write_p = 0;

    if( (nsapi_e_ptr->op_mode == UNACK) || init_flag == TRUE )
    {
      /* SN_UNITDATA primitive. Reset the compression entity
      ** before processing next N-PDU.
      */
      gsn_V42_initialise(
                              &dcomp_e_ptr->comp_e,
                              dcomp_e_ptr->alg.xid_v42bis.p2,
                              dcomp_e_ptr->alg.xid_v42bis.p1,
                              dcomp_e_ptr->alg.xid_v42bis.p0,
                              GSN_V42_SGSN_TO_MS
                            );

      init_flag = FALSE;

      /* If ACK mode, re-establish LLC SAPI */
      if(  nsapi_e_ptr->op_mode == ACK )
      {
        gsn_send_ll_est_req( gas_id, nsapi_e_ptr->sapi_ptr, NULL );
        MSG_GERAN_MED_0_G("GSN "" V42bis decomp failed in ACK mode. Re-EST SAPI");
      }
    }
  }
  else
    MSG_GERAN_ERROR_0_G("GSN ""DCOMP SGSN to MS dir is disabled");

}


#endif /* #ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP */

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */


/*** EOF: don't remove! ***/

