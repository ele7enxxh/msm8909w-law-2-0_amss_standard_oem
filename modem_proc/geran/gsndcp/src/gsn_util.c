/*****************************************************************************
***
*** TITLE
***
***  GPRS SNDCP UTILITY FUNCTIONS
***
***
*** DESCRIPTION
***
***   This module includes functions for SNDCP XID manipulation
***   and SN-PDU header encoding/decoding.
***
***
*** EXTERNALIZED FUNCTIONS
***
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gsndcp/src/gsn_util.c#1 $
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

#define FEATURE_DSM_WM_CB

#ifdef FEATURE_GSM_SNDCP_IPHC
#include "ps_iphc.h"
#endif /* FEATURE_GSM_SNDCP_IPHC */

#include "gsn_util.h"
#include "gbitutil.h"
#include "gsn_hdlr.h"
#include "gsn_comp.h"
#include "gllc_v.h"
#include "gllc_v_g.h"
#include "gprs_mem.h"
#include <string.h>



/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/

/* Default XID blocks */
const uint8  xid_all_types_def[] = {
                                    0x00, 0x01, 0x00,   /* T0 */
                                    0x01, 0x00, /* T1 */
                                    0x02, 0x00  /* T2 */
                                   };

const uint8  xid_t0_def[]   = {
                                0x00, 0x01, 0x00,   /* T0 */
                              };

/* Length in bytes of the default XID blocks */
#define XID_DEF_ALL_TYPE_LEN_BLK     7
#define XID_DEF_T0_LEN_BLK           3


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

/* The following data types are used for decoding
** incoming SGSN SNDCP XID blocks
*/
typedef enum
{
  XID_ABSENT = 0, /* Must be 0 */
  XID_INVALID,
  XID_VALID

}gsn_dec_xid_status_t;

typedef struct
{
  gsn_dec_xid_status_t    status;
  gsn_xid_t2_t            xid;

}gsn_dec_t2_ent_t;

typedef struct
{
  gsn_dec_xid_status_t      status;
  gsn_xid_t1_t              xid;

}gsn_dec_t1_ent_t;


typedef struct
{
  /*
  ** T0 XID parameter - SNDCP version number
  *--------------------------------------*/
  struct
  {
    gsn_dec_xid_status_t  status;
    uint8                 ver_num;

  }t0;

  /*
  ** T1 XID parameters - Data compression
  *--------------------------------------*/
  struct
  {
    gsn_dec_xid_status_t  status;
    uint8                 ent_in_blk;
    uint32                ent_map;
    gsn_dec_t1_ent_t      ent[ENT_NUM_MAX + 1];

  }t1;

  /*
  ** T2 XID parameters - PCI compression
  *--------------------------------------*/
  struct
  {
    gsn_dec_xid_status_t  status;
    uint8                 ent_in_blk;
    uint32                ent_map;
    gsn_dec_t2_ent_t      ent[ENT_NUM_MAX + 1];

  }t2;

}gsn_dec_xid_t;

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

/* For more information refer to relevant function headers below.
*/
uint16 gsn_get_ack_nsapis_on_sapi( gsn_sapi_e_t * );

uint16 gsn_get_unack_nsapis_on_sapi( gsn_sapi_e_t * );

void gsn_get_nsapis_on_sapi( gas_id_t, gsn_sapi_e_t *, uint8 * );

void gsn_get_app_nsapis( gsn_nsapi_t *, uint16  );

boolean gsn_decode_peer_xid( gas_id_t gas_id,
                             dsm_item_type **,
                             gsn_dec_xid_t *,
                             gsn_sapi_e_t *sapi_ptr,
                             boolean );

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

void gsn_del_pcomp_e_mapped_to_alg( gas_id_t, gsn_sapi_e_t *, uint8);

uint8 gsn_next_unassigned_pcomp_e( const gsn_sapi_e_t * );

uint8 gsn_next_unassigned_pcomp_val( const gsn_sapi_e_t * );

int8 gsn_create_pcomp_val( uint8 *, uint8, gsn_sapi_e_t *);

boolean gsn_check_pcomp_e( gas_id_t, const pcomp_e_t *, gsn_sapi_e_t * );

void gsn_reset_pcomp_ent( gas_id_t gas_id, pcomp_e_t *pcomp_e_ptr );

pcomp_e_t *gsn_create_pcomp_e( const gsn_xid_t2_t *, gsn_sapi_e_t * );

void gsn_del_all_pcomp_e( gas_id_t, gsn_sapi_e_t *);

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

void gsn_del_dcomp_e_mapped_to_alg( gas_id_t gas_id, gsn_sapi_e_t *, uint8);

uint8 gsn_next_unassigned_dcomp_e( const gsn_sapi_e_t * );

uint8 gsn_next_unassigned_dcomp_val( const gsn_sapi_e_t * );

int8 gsn_create_dcomp_val( uint8 *, uint8, gsn_sapi_e_t * );

boolean gsn_check_dcomp_e( gas_id_t, const dcomp_e_t *, gsn_sapi_e_t * );

void gsn_reset_dcomp_ent( gas_id_t, dcomp_e_t * );

dcomp_e_t *gsn_create_dcomp_e( const gsn_xid_t1_t *, gsn_sapi_e_t * );

void gsn_del_all_dcomp_e( gas_id_t, gsn_sapi_e_t *);

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */


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
===  FUNCTION      GSN_NEXT_UNASSIGNED_PCOMP_E()
===
===  DESCRIPTION
===
===   Returns the next available unassigned PCI compression entity number
===   of the SAPI
===
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    Unassigned PCI compression entity number
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

uint8 gsn_next_unassigned_pcomp_e( const gsn_sapi_e_t *sapi_ptr )
{
  uint8 i;
  uint8 ent_num = NO_COMP_ENT;

  for( i = 0; i <= ENT_NUM_MAX; i++ )
  {
    if( sapi_ptr->pcomp_e_pool[ i ] == NULL )
    {
      /* Found unassigned entity. Exit loop.*/
       ent_num = i;
      break;
    }
  }

  return( ent_num );
}

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_NEXT_UNASSIGNED_DCOMP_E()
===
===  DESCRIPTION
===
===   Returns the next available unassigned Data compression entity
===   number of the SAPI
===
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    Unassigned Data compression entity number
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

uint8 gsn_next_unassigned_dcomp_e( const gsn_sapi_e_t *sapi_ptr )
{
  uint8 i;
  uint8 ent_num = NO_COMP_ENT;

  for( i = 0; i <= ENT_NUM_MAX; i++ )
  {
    if( sapi_ptr->dcomp_e_pool[ i ] == NULL )
    {
      /* Found unassigned entity. Exit loop.*/
       ent_num = i;
      break;
    }
  }

  return( ent_num );
}

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
/*===========================================================================
===
===  FUNCTION      GSN_NEXT_UNASSIGNED_PCOMP_VAL()
===
===  DESCRIPTION
===
===   Returns the next available unassigned PCOMP value of the SAPI
===
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    Unassigned PCOMP value
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
uint8 gsn_next_unassigned_pcomp_val( const gsn_sapi_e_t *sapi_ptr )
{
  uint8 i;
  uint8 pcomp_val = NO_COMP_VAL;

  /* PCOMP=0 and PCOMP=15 are reserved. See TS 0465 par 7.2 */
  for( i = 1; i < PCOMP_VAL_MAX; i++ )
  {
    if( sapi_ptr->pcomp_pool[ i ] == NULL )
    {
      /* Found unassigned value. Exit loop.*/
       pcomp_val = i;
      break;
    }
  }

  return( pcomp_val );
}
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
/*===========================================================================
===
===  FUNCTION      GSN_CHECK_PCOMP_E()
===
===  DESCRIPTION
===
===   Performs consistency check on the compression entity.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   TRUE if all the compression entity parameters are consistent
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
boolean gsn_check_pcomp_e
(
  gas_id_t            gas_id_t,
  const pcomp_e_t     *pcomp_e_ptr, /* Compression entity to be checked */
  gsn_sapi_e_t        *sapi_ptr     /* SAPI in use */
)
{
  boolean       comp_e_consistent = TRUE;
  gsn_nsapi_q_t *nsapi_item_ptr;

  if( pcomp_e_ptr != NULL  )
  {
    if( pcomp_e_ptr ->t2_alg == T2_ALG_RFC1144 )
    {
      if( (pcomp_e_ptr->alg.xid_rfc1144.pcomp1 > PCOMP_VAL_MAX )  ||
          (pcomp_e_ptr->alg.xid_rfc1144.pcomp2 > PCOMP_VAL_MAX )
        )
      {
        comp_e_consistent = FALSE;
      }
      else if( (pcomp_e_ptr->state != ASSIGNED)                    ||
               ( (pcomp_e_ptr->alg.xid_rfc1144.pcomp1 ) &&
                 (pcomp_e_ptr->t2_alg != sapi_ptr->pcomp_pool[pcomp_e_ptr->
                    alg.xid_rfc1144.pcomp1]->alg)
               )                                                  ||
               ( (pcomp_e_ptr->alg.xid_rfc1144.pcomp2 ) &&
                 (pcomp_e_ptr->t2_alg != sapi_ptr->pcomp_pool[pcomp_e_ptr->
                   alg.xid_rfc1144.pcomp2]->alg)
               )
             )
      {
        comp_e_consistent = FALSE;
      }
      else
      {

        if( sapi_ptr->is_nsapi_ptr_q_valid )
        {
          nsapi_item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

          while( nsapi_item_ptr != NULL )
          {
            if( (pcomp_e_ptr->alg.xid_rfc1144.nsapis & APP_NSAPIS_MASK &
                    (0x1 << GSN_EXTERN_NSAPI(nsapi_item_ptr->nsapi_ptr->nsapi))) )
            {
              if( (nsapi_item_ptr->nsapi_ptr->op_mode != pcomp_e_ptr->op_mode )  ||
                  (nsapi_item_ptr->nsapi_ptr->pcomp_e_num[T2_ALG_RFC1144] !=
                      pcomp_e_ptr->ent_num) )
              {
                comp_e_consistent = FALSE;
              }

            }
            /* Get next NSAPI from the list */
            nsapi_item_ptr = (gsn_nsapi_q_t*)
              q_next( &(sapi_ptr->nsapi_ptr_q), &nsapi_item_ptr->link );
          }
        }
        else
        {
          comp_e_consistent = FALSE;
          MSG_GERAN_ERROR_0("GSN "" NSAPI ptr queue invalid ");
        }
      }
    }
    else if( pcomp_e_ptr ->t2_alg == T2_ALG_RFC2507 )
    {
      if( (pcomp_e_ptr->alg.xid_rfc2507.pcomp1 > PCOMP_VAL_MAX) ||
          (pcomp_e_ptr->alg.xid_rfc2507.pcomp2 > PCOMP_VAL_MAX) ||
          (pcomp_e_ptr->alg.xid_rfc2507.pcomp3 > PCOMP_VAL_MAX) ||
          (pcomp_e_ptr->alg.xid_rfc2507.pcomp4 > PCOMP_VAL_MAX) ||
          (pcomp_e_ptr->alg.xid_rfc2507.pcomp5 > PCOMP_VAL_MAX)
        )
      {
        comp_e_consistent = FALSE;
      }
      else if( (pcomp_e_ptr->state != ASSIGNED)                    ||
               ( (pcomp_e_ptr->alg.xid_rfc2507.pcomp1 ) &&
                 (pcomp_e_ptr->t2_alg != sapi_ptr->pcomp_pool[pcomp_e_ptr->
                       alg.xid_rfc2507.pcomp1]->alg)
               )                                                   ||
               ( (pcomp_e_ptr->alg.xid_rfc2507.pcomp2 ) &&
                 (pcomp_e_ptr->t2_alg != sapi_ptr->pcomp_pool[pcomp_e_ptr->
                       alg.xid_rfc2507.pcomp2]->alg)
               )                                                   ||
               ( (pcomp_e_ptr->alg.xid_rfc2507.pcomp3 ) &&
                 (pcomp_e_ptr->t2_alg != sapi_ptr->pcomp_pool[pcomp_e_ptr->
                       alg.xid_rfc2507.pcomp3]->alg)
               )                                                   ||
               ( (pcomp_e_ptr->alg.xid_rfc2507.pcomp4 ) &&
                 (pcomp_e_ptr->t2_alg != sapi_ptr->pcomp_pool[pcomp_e_ptr->
                       alg.xid_rfc2507.pcomp4]->alg)
               )                                                   ||
               ( (pcomp_e_ptr->alg.xid_rfc2507.pcomp5 ) &&
                 (pcomp_e_ptr->t2_alg != sapi_ptr->pcomp_pool[pcomp_e_ptr->
                      alg.xid_rfc2507.pcomp5]->alg)
               )
             )
      {
        comp_e_consistent = FALSE;
      }
      else
      {

        if( sapi_ptr->is_nsapi_ptr_q_valid )
        {
          nsapi_item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

          while( nsapi_item_ptr != NULL )
          {
            if( (pcomp_e_ptr->alg.xid_rfc2507.nsapis & APP_NSAPIS_MASK &
                    (0x1 << GSN_EXTERN_NSAPI(nsapi_item_ptr->nsapi_ptr->nsapi))) )
            {
              if( (nsapi_item_ptr->nsapi_ptr->op_mode != pcomp_e_ptr->op_mode )  ||
                  (nsapi_item_ptr->nsapi_ptr->pcomp_e_num[T2_ALG_RFC2507] !=
                      pcomp_e_ptr->ent_num) )
              {
                comp_e_consistent = FALSE;
              }
            }
            /* Get next NSAPI from the list */
            nsapi_item_ptr = (gsn_nsapi_q_t*)
              q_next( &(sapi_ptr->nsapi_ptr_q), &nsapi_item_ptr->link );
          }
        }
        else
        {
          comp_e_consistent = FALSE;
          MSG_GERAN_ERROR_0("GSN "" NSAPI ptr queue invalid ");
        }
      }
    }
    else if (pcomp_e_ptr ->t2_alg == T2_ALG_RFC3095 )
    {
      if( (pcomp_e_ptr->alg.xid_rfc3095.pcomp1 > PCOMP_VAL_MAX) ||
          (pcomp_e_ptr->alg.xid_rfc3095.pcomp2 > PCOMP_VAL_MAX)
        )
      {
        comp_e_consistent = FALSE;
      }
      else if( (pcomp_e_ptr->state != ASSIGNED)                    ||
               ( (pcomp_e_ptr->alg.xid_rfc3095.pcomp1 ) &&
               (pcomp_e_ptr->t2_alg != sapi_ptr->pcomp_pool[pcomp_e_ptr->
                    alg.xid_rfc3095.pcomp1]->alg)
               )                                                   ||
               ( (pcomp_e_ptr->alg.xid_rfc3095.pcomp2 ) &&
                 (pcomp_e_ptr->t2_alg != sapi_ptr->pcomp_pool[pcomp_e_ptr->
                    alg.xid_rfc3095.pcomp2]->alg)
               )
             )
      {
        comp_e_consistent = FALSE;
      }
      else
      {

        if( sapi_ptr->is_nsapi_ptr_q_valid )
        {
          nsapi_item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

          while( nsapi_item_ptr != NULL )
          {
            if( (pcomp_e_ptr->alg.xid_rfc3095.nsapis & APP_NSAPIS_MASK &
                    (0x1 << GSN_EXTERN_NSAPI(nsapi_item_ptr->nsapi_ptr->nsapi))) )
            {
              if( (nsapi_item_ptr->nsapi_ptr->op_mode != pcomp_e_ptr->op_mode )  ||
                  (nsapi_item_ptr->nsapi_ptr->pcomp_e_num[T2_ALG_RFC3095] !=
                      pcomp_e_ptr->ent_num) )
              {
                comp_e_consistent = FALSE;
              }
            }
            /* Get next NSAPI from the list */
            nsapi_item_ptr = (gsn_nsapi_q_t*)
              q_next( &(sapi_ptr->nsapi_ptr_q), &nsapi_item_ptr->link );
          }
        }
        else
        {
          comp_e_consistent = FALSE;
          MSG_GERAN_ERROR_0("GSN "" NSAPI ptr queue invalid ");
        }
      }
    }
    else
    {
      MSG_GERAN_HIGH_1("GSN "" Invalid PCI algorithm %d", pcomp_e_ptr ->t2_alg);
      comp_e_consistent = FALSE;
    }
  }
  else
    comp_e_consistent = FALSE;

  return ( comp_e_consistent );
}
#endif /*  FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_CHECK_DCOMP_E()
===
===  DESCRIPTION
===
===   Performs consistency check on the compression entity.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   TRUE if all the compression entity parameters are consistent
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
boolean gsn_check_dcomp_e
(
  gas_id_t            gas_id,
  const dcomp_e_t     *dcomp_e_ptr, /* Compression entity to be checked */
  gsn_sapi_e_t        *sapi_ptr     /* SAPI in use */
)
{
  boolean       comp_e_consistent = TRUE;
  gsn_nsapi_q_t *nsapi_item_ptr;

  if( dcomp_e_ptr != NULL  )
  {
    if( dcomp_e_ptr->t1_alg == T1_ALG_V42BIS )
    {
      if( (dcomp_e_ptr->state != ASSIGNED)                    ||
          (dcomp_e_ptr->t1_alg!= sapi_ptr->dcomp_pool[dcomp_e_ptr->
                alg.xid_v42bis.dcomp1]->alg) )
      {
        comp_e_consistent = FALSE;
      }
      else
      {

        if( sapi_ptr->is_nsapi_ptr_q_valid )
        {
          nsapi_item_ptr = (gsn_nsapi_q_t*)q_check( &(sapi_ptr->nsapi_ptr_q) );

          while( nsapi_item_ptr != NULL )
          {
            if( (dcomp_e_ptr->alg.xid_v42bis.nsapis & APP_NSAPIS_MASK &
                    (0x1 << GSN_EXTERN_NSAPI(nsapi_item_ptr->nsapi_ptr->nsapi))) )
            {
              if( (nsapi_item_ptr->nsapi_ptr->op_mode != dcomp_e_ptr->op_mode )  ||
                  (nsapi_item_ptr->nsapi_ptr->dcomp_e_num[T1_ALG_V42BIS] !=
                      dcomp_e_ptr->ent_num) )
              {
                comp_e_consistent = FALSE;
              }

            }
            /* Get next NSAPI from the list */
            nsapi_item_ptr = (gsn_nsapi_q_t*)
                q_next( &(sapi_ptr->nsapi_ptr_q), &nsapi_item_ptr->link );
          }
        }
        else
        {
          comp_e_consistent = FALSE;
          MSG_GERAN_ERROR_0("GSN "" NSAPI ptr queue invalid ");
        }
      }
    }
    else
    {
      MSG_GERAN_HIGH_1("GSN "" Invalid DATA algorithm %d", dcomp_e_ptr->t1_alg);
      comp_e_consistent = FALSE;
    }
  }
  else
    comp_e_consistent = FALSE;

  return ( comp_e_consistent );
}

#endif /*  FEATURE_GSM_GPRS_SNDCP_DCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_NEXT_UNASSIGNED_DCOMP_VAL()
===
===  DESCRIPTION
===
===   Returns the next available unassigned DCOMP value of the SAPI
===
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    Unassigned DCOMP value
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
uint8 gsn_next_unassigned_dcomp_val( const gsn_sapi_e_t *sapi_ptr )
{
  uint8 i;
  uint8 dcomp_val = NO_COMP_VAL;

  /* DCOMP=0 and DCOMP=15 are reserved. See TS 0465 par 7.2 */
  for( i = 1; i < DCOMP_VAL_MAX; i++ )
  {
    if( sapi_ptr->dcomp_pool[ i ] == NULL )
    {
      /* Found unassigned value. Exit loop.*/
       dcomp_val = i;
      break;
    }
  }

  return( dcomp_val );
}

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_RESET_DCOMP_ENT()
===
===  DESCRIPTION
===
===   Resets the Data compression entity
===
===
===  DEPENDENCIES
===
===   None
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
void gsn_reset_dcomp_ent( gas_id_t gas_id, dcomp_e_t *dcomp_e_ptr )
{
  if( dcomp_e_ptr->state == ASSIGNED )
  {
    gsn_V42_initialise
        (
          &dcomp_e_ptr->comp_e,
          dcomp_e_ptr->alg.xid_v42bis.p2,
          dcomp_e_ptr->alg.xid_v42bis.p1,
          dcomp_e_ptr->alg.xid_v42bis.p0,
          dcomp_e_ptr->alg.xid_v42bis.p0
        );
  }
}
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */


#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
/*===========================================================================
===
===  FUNCTION      GSN_RESET_PCOMP_ENT()
===
===  DESCRIPTION
===
===   Resets the Protocol Control Information  compression entity
===
===
===  DEPENDENCIES
===
===   None
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

void gsn_reset_pcomp_ent( gas_id_t gas_id, pcomp_e_t *pcomp_e_ptr )
{
  if( pcomp_e_ptr->state == ASSIGNED )
  {

    if(pcomp_e_ptr->t2_alg == T2_ALG_RFC1144)
    {
      slhc_ioctl_u_type ioctl_data;
      int32 result;

      ioctl_data.options.rslots = pcomp_e_ptr->alg.xid_rfc1144.s0_1;
      ioctl_data.options.tslots = pcomp_e_ptr->alg.xid_rfc1144.s0_1;

      result = slhc_ioctl ( (slhc_handle_type *) pcomp_e_ptr->comp_e,
                     SLHC_IOCTL_CHANNEL_REINITIALIZATION,
                     &ioctl_data );

      if(result < 0)
      {
        MSG_GERAN_ERROR_1("GSN ""slhc reinit failed: %d",result);
      }
    }
#ifdef FEATURE_GSM_SNDCP_IPHC
    else if(pcomp_e_ptr->t2_alg == T2_ALG_RFC2507)
    {

      iphc_ioctl_u_type ioctl_data;
      iphc_config_options_s_type   iphc_options;
      int32 result;

      iphc_options.tcp_space     = pcomp_e_ptr->alg.xid_rfc2507.tcp_sp;
      iphc_options.non_tcp_space = pcomp_e_ptr->alg.xid_rfc2507.ntcp_sp;
      iphc_options.f_max_period  = pcomp_e_ptr->alg.xid_rfc2507.max_per;
      iphc_options.f_max_time    = pcomp_e_ptr->alg.xid_rfc2507.max_time;
      iphc_options.max_header    = pcomp_e_ptr->alg.xid_rfc2507.max_hdr;
      iphc_options.rtp_enabled   = FALSE;
      iphc_options.expect_reordering = FALSE;
      ioctl_data.options = iphc_options;
      result = gsn_iphc_ioctl(gas_id,
                              (void *)pcomp_e_ptr->comp_e,
                              IPHC_IOCTL_COMPRESSOR_V4_SET_OPTIONS,
                              &ioctl_data);
      if(result < 0)
      {
        MSG_GERAN_ERROR_1("GSN ""iphc reinit failed: %d",result);

      }
    }
#endif /* FEATURE_GSM_SNDCP_IPHC */
    else
    {
      MSG_GERAN_ERROR_1("GSN ""Invalid t2 alg %d",pcomp_e_ptr->t2_alg);
    }
  }
}

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

/*===========================================================================
===
===  FUNCTION      GSN_BLD_REJ_T1_XID()
===
===  DESCRIPTION
===
===   Builds up a T1 (Data compression) XID parameter field with Applicable NSAPIs
===   set to zero, to be used as a response to an invalid/unsupported XID request
===
===  DEPENDENCIES
===
===   The calling function has to fill in the "Entity Number" and the
===   "Type length" field in xid_i.
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
 void gsn_bld_rej_t1_xid
(
    gas_id_t         gas_id,
    gsn_xid_block_t *xid_i,       /* Internal format of T1 XID  */
    uint8           *xid_ex,      /* Array of bytes */
    dsm_item_type   **xid_neg     /* Will convey the negotiated XID  */
)
{
  uint16    xid_neg_len;

  xid_i->type   = XID_TYPE_1;

  if( xid_i->length )
    xid_i->length = XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

  xid_i->type_field.xid_t1.hdr.p          = 0;
  xid_i->type_field.xid_t1.hdr.x1         = 0;

  /* Skip "Entity Number" field, it has been filled
  ** in by the calling function
  */
  xid_i->type_field.xid_t1.hdr.x2         = 0;
  xid_i->type_field.xid_t1.hdr.alg_type   = T1_ALG_V42BIS;

  xid_i->type_field.xid_t1.hdr.length     = APP_NSAPIS_LEN;
  xid_i->type_field.xid_t1.alg.xid_v42bis.nsapis = 0;

  /* Encode T1 XID block */
  xid_neg_len = gsn_encode_xid( xid_i, xid_ex );

  /* check length of T1 XID */
  if( xid_i->length )
  {
    if( xid_neg_len != (xid_i->length + XID_TYPE_HDR_LEN) )
      MSG_GERAN_ERROR_0("GSN ""XID encode. Error encoding T1 XID params");
  }
  else
  {
    if( xid_neg_len != (APP_NSAPIS_LEN + XID_FIELD_HDR_P0_LEN ) )
      MSG_GERAN_ERROR_0("GSN ""XID encode. Error encoding T1 XID params");
  }


  /* Check if there is need to allocate a pdu item to carry the XID block */
  if( *xid_neg != NULL )
  {
    /* Append XID type 1 block to the existing XID block */
    if( !gprs_pdu_append_head( xid_neg, xid_ex, (uint16)xid_neg_len) )
    {
      MSG_GERAN_ERROR_0("GSN ""XID RSP append XID block failed");
    }
  }
  else
  {
    /* Create a pdu item to carry the XID block */
    *xid_neg = gsn_pdu_construct( xid_ex, (uint16)xid_neg_len );
  }
}

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_CREATE_DCOMP_E()
===
===  DESCRIPTION
===
===   Deletes the Data compession entity
===
===  DEPENDENCIES
===
===   None
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
dcomp_e_t *gsn_create_dcomp_e( const gsn_xid_t1_t *t1, gsn_sapi_e_t *sapi_ptr )
{
  uint8       ent_num;
  t1_alg_q_t  *q_item;
  dcomp_e_t   *dcomp_e_ptr = NULL;

  ent_num = t1->hdr.ent_num;

  /* Ensure that comp entity number is valid */
  if( (ent_num <= ENT_NUM_MAX) &&
      (sapi_ptr->dcomp_e_pool[ent_num] == NULL) )
  {
    /* Allocate memmory for the new comp entity */
    sapi_ptr->dcomp_e_pool[ ent_num ] =
            GPRS_MEM_MALLOC( sizeof(dcomp_e_t) );
    GSN_ASSERT( sapi_ptr->dcomp_e_pool[ ent_num ] != NULL );

    dcomp_e_ptr = sapi_ptr->dcomp_e_pool[ ent_num ];

    /* Initialise Entity */
    dcomp_e_ptr->ent_num = ent_num;
    dcomp_e_ptr->op_mode = UNDEF;
    dcomp_e_ptr->state   = UNASSIGNED;
    memset( &dcomp_e_ptr->comp_e, 0, sizeof(comp_ent_V42bis_t) );

    if( t1->hdr.alg_type == T1_ALG_V42BIS )
    {
      dcomp_e_ptr->t1_alg = T1_ALG_V42BIS;
      dcomp_e_ptr->alg.xid_v42bis.dcomp1 = t1->alg.xid_v42bis.dcomp1;
      dcomp_e_ptr->alg.xid_v42bis.nsapis = t1->alg.xid_v42bis.nsapis;
      dcomp_e_ptr->alg.xid_v42bis.p0     = t1->alg.xid_v42bis.p0;
      dcomp_e_ptr->alg.xid_v42bis.p1     = t1->alg.xid_v42bis.p1;
      dcomp_e_ptr->alg.xid_v42bis.p2     = t1->alg.xid_v42bis.p2;
    }
    else
    {
      MSG_GERAN_ERROR_2("GSN ""XID %d: Inval Comp Algorithm %d",
                    gsn_extern_sapi[sapi_ptr->sapi],t1->hdr.alg_type);

      GSN_MEM_FREE( dcomp_e_ptr );
      return( NULL );
    }

    /* Link entity with the Data compression algorithm
    */

    /* Allocate memory for the item to be queued */
    q_item = GPRS_MEM_MALLOC( sizeof(t1_alg_q_t) );

    GSN_ASSERT( q_item != NULL ); /* check for valid pointer */

    q_item->dcomp_e_ptr = dcomp_e_ptr;

    /* First initialize the queue link. This is always done
    ** before putting an item in a queue.
    */
    (void)q_link(q_item, &(q_item->link));

    /* Put item to the queue (list) */
    q_put( &(sapi_ptr->t1_alg_q[t1->hdr.alg_type]), &(q_item->link) );

  }
  else
  {
    MSG_GERAN_ERROR_2("GSN ""XID %d: Inval Entity %d",
                    gsn_extern_sapi[sapi_ptr->sapi],ent_num);
    return( NULL );
  }

  return( dcomp_e_ptr );
}
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_CREATE_DCOMP_VAL()
===
===  DESCRIPTION
===
===   Creates a DCOMP value
===
===  DEPENDENCIES
===
===   None
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
int8 gsn_create_dcomp_val
(
  uint8 *dcomp_val,         /* DCOMP values to be created */
  uint8 alg,                /* Algorithm the DCOMP value gets assigned to */
  gsn_sapi_e_t *sapi_ptr    /* SAPI in use */
)
{
  uint8   i, dcomp_max;
  int8    dcomp_val_cnt = 0;

  if( alg == T1_ALG_V42BIS )
    dcomp_max = V42BIS_DCOMP_MAX;
  else
  {
    MSG_GERAN_HIGH_0("GSN ""XID Invalid algorithm");
    return( -1 );
  }

  /* DCOMP 0 is reserved for no compression */
  for( i = 1; i <= dcomp_max; i++ )
  {
    /* Get next available DCOMP value from the pool */
    dcomp_val[i] = gsn_next_unassigned_dcomp_val( sapi_ptr );

    /* Increment DCOMP value assigned counter */
    dcomp_val_cnt++;

    /* Ensure that DCOMP value is valid. DCOMP 15 is reserved for future use */
    if( (dcomp_val[i] < DCOMP_VAL_MAX )   &&
        (sapi_ptr->dcomp_pool[dcomp_val[i]] == NULL) )
    {
      sapi_ptr->dcomp_pool[ dcomp_val[i] ] =
            GPRS_MEM_MALLOC( sizeof(dcomp_t) );
      GSN_ASSERT( sapi_ptr->dcomp_pool[ dcomp_val[i] ] != NULL );

      sapi_ptr->dcomp_pool[ dcomp_val[i] ]->alg     = alg;
      sapi_ptr->dcomp_pool[ dcomp_val[i] ]->state   = UNASSIGNED;
      sapi_ptr->dcomp_pool[ dcomp_val[i] ]->val_num = dcomp_val[i];
    }
    else
    {
      MSG_GERAN_HIGH_2("GSN ""XID %d: Inval DCOMP val %d",
                      gsn_extern_sapi[sapi_ptr->sapi],dcomp_val[i]);
      return( -1 );
    }
  }
  return( dcomp_val_cnt );
}
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_DEL_ALL_DCOMP_E()
===
===  DESCRIPTION
===
===   Deletes all Data compession entities on the SAPI
===
===  DEPENDENCIES
===
===   None
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
void gsn_del_all_dcomp_e( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr)
{
  /* Change to uint8 when more data compression algorithms added and
  ** T1_ALG_MAX != 0. int8 is used to avoid unassigned comparison with 0 warning.
  */
  int8  t1_alg;

  for( t1_alg = 0; t1_alg <= T1_ALG_MAX; t1_alg++ )
  {
    if( q_cnt( &sapi_ptr->t1_alg_q[t1_alg] ) )
      gsn_del_dcomp_e_mapped_to_alg( gas_id, sapi_ptr, (uint8)t1_alg );
  }
}
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */


#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_DEL_DCOMP_E_MAPPED_TO_ALG()
===
===  DESCRIPTION
===
===   Deletes all SAPI's Data compression entities that
===   use the T1 algorithm.
===
===  DEPENDENCIES
===
===   None
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
void gsn_del_dcomp_e_mapped_to_alg( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, uint8 t1_alg )
{
  t1_alg_q_t        *q_item = NULL;
  t1_alg_q_t        *q_next_item;
  gsn_nsapi_q_t     *nsapi_q_item;


  if( sapi_ptr->is_nsapi_ptr_q_valid )
  {
    if( q_cnt( &sapi_ptr->t1_alg_q[t1_alg] ) )
    {
      /* One or more comp entities use the comp algorithm.
      ** Remove entities from the list
      */
      q_item = q_get( &sapi_ptr->t1_alg_q[t1_alg] );
      while( q_item != NULL )
      {
        /* Inform App NSAPIs to stop using the entity */
        nsapi_q_item = q_check( &sapi_ptr->nsapi_ptr_q );
        while( nsapi_q_item != NULL )
        {
          if( (t1_alg == T1_ALG_V42BIS) &&
              (q_item->dcomp_e_ptr->t1_alg == T1_ALG_V42BIS) )
          {
            if( (q_item->dcomp_e_ptr->alg.xid_v42bis.nsapis) &
                (0x01 << GSN_EXTERN_NSAPI(nsapi_q_item->nsapi_ptr->nsapi)) )
            {
              /* Delink NSAPI from the comp entity */
              actv_nsapis[ gas_id ][nsapi_q_item->nsapi_ptr->nsapi]->
                                    dcomp_e_num[T1_ALG_V42BIS] = 0xff;
            }
          }
          else
          {
            MSG_GERAN_HIGH_1("GSN ""XID %d: Inval T1 algorithm",
                  gsn_extern_sapi[sapi_ptr->sapi]);
          }

          /* Get next nsapi  */
          nsapi_q_item = q_next( &sapi_ptr->nsapi_ptr_q, &nsapi_q_item->link );

        } /* End while loop */

        if( !q_cnt( &sapi_ptr->t1_alg_q[t1_alg] ) )
        {
          /* No comp entities use the algorithm. Unassign DCOMP values */
          if( t1_alg == T1_ALG_V42BIS )
          {
            if ( q_item->dcomp_e_ptr->alg.xid_v42bis.dcomp1  &&
               sapi_ptr->dcomp_pool[q_item->dcomp_e_ptr->alg.xid_v42bis.dcomp1] )
            {
              GSN_MEM_FREE( sapi_ptr->dcomp_pool
               [q_item->dcomp_e_ptr->alg.xid_v42bis.dcomp1] );
            }
          }
        }

        /* Free memmory allocated for V42bis tables */
        if( t1_alg == T1_ALG_V42BIS )
        {
          gsn_v42_dealloc_tables( gas_id, &q_item->dcomp_e_ptr->comp_e,
                                  q_item->dcomp_e_ptr->alg.xid_v42bis.p0 );
        }

        MSG_GERAN_MED_2("GSN ""XID %d: Rmv DCOMP entity %d", q_item->dcomp_e_ptr->ent_num,
                                                   0);

        /* Put entity back to the pool of unassigned entities */
        GSN_MEM_FREE( sapi_ptr->dcomp_e_pool[q_item->dcomp_e_ptr->ent_num] );

        /* Get next item before freeing item's memory */
        q_next_item = q_next( &sapi_ptr->t1_alg_q[t1_alg], &q_item->link );

        GSN_MEM_FREE( q_item );

        /* Get next queue item */
        q_item = q_next_item;

      } /* End while loop */
    }
    else
    {
      /* At least one item should be in the queue */
      MSG_GERAN_HIGH_1("GSN ""XID %d: Resp inval T1 alg queue",
                  gsn_extern_sapi[sapi_ptr->sapi]);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("GSN "" XID %d: NSAPI ptr queue invalid",
                    gsn_extern_sapi[sapi_ptr->sapi]);
  }
}
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

/*===========================================================================
===
===  FUNCTION      GSN_BLD_REJ_T2_XID()
===
===  DESCRIPTION
===
===   Builds up a T2 (PCI compression) XID parameter field with Applicable NSAPIs
===   set to zero, to be used as a response to an invalid/unsupported XID request
===
===  DEPENDENCIES
===
===   The calling function has to fill in the "Entity Number" and the
===   "Type length" field in xid_i.
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
void gsn_bld_rej_t2_xid
(
  gas_id_t         gas_id,
  gsn_xid_block_t  *xid_i,       /* Internal format of T2 XID  */
  uint8            *xid_ex,      /* Array of bytes */
  dsm_item_type    ** xid_neg    /* Will convey the negotiated XID  */
)
{
  uint8 xid_neg_len;

  xid_i->type   = XID_TYPE_2;

  if( xid_i->length )
    xid_i->length = XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

  xid_i->type_field.xid_t2.hdr.p          = 0;
  xid_i->type_field.xid_t2.hdr.x1         = 0;

  /* Skip "Entity Number" field, it has been filled
  ** in by the calling function
  */
  xid_i->type_field.xid_t2.hdr.x2         = 0;
  xid_i->type_field.xid_t2.hdr.alg_type   = T2_ALG_RFC1144;
  xid_i->type_field.xid_t2.hdr.length     = APP_NSAPIS_LEN;
  xid_i->type_field.xid_t2.alg.xid_rfc1144.nsapis = 0;
  /* xid_i->type_field.xid_t2.alg.xid_rfc2507.nsapis = 0;
  xid_i->type_field.xid_t2.alg.xid_rfc3095.nsapis = 0; */

  /* Encode T2 XID block */
  xid_neg_len = gsn_encode_xid( xid_i, xid_ex );

  /* check length of T2 XID */
  if( xid_i->length )
  {
    if( xid_neg_len != (xid_i->length + XID_TYPE_HDR_LEN) )
      MSG_GERAN_ERROR_0("GSN ""XID encode. Error encoding T2 XID params");
  }
  else
  {
    if( xid_neg_len != (APP_NSAPIS_LEN + XID_FIELD_HDR_P0_LEN ) )
      MSG_GERAN_ERROR_0("GSN ""XID encode. Error encoding T2 XID params");
  }

  /* Check if there is need to allocate a pdu item to carry the XID block */
  if( *xid_neg != NULL )
  {
    /* Append XID T2 block to the existing XID block */
    if( !gprs_pdu_append_head( xid_neg, xid_ex, (uint16)xid_neg_len) )
    {
      MSG_GERAN_ERROR_0("GSN ""XID RSP append XID block failed");
    }
  }
  else
  {
    /* Create a pdu item to carry the XID block */
    *xid_neg = gsn_pdu_construct( xid_ex, (uint16)xid_neg_len );
  }
}

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
/*===========================================================================
===
===  FUNCTION      GSN_CREATE_PCOMP_E()
===
===  DESCRIPTION
===
===   Creates a PCI compession entity
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    Pointer to the compression entity created
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
pcomp_e_t *gsn_create_pcomp_e( const gsn_xid_t2_t *t2, gsn_sapi_e_t *sapi_ptr )
{
  uint8       ent_num;
  t2_alg_q_t  *q_item;
  pcomp_e_t   *pcomp_e_ptr;

  ent_num = t2->hdr.ent_num;

  /* Ensure that comp entity number is valid */
  if( (ent_num <= ENT_NUM_MAX) &&
      (sapi_ptr->pcomp_e_pool[ent_num] == NULL) )
  {
    /* Allocate memory for the new comp entity */
    sapi_ptr->pcomp_e_pool[ ent_num ] =
            GPRS_MEM_MALLOC( sizeof(pcomp_e_t) );
    GSN_ASSERT( sapi_ptr->pcomp_e_pool[ ent_num ] != NULL );
    memset( sapi_ptr->pcomp_e_pool[ ent_num ], 0, sizeof(pcomp_e_t));

    pcomp_e_ptr = sapi_ptr->pcomp_e_pool[ ent_num ];

    /* Initialise Entity */
    pcomp_e_ptr->ent_num = ent_num;
    pcomp_e_ptr->op_mode = UNDEF;
    pcomp_e_ptr->state   = UNASSIGNED;

    if( t2->hdr.alg_type == T2_ALG_RFC1144 )
    {
      /* Memory for RFC1144 compression instance is now dynamically allocated
         in gsn_1144_comp_init() */

      pcomp_e_ptr->t2_alg = T2_ALG_RFC1144;
      pcomp_e_ptr->alg.xid_rfc1144.pcomp1 = t2->alg.xid_rfc1144.pcomp1;
      pcomp_e_ptr->alg.xid_rfc1144.pcomp2 = t2->alg.xid_rfc1144.pcomp2;
      pcomp_e_ptr->alg.xid_rfc1144.nsapis = t2->alg.xid_rfc1144.nsapis;
      pcomp_e_ptr->alg.xid_rfc1144.s0_1   = t2->alg.xid_rfc1144.s0_1;

    }
    else if( t2->hdr.alg_type == T2_ALG_RFC2507 )
    {

      /* Memory for RFC2507 compression instance is now dynamically allocated
         in gsn_iphc_comp_init() */

      pcomp_e_ptr->t2_alg = T2_ALG_RFC2507;
      pcomp_e_ptr->alg.xid_rfc2507.pcomp1 = t2->alg.xid_rfc2507.pcomp1;
      pcomp_e_ptr->alg.xid_rfc2507.pcomp2 = t2->alg.xid_rfc2507.pcomp2;
      pcomp_e_ptr->alg.xid_rfc2507.pcomp3 = t2->alg.xid_rfc2507.pcomp3;
      pcomp_e_ptr->alg.xid_rfc2507.pcomp4 = t2->alg.xid_rfc2507.pcomp4;
      pcomp_e_ptr->alg.xid_rfc2507.pcomp5 = t2->alg.xid_rfc2507.pcomp5;
      pcomp_e_ptr->alg.xid_rfc2507.pcomp6 = 0;

      pcomp_e_ptr->alg.xid_rfc2507.nsapis   = t2->alg.xid_rfc2507.nsapis;
      pcomp_e_ptr->alg.xid_rfc2507.max_hdr  = t2->alg.xid_rfc2507.max_hdr;
      pcomp_e_ptr->alg.xid_rfc2507.max_per  = t2->alg.xid_rfc2507.max_per;
      pcomp_e_ptr->alg.xid_rfc2507.max_time = t2->alg.xid_rfc2507.max_time;
      pcomp_e_ptr->alg.xid_rfc2507.ntcp_sp  = t2->alg.xid_rfc2507.ntcp_sp;
      pcomp_e_ptr->alg.xid_rfc2507.tcp_sp   = t2->alg.xid_rfc2507.tcp_sp;

    }
    else if (t2->hdr.alg_type == T2_ALG_RFC3095)
    {

      /* Memory for RFC3095 compression instance is now dynamically allocated
         in gsn_rohc_comp_init() */

      pcomp_e_ptr->t2_alg = T2_ALG_RFC3095;
      pcomp_e_ptr->alg.xid_rfc3095.pcomp1   = t2->alg.xid_rfc3095.pcomp1;
      pcomp_e_ptr->alg.xid_rfc3095.pcomp2   = t2->alg.xid_rfc3095.pcomp2;

      pcomp_e_ptr->alg.xid_rfc3095.nsapis   = t2->alg.xid_rfc3095.nsapis;
      pcomp_e_ptr->alg.xid_rfc3095.max_cid  = t2->alg.xid_rfc3095.max_cid;
      pcomp_e_ptr->alg.xid_rfc3095.max_hdr  = t2->alg.xid_rfc3095.max_hdr;
      pcomp_e_ptr->alg.xid_rfc3095.profile1 = t2->alg.xid_rfc3095.profile1;
      pcomp_e_ptr->alg.xid_rfc3095.profile2 = t2->alg.xid_rfc3095.profile2;
      pcomp_e_ptr->alg.xid_rfc3095.profile3 = t2->alg.xid_rfc3095.profile3;
      pcomp_e_ptr->alg.xid_rfc3095.profile4 = t2->alg.xid_rfc3095.profile4;
    }
    else
    {
      MSG_GERAN_ERROR_2("GSN ""XID %d: Inval Comp Algorithm %d",
                    gsn_extern_sapi[sapi_ptr->sapi],t2->hdr.alg_type);

      GSN_MEM_FREE( pcomp_e_ptr );
      return( NULL );
    }

    /* Link entity with the Data compression algorithm
    */

    /* Allocate memory for the item to be queued */
    q_item = GPRS_MEM_MALLOC( sizeof(t2_alg_q_t) );

    GSN_ASSERT( q_item != NULL ); /* check for valid pointer */

    q_item->pcomp_e_ptr = pcomp_e_ptr;

    /* First initialize the queue link. This is always done
    ** before putting an item in a queue.
    */
    (void)q_link(q_item, &(q_item->link));

    /* Put item to the queue (list) */
    q_put( &(sapi_ptr->t2_alg_q[t2->hdr.alg_type]), &(q_item->link) );

  }
  else
  {
    MSG_GERAN_ERROR_2("GSN ""XID %d: Inval Entity %d",
                    gsn_extern_sapi[sapi_ptr->sapi],ent_num);
    return( NULL );
  }

  return( pcomp_e_ptr );
}
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
/*===========================================================================
===
===  FUNCTION      GSN_CREATE_PCOMP_VAL()
===
===  DESCRIPTION
===
===   Creates the PCOMP values to be used by the algorithm
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    Number of PCOMP values created.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
int8 gsn_create_pcomp_val
(
  uint8 *pcomp_val,             /* PCOMP values to be created */
  uint8 alg,                    /* Algorithm the PCOMP value gets assigned to */
  gsn_sapi_e_t *sapi_ptr        /* SAPI in use */
)
{
  uint8   i, pcomp_max;
  int8    pcomp_val_cnt = 0;

  if( alg == T2_ALG_RFC2507 )
  {
    pcomp_max = RFC2507_PCOMP_MAX;
  }
  else if( alg == T2_ALG_RFC1144)
  {
    pcomp_max = RFC1144_PCOMP_MAX;
  }
  else if( alg == T2_ALG_RFC3095)
  {
    pcomp_max = RFC3095_PCOMP_MAX;
  }
  else
  {
    MSG_GERAN_HIGH_0("GSN ""XID Invalid algorithm");
    return( -1 );
  }

  /* PCOMP 0 is reserved for no compression */
  for( i = 1; i <= pcomp_max; i++ )
  {
    /* Get next available PCOMP value from the pool */
    pcomp_val[i] = gsn_next_unassigned_pcomp_val( sapi_ptr );

    /* Increment PCOMP value assigned counter */
    pcomp_val_cnt++;

    /* Ensure that PCOMP value is valid. PCOMP 15 is reserved for future use */
    if( (pcomp_val[i] < PCOMP_VAL_MAX )   &&
        (sapi_ptr->pcomp_pool[pcomp_val[i]] == NULL) )
    {
      sapi_ptr->pcomp_pool[ pcomp_val[i] ] =
            GPRS_MEM_MALLOC( sizeof(pcomp_t) );
      GSN_ASSERT( sapi_ptr->pcomp_pool[ pcomp_val[i] ] != NULL );

      sapi_ptr->pcomp_pool[ pcomp_val[i] ]->alg     = alg;
      sapi_ptr->pcomp_pool[ pcomp_val[i] ]->state   = UNASSIGNED;
      sapi_ptr->pcomp_pool[ pcomp_val[i] ]->val_num = pcomp_val[i];
    }
    else
    {
      MSG_GERAN_HIGH_2("GSN ""XID %d: Inval PCOMP val %d",
                      gsn_extern_sapi[sapi_ptr->sapi],pcomp_val[i]);
      return( -1 );
    }
  }
  return( pcomp_val_cnt );
}

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
/*===========================================================================
===
===  FUNCTION      GSN_DEL_ALL_PCOMP_E()
===
===  DESCRIPTION
===
===   Deletes all PCI compession entities on the SAPI
===
===  DEPENDENCIES
===
===   None
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
void gsn_del_all_pcomp_e( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr )
{
  uint8       t2_alg;

  for( t2_alg = 0; t2_alg <= T2_ALG_MAX; t2_alg++ )
  {
    if( q_cnt( &sapi_ptr->t2_alg_q[t2_alg] ) )
      gsn_del_pcomp_e_mapped_to_alg( gas_id, sapi_ptr, t2_alg );
  }
}
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */


#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
/*===========================================================================
===
===  FUNCTION      GSN_DEL_PCOMP_E_MAPPED_TO_ALG()
===
===  DESCRIPTION
===
===   Deletes all SAPI's PCI compession entities that
===   use the T2 algorithm.
===
===  DEPENDENCIES
===
===   None
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
void gsn_del_pcomp_e_mapped_to_alg( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr, uint8 t2_alg )
{
  t2_alg_q_t        *q_item = NULL;
  t2_alg_q_t        *q_next_item;
  gsn_nsapi_q_t     *nsapi_q_item;

  if( sapi_ptr->is_nsapi_ptr_q_valid )
  {

    if( q_cnt( &sapi_ptr->t2_alg_q[t2_alg] ) )
    {
      /* One or more comp entities use the comp algorithm.
      ** Remove entities from the list
      */
      q_item = q_get( &sapi_ptr->t2_alg_q[t2_alg] );
      while( q_item != NULL )
      {
        /* Inform App NSAPIs to stop using the entity */
        nsapi_q_item = q_check( &sapi_ptr->nsapi_ptr_q );
        while( nsapi_q_item != NULL )
        {
          if( (q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC1144) &&
              ( t2_alg == T2_ALG_RFC1144 ) )
          {
            if( (q_item->pcomp_e_ptr->alg.xid_rfc1144.nsapis) &
                (0x01 << GSN_EXTERN_NSAPI(nsapi_q_item->nsapi_ptr->nsapi)) )
            {
              /* Delink NSAPI from the comp entity */
              actv_nsapis[ gas_id ][nsapi_q_item->nsapi_ptr->nsapi]->
                          pcomp_e_num[T2_ALG_RFC1144] = NO_COMP_ENT;
            }
          }
          else if( (q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC2507) &&
                   ( t2_alg == T2_ALG_RFC2507 ) )
          {
            if( (q_item->pcomp_e_ptr->alg.xid_rfc2507.nsapis) &
                (0x01 << GSN_EXTERN_NSAPI(nsapi_q_item->nsapi_ptr->nsapi)) )
            {
              /* Delink NSAPI from the comp entity */
              actv_nsapis[ gas_id ][nsapi_q_item->nsapi_ptr->nsapi]->
                            pcomp_e_num[T2_ALG_RFC2507] = NO_COMP_ENT;
            }
          }
          else if( (q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC3095) &&
                   ( t2_alg == T2_ALG_RFC3095 ) )
          {
            if( (q_item->pcomp_e_ptr->alg.xid_rfc3095.nsapis) &
                (0x01 << GSN_EXTERN_NSAPI(nsapi_q_item->nsapi_ptr->nsapi)) )
            {
              /* Delink NSAPI from the comp entity */
              actv_nsapis[ gas_id ][nsapi_q_item->nsapi_ptr->nsapi]->
                            pcomp_e_num[T2_ALG_RFC3095] = NO_COMP_ENT;
            }
          }
          else
            MSG_GERAN_ERROR_1("GSN ""Invalid algorithm type %d",
                          q_item->pcomp_e_ptr->t2_alg);

          /* Get next nsapi  */
          nsapi_q_item = q_next( &sapi_ptr->nsapi_ptr_q, &nsapi_q_item->link );
        }

        if( !q_cnt( &sapi_ptr->t2_alg_q[t2_alg] ) )
        {
          /* No comp entities use the algorithm. Unassign PCOMP values */
          if( t2_alg == T2_ALG_RFC1144 )
          {
            if ( q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp1  &&
                 sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp1] )
            {
              GSN_MEM_FREE(
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp1] );
            }
            if ( q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp2  &&
                 sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp2] )
            {
              GSN_MEM_FREE(
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp2] );
            }
          }
          else if( t2_alg == T2_ALG_RFC2507 )
          {
            GSN_MEM_FREE(
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp1] );
            GSN_MEM_FREE(
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp2] );
            GSN_MEM_FREE(
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp3] );
            GSN_MEM_FREE(
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp4] );
            GSN_MEM_FREE(
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp5] );
          }
          else if (t2_alg == T2_ALG_RFC3095 )
          {
            GSN_MEM_FREE(
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp1]);
            GSN_MEM_FREE(
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp2]);
          }
          else
          {
            MSG_GERAN_ERROR_1("GSN ""Invalid algorithm type %d",t2_alg);
          }
        }

        /* Free memory allocated for slhc */
        if( t2_alg == T2_ALG_RFC1144 )
        {
          gsn_slhc_free_mem(gas_id, (slhc_handle_type *)q_item->pcomp_e_ptr->comp_e );
        }
        else if( t2_alg == T2_ALG_RFC2507 )
        {
          gsn_iphc_free(gas_id, q_item->pcomp_e_ptr->comp_e);
        }
        else if( t2_alg == T2_ALG_RFC3095 )
        {
          gsn_rohc_free(gas_id, q_item->pcomp_e_ptr->comp_e);
        }
        else
        {
          MSG_GERAN_ERROR_1("GSN ""Invalid t2 algorithm type %d",t2_alg);
        }

        /* Put entity back to the pool of unassigned entities */
        GSN_MEM_FREE( sapi_ptr->pcomp_e_pool[q_item->pcomp_e_ptr->ent_num] );

        /* Get next item before freeing item's memory */
        q_next_item = q_next( &sapi_ptr->t2_alg_q[t2_alg], &q_item->link );

        GSN_MEM_FREE( q_item );

        /* Get next item */
        q_item = q_next_item;
      }
    }
    else
    {
      /* At least one item should be in the queue */
      MSG_GERAN_ERROR_1("GSN ""XID %d: Resp inval T2 alg queue",
                  gsn_extern_sapi[sapi_ptr->sapi]);
    }
  }
  else
  {
    MSG_GERAN_ERROR_1("GSN ""XID %d: NSAPI ptr queue invalid",
                gsn_extern_sapi[sapi_ptr->sapi]);
  }
}
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

/*===========================================================================
===
===  FUNCTION      GSN_GET_APPL_NSAPIS()
===
===  DESCRIPTION
===
===   Returns the mapped ACK NSAPIs on the SAPI in an array
===   GSN_NSAPI_MAX indicates end of the array
===
===  DEPENDENCIES
===
===   None
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
void gsn_get_app_nsapis( gsn_nsapi_t *nsapis, uint16 appl_nsapis )
{

  uint8 i;

  for( i = 5; i < 16; i++ )
  {
    if( appl_nsapis & (0x01 << i) )
      *nsapis++ = (gsn_nsapi_t) (i-5);
  }
  *nsapis = (gsn_nsapi_t)GSN_NSAPI_MAX;
}

/*===========================================================================
===
===  FUNCTION      GSN_GET_ACK_NSAPIS_ON_SAPI()
===
===  DESCRIPTION
===
===   Returns the mapped ACK NSAPIs on the SAPI in a bit map format ( NSAPI15 MSB )
===
===  DEPENDENCIES
===
===   None
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
uint16 gsn_get_ack_nsapis_on_sapi( gsn_sapi_e_t *sapi_ptr )
{
  gsn_nsapi_q_t     *item_ptr;
  uint16            nsapis = 0x0;

  if ( sapi_ptr->is_nsapi_ptr_q_valid )
  {

    item_ptr = q_check( &sapi_ptr->nsapi_ptr_q );
    while( item_ptr != NULL )
    {
      if( item_ptr->nsapi_ptr->op_mode == ACK )
        nsapis |=
          (uint16) (0x01 << ( GSN_EXTERN_NSAPI( item_ptr->nsapi_ptr->nsapi ) ));

      /* Get next NSAPI  */
      item_ptr = q_next( &(sapi_ptr->nsapi_ptr_q), &(item_ptr->link) );
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("GSN ""NSAPI ptr queue invalid");
  }

  return( nsapis );
}

/*===========================================================================
===
===  FUNCTION      GSN_GET_UNACK_NSAPIS_ON_SAPI()
===
===  DESCRIPTION
===
===   Returns the mapped UNACK NSAPIs on the SAPI in a bit map format ( NSAPI15 MSB )
===
===  DEPENDENCIES
===
===   None
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
uint16 gsn_get_unack_nsapis_on_sapi( gsn_sapi_e_t *sapi_ptr )
{
  gsn_nsapi_q_t     *item_ptr;
  uint16            nsapis = 0x0;

  if ( sapi_ptr->is_nsapi_ptr_q_valid )
  {
    item_ptr = q_check( &sapi_ptr->nsapi_ptr_q );
    while( item_ptr != NULL )
    {
      if( item_ptr->nsapi_ptr->op_mode == UNACK )
        nsapis |=
          (uint16) (0x01 << ( GSN_EXTERN_NSAPI( item_ptr->nsapi_ptr->nsapi ) ));

      /* Get next NSAPI  */
      item_ptr = q_next( &(sapi_ptr->nsapi_ptr_q), &(item_ptr->link) );
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("GSN ""NSAPI ptr queue invalid");
  }

  return( nsapis );
}

/*===========================================================================
===
===  FUNCTION      GSN_GET_NSAPIS_ON_SAPI()
===
===  DESCRIPTION
===
===   Returns the NSAPI's numbers mapped on the SAPI in an array.
===
===  DEPENDENCIES
===
===   None
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
void gsn_get_nsapis_on_sapi
(
  gas_id_t      gas_id,
  gsn_sapi_e_t *sapi_ptr,  /* SAPI in use */
  /* Points to the array to be field in with the NSAPI's numbers mapped on
  ** the SAPI
  */
  uint8        *nsapis_on_sapi
)
{
  gsn_nsapi_q_t     *item_ptr;

  GSN_ASSERT( sapi_ptr != NULL );

  if ( sapi_ptr->is_nsapi_ptr_q_valid )
  {
    item_ptr = q_check( &sapi_ptr->nsapi_ptr_q );

    while( item_ptr != NULL )
    {
      *nsapis_on_sapi = (uint8) item_ptr->nsapi_ptr->nsapi;

      nsapis_on_sapi++;

      /* Get next NSAPI  */
      item_ptr = q_next( &(sapi_ptr->nsapi_ptr_q), &(item_ptr->link) );
    }
  }
  else
  {
    MSG_GERAN_ERROR_0("GSN ""NSAPI ptr queue invalid");
  }
  /* Indicates the end of the NSAPIs list */
  *nsapis_on_sapi = GSN_INVAL_NSAPI;
}


/*===========================================================================
===
===  FUNCTION      GSN_DECODE_PEER_XID()
===
===  DESCRIPTION
===
===   Decodes  XID block sent by SGSN SNDCP.
===
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    TRUE if the XID block was decoded succesfully
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

boolean gsn_decode_peer_xid
(
  gas_id_t            gas_id,
  dsm_item_type       **peer_xid, /* points to XID requested by SGSN */
  gsn_dec_xid_t       *dec_blk,   /* decoded XID parameters */
  gsn_sapi_e_t        *sapi_ptr,  /* pointer to sapi */
  boolean             neg_xid     /* TRUE if peer XID is an XID responce, otherwise FALSE */
)
{
  uint16              msg_pos = 0;
  uint8               src[XID_BLOCK_LEN_MAX];
  uint16              xid_blck_len = 0;
  uint8               dec_xid_type[XID_TYPE_MAX + 1] = {0xff,0xff,0xff};
  gsn_xid_block_t     dxid_tmp;

  if( !*peer_xid || !dec_blk )
    return( FALSE );

  /* Get XID pdu length */
  xid_blck_len = GPRS_PDU_LENGTH( *peer_xid );

  /* Check if greater than 255 bytes */
  if( xid_blck_len <= XID_BLOCK_LEN_MAX )
  {
    /* Strip XID pdu to a buffer. The DSM item that conveys the pdu is released */
    if( !gprs_pdu_strip_head( peer_xid, src, xid_blck_len) )
    {
      MSG_GERAN_ERROR_0("GSN ""XID strip to buff failed");
      return ( FALSE );
    }
  }
  else
  {
    /* Invalid length of XID block */
    MSG_GERAN_ERROR_1("GSN ""XID block len %d > XID_BLOCK_LEN_MAX",
                    xid_blck_len);

    return ( FALSE );
  }

  while( xid_blck_len > (msg_pos / XID_BYTE_SIZE) )
  {
    dxid_tmp.type    = gunpackb(src, &msg_pos, GSIZE(gsn_xid_block_t, type));
    dxid_tmp.length  = gunpackb(src, &msg_pos, GSIZE(gsn_xid_block_t, length));

    MSG_GERAN_HIGH_1("GSN "" xid blk:    type       = %d",dxid_tmp.type);
    MSG_GERAN_HIGH_1("GSN "" xid length: length     = %d",dxid_tmp.length);

    /* Check for duplicated instances of XID type */
    if( dxid_tmp.type <= XID_TYPE_MAX )
    {
      if( dec_xid_type[dxid_tmp.type] == dxid_tmp.type )
      {
        /* XID has been decoded. Advance position of the
        ** read pointer of the XID block
        ** by the length of the parameter's field.
        */
        msg_pos += (dxid_tmp.length * XID_BYTE_SIZE);

        break;
      }
    }

    if( dxid_tmp.length > xid_blck_len )
    {
      /* XID Type length greater than XID block */
      MSG_GERAN_ERROR_2("GSN ""XID type len %d > XID block len %d",
                    dxid_tmp.length,xid_blck_len);

      return( FALSE );
    }

    switch( dxid_tmp.type )
    {
    case XID_TYPE_0:

      /* Record that the type T0 has been decoded in this XID block.
      ** This is used to avoid decoding duplicated parameters.
      */
      dec_xid_type[XID_TYPE_0] = XID_TYPE_0;

      /* Length of the parameter is 1 */
      if( dxid_tmp.length > GSN_VER_NUM_LEN )
      {
        MSG_GERAN_LOW_1("GSN ""XID T0 invalid len %d",dxid_tmp.length);

        dec_blk->t0.status = XID_INVALID;

        /* Advance position of the read pointer in the XID block
        ** by the length of the parameter's field
        */
        msg_pos += (dxid_tmp.length * XID_BYTE_SIZE);

        break;
      }
      else if( !dxid_tmp.length )
      {
        dec_blk->t0.status = XID_ABSENT;
        MSG_GERAN_LOW_0("GSN ""XID T0 len is 0");
        break;
      }

      dec_blk->t0.status = XID_VALID;
      dec_blk->t0.ver_num = gunpackb(src, &msg_pos, GSIZE(gsn_xid_t0_t, ver));

      break;

    case XID_TYPE_1:
      {
        uint16          ent_end_pos;
        uint16          t1_end_pos;
        uint8           ent_num;
        gsn_xid_t1_t    *ent_xid;
        uint8           num_of_ent  = 0;
        uint8           ent_in_blck = 0;
        uint8           dec_dcomp_ent[ENT_NUM_MAX + 1];

        memset( dec_dcomp_ent, 0xff, (ENT_NUM_MAX + 1) );

        /* Record that the type T1 has been decoded in this XID block.
        ** This is used to avoid decoding duplicated parameters.
        */
        dec_xid_type[XID_TYPE_1] = XID_TYPE_1;

        /* Decode (TS 04.65 Fig10) Type 1
        ** compression entities in the XID block.
        */
        if( dxid_tmp.length )
        {
          /* mark the end of T1 parameter's body */
          t1_end_pos  = (dxid_tmp.length * XID_BYTE_SIZE) + msg_pos;

          while( (t1_end_pos > msg_pos ) && (num_of_ent < ENT_NUM_MAX))
          {
            /* Read the 1st Octet from the header. Includes the
            ** Propose bit (P), Spare bit (X) and Entity number.
            */
            dxid_tmp.type_field.xid_t1.hdr.p =
                gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, p));
            dxid_tmp.type_field.xid_t1.hdr.x1=
                gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, x1));
            dxid_tmp.type_field.xid_t1.hdr.ent_num =
                gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, ent_num));

            /* To avoid long lines of code */
            ent_num = dxid_tmp.type_field.xid_t1.hdr.ent_num;

            /* if entity number is invalid. Then return FALSE. */
            if( ent_num > ENT_NUM_MAX )
            {
              return( FALSE );
            }

            /* Check for duplicated instances of entity in the T1 block */
            if( dec_dcomp_ent[ent_num] == ent_num )
            {
              if( neg_xid )
              {
                /* Negotiated XID. Parameter with duplicated instances.
                ** Re-initiate the XID negotiation.
                */
                return( FALSE );
              }
              else
              {
                /* Entity has been decoded. Read length of parameters body first
                ** and advance position of the read pointer to the next entity.
                */
                if( dxid_tmp.type_field.xid_t1.hdr.p )
                {
                  dxid_tmp.type_field.xid_t1.hdr.x2 =
                    gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, x2));
                  dxid_tmp.type_field.xid_t1.hdr.alg_type =
                    gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, alg_type));
                }

                dxid_tmp.type_field.xid_t1.hdr.length =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, length));

                msg_pos += (dxid_tmp.type_field.xid_t1.hdr.length * XID_BYTE_SIZE);
              }

              continue;
            }
            else
              /* Record that the entity has been decoded. */
              dec_dcomp_ent[ent_num] = ent_num;

            /* Remeber which entity in the T1 block we are reading and increment
            ** number of entities counter for the next loop
            */
            dec_blk->t1.ent_map |= 0x01 << ent_num;
            ent_in_blck = num_of_ent++;

            /* Set entity's xid parameters to invalid values */
            memset( &dec_blk->t1.ent[ent_in_blck].xid, 0xff, sizeof( gsn_xid_t1_t ) );

            /* Valid T1 block and compression entity */
            dec_blk->t1.status = XID_VALID;
            dec_blk->t1.ent[ent_in_blck].status = XID_VALID;
            dec_blk->t1.ent_in_blk = ent_in_blck + 1;

            /* To avoid long lines of code */
            ent_xid = &dec_blk->t1.ent[ent_in_blck].xid;

            /* Store parameters decoded so far */
            ent_xid->hdr.p  = dxid_tmp.type_field.xid_t1.hdr.p;
            ent_xid->hdr.x1 = dxid_tmp.type_field.xid_t1.hdr.x1;
            ent_xid->hdr.ent_num = dxid_tmp.type_field.xid_t1.hdr.ent_num;

            if( ent_xid->hdr.p )
            {
              /* If P bit set, then read the 2nd Octet from the header.
              ** Includes: Spare bits (X) and the Algorithm type.
              */
               ent_xid->hdr.x2 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, x2));
               ent_xid->hdr.alg_type =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, alg_type));
            }

            /* Read the 3rd octet from the header. Includes only the length */
            ent_xid->hdr.length =
                gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, length));

            /* Mark the end of the entity's parameters */
            ent_end_pos = msg_pos  +
                  (ent_xid->hdr.length * XID_BYTE_SIZE);

            /* Check if comp entity's field is within the T1 block */
            if( (ent_xid->hdr.length > V42BIS_LEN_P1_MAX) ||
                (ent_end_pos > t1_end_pos ) )
            {
              /* We do not know which one of the two length values is
              ** erroneous. Mark T1 as invalid and read next type.
              */
              MSG_GERAN_HIGH_2("GSN ""Inval T1 Param len %d, T1 blck %d",
                ent_xid->hdr.length,t1_end_pos);

              dec_blk->t1.ent[ent_in_blck].status = XID_INVALID;

              msg_pos = t1_end_pos;

              break;
            }

            /* Check for Invalid algorithm type */
            if( ent_xid->hdr.p && (ent_xid->hdr.alg_type > T1_ALG_MAX) )
            {
              /* Unsupported algorithm */
              MSG_GERAN_HIGH_1("GSN ""T1 unsupported alg %d",
                ent_xid->hdr.alg_type);

              dec_blk->t1.ent[ent_in_blck].status = XID_INVALID;

              /* Advance position of the read pointer to the next entity */
              msg_pos = ent_end_pos;

              continue;
            }

            /* Note. Only V42 bis is supported in the current implementation. If more
            ** that one T1 algorithms are supported then the compression parameters
            ** must be decoded according to the algorithm type.
            */
            if( ( ent_xid->hdr.p    &&
                ( ent_xid->hdr.length == V42BIS_DCOMP_LEN)) ||
                ( !ent_xid->hdr.p   &&
                ( ent_xid->hdr.length == 0)) )
            {
              /* App NSAPIs is not included in the entity's field. The default value is 0.
              */
              ent_xid->alg.xid_v42bis.nsapis = 0;

              /* Advance position of the read pointer to the next entity */
              msg_pos = ent_end_pos;

              dec_blk->t1.ent[ent_in_blck].status = XID_ABSENT;

              continue;
            }

            /* If P bit set read the 4nd Octet.It Includes DCOMP1 and DCOMP2 values
            */
            if( ent_xid->hdr.p && ent_xid->hdr.length )
            {
              ent_xid->alg.xid_v42bis.dcomp1 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_dcomp_v42bis_t, dcomp1));

              /* Ignore value of DCOMP2. Is not used by V42bis algorithm */
              ent_xid->alg.xid_v42bis.dcomp2 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_dcomp_v42bis_t, dcomp2));
            }

            /* Read the parameters included in the compression field,
            ** according to the length of the comp entity parameters
            ** and the P bit. See TS04.65 table 7(2nd table).*/
            /* -----------------------------------------------------------------*/
            if( ( ent_xid->hdr.p  &&
                ( ent_xid->hdr.length >= 3)) ||
                ( !ent_xid->hdr.p &&
                ( ent_xid->hdr.length >= 2)) )
            {
              /* Read Applicable NSAPIs */
              ent_xid->alg.xid_v42bis.nsapis = APP_NSAPIS_MASK &
                  gunpackw(src, &msg_pos, GSIZE(gsn_xid_dcomp_v42bis_t, nsapis));
            }

            if( ( ent_xid->hdr.p  &&
                ( ent_xid->hdr.length >= 4)) ||
                ( !ent_xid->hdr.p &&
                ( ent_xid->hdr.length >= 3)) )
            {
              /* Read P0 parameter */
              ent_xid->alg.xid_v42bis.p0 = V42BIS_P0_MASK &
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_dcomp_v42bis_t, p0));
            }

            if( ( ent_xid->hdr.p  &&
                ( ent_xid->hdr.length >= 6)) ||
                ( !ent_xid->hdr.p &&
                ( ent_xid->hdr.length >= 5)) )
            {
              /* Read P1 parameter */
              ent_xid->alg.xid_v42bis.p1 =
                  gunpackw(src, &msg_pos, GSIZE(gsn_xid_dcomp_v42bis_t, p1));
            }

            if( ( ent_xid->hdr.p  &&
                ( ent_xid->hdr.length >= 7)) ||
                ( !ent_xid->hdr.p &&
                ( ent_xid->hdr.length >= 6)) )
            {
              /* Read P2 parameter */
              ent_xid->alg.xid_v42bis.p2 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_dcomp_v42bis_t, p2));
            }

            if( ( ent_xid->hdr.p  &&
                ( ent_xid->hdr.length > 7)) ||
                ( !ent_xid->hdr.p &&
                ( ent_xid->hdr.length > 6)) )
            {
              MSG_GERAN_HIGH_1("GSN ""Inval T1 field len %d",
               ent_xid->hdr.length);

               /* Advance position of the read pointer to the next entity */
              msg_pos = ent_end_pos;

              continue;
            }
            /* ------------- End of comp entity's parameters --------- */

          } /* while ( t1_end_pos > msg_pos ) */
        }
        else
        {
          /* Body of T1 compression field is not included */
          MSG_GERAN_MED_0("GSN "" XID T1 body not included");

          dec_blk->t1.status = XID_ABSENT;
        }
      }  /*  case XID_TYPE_1: */

      break;

    case XID_TYPE_2:
      {
        uint16          ent_end_pos;
        uint16          t2_end_pos;
        uint8           ent_num;
        gsn_xid_t2_t    *ent_xid;
        uint8           ent_in_blck = 0;
        uint8           num_of_ent  = 0;
        uint8           dec_pcomp_ent[ENT_NUM_MAX + 1];
        uint8           alg_type = T2_ALG_NONE;

        memset( dec_pcomp_ent, 0xff, (ENT_NUM_MAX + 1) );

        /* Record that the type T2 has been decoded in this XID block.
        ** This is used to avoid decoding duplicated parameters.
        */
        dec_xid_type[XID_TYPE_2] = XID_TYPE_2;

        /* Decode (TS 04.65 Fig10) Type 2
        ** compression entities in the XID block.
        */
        if( dxid_tmp.length )
        {
          /* mark the end of T2 parameter's body */
          t2_end_pos  = (dxid_tmp.length * XID_BYTE_SIZE) + msg_pos;

          while(( t2_end_pos > msg_pos ) && ( num_of_ent < ENT_NUM_MAX ))
          {
            /* Read the 1st Octet from the header. Includes the
            ** Propose bit (P), Spare bit (X) and Entity number.
            */
            dxid_tmp.type_field.xid_t2.hdr.p =
                gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, p));
            dxid_tmp.type_field.xid_t2.hdr.x1=
                gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, x1));
            dxid_tmp.type_field.xid_t2.hdr.ent_num =
                gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, ent_num));

            /* To avoid long lines of code */
            ent_num = dxid_tmp.type_field.xid_t2.hdr.ent_num;

            /* if entity number is invalid. Then return FALSE. */
            if( ent_num > ENT_NUM_MAX )
            {
              return( FALSE );
            }

            /* Check for duplicated instances of entity in the T2 block */
            if( dec_pcomp_ent[ent_num] == ent_num )
            {
              if( neg_xid )
              {
                /* Negotiated XID. Parameter with duplicated instances.
                ** Re-initiate the XID negotiation.
                */
                return( FALSE );
              }
              else
              {
                /* Entity has been decoded. Read length of parameters body first
                ** and advance position of the read pointer to the next entity.
                */
                if( dxid_tmp.type_field.xid_t2.hdr.p )
                {
                  dxid_tmp.type_field.xid_t2.hdr.x2 =
                    gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, x2));
                  dxid_tmp.type_field.xid_t2.hdr.alg_type =
                    gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, alg_type));
                }

                dxid_tmp.type_field.xid_t2.hdr.length =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, length));

                msg_pos += (dxid_tmp.type_field.xid_t2.hdr.length * XID_BYTE_SIZE);
              }

              continue;
            }
            else
              /* Record that the entity has been decoded. */
              dec_pcomp_ent[ent_num] = ent_num;

            /* Remeber which entity in the T2 block we are reading and increment
            ** number of entities counter for the next loop
            */
            dec_blk->t2.ent_map |= 0x01 << ent_num;
            ent_in_blck = num_of_ent++;

            /* Set XID parameters to invalid values */
            memset( &dec_blk->t2.ent[ent_in_blck].xid, 0xff, sizeof( gsn_xid_t2_t ) );

            /* Valid T2 block and compression entity */
            dec_blk->t2.status = XID_VALID;
            dec_blk->t2.ent[ent_in_blck].status = XID_VALID;
            dec_blk->t2.ent_in_blk = ent_in_blck + 1;

            /* To avoid long lines of code */
            ent_xid = &dec_blk->t2.ent[ent_in_blck].xid;

            /* Store parameters decoded so far */
            ent_xid->hdr.p  = dxid_tmp.type_field.xid_t2.hdr.p;
            ent_xid->hdr.x1 = dxid_tmp.type_field.xid_t2.hdr.x1;
            ent_xid->hdr.ent_num = dxid_tmp.type_field.xid_t2.hdr.ent_num;

            MSG_GERAN_HIGH_1("GSN "" xid hdr: p-bit      = %d",ent_xid->hdr.p);
            MSG_GERAN_HIGH_1("GSN "" xid hdr: x1-bit     = %d",ent_xid->hdr.x1);
            MSG_GERAN_HIGH_1("GSN "" xid hdr: entity num = %d",ent_xid->hdr.ent_num);

            if( ent_xid->hdr.p )
            {
              /* If P bit set, then read the 2nd Octet from the header.
              ** Includes: Spare bits (X) and the Algorithm type.
              */
               ent_xid->hdr.x2 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, x2));
               ent_xid->hdr.alg_type =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, alg_type));
               alg_type = ent_xid->hdr.alg_type;

               MSG_GERAN_HIGH_1("GSN "" xid hdr: x2-bit   = %d",ent_xid->hdr.x2);
               MSG_GERAN_HIGH_1("GSN "" xid hdr: alg type = %d",ent_xid->hdr.alg_type);
            }
            else if ((!ent_xid->hdr.p) && (sapi_ptr->pcomp_e_pool[ent_num] != NULL))
            {
              /* P bit is not set if it is a response. In this case, retrieve
                 algorithm type from pcomp entity. */
              alg_type = sapi_ptr->pcomp_e_pool[ent_num]->t2_alg;
              ent_xid->hdr.alg_type = alg_type;

              MSG_GERAN_HIGH_1("GSN "" xid hdr: alg type = %d",ent_xid->hdr.alg_type);
            }

            /* Read the 3rd octet from the header. Includes only the length */
            ent_xid->hdr.length =
                gunpackb(src, &msg_pos, GSIZE(gsn_xid_comp_hdr_t, length));

            MSG_GERAN_HIGH_1("GSN "" xid hdr: length       = %d",ent_xid->hdr.length);

            /* Mark the end of the entity's parameters */
            ent_end_pos = msg_pos  +
                  (ent_xid->hdr.length * XID_BYTE_SIZE);

            /* Check if comp entity's field is within the T2 block */
            if( ent_end_pos > t2_end_pos )
            {
              /* We do not know which one of the two length values is
              ** erroneous. Mark entity as invalid and read next type.
              */
              MSG_GERAN_HIGH_2("GSN ""Inval T2 Param len %d, T2 blck %d",
                ent_xid->hdr.length,t2_end_pos);

              dec_blk->t2.ent[ent_in_blck].status = XID_INVALID;

              msg_pos = t2_end_pos;

              break;
            }
#if 0
            if(alg_type == T2_ALG_RFC3095)
            {
              /* Disable ROHC for now */

              /* Advance position of the read pointer to the next entity*/
              msg_pos = ent_end_pos;

              dec_blk->t2.ent[ent_in_blck].status = XID_INVALID;
              MSG_GERAN_HIGH_0("GSN ""ROHC disabled");
              continue;
            }
#endif
            switch(alg_type)
            {
              case T2_ALG_RFC1144:
              {
                if(( ent_xid->hdr.p    &&
                   ( ent_xid->hdr.length == RFC1144_PCOMP_LEN)) ||
                   ( !ent_xid->hdr.p   &&
                   ( ent_xid->hdr.length == 0)) )
                {
                  /* App NSAPIs is not included in the entity's field. The
                     default value is 0. */
                  ent_xid->alg.xid_rfc1144.nsapis = 0;

                  /* Advance position of the read pointer to the next entity */
                  msg_pos = ent_end_pos;

                  dec_blk->t2.ent[ent_in_blck].status = XID_ABSENT;

                  continue;
                }

                if(( ent_xid->hdr.p    &&
                   ( ent_xid->hdr.length > 4)) ||
                   ( !ent_xid->hdr.p   &&
                   ( ent_xid->hdr.length > 3)))
                {
                  MSG_GERAN_HIGH_1("GSN ""Inval T2 len %d", ent_xid->hdr.length);

                  /* Advance position of the read pointer to the next entity */
                  msg_pos = ent_end_pos;

                  continue;
                }


                /* If P bit set read the 4th Octet. It Includes PCOMP1 and
                   PCOMP2 values */
                if( ent_xid->hdr.p && ent_xid->hdr.length )
                {
                  ent_xid->alg.xid_rfc1144.pcomp1 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_1144_t, pcomp1));

                  ent_xid->alg.xid_rfc1144.pcomp2 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_1144_t, pcomp2));

                  if ( ( ent_xid->alg.xid_rfc1144.pcomp1 >= PCOMP_VAL_MAX ) ||
                       ( ent_xid->alg.xid_rfc1144.pcomp2 >= PCOMP_VAL_MAX )
                     )
                  {
                    MSG_GERAN_HIGH_2("GSN ""Inval PCOMP Value (RFC1144) %d, %d",
                      ent_xid->alg.xid_rfc1144.pcomp1,
                      ent_xid->alg.xid_rfc1144.pcomp2);

                    /* Advance position of the read pointer to the next entity */
                    msg_pos = ent_end_pos;

                    continue;
                  }


                }

                /* Read the parameters included in the compression field,
                 * according to the length of the comp entity parameters
                 * and the P bit. See TS04.65 table 5.*/

                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 3)) ||
                    ( !ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 2)) )
                {
                  /* Read Applicable NSAPIs */
                  ent_xid->alg.xid_rfc1144.nsapis = APP_NSAPIS_MASK &
                  gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_1144_t, nsapis));
                }

                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 4)) ||
                    ( !ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 3)) )
                {
                  /* Read S0-1 parameter */
                  ent_xid->alg.xid_rfc1144.s0_1 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_1144_t, s0_1));
                }

                break;
              }

              case T2_ALG_RFC2507:
              {
                if( ( ent_xid->hdr.p  &&
                   ( ent_xid->hdr.length == RFC2507_PCOMP_LEN)) ||
                   ( !ent_xid->hdr.p &&
                   ( ent_xid->hdr.length == 0)) )
                {
                  /* App NSAPIs is not included in the entity's field. The
                     default value is 0. */
                  ent_xid->alg.xid_rfc2507.nsapis = 0;

                  /* Advance position of the read pointer to the next entity */
                  msg_pos = ent_end_pos;

                  dec_blk->t2.ent[ent_in_blck].status = XID_ABSENT;

                  continue;
                }

                if(( ent_xid->hdr.p  &&
                   ( ent_xid->hdr.length > 12)) ||
                   (!ent_xid->hdr.p &&
                   ( ent_xid->hdr.length > 9)) )
                {
                  MSG_GERAN_HIGH_1("GSN ""Inval T2 len %d", ent_xid->hdr.length);

                  /* Advance position of the read pointer to the next entity */
                  msg_pos = ent_end_pos;

                  continue;
                }


                /* If P bit set read next three octets. It Includes PCOMP1,
                   PCOMP2,PCOMP3, PCOMP4, PCOMP5 values */
                if( ent_xid->hdr.p && (ent_xid->hdr.length >= 3))
                {
                  ent_xid->alg.xid_rfc2507.pcomp1 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, pcomp1));

                  ent_xid->alg.xid_rfc2507.pcomp2 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, pcomp2));

                  ent_xid->alg.xid_rfc2507.pcomp3 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, pcomp3));

                  ent_xid->alg.xid_rfc2507.pcomp4 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, pcomp4));

                  ent_xid->alg.xid_rfc2507.pcomp5 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, pcomp5));

                  /* PCOMP6 is unused */
                  ent_xid->alg.xid_rfc2507.pcomp6 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, pcomp6));

                  if ( ( ent_xid->alg.xid_rfc2507.pcomp1 >= PCOMP_VAL_MAX ) ||
                       ( ent_xid->alg.xid_rfc2507.pcomp2 >= PCOMP_VAL_MAX ) ||
                       ( ent_xid->alg.xid_rfc2507.pcomp3 >= PCOMP_VAL_MAX ) ||
                       ( ent_xid->alg.xid_rfc2507.pcomp4 >= PCOMP_VAL_MAX ) ||
                       ( ent_xid->alg.xid_rfc2507.pcomp5 >= PCOMP_VAL_MAX )
                     )
                  {
                    MSG_GERAN_HIGH_3("GSN ""Inval PCOMP Val (RFC2507) 1-%d, 2-%d, 3- %d",
                      ent_xid->alg.xid_rfc2507.pcomp1,
                      ent_xid->alg.xid_rfc2507.pcomp2,
                      ent_xid->alg.xid_rfc2507.pcomp3 );

                    MSG_GERAN_HIGH_2("GSN ""Inval PCOMP Val (RFC2507) 4-%d, 5-%d",
                      ent_xid->alg.xid_rfc2507.pcomp4,
                      ent_xid->alg.xid_rfc2507.pcomp5);

                    /* Advance position of the read pointer to the next entity */
                    msg_pos = ent_end_pos;

                    continue;
                  }

                }

                /* Read the parameters included in the compression field,
                 * according to the length of the comp entity parameters
                 * and the P bit. See TS04.65 table 5.*/

                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 5)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 2)) )
                {
                  /* Read Applicable NSAPIs */
                  ent_xid->alg.xid_rfc2507.nsapis = APP_NSAPIS_MASK &
                    gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, nsapis));
                }

                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 7)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 4)) )
                {
                  /* Read F_MAX_PERIOD parameter */
                  ent_xid->alg.xid_rfc2507.max_per  =
                    gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, max_per));
                }


                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 8)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 5)) )
                {
                  /* Read F_MAX_TIME parameter */
                  ent_xid->alg.xid_rfc2507.max_time  =
                    gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, max_time));
                }


                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 9)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 6)) )
                {
                  /* Read MAX_HEADER parameter */
                  ent_xid->alg.xid_rfc2507.max_hdr  =
                    gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, max_hdr));
                }


                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 10)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 7)) )
                {
                  /* Read TCP_SPACE parameter */
                  ent_xid->alg.xid_rfc2507.tcp_sp  =
                    gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, tcp_sp));
                }


                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 12)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 9)) )
                {
                  /* Read NON_TCP_SPACE parameter */
                  ent_xid->alg.xid_rfc2507.ntcp_sp  =
                    gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_2507_t, ntcp_sp));
                }

                break;
              }

              case T2_ALG_RFC3095:
              {
                if(( ent_xid->hdr.p    &&
                   ( ent_xid->hdr.length == RFC3095_PCOMP_LEN)) ||
                   ( !ent_xid->hdr.p   &&
                   ( ent_xid->hdr.length == 0)) )
                {
                  /* App NSAPIs is not included in the entity's field. The
                     default value is 0. */
                  ent_xid->alg.xid_rfc3095.nsapis = 0;

                  /* Advance position of the read pointer to the next entity*/
                  msg_pos = ent_end_pos;

                  dec_blk->t2.ent[ent_in_blck].status = XID_ABSENT;

                  continue;
                }

                if(( ent_xid->hdr.p  &&
                   ( ent_xid->hdr.length > 15)) ||
                   (!ent_xid->hdr.p &&
                   ( ent_xid->hdr.length > 14)) )
                {
                  MSG_GERAN_HIGH_1("GSN ""Inval T2 len %d", ent_xid->hdr.length);

                  /* Advance position of the read pointer to the next entity */
                  msg_pos = ent_end_pos;

                  continue;
                }

                /* If P bit set read next octet. It Includes PCOMP1,PCOMP2
                   values */
                if( ent_xid->hdr.p && ent_xid->hdr.length )
                {
                  ent_xid->alg.xid_rfc3095.pcomp1 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_3095_t, pcomp1));

                  ent_xid->alg.xid_rfc3095.pcomp2 =
                  gunpackb(src, &msg_pos, GSIZE(gsn_xid_pcomp_3095_t, pcomp2));

                  MSG_GERAN_HIGH_3("GSN "" xid : pcomp1 %d, pcomp2 %d",
                                          ent_xid->alg.xid_rfc3095.pcomp1,
                                          ent_xid->alg.xid_rfc3095.pcomp2,
                                          0);

                  if ( ( ent_xid->alg.xid_rfc3095.pcomp1 >= PCOMP_VAL_MAX ) ||
                       ( ent_xid->alg.xid_rfc3095.pcomp2 >= PCOMP_VAL_MAX )
                     )
                  {
                    MSG_GERAN_HIGH_2("GSN ""Inval PCOMP Value (RFC3095) %d, %d",
                      ent_xid->alg.xid_rfc3095.pcomp1,
                      ent_xid->alg.xid_rfc3095.pcomp2);

                    /* Advance position of the read pointer to the next entity */
                    msg_pos = ent_end_pos;

                    continue;
                  }

                }

                /* Read the parameters included in the compression field,
                 * according to the length of the comp entity parameters
                 * and the P bit. See TS04.65 table 10.*/

                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 3)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 2)) )
                {
                  /* Read Applicable NSAPIs */
                  ent_xid->alg.xid_rfc3095.nsapis = APP_NSAPIS_MASK &
                    gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_3095_t, nsapis));
                  MSG_GERAN_HIGH_1("GSN "" xid : nsapis  = %d",ent_xid->alg.xid_rfc3095.nsapis);
                }

                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 5)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 4)) )
                {
                  /* Read MAX_CID parameter */
                  ent_xid->alg.xid_rfc3095.max_cid  =
                    gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_3095_t, max_cid));
                  MSG_GERAN_HIGH_1("GSN "" xid : max_cid  = %d",ent_xid->alg.xid_rfc3095.max_cid);
                }


                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 7)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 6)) )
                {
                  /* Read MAX_HEADER parameter */
                  ent_xid->alg.xid_rfc3095.max_hdr  =
                    gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_3095_t, max_hdr));
                  MSG_GERAN_HIGH_1("GSN "" xid : max_hdr  = %d",ent_xid->alg.xid_rfc3095.max_hdr);
                }


                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 9)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 8)) )
                {
                  /* Read PROFILE1 parameter */
                  ent_xid->alg.xid_rfc3095.profile1=
                    gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_3095_t, profile1));
                  MSG_GERAN_HIGH_1("GSN "" xid : profile1  = %d",ent_xid->alg.xid_rfc3095.profile1);
                }


                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 11)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 10)) )
                {
                  /* Read PROFILE2 parameter */
                  ent_xid->alg.xid_rfc3095.profile2=
                    gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_3095_t, profile2));
                  MSG_GERAN_HIGH_1("GSN "" xid : profile2  = %d",ent_xid->alg.xid_rfc3095.profile2);
                }


                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 13)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 12)) )
                {
                  /* Read PROFILE3 parameter */
                  ent_xid->alg.xid_rfc3095.profile3  =
                    gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_3095_t, profile3));
                  MSG_GERAN_HIGH_1("GSN "" xid : profile3  = %d",ent_xid->alg.xid_rfc3095.profile3);
                }

                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 15)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 14)) )
                {
                  /* Read PROFILE4 parameter */
                  ent_xid->alg.xid_rfc3095.profile4  =
                    gunpackw(src, &msg_pos, GSIZE(gsn_xid_pcomp_3095_t, profile4));
                  MSG_GERAN_HIGH_1("GSN "" xid : profile4  = %d",ent_xid->alg.xid_rfc3095.profile4);
                }

                if( ( ent_xid->hdr.p  &&
                    ( ent_xid->hdr.length >= 17)) ||
                    (!ent_xid->hdr.p &&
                    ( ent_xid->hdr.length >= 16)) )
                {
                  /* Unexpected profile parameter */
                  MSG_GERAN_HIGH_0("Unexpected profiles");
                }

                break;
              }

              default:
              {
                /* Unsupported algorithm. Current implementation of SNDCP
                   supports RFCs 1144,2507, and 3095. */

                MSG_GERAN_HIGH_1("GSN ""Unsupported T2 alg %d",
                  ent_xid->hdr.alg_type);

                dec_blk->t2.ent[ent_in_blck].status = XID_INVALID;

                /* Advance position of the read pointer to the next entity*/
                msg_pos = ent_end_pos;

                break;
              }
            } /* End of switch on T2 alg */
          } /* while ( t2_end_pos > msg_pos ) */
        }
        else
        {
          /* Body of T2 compression field is not included */
          MSG_GERAN_MED_0("GSN "" XID T2 body not included");

          dec_blk->t2.status = XID_ABSENT;
        }
      }  /*  case XID_TYPE_2: */

      break;

    default:

      if( neg_xid )
      {
        /* Negotiated XID with unrecognised Type field */
        return( FALSE );
      }
      else
      {
        /* Requested XID. Ignore parameter. See TS 04.65/6.8.3 */
        MSG_GERAN_HIGH_1("GSN ""Invalid XID param type : %d ",dxid_tmp.type);

        /* Advance position of the read pointer in the XID block
        ** by the length of the parameter's field */
        msg_pos += (dxid_tmp.length * XID_BYTE_SIZE);
      }

      break;

    } /* switch ( dxid_tmp.type ) */

  }/* while (xid_blck_len - mes_pos ) */

  return( TRUE );
}


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/



/*===========================================================================
===
===  FUNCTION      GSN_DECODE_DATA_PDU_HDR()
===
===  DESCRIPTION
===
===   Decodes SN-DATA PDU header from external format into
===   internal format ( gsn_data_pdu_hdr_t ).
===
===   *src:       pointer to down-link LLC data block
===   *sn_data:   structure representing SNDCP SN-DATA PDU format
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    TRUE if valid SN-DATA PDU header format, otherwise FALSE
===
===  SIDE EFFECTS
===
===
===
===========================================================================*/

boolean gsn_decode_data_pdu_hdr( gas_id_t gas_id, uint8 *src, gsn_data_pdu_hdr_t *sn_data)
{

  uint16    msg_pos   = 0;
  boolean   valid_hdr = TRUE;

  /* Get SN-DATA PDU header fields */
  sn_data->x = gunpackb(src, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, x));
  sn_data->f = gunpackb(src, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, f));
  sn_data->t = gunpackb(src, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, t));

  if ( sn_data->t )
  {
    /* Invalid type. SN-PDU Type is set to SN-UNITDATA */
    valid_hdr = FALSE;
    MSG_GERAN_HIGH_0("GSN ""DL SN-UDATA rcvd while SN-DATA expected");
  }

  sn_data->m      = gunpackb(src, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, m));
  sn_data->nsapi  = gunpackb(src, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, nsapi));

  /* Check if valid NSAPI number */
  if( GSN_VALID_NSAPI( sn_data->nsapi ) )
  {
    /* Translate external value of NSAPI into SNDCP NSAPI number */
    sn_data->nsapi = (uint8) GSN_INTERN_NSAPI( sn_data->nsapi );

    /* Check if NSAPI is active and uses Ack mode  */
    if( (actv_nsapis[ gas_id ][sn_data->nsapi] == NULL) ||
        (actv_nsapis[ gas_id ][sn_data->nsapi]->op_mode != ACK) ||
        (actv_nsapis[ gas_id ][sn_data->nsapi]->sapi_ptr == NULL) )
    {
      /* Invalid NSAPI */
      valid_hdr = FALSE;

      MSG_GERAN_HIGH_1("GSN ""DL LL-DATA received at an invalid NSAPI%d",
        GSN_EXTERN_NSAPI(sn_data->nsapi));
    }
  }
  else
  {
    /* Invalid NSAPI number */
    valid_hdr = FALSE;

    MSG_GERAN_HIGH_1("GSN ""DL SN-DATA rcvd at an invalid NSAPI %d", sn_data->nsapi);
  }

  /* Check if first segment of an N-PDU */
  if(( sn_data->f ) && ( valid_hdr ))
  {
    /* First segment, get DCOMP, PCOMP and N-PDU number */
    sn_data->dcomp  = gunpackb(src, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, dcomp));
    sn_data->pcomp  = gunpackb(src, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, pcomp));
    sn_data->num    = gunpackb(src, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, num));

    /* SN-PDU with unallocated DCOMP or PCOMP shall be ignored only if NSAPI's
    ** reassembly state is " Receive First Segment".
    ** If compression is not supported  PCOMP = 0,
    ** DCOMP = 0 are the only valid values.
    */

    if( actv_nsapis[ gas_id ][sn_data->nsapi]->dle.reas_state == DL_RCV_1ST_SEQ )
    {
      if( sn_data->pcomp >= PCOMP_VAL_MAX )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN ""DL LL-DATA rcvd inval PCOMP %d ", sn_data->pcomp);
      }

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

      else if( (sn_data->pcomp ) &&
          (actv_nsapis[ gas_id ][sn_data->nsapi]->sapi_ptr->pcomp_pool[sn_data->pcomp] == NULL) )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN " "DL LL-DATA rcvd with unallocated PCOMP %d", sn_data->pcomp);
      }

#else

      else if( sn_data->pcomp )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN " "DL LL-DATA rcvd with unallocated PCOMP %d", sn_data->pcomp);
      }

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

      if( sn_data->dcomp >= DCOMP_VAL_MAX )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN ""DL LL-DATA rcvd inval DCOMP %d",
                      sn_data->dcomp);
      }

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

      else if( (sn_data->dcomp ) &&
          (actv_nsapis[ gas_id ][sn_data->nsapi]->sapi_ptr->dcomp_pool[sn_data->dcomp] == NULL) )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN " "DL LL-UDATA rcvd with unallocated DCOMP %d", sn_data->dcomp);
      }

#else

      else if( sn_data->dcomp )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN " "DL LL-UDATA rcvd with unallocated DCOMP %d", sn_data->dcomp);
      }

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */
    }
  }

  return( valid_hdr );
}


/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_DATA_PDU_HDR()
===
===  DESCRIPTION
===
===   Encodes SN-DATA PDU header from internal format to external format
===
===   *hdr_ptr:  points to SN-DATA PDU header struct in internal format
===   *dst:      points to SN-DATA PDU header external format as an array of bytes
===
===  DEPENDENCIES
===
===   None
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

void gsn_encode_data_pdu_hdr( const gsn_data_pdu_hdr_t *hdr_ptr, uint8 *dst)
{

  uint16 msg_pos = 0;    /* offset from the beginning of the message to the current item */

  /* Format SN-PDU header */
  gpackb(hdr_ptr->x, dst, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, x));
  gpackb(hdr_ptr->f, dst, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, f));
  gpackb(hdr_ptr->t, dst, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, t));
  gpackb(hdr_ptr->m, dst, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, m));
  gpackb(hdr_ptr->nsapi, dst, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, nsapi));

  /* Check if First segment of the N-PDU */
  if( hdr_ptr->f )
  {
    /* It's the first segment, include PCOMP, DCOMP and N-PDU number in the header */
    gpackb(hdr_ptr->dcomp, dst, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, dcomp));
    gpackb(hdr_ptr->pcomp, dst, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, pcomp));
    gpackb(hdr_ptr->num, dst, &msg_pos, GSIZE(gsn_data_pdu_hdr_t, num));
  }
}


/*===========================================================================
===
===  FUNCTION      GSN_DECODE_UNITDATA_PDU_HDR()
===
===  DESCRIPTION
===
===   Decodes SN-UNITDATA PDU header from external format into
===   internal format ( gsn_unitdata_pdu_hdr_t ).
===
===   *src:       array of bytes representing Down-link LLC data block
===   *sn_data:   structure representing SNDCP SN-UNITDATA PDU format
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   TRUE if valid SN-UNITDATA PDU header format, otherwise FALSE
===
===  SIDE EFFECTS
===
===
===
===========================================================================*/

boolean gsn_decode_unitdata_pdu_hdr( gas_id_t gas_id, uint8 *src, gsn_unitdata_pdu_hdr_t *sn_udata)
{
  uint16    msg_pos   = 0;
  boolean   valid_hdr = TRUE;

  /*-----------------------------------
  **  Get SN-UNITDATA PDU header fields
  ------------------------------------*/
  sn_udata->x = gunpackb(src, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, x));
  sn_udata->f = gunpackb(src, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, f));
  sn_udata->t = gunpackb(src, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, t));

  if( !(sn_udata->t) )
  {
    /* Invalid type. SN-PDU Type is set to SN-DATA */
    valid_hdr = FALSE;
    MSG_GERAN_HIGH_0("GSN ""DL SN-DATA rcvd while SN-UDATA expected");
  }

  sn_udata->m     = gunpackb(src, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, m));
  sn_udata->nsapi = gunpackb(src, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, nsapi));

  /* Check if valid NSAPI number */
  if( GSN_VALID_NSAPI( sn_udata->nsapi ) )
  {
    /* Translate external value of NSAPI into SNDCP NSAPI number */
    sn_udata->nsapi = (uint8) GSN_INTERN_NSAPI( sn_udata->nsapi );

    /* Check if NSAPI is active and uses Unack mode */
    if( (actv_nsapis[ gas_id ][sn_udata->nsapi] == NULL) ||
        (actv_nsapis[ gas_id ][sn_udata->nsapi]->op_mode != UNACK) ||
        (actv_nsapis[ gas_id ][sn_udata->nsapi]->sapi_ptr == NULL) )
    {
      /* Invalid NSAPI number */
      valid_hdr = FALSE;

      MSG_GERAN_HIGH_1("GSN ""DL SN-UDATA rcvd at an inactive NSAPI %d",
        GSN_EXTERN_NSAPI(sn_udata->nsapi));
    }
  }
  else
  {
    /* Invalid NSAPI number */
    valid_hdr = FALSE;

    MSG_GERAN_HIGH_1("GSN ""DL SN-UDATA rcvd at an invalid NSAPI %d", sn_udata->nsapi);
  }

  /* if pdu header is not valid, do not proceed but return to the caller. */
  if(valid_hdr == FALSE)
  {
    return (valid_hdr);
  }

  /* Check if first segment of an N-PDU */
  if( sn_udata->f )
  {
    /* First segment, get DCOMP, PCOMP, segment number and N-PDU number  */
    sn_udata->dcomp  = gunpackb(src, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, dcomp));
    sn_udata->pcomp  = gunpackb(src, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, pcomp));


    /* SN-PDU with unallocated DCOMP or PCOMP shall be ignored only if NSAPI's
    ** reassembly state is " Receive First Segment".
    ** If compression is not supported  PCOMP = 0,
    ** DCOMP = 0 are the only valid values.
    */

    if( actv_nsapis[ gas_id ][sn_udata->nsapi]->dle.reas_state == DL_RCV_1ST_SEQ )
    {
      if( sn_udata->pcomp >=  PCOMP_VAL_MAX )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN ""DL LL-UDATA rcvd inval PCOMP %d ",sn_udata->pcomp);
      }

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

      else if( (sn_udata->pcomp ) &&
          (actv_nsapis[ gas_id ][sn_udata->nsapi]->sapi_ptr->pcomp_pool[sn_udata->pcomp] == NULL) )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN " "DL LL-UDATA rcvd with unallocated PCOMP %d", sn_udata->pcomp);
      }

#else

      else if( sn_udata->pcomp )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN " "DL LL-UDATA rcvd with unallocated PCOMP %d", sn_udata->pcomp);
      }

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */


      if( sn_udata->dcomp >=  DCOMP_VAL_MAX )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN ""DL LL-UDATA rcvd inval DCOMP %d ",sn_udata->dcomp);
      }

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

      else if( (sn_udata->dcomp ) &&
          (actv_nsapis[ gas_id ][sn_udata->nsapi]->sapi_ptr->dcomp_pool[sn_udata->dcomp] == NULL) )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN " "DL LL-UDATA rcvd with unallocated DCOMP %d", sn_udata->dcomp);
      }

#else

      else if( sn_udata->dcomp )
      {
        valid_hdr = FALSE;
        MSG_GERAN_HIGH_1("GSN " "DL LL-UDATA rcvd with unallocated DCOMP %d", sn_udata->dcomp);
      }

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */
    }
  }

  sn_udata->seg  = gunpackb(src, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, seg));

  sn_udata->num  = gunpackw(src, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, num));

  return( valid_hdr );
}


/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_UNITDATA_PDU_HDR()
===
===  DESCRIPTION
===
===   Encodes SN-UNITDATA PDU header from internal format to external format
===
===   *hdr_ptr:   points to SN-UNITDATA PDU header struct in internal format
===   *dst:       points to SN-UNITDATA PDU header external format as an array of bytes
===
===  DEPENDENCIES
===
===   None
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

void gsn_encode_unitdata_pdu_hdr( gas_id_t gas_id, const gsn_unitdata_pdu_hdr_t *hdr_ptr, uint8 *dst)
{
  uint16 msg_pos = 0;    /* offset from the beginning of the message to the current item */

  /* format SN-PDU header */
  gpackb(hdr_ptr->x, dst, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, x));
  gpackb(hdr_ptr->f, dst, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, f));
  gpackb(hdr_ptr->t, dst, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, t));
  gpackb(hdr_ptr->m, dst, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, m));
  gpackb(hdr_ptr->nsapi, dst, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, nsapi));

  /* Check if First segment of the N-PDU */
  if( hdr_ptr->f )
  {
    /* it's the first segment, include PCOMP and DCOMP  */
    gpackb(hdr_ptr->dcomp, dst, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, dcomp));
    gpackb(hdr_ptr->pcomp, dst, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, pcomp));
  }

  gpackb(hdr_ptr->seg, dst, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, seg));
  gpackw(hdr_ptr->num, dst, &msg_pos, GSIZE(gsn_unitdata_pdu_hdr_t, num));
}

/*===========================================================================
===
===  FUNCTION      GSN_DECODE_NEG_XID()
===
===  DESCRIPTION
===
===   Decodes negotiated (by SGSN) XID block from external format into internal format
===   ( gsn_xid_block_t ) and checks if negotiated XID differ from the requested XID.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   TRUE if negotiated XID do not differ from the requested XID
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

boolean gsn_decode_neg_xid
(
  gas_id_t      gas_id,
  dsm_item_type **l3_xid,
  gsn_sapi_e_t  *sapi_ptr
)
{
  dsm_item_type     *xid_req = NULL;
  uint8             nsapis_on_sapi[GSN_NSAPI_MAX];
  gsn_dec_xid_t     dec_xid;
  uint8             nsapi, i = 0;
  gsn_xid_block_t   xid_i;
  uint8             type_hdr[XID_TYPE_HDR_LEN];
  uint8             type_len = 0;
  uint8             xid_ex[XID_T2_LEN_MAX];

  /* Clear structure that is about to hold the decoded xid parameters */
  memset( &dec_xid, 0, sizeof(gsn_dec_xid_t) );

  /* Get the NSAPIs mapped on the SAPI */
  gsn_get_nsapis_on_sapi( gas_id, sapi_ptr, nsapis_on_sapi );


  /* Print XID parameters in F3 message */
  gsn_print_xid_params ( gas_id, *l3_xid, FALSE );

  if( *l3_xid == NULL )
  {
    /* Mark all parameters as ABSENT */
    dec_xid.t0.status = XID_ABSENT;
    dec_xid.t1.status = XID_ABSENT;
    dec_xid.t2.status = XID_ABSENT;
  }
  else
  {
    /* Decode negotiated parameters */
    if( !gsn_decode_peer_xid( gas_id, l3_xid, &dec_xid, sapi_ptr, TRUE ) )
    {
      /* XID block decoding was unsuccessful.
      */
      return( FALSE );
    }
  }

  if( (sapi_ptr->xid_state != XID_EST_CNF_PEND) &&
      (sapi_ptr->xid_state != XID_CNF_PEND) )
  {
    /* No XID negotiation was initiated from SNDCP.
    ** Only default XID should be sent by the peer.
    */
    if( (dec_xid.t2.status  ==  XID_VALID )      &&
        ( (dec_xid.t2.ent[0].xid.hdr.p  != 0)      ||
          (dec_xid.t2.ent[0].xid.hdr.ent_num != 0) ||
          (dec_xid.t2.ent[0].xid.hdr.length  != 0) ) )
    {
      return( FALSE );
    }

    if( (dec_xid.t1.status  ==  XID_VALID )      &&
        ( (dec_xid.t1.ent[0].xid.hdr.p  != 0)      ||
          (dec_xid.t1.ent[0].xid.hdr.ent_num != 0) ||
          (dec_xid.t1.ent[0].xid.hdr.length  != 0))  )
    {
      return( FALSE );
    }

    if( (dec_xid.t2.status    ==  XID_ABSENT)        ||
        ( (dec_xid.t2.status  ==  XID_VALID )      &&
          (dec_xid.t2.ent[0].xid.hdr.p  == 0)      &&
          (dec_xid.t2.ent[0].xid.hdr.ent_num == 0) &&
          (dec_xid.t2.ent[0].xid.hdr.length  == 0) ) ||
        (dec_xid.t1.status    ==  XID_ABSENT)        ||
        ( (dec_xid.t1.status  ==  XID_VALID )      &&
          (dec_xid.t1.ent[0].xid.hdr.p  == 0)      &&
          (dec_xid.t1.ent[0].xid.hdr.ent_num == 0) &&
          (dec_xid.t1.ent[0].xid.hdr.length  == 0) ) ||
        (dec_xid.t0.status    ==  XID_ABSENT)        ||
        ( (dec_xid.t0.status  == XID_VALID)        &&
          ( (dec_xid.t0.ver_num == GSN_VER_NUM_DEF)  ||
            (dec_xid.t0.ver_num == GSN_VER_NUM_1  )))   )
    {
      return( TRUE );
    }
    else
    {
      return( FALSE );
    }
  }

  /*-------------- Start processing T2 xid type  -------------------------*/
  /* ----------------------------------------------------------------------*/

  if( dec_xid.t2.status !=  XID_ABSENT )
  {
    if( (dec_xid.t2.status == XID_INVALID) ||
        (dec_xid.t2.ent_in_blk > ENT_NUM_MAX +1 ) )
    {
      return( FALSE );
    }

    if( dec_xid.t2.status == XID_VALID )
    {
      for( i = 0; i < dec_xid.t2.ent_in_blk; i++ )
      {
        if( dec_xid.t2.ent[i].status == XID_INVALID )
        {
          return( FALSE );
        }

        if( dec_xid.t2.ent[i].status == XID_VALID ||
            dec_xid.t2.ent[i].status == XID_ABSENT )
        {

#ifndef FEATURE_GSM_GPRS_SNDCP_PCOMP

          /* PCI compression is not supported. The only valid XID
          ** responses are those with App NSAPIs set to zero.
          */
          if((!dec_xid.t2.ent[i].xid.alg.xid_rfc1144.nsapis)||
             (!dec_xid.t2.ent[i].xid.alg.xid_rfc2507.nsapis)||
             (!dec_xid.t2.ent[i].xid.alg.xid_rfc3095.nsapis))
          {
            return( TRUE );
          }
          else
          {
            return( FALSE );
          }
#else
          {
            uint8             ent_num;
            gsn_dec_t2_ent_t  *dec_ent;
            uint16            app_nsapis;
            pcomp_e_t         *pcomp_e_ptr;

            /* To avoid long lines of code */
            dec_ent = &dec_xid.t2.ent[i];
            ent_num = dec_ent->xid.hdr.ent_num;

            /* Verify that the entity exists */
            if(!sapi_ptr->pcomp_e_pool[ent_num] )
            {
              /* Comp Entity doesn't exist. This is only valid if App NSAPIs
              ** are set to zero as a default responce.
              */
              if( (!dec_xid.t2.ent[i].xid.alg.xid_rfc1144.nsapis)||
                  (!dec_xid.t2.ent[i].xid.alg.xid_rfc2507.nsapis)||
                  (!dec_xid.t2.ent[i].xid.alg.xid_rfc3095.nsapis))
              {
                continue; /* Get next entity */
              }
              else
              {
                /* Invalid Comp Entity number. Compression entity does not exist.
                */
                MSG_GERAN_LOW_2("GSN "" XID %d: Resp inval PCOMP Entity num %d",
                  gsn_extern_sapi[sapi_ptr->sapi],ent_num);

                return( FALSE );
              }
            }
            pcomp_e_ptr  = sapi_ptr->pcomp_e_pool[ent_num];

            /* Check XID parameter values according to the algorithm
            ** type of the entity requested.
            */
            if( pcomp_e_ptr->t2_alg == T2_ALG_RFC1144 )
            {
              /* ---------------- Applicable NSAPIs parameter ---------------*/
              if(  (dec_ent->xid.alg.xid_rfc1144.nsapis ^
                    pcomp_e_ptr->alg.xid_rfc1144.nsapis) &
                   (dec_ent->xid.alg.xid_rfc1144.nsapis) )
              {
                /* One or more App NSAPIs that were set to 0 on the XID request
                ** were set to 1 on the  XID response. Invalid value of App NSAPIs.
                ** Sense of negotiation is down (each bit separetely).
                */
                MSG_GERAN_LOW_2("GSN "" XID %d: Resp Inval App NSAPIs %d ",
                    gsn_extern_sapi[sapi_ptr->sapi],
                      dec_ent->xid.alg.xid_rfc1144.nsapis);

                return( FALSE );
              }

              if(  pcomp_e_ptr->state == UNASSIGNED )
              {
                /* XID_REQ was sent to the peer to remove comp entity.
                ** If App NSAPIs are not set to 0 in the response
                ** resend XID_REQ to the peer to remove comp entity.
                */
                if( dec_ent->xid.alg.xid_rfc1144.nsapis )
                  return( FALSE );
              }

              if( !dec_ent->xid.alg.xid_rfc1144.nsapis )
              {
                /* No App NSAPIs are set to "1". Delete the comp entity */
                gsn_del_pcomp_e( gas_id, pcomp_e_ptr->ent_num, sapi_ptr );
                MSG_GERAN_MED_2("GSN ""XID %d: Rmv PCOMP entity %d",
                    gsn_extern_sapi[sapi_ptr->sapi],ent_num);

                continue; /* Get next entity */
              }
              else
              {
                app_nsapis = ( (dec_ent->xid.alg.xid_rfc1144.nsapis ^
                                pcomp_e_ptr->alg.xid_rfc1144.nsapis) &
                               (pcomp_e_ptr->alg.xid_rfc1144.nsapis)
                             );

                if( app_nsapis )
                {
                  /* The App NSAPIs proposed to use the entity differ to the
                  ** ones suggested by the peer. The App NSAPIs that
                  ** were set from 1 to 0 must stop using the comp entity.
                  */
                  for( i = 0; i < (uint8) GSN_NSAPI_MAX; i++ )
                  {
                    /* Get the first NSAPI mapped on the SAPI */
                    nsapi = nsapis_on_sapi[i];

                    /* Exit the for loop if end of valid NSAPIs */
                    if( nsapi == GSN_INVAL_NSAPI )
                      break;

                    if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapi) ) )
                    {
                      /* Remove comp entity from NSAPI if it has been using the entity */
                      if( actv_nsapis[ gas_id ][nsapi] )
                        actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC1144]= NO_COMP_ENT;
                    }
                  }
                }
              }

              /* Update entity's parameter value */
              pcomp_e_ptr->alg.xid_rfc1144.nsapis = dec_ent->xid.alg.xid_rfc1144.nsapis;

              /* ---------------- "S0_1" parameter -----------------------------------*/
              if( dec_ent->xid.hdr.length >= (APP_NSAPIS_LEN + RFC1144_S0_1_LEN) )
              {
                if( dec_ent->xid.alg.xid_rfc1144.s0_1 > pcomp_e_ptr->alg.xid_rfc1144.s0_1 )
                {
                  /* Invalid value of S0. Sense of negotiation is down */
                  MSG_GERAN_LOW_2("GSN "" XID %d: Resp Inval S0_1 %d ",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_rfc1144.s0_1);
                  return( FALSE );
                }

                /* Accept the value proposed by the peer */
                pcomp_e_ptr->alg.xid_rfc1144.s0_1 = dec_ent->xid.alg.xid_rfc1144.s0_1;
              }

              /* The Comp entity parameters negotiated by the peer were accepted
              ** by the originator. The NSAPIs can start using the comp entity.
              */
              if( pcomp_e_ptr->state == SELECTED )
              {
                /* Assign PCOMP values to the algorithm */
                if( (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc1144.pcomp1] != NULL) &&
                    (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc1144.pcomp2] != NULL) &&
                    (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc1144.pcomp1]->
                          alg == T2_ALG_RFC1144)                                        &&
                    (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc1144.pcomp2]->
                          alg == T2_ALG_RFC1144) )
                {
                  if(sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc1144.pcomp1]->
                          state == SELECTED )
                  {
                    sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc1144.pcomp1]->
                          state = ASSIGNED;
                  }
                  if(sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc1144.pcomp2]->
                          state == SELECTED )
                  {
                    sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc1144.pcomp2]->
                          state = ASSIGNED;
                  }
                }
                else
                {
                  /* PCOMP values are assigned to another PCI comp algorithm.
                  ** It should not be requested for the selected compression
                  ** entity.
                  */
                  MSG_GERAN_ERROR_1("GSN "" XID %d: PCOMP requested on different alg ",
                    gsn_extern_sapi[sapi_ptr->sapi]);
                  return( FALSE );
                }

                /* Entity number is assigned and can be used by the App NSAPIs */
                pcomp_e_ptr->state = ASSIGNED;
              }

              /* link App NSAPI to the comp entity
              */
              for( i = 0; i < (uint8) GSN_NSAPI_MAX; i++ )
              {
                /* Get the first NSAPI mapped on the SAPI */
                nsapi = nsapis_on_sapi[i];

                /* Exit the for loop if end of valid NSAPIs */
                if( nsapi == GSN_INVAL_NSAPI )
                  break;

                if( pcomp_e_ptr->alg.xid_rfc1144.nsapis &
                    (0x1 << GSN_EXTERN_NSAPI(nsapi)) )
                {
                  if( actv_nsapis[ gas_id ][nsapi] != NULL )
                    actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC1144] =
                      pcomp_e_ptr->ent_num;
                  else
                    MSG_GERAN_ERROR_1("GSN ""XID: Invalid NSAPI %d",nsapi);
                }
              }

              /* Before start using the entity perform the consistency test*/
              if(!gsn_check_pcomp_e( gas_id, pcomp_e_ptr, sapi_ptr ) )
              {
                MSG_GERAN_ERROR_2("GSN "" XID: PCI comp entity %d params not consistant",
                    gsn_extern_sapi[sapi_ptr->sapi], pcomp_e_ptr->ent_num);
                  return( FALSE );
              }

              /* Initialise RFC 1144 compression entity */
              if( !gsn_1144_comp_init(gas_id, pcomp_e_ptr,
                   (uint8) (pcomp_e_ptr->alg.xid_rfc1144.s0_1 + 1) ) )
              {
                /* Heap Exhaustion. Remove compression entity */

                /* Mark entity as unassigned in order to be deleted
                ** after XID_CNF/EST_CNF received
                */
                pcomp_e_ptr->state = UNASSIGNED;

                xid_i.type_field.xid_t1.hdr.ent_num = ent_num;

                /* Do not include Type header. More entities might follow */
                xid_i.length = 0;
                gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_req );

                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
              }
              else
              {
                MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use RFC 1144 ent %d ",
                  gsn_extern_sapi[sapi_ptr->sapi],
                    pcomp_e_ptr->alg.xid_rfc1144.nsapis,pcomp_e_ptr->ent_num);
              }

            } /* ------ End processing xid params for RFC 1144 algorithm -------- */
            else if(pcomp_e_ptr->t2_alg == T2_ALG_RFC2507)
            {

              /* ---------------- Applicable NSAPIs parameter ---------------*/
              if(  (dec_ent->xid.alg.xid_rfc2507.nsapis ^
                    pcomp_e_ptr->alg.xid_rfc2507.nsapis) &
                   (dec_ent->xid.alg.xid_rfc2507.nsapis) )
              {
                /* One or more App NSAPIs that were set to 0 on the XID request
                ** were set to 1 on the  XID response. Invalid value of App NSAPIs.
                ** Sense of negotiation is down (each bit separetely).
                */
                MSG_GERAN_LOW_2("GSN "" XID %d: Resp Inval App NSAPIs %d ",
                    gsn_extern_sapi[sapi_ptr->sapi],
                      dec_ent->xid.alg.xid_rfc2507.nsapis);

                /* 3gpp 44.065, 6.8.1 states that a new entity should be
                   created in this case */

                return( FALSE );
              }

              if(  pcomp_e_ptr->state == UNASSIGNED )
              {
                /* XID_REQ was sent to the peer to remove comp entity.
                ** If App NSAPIs are not set to 0 in the response
                ** resend XID_REQ to the peer to remove comp entity.
                */
                if( dec_ent->xid.alg.xid_rfc2507.nsapis )
                  return( FALSE );
              }

              if( !dec_ent->xid.alg.xid_rfc2507.nsapis )
              {
                /* No App NSAPIs are set to "1". Delete the comp entity */
                gsn_del_pcomp_e( gas_id, pcomp_e_ptr->ent_num, sapi_ptr );
                MSG_GERAN_MED_2("GSN ""XID %d: Rmv PCOMP entity %d",
                    gsn_extern_sapi[sapi_ptr->sapi],ent_num);

                continue; /* Get next entity */
              }
              else
              {
                app_nsapis = ( (dec_ent->xid.alg.xid_rfc2507.nsapis ^
                                pcomp_e_ptr->alg.xid_rfc2507.nsapis) &
                               (pcomp_e_ptr->alg.xid_rfc2507.nsapis)
                             );

                if( app_nsapis )
                {
                  /* The App NSAPIs proposed to use the entity differ to the
                  ** ones suggested by the peer. The App NSAPIs that
                  ** were set from 1 to 0 must stop using the comp entity.
                  */
                  for( i = 0; i < (uint8) GSN_NSAPI_MAX; i++ )
                  {
                    /* Get the first NSAPI mapped on the SAPI */
                    nsapi = nsapis_on_sapi[i];

                    /* Exit the for loop if end of valid NSAPIs */
                    if( nsapi == GSN_INVAL_NSAPI )
                      break;

                    if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapi) ) )
                    {
                      /* Remove comp entity from NSAPI if it has been using the entity */
                      if( actv_nsapis[ gas_id ][nsapi] )
                        actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC2507]= NO_COMP_ENT;
                    }
                  }
                }
              }

              /* Update entity's parameter value */
              pcomp_e_ptr->alg.xid_rfc2507.nsapis =
                                      dec_ent->xid.alg.xid_rfc2507.nsapis;


              if( dec_ent->xid.hdr.length >= (APP_NSAPIS_LEN       +
                                              RFC2507_MAX_PER_LEN  +
                                              RFC2507_MAX_TIME_LEN +
                                              RFC2507_MAX_HDR_LEN  +
                                              RFC2507_TCP_SP_LEN   +
                                              RFC2507_NTCP_SP_LEN) )
              {
                if( dec_ent->xid.alg.xid_rfc2507.max_per >
                      pcomp_e_ptr->alg.xid_rfc2507.max_per)
                {
                  /* Invalid value of F_MAX_PERIOD. Sense of negotiation is down */
                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval F_MAX_PERIOD %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc2507.max_per,
                                  0);
                  return( FALSE );
                }

                if( dec_ent->xid.alg.xid_rfc2507.max_time >
                      pcomp_e_ptr->alg.xid_rfc2507.max_time)
                {
                  /* Invalid value of F_MAX_TIME. Sense of negotiation is down */
                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval F_MAX_TIME %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc2507.max_time,
                                  0);
                  return( FALSE );
                }

                if( dec_ent->xid.alg.xid_rfc2507.max_hdr >
                      pcomp_e_ptr->alg.xid_rfc2507.max_hdr)
                {
                  /* Invalid value of F_MAX_HEADER. Sense of negotiation is down */
                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval F_MAX_HEADER %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc2507.max_hdr,
                                  0);
                  return( FALSE );
                }

                if( dec_ent->xid.alg.xid_rfc2507.tcp_sp >
                      pcomp_e_ptr->alg.xid_rfc2507.tcp_sp)
                {
                  /* Invalid value of TCP_SPACE. Sense of negotiation is down */
                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval TCP_SPACE %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc2507.tcp_sp,
                                  0);
                  return( FALSE );
                }

                if( dec_ent->xid.alg.xid_rfc2507.ntcp_sp >
                      pcomp_e_ptr->alg.xid_rfc2507.ntcp_sp)
                {
                  /* Invalid value of TCP_SPACE. Sense of negotiation is down */
                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval NON_TCP_SPACE %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc2507.ntcp_sp,
                                  0);
                  return( FALSE );
                }


                /* Accept the values proposed by the peer */
                pcomp_e_ptr->alg.xid_rfc2507.max_per =
                  dec_ent->xid.alg.xid_rfc2507.max_per;

                pcomp_e_ptr->alg.xid_rfc2507.max_time =
                  dec_ent->xid.alg.xid_rfc2507.max_time;

                pcomp_e_ptr->alg.xid_rfc2507.max_hdr =
                  dec_ent->xid.alg.xid_rfc2507.max_hdr;

                pcomp_e_ptr->alg.xid_rfc2507.tcp_sp =
                  dec_ent->xid.alg.xid_rfc2507.tcp_sp;

                pcomp_e_ptr->alg.xid_rfc2507.ntcp_sp =
                  dec_ent->xid.alg.xid_rfc2507.ntcp_sp;

              }

              /* The Comp entity parameters negotiated by the peer were accepted
              ** by the originator. The NSAPIs can start using the comp entity.
              */
              if( pcomp_e_ptr->state == SELECTED )
              {
                /* Assign PCOMP values to the algorithm */
                if((sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp1] != NULL) &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp2] != NULL) &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp3] != NULL) &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp4] != NULL) &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp5] != NULL) &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp1]->alg ==
                                T2_ALG_RFC2507)                                        &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp2]->alg ==
                                T2_ALG_RFC2507)                                        &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp3]->alg ==
                                T2_ALG_RFC2507)                                        &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp4]->alg ==
                                T2_ALG_RFC2507)                                        &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp5]->alg ==
                                T2_ALG_RFC2507)
                  )
                {
                  if(sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp1]->
                          state == SELECTED )
                  {
                    sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp1]->
                          state = ASSIGNED;
                  }
                  if(sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp2]->
                          state == SELECTED )
                  {
                    sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp2]->
                          state = ASSIGNED;
                  }
                  if(sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp3]->
                          state == SELECTED )
                  {
                    sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp3]->
                          state = ASSIGNED;
                  }
                  if(sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp4]->
                          state == SELECTED )
                  {
                    sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp4]->
                          state = ASSIGNED;
                  }
                  if(sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp5]->
                          state == SELECTED )
                  {
                    sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc2507.pcomp5]->
                          state = ASSIGNED;
                  }
                }
                else
                {
                  /* PCOMP values are assigned to another PCI comp algorithm.
                  ** It should not be requested for the selected compression
                  ** entity.
                  */
                  MSG_GERAN_ERROR_1("GSN "" XID %d: PCOMP requested on different alg ",
                    gsn_extern_sapi[sapi_ptr->sapi]);
                  return( FALSE );
                }

                /* Entity number is assigned and can be used by the App NSAPIs */
                pcomp_e_ptr->state = ASSIGNED;
              }

              /* link App NSAPI to the comp entity
              */
              for( i = 0; i < (uint8) GSN_NSAPI_MAX; i++ )
              {
                /* Get the first NSAPI mapped on the SAPI */
                nsapi = nsapis_on_sapi[i];

                /* Exit the for loop if end of valid NSAPIs */
                if( nsapi == GSN_INVAL_NSAPI )
                  break;

                if( pcomp_e_ptr->alg.xid_rfc2507.nsapis &
                    (0x1 << GSN_EXTERN_NSAPI(nsapi)) )
                {
                  if( actv_nsapis[ gas_id ][nsapi] != NULL )
                    actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC2507] =
                      pcomp_e_ptr->ent_num;
                  else
                    MSG_GERAN_ERROR_1("GSN ""XID: Invalid NSAPI %d",nsapi);
                }
              }

              /* Before start using the entity perform the consistency test*/
              if(!gsn_check_pcomp_e( gas_id, pcomp_e_ptr, sapi_ptr ) )
              {
                MSG_GERAN_ERROR_2("GSN "" XID: PCI comp entity %d params not consistant",
                    gsn_extern_sapi[sapi_ptr->sapi], pcomp_e_ptr->ent_num);
                  return( FALSE );
              }

              /* Initialise RFC 2507 compression entity */
              if( !gsn_iphc_comp_init(
                    gas_id, pcomp_e_ptr,
                   (gsn_xid_pcomp_2507_t *) (&pcomp_e_ptr->alg.xid_rfc2507) ))
              {
                /* Memory allocation failed in pcomp engine. Remove compression entity */

                /* Mark entity as unassigned in order to be deleted
                ** after XID_CNF/EST_CNF received
                */
                pcomp_e_ptr->state = UNASSIGNED;

                xid_i.type_field.xid_t1.hdr.ent_num = ent_num;

                /* Do not include Type header. More entities might follow */
                xid_i.length = 0;
                gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_req );

                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
              }
              else
              {
                MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use RFC 2507 ent %d ",
                             gsn_extern_sapi[sapi_ptr->sapi],
                             pcomp_e_ptr->alg.xid_rfc2507.nsapis,
                             pcomp_e_ptr->ent_num);
              }

            }/* End processing xid params for RFC 2507 algorithm */

            else if(pcomp_e_ptr->t2_alg == T2_ALG_RFC3095)
            {

              /* ---------------- Applicable NSAPIs parameter ---------------*/
              if(  (dec_ent->xid.alg.xid_rfc3095.nsapis ^
                    pcomp_e_ptr->alg.xid_rfc3095.nsapis) &
                   (dec_ent->xid.alg.xid_rfc3095.nsapis) )
              {
                /* One or more App NSAPIs that were set to 0 on the XID request
                ** were set to 1 on the  XID response. Invalid value of App
                ** NSAPIs. Sense of negotiation is down (each bit separetely).
                */
                MSG_GERAN_LOW_2("GSN "" XID %d: Resp Inval App NSAPIs %d ",
                    gsn_extern_sapi[sapi_ptr->sapi],
                      dec_ent->xid.alg.xid_rfc3095.nsapis);

                /* 3gpp 44.065, 6.8.1 states that a new entity should be
                   created in this case */

                return( FALSE );
              }

              if(  pcomp_e_ptr->state == UNASSIGNED )
              {
                /* XID_REQ was sent to the peer to remove comp entity.
                ** If App NSAPIs are not set to 0 in the response
                ** resend XID_REQ to the peer to remove comp entity.
                */
                if( dec_ent->xid.alg.xid_rfc3095.nsapis )
                  return( FALSE );
              }

#if 0
              /* Disable ROHC for now */
              dec_ent->xid.alg.xid_rfc3095.nsapis = 0;
#endif
              if( !dec_ent->xid.alg.xid_rfc3095.nsapis )
              {
                /* No App NSAPIs are set to "1". Delete the comp entity */
                gsn_del_pcomp_e( gas_id, pcomp_e_ptr->ent_num, sapi_ptr );
                MSG_GERAN_MED_2("GSN ""XID %d: Rmv PCOMP entity %d",
                    gsn_extern_sapi[sapi_ptr->sapi],ent_num);

                continue; /* Get next entity */
              }
              else
              {
                app_nsapis = ( (dec_ent->xid.alg.xid_rfc3095.nsapis ^
                                pcomp_e_ptr->alg.xid_rfc3095.nsapis) &
                               (pcomp_e_ptr->alg.xid_rfc3095.nsapis)
                             );

                if( app_nsapis )
                {
                  /* The App NSAPIs proposed to use the entity differ to the
                  ** ones suggested by the peer. The App NSAPIs that
                  ** were set from 1 to 0 must stop using the comp entity.
                  */
                  for( i = 0; i < (uint8) GSN_NSAPI_MAX; i++ )
                  {
                    /* Get the first NSAPI mapped on the SAPI */
                    nsapi = nsapis_on_sapi[i];

                    /* Exit the for loop if end of valid NSAPIs */
                    if( nsapi == GSN_INVAL_NSAPI )
                      break;

                    if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapi) ) )
                    {
                      /* Remove comp entity from NSAPI if it has been using the entity */
                      if( actv_nsapis[ gas_id ][nsapi] )
                        actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC3095]= NO_COMP_ENT;
                    }
                  }
                }
              }

              /* Update entity's parameter value */
              pcomp_e_ptr->alg.xid_rfc3095.nsapis =
                                      dec_ent->xid.alg.xid_rfc3095.nsapis;


              if( dec_ent->xid.hdr.length >= (APP_NSAPIS_LEN      +
                                              RFC3095_MAX_CID_LEN  +
                                              RFC3095_MAX_HDR_LEN  +
                                              (RFC3095_PROF_LEN*3)))
              {
                if( dec_ent->xid.alg.xid_rfc3095.max_cid >
                      pcomp_e_ptr->alg.xid_rfc3095.max_cid)
                {
                  /* Invalid value of F_MAX_CID. Sense of negotiation is down */
                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval F_MAX_TIME %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc3095.max_cid,
                                  0);
                  return( FALSE );
                }

                if( dec_ent->xid.alg.xid_rfc3095.max_hdr >
                      pcomp_e_ptr->alg.xid_rfc3095.max_hdr)
                {
                  /* Invalid value of F_MAX_HEADER. Sense of negotiation is down */
                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval F_MAX_HEADER %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc3095.max_hdr,
                                  0);
                  return( FALSE );
                }

                if( dec_ent->xid.alg.xid_rfc3095.profile1 !=
                      pcomp_e_ptr->alg.xid_rfc3095.profile1)
                {
                  /* Invalid value of PROFILE1 */
                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval PROFILE1 %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc3095.profile1,
                                  0);
                  return( FALSE );
                }

                if( dec_ent->xid.alg.xid_rfc3095.profile2 !=
                      pcomp_e_ptr->alg.xid_rfc3095.profile2)
                {
                  /* Invalid value of PROFILE2 */
                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval PROFILE2 %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc3095.profile2,
                                  0);
                  return( FALSE );
                }


                if( dec_ent->xid.alg.xid_rfc3095.profile3 !=
                      pcomp_e_ptr->alg.xid_rfc3095.profile3)
                {

                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval PROFILE3 %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc3095.profile3,
                                  0);
                  return( FALSE );
                }


                if( dec_ent->xid.alg.xid_rfc3095.profile4 !=
                      pcomp_e_ptr->alg.xid_rfc3095.profile4)
                {

                  MSG_GERAN_LOW_3("GSN "" XID %d: Resp Inval PROFILE3 %d ",
                                  gsn_extern_sapi[sapi_ptr->sapi],
                                  dec_ent->xid.alg.xid_rfc3095.profile3,
                                  0);
                  return( FALSE );
                }

                /* Accept the values proposed by the peer */
                pcomp_e_ptr->alg.xid_rfc3095.max_cid =
                  dec_ent->xid.alg.xid_rfc3095.max_cid;

                pcomp_e_ptr->alg.xid_rfc3095.max_hdr =
                  dec_ent->xid.alg.xid_rfc3095.max_hdr;

                pcomp_e_ptr->alg.xid_rfc3095.profile1 =
                  dec_ent->xid.alg.xid_rfc3095.profile1;

                pcomp_e_ptr->alg.xid_rfc3095.profile2 =
                  dec_ent->xid.alg.xid_rfc3095.profile2;

                pcomp_e_ptr->alg.xid_rfc3095.profile3 =
                  dec_ent->xid.alg.xid_rfc3095.profile3;

                pcomp_e_ptr->alg.xid_rfc3095.profile4 =
                  dec_ent->xid.alg.xid_rfc3095.profile4;
              }

              /* The Comp entity parameters negotiated by the peer were accepted
              ** by the originator. The NSAPIs can start using the comp entity.
              */
              if( pcomp_e_ptr->state == SELECTED )
              {
                /* Assign PCOMP values to the algorithm */
                if((sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc3095.pcomp1] != NULL) &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc3095.pcomp2] != NULL) &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc3095.pcomp1]->alg ==
                                T2_ALG_RFC3095)                                        &&
                   (sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc3095.pcomp2]->alg ==
                                T2_ALG_RFC3095)
                  )
                {
                  if(sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc3095.pcomp1]->
                          state == SELECTED )
                  {
                    sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc3095.pcomp1]->
                          state = ASSIGNED;
                  }
                  if(sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc3095.pcomp2]->
                          state == SELECTED )
                  {
                    sapi_ptr->pcomp_pool[pcomp_e_ptr->alg.xid_rfc3095.pcomp2]->
                          state = ASSIGNED;
                  }
                }
                else
                {
                  /* PCOMP values are assigned to another PCI comp algorithm.
                  ** It should not be requested for the selected compression
                  ** entity.
                  */
                  MSG_GERAN_ERROR_1("GSN "" XID %d: PCOMP requested on different alg ",
                    gsn_extern_sapi[sapi_ptr->sapi]);
                  return( FALSE );
                }

                /* Entity number is assigned and can be used by the App NSAPIs */
                pcomp_e_ptr->state = ASSIGNED;
              }

              /* link App NSAPI to the comp entity
              */
              for( i = 0; i < (uint8) GSN_NSAPI_MAX; i++ )
              {
                /* Get the first NSAPI mapped on the SAPI */
                nsapi = nsapis_on_sapi[i];

                /* Exit the for loop if end of valid NSAPIs */
                if( nsapi == GSN_INVAL_NSAPI )
                  break;

                if( pcomp_e_ptr->alg.xid_rfc3095.nsapis &
                    (0x1 << GSN_EXTERN_NSAPI(nsapi)) )
                {
                  if( actv_nsapis[ gas_id ][nsapi] != NULL )
                    actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC3095] =
                      pcomp_e_ptr->ent_num;
                  else
                    MSG_GERAN_ERROR_1("GSN ""XID: Invalid NSAPI %d",nsapi);
                }
              }

              /* Before start using the entity perform the consistency test*/
              if(!gsn_check_pcomp_e( gas_id, pcomp_e_ptr, sapi_ptr ) )
              {
                MSG_GERAN_ERROR_2("GSN "" XID: PCI comp entity %d params not consistant",
                    gsn_extern_sapi[sapi_ptr->sapi], pcomp_e_ptr->ent_num);
                  return( FALSE );
              }

              /* Initialise RFC 3095 compression entity */
              if(!gsn_rohc_comp_init(
                   gas_id, (void *)  pcomp_e_ptr,
                   (gsn_xid_pcomp_3095_t *) (&pcomp_e_ptr->alg.xid_rfc3095) ))

              {
                /* Memory allocation failed in pcomp engine. Remove compression entity */

                /* Mark entity as unassigned in order to be deleted
                ** after XID_CNF/EST_CNF received
                */
                pcomp_e_ptr->state = UNASSIGNED;

                xid_i.type_field.xid_t1.hdr.ent_num = ent_num;

                /* Do not include Type header. More entities might follow */
                xid_i.length = 0;
                gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_req );

                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
              }
              else
              {
                MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use RFC 3095 ent %d ",
                             gsn_extern_sapi[sapi_ptr->sapi],
                             pcomp_e_ptr->alg.xid_rfc3095.nsapis,
                             pcomp_e_ptr->ent_num);
              }
            }/* End processing xid params for RFC 3095 algorithm */
            else
            {
              MSG_GERAN_ERROR_0("GSN ""Invalid T2 alg on selected comp entity ");
            }
          }
#endif /*  FEATURE_GSM_GPRS_SNDCP_PCOMP */

        } /* dec_xid.t2.ent[i].status == XID_VALID || XID_ABSENT */
      } /* for( i = 0; i < dec_xid.t2.ent_in_blk; i++ ) */
    } /* if( dec_xid.t2.status = XID_VALID )            */
    else if( dec_xid.t2.status == XID_INVALID )
      return( FALSE );

  } /* if( dec_xid.t2.status !=  XID_ABSENT )           */

/* Find any SELECTED entities that have been implicitly accepted by
  ** the peer (not included in the XID reponse).See TS 04.65 par 6.8.2
  */
#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
  {
    t2_alg_q_t        *q_item;
    uint8             alg;

    for( alg = 0; alg <= T2_ALG_MAX; alg++ )
    {
      q_item = q_check( &sapi_ptr->t2_alg_q[alg] );

      /* Find the SELECTED entities that need to be ASSIGNED */
      while( q_item != NULL )
      {
        if( q_item->pcomp_e_ptr->state == SELECTED )
        {
          if(q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC1144)
          {
          /* Assign PCOMP values to the algorithm */
          if( (sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp1] != NULL) &&
              (sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp2] != NULL) )
          {
            if(sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp1]->
                    state == SELECTED )
            {
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp1]->
                    state = ASSIGNED;
            }
            if(sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp2]->
                    state == SELECTED )
            {
              sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp2]->
                    state = ASSIGNED;
            }
            else
            {
              /* PCOMP values are not SELECTED.
              */
              MSG_GERAN_ERROR_1("GSN "" XID %d: PCOMP values are't SELECTED ",
                gsn_extern_sapi[sapi_ptr->sapi]);
              return( FALSE );
            }

            /* Entity number is assigned and can be used by the App NSAPIs */
            q_item->pcomp_e_ptr->state = ASSIGNED;
          }
          }
          else if (q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC2507)
          {
            /* Assign PCOMP values to the algorithm */
            if( (sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp1] != NULL) &&
                (sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp2] != NULL) )
            {
              if(sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp1]->
                    state == SELECTED )
              {
                sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp1]->
                    state = ASSIGNED;
              }
              if(sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp2]->
                    state == SELECTED )
              {
                sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp2]->
                    state = ASSIGNED;
              }
              if(sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp3]->
                    state == SELECTED )
              {
                sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp3]->
                    state = ASSIGNED;
              }
              if(sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp4]->
                    state == SELECTED )
              {
                sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp4]->
                    state = ASSIGNED;
              }
              if(sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp5]->
                    state == SELECTED )
              {
                sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp5]->
                    state = ASSIGNED;
              }
              else
              {
                /* PCOMP values are not SELECTED. */
                MSG_GERAN_ERROR_1("GSN "" XID %d: PCOMP values are't SELECTED ",
                            gsn_extern_sapi[sapi_ptr->sapi]);
                return( FALSE );
              }

              /* Entity number is assigned and can be used by the App NSAPIs */
              q_item->pcomp_e_ptr->state = ASSIGNED;
            }
          }
          else if (q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC3095)
          {
            /* Assign PCOMP values to the algorithm */
            if( (sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp1] != NULL) &&
                (sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp2] != NULL) )
            {
              if(sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp1]->
                    state == SELECTED )
              {
                sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp1]->
                    state = ASSIGNED;
              }
              if(sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp2]->
                    state == SELECTED )
              {
                sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp2]->
                    state = ASSIGNED;
              }
              else
              {
                /* PCOMP values are not SELECTED. */
                MSG_GERAN_ERROR_1("GSN "" XID %d: PCOMP values are't SELECTED ",
                            gsn_extern_sapi[sapi_ptr->sapi]);
                return( FALSE );
              }

              /* Entity number is assigned and can be used by the App NSAPIs */
              q_item->pcomp_e_ptr->state = ASSIGNED;
            }
          }
          else
          {
            MSG_GERAN_ERROR_0("GSN ""Invalid algorithm type");
          }

          /* link App NSAPI to the comp entity */
          for( i = 0; i < (uint8) GSN_NSAPI_MAX; i++ )
          {
            /* Get the first NSAPI mapped on the SAPI */
            nsapi = nsapis_on_sapi[i];

            /* Exit the for loop if end of valid NSAPIs */
            if( nsapi == GSN_INVAL_NSAPI )
              break;

            if(q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC1144)
            {
              if( q_item->pcomp_e_ptr->alg.xid_rfc1144.nsapis &
                ( 0x1 << GSN_EXTERN_NSAPI(nsapi) ) )
              {
                if( actv_nsapis[ gas_id ][nsapi] != NULL )
                 actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC1144] =
                   q_item->pcomp_e_ptr->ent_num;
                else
                 MSG_GERAN_ERROR_1("GSN ""XID: Invalid NSAPI %d",nsapi);
              }
            }
            else if(q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC2507)
            {
              if( q_item->pcomp_e_ptr->alg.xid_rfc2507.nsapis &
                ( 0x1 << GSN_EXTERN_NSAPI(nsapi) ) )
              {
                if( actv_nsapis[ gas_id ][nsapi] != NULL )
                  actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC2507] =
                              q_item->pcomp_e_ptr->ent_num;
                else
                  MSG_GERAN_ERROR_1("GSN ""XID: Invalid NSAPI %d",nsapi);
              }
            }
            else if(q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC3095)
            {
              if( q_item->pcomp_e_ptr->alg.xid_rfc3095.nsapis &
                ( 0x1 << GSN_EXTERN_NSAPI(nsapi) ) )
              {
                if( actv_nsapis[ gas_id ][nsapi] != NULL )
                  actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC3095] =
                              q_item->pcomp_e_ptr->ent_num;
                else
                  MSG_GERAN_ERROR_1("GSN ""XID: Invalid NSAPI %d",nsapi);
              }
            }
          }

          /* Before start using the entity perform the consistency test*/
          if(!gsn_check_pcomp_e( gas_id, q_item->pcomp_e_ptr, sapi_ptr ) )
          {
            MSG_GERAN_ERROR_2("GSN "" XID: PCI comp entity %d params not consistant",
                gsn_extern_sapi[sapi_ptr->sapi], q_item->pcomp_e_ptr->ent_num);
              return( FALSE );
          }

          if(q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC1144)
          {
            /* Initialise RFC 1144 compression entity */
            if(!gsn_1144_comp_init(gas_id, q_item->pcomp_e_ptr,
               (uint8) (q_item->pcomp_e_ptr->alg.xid_rfc1144.s0_1 + 1) ) )
            {
              /* Heap Exhaustion. Remove compression entity */

              /* Mark entity as unassigned in order to be deleted
               ** after XID_CNF/EST_CNF received
               */
              q_item->pcomp_e_ptr->state = UNASSIGNED;

              xid_i.type_field.xid_t1.hdr.ent_num =
                                      q_item->pcomp_e_ptr->ent_num;

              /* Do not include Type header. More entities might follow */
              xid_i.length = 0;
              gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_req );

              type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
            }
            else
            {
              MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use RFC 1144 ent %d ",
               gsn_extern_sapi[sapi_ptr->sapi],
                          q_item->pcomp_e_ptr->alg.xid_rfc1144.nsapis,
                          q_item->pcomp_e_ptr->ent_num);
            }
          }
          else if(q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC2507)
          {
            /* Initialise RFC 2507 compression entity */
            if(!gsn_iphc_comp_init(
                 gas_id, q_item->pcomp_e_ptr,
                (gsn_xid_pcomp_2507_t *)(&q_item->pcomp_e_ptr->alg.xid_rfc2507)) )
            {
              /* Memory allocation failed in pcomp engine. Remove compression entity */

              /* Mark entity as unassigned in order to be deleted
               ** after XID_CNF/EST_CNF received
               */
              q_item->pcomp_e_ptr->state = UNASSIGNED;

              xid_i.type_field.xid_t1.hdr.ent_num = q_item->pcomp_e_ptr->ent_num;

              /* Do not include Type header. More entities might follow */
              xid_i.length = 0;
              gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_req );

              type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
            }
            else
            {
              MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use RFC 2507 ent %d ",
                          gsn_extern_sapi[sapi_ptr->sapi],
                          q_item->pcomp_e_ptr->alg.xid_rfc2507.nsapis,
                          q_item->pcomp_e_ptr->ent_num);
            }

          }
          else if(q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC3095)
          {
            if(!gsn_rohc_comp_init(
                gas_id, (void *)q_item->pcomp_e_ptr,
                (gsn_xid_pcomp_3095_t *)(&q_item->pcomp_e_ptr->alg.xid_rfc3095)))
            {
              /* Memory allocation failed in pcomp engine. Remove compression entity */

              /* Mark entity as unassigned in order to be deleted
               ** after XID_CNF/EST_CNF received
               */
              q_item->pcomp_e_ptr->state = UNASSIGNED;

              xid_i.type_field.xid_t1.hdr.ent_num = q_item->pcomp_e_ptr->ent_num;

              /* Do not include Type header. More entities might follow */
              xid_i.length = 0;
              gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_req );

              type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
            }
            else
            {
              MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use RFC 3095 ent %d ",
                          gsn_extern_sapi[sapi_ptr->sapi],
                          q_item->pcomp_e_ptr->alg.xid_rfc3095.nsapis,
                          q_item->pcomp_e_ptr->ent_num);
            }
          }
          else
          {
            MSG_GERAN_ERROR_0("GSN ""Invalid T2 alg");
          }
        }
        else if( (q_item->pcomp_e_ptr->state == UNASSIGNED) &&
                 !(dec_xid.t2.ent_map & (0x01 << q_item->pcomp_e_ptr->ent_num)) )
        {
          /* XID_REQ was sent to the peer to remove comp entity.
          ** The peer implicitly accepted by responding with empty XID block.
          ** Remove compression entity.
          */
          gsn_del_pcomp_e( gas_id, q_item->pcomp_e_ptr->ent_num, sapi_ptr);
        }

        /* Get next entity from the queue */
        q_item = q_next( &sapi_ptr->t2_alg_q[alg], &q_item->link );

      }/* end while loop */
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

  /* All T2 entities have been processed. If an XID request
  ** block has been formed, add the type header.
  */
  if( xid_req )
  {
    type_hdr[XID_TYPE_PARAM_HDR_POS] = XID_TYPE_2;
    type_hdr[XID_LEN_PARAM_HDR_POS]  = type_len;
    type_len = 0;

    /* Append type header to the existing XID block */
    if( !gprs_pdu_append_head( &xid_req, type_hdr, XID_TYPE_HDR_LEN) )
    {
      MSG_GERAN_ERROR_1("GSN ""LL %d: Append XID block failed",
        gsn_extern_sapi[sapi_ptr->sapi]);
    }
  }

  /*-------------- Start processing T1 xid type  -------------------------*/
  /* ----------------------------------------------------------------------*/

  if( dec_xid.t1.status !=  XID_ABSENT )
  {
    if( (dec_xid.t1.status == XID_INVALID) ||
        (dec_xid.t1.ent_in_blk > ENT_NUM_MAX +1 ) )
    {
      MSG_GERAN_LOW_2("GSN "" XID %d: inval DCOMP XID ent %d",
                gsn_extern_sapi[sapi_ptr->sapi],i);
      return( FALSE );
    }

    if( dec_xid.t1.status == XID_VALID )
    {
      for( i = 0; i < dec_xid.t1.ent_in_blk; i++ )
      {
        if( dec_xid.t1.ent[i].status == XID_INVALID )
        {
          MSG_GERAN_LOW_2("GSN "" XID %d: inval DCOMP XID ent %d",
                gsn_extern_sapi[sapi_ptr->sapi],i);
          return( FALSE );
        }

        if( dec_xid.t1.ent[i].status == XID_VALID ||
            dec_xid.t1.ent[i].status == XID_ABSENT )
        {

#ifndef FEATURE_GSM_GPRS_SNDCP_DCOMP

          /* Data compression is not supported. The only valid XID
          ** responses are those with App NSAPIs set to zero.
          */
          if( (!dec_xid.t1.ent[i].xid.alg.xid_v42bis.nsapis) )
          {
            return( TRUE );
          }
          else
          {
            MSG_GERAN_LOW_2("GSN "" XID %d: Inval DCOMP XID ent %d",
                gsn_extern_sapi[sapi_ptr->sapi],i);

            return( FALSE );
          }
#else
          {
            uint8             ent_num;
            gsn_dec_t1_ent_t  *dec_ent;
            uint16            app_nsapis;
            dcomp_e_t         *dcomp_e_ptr;

             /* To avoid long lines of code */
            dec_ent = &dec_xid.t1.ent[i];
            ent_num = dec_ent->xid.hdr.ent_num;

            /* Verify that the entity exists */
            if(!sapi_ptr->dcomp_e_pool[ent_num] )
            {
              /* Comp Entity doesn't exist. This is only valid if App NSAPIs
              ** are set to zero as a default responce.
              */
              if( (!dec_xid.t1.ent[i].xid.alg.xid_v42bis.nsapis) )
              {
                continue; /* Get next entity */
              }
              else
              {
                /* Invalid Comp Entity number. Compression entity does not exist.
                */
                MSG_GERAN_LOW_2("GSN "" XID %d: Resp inval DCOMP Entity num %d",
                  gsn_extern_sapi[sapi_ptr->sapi],ent_num);
                return( FALSE );
              }
            }
            dcomp_e_ptr  = sapi_ptr->dcomp_e_pool[ent_num];

            /* Check XID parameter values according to the algorithm
            ** type of the entity requested.
            */
            if( dcomp_e_ptr->t1_alg == T1_ALG_V42BIS )
            {
              /* ---------------- Applicable NSAPIs parameter ---------------*/

              if(  (dec_ent->xid.alg.xid_v42bis.nsapis ^
                    dcomp_e_ptr->alg.xid_v42bis.nsapis) &
                    (dec_ent->xid.alg.xid_v42bis.nsapis) )
              {
                /* One or more App NSAPIs that were set to 0 on the XID request
                ** were set to 1 on the  XID response.
                ** Invalid value of App NSAPIs. Sense of negotiation is
                ** down (each bit separetely).
                */
                MSG_GERAN_LOW_2("GSN "" XID %d: Resp Inval App NSAPIs %d ",
                    gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_v42bis.nsapis);
                return( FALSE );
              }

              if(  dcomp_e_ptr->state == UNASSIGNED )
              {
                /* XID_REQ was sent to the peer to remove comp entity.
                ** If App NSAPIs are not set to 0 in the response
                ** resend XID_REQ to the peer to remove comp entity.
                */
                if( dec_ent->xid.alg.xid_v42bis.nsapis )
                  return( FALSE );
              }

              if( !dec_ent->xid.alg.xid_v42bis.nsapis )
              {
                /* No App NSAPIs are set to "1". Delete the comp entity */
                gsn_del_dcomp_e( gas_id, dcomp_e_ptr->ent_num, sapi_ptr );

                continue; /* Get next entity */
              }
              else
              {
                app_nsapis = ( (dec_ent->xid.alg.xid_v42bis.nsapis ^
                                dcomp_e_ptr->alg.xid_v42bis.nsapis) &
                               (dcomp_e_ptr->alg.xid_v42bis.nsapis)
                             );

                if( app_nsapis )
                {
                  /* The App NSAPIs proposed to use the entity differ to the
                  ** ones suggested by the peer. The App NSAPIs that
                  ** were set from 1 to 0 must stop using the comp entity.
                  */
                  for( i = 0; i < (uint8)GSN_NSAPI_MAX; i++ )
                  {
                    /* Get the first NSAPI mapped on the SAPI */
                    nsapi = nsapis_on_sapi[i];

                    /* Exit the for loop if end of valid NSAPIs */
                    if( nsapi == GSN_INVAL_NSAPI )
                      break;

                    if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapi) ) )
                    {
                      /* Remove comp entity from NSAPI, if it has been using the entity */
                      if( actv_nsapis[ gas_id ][nsapi] )
                        actv_nsapis[ gas_id ][nsapi]->dcomp_e_num[T1_ALG_V42BIS]= NO_COMP_ENT;
                    }
                  }
                }
                /* Update entity's parameter value */
                dcomp_e_ptr->alg.xid_v42bis.nsapis = dec_ent->xid.alg.xid_v42bis.nsapis;
              }

              /* ----------------  "P0" parameter ---------------*/
              if( dec_ent->xid.hdr.length >=
                    (APP_NSAPIS_LEN + V42BIS_P0_LEN) )
              {
                if( dec_ent->xid.alg.xid_v42bis.p0 > dcomp_e_ptr->alg.xid_v42bis.p0 )
                {
                  /* Invalid value of P0. Sense of negotiation is down */
                  MSG_GERAN_LOW_2("GSN "" XID %d: Resp Inval P0 %d ",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_v42bis.p0);
                  return( FALSE );
                }
                /* Accept the value proposed by the peer */
                dcomp_e_ptr->alg.xid_v42bis.p0 = dec_ent->xid.alg.xid_v42bis.p0;
              }

               /* ----------------  "P1" parameter ---------------*/
              if( dec_ent->xid.hdr.length >=
                  (APP_NSAPIS_LEN + V42BIS_P0_LEN + V42BIS_P1_LEN) )
              {
                if( (dec_ent->xid.alg.xid_v42bis.p1 < V42BIS_P1_MIN)   ||
                    (dec_ent->xid.alg.xid_v42bis.p1 >
                        dcomp_e_ptr->alg.xid_v42bis.p1) )
                {
                  /* Invalid value of P1. Sense of negotiation is down */
                  MSG_GERAN_LOW_2("GSN "" XID %d: Resp Inval P1 %d ",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_v42bis.p1);
                  return( FALSE );
                }

                /* Accept the value proposed by the peer */
                dcomp_e_ptr->alg.xid_v42bis.p1 = dec_ent->xid.alg.xid_v42bis.p1;
              }

               /* ----------------  "P2" parameter ---------------*/
              if( dec_ent->xid.hdr.length ==
                  (V42BIS_LEN_P1_MAX - V42BIS_DCOMP_LEN) )
              {
                if( (dec_ent->xid.alg.xid_v42bis.p2 < V42BIS_P2_MIN)   ||
                    (dec_ent->xid.alg.xid_v42bis.p2 >
                        dcomp_e_ptr->alg.xid_v42bis.p2) )
                {
                  /* Invalid value of P2. Sense of negotiation is down */
                  MSG_GERAN_LOW_2("GSN "" XID %d: Resp Inval P2 %d ",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_v42bis.p2);
                  return( FALSE );
                }

                /* Accept the value proposed by the peer */
                dcomp_e_ptr->alg.xid_v42bis.p2 = dec_ent->xid.alg.xid_v42bis.p2;
              }

              /* The Comp entity parameters negotiated by the peer were accepted
              ** by the originator. The NSAPIs can start using the comp entity.
              */
              if( dcomp_e_ptr->state == SELECTED )
              {
                /* Assign DCOMP1 to the algorithm */
                if( (sapi_ptr->dcomp_pool[dcomp_e_ptr->alg.xid_v42bis.dcomp1] != NULL) &&
                    (sapi_ptr->dcomp_pool[dcomp_e_ptr->alg.xid_v42bis.dcomp1]->
                          alg == T1_ALG_V42BIS) )
                {
                  if(sapi_ptr->dcomp_pool[dcomp_e_ptr->alg.xid_v42bis.dcomp1]->
                          state == SELECTED )
                  {
                    sapi_ptr->dcomp_pool
                      [dcomp_e_ptr->alg.xid_v42bis.dcomp1]->state = ASSIGNED;
                  }
                }
                else
                {
                  /* DCOMP1 is assigned to another data comp algorithm.
                  ** It should not be requested for the selected compression
                  ** entity.
                  */
                  MSG_GERAN_ERROR_1("GSN "" XID %d: DCOMP1 requested on different alg ",
                    gsn_extern_sapi[sapi_ptr->sapi]);
                  return( FALSE );
                }

                /* Entity number is assigned and can be used by the App NSAPIs */
                dcomp_e_ptr->state = ASSIGNED;
              }

              /* link App NSAPI to the comp entity
              */
              for( i = 0; i < (uint8)GSN_NSAPI_MAX; i++ )
              {
                /* Get the first NSAPI mapped on the SAPI */
                nsapi = nsapis_on_sapi[i];

                /* Exit the for loop if end of valid NSAPIs */
                if( nsapi == GSN_INVAL_NSAPI )
                  break;

                if( dcomp_e_ptr->alg.xid_v42bis.nsapis &
                    ( 0x1 << GSN_EXTERN_NSAPI(nsapi) ) )
                {
                  if( actv_nsapis[ gas_id ][nsapi] != NULL )
                    actv_nsapis[ gas_id ][nsapi]->dcomp_e_num[T1_ALG_V42BIS] =
                      dcomp_e_ptr->ent_num;
                  else
                    MSG_GERAN_ERROR_1("GSN ""XID: Invalid NSAPI %d",nsapi);
                }
              }

              /* Before start using the entity perform the consistency test*/
              if(!gsn_check_dcomp_e( gas_id, dcomp_e_ptr, sapi_ptr ) )
              {
                MSG_GERAN_ERROR_2("GSN "" XID: Data comp entity %d params not consistant",
                    gsn_extern_sapi[sapi_ptr->sapi], dcomp_e_ptr->ent_num);
                  return( FALSE );
              }

              /* Initialise V42 bis compression entity */
              if( !gsn_v42bis_comp_init( gas_id, dcomp_e_ptr ) )
              {
                /* Heap Exhaustion. Remove compression entity */

                /* Mark entity as unassigned in order to be deleted
                ** after XID_CNF/EST_CNF received
                */
                dcomp_e_ptr->state = UNASSIGNED;

                xid_i.type_field.xid_t1.hdr.ent_num = ent_num;

                /* Do not include Type header. More entities might follow */
                xid_i.length = 0;
                gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_req );

                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
              }
              else
              {
                MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use V42bis ent %d ",
                  gsn_extern_sapi[sapi_ptr->sapi],
                    dcomp_e_ptr->alg.xid_v42bis.nsapis,dcomp_e_ptr->ent_num);
              }

            } /* End processing xid block for V42bis algorithm */
            else
            {
              MSG_GERAN_ERROR_0("GSN ""Invalid T1 alg on selected comp entity ");
            }
          }
#endif /*  FEATURE_GSM_GPRS_SNDCP_DCOMP */

        } /* dec_xid.t1.ent[i].status == XID_VALID || XID_ABSENT */
      } /* for( i = 0; i < dec_xid.t1.ent_in_blk; i++ ) */
    } /* if( dec_xid.t1.status = XID_VALID )            */
    else if ( dec_xid.t1.status == XID_INVALID )
      return( FALSE );

  } /* if( dec_xid.t1.status !=  XID_ABSENT )           */

  /* Find any SELECTED entities that have been implicitly accepted by
  ** the peer (not included in the XID reponse).See TS 04.65 par 6.8.2
  */
#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
  {

    t1_alg_q_t        *q_item;
    /* Change to uint8 when more data compression algorithms added and
    ** T1_ALG_MAX != 0. int8 is used to avoid unassigned comparison with 0 warning.
    */
    int8            alg;

    for( alg = 0; alg <= T1_ALG_MAX; alg++ )
    {
      q_item = q_check( &sapi_ptr->t1_alg_q[alg] );

      /* Find the SELECTED entities that need to be ASSIGNED */
      while( q_item != NULL )
      {
        if( q_item->dcomp_e_ptr->state == SELECTED )
        {
          /* Assign DCOMP values to the algorithm */
          if( (sapi_ptr->dcomp_pool[q_item->dcomp_e_ptr->alg.xid_v42bis.dcomp1] != NULL) )
          {
            if(sapi_ptr->dcomp_pool[q_item->dcomp_e_ptr->alg.xid_v42bis.dcomp1]->
                    state == SELECTED )
            {
              sapi_ptr->dcomp_pool[q_item->dcomp_e_ptr->alg.xid_v42bis.dcomp1]->
                    state = ASSIGNED;
            }
            else
            {
              /* DCOMP value is not SELECTED.
              */
              MSG_GERAN_ERROR_1("GSN "" XID %d: DCOMP values is not SELECTED ",
                gsn_extern_sapi[sapi_ptr->sapi]);
              return( FALSE );
            }

            /* Entity number is assigned and can be used by the App NSAPIs */
            q_item->dcomp_e_ptr->state = ASSIGNED;
          }

          /* link App NSAPI to the comp entity
          */
          for( i = 0; i < (uint8)GSN_NSAPI_MAX; i++ )
          {
            /* Get the first NSAPI mapped on the SAPI */
            nsapi = nsapis_on_sapi[i];

            /* Exit the for loop if end of valid NSAPIs */
            if( nsapi == GSN_INVAL_NSAPI )
              break;

            if( q_item->dcomp_e_ptr->alg.xid_v42bis.nsapis &
                ( 0x1 << GSN_EXTERN_NSAPI(nsapi) ) )
            {
              if( actv_nsapis[ gas_id ][nsapi] != NULL )
                actv_nsapis[ gas_id ][nsapi]->dcomp_e_num[T1_ALG_V42BIS] =
                  q_item->dcomp_e_ptr->ent_num;
              else
                MSG_GERAN_ERROR_1("GSN ""XID: Invalid NSAPI %d",nsapi);
            }
          }

          /* Before start using the entity perform the consistency test*/
          if(!gsn_check_dcomp_e( gas_id, q_item->dcomp_e_ptr, sapi_ptr ) )
          {
            MSG_GERAN_ERROR_2("GSN "" XID: Data comp entity %d params not consistant",
                gsn_extern_sapi[sapi_ptr->sapi], q_item->dcomp_e_ptr->ent_num);
              return( FALSE );
          }

          /* Initialise V42 bis compression entity */
          if( !gsn_v42bis_comp_init( gas_id, q_item->dcomp_e_ptr ) )
          {
            /* Heap Exhaustion. Remove compression entity */

            /* Mark entity as unassigned in order to be deleted
            ** after XID_CNF/EST_CNF received
            */
            q_item->dcomp_e_ptr->state = UNASSIGNED;

            xid_i.type_field.xid_t1.hdr.ent_num = q_item->dcomp_e_ptr->ent_num;

            /* Do not include Type header. More entities might follow */
            xid_i.length = 0;
            gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_req );

            type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
          }
          else
          {
            MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use V42bis ent %d ",
              gsn_extern_sapi[sapi_ptr->sapi],
                q_item->dcomp_e_ptr->alg.xid_v42bis.nsapis,q_item->dcomp_e_ptr->ent_num);
          }
        }
        else if( (q_item->dcomp_e_ptr->state == UNASSIGNED) &&
                 !(dec_xid.t1.ent_map & (0x01 << q_item->dcomp_e_ptr->ent_num)) )
        {
          /* XID_REQ was sent to the peer to remove comp entity.
          ** The peer implicitly accepted by responding with empty XID block.
          ** Remove compression entity.
          */
          gsn_del_dcomp_e( gas_id, q_item->dcomp_e_ptr->ent_num, sapi_ptr);
        }

        /* Get next entity from the queue */
        q_item = q_next( &sapi_ptr->t1_alg_q[alg], &q_item->link );
      } /* end while loop */
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

  /* All T1 entities have been processed. If an XID request
  ** block has been formed, add the type header.
  */
  if( xid_req )
  {
    type_hdr[XID_TYPE_PARAM_HDR_POS] = XID_TYPE_1;
    type_hdr[XID_LEN_PARAM_HDR_POS]  = type_len;
    type_len = 0;

    /* Append type header to the existing XID block */
    if( !gprs_pdu_append_head( &xid_req, type_hdr, XID_TYPE_HDR_LEN) )
    {
      MSG_GERAN_ERROR_1("GSN ""LL %d: Append XID block failed",
        gsn_extern_sapi[sapi_ptr->sapi]);
    }
  }

  /* ------------- Start processing  T0 xid type  --------------------- */
  /* -------------------------------------------------------------------*/
  if( dec_xid.t0.status != XID_ABSENT )
  {
    if( (dec_xid.t0.status == XID_INVALID) ||
        ((dec_xid.t0.status == XID_VALID) &&
         ((dec_xid.t0.ver_num != GSN_VER_NUM_DEF) &&
          (dec_xid.t0.ver_num != GSN_VER_NUM_1  ) )))
    {

      MSG_GERAN_LOW_0("GSN "" XID: Resp Inval len or value of XID T0");
      return( FALSE );
    }
  }
  /* --------------- End processing T0 xid type --------------------------*/

  /* If XID request pdu has been formed, send the SNDCP internal
  ** cmd to initiate the XID request procedure.
  */
  if( xid_req )
  {
    gsn_internal_msg_t *msg_ptr;

    /* Allocate memory to convey SNDCP internal cmd */
    msg_ptr = gsn_int_get_mes_buf();
    GSN_ASSERT( msg_ptr != NULL );

    msg_ptr->cmd_id                  = SEND_XID_REQ;
    msg_ptr->gsn_cmd.xid_req.sapi    = sapi_ptr->sapi;
    msg_ptr->gsn_cmd.xid_req.xid_pdu = xid_req;
    msg_ptr->gsn_cmd.xid_req.gas_id  = gas_id;
    gsn_int_put_mes( msg_ptr );
  }

  return( TRUE );
}

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_REQ_XID()
===
===  DESCRIPTION
===
===   Processes XID block requested (by SGSN) and forms the response
===   (negotiated) XID block.
===
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    Pointer to the XID block that will form the response to the
==     XID block requested
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

dsm_item_type* gsn_process_req_xid
(
  gas_id_t            gas_id,
  gsn_sapi_e_t        *sapi_ptr,  /* SAPI entity in use */
  dsm_item_type       **l3_xid,   /* points to XID requested by SGSN */
  boolean             collision,  /* TRUE in case of collision */
  boolean             xid_on_est_msg /* TRUE if XID requested conveyed on LL_ESTABLISH_IND */
)
{
  dsm_item_type       *xid_rsp = NULL;
  uint8               nsapis_on_sapi[GSN_NSAPI_MAX];
  gsn_dec_xid_t       dec_xid;
  uint8               i;
  gsn_xid_block_t     xid_i;
  uint8               type_hdr[XID_TYPE_HDR_LEN];
  uint8               type_len = 0;

  /* Clear structure that is about to hold the decoded xid parameters */
  memset( &dec_xid, 0, sizeof(gsn_dec_xid_t) );

  /* Get the NSAPIs mapped on the SAPI */
  gsn_get_nsapis_on_sapi( gas_id, sapi_ptr, nsapis_on_sapi );

  /* Print XID parameters in F3 message */
  gsn_print_xid_params ( gas_id, *l3_xid, FALSE );

  if( !gsn_decode_peer_xid( gas_id, l3_xid, &dec_xid, sapi_ptr, FALSE ) )
  {
    /* XID block decoding was unsuccessful. Respond with SNDCP
    ** version number 0, and T1 and T2 length set to 0.
    */
    xid_rsp = gsn_encode_def_xid();

    return( xid_rsp );
  }

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

  if( sapi_ptr->xid_req_dup_ptr && collision )
  {
    /* If collision, free dup dsm item holding the xid requested */
    GPRS_PDU_FREE( &sapi_ptr->xid_req_dup_ptr );
  }
#endif

  /*-------------- Start processing T2 xid type  -------------------------*/
  /* ----------------------------------------------------------------------*/

  if( dec_xid.t2.status !=  XID_ABSENT )
  {
    uint8   xid_ex[XID_T2_LEN_MAX];

    if( (dec_xid.t2.status == XID_INVALID) ||
        (dec_xid.t2.ent_in_blk > ENT_NUM_MAX +1 ) )
    {
      /* Respond with Entity number set to 0 and App NSAPIs set to 0 */
      xid_i.type_field.xid_t2.hdr.ent_num = 0;

      /* Include type header, only one T2 entity */
      xid_i.length = XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
      gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
    }

    if( dec_xid.t2.status == XID_VALID )
    {
      for( i = 0; i < dec_xid.t2.ent_in_blk; i++ )
      {
        if( dec_xid.t2.ent[i].status == XID_INVALID )
        {
          /* Invalid entity. If entity number is decoded, respond
          ** to the specified entity otherwise set entity number
          ** and App NSAPIS to 0 in the response.
          */
          if( dec_xid.t2.ent[i].xid.hdr.ent_num > ENT_NUM_MAX )
            xid_i.type_field.xid_t2.hdr.ent_num = 0;
          else
          {
            xid_i.type_field.xid_t2.hdr.ent_num =
                dec_xid.t2.ent[i].xid.hdr.ent_num;

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

            /* If entity exists, delete comp entity */
            if( sapi_ptr->pcomp_e_pool[dec_xid.t2.ent[i].xid.hdr.ent_num] != NULL )
              gsn_del_pcomp_e( gas_id, dec_xid.t2.ent[i].xid.hdr.ent_num, sapi_ptr);

#endif
          }

          /* Do not include Type header. More entities might follow */
          xid_i.length = 0;
          gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

          type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

          continue;
        }

        if( dec_xid.t2.ent[i].status == XID_VALID ||
            dec_xid.t2.ent[i].status == XID_ABSENT )
        {

#ifndef FEATURE_GSM_GPRS_SNDCP_PCOMP

          /* PCI compression is not supported. If entity number is
          ** decoded, respond to the specified entity, otherwise set
          ** entity number and App NSAPIS to 0 in the response.
          */
          if( dec_xid.t2.ent[i].xid.hdr.ent_num > ENT_NUM_MAX )
            xid_i.type_field.xid_t2.hdr.ent_num = 0;
          else
          {
            xid_i.type_field.xid_t2.hdr.ent_num =
                dec_xid.t2.ent[i].xid.hdr.ent_num;
           }

          /* Do not include Type header. More entities might follow */
          xid_i.length = 0;
          gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

          type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

          continue;
#else
          {
            uint8             ent_num;
            gsn_dec_t2_ent_t  *dec_ent;
            uint8             idx;
            uint16            app_nsapis;
            gsn_nsapi_t       nsapis[GSN_NSAPI_MAX];
            uint8             xid_neg_len;
            boolean           ack_nsapis   = FALSE;
            boolean           unack_nsapis = FALSE;
            boolean           ent_exist    = FALSE;
            boolean           rmv_ent      = FALSE;

            /* To avoid long lines of code */
            dec_ent = &dec_xid.t2.ent[i];
            ent_num = dec_ent->xid.hdr.ent_num;

            /* Set the header of the compression field for the response */
            xid_i.type = XID_TYPE_2;
            xid_i.type_field.xid_t2.hdr.p = 0; /* Always 0 in a response */
            xid_i.type_field.xid_t2.hdr.ent_num  = ent_num;

            /* Algorithm type is not included in the response but is needed
            ** for PCI entity initialisation.
            */
            xid_i.type_field.xid_t2.hdr.alg_type = dec_ent->xid.hdr.alg_type;

            /* Fill in the Spare bits */
            xid_i.type_field.xid_t2.hdr.x1 = 0;
            xid_i.type_field.xid_t2.hdr.x2 = 0;

            /* In the case of a collision (see TS 04.65 par 6.5.1.1.3)
            ** --------------------------------------------------------
            */
            if( collision )
            {
              if( sapi_ptr->pcomp_e_pool[ent_num] != NULL )
              {
                if( sapi_ptr->pcomp_e_pool[ent_num]->state == SELECTED )
                {
                  /* Entity number is currently selected. If P=0, assume
                  ** that the peer SNDCP entity agreed to the creation of
                  ** the proposed entity and the response from the peer was lost.
                  */
                  if( dec_ent->xid.hdr.p == 0 )
                  {
                    sapi_ptr->pcomp_e_pool[ent_num]->state = ASSIGNED;

                    if( sapi_ptr->pcomp_e_pool[ent_num]->t2_alg == T2_ALG_RFC1144 )
                    {
                      if( (sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                              alg.xid_rfc1144.pcomp1] != NULL) &&
                          (sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                              alg.xid_rfc1144.pcomp2] != NULL)  )
                      {
                        sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                          alg.xid_rfc1144.pcomp1]->state = ASSIGNED;

                        sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                          alg.xid_rfc1144.pcomp2]->state = ASSIGNED;
                      }
                      else
                      {
                        MSG_GERAN_ERROR_3("GSN "" Inval PCOMP1 %d, PCOMP2 %d",
                          sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.pcomp1,
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.pcomp2,
                        0
                        );
                      }
                      /* If the XID requested contains one or more XID params,
                      ** that are not negotiated as part of the collision resolution,
                      ** then the negotiation of these XID params shall be performed
                      ** after conclution of the collision resolution. TBD!
                      */
                      gsn_get_app_nsapis( nsapis,
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.nsapis);

                      /* Link NSAPIs and comp entity */
                      for( idx=0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
                      {
                        if( (uint8)nsapis[idx] == GSN_NSAPI_MAX )
                          break;

                        if( actv_nsapis[ gas_id ][nsapis[idx]] )/* NSAPIs can use the comp entity */
                          actv_nsapis[ gas_id ][nsapis[idx]]->pcomp_e_num[T2_ALG_RFC1144] = ent_num;
                      }
                    }
                    else if( sapi_ptr->pcomp_e_pool[ent_num]->t2_alg == T2_ALG_RFC2507 )
                    {
                      if( (sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                              alg.xid_rfc2507.pcomp1] != NULL) &&
                          (sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                              alg.xid_rfc2507.pcomp2] != NULL) &&
                          (sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                              alg.xid_rfc2507.pcomp3] != NULL) &&
                          (sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                              alg.xid_rfc2507.pcomp4] != NULL) &&
                          (sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                              alg.xid_rfc2507.pcomp5] != NULL))
                      {
                        sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                          alg.xid_rfc2507.pcomp1]->state = ASSIGNED;

                        sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                          alg.xid_rfc2507.pcomp2]->state = ASSIGNED;

                        sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                          alg.xid_rfc2507.pcomp3]->state = ASSIGNED;

                        sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                          alg.xid_rfc2507.pcomp4]->state = ASSIGNED;

                        sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                          alg.xid_rfc2507.pcomp5]->state = ASSIGNED;
                      }
                      else
                      {
                        MSG_GERAN_ERROR_3("GSN "" Inval PCOMP1 %d, PCOMP2 %d PCOMP3 %d",
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp1,
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp2,
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp3
                        );

                        MSG_GERAN_ERROR_3("GSN "" Inval PCOMP4 %d, PCOMP5 %d",
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp4,
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp5,
                        0
                        );
                      }
                      /* If the XID requested contains one or more XID params,
                      ** that are not negotiated as part of the collision resolution,
                      ** then the negotiation of these XID params shall be performed
                      ** after conclution of the collision resolution. TBD!
                      */
                      gsn_get_app_nsapis( nsapis,
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.nsapis);

                      /* Link NSAPIs and comp entity */
                      for( idx=0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
                      {
                        if( (uint8)nsapis[idx] == GSN_NSAPI_MAX )
                          break;

                        if( actv_nsapis[ gas_id ][nsapis[idx]] )/* NSAPIs can use the comp entity */
                          actv_nsapis[ gas_id ][nsapis[idx]]->pcomp_e_num[T2_ALG_RFC2507] = ent_num;
                      }
                    }
                    else if( sapi_ptr->pcomp_e_pool[ent_num]->t2_alg == T2_ALG_RFC3095 )
                    {
                      if( (sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                              alg.xid_rfc3095.pcomp1] != NULL) &&
                          (sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                              alg.xid_rfc3095.pcomp2] != NULL)  )
                      {
                        sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                          alg.xid_rfc3095.pcomp1]->state = ASSIGNED;

                        sapi_ptr->pcomp_pool[sapi_ptr->pcomp_e_pool[ent_num]->
                          alg.xid_rfc3095.pcomp2]->state = ASSIGNED;
                      }
                      else
                      {
                        MSG_GERAN_ERROR_3("GSN "" Inval PCOMP1 %d, PCOMP2 %d",
                          sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.pcomp1,
                          sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.pcomp2,
                          0);
                      }
                      /* If the XID requested contains one or more XID params,
                      ** that are not negotiated as part of the collision resolution,
                      ** then the negotiation of these XID params shall be performed
                      ** after conclution of the collision resolution. TBD!
                      */
                      gsn_get_app_nsapis( nsapis,
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.nsapis);

                      /* Link NSAPIs and comp entity */
                      for( idx=0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
                      {
                        if( (uint8)nsapis[idx] == GSN_NSAPI_MAX )
                          break;

                        if( actv_nsapis[ gas_id ][nsapis[idx]] )/* NSAPIs can use the comp entity */
                          actv_nsapis[ gas_id ][nsapis[idx]]->pcomp_e_num[T2_ALG_RFC3095] = ent_num;
                      }
                    }
                    else
                    {
                      MSG_GERAN_ERROR_3("GSN "" Invalid t2 alg %d",
                                      sapi_ptr->pcomp_e_pool[ent_num]->t2_alg,
                                      0,
                                      0);
                    }
                  }
                  else  /* p == 1 */
                  {
                    /* Dealocate memory for comp entity and PCOMP value */
                    gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr );

                    /* The originally proposed entity shall be proposed again. TBD */
                  }
                }
                else if( sapi_ptr->pcomp_e_pool[ent_num]->state == ASSIGNED )
                {
                  if( dec_ent->xid.hdr.p == 1 )
                  {
                    /* Entity number is currently assigned and P=1.
                    ** Delete original entity.
                    */
                    gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr );
                  }
                }
              }
              else
              {
                if( dec_ent->xid.hdr.p == 0 )
                {
                  /* Unassigned entity number with P = 0.
                  ** Respond with App NSAPI set to 0.
                  ** Do not include Type header. More entities might follow */
                  xid_i.length = 0;
                  gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                  type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                  continue;
                }

                /* In the case of a collision in which a PCOMP value is assigned
                ** to a comp algorithm, if the peer proposes a new comp entity
                ** with the same PCOMP value assigned to a different algorithm, then all
                ** comp entities using the algorithm shall be deleted.
                */
                if(dec_ent->xid.hdr.alg_type == T2_ALG_RFC1144)
                {
                  if((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc1144.pcomp1] != NULL)           &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc1144.pcomp1]->state == ASSIGNED)&&
                     (dec_ent->xid.hdr.p == 1)                                 &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc1144.pcomp1]->val_num ==
                       dec_ent->xid.alg.xid_rfc1144.pcomp1)                    &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc1144.pcomp1]->alg    !=
                       dec_ent->xid.hdr.alg_type) )
                  {
                    gsn_del_pcomp_e_mapped_to_alg( gas_id,
                                                   sapi_ptr,
                                                   sapi_ptr->pcomp_pool
                                                   [dec_ent->xid.alg.xid_rfc1144.pcomp1]->alg );
                  }

                  if((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc1144.pcomp2] != NULL)            &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc1144.pcomp2]->state == ASSIGNED) &&
                     (dec_ent->xid.hdr.p == 1)                                  &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc1144.pcomp2]->val_num ==
                       dec_ent->xid.alg.xid_rfc1144.pcomp2)                     &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc1144.pcomp2]->alg    !=
                       dec_ent->xid.hdr.alg_type) )
                  {
                    gsn_del_pcomp_e_mapped_to_alg( gas_id,
                                                   sapi_ptr,
                                                   sapi_ptr->pcomp_pool
                                                   [dec_ent->xid.alg.xid_rfc1144.pcomp2]->alg );
                  }
                }
                else if(dec_ent->xid.hdr.alg_type == T2_ALG_RFC2507)
                {
                  if((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp1] != NULL)            &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp1]->state == ASSIGNED) &&
                     (dec_ent->xid.hdr.p == 1)                                  &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp1]->val_num ==
                      dec_ent->xid.alg.xid_rfc2507.pcomp1)                       &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp1]->alg    !=
                      dec_ent->xid.hdr.alg_type) )
                  {
                    gsn_del_pcomp_e_mapped_to_alg( gas_id,
                                                   sapi_ptr,
                                                   sapi_ptr->pcomp_pool
                                                   [dec_ent->xid.alg.xid_rfc2507.pcomp1]->alg );
                  }

                  if((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp2] != NULL)            &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp2]->state == ASSIGNED) &&
                     (dec_ent->xid.hdr.p == 1)                                  &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp2]->val_num ==
                       dec_ent->xid.alg.xid_rfc2507.pcomp2)                     &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp2]->alg    !=
                       dec_ent->xid.hdr.alg_type) )
                  {
                    gsn_del_pcomp_e_mapped_to_alg( gas_id, sapi_ptr,
                                   sapi_ptr->pcomp_pool
                                   [dec_ent->xid.alg.xid_rfc2507.pcomp2]->alg );
                  }

                  if((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp3] != NULL)            &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp3]->state == ASSIGNED) &&
                     (dec_ent->xid.hdr.p == 1)                                  &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp3]->val_num ==
                       dec_ent->xid.alg.xid_rfc2507.pcomp3)                     &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp3]->alg    !=
                       dec_ent->xid.hdr.alg_type) )
                  {
                    gsn_del_pcomp_e_mapped_to_alg( gas_id, sapi_ptr,
                                   sapi_ptr->pcomp_pool
                                   [dec_ent->xid.alg.xid_rfc2507.pcomp3]->alg );
                  }

                  if((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp4] != NULL)            &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp4]->state == ASSIGNED) &&
                     (dec_ent->xid.hdr.p == 1)                                  &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp4]->val_num ==
                       dec_ent->xid.alg.xid_rfc2507.pcomp4)                     &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp4]->alg    !=
                       dec_ent->xid.hdr.alg_type) )
                  {
                    gsn_del_pcomp_e_mapped_to_alg( gas_id, sapi_ptr,
                                   sapi_ptr->pcomp_pool
                                   [dec_ent->xid.alg.xid_rfc2507.pcomp4]->alg );
                  }

                  if((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp5] != NULL)            &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp5]->state == ASSIGNED) &&
                     (dec_ent->xid.hdr.p == 1)                                  &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp5]->val_num ==
                       dec_ent->xid.alg.xid_rfc2507.pcomp5)                     &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp5]->alg    !=
                       dec_ent->xid.hdr.alg_type) )
                  {
                    gsn_del_pcomp_e_mapped_to_alg( gas_id, sapi_ptr,
                                   sapi_ptr->pcomp_pool
                                   [dec_ent->xid.alg.xid_rfc2507.pcomp5]->alg );
                  }
                }
                else if(dec_ent->xid.hdr.alg_type == T2_ALG_RFC3095)
                {
                  if((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp1] != NULL)            &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp1]->state == ASSIGNED)  &&
                     (dec_ent->xid.hdr.p == 1)                                  &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp1]->val_num ==
                      dec_ent->xid.alg.xid_rfc3095.pcomp1)                       &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp1]->alg    !=
                      dec_ent->xid.hdr.alg_type) )
                  {
                    gsn_del_pcomp_e_mapped_to_alg(gas_id,
                                   sapi_ptr,
                                   sapi_ptr->pcomp_pool
                                  [dec_ent->xid.alg.xid_rfc3095.pcomp1]->alg );
                  }

                  if((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp2] != NULL)            &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp2]->state == ASSIGNED) &&
                     (dec_ent->xid.hdr.p == 1)                                  &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp2]->val_num ==
                       dec_ent->xid.alg.xid_rfc3095.pcomp2)                     &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp2]->alg    !=
                       dec_ent->xid.hdr.alg_type) )
                  {
                    gsn_del_pcomp_e_mapped_to_alg( gas_id, sapi_ptr,
                                   sapi_ptr->pcomp_pool
                                   [dec_ent->xid.alg.xid_rfc3095.pcomp2]->alg );
                  }
                }
                else
                {
                  MSG_GERAN_ERROR_3("GSN "" Invalid t2 alg %d",
                                      sapi_ptr->pcomp_e_pool[ent_num]->t2_alg,
                                      0,
                                      0);
                }
              }
            }
            /*-------------- End of collision handling ---------------------------*/

            /* Exception Handling. See TS 04.65 par 6.8.3 */
            /* -----------------------------------------------------------------*/

            /* If P bit is not set, then the XID parameters that follow shall
            ** refer to  an existing entity. Respond with App NSAPIs
            ** set to 0 if entity doesn't exist or if T1 algorithm is not supported
            */
            if( !dec_ent->xid.hdr.p  )
            {
              if( sapi_ptr->pcomp_e_pool[ent_num] == NULL )
              {
                /* Do not include Type header. More entities might follow */
                xid_i.length = 0;
                gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                MSG_GERAN_MED_0("GSN ""XID req: P=0 for unassigned entity");

                continue;
              }
            }

            if( dec_ent->xid.hdr.p && dec_ent->xid.hdr.length )
            {
              if( (sapi_ptr->pcomp_e_pool[ent_num] != NULL) &&
                  (sapi_ptr->pcomp_e_pool[ent_num]->state == ASSIGNED) )
              {
                /* Assigned entity with P=1. Ignore algorithm and PCOMP values if
                ** the same as the previously-assined values. Otherwise,
                ** respond to XID request with "App NSAPIs" field set to zero and send
                ** SNSM_STATUS.REQ with cause "invalid XID command" to SM.
                ** SM shall deactivate all PDP contexts for this SAPI.
                */
                if(sapi_ptr->pcomp_e_pool[ent_num]->t2_alg == T2_ALG_RFC1144)
                {
                  if((sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.pcomp1 !=
                      dec_ent->xid.alg.xid_rfc1144.pcomp1)                        ||
                    (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.pcomp2 !=
                      dec_ent->xid.alg.xid_rfc1144.pcomp2)                        ||
                    (sapi_ptr->pcomp_e_pool[ent_num]->t2_alg !=
                      dec_ent->xid.hdr.alg_type ) )
                  {
                    /* Last entity. Include type header */
                    xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                    gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                    /* Delete comp entity */
                    gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr);

                    /* Send SNSM_STATUS.REQ to SM */
                    gsn_send_snsm_stat_req( gas_id,
                                            sapi_ptr->sapi,
                                            sapi_ptr->tlli,
                                            (uint8) SNSM_INVALID_XID_RESP );

                    return( xid_rsp );
                  }
                }
                else if(sapi_ptr->pcomp_e_pool[ent_num]->t2_alg == T2_ALG_RFC2507)
                {
                  if((sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp1 !=
                      dec_ent->xid.alg.xid_rfc2507.pcomp1)                        ||
                     (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp2 !=
                      dec_ent->xid.alg.xid_rfc2507.pcomp2)                        ||
                     (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp3 !=
                      dec_ent->xid.alg.xid_rfc2507.pcomp3)                        ||
                     (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp4 !=
                      dec_ent->xid.alg.xid_rfc2507.pcomp4)                        ||
                     (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp5 !=
                      dec_ent->xid.alg.xid_rfc2507.pcomp5)                        ||
                     (sapi_ptr->pcomp_e_pool[ent_num]->t2_alg !=
                      dec_ent->xid.hdr.alg_type )
                  )
                  {
                    /* Last entity. Include type header */
                    xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                    gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                    /* Delete comp entity */
                    gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr);

                    /* Send SNSM_STATUS.REQ to SM */
                    gsn_send_snsm_stat_req( gas_id,
                                            sapi_ptr->sapi,
                                            sapi_ptr->tlli,
                                            (uint8) SNSM_INVALID_XID_RESP );

                    return( xid_rsp );
                  }
                }
                else if(sapi_ptr->pcomp_e_pool[ent_num]->t2_alg == T2_ALG_RFC3095)
                {
                  if((sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.pcomp1 !=
                      dec_ent->xid.alg.xid_rfc3095.pcomp1)                        ||
                     (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.pcomp2 !=
                      dec_ent->xid.alg.xid_rfc3095.pcomp2)                        ||
                     (sapi_ptr->pcomp_e_pool[ent_num]->t2_alg !=
                      dec_ent->xid.hdr.alg_type ) )
                  {
                    /* Last entity. Include type header */
                    xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                    gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                    /* Delete comp entity */
                    gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr);

                    /* Send SNSM_STATUS.REQ to SM */
                    gsn_send_snsm_stat_req( gas_id,
                                            sapi_ptr->sapi,
                                            sapi_ptr->tlli,
                                            (uint8) SNSM_INVALID_XID_RESP );

                    return( xid_rsp );
                  }
                }
                else
                {
                  MSG_GERAN_ERROR_3("GSN "" Invalid t2 alg %d",
                                      sapi_ptr->pcomp_e_pool[ent_num]->t2_alg,
                                      0,
                                      0);
                }
              }


              if(dec_ent->xid.hdr.alg_type == T2_ALG_RFC1144)
              {
                 /* If PCOMP specified is already assigned to a different comp
                 ** algorithm, then, respond to XID request with App NSAPIs"
                 ** field set to zero and send SNSM_STATUS.REQ with cause
                 ** "invalid XID command" to SM. SM shall deactivate all PDP
                 ** contexts for this SAPI.
                 */

                if( ( ( sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc1144.pcomp1] != NULL) &&
                    ( sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc1144.pcomp1]->alg !=
                          dec_ent->xid.hdr.alg_type) )                    ||
                  ( ( sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc1144.pcomp2] != NULL) &&
                    ( sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc1144.pcomp2]->alg !=
                          dec_ent->xid.hdr.alg_type) ) )

                {
                  /* Last entity. Include type header */
                  xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                  gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                  /* If entity exists, delete comp entity */
                  if( sapi_ptr->pcomp_e_pool[ent_num] != NULL )
                    gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr);

                  /* Send SNSM_STATUS.REQ to SM */
                  gsn_send_snsm_stat_req( gas_id,
                                          sapi_ptr->sapi,
                                          sapi_ptr->tlli,
                                          (uint8) SNSM_INVALID_XID_RESP );

                  return( xid_rsp );
                }

                /* If one or more new PCOMP values are specified for an existing
                 ** comp algorithm, then respond to XID request with App NSAPIs"
                 ** field set to zero and send SNSM_STATUS.REQ with cause
                 ** "invalid XID command" to SM.SM shall deactivate all PDP
                 ** contexts for this SAPI.
                 */
                if( ( (sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc1144.pcomp1] == NULL) &&
                    (sapi_ptr->pcomp_e_pool[ent_num] != NULL )        &&
                    (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.pcomp1 !=
                      dec_ent->xid.alg.xid_rfc1144.pcomp1) )                    ||
                  ( (sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc1144.pcomp2] == NULL) &&
                    (sapi_ptr->pcomp_e_pool[ent_num] != NULL )        &&
                    (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.pcomp2 !=
                      dec_ent->xid.alg.xid_rfc1144.pcomp2) ) )
                {
                  /* Last entity. Include type header */
                  xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                  gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                  /* If entity exists, delete comp entity */
                  if( sapi_ptr->pcomp_e_pool[ent_num] != NULL )
                   gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr);

                  /* Send SNSM_STATUS.REQ to SM */
                  gsn_send_snsm_stat_req( gas_id,
                                          sapi_ptr->sapi,
                                          sapi_ptr->tlli,
                                          (uint8) SNSM_INVALID_XID_RESP );

                  return( xid_rsp );
                }

                /* Check if PCOMP values already exist
                 */
                if( ((sapi_ptr->pcomp_pool
                    [dec_ent->xid.alg.xid_rfc1144.pcomp1] != NULL) &&
                   (sapi_ptr->pcomp_pool
                    [dec_ent->xid.alg.xid_rfc1144.pcomp1]->state == SELECTED) ) ||
                  ((sapi_ptr->pcomp_pool
                    [dec_ent->xid.alg.xid_rfc1144.pcomp2] != NULL) &&
                   (sapi_ptr->pcomp_pool
                    [dec_ent->xid.alg.xid_rfc1144.pcomp2]->state == SELECTED) ) )
                {
                  /* After collision reslolution there shouldn't be any SELECTED DCOMP values */
                  MSG_GERAN_ERROR_1("GSN ""LL %d: PCOMP val SELECTED after collision res",
                        gsn_extern_sapi[sapi_ptr->sapi]);
                }

                /* PCOMP values are not included in the response but are needed
                 ** for PCI entity initialisation.
                 */
                xid_i.type_field.xid_t2.alg.xid_rfc1144.pcomp1 =
                                  dec_ent->xid.alg.xid_rfc1144.pcomp1;
                xid_i.type_field.xid_t2.alg.xid_rfc1144.pcomp2 =
                                  dec_ent->xid.alg.xid_rfc1144.pcomp2;
              }

              else if(dec_ent->xid.hdr.alg_type == T2_ALG_RFC2507)
              {
                 /* If PCOMP specified is already assigned to a different comp
                 ** algorithm, then, respond to XID request with App NSAPIs"
                 ** field set to zero and send SNSM_STATUS.REQ with cause
                 ** "invalid XID command" to SM. SM shall deactivate all PDP
                 ** contexts for this SAPI.
                 */

                if( ( ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc2507.pcomp1] != NULL) &&
                       ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc2507.pcomp1]->alg !=
                         dec_ent->xid.hdr.alg_type) )                    ||
                    ( ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc2507.pcomp2] != NULL) &&
                       ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc2507.pcomp2]->alg !=
                         dec_ent->xid.hdr.alg_type) )                     ||
                    ( ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc2507.pcomp3] != NULL) &&
                       ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc2507.pcomp3]->alg !=
                         dec_ent->xid.hdr.alg_type) )                     ||
                    ( ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc2507.pcomp4] != NULL) &&
                       ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc2507.pcomp4]->alg !=
                         dec_ent->xid.hdr.alg_type) )                     ||
                    ( ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc2507.pcomp5] != NULL) &&
                       ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc2507.pcomp5]->alg !=
                         dec_ent->xid.hdr.alg_type) ))
                {
                  /* Last entity. Include type header */
                  xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                  gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                  /* If entity exists, delete comp entity */
                  if( sapi_ptr->pcomp_e_pool[ent_num] != NULL )
                    gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr);

                  /* Send SNSM_STATUS.REQ to SM */
                  gsn_send_snsm_stat_req( gas_id,
                                          sapi_ptr->sapi,
                                          sapi_ptr->tlli,
                                          (uint8) SNSM_INVALID_XID_RESP );

                  return( xid_rsp );
                }

                /* If one or more new PCOMP values are specified for an existing
                ** comp algorithm, then respond to XID request with App NSAPIs"
                ** field set to zero and send SNSM_STATUS.REQ with cause
                ** "invalid XID command" to SM. SM shall deactivate all PDP
                ** contexts for this SAPI.
                */
                if( ( (sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc2507.pcomp1] == NULL) &&
                      (sapi_ptr->pcomp_e_pool[ent_num] != NULL )        &&
                      (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp1 !=
                       dec_ent->xid.alg.xid_rfc2507.pcomp1) )                    ||
                    ( (sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc2507.pcomp2] == NULL) &&
                      (sapi_ptr->pcomp_e_pool[ent_num] != NULL )        &&
                      (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp2 !=
                       dec_ent->xid.alg.xid_rfc2507.pcomp2) )                     ||
                    ( (sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc2507.pcomp3] == NULL) &&
                      (sapi_ptr->pcomp_e_pool[ent_num] != NULL )        &&
                      (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp3 !=
                       dec_ent->xid.alg.xid_rfc2507.pcomp3) )                     ||
                    ( (sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc2507.pcomp4] == NULL) &&
                      (sapi_ptr->pcomp_e_pool[ent_num] != NULL )        &&
                      (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp4 !=
                       dec_ent->xid.alg.xid_rfc2507.pcomp4) )                     ||
                    ( (sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc2507.pcomp5] == NULL) &&
                      (sapi_ptr->pcomp_e_pool[ent_num] != NULL )        &&
                      (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.pcomp5 !=
                       dec_ent->xid.alg.xid_rfc2507.pcomp5) ))
                {
                  /* Last entity. Include type header */
                  xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                  gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                  /* If entity exists, delete comp entity */
                  if( sapi_ptr->pcomp_e_pool[ent_num] != NULL )
                    gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr);

                  /* Send SNSM_STATUS.REQ to SM */
                  gsn_send_snsm_stat_req( gas_id,
                                          sapi_ptr->sapi,
                                          sapi_ptr->tlli,
                                          (uint8) SNSM_INVALID_XID_RESP );

                  return( xid_rsp );
                }

                /* Check if PCOMP values already exist
                 */
                if( ((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp1] != NULL) &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp1]->state == SELECTED) ) ||
                    ((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp2] != NULL) &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp2]->state == SELECTED) ) ||
                    ((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp3] != NULL) &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp3]->state == SELECTED) ) ||
                    ((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp4] != NULL) &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp4]->state == SELECTED) ) ||
                    ((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp5] != NULL) &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc2507.pcomp5]->state == SELECTED) ) )
                {
                  /* After collision reslolution there shouldn't be any SELECTED DCOMP values */
                  MSG_GERAN_ERROR_1("GSN ""LL %d: PCOMP val SELECTED after collision res",
                        gsn_extern_sapi[sapi_ptr->sapi]);
                }

                /* PCOMP values are not included in the response but are needed
                 ** for PCI entity initialisation.
                 */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.pcomp1 =
                                  dec_ent->xid.alg.xid_rfc2507.pcomp1;
                xid_i.type_field.xid_t2.alg.xid_rfc2507.pcomp2 =
                                  dec_ent->xid.alg.xid_rfc2507.pcomp2;
                xid_i.type_field.xid_t2.alg.xid_rfc2507.pcomp3 =
                                  dec_ent->xid.alg.xid_rfc2507.pcomp3;
                xid_i.type_field.xid_t2.alg.xid_rfc2507.pcomp4 =
                                  dec_ent->xid.alg.xid_rfc2507.pcomp4;
                xid_i.type_field.xid_t2.alg.xid_rfc2507.pcomp5 =
                                  dec_ent->xid.alg.xid_rfc2507.pcomp5;
              }
              else if(dec_ent->xid.hdr.alg_type == T2_ALG_RFC3095)
              {
                 /* If PCOMP specified is already assigned to a different comp
                 ** algorithm, then, respond to XID request with App NSAPIs"
                 ** field set to zero and send SNSM_STATUS.REQ with cause
                 ** "invalid XID command" to SM. SM shall deactivate all PDP
                 ** contexts for this SAPI.
                 */

                if( ( ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc3095.pcomp1] != NULL) &&
                       ( sapi_ptr->pcomp_pool
                        [dec_ent->xid.alg.xid_rfc3095.pcomp1]->alg !=
                         dec_ent->xid.hdr.alg_type) )                    ||
                  ( ( sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc3095.pcomp2] != NULL) &&
                    ( sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc3095.pcomp2]->alg !=
                          dec_ent->xid.hdr.alg_type) ) )

                {
                  /* Last entity. Include type header */
                  xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                  gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                  /* If entity exists, delete comp entity */
                  if( sapi_ptr->pcomp_e_pool[ent_num] != NULL )
                    gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr);

                  /* Send SNSM_STATUS.REQ to SM */
                  gsn_send_snsm_stat_req( gas_id,
                                          sapi_ptr->sapi,
                                          sapi_ptr->tlli,
                                          (uint8) SNSM_INVALID_XID_RESP );

                  return( xid_rsp );
                }

                /* If one or more new PCOMP values are specified for an existing
                ** comp algorithm, then respond to XID request with App NSAPIs"
                ** field set to zero and send SNSM_STATUS.REQ with cause
                ** "invalid XID command" to SM.SM shall deactivate all PDP
                ** contexts for this SAPI.
                */
                if( ( (sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc3095.pcomp1] == NULL) &&
                      (sapi_ptr->pcomp_e_pool[ent_num] != NULL )        &&
                      (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.pcomp1 !=
                       dec_ent->xid.alg.xid_rfc3095.pcomp1) )                    ||
                    ( (sapi_ptr->pcomp_pool
                       [dec_ent->xid.alg.xid_rfc3095.pcomp2] == NULL) &&
                      (sapi_ptr->pcomp_e_pool[ent_num] != NULL )        &&
                      (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.pcomp2 !=
                       dec_ent->xid.alg.xid_rfc3095.pcomp2) ) )
                {
                  /* Last entity. Include type header */
                  xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                  gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                  /* If entity exists, delete comp entity */
                  if( sapi_ptr->pcomp_e_pool[ent_num] != NULL )
                    gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr);

                  /* Send SNSM_STATUS.REQ to SM */
                  gsn_send_snsm_stat_req( gas_id,
                                          sapi_ptr->sapi,
                                          sapi_ptr->tlli,
                                          (uint8) SNSM_INVALID_XID_RESP );

                  return( xid_rsp );
                }

                /* Check if PCOMP values already exist
                 */
                if( ((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp1] != NULL) &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp1]->state == SELECTED) ) ||
                    ((sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp2] != NULL) &&
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc3095.pcomp2]->state == SELECTED) ) )
                {
                  /* After collision reslolution there shouldn't be any SELECTED DCOMP values */
                  MSG_GERAN_ERROR_1("GSN ""LL %d: PCOMP val SELECTED after collision res",
                        gsn_extern_sapi[sapi_ptr->sapi]);
                }

                /* PCOMP values are not included in the response but are needed
                 ** for PCI entity initialisation.
                 */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.pcomp1 =
                                  dec_ent->xid.alg.xid_rfc3095.pcomp1;
                xid_i.type_field.xid_t2.alg.xid_rfc3095.pcomp2 =
                                  dec_ent->xid.alg.xid_rfc3095.pcomp2;
              }
              else
              {
                MSG_GERAN_ERROR_3("GSN ""Invalid t2 alg %d",
                             sapi_ptr->pcomp_e_pool[ent_num]->t2_alg,
                             0,
                             0);
              }
            }


            /* ---------- End of exception handling --------------------------*/

            /* Check if compression entity already exist
            */
            if( (sapi_ptr->pcomp_e_pool[ent_num]        != NULL)     &&
                (sapi_ptr->pcomp_e_pool[ent_num]->state == ASSIGNED) )
            {
              /* Compression entity exists. */
              ent_exist = TRUE;
            }
            else if( (sapi_ptr->pcomp_e_pool[ent_num]          != NULL)     &&
                     (sapi_ptr->pcomp_e_pool[ent_num]->state   == SELECTED) )
            {
              /* After collision reslolution there shouldn't be any SELECTED entities */
              MSG_GERAN_ERROR_1("GSN ""LL %d: Ent SELECTED after collision res",
                      gsn_extern_sapi[sapi_ptr->sapi]);
            }

            /* Process the parameters included in the decoded compression field */


            if(dec_ent->xid.hdr.alg_type == T2_ALG_RFC1144)
            {
              /* ---------------------- Applicable NSAPIs ------------------------*/

              /* If App NSAPIs is 0. Delete compression entity */
              if( ent_exist && !dec_ent->xid.alg.xid_rfc1144.nsapis )
              {
                /* Not the last entity. Do not Include type header */
                xid_i.length = 0;
                gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr );

                continue;
              }

              /* The response will be the same if no exception occurs */
              xid_i.type_field.xid_t2.alg.xid_rfc1144.nsapis =
                            dec_ent->xid.alg.xid_rfc1144.nsapis;

              /* Increment comp entity's parameter length so that
               ** the App NSAPIs field will be included in the XID response
               ** Note that in the response the Propose bit (P) is set to 0.
               */
              xid_i.type_field.xid_t2.hdr.length = APP_NSAPIS_LEN;

              /* Check if any NSAPIs have been removed from an existing comp entity
               */
              if( ent_exist )
              {
                app_nsapis = (dec_ent->xid.alg.xid_rfc1144.nsapis^
                            sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.nsapis) &
                            sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.nsapis;

                /* LL_ESTABLISH primitive shall be used if one or more NSAPIs are
                ** removed from existing comp entities used with ack NSAPIs, except
                ** when all NSAPIs using the comp entity are removed,
                ** or LLC is already in ADM.
                */
                if( app_nsapis )
                {
                  if( (sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK) &&
                    (xid_on_est_msg == FALSE) &&
                    ( (dec_ent->xid.alg.xid_rfc1144.nsapis != 0) ||
                      (sapi_ptr->state != IDLE) ) )
                  {
                    /* Negotiation is not possible. Respond with the
                     ** priviously-negotiated App NSAPIs value
                     */
                    xid_i.type_field.xid_t2.alg.xid_rfc1144.nsapis =
                    sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.nsapis;
                  }
                  else
                  {
                    for( idx = 0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
                    {
                      /* Exit the for loop if end of valid NSAPIs */
                      if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                        break;

                      if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx]) ) )
                      {
                        /* Remove comp entity from NSAPI if it has been using the entity */
                        if( actv_nsapis[ gas_id ][nsapis_on_sapi[idx]] )
                          actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                           pcomp_e_num[T2_ALG_RFC1144]= NO_COMP_ENT;
                      }
                    }
                  }
                }

                /* Check if any NSAPIs have been added to the existing comp entity
                 */
                app_nsapis = (dec_ent->xid.alg.xid_rfc1144.nsapis ^
                              sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.nsapis) &
                              dec_ent->xid.alg.xid_rfc1144.nsapis;

                if( app_nsapis )
                {
                  for( idx = 0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
                  {
                    /* Exit the for loop if end of valid NSAPIs */
                    if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                      break;

                    if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx]) ) )
                    {
                      /* If data compression is enabled in the NSAPI and comp entity
                      ** use the same operation mode, then add NSAPI to the comp entity.
                      */
                      if( actv_nsapis[ gas_id ][nsapis_on_sapi[idx]] )
                      {
                        if( (actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->op_mode ==
                              sapi_ptr->pcomp_e_pool[ent_num]->op_mode) &&
                          (actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->pci_comp) )
                        {
                          actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                             pcomp_e_num[T2_ALG_RFC1144]= ent_num;
                        }
                        else
                        {
                          /* Clear bit in App NSAPIs */
                          /*lint -e{502} expected unsigned type for bit operation */
                          xid_i.type_field.xid_t2.alg.xid_rfc1144.nsapis &=
                            ~(uint16)( 0x1 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx]));
                        }
                      }
                    }
                  }
                }
              }
              else
              {
                /* Comp entity does not exist.
                ** If more than one NSAPI is set, check if they are using
                ** the same operation mode (ACK or UNACK). ACK and UNACK
                ** NSAPIs shall not share the same compression entity
                */
                gsn_get_app_nsapis( nsapis,
                  dec_ent->xid.alg.xid_rfc1144.nsapis);

                for( idx=0; (idx < (uint8) GSN_NSAPI_MAX) &&
                    ((uint8)nsapis[idx] != GSN_NSAPI_MAX) ; idx++ )
                {
                  /* Check if data compression is enabled for the NSAPI  */
                  if( (actv_nsapis[ gas_id ][nsapis[idx]] != NULL)  &&
                      (actv_nsapis[ gas_id ][nsapis[idx]]->pci_comp) )
                  {
                    if( actv_nsapis[ gas_id ][nsapis[idx]]->op_mode == ACK)
                      ack_nsapis = TRUE;
                    else
                      unack_nsapis = TRUE;
                  }
                  else
                  {
                    /* Clear bit in App NSAPIs */
                    /*lint -e{502} expected unsigned type for bit operation */
                    xid_i.type_field.xid_t2.alg.xid_rfc1144.nsapis &=
                     ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( (uint8)nsapis[idx] ) );
                  }
                }

                if( ack_nsapis && unack_nsapis )
                {
                  /* Chose which NSAPIs will use the comp entity
                  ** and fill in the field for the XID response. XID_HOLE!
                  */
                  if( gsn_get_ack_nsapis_on_sapi( sapi_ptr ) )
                  {
                    xid_i.type_field.xid_t2.alg.xid_rfc1144.nsapis =
                                  gsn_get_ack_nsapis_on_sapi( sapi_ptr );
                    unack_nsapis = FALSE;
                  }
                  else if( gsn_get_unack_nsapis_on_sapi( sapi_ptr ) )
                  {
                    xid_i.type_field.xid_t2.alg.xid_rfc1144.nsapis =
                                  gsn_get_unack_nsapis_on_sapi( sapi_ptr );
                    ack_nsapis = FALSE;
                  }
                  else
                  {
                    /* Something is wrong! No active NSAPIs on an active SAPI */
                    MSG_GERAN_ERROR_1("GSN ""LL %d: No Actv NSAPIs on an Actv SAPI",
                      gsn_extern_sapi[sapi_ptr->sapi]);
                  }
                }

                /* If no App NSAPIs left, respont with App NSAPIs set to 0. */
                if( !xid_i.type_field.xid_t2.alg.xid_rfc1144.nsapis )
                {
                  xid_i.length = 0;
                  gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
                  type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                  continue;
                }
              }

              /*-------------------- Process parameter S0-1  -------------------*/
              if( ( dec_ent->xid.hdr.p  &&
                ( dec_ent->xid.hdr.length >= 4)) ||
                ( !dec_ent->xid.hdr.p &&
                ( dec_ent->xid.hdr.length >= 3)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc1144.s0_1 =
                               dec_ent->xid.alg.xid_rfc1144.s0_1;

                /* Increment comp entity's parameter length so that
                ** the S0-1 field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t1.hdr.length += RFC1144_S0_1_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len +=  xid_i.type_field.xid_t2.hdr.length + XID_FIELD_HDR_P0_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                 ** any parameter of existing compression entities used with Ack NSAPIs
                 ** are non-negotiable. Respond with the priviously-negotiated values.
                 ** If one or more parameters of a comp entity used with Unack NSAPIs
                 ** are changed, the comp entity shall be reset locally upon completion
                 ** of the XID negotiation.
                 */
                if( ent_exist &&
                  ( dec_ent->xid.alg.xid_rfc1144.s0_1 !=
                     sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.s0_1) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated S0-1 value */
                      xid_i.type_field.xid_t2.alg.xid_rfc1144.s0_1 =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.s0_1;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {

                    /* Update entity's parameter value */
                    sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.s0_1 =
                                  xid_i.type_field.xid_t2.alg.xid_rfc1144.s0_1;

                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc1144.s0_1 = RFC1144_S0_1_DEF;

                xid_i.type_field.xid_t2.hdr.length += RFC1144_S0_1_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC1144_LEN_P0_MAX;
              }

              /* -------End of processing compression field xid parameters ---------*/

              if( sapi_ptr->pcomp_e_pool[ent_num]  == NULL )
              {
                /* Compression entity does not exist. Create and initialise entity */
                if( gsn_create_pcomp_e( &xid_i.type_field.xid_t2, sapi_ptr ) )
                {
                  /* Set entity's state to ASSIGNED. Entity is ready for
                  ** use after the XID response is sent to the peer
                  */
                  sapi_ptr->pcomp_e_pool[ent_num]->state = ASSIGNED;

                  /* Set entity's operation mode */
                  if( ack_nsapis )
                    sapi_ptr->pcomp_e_pool[ent_num]->op_mode = ACK;
                  else
                    sapi_ptr->pcomp_e_pool[ent_num]->op_mode = UNACK;

                  /* Initialise RFC 1144 compression entity */
                  if( !gsn_1144_comp_init(
                     gas_id, sapi_ptr->pcomp_e_pool[ent_num],
                     (uint8) (sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.s0_1 + 1) ) )
                  {
                    MSG_GERAN_HIGH_1("GSN ""PCI comp ent %d couldn't be initialised",ent_num);
                    rmv_ent = TRUE;
                  }
                }
                else
                {
                  MSG_GERAN_HIGH_1("GSN ""PCI comp ent %d could not be created ",ent_num);
                  rmv_ent = TRUE;
                }
              }

              if( (sapi_ptr->pcomp_pool
                  [dec_ent->xid.alg.xid_rfc1144.pcomp1]  == NULL) &&
                (sapi_ptr->pcomp_pool
                  [dec_ent->xid.alg.xid_rfc1144.pcomp2]  == NULL)  )
              {

                if( dec_ent->xid.alg.xid_rfc1144.pcomp1 )
                {
                  /* Ensure that PCOMP value is valid. PCOMP 15 is reserved for future use */
                  if( (dec_ent->xid.alg.xid_rfc1144.pcomp1 < PCOMP_VAL_MAX )   &&
                    (sapi_ptr->pcomp_pool[dec_ent->xid.alg.xid_rfc1144.pcomp1] == NULL) )
                  {
                    /* PCOMP value does not exist. Create PCOMP value and
                    ** link it to the algorithm.
                    */
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp1 ] =
                        GPRS_MEM_MALLOC( sizeof(pcomp_t) );
                    GSN_ASSERT( sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp1 ] != NULL );

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp1 ]->alg  = T2_ALG_RFC1144;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp1 ]->state= UNASSIGNED;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp1 ]->val_num =
                     dec_ent->xid.alg.xid_rfc1144.pcomp1;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp1 ]->state = ASSIGNED;
                  }
                  else
                  {
                    MSG_GERAN_HIGH_2("GSN ""XID %d: Inval PCOMP val %d",
                     gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_rfc1144.pcomp1);
                    rmv_ent = TRUE;
                  }
                }

                if( dec_ent->xid.alg.xid_rfc1144.pcomp2 )
                {
                  /* Ensure that PCOMP value is valid. PCOMP 15 is reserved for future use */
                  if( (dec_ent->xid.alg.xid_rfc1144.pcomp2 < PCOMP_VAL_MAX )   &&
                    (sapi_ptr->pcomp_pool[dec_ent->xid.alg.xid_rfc1144.pcomp2] == NULL) )
                  {
                    /* PCOMP value does not exist. Create PCOMP value and
                    ** link it to the algorithm.
                    */
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp2 ] =
                        GPRS_MEM_MALLOC( sizeof(pcomp_t) );
                    GSN_ASSERT( sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp2 ] != NULL );

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp2 ]->alg  = T2_ALG_RFC1144;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp2 ]->state= UNASSIGNED;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp2 ]->val_num =
                      dec_ent->xid.alg.xid_rfc1144.pcomp2;

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc1144.pcomp2 ]->state = ASSIGNED;

                  }
                  else
                  {
                   MSG_GERAN_HIGH_2("GSN ""XID %d: Inval PCOMP val %d",
                    gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_rfc1144.pcomp2);
                    rmv_ent = TRUE;
                  }
                }
              }
              else if( (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc1144.pcomp1]  == NULL) ||
                     (sapi_ptr->pcomp_pool
                      [dec_ent->xid.alg.xid_rfc1144.pcomp2]  == NULL)  )
              {
                /* One of the PCOMP values do not exist. All PCOMP values for one
                 ** algorithm should be created and deleted at the same insatnce.
                 */
                MSG_GERAN_ERROR_0("GSN "" Invalid state of PCOMP values");
                rmv_ent = TRUE;
              }

              if( rmv_ent == TRUE )
              {
                /* PCI compression entity could not be created or initialised.
                ** Delete PCI compression entity and inform the peer.
                */
                xid_i.length = 0;
                gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr );

                continue;
              }

              for( idx = 0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
              {
                /* Exit the for loop if end of valid NSAPIs */
                if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                  break;

                if( xid_i.type_field.xid_t2.alg.xid_rfc1144.nsapis &
                  (0x01 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx])) )
                {
                  /* NSAPI can start using the comp entity */
                  actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                          pcomp_e_num[T2_ALG_RFC1144]= ent_num;
                }
              }

              /* Before start using the entity perform the consistency test*/
              if(!gsn_check_pcomp_e( gas_id, sapi_ptr->pcomp_e_pool[ent_num], sapi_ptr ) )
              {
                MSG_GERAN_ERROR_2("GSN "" XID: PCI comp entity %d params not consistant",
                  gsn_extern_sapi[sapi_ptr->sapi], ent_num);
              }

              MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use RFC 1144 ent %d ",
                gsn_extern_sapi[sapi_ptr->sapi],
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc1144.nsapis, ent_num);

              /* Encode XID response  */
              xid_i.length = 0;  /* Not the last entity */
              xid_neg_len  = gsn_encode_xid( &xid_i, xid_ex );

              /* Check if there is need to allocate a pdu item to carry the XID block */
              if( xid_rsp != NULL )
              {
                /* Append entity's comp field to the existing XID block */
                if( !gprs_pdu_append_head( &xid_rsp, xid_ex, xid_neg_len ) )
                {
                  MSG_GERAN_ERROR_1("GSN ""LL %d: Append XID block failed",
                   gsn_extern_sapi[sapi_ptr->sapi]);
                }
              }
              else
              {
                /* Create a pdu item to carry the XID block */
                xid_rsp = gsn_pdu_construct( xid_ex, xid_neg_len );
              }
            }
            else if(dec_ent->xid.hdr.alg_type == T2_ALG_RFC2507)
            {
              /* ---------------------- Applicable NSAPIs ------------------------*/

              /* If App NSAPIs is 0. Delete compression entity */
              if( ent_exist && !dec_ent->xid.alg.xid_rfc2507.nsapis )
              {
                /* Not the last entity. Do not Include type header */
                xid_i.length = 0;
                gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr );

                continue;
              }

              /* The response will be the same if no exception occurs */
              xid_i.type_field.xid_t2.alg.xid_rfc2507.nsapis =
                            dec_ent->xid.alg.xid_rfc2507.nsapis;

              /* Increment comp entity's parameter length so that
              ** the App NSAPIs field will be included in the XID response
              ** Note that in the response the Propose bit (P) is set to 0.
              */
              xid_i.type_field.xid_t2.hdr.length = APP_NSAPIS_LEN;
              type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

              /* Check if any NSAPIs have been removed from an existing comp entity
              */
              if( ent_exist )
              {
                app_nsapis = (dec_ent->xid.alg.xid_rfc2507.nsapis^
                            sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.nsapis) &
                            sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.nsapis;

                /* LL_ESTABLISH primitive shall be used if one or more NSAPIs are
                ** removed from existing comp entities used with ack NSAPIs, except
                ** when all NSAPIs using the comp entity are removed,
                ** or LLC is already in ADM.
                */
                if( app_nsapis )
                {
                  if( (sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK) &&
                      (xid_on_est_msg == FALSE) &&
                    ( (dec_ent->xid.alg.xid_rfc2507.nsapis != 0) ||
                      (sapi_ptr->state != IDLE) ) )
                  {
                    /* Negotiation is not possible. Respond with the
                    ** priviously-negotiated App NSAPIs value
                    */
                    xid_i.type_field.xid_t2.alg.xid_rfc2507.nsapis =
                     sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.nsapis;
                  }
                  else
                  {
                    for( idx = 0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
                    {
                      /* Exit the for loop if end of valid NSAPIs */
                      if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                        break;

                      if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx]) ) )
                      {
                        /* Remove comp entity from NSAPI if it has been using the entity */
                        if( actv_nsapis[ gas_id ][nsapis_on_sapi[idx]] )
                          actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                            pcomp_e_num[T2_ALG_RFC1144]= NO_COMP_ENT;
                      }
                    }
                  }
                }

                /* Check if any NSAPIs have been added to the existing comp entity
                */
                app_nsapis = (dec_ent->xid.alg.xid_rfc2507.nsapis ^
                              sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.nsapis) &
                              dec_ent->xid.alg.xid_rfc2507.nsapis;

                if( app_nsapis )
                {
                  for( idx = 0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
                  {
                    /* Exit the for loop if end of valid NSAPIs */
                    if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                      break;

                    if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx]) ) )
                    {
                      /* If data compression is enabled in the NSAPI and comp entity
                      ** use the same operation mode, then add NSAPI to the comp entity.
                      */
                      if( actv_nsapis[ gas_id ][nsapis_on_sapi[idx]] )
                      {
                        if( (actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->op_mode ==
                              sapi_ptr->pcomp_e_pool[ent_num]->op_mode) &&
                          (actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->pci_comp) )
                        {
                          actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                             pcomp_e_num[T2_ALG_RFC2507]= ent_num;
                        }
                        else
                        {
                          /* Clear bit in App NSAPIs */
                          /*lint -e{502} expected unsigned type for bit operation */
                          xid_i.type_field.xid_t2.alg.xid_rfc2507.nsapis &=
                            ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( nsapis_on_sapi[idx] ) );
                        }
                      }
                    }
                  }
                }
              }
              else
              {
                /* Comp entity does not exist.
                ** If more than one NSAPI is set, check if they are using
                ** the same operation mode (ACK or UNACK). ACK and UNACK
                ** NSAPIs shall not share the same compression entity
                */
                gsn_get_app_nsapis( nsapis,
                  dec_ent->xid.alg.xid_rfc2507.nsapis);

                for( idx=0; (idx < (uint8) GSN_NSAPI_MAX) &&
                    ((uint8)nsapis[idx] != GSN_NSAPI_MAX) ; idx++ )
                {
                  /* Check if data compression is enabled for the NSAPI  */
                  if( (actv_nsapis[ gas_id ][nsapis[idx]] != NULL)  &&
                      (actv_nsapis[ gas_id ][nsapis[idx]]->pci_comp) )
                  {
                    if( actv_nsapis[ gas_id ][nsapis[idx]]->op_mode == ACK)
                      ack_nsapis = TRUE;
                    else
                      unack_nsapis = TRUE;
                  }
                  else
                  {
                    /* Clear bit in App NSAPIs */
                    /*lint -e{502} expected unsigned type for bit operation */
                    xid_i.type_field.xid_t2.alg.xid_rfc2507.nsapis &=
                      ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( nsapis[idx] ) );
                  }
                }

                if( ack_nsapis && unack_nsapis )
                {
                  /* Chose which NSAPIs will use the comp entity
                  ** and fill in the field for the XID response. XID_HOLE!
                  */
                  if( gsn_get_ack_nsapis_on_sapi( sapi_ptr ) )
                  {
                    xid_i.type_field.xid_t2.alg.xid_rfc2507.nsapis =
                                  gsn_get_ack_nsapis_on_sapi( sapi_ptr );
                    unack_nsapis = FALSE;
                  }
                  else if( gsn_get_unack_nsapis_on_sapi( sapi_ptr ) )
                  {
                    xid_i.type_field.xid_t2.alg.xid_rfc2507.nsapis =
                                  gsn_get_unack_nsapis_on_sapi( sapi_ptr );
                    ack_nsapis = FALSE;
                  }
                  else
                  {
                    /* Something is wrong! No active NSAPIs on an active SAPI */
                    MSG_GERAN_ERROR_1("GSN ""LL %d: No Actv NSAPIs on an Actv SAPI",
                      gsn_extern_sapi[sapi_ptr->sapi]);
                  }
                }

                /* If no App NSAPIs left, respont with App NSAPIs set to 0. */
                if( !xid_i.type_field.xid_t2.alg.xid_rfc2507.nsapis )
                {
                  xid_i.length = 0;
                  gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
                  type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                  continue;
                }
              }

              /*-------------------- Process parameter F_MAX_PERIOD  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 7)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 4)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.max_per =
                               dec_ent->xid.alg.xid_rfc2507.max_per;

                /* Increment comp entity's parameter length so that
                ** the F_MAX_PERIOD field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC2507_MAX_PER_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len += RFC2507_MAX_PER_LEN ;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc2507.max_per !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.max_per) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated S0-1 value */
                      xid_i.type_field.xid_t2.alg.xid_rfc2507.max_per =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.max_per;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.max_per =
                                  xid_i.type_field.xid_t2.alg.xid_rfc2507.max_per;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.max_per = RFC2507_MAX_PER_DEF;

                xid_i.type_field.xid_t2.hdr.length += RFC2507_MAX_PER_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC2507_LEN_P0_MAX;
              }


              /*-------------------- Process parameter F_MAX_TIME  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 8)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 5)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.max_time =
                               dec_ent->xid.alg.xid_rfc2507.max_time;

                /* Increment comp entity's parameter length so that
                ** the F_MAX_PERIOD field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC2507_MAX_TIME_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len += RFC2507_MAX_TIME_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc2507.max_time !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.max_time) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated F_MAX_TIME_MAX value */
                      xid_i.type_field.xid_t2.alg.xid_rfc2507.max_time =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.max_time;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.max_time =
                                  xid_i.type_field.xid_t2.alg.xid_rfc2507.max_time;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.max_per = RFC2507_MAX_TIME_DEF;

                xid_i.type_field.xid_t2.hdr.length += RFC2507_MAX_TIME_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC2507_LEN_P0_MAX;
              }


              /*-------------------- Process parameter MAX_HEADER  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 9)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 6)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.max_hdr=
                               dec_ent->xid.alg.xid_rfc2507.max_hdr;

                /* Increment comp entity's parameter length so that
                ** the F_MAX_HEADER field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC2507_MAX_HDR_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len += RFC2507_MAX_HDR_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc2507.max_hdr !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.max_hdr) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated F_MAX_TIME_MAX value */
                      xid_i.type_field.xid_t2.alg.xid_rfc2507.max_hdr =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.max_hdr;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.max_hdr =
                                  xid_i.type_field.xid_t2.alg.xid_rfc2507.max_hdr;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.max_hdr = RFC2507_MAX_HDR_DEF;

                xid_i.type_field.xid_t2.hdr.length += RFC2507_MAX_HDR_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC2507_LEN_P0_MAX;
              }


              /*-------------------- Process parameter TCP_SPACE  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 10)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 7)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.tcp_sp=
                               dec_ent->xid.alg.xid_rfc2507.tcp_sp;

                /* Increment comp entity's parameter length so that
                ** the TCP_SPACE field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC2507_TCP_SP_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len += RFC2507_TCP_SP_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc2507.tcp_sp !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.tcp_sp) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated F_MAX_TIME_MAX value */
                      xid_i.type_field.xid_t2.alg.xid_rfc2507.tcp_sp =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.tcp_sp;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.tcp_sp =
                                  xid_i.type_field.xid_t2.alg.xid_rfc2507.tcp_sp;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.tcp_sp = RFC2507_TCP_SP_DEF;

                xid_i.type_field.xid_t2.hdr.length += RFC2507_TCP_SP_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC2507_LEN_P0_MAX;
              }


              /*-------------------- Process parameter NON_TCP_SPACE  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 12)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 9)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.ntcp_sp=
                               dec_ent->xid.alg.xid_rfc2507.ntcp_sp;

                /* Increment comp entity's parameter length so that
                ** the NON_TCP_SPACE field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC2507_NTCP_SP_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len += RFC2507_NTCP_SP_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc2507.ntcp_sp !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.ntcp_sp) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated F_MAX_TIME_MAX value */
                      xid_i.type_field.xid_t2.alg.xid_rfc2507.ntcp_sp =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.ntcp_sp;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.ntcp_sp =
                                  xid_i.type_field.xid_t2.alg.xid_rfc2507.ntcp_sp;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc2507.tcp_sp = RFC2507_NTCP_SP_DEF;

                xid_i.type_field.xid_t2.hdr.length += RFC2507_NTCP_SP_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC2507_LEN_P0_MAX;
              }


              /* -------End of processing compression field xid parameters ---------*/

              if( sapi_ptr->pcomp_e_pool[ent_num]  == NULL )
              {
                /* Compression entity does not exist. Create and initialise entity */
                if( gsn_create_pcomp_e( &xid_i.type_field.xid_t2, sapi_ptr ) )
                {
                  /* Set entity's state to ASSIGNED. Entity is ready for
                  ** use after the XID response is sent to the peer
                  */
                  sapi_ptr->pcomp_e_pool[ent_num]->state = ASSIGNED;

                  /* Set entity's operation mode */
                  if( ack_nsapis )
                    sapi_ptr->pcomp_e_pool[ent_num]->op_mode = ACK;
                  else
                    sapi_ptr->pcomp_e_pool[ent_num]->op_mode = UNACK;

                  /* Initialise RFC 2507 compression entity */
                  if(!gsn_iphc_comp_init(
                       gas_id, sapi_ptr->pcomp_e_pool[ent_num],
                      (gsn_xid_pcomp_2507_t *)(&sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507)))
                  {
                    MSG_GERAN_HIGH_1("GSN ""iphc comp ent %d couldn't be initialised",ent_num);
                    rmv_ent = TRUE;
                  }


                }
                else
                {
                  MSG_GERAN_HIGH_1("GSN ""PCI comp ent %d could not be created ",ent_num);
                  rmv_ent = TRUE;
                }
              }

              if( (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc2507.pcomp1]  == NULL) &&
                  (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc2507.pcomp2]  == NULL) &&
                  (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc2507.pcomp3]  == NULL) &&
                  (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc2507.pcomp4]  == NULL) &&
                  (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc2507.pcomp5]  == NULL))
              {

                if( dec_ent->xid.alg.xid_rfc2507.pcomp1 )
                {
                  /* Ensure that PCOMP value is valid. PCOMP 15 is reserved for future use */
                  if( (dec_ent->xid.alg.xid_rfc2507.pcomp1 < PCOMP_VAL_MAX )   &&
                      (sapi_ptr->pcomp_pool[dec_ent->xid.alg.xid_rfc2507.pcomp1] == NULL) )
                  {
                    /* PCOMP value does not exist. Create PCOMP value and
                    ** link it to the algorithm.
                    */
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp1 ] =
                          GPRS_MEM_MALLOC( sizeof(pcomp_t) );
                    GSN_ASSERT( sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp1 ] != NULL );

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp1 ]->alg  = T2_ALG_RFC2507;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp1 ]->state= UNASSIGNED;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp1 ]->val_num =
                     dec_ent->xid.alg.xid_rfc2507.pcomp1;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp1 ]->state = ASSIGNED;
                  }
                  else
                  {
                    MSG_GERAN_HIGH_2("GSN ""XID %d: Inval PCOMP val %d",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_rfc2507.pcomp1);
                    rmv_ent = TRUE;
                  }
                }

                if( dec_ent->xid.alg.xid_rfc2507.pcomp2 )
                {
                  /* Ensure that PCOMP value is valid. PCOMP 15 is reserved for future use */
                  if( (dec_ent->xid.alg.xid_rfc2507.pcomp2 < PCOMP_VAL_MAX )   &&
                      (sapi_ptr->pcomp_pool[dec_ent->xid.alg.xid_rfc2507.pcomp2] == NULL) )
                  {
                    /* PCOMP value does not exist. Create PCOMP value and
                    ** link it to the algorithm.
                    */
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp2 ] =
                          GPRS_MEM_MALLOC( sizeof(pcomp_t) );
                    GSN_ASSERT( sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp2 ] != NULL );

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp2 ]->alg  = T2_ALG_RFC2507;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp2 ]->state= UNASSIGNED;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp2 ]->val_num =
                      dec_ent->xid.alg.xid_rfc2507.pcomp2;

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp2 ]->state = ASSIGNED;

                  }
                  else
                  {
                    MSG_GERAN_HIGH_2("GSN ""XID %d: Inval PCOMP val %d",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_rfc2507.pcomp2);
                    rmv_ent = TRUE;
                  }
                }

                if( dec_ent->xid.alg.xid_rfc2507.pcomp3 )
                {
                  /* Ensure that PCOMP value is valid. PCOMP 15 is reserved for future use */
                  if( (dec_ent->xid.alg.xid_rfc2507.pcomp3 < PCOMP_VAL_MAX )   &&
                      (sapi_ptr->pcomp_pool[dec_ent->xid.alg.xid_rfc2507.pcomp3] == NULL) )
                  {
                    /* PCOMP value does not exist. Create PCOMP value and
                    ** link it to the algorithm.
                    */
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp3 ] =
                          GPRS_MEM_MALLOC( sizeof(pcomp_t) );
                    GSN_ASSERT( sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp3 ] != NULL );

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp3 ]->alg  = T2_ALG_RFC2507;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp3 ]->state= UNASSIGNED;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp3 ]->val_num =
                      dec_ent->xid.alg.xid_rfc2507.pcomp3;

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp3 ]->state = ASSIGNED;

                  }
                  else
                  {
                    MSG_GERAN_HIGH_2("GSN ""XID %d: Inval PCOMP val %d",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_rfc2507.pcomp3);
                    rmv_ent = TRUE;
                  }
                }

                if( dec_ent->xid.alg.xid_rfc2507.pcomp4 )
                {
                  /* Ensure that PCOMP value is valid. PCOMP 15 is reserved for future use */
                  if( (dec_ent->xid.alg.xid_rfc2507.pcomp4 < PCOMP_VAL_MAX )   &&
                      (sapi_ptr->pcomp_pool[dec_ent->xid.alg.xid_rfc2507.pcomp4] == NULL) )
                  {
                    /* PCOMP value does not exist. Create PCOMP value and
                    ** link it to the algorithm.
                    */
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp4 ] =
                          GPRS_MEM_MALLOC( sizeof(pcomp_t) );
                    GSN_ASSERT( sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp4 ] != NULL );

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp4 ]->alg  = T2_ALG_RFC2507;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp4 ]->state= UNASSIGNED;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp4 ]->val_num =
                      dec_ent->xid.alg.xid_rfc2507.pcomp4;

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp4 ]->state = ASSIGNED;

                  }
                  else
                  {
                    MSG_GERAN_HIGH_2("GSN ""XID %d: Inval PCOMP val %d",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_rfc2507.pcomp4);
                    rmv_ent = TRUE;
                  }
                }

                if( dec_ent->xid.alg.xid_rfc2507.pcomp5 )
                {
                  /* Ensure that PCOMP value is valid. PCOMP 15 is reserved for future use */
                  if( (dec_ent->xid.alg.xid_rfc2507.pcomp5 < PCOMP_VAL_MAX )   &&
                      (sapi_ptr->pcomp_pool[dec_ent->xid.alg.xid_rfc2507.pcomp5] == NULL) )
                  {
                    /* PCOMP value does not exist. Create PCOMP value and
                    ** link it to the algorithm.
                    */
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp5 ] =
                          GPRS_MEM_MALLOC( sizeof(pcomp_t) );
                    GSN_ASSERT( sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp5 ] != NULL );

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp5 ]->alg  = T2_ALG_RFC2507;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp5 ]->state= UNASSIGNED;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp5 ]->val_num =
                      dec_ent->xid.alg.xid_rfc2507.pcomp5;

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc2507.pcomp5 ]->state = ASSIGNED;

                  }
                  else
                  {
                    MSG_GERAN_HIGH_2("GSN ""XID %d: Inval PCOMP val %d",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_rfc2507.pcomp5);
                    rmv_ent = TRUE;
                  }
                }

              }
              else if( (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc2507.pcomp1]  == NULL) ||
                  (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc2507.pcomp2]  == NULL) ||
                  (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc2507.pcomp3]  == NULL) ||
                  (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc2507.pcomp4]  == NULL) ||
                  (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc2507.pcomp5]  == NULL))
              {
                /* One of the PCOMP values do not exist. All PCOMP values for one
                ** algorithm should be created and deleted at the same insatnce.
                */
                MSG_GERAN_ERROR_0("GSN "" Invalid state of PCOMP values");
                rmv_ent = TRUE;
              }

              if( rmv_ent == TRUE )
              {
                /* PCI compression entity could not be created or initialised.
                ** Delete PCI compression entity and inform the peer.
                */
                xid_i.length = 0;
                gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr );

                continue;
              }

              for( idx = 0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
              {
                /* Exit the for loop if end of valid NSAPIs */
                if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                  break;

                if( xid_i.type_field.xid_t2.alg.xid_rfc2507.nsapis &
                   (0x01 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx])) )
                {
                  /* NSAPI can start using the comp entity */
                  actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                          pcomp_e_num[T2_ALG_RFC2507]= ent_num;
                }
              }

              /* Before start using the entity perform the consistency test*/
              if(!gsn_check_pcomp_e( gas_id, sapi_ptr->pcomp_e_pool[ent_num], sapi_ptr ) )
              {
                MSG_GERAN_ERROR_2("GSN "" XID: PCI comp entity %d params not consistant",
                    gsn_extern_sapi[sapi_ptr->sapi], ent_num);
              }

              MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use RFC 2507 ent %d ",
                gsn_extern_sapi[sapi_ptr->sapi],
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc2507.nsapis, ent_num);

              /* Encode XID response  */
              xid_i.length = 0;  /* Not the last entity */
              xid_neg_len  = gsn_encode_xid( &xid_i, xid_ex );

              /* Check if there is need to allocate a pdu item to carry the XID block */
              if( xid_rsp != NULL )
              {
                /* Append entity's comp field to the existing XID block */
                if( !gprs_pdu_append_head( &xid_rsp, xid_ex, xid_neg_len ) )
                {
                  MSG_GERAN_ERROR_1("GSN ""LL %d: Append XID block failed",
                    gsn_extern_sapi[sapi_ptr->sapi]);
                }
              }
              else
              {
                /* Create a pdu item to carry the XID block */
                xid_rsp = gsn_pdu_construct( xid_ex, xid_neg_len );
              }
            }
            else if(dec_ent->xid.hdr.alg_type == T2_ALG_RFC3095)
            {
              /* ---------------------- Applicable NSAPIs ------------------------*/

              /* If App NSAPIs is 0. Delete compression entity */
              if( ent_exist && !dec_ent->xid.alg.xid_rfc3095.nsapis )
              {
                /* Not the last entity. Do not Include type header */
                xid_i.length = 0;
                gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr );

                continue;
              }

              /* The response will be the same if no exception occurs */
              xid_i.type_field.xid_t2.alg.xid_rfc3095.nsapis =
                            dec_ent->xid.alg.xid_rfc3095.nsapis;
#if 0
              /* Disable ROHC for now */
              xid_i.type_field.xid_t2.alg.xid_rfc3095.nsapis = 0;
#endif

              /* Increment comp entity's parameter length so that
              ** the App NSAPIs field will be included in the XID response
              ** Note that in the response the Propose bit (P) is set to 0.
              */
              xid_i.type_field.xid_t2.hdr.length = APP_NSAPIS_LEN;
              type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

              /* Check if any NSAPIs have been removed from an existing comp entity
              */
              if( ent_exist )
              {
                app_nsapis = (dec_ent->xid.alg.xid_rfc3095.nsapis^
                            sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.nsapis) &
                            sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.nsapis;

                /* LL_ESTABLISH primitive shall be used if one or more NSAPIs are
                ** removed from existing comp entities used with ack NSAPIs, except
                ** when all NSAPIs using the comp entity are removed,
                ** or LLC is already in ADM.
                */
                if( app_nsapis )
                {
                  if( (sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK) &&
                      (xid_on_est_msg == FALSE) &&
                    ( (dec_ent->xid.alg.xid_rfc3095.nsapis != 0) ||
                      (sapi_ptr->state != IDLE) ) )
                  {
                    /* Negotiation is not possible. Respond with the
                    ** priviously-negotiated App NSAPIs value
                    */
                    xid_i.type_field.xid_t2.alg.xid_rfc3095.nsapis =
                     sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.nsapis;
                  }
                  else
                  {
                    for( idx = 0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
                    {
                      /* Exit the for loop if end of valid NSAPIs */
                      if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                        break;

                      if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx]) ) )
                      {
                        /* Remove comp entity from NSAPI if it has been using the entity */
                        if( actv_nsapis[ gas_id ][nsapis_on_sapi[idx]] )
                          actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                            pcomp_e_num[T2_ALG_RFC3095]= NO_COMP_ENT;
                      }
                    }
                  }
                }

                /* Check if any NSAPIs have been added to the existing comp entity
                */
                app_nsapis = (dec_ent->xid.alg.xid_rfc3095.nsapis ^
                              sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.nsapis) &
                              dec_ent->xid.alg.xid_rfc3095.nsapis;

                if( app_nsapis )
                {
                  for( idx = 0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
                  {
                    /* Exit the for loop if end of valid NSAPIs */
                    if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                      break;

                    if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx]) ) )
                    {
                      /* If data compression is enabled in the NSAPI and comp entity
                      ** use the same operation mode, then add NSAPI to the comp entity.
                      */
                      if( actv_nsapis[ gas_id ][nsapis_on_sapi[idx]] )
                      {
                        if( (actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->op_mode ==
                              sapi_ptr->pcomp_e_pool[ent_num]->op_mode) &&
                          (actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->pci_comp) )
                        {
                          actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                             pcomp_e_num[T2_ALG_RFC3095]= ent_num;
                        }
                        else
                        {
                          /* Clear bit in App NSAPIs */
                          /*lint -e{502} expected unsigned type for bit operation */
                          xid_i.type_field.xid_t2.alg.xid_rfc2507.nsapis &=
                            ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( nsapis_on_sapi[idx] ) );
                        }
                      }
                    }
                  }
                }
              }
              else
              {
                /* Comp entity does not exist.
                ** If more than one NSAPI is set, check if they are using
                ** the same operation mode (ACK or UNACK). ACK and UNACK
                ** NSAPIs shall not share the same compression entity
                */
                gsn_get_app_nsapis( nsapis,
                  dec_ent->xid.alg.xid_rfc3095.nsapis);

                for( idx=0; (idx < (uint8) GSN_NSAPI_MAX) &&
                    ((uint8)nsapis[idx] != GSN_NSAPI_MAX) ; idx++ )
                {
                  /* Check if data compression is enabled for the NSAPI  */
                  if( (actv_nsapis[ gas_id ][nsapis[idx]] != NULL)  &&
                      (actv_nsapis[ gas_id ][nsapis[idx]]->pci_comp) )
                  {
                    if( actv_nsapis[ gas_id ][nsapis[idx]]->op_mode == ACK)
                      ack_nsapis = TRUE;
                    else
                      unack_nsapis = TRUE;
                  }
                  else
                  {
                    /* Clear bit in App NSAPIs */
                    /*lint -e{502} expected unsigned type for bit operation */
                    xid_i.type_field.xid_t2.alg.xid_rfc3095.nsapis &=
                      ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( nsapis[idx] ) );
                  }
                }

                if( ack_nsapis && unack_nsapis )
                {
                  /* Chose which NSAP  Is will use the comp entity
                  ** and fill in the field for the XID response. XID_HOLE!
                  */
                  if( gsn_get_ack_nsapis_on_sapi( sapi_ptr ) )
                  {
                    xid_i.type_field.xid_t2.alg.xid_rfc3095.nsapis =
                                  gsn_get_ack_nsapis_on_sapi( sapi_ptr );
                    unack_nsapis = FALSE;
                  }
                  else if( gsn_get_unack_nsapis_on_sapi( sapi_ptr ) )
                  {
                    xid_i.type_field.xid_t2.alg.xid_rfc3095.nsapis =
                                  gsn_get_unack_nsapis_on_sapi( sapi_ptr );
                    ack_nsapis = FALSE;
                  }
                  else
                  {
                    /* Something is wrong! No active NSAPIs on an active SAPI */
                    MSG_GERAN_ERROR_1("GSN ""LL %d: No Actv NSAPIs on an Actv SAPI",
                      gsn_extern_sapi[sapi_ptr->sapi]);
                  }
                }

                /* If no App NSAPIs left, respont with App NSAPIs set to 0. */
                if( !xid_i.type_field.xid_t2.alg.xid_rfc3095.nsapis )
                {
                  xid_i.length = 0;
                  gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
                  type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                  continue;
                }
              }

              /*-------------------- Process parameter MAX_CID  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 5)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 4)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.max_cid =
                               dec_ent->xid.alg.xid_rfc3095.max_cid;

                /* Increment comp entity's parameter length so that
                ** the MAX_CID field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC3095_MAX_CID_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len +=  RFC3095_MAX_CID_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc3095.max_cid !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.max_cid) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated S0-1 value */
                      xid_i.type_field.xid_t2.alg.xid_rfc3095.max_cid =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.max_cid;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.max_cid =
                                  xid_i.type_field.xid_t2.alg.xid_rfc3095.max_cid;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.max_cid = RFC3095_MAX_CID_DEF;

                xid_i.type_field.xid_t2.hdr.length += RFC3095_MAX_CID_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC3095_LEN_P0_MAX;
              }


              /*-------------------- Process parameter MAX_HEADER  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 7)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 6)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.max_hdr =
                               dec_ent->xid.alg.xid_rfc3095.max_hdr;

                /* Increment comp entity's parameter length so that
                ** the MAX_HEADER field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC3095_MAX_HDR_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len += RFC3095_MAX_HDR_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc3095.max_hdr !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.max_hdr) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated F_MAX_TIME_MAX value */
                      xid_i.type_field.xid_t2.alg.xid_rfc3095.max_hdr =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.max_hdr;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.max_hdr =
                                  xid_i.type_field.xid_t2.alg.xid_rfc3095.max_hdr;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.max_hdr = RFC3095_MAX_HDR_DEF;

                xid_i.type_field.xid_t2.hdr.length += RFC3095_MAX_HDR_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC3095_LEN_P0_MAX;
              }


              /*-------------------- Process parameter PROFILE1  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 9)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 8)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.profile1=
                               dec_ent->xid.alg.xid_rfc3095.profile1;

                /* Increment comp entity's parameter length so that
                ** the PROFILE1 field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC3095_PROF_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len += RFC3095_PROF_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc3095.profile1 !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile1) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated F_MAX_TIME_MAX value */
                      xid_i.type_field.xid_t2.alg.xid_rfc3095.profile1 =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile1;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile1 =
                                  xid_i.type_field.xid_t2.alg.xid_rfc3095.profile1;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.profile1= RFC3095_NOCOMP_PROF;

                xid_i.type_field.xid_t2.hdr.length += RFC3095_PROF_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC3095_LEN_P0_MAX;
              }


              /*-------------------- Process parameter PROFILE2  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 11)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 10)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.profile2=
                               dec_ent->xid.alg.xid_rfc3095.profile2;

                /* Increment comp entity's parameter length so that
                ** the PROFILE2 field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC3095_PROF_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len +=  RFC3095_PROF_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc3095.profile2 !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile2) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated F_MAX_TIME_MAX value */
                      xid_i.type_field.xid_t2.alg.xid_rfc3095.profile2 =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile2;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile2 =
                                  xid_i.type_field.xid_t2.alg.xid_rfc3095.profile2;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.profile2 = RFC3095_NOCOMP_PROF;

                xid_i.type_field.xid_t2.hdr.length += RFC3095_PROF_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC3095_LEN_P0_MAX;
              }


              /*-------------------- Process parameter PROFILE3  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 13)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 12)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.profile3=
                               dec_ent->xid.alg.xid_rfc3095.profile3;

                /* Increment comp entity's parameter length so that
                ** the PROFILE3 field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC3095_PROF_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len +=  RFC3095_PROF_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc3095.profile3 !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile3) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated F_MAX_TIME_MAX value */
                      xid_i.type_field.xid_t2.alg.xid_rfc3095.profile3 =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile3;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile3 =
                                  xid_i.type_field.xid_t2.alg.xid_rfc3095.profile3;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.profile3= RFC3095_NOCOMP_PROF;

                xid_i.type_field.xid_t2.hdr.length += RFC3095_PROF_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC3095_LEN_P0_MAX;
              }


              /*-------------------- Process parameter PROFILE4  --------*/
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 15)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 14)) )
              {
                /* Check if we support this value. TBC.
                ** The response will be the same if no exception occurs
                */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.profile4=
                               dec_ent->xid.alg.xid_rfc3095.profile4;

                /* Increment comp entity's parameter length so that
                ** the PROFILE4 field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t2.hdr.length += RFC3095_PROF_LEN;

                /* Last parameter. Add entity's length to overall T2 parameter length. */
                type_len +=  RFC3095_PROF_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    ( dec_ent->xid.alg.xid_rfc3095.profile4 !=
                      sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile4) )
                {
                  if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated F_MAX_TIME_MAX value */
                      xid_i.type_field.xid_t2.alg.xid_rfc3095.profile4 =
                        sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile4;
                    }
                  }
                  else if( sapi_ptr->pcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_pcomp_ent( gas_id, sapi_ptr->pcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.profile4 =
                                  xid_i.type_field.xid_t2.alg.xid_rfc3095.profile3;
                }
              }
              else
              {
                /* Set the preferred xid parameter values for the response */
                xid_i.type_field.xid_t2.alg.xid_rfc3095.profile4= RFC3095_NOCOMP_PROF;

                xid_i.type_field.xid_t2.hdr.length += RFC3095_PROF_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + RFC3095_LEN_P0_MAX;
              }


              /* -------End of processing compression field xid parameters ---------*/



              if( sapi_ptr->pcomp_e_pool[ent_num]  == NULL )
              {
                /* Compression entity does not exist. Create and initialise entity */
                if( gsn_create_pcomp_e( &xid_i.type_field.xid_t2, sapi_ptr ) )
                {
                  /* Set entity's state to ASSIGNED. Entity is ready for
                  ** use after the XID response is sent to the peer
                  */
                  sapi_ptr->pcomp_e_pool[ent_num]->state = ASSIGNED;

                  /* Set entity's operation mode */
                  if( ack_nsapis )
                    sapi_ptr->pcomp_e_pool[ent_num]->op_mode = ACK;
                  else
                    sapi_ptr->pcomp_e_pool[ent_num]->op_mode = UNACK;

                  /* Initialise RFC 3095 compression entity */
                  if(!gsn_rohc_comp_init(
                     gas_id, (void *)sapi_ptr->pcomp_e_pool[ent_num],
                     (gsn_xid_pcomp_3095_t *)(&sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095)))
                  {
                    MSG_GERAN_HIGH_1("GSN ""rohc comp ent %d couldn't be initialised",ent_num);
                    rmv_ent = TRUE;
                  }
                }
                else
                {
                  MSG_GERAN_HIGH_1("GSN ""PCI comp ent %d could not be created ",ent_num);
                  rmv_ent = TRUE;
                }
              }

              if( (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc3095.pcomp1]  == NULL) &&
                  (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc3095.pcomp2]  == NULL))
              {

                if( dec_ent->xid.alg.xid_rfc3095.pcomp1 )
                {
                  /* Ensure that PCOMP value is valid. PCOMP 15 is reserved for future use */
                  if( (dec_ent->xid.alg.xid_rfc3095.pcomp1 < PCOMP_VAL_MAX )   &&
                      (sapi_ptr->pcomp_pool[dec_ent->xid.alg.xid_rfc3095.pcomp1] == NULL) )
                  {
                    /* PCOMP value does not exist. Create PCOMP value and
                    ** link it to the algorithm.
                    */
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp1 ] =
                          GPRS_MEM_MALLOC( sizeof(pcomp_t) );
                    GSN_ASSERT( sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp1 ] != NULL );

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp1 ]->alg  = T2_ALG_RFC3095;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp1 ]->state= UNASSIGNED;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp1 ]->val_num =
                     dec_ent->xid.alg.xid_rfc3095.pcomp1;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp1 ]->state = ASSIGNED;
                  }
                  else
                  {
                    MSG_GERAN_HIGH_2("GSN ""XID %d: Inval PCOMP val %d",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_rfc3095.pcomp1);
                    rmv_ent = TRUE;
                  }
                }

                if( dec_ent->xid.alg.xid_rfc3095.pcomp2 )
                {
                  /* Ensure that PCOMP value is valid. PCOMP 15 is reserved for future use */
                  if( (dec_ent->xid.alg.xid_rfc3095.pcomp2 < PCOMP_VAL_MAX )   &&
                      (sapi_ptr->pcomp_pool[dec_ent->xid.alg.xid_rfc3095.pcomp2] == NULL) )
                  {
                    /* PCOMP value does not exist. Create PCOMP value and
                    ** link it to the algorithm.
                    */
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp2 ] =
                          GPRS_MEM_MALLOC( sizeof(pcomp_t) );
                    GSN_ASSERT( sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp2 ] != NULL );

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp2 ]->alg  = T2_ALG_RFC3095;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp2 ]->state= UNASSIGNED;
                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp2 ]->val_num =
                      dec_ent->xid.alg.xid_rfc3095.pcomp2;

                    sapi_ptr->pcomp_pool[ dec_ent->xid.alg.xid_rfc3095.pcomp2 ]->state = ASSIGNED;

                  }
                  else
                  {
                    MSG_GERAN_HIGH_2("GSN ""XID %d: Inval PCOMP val %d",
                      gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_rfc3095.pcomp2);
                    rmv_ent = TRUE;
                  }
                }

              }
              else if( (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc3095.pcomp1]  == NULL) ||
                  (sapi_ptr->pcomp_pool
                   [dec_ent->xid.alg.xid_rfc3095.pcomp2]  == NULL))
              {
                /* One of the PCOMP values do not exist. All PCOMP values for one
                ** algorithm should be created and deleted at the same insatnce.
                */
                MSG_GERAN_ERROR_0("GSN "" Invalid state of PCOMP values");
                rmv_ent = TRUE;
              }

              if( rmv_ent == TRUE )
              {
                /* PCI compression entity could not be created or initialised.
                ** Delete PCI compression entity and inform the peer.
                */
                xid_i.length = 0;
                gsn_bld_rej_t2_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                gsn_del_pcomp_e( gas_id, ent_num, sapi_ptr );

                continue;
              }

              for( idx = 0; idx < (uint8) GSN_NSAPI_MAX; idx++ )
              {
                /* Exit the for loop if end of valid NSAPIs */
                if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                  break;

                if( xid_i.type_field.xid_t2.alg.xid_rfc3095.nsapis &
                   (0x01 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx])) )
                {
                  /* NSAPI can start using the comp entity */
                  actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                          pcomp_e_num[T2_ALG_RFC3095]= ent_num;
                }
              }

              /* Before start using the entity perform the consistency test*/
              if(!gsn_check_pcomp_e( gas_id, sapi_ptr->pcomp_e_pool[ent_num], sapi_ptr ) )
              {
                MSG_GERAN_ERROR_2("GSN "" XID: PCI comp entity %d params not consistant",
                    gsn_extern_sapi[sapi_ptr->sapi], ent_num);
              }

              MSG_GERAN_MED_3("GSN ""LL %d, NS %d: Use RFC 3095 ent %d ",
                gsn_extern_sapi[sapi_ptr->sapi],
                  sapi_ptr->pcomp_e_pool[ent_num]->alg.xid_rfc3095.nsapis, ent_num);

              /* Encode XID response  */
              xid_i.length = 0;  /* Not the last entity */
              xid_neg_len  = gsn_encode_xid( &xid_i, xid_ex );

              /* Check if there is need to allocate a pdu item to carry the XID block */
              if( xid_rsp != NULL )
              {
                /* Append entity's comp field to the existing XID block */
                if( !gprs_pdu_append_head( &xid_rsp, xid_ex, xid_neg_len ) )
                {
                  MSG_GERAN_ERROR_1("GSN ""LL %d: Append XID block failed",
                    gsn_extern_sapi[sapi_ptr->sapi]);
                }
              }
              else
              {
                /* Create a pdu item to carry the XID block */
                xid_rsp = gsn_pdu_construct( xid_ex, xid_neg_len );
              }
            }
            else
            {
              MSG_GERAN_ERROR_1("GSN ""Invalid t2 alg %d",
                    sapi_ptr->pcomp_e_pool[ent_num]->t2_alg);
            }
          }
#endif /*  FEATURE_GSM_GPRS_SNDCP_PCOMP */
        }/* dec_xid.t2.ent[i].status == XID_VALID || XID_ABSENT */
      } /* for( i = 0; i < dec_xid.t2.ent_in_blk; i++ ) */

      /* All T2 entities have been processed. Add the T2 header in the response
      */
      if( xid_rsp )
      {
        type_hdr[XID_TYPE_PARAM_HDR_POS] = XID_TYPE_2;
        type_hdr[XID_LEN_PARAM_HDR_POS]  = type_len;
        type_len = 0;

        /* Append type header to the existing XID block */
        if( !gprs_pdu_append_head( &xid_rsp, type_hdr, XID_TYPE_HDR_LEN) )
        {
          MSG_GERAN_ERROR_1("GSN ""LL %d: Append XID block failed",
            gsn_extern_sapi[sapi_ptr->sapi]);
        }
      }
    } /* if( dec_xid.t2.status = XID_VALID )            */
  } /* if( dec_xid.t2.status !=  XID_ABSENT )           */

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
  {
    /* SELECTED entity and PCOMP values that did not become ASSIGNED
    ** after processing the XID response from the peer, shall become unassigned.
    */
    t2_alg_q_t        *q_item, *q_next_item;
    uint8             alg;

    for( alg = 0; alg <= T2_ALG_MAX; alg++ )
    {
      q_item = q_check( &sapi_ptr->t2_alg_q[alg] );

      while( q_item != NULL )
      {
        /* Get next entity from the queue */
        q_next_item = q_next( &sapi_ptr->t2_alg_q[alg], &q_item->link );

        if( q_item->pcomp_e_ptr->state == SELECTED )
        {
          /* Entity could be deleted or it could be re-proposed to the peer. TBC/TBD
          */
          gsn_del_pcomp_e( gas_id, q_item->pcomp_e_ptr->ent_num, sapi_ptr );
        }
        /* Get next entity from the queue */
        q_item = q_next_item;

      } /* end while loop */
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

  /*-------------- End of processing T2 xid type  -------------------------*/


  /* ------------- Start processing T1 xid type ---------------------------*/
  /* ----------------------------------------------------------------------*/

  if( dec_xid.t1.status !=  XID_ABSENT )
  {
    uint8  xid_ex[XID_T1_LEN_MAX];

    if( (dec_xid.t1.status == XID_INVALID) ||
        (dec_xid.t1.ent_in_blk > ENT_NUM_MAX + 1) )
    {
      /* Respond with Entity number set to 0 and App NSAPIs set to 0 */
      xid_i.type_field.xid_t1.hdr.ent_num = 0;

      /* Include type header, only one T1 entity */
      xid_i.length = XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
      gsn_bld_rej_t1_xid(  gas_id, &xid_i, xid_ex, &xid_rsp );
    }

    if( dec_xid.t1.status == XID_VALID )
    {
      for( i = 0; i < dec_xid.t1.ent_in_blk; i++ )
      {
        if( dec_xid.t1.ent[i].status == XID_INVALID )
        {
          /* Invalid entity. If entity number is decoded, respond
          ** to the specified entity otherwise set entity number
          ** and App NSAPIS to 0 in the response.
          */
          if( dec_xid.t1.ent[i].xid.hdr.ent_num > ENT_NUM_MAX )
            xid_i.type_field.xid_t1.hdr.ent_num = 0;
          else
          {
            xid_i.type_field.xid_t1.hdr.ent_num =
                dec_xid.t1.ent[i].xid.hdr.ent_num;

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

            /* If entity exists, delete comp entity */
            if( sapi_ptr->dcomp_e_pool[dec_xid.t1.ent[i].xid.hdr.ent_num] != NULL )
              gsn_del_dcomp_e( gas_id, dec_xid.t1.ent[i].xid.hdr.ent_num, sapi_ptr);

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

          }

          /* Do not include Type header. More entities might follow */
          xid_i.length = 0;
          gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

          type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

          continue;
        }

        if( dec_xid.t1.ent[i].status == XID_VALID ||
              dec_xid.t1.ent[i].status == XID_ABSENT )
        {

#ifndef FEATURE_GSM_GPRS_SNDCP_DCOMP

          /* Data compression is not supported. If entity number is
          ** decoded, respond to the specified entity, otherwise set
          ** entity number and App NSAPIS to 0 in the response.
          */
          if( dec_xid.t1.ent[i].xid.hdr.ent_num > ENT_NUM_MAX )
            xid_i.type_field.xid_t1.hdr.ent_num = 0;
          else
          {
            xid_i.type_field.xid_t1.hdr.ent_num =
                dec_xid.t1.ent[i].xid.hdr.ent_num;
          }

          /* Do not include Type header. More entities might follow */
          xid_i.length = 0;
          gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

          type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

          continue;

#else
          {
            uint8             ent_num;
            gsn_dec_t1_ent_t  *dec_ent;
            uint8             idx;
            uint16            app_nsapis;
            gsn_nsapi_t       nsapis[GSN_NSAPI_MAX];
            uint8             xid_neg_len;
            boolean           ack_nsapis   = FALSE;
            boolean           unack_nsapis = FALSE;
            boolean           ent_exist    = FALSE;
            boolean           rmv_ent      = FALSE;

            /* To avoid long lines of code */
            dec_ent = &dec_xid.t1.ent[i];
            ent_num = dec_ent->xid.hdr.ent_num;

            /* Set the header of the compression field for the response */
            xid_i.type = XID_TYPE_1;
            xid_i.type_field.xid_t1.hdr.p = 0; /* Always 0 in a response */
            xid_i.type_field.xid_t1.hdr.ent_num  = ent_num;

            /* Algorithm type is not included in the response but is needed
            ** for data comp entity initialisation.
            */
            xid_i.type_field.xid_t1.hdr.alg_type = dec_ent->xid.hdr.alg_type;

            /* Fill in the Spare bits */
            xid_i.type_field.xid_t1.hdr.x1 = 0;
            xid_i.type_field.xid_t1.hdr.x2 = 0;

            /* In the case of a collision (see TS 04.65 par 6.5.1.1.3)
            ** --------------------------------------------------------
            */
            if( collision )
            {
              if( sapi_ptr->dcomp_e_pool[ent_num] != NULL )
              {
                if( sapi_ptr->dcomp_e_pool[ent_num]->state == SELECTED )
                {
                  /* Entity number is currently selected. If P=0, assume
                  ** that the peer SNDCP entity agreed to the creation of
                  ** the proposed entity  the response from the peer was lost.
                  */
                  if( dec_ent->xid.hdr.p == 0 )
                  {
                    sapi_ptr->dcomp_e_pool[ent_num]->state = ASSIGNED;

                    if( sapi_ptr->dcomp_e_pool[ent_num]->t1_alg == T1_ALG_V42BIS )
                    {
                      if( sapi_ptr->dcomp_pool
                            [sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.dcomp1] != NULL )
                      {
                        sapi_ptr->dcomp_pool
                          [sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.dcomp1]->state = ASSIGNED;
                      }
                      else
                        MSG_GERAN_ERROR_1("GSN "" Invalid DCOMP value %d", sapi_ptr->dcomp_e_pool[ent_num]->
                          alg.xid_v42bis.dcomp1);

                      /* If the XID requested contains one or more XID params,
                      ** that are not negotiated as part of the collision resolution,
                      ** then the negotiation of these XID params shall be performed
                      ** after conclution of the collision resolution. TBD!
                      */
                      gsn_get_app_nsapis( nsapis,
                        sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.nsapis);

                      /* Link NSAPIs and comp entity */
                      for( idx=0; idx < (uint8)GSN_NSAPI_MAX; idx++ )
                      {
                        if( (uint8)nsapis[idx] == GSN_NSAPI_MAX )
                          break;

                        if( actv_nsapis[ gas_id ][nsapis[idx]] )/* NSAPIs can use the comp entity */
                          actv_nsapis[ gas_id ][nsapis[idx]]->dcomp_e_num[T1_ALG_V42BIS] = ent_num;
                      }
                    }
                  }
                  else  /* p == 1 */
                  {
                    /* Dealocate memory for comp entity and DCOMP value */
                    gsn_del_dcomp_e( gas_id, ent_num, sapi_ptr );

                    /* The originally proposed entity shall be proposed again. TBD */
                  }
                }
                else if( sapi_ptr->dcomp_e_pool[ent_num]->state == ASSIGNED )
                {
                  if( dec_ent->xid.hdr.p == 1 )
                  {
                    /* Entity number is currently assigned and P=1.
                    ** Delete original entity.
                    */
                    gsn_del_dcomp_e( gas_id, ent_num, sapi_ptr );
                  }
                }
              }
              else
              {
                if( dec_ent->xid.hdr.p == 0 )
                {
                  /* Unassigned entity number with P = 0.
                  ** Respond with App NSAPI set to 0.
                  ** Do not include Type header. More entities might follow */
                  xid_i.length = 0;
                  gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                  type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                  continue;
                }

                /* In the case of a collision in which a DCOMP value is assigned
                ** to a comp algorithm, if the peer proposes a new comp entity
                ** with the same DCOMP assigned to a different algorithm, then all
                ** comp entities using the algorithm shall be deleted.
                */
                if( (sapi_ptr->dcomp_pool
                     [dec_ent->xid.alg.xid_v42bis.dcomp1] != NULL)    &&
                    (sapi_ptr->dcomp_pool
                     [dec_ent->xid.alg.xid_v42bis.dcomp1]->state   ==
                        ASSIGNED)                                     &&
                    (dec_ent->xid.hdr.p == 1)                         &&
                    (sapi_ptr->dcomp_pool
                     [dec_ent->xid.alg.xid_v42bis.dcomp1]->val_num ==
                       dec_ent->xid.alg.xid_v42bis.dcomp1)            &&
                    (sapi_ptr->dcomp_pool
                      [dec_ent->xid.alg.xid_v42bis.dcomp1]->alg    !=
                      dec_ent->xid.hdr.alg_type) )
                {
                  gsn_del_dcomp_e_mapped_to_alg( gas_id,
                                                 sapi_ptr,
                                                 sapi_ptr->dcomp_pool
                                                 [dec_ent->xid.alg.xid_v42bis.dcomp1]->alg );
                }
              }

            }
            /*-------------- End of collision handling ---------------------------*/

            /* Excaption Handling. See TS 04.65 par 6.8.3 */
            /* -----------------------------------------------------------------*/

            /* If P bit is not set, then the XID parameters that follow shall
            ** refer to  an existing entity. Respond with App NSAPIs
            ** set to 0 if entity doesn't exist or if T1 algorithm is not supported
            */
            if( !dec_ent->xid.hdr.p  )
            {
              if( sapi_ptr->dcomp_e_pool[ent_num] == NULL )
              {
                /* Do not include Type header. More entities might follow */
                xid_i.length = 0;
                gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                MSG_GERAN_MED_0("GSN ""XID req: P=0 for unassigned entity");

                continue;
              }
            }

            if( dec_ent->xid.hdr.p && dec_ent->xid.hdr.length  )
            {
              if( (sapi_ptr->dcomp_e_pool[ent_num] != NULL) &&
                  (sapi_ptr->dcomp_e_pool[ent_num]->state == ASSIGNED) )
              {
                /* Assigned entity with P=1. Ignore algorithm and DCOMP value if
                ** the same as the previously-assined values. Otherwise,
                ** respond to XID request with "App NSAPIs" field set to zero and send
                ** SNSM_STATUS.REQ with cause "invalid XID command" to SM.
                ** SM shall deactivate all PDP contexts for this SAPI.
                */
                if( (sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.dcomp1 !=
                      dec_ent->xid.alg.xid_v42bis.dcomp1)  ||
                    (sapi_ptr->dcomp_e_pool[ent_num]->t1_alg !=
                      dec_ent->xid.hdr.alg_type ) )
                {
                  /* Last entity. Include type header */
                  xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                  gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                  /* Delete comp entity */
                  gsn_del_dcomp_e( gas_id, ent_num, sapi_ptr);

                  /* Send SNSM_STATUS.REQ to SM */
                  gsn_send_snsm_stat_req( gas_id,
                                          sapi_ptr->sapi,
                                          sapi_ptr->tlli,
                                          (uint8)SNSM_INVALID_XID_RESP );

                  return( xid_rsp );
                }
              }

              /* If DCOMP specified is already assigned to a different comp algorithm,
              ** then, respond to XID request with App NSAPIs" field set to zero and send
              ** SNSM_STATUS.REQ with cause "invalid XID command" to SM.
              ** SM shall deactivate all PDP contexts for this SAPI.
              */
              if( ( sapi_ptr->dcomp_pool
                      [dec_ent->xid.alg.xid_v42bis.dcomp1] != NULL) &&
                  ( sapi_ptr->dcomp_pool
                      [dec_ent->xid.alg.xid_v42bis.dcomp1]->alg !=
                    dec_ent->xid.hdr.alg_type) )
              {
                /* Last entity. Include type header */
                xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                /* If entity exists, delete comp entity */
                if( sapi_ptr->dcomp_e_pool[ent_num] != NULL )
                  gsn_del_dcomp_e( gas_id, ent_num, sapi_ptr);

                /* Send SNSM_STATUS.REQ to SM */
                gsn_send_snsm_stat_req( gas_id,
                                        sapi_ptr->sapi,
                                        sapi_ptr->tlli,
                                        (uint8)SNSM_INVALID_XID_RESP );

                return( xid_rsp );

              }

              /* If one or more new DCOMP values are specified for an existing
              ** comp algorithm, then respond to XID request with App NSAPIs"
              ** field set to zero and send SNSM_STATUS.REQ with cause
              ** "invalid XID command" to SM.SM shall deactivate all PDP
              ** contexts for this SAPI.
              */
              if( (sapi_ptr->dcomp_pool
                      [dec_ent->xid.alg.xid_v42bis.dcomp1] == NULL) &&
                  (sapi_ptr->dcomp_e_pool[ent_num] != NULL )        &&
                  (sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.dcomp1 !=
                      dec_ent->xid.alg.xid_v42bis.dcomp1) )
              {
                /* Last entity. Include type header */
                xid_i.length = type_len + XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
                gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_rsp );

                /* If entity exists, delete comp entity */
                if( sapi_ptr->dcomp_e_pool[ent_num] != NULL )
                  gsn_del_dcomp_e( gas_id, ent_num, sapi_ptr);

                /* Send SNSM_STATUS.REQ to SM */
                gsn_send_snsm_stat_req( gas_id,
                                        sapi_ptr->sapi,
                                        sapi_ptr->tlli,
                                        (uint8)SNSM_INVALID_XID_RESP );

                return( xid_rsp );
              }

              /* Check if the DCOMP value already exist
              */
              if( (sapi_ptr->dcomp_pool
                    [dec_ent->xid.alg.xid_v42bis.dcomp1] != NULL) &&
                  (sapi_ptr->dcomp_pool
                    [dec_ent->xid.alg.xid_v42bis.dcomp1]->state == SELECTED) )
              {
                /* After collision reslolution there shouldn't be any SELECTED DCOMP values */
                MSG_GERAN_ERROR_1("GSN ""LL %d: DCOMP val SELECTED after collision res",
                        gsn_extern_sapi[sapi_ptr->sapi]);
              }

              /* DCOMP value is not included in the response but is needed
              ** for Data comp entity initialisation.
              */
              xid_i.type_field.xid_t1.alg.xid_v42bis.dcomp1 =
                              dec_ent->xid.alg.xid_v42bis.dcomp1;
            }
            /* ---------- End of excaption handling --------------------------*/

            /* Check if compression entity already exist
            */
            if( (sapi_ptr->dcomp_e_pool[ent_num]        != NULL)     &&
                (sapi_ptr->dcomp_e_pool[ent_num]->state == ASSIGNED) )
            {
              /* Compression entity exists. */
              ent_exist = TRUE;
            }
            else if( (sapi_ptr->dcomp_e_pool[ent_num]          != NULL)     &&
                     (sapi_ptr->dcomp_e_pool[ent_num]->state   == SELECTED) )
            {
              /* After collision reslolution there shouldn't be any SELECTED entities */
              MSG_GERAN_ERROR_1("GSN ""LL %d: Ent SELECTED after collision res",
                      gsn_extern_sapi[sapi_ptr->sapi]);
            }

            /* Process the parameters included in the decoded compression field */

            /* ---------------------- Applicable NSAPIs ------------------------*/

            /* If App NSAPIs is 0. Delete compression entity */
            if( ent_exist && !dec_ent->xid.alg.xid_v42bis.nsapis )
            {
              /* Not the last entity. Do not Include type header */
              xid_i.length = 0;
              gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
              type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

              gsn_del_dcomp_e( gas_id, ent_num, sapi_ptr );

              continue;
            }

            /* The response will be the same if no exception occurs */
            xid_i.type_field.xid_t1.alg.xid_v42bis.nsapis =
                            dec_ent->xid.alg.xid_v42bis.nsapis;

            /* Increment comp entity's parameter length so that
            ** the App NSAPIs field will be included in the XID response
            ** Note that in the response the Propose bit (P) is set to 0.
            */
            xid_i.type_field.xid_t1.hdr.length = APP_NSAPIS_LEN;

            /* Check if any NSAPIs have been removed from an existing comp entity
            */
            if( ent_exist )
            {
              app_nsapis = (dec_ent->xid.alg.xid_v42bis.nsapis^
                            sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.nsapis) &
                            sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.nsapis;

              /* LL_ESTABLISH primitive shall be used if one or more NSAPIs are
              ** removed from existing comp entities used with ack NSAPIs, except
              ** when all NSAPIs using the comp entity are removed,
              ** or LLC is already in ADM.
              */
              if( app_nsapis )
              {
                if( (sapi_ptr->dcomp_e_pool[ent_num]->op_mode == ACK) &&
                    (xid_on_est_msg == FALSE) &&
                    ( (dec_ent->xid.alg.xid_v42bis.nsapis != 0) ||
                      (sapi_ptr->state != IDLE) ) )
                {
                  /* Negotiation is not possible. Respond with the
                  ** priviously-negotiated App NSAPIs value
                  */
                  xid_i.type_field.xid_t1.alg.xid_v42bis.nsapis =
                   sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.nsapis;
                }
                else
                {
                  for( idx = 0; idx < (uint8)GSN_NSAPI_MAX; idx++ )
                  {
                    /* Exit the for loop if end of valid NSAPIs */
                    if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                      break;

                    if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx]) ) )
                    {
                      /* Remove comp entity from NSAPI if it has been using the entity */
                      if( actv_nsapis[ gas_id ][nsapis_on_sapi[idx]] )
                        actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                          dcomp_e_num[T1_ALG_V42BIS]= NO_COMP_ENT;
                    }
                  }
                }
              }

              /* Check if any NSAPIs have been added to the existing comp entity
              */
              app_nsapis = (dec_ent->xid.alg.xid_v42bis.nsapis ^
                             sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.nsapis) &
                            dec_ent->xid.alg.xid_v42bis.nsapis;

              if( app_nsapis )
              {
                for( idx = 0; idx < (uint8)GSN_NSAPI_MAX; idx++ )
                {
                  /* Exit the for loop if end of valid NSAPIs */
                  if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                    break;

                  if( app_nsapis & ( 0x1 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx]) ) )
                  {
                    /* If data compression is enabled in the NSAPI and comp entity
                    ** use the same operation mode, then add NSAPI to the comp entity.
                    */
                    if( actv_nsapis[ gas_id ][nsapis_on_sapi[idx]] )
                    {
                      if( (actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->op_mode ==
                              sapi_ptr->dcomp_e_pool[ent_num]->op_mode) &&
                          (actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->data_comp) )
                      {
                        actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                            dcomp_e_num[T1_ALG_V42BIS]= ent_num;
                      }
                      else
                      {
                        /* Clear bit in App NSAPIs */
                        /*lint -e{502} expected unsigned type for bit operation */
                        xid_i.type_field.xid_t1.alg.xid_v42bis.nsapis &=
                          ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( nsapis_on_sapi[idx] ) );
                      }
                    }
                  }
                }
              }
            }
            else
            {
              /* Comp entity does not exist.
              ** If more than one NSAPI is set, check if they are using
              ** the same operation mode (ACK or UNACK). ACK and UNACK
              ** NSAPIs shall not share the same compression entity
              */
              gsn_get_app_nsapis( nsapis,
                dec_ent->xid.alg.xid_v42bis.nsapis);

              for( idx=0; (idx < (uint8)GSN_NSAPI_MAX) && ((uint8)nsapis[idx] != GSN_NSAPI_MAX) ; idx++ )
              {
                /* Check if data compression is enabled in the NSAPI  */
                if( (actv_nsapis[ gas_id ][nsapis[idx]] != NULL)  &&
                    (actv_nsapis[ gas_id ][nsapis[idx]]->data_comp) )
                {
                  if( actv_nsapis[ gas_id ][nsapis[idx]]->op_mode == ACK)
                    ack_nsapis = TRUE;
                  else
                    unack_nsapis = TRUE;
                }
                else
                {
                  /* Clear bit in App NSAPIs */
                  /*lint -e{502} expected unsigned type for bit operation */
                  xid_i.type_field.xid_t1.alg.xid_v42bis.nsapis &=
                    ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( nsapis[idx] ) );
                }
              }

              if( ack_nsapis && unack_nsapis )
              {
                /* Chose which NSAPIs will use the comp entity
                ** and fill in the field for the XID response. XID_HOLE!
                */
                if( gsn_get_ack_nsapis_on_sapi( sapi_ptr ) )
                {
                  xid_i.type_field.xid_t1.alg.xid_v42bis.nsapis =
                                    gsn_get_ack_nsapis_on_sapi( sapi_ptr );
                  unack_nsapis = FALSE;
                }
                else if( gsn_get_unack_nsapis_on_sapi( sapi_ptr ) )
                {
                  xid_i.type_field.xid_t1.alg.xid_v42bis.nsapis =
                                  gsn_get_unack_nsapis_on_sapi( sapi_ptr );
                  ack_nsapis = FALSE;
                }
                else
                {
                  /* Something is wrong! No active NSAPIs on an active SAPI */
                  MSG_GERAN_ERROR_1("GSN ""LL %d: No Actv NSAPIs on an Actv SAPI",
                    gsn_extern_sapi[sapi_ptr->sapi]);
                }
              }

              /* If no App NSAPIs left, respont with App NSAPIs set to 0. */
              if( !xid_i.type_field.xid_t1.alg.xid_v42bis.nsapis )
              {
                xid_i.length = 0;
                gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
                type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

                continue;
              }
            }

            /*-------------------- Process parameter P0  -------------------*/
            if( ( dec_ent->xid.hdr.p  &&
                ( dec_ent->xid.hdr.length >= 4)) ||
                ( !dec_ent->xid.hdr.p &&
                ( dec_ent->xid.hdr.length >= 3)) )
            {
              /* Check if we support this value. TBC.
              ** The response will be the same if no exception occurs
              */
              xid_i.type_field.xid_t1.alg.xid_v42bis.p0 =
                               dec_ent->xid.alg.xid_v42bis.p0;

              /* Increment comp entity's parameter length so that
              ** the P0 field will be included in the XID response
              ** Note that in the response the Propose bit (P) is set to 0.
              */
              xid_i.type_field.xid_t1.hdr.length += V42BIS_P0_LEN;

              /* If LL_XID primitive is used for XID negotiation,
              ** any parameter of existing compression entities used with Ack NSAPIs
              ** are non-negotiable. Respond with the priviously-negotiated values.
              ** If one or more parameters of a comp entity used with Unack NSAPIs
              ** are changed, the comp entity shall be reset locally upon completion
              ** of the XID negotiation.
              */
              if( ent_exist &&
                  ( dec_ent->xid.alg.xid_v42bis.p0 !=
                     sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p0) )
              {
                if( sapi_ptr->dcomp_e_pool[ent_num]->op_mode == ACK )
                {
                  if( xid_on_est_msg == FALSE )
                  {
                    /* Respond with the priviously-negotiated P0 value */
                    xid_i.type_field.xid_t1.alg.xid_v42bis.p0 =
                      sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p0;
                  }
                }
                else if( sapi_ptr->dcomp_e_pool[ent_num]->op_mode == UNACK )
                {
                  /* Reset localy the comp entity */
                  gsn_reset_dcomp_ent( gas_id, sapi_ptr->dcomp_e_pool[ent_num] );
                }

                /* Update entity's parameter value */
                sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p0 =
                                  xid_i.type_field.xid_t1.alg.xid_v42bis.p0;
              }

              /* ------------------ Process parameter P1 -------------------- */
              if( ( dec_ent->xid.hdr.p  &&
                  ( dec_ent->xid.hdr.length >= 6)) ||
                  ( !dec_ent->xid.hdr.p &&
                  ( dec_ent->xid.hdr.length >= 5)) )
              {
                if( ( dec_ent->xid.alg.xid_v42bis.p1 < V42BIS_P1_MIN ) ||
                    ( dec_ent->xid.alg.xid_v42bis.p1 > V42BIS_P1_MAX )   )
                {
                  if( ent_exist )
                    xid_i.type_field.xid_t1.alg.xid_v42bis.p1 =
                        sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p1;
                  else
                    xid_i.type_field.xid_t1.alg.xid_v42bis.p1 = V42BIS_P1_DEF;
                }
                else
                {
                  /* The response will be the same if no exception occurs.
                  */
                  xid_i.type_field.xid_t1.alg.xid_v42bis.p1 =
                                dec_ent->xid.alg.xid_v42bis.p1;
                }

                /* Increment comp entity's parameter length so that
                ** the P1 field will be included in the XID response
                ** Note that in the response the Propose bit (P) is set to 0.
                */
                xid_i.type_field.xid_t1.hdr.length += V42BIS_P1_LEN;

                /* If LL_XID primitive is used for XID negotiation,
                ** any parameter of existing compression entities used with Ack NSAPIs
                ** are non-negotiable. Respond with the priviously-negotiated values.
                ** If one or more parameters of a comp entity used with Unack NSAPIs
                ** are changed, the comp entity shall be reset locally upon completion
                ** of the XID negotiation.
                */
                if( ent_exist &&
                    (dec_ent->xid.alg.xid_v42bis.p1 !=
                       sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p1) )
                {
                  if( sapi_ptr->dcomp_e_pool[ent_num]->op_mode == ACK )
                  {
                    if( xid_on_est_msg == FALSE )
                    {
                      /* Respond with the priviously-negotiated P1 value */
                      xid_i.type_field.xid_t1.alg.xid_v42bis.p1 =
                        sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p1;
                    }
                  }
                  else if( sapi_ptr->dcomp_e_pool[ent_num]->op_mode == UNACK )
                  {
                    /* Reset localy the comp entity */
                    gsn_reset_dcomp_ent( gas_id, sapi_ptr->dcomp_e_pool[ent_num] );
                  }

                  /* Update entity's parameter value */
                  sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p1 =
                                      xid_i.type_field.xid_t1.alg.xid_v42bis.p1;
                }

                /* ------------- Process parameter P2 ------------------------- */
                if( ( dec_ent->xid.hdr.p  &&
                    ( dec_ent->xid.hdr.length == 7)) ||
                    ( !dec_ent->xid.hdr.p &&
                    ( dec_ent->xid.hdr.length == 6)) )
                {
                  if( (dec_ent->xid.alg.xid_v42bis.p2 < V42BIS_P2_MIN) ||
                      (dec_ent->xid.alg.xid_v42bis.p2 > V42BIS_P2_MAX))
                  {
                    if( ent_exist )
                      xid_i.type_field.xid_t1.alg.xid_v42bis.p2 =
                          sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p2;
                    else
                      xid_i.type_field.xid_t1.alg.xid_v42bis.p2 = V42BIS_P2_DEF;
                  }
                  else
                  {
                    /* Check if we support this value. TBC.
                    ** The response will be the same if no exception occurs.
                    */
                    xid_i.type_field.xid_t1.alg.xid_v42bis.p2 =
                                  dec_ent->xid.alg.xid_v42bis.p2;
                  }

                  /* Increment comp entity's parameter length so that
                  ** the P2 field will be included in the XID response
                  ** Note that in the response the Propose bit (P) is set to 0.
                  */
                  xid_i.type_field.xid_t1.hdr.length += V42BIS_P2_LEN;

                  /* Last parameter. Add entity's length to overall T1 parameter length. */
                  type_len +=  xid_i.type_field.xid_t1.hdr.length + XID_FIELD_HDR_P0_LEN;

                  /* If LL_XID primitive is used for XID negotiation,
                  ** any parameter of existing compression entities used with Ack NSAPIs
                  ** are non-negotiable. Respond with the priviously-negotiated values.
                  ** If one or more parameters of a comp entity used with Unack NSAPIs
                  ** are changed, the comp entity shall be reset locally upon completion
                  ** of the XID negotiation.
                  */
                  if( ent_exist &&
                      (dec_ent->xid.alg.xid_v42bis.p2 !=
                         sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p2) )
                  {
                    if( sapi_ptr->dcomp_e_pool[ent_num]->op_mode == ACK )
                    {
                      if( xid_on_est_msg == FALSE )
                      {
                        /* Respond with the priviously-negotiated P2 value */
                        xid_i.type_field.xid_t1.alg.xid_v42bis.p2 =
                          sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p2;
                      }
                    }
                    else if( sapi_ptr->dcomp_e_pool[ent_num]->op_mode == UNACK )
                    {
                      /* Reset localy the comp entity */
                      gsn_reset_dcomp_ent( gas_id, sapi_ptr->dcomp_e_pool[ent_num] );
                    }

                    /* Update entity's parameter value */
                    sapi_ptr->dcomp_e_pool[ent_num]->alg.xid_v42bis.p2 =
                                      xid_i.type_field.xid_t1.alg.xid_v42bis.p2;
                  }
                }
                else if(( dec_ent->xid.hdr.p  &&
                        ( dec_ent->xid.hdr.length < 7)) ||
                        ( !dec_ent->xid.hdr.p &&
                        ( dec_ent->xid.hdr.length < 6)) )
                {
                  /* Parameter is not included.
                  ** Set the preferred xid parameter values for the response
                  */
                  xid_i.type_field.xid_t1.alg.xid_v42bis.p2 = V42BIS_P2_DEF;

                  xid_i.type_field.xid_t1.hdr.length += V42BIS_P2_LEN;

                  type_len += XID_FIELD_HDR_P0_LEN + V42BIS_LEN_P0_MAX;
                }
                else
                {
                  MSG_GERAN_HIGH_2("GSN ""LL %d: Invalid XID header len %d",
                    gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.hdr.length);

                }/* ------------- END of Process parameter P2 ------------------------- */
              }
              else
              {
                /* Parameters are not included.
                ** Set the preferred xid parameter values for the response
                */
                xid_i.type_field.xid_t1.alg.xid_v42bis.p1 = V42BIS_P1_DEF;
                xid_i.type_field.xid_t1.alg.xid_v42bis.p2 = V42BIS_P2_DEF;

                xid_i.type_field.xid_t1.hdr.length += V42BIS_P1_LEN + V42BIS_P2_LEN;

                type_len += XID_FIELD_HDR_P0_LEN + V42BIS_LEN_P0_MAX;

              }/* ------------- END of Process parameter P1 ------------------------- */
            }
            else
            {
              /* Parameters are not included.
              ** Set the preferred xid parameter values for the response
              */
              xid_i.type_field.xid_t1.alg.xid_v42bis.p0 = V42BIS_P0_DEF;
              xid_i.type_field.xid_t1.alg.xid_v42bis.p1 = V42BIS_P1_DEF;
              xid_i.type_field.xid_t1.alg.xid_v42bis.p2 = V42BIS_P2_DEF;

              xid_i.type_field.xid_t1.hdr.length +=
                V42BIS_P0_LEN + V42BIS_P1_LEN + V42BIS_P2_LEN;

              type_len += XID_FIELD_HDR_P0_LEN + V42BIS_LEN_P0_MAX;

            } /* ------------- END of Process parameter P0 ------------------------- */

            /* -------End of processing compression field xid parameters ------------*/

            if( sapi_ptr->dcomp_e_pool[ent_num]  == NULL )
            {
              /* Compression entity does not exist. Create and initialise entity */
              if( gsn_create_dcomp_e( &xid_i.type_field.xid_t1, sapi_ptr ) )
              {
                /* Set entity's state to ASSIGNED. Entity is ready for
                ** use after the XID response is sent to the peer
                */
                sapi_ptr->dcomp_e_pool[ent_num]->state = ASSIGNED;

                /* Set entity's operation mode */
                if( ack_nsapis )
                  sapi_ptr->dcomp_e_pool[ent_num]->op_mode = ACK;
                else
                  sapi_ptr->dcomp_e_pool[ent_num]->op_mode = UNACK;

                /* Initialise V42 bis compression entity */
                if( !gsn_v42bis_comp_init( gas_id, sapi_ptr->dcomp_e_pool[ent_num] ) )
                {
                  MSG_GERAN_HIGH_1("GSN ""Data comp ent %d couldn't be initialised",ent_num);
                  rmv_ent = TRUE;
                }
              }
              else
              {
                MSG_GERAN_HIGH_1("GSN ""Data comp ent %d could not be created ",ent_num);
                rmv_ent = TRUE;
              }
            }

            if( sapi_ptr->dcomp_pool
                [dec_ent->xid.alg.xid_v42bis.dcomp1]  == NULL )
            {
              /* Create DCOMP value and link it to the algorithm */

              if( dec_ent->xid.alg.xid_v42bis.dcomp1 )
              {
                /* Ensure that DCOMP value is valid. DCOMP 15 is reserved for future use */
                if( (dec_ent->xid.alg.xid_v42bis.dcomp1 < DCOMP_VAL_MAX )   &&
                    (sapi_ptr->dcomp_pool[dec_ent->xid.alg.xid_v42bis.dcomp1] == NULL) )
                {
                  sapi_ptr->dcomp_pool[ dec_ent->xid.alg.xid_v42bis.dcomp1 ] =
                        GPRS_MEM_MALLOC( sizeof(dcomp_t) );
                  GSN_ASSERT( sapi_ptr->dcomp_pool[ dec_ent->xid.alg.xid_v42bis.dcomp1 ] != NULL );

                  sapi_ptr->dcomp_pool[ dec_ent->xid.alg.xid_v42bis.dcomp1 ]->alg     = V42BIS_DCOMP_MAX;
                  sapi_ptr->dcomp_pool[ dec_ent->xid.alg.xid_v42bis.dcomp1 ]->state   = UNASSIGNED;
                  sapi_ptr->dcomp_pool[ dec_ent->xid.alg.xid_v42bis.dcomp1 ]->val_num =
                                                                dec_ent->xid.alg.xid_v42bis.dcomp1;
                  sapi_ptr->dcomp_pool[ dec_ent->xid.alg.xid_v42bis.dcomp1 ]->state  = ASSIGNED;
                }
                else
                {
                  MSG_GERAN_HIGH_2("GSN ""XID %d: Inval DCOMP val %d",
                    gsn_extern_sapi[sapi_ptr->sapi],dec_ent->xid.alg.xid_v42bis.dcomp1);
                  rmv_ent = TRUE;
                }
              }
            }

            if( rmv_ent == TRUE )
            {
              /* Data compression entity could not be created or initialised.
              ** Delete data compression and inform the peer
              */
              xid_i.length = 0;
              gsn_bld_rej_t1_xid( gas_id, &xid_i, xid_ex, &xid_rsp );
              type_len += XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

              gsn_del_dcomp_e( gas_id, ent_num, sapi_ptr );

              continue;
            }

            for( idx = 0; idx < (uint8)GSN_NSAPI_MAX; idx++ )
            {
              /* Exit the for loop if end of valid NSAPIs */
              if( nsapis_on_sapi[idx] == GSN_INVAL_NSAPI )
                break;

              if( xid_i.type_field.xid_t1.alg.xid_v42bis.nsapis &
                  (0x01 << GSN_EXTERN_NSAPI(nsapis_on_sapi[idx])) )
              {
                /* NSAPI can start using the comp entity */
                actv_nsapis[ gas_id ][nsapis_on_sapi[idx]]->
                          dcomp_e_num[T1_ALG_V42BIS]= ent_num;
              }
            }

            /* Encode XID response  */
            xid_i.length = 0;  /* Not the last entity */
            xid_neg_len = gsn_encode_xid( &xid_i, xid_ex );

            /* Check if there is need to allocate a pdu item to carry the XID block */
            if( xid_rsp != NULL )
            {
              /* Append entity's comp field to the existing XID block */
              if( !gprs_pdu_append_head( &xid_rsp, xid_ex, xid_neg_len ) )
              {
                MSG_GERAN_ERROR_1("GSN ""LL %d: Append XID block failed",
                  gsn_extern_sapi[sapi_ptr->sapi]);
              }
            }
            else
            {
              /* Create a pdu item to carry the XID block */
              xid_rsp = gsn_pdu_construct( xid_ex, xid_neg_len );
            }
          }
#endif /*  FEATURE_GSM_GPRS_SNDCP_DCOMP */
        }  /* dec_xid.t1.ent[i].status == XID_VALID || XID_ABSENT */
      } /* for( i = 0; i < dec_xid.t1.ent_in_blk; i++ ) */

      /* All T1 entities have been processed. Add the T1 header in the response
      */
      if( xid_rsp )
      {
        type_hdr[XID_TYPE_PARAM_HDR_POS] = XID_TYPE_1;
        type_hdr[XID_LEN_PARAM_HDR_POS]  = type_len;
        type_len = 0;

        /* Append type header to the existing XID block */
        if( !gprs_pdu_append_head( &xid_rsp, type_hdr, XID_TYPE_HDR_LEN) )
        {
          MSG_GERAN_ERROR_1("GSN ""LL %d: Append XID block failed",
            gsn_extern_sapi[sapi_ptr->sapi]);
        }
      }
    } /* if( dec_xid.t1.status = XID_VALID )            */
  } /* if( dec_xid.t1.status !=  XID_ABSENT )           */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
  {
    /* SELECTED entity and DCOMP values that did not become ASSIGNED
    ** after processing the XID response from the peer, shall become unassigned.
    */
    t1_alg_q_t        *q_item, *q_next_item;
    /* Change to uint8 when more data compression algorithms added and
    ** T1_ALG_MAX != 0. int8 is used to avoid unassigned comparison with 0 warning.
    */
    int8            alg;

    for( alg = 0; alg <= T1_ALG_MAX; alg++ )
    {
      q_item = q_check( &sapi_ptr->t1_alg_q[alg] );

      while( q_item != NULL )
      {
        /* Get next entity from the queue */
        q_next_item = q_next( &sapi_ptr->t1_alg_q[alg], &q_item->link );

        if( q_item->dcomp_e_ptr->state == SELECTED )
        {
          /* Entity could be deleted or it could be re-proposed to the peer. TBC/TBD
          */
          gsn_del_dcomp_e( gas_id, q_item->dcomp_e_ptr->ent_num, sapi_ptr );
        }
        /* Get next entity from the queue */
        q_item = q_next_item;

      } /* end while loop */
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

  /*-------------- End of processing T1 xid type  -------------------------*/


  /* ------------- Start processing  T0 xid type  --------------------- */
  /* -------------------------------------------------------------------*/

  if( (dec_xid.t0.status == XID_INVALID) || (dec_xid.t0.status == XID_VALID) )
  {
    /* Only SNDCP Version 0 is supported */

    /* Create a pdu item to carry the XID block */
    if( xid_rsp )
    {
      /* Append type 0 to the existing XID block */
      if( !gprs_pdu_append_head( &xid_rsp, (void*)xid_t0_def, XID_DEF_T0_LEN_BLK) )
      {
        MSG_GERAN_ERROR_1("GSN ""LL %d: Append XID block failed",
          gsn_extern_sapi[sapi_ptr->sapi]);
      }
    }
  }
  /* --------------- End processing T0 xid type --------------------------*/

  return( xid_rsp );
}

/*===========================================================================
===
===  FUNCTION      gsn_encode_xid()
===
===  DESCRIPTION
===
===   Encodes XID block from internal format ( gsn_xid_block_t ) into external format
===   ( array of bytes  )
===
===   *xid_src: XID block in internal format
===   *xid_dst: Points to an array of bytes that holds the XID block
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    Length of buffer that holds the XID block in byte format
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

uint8 gsn_encode_xid( const gsn_xid_block_t *xid_src, uint8 *xid_dst  )
{

  /* offset from the beginning of the message to the current item */
  uint16 msg_pos = 0;

  if( xid_src == NULL )
  {
    xid_dst = NULL;

    return( 0 );
  }
  else
  {
    switch( xid_src->type )
    {
      case XID_TYPE_0:

        /* format Type 0 XID block */
        gpackb(xid_src->type, xid_dst, &msg_pos, GSIZE(gsn_xid_block_t, type));
        gpackb(xid_src->length, xid_dst, &msg_pos, GSIZE(gsn_xid_block_t, length));
        gpackb(xid_src->type_field.xid_t0.ver, xid_dst,
                                            &msg_pos, GSIZE(gsn_xid_t0_t, ver));

        if( (msg_pos / XID_BYTE_SIZE) > XID_T0_LEN_MAX )
          MSG_GERAN_ERROR_0("GSN ""XID encode. Invalid XID T0 block length");

        break;

      case XID_TYPE_1:

        if( xid_src->length != 0 )
        {
          /* If T1 header lenght is not zero ( set to zero by the calling function to inticate
          ** that the entity is not the last entity in the T1 XID Block) include T1 header.
          */
          gpackb(xid_src->type, xid_dst, &msg_pos, GSIZE(gsn_xid_block_t, type));
          gpackb(xid_src->length, xid_dst, &msg_pos, GSIZE(gsn_xid_block_t, length));
        }

        switch( xid_src->type_field.xid_t1.hdr.alg_type )
        {
        case T1_ALG_V42BIS:

          gpackb(xid_src->type_field.xid_t1.hdr.p, xid_dst, &msg_pos,
                  GSIZE(gsn_xid_comp_hdr_t, p));
          gpackb(xid_src->type_field.xid_t1.hdr.x1, xid_dst, &msg_pos,
                  GSIZE(gsn_xid_comp_hdr_t, x1));
          gpackb(xid_src->type_field.xid_t1.hdr.ent_num, xid_dst, &msg_pos,
                  GSIZE(gsn_xid_comp_hdr_t, ent_num));

          /* If P set to "0", do not include octet 2 and octets 4 to x-1.
          ** See TS04.64 par 6.5.1.1.2
          */
          if( xid_src->type_field.xid_t1.hdr.p )
          {
            gpackb(xid_src->type_field.xid_t1.hdr.x2, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_comp_hdr_t, x2));
            gpackb(xid_src->type_field.xid_t1.hdr.alg_type, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_comp_hdr_t, alg_type));
          }

          gpackb(xid_src->type_field.xid_t1.hdr.length, xid_dst, &msg_pos,
                  GSIZE(gsn_xid_comp_hdr_t, length));

          if( xid_src->type_field.xid_t1.hdr.p )
          {
            gpackb(xid_src->type_field.xid_t1.alg.xid_v42bis.dcomp1, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_dcomp_v42bis_t, dcomp1));
            gpackb(xid_src->type_field.xid_t1.alg.xid_v42bis.dcomp2, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_dcomp_v42bis_t, dcomp2));
          }

          /* Check if parameter field shall be included by checking the
          ** length of compression parametes in respect to P bit. See TS04.65 table 7(2nd table)
          */
          if( ( xid_src->type_field.xid_t1.hdr.p  &&
              ( xid_src->type_field.xid_t1.hdr.length >= 3)) ||
              ( !xid_src->type_field.xid_t1.hdr.p &&
              ( xid_src->type_field.xid_t1.hdr.length >= 2)) )
          {
            gpackw(xid_src->type_field.xid_t1.alg.xid_v42bis.nsapis, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_dcomp_v42bis_t, nsapis));
          }

          if( ( xid_src->type_field.xid_t1.hdr.p &&
              ( xid_src->type_field.xid_t1.hdr.length >= 4)) ||
              ( !xid_src->type_field.xid_t1.hdr.p &&
              ( xid_src->type_field.xid_t1.hdr.length >= 3)) )
          {
            gpackb(xid_src->type_field.xid_t1.alg.xid_v42bis.p0, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_dcomp_v42bis_t, p0));
          }

          if( ( xid_src->type_field.xid_t1.hdr.p  &&
              ( xid_src->type_field.xid_t1.hdr.length >= 6)) ||
              ( !xid_src->type_field.xid_t1.hdr.p &&
              ( xid_src->type_field.xid_t1.hdr.length >= 5)) )
          {
            gpackw(xid_src->type_field.xid_t1.alg.xid_v42bis.p1, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_dcomp_v42bis_t, p1));
          }

          if( ( xid_src->type_field.xid_t1.hdr.p  &&
              ( xid_src->type_field.xid_t1.hdr.length == 7)) ||
              ( !xid_src->type_field.xid_t1.hdr.p  &&
              ( xid_src->type_field.xid_t1.hdr.length == 6)) )
          {
            gpackb(xid_src->type_field.xid_t1.alg.xid_v42bis.p2, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_dcomp_v42bis_t, p2));
          }

          break;

        default:
          MSG_GERAN_ERROR_0("GSN ""XID encode. Invalid XID T1 algorithm type ");
          break;
        }

        if( (msg_pos / XID_BYTE_SIZE) >
            (XID_T1_LEN_MAX + XID_FIELD_HDR_P1_LEN + XID_TYPE_HDR_LEN ) )
          MSG_GERAN_ERROR_0("GSN ""XID encode. Invalid XID T1 block length");

        break;

      case XID_TYPE_2:

        if( xid_src->length != 0 )
        {
          /* If T2 header lenght is not zero ( set to zero by the calling function to inticate
          ** that the entity is not the last entity in the T2 XID Block) include T1 header.
          */
          gpackb(xid_src->type, xid_dst, &msg_pos, GSIZE(gsn_xid_block_t, type));
          gpackb(xid_src->length, xid_dst, &msg_pos, GSIZE(gsn_xid_block_t, length));
        }

        gpackb(xid_src->type_field.xid_t2.hdr.p, xid_dst, &msg_pos,
                  GSIZE(gsn_xid_comp_hdr_t, p));

        gpackb(xid_src->type_field.xid_t2.hdr.x1, xid_dst, &msg_pos,
                GSIZE(gsn_xid_comp_hdr_t, x1));

        gpackb(xid_src->type_field.xid_t2.hdr.ent_num, xid_dst, &msg_pos,
                  GSIZE(gsn_xid_comp_hdr_t, ent_num));

        /* If P set to "0", do not include octet 2 and octets 4 to x-1.
        ** See TS04.64 par 6.5.1.1.2
        */
        if( xid_src->type_field.xid_t2.hdr.p  )
        {
          gpackb(xid_src->type_field.xid_t2.hdr.x2, xid_dst, &msg_pos,
                  GSIZE(gsn_xid_comp_hdr_t, x2));
          gpackb(xid_src->type_field.xid_t2.hdr.alg_type, xid_dst, &msg_pos,
                  GSIZE(gsn_xid_comp_hdr_t, alg_type));
        }

        gpackb(xid_src->type_field.xid_t2.hdr.length, xid_dst, &msg_pos,
                 GSIZE(gsn_xid_comp_hdr_t, length));

        switch( xid_src->type_field.xid_t2.hdr.alg_type )
        {
        case T2_ALG_RFC1144:

          if( xid_src->type_field.xid_t2.hdr.p )
          {
            gpackb(xid_src->type_field.xid_t2.alg.xid_rfc1144.pcomp1, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_1144_t, pcomp1));
            gpackb(xid_src->type_field.xid_t2.alg.xid_rfc1144.pcomp2, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_1144_t, pcomp2));
          }

          /* Check if parameter field shall be included by checking the
          ** length of the compression parameters in respect to P bit. See TS04.65 table 5
          */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 3)) ||
              ( xid_src->type_field.xid_t2.hdr.p == 0 &&
              ( xid_src->type_field.xid_t2.hdr.length >= 2)) )
          {
            gpackw(xid_src->type_field.xid_t2.alg.xid_rfc1144.nsapis, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_1144_t, nsapis));
          }

          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length == 4)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length == 3)) )
          {
            gpackb((uint8)(xid_src->type_field.xid_t2.alg.xid_rfc1144.s0_1 ), xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_1144_t, s0_1));
          }

          if((msg_pos / XID_BYTE_SIZE ) >
             (RFC1144_LEN_P1_MAX + XID_FIELD_HDR_P1_LEN + XID_TYPE_HDR_LEN))
          MSG_GERAN_ERROR_0("GSN ""XID encode. Invalid XID T2 block length");

          break;

        case T2_ALG_RFC2507:

          if( xid_src->type_field.xid_t2.hdr.p )
          {
            gpackb(xid_src->type_field.xid_t2.alg.xid_rfc2507.pcomp1, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, pcomp1));
            gpackb(xid_src->type_field.xid_t2.alg.xid_rfc2507.pcomp2, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, pcomp2));
            gpackb(xid_src->type_field.xid_t2.alg.xid_rfc2507.pcomp3, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, pcomp3));
            gpackb(xid_src->type_field.xid_t2.alg.xid_rfc2507.pcomp4, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, pcomp4));
            gpackb(xid_src->type_field.xid_t2.alg.xid_rfc2507.pcomp5, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, pcomp5));
            gpackb(xid_src->type_field.xid_t2.alg.xid_rfc2507.pcomp6, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, pcomp6));
          }

          /* Check if parameter field shall be included by checking the
          ** length of compression parametes in respect to P bit. See TS04.65
          ** table 5
          */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 3)) ||
              ( xid_src->type_field.xid_t2.hdr.p == 0 &&
              ( xid_src->type_field.xid_t2.hdr.length >= 2)) )
          {
            gpackw(xid_src->type_field.xid_t2.alg.xid_rfc2507.nsapis, xid_dst, &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, nsapis));
          }

          /* Pack F_MAX_PERIOD */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 5)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 4)) )
          {
            gpackw((xid_src->type_field.xid_t2.alg.xid_rfc2507.max_per),
                    xid_dst,
                    &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, max_per));
          }

          /* Pack F_MAX_TIME */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 7)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 5)) )
          {
            gpackb((uint8)(xid_src->type_field.xid_t2.alg.xid_rfc2507.max_time),
                    xid_dst,
                    &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, max_time));
          }


          /* Pack F_MAX_HEADER */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 8)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 6)) )
          {
            gpackb((uint8)(xid_src->type_field.xid_t2.alg.xid_rfc2507.max_hdr),
                    xid_dst,
                    &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, max_hdr));
          }


          /* Pack TCP_SPACE */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 9)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 7)) )
          {
            gpackb((uint8)(xid_src->type_field.xid_t2.alg.xid_rfc2507.tcp_sp),
                    xid_dst,
                    &msg_pos,
                    GSIZE(gsn_xid_pcomp_2507_t, tcp_sp));
          }


          /* Pack TCP_SPACE */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 10)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 9)) )
          {
            gpackw((xid_src->type_field.xid_t2.alg.xid_rfc2507.ntcp_sp),
                     xid_dst,
                     &msg_pos,
                     GSIZE(gsn_xid_pcomp_2507_t, ntcp_sp));
          }

          if((msg_pos / XID_BYTE_SIZE ) >
             (RFC2507_LEN_P1_MAX + XID_FIELD_HDR_P1_LEN + XID_TYPE_HDR_LEN))
          MSG_GERAN_ERROR_0("GSN ""XID encode. Invalid XID T2 block length");

          break;

        case T2_ALG_RFC3095:

          if( xid_src->type_field.xid_t2.hdr.p )
          {
            gpackb(xid_src->type_field.xid_t2.alg.xid_rfc3095.pcomp1,
                   xid_dst,
                   &msg_pos,
                   GSIZE(gsn_xid_pcomp_3095_t, pcomp1));
            gpackb(xid_src->type_field.xid_t2.alg.xid_rfc3095.pcomp2,
                   xid_dst,
                   &msg_pos,
                   GSIZE(gsn_xid_pcomp_3095_t, pcomp2));
          }

          /* Check if parameter field shall be included by checking the
          ** length of compression parametes in respect to P bit. See TS04.65
          ** table 5
          */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 1)) ||
              ( xid_src->type_field.xid_t2.hdr.p == 0 &&
              ( xid_src->type_field.xid_t2.hdr.length >= 2)) )
          {
            gpackw(xid_src->type_field.xid_t2.alg.xid_rfc3095.nsapis,
                   xid_dst,
                   &msg_pos,
                   GSIZE(gsn_xid_pcomp_3095_t, nsapis));
          }

          /* Pack MAX_CID */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 3)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 4)) )
          {
            gpackw((xid_src->type_field.xid_t2.alg.xid_rfc3095.max_cid),
                    xid_dst,
                    &msg_pos,
                    GSIZE(gsn_xid_pcomp_3095_t, max_cid));
          }

          /* Pack MAX_HEADER */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 5)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 6)) )
          {
            gpackw((xid_src->type_field.xid_t2.alg.xid_rfc3095.max_hdr),
                    xid_dst,
                    &msg_pos,
                    GSIZE(gsn_xid_pcomp_3095_t, max_hdr));
          }


          /* Pack PROFILE1 */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 7)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 8)) )
          {
            gpackw((xid_src->type_field.xid_t2.alg.xid_rfc3095.profile1),
                    xid_dst,
                    &msg_pos,
                    GSIZE(gsn_xid_pcomp_3095_t, profile1));
          }


          /* Pack PROFILE2 */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 9)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 10)) )
          {
            gpackw((xid_src->type_field.xid_t2.alg.xid_rfc3095.profile2),
                    xid_dst,
                    &msg_pos,
                    GSIZE(gsn_xid_pcomp_3095_t, profile2));
          }


          /* Pack PROFILE3 */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 11)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 12)) )
          {
            gpackw((xid_src->type_field.xid_t2.alg.xid_rfc3095.profile3),
                     xid_dst,
                     &msg_pos,
                     GSIZE(gsn_xid_pcomp_3095_t, profile3));
          }


          /* Pack PROFILE4 */
          if( ( xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length > 13)) ||
              (!xid_src->type_field.xid_t2.hdr.p  &&
              ( xid_src->type_field.xid_t2.hdr.length >= 14)) )
          {
            gpackw((xid_src->type_field.xid_t2.alg.xid_rfc3095.profile4),
                     xid_dst,
                     &msg_pos,
                     GSIZE(gsn_xid_pcomp_3095_t, profile4));
          }

          if((msg_pos / XID_BYTE_SIZE ) >
             (RFC3095_LEN_P1_MAX + XID_FIELD_HDR_P1_LEN + XID_TYPE_HDR_LEN))
          MSG_GERAN_ERROR_0("GSN ""XID encode. Invalid XID T2 block length");


          break;

        default:

          MSG_GERAN_ERROR_0("GSN ""XID encode. Invalid XID T2 algorithm type ");
          break;
        }

        break;

      default:
        MSG_GERAN_ERROR_0("GSN ""XID encode. Invalid XID type");
        break;
    }

    if( ( msg_pos / XID_BYTE_SIZE ) > XID_BLOCK_LEN_MAX )
    {
      MSG_GERAN_ERROR_0("GSN ""XID encode. Invalid XID block length");
      return (0);
    }
    else
      return( (uint8) ( msg_pos / XID_BYTE_SIZE ) );
  }

}

/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_REQ_XID()
===
===  DESCRIPTION
===
===   Builds XID block to be send to SGSN SNDCP to initiate XID negotiation.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===    Pointer to the XID block
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
dsm_item_type* gsn_encode_req_xid( gas_id_t gas_id, const gsn_nsapi_e_t *nsapi_ptr )
{

  dsm_item_type  *xid_req_ptr     = NULL; /* xid block to be send to SGSN SNDCP */
  dsm_item_type  *pxid_req_ptr    = NULL; /* xid parameters related to PCI compression */
  dsm_item_type  *dxid_req_ptr    = NULL; /* xid parameters related to Data compression */
  dsm_item_type  *ver_xid_req_ptr = NULL; /* xid parameters related to version number */

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )
  {
    uint8          t0_xid_par[XID_T0_LEN_MAX];

    if( ( nsapi_ptr->pci_comp == TRUE) || ( nsapi_ptr->data_comp == TRUE) )
    {
      /* Encode T0 xid parameter - SNDCP version number.
      ** This parameter will not be send to the peer SNDCP
      ** if no T1 or T2 xid parameters need to be proposed to the peer.
      ** It will only be part of an xid block if the block contains
      ** a PCI (T2) or/and a data (T1) compression entity.
      */
      t0_xid_par[0] = XID_TYPE_0; /* Type 0 XID parameter */
      t0_xid_par[1] = 0x01;       /* Length of Version Number */
      t0_xid_par[2] = nsapi_ptr->sn_xid_req.ver_num;  /* SNDCP Version Number */

      /* Allocate a dsm item to carry the T0 xid parameter */
      ver_xid_req_ptr = gsn_pdu_construct( t0_xid_par, XID_T0_LEN_MAX );
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP

  /* Check if PCI compression has been requested on this NSAPI.*/
  if( nsapi_ptr->pci_comp == TRUE )
  {
    gsn_xid_block_t pxid_src;
    uint8           t2_xid_ex[XID_T2_LEN_MAX];
    uint8           t2_len          = 0;
    uint8           ent_num         = 0;
    t2_alg_q_t      *t2_q_item      = NULL;
    pcomp_e_t       *pcomp_e_ptr    = NULL;
    uint8           pcomp_val[PCOMP_VAL_MAX];

    memset(pcomp_val, 0, PCOMP_VAL_MAX);

    /* Check if NSAPI already uses a PCI comp entity */
    if( (nsapi_ptr->pcomp_e_num[T2_ALG_RFC1144] != NO_COMP_ENT ) ||
        (nsapi_ptr->pcomp_e_num[T2_ALG_RFC2507] != NO_COMP_ENT ) ||
        (nsapi_ptr->pcomp_e_num[T2_ALG_RFC3095] != NO_COMP_ENT ))
    {
      MSG_GERAN_LOW_1( "GSN NS %d: No need for PCI comp XID req",
        GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ));

      pxid_req_ptr = NULL;
    }
    else
    {
      /* TS 04.65 par 6.10 suggests that one or several NSAPIs may use
      ** the same PCI compression entity. However, for RFC 1144 and
      ** RFC 2507, the performance of the algorithm is based on the
      ** commonality of the TCP/UDP/IP header fields among subsequent packets
      ** of the same TCP/UDP connection. If different NSAPIs, i.e. different
      ** TCP/UDP connection, use the same compression entity, the performance
      ** of the compressor could be uncertain. So, in this implementation, the MS
      ** SNDCP will always request a new PCI compression entity for each NSAPI.
      ** If the PCOMP values for the desired PCI algorithm have been already assigned
      ** on the SAPI( some comp entity on the SAPI uses the same algorithm),
      ** then there is no need to re-negotiate the PCOMP values.
      */

      /* Select a new compression entity to propose to the peer
      */
      ent_num = gsn_next_unassigned_pcomp_e( nsapi_ptr->sapi_ptr );

      if( ent_num == NO_COMP_ENT )
      {
        MSG_GERAN_HIGH_0("GSN ""No PCI comp entities left");
        pxid_req_ptr = NULL;
      }
      else
      {
        /* Before the new compression entity is created,
        ** make sure that there are enough PCOMP values available
        ** to be assigned to the algorithm to be used.
        */
        if( nsapi_ptr->sn_xid_req.t2_alg == T2_ALG_RFC1144 )
        {
          /* Check if the PCOMP values of the algorithm are
          ** already assigned on the SAPI.
          */
          if( q_cnt(&nsapi_ptr->sapi_ptr->t2_alg_q[T2_ALG_RFC1144]) )
          {
            t2_q_item = q_check(&nsapi_ptr->sapi_ptr->t2_alg_q[T2_ALG_RFC1144]);
            if( t2_q_item )
            {
              pcomp_val[PCOMP1_VAL] = t2_q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp1;
              pcomp_val[PCOMP2_VAL] = t2_q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp2;
            }
            else
            {
              MSG_GERAN_HIGH_0("GSN ""Invalid PCI comp entity in T2 queue");
              ent_num = NO_COMP_ENT; /* Do not create enity */
            }
          }
          else
          {
            if( gsn_create_pcomp_val
                  (pcomp_val, T2_ALG_RFC1144, nsapi_ptr->sapi_ptr) != RFC1144_PCOMP_MAX )
              ent_num = NO_COMP_ENT; /* Do not create enity */
          }
        }
        else if( nsapi_ptr->sn_xid_req.t2_alg == T2_ALG_RFC2507 )
        {
          /* Check if the PCOMP values for the algorithm are
          ** already assigned on the SAPI.
          */
          if( q_cnt(&nsapi_ptr->sapi_ptr->t2_alg_q[T2_ALG_RFC2507]) )
          {
            t2_q_item = q_check(&nsapi_ptr->sapi_ptr->t2_alg_q[T2_ALG_RFC2507]);
            if( t2_q_item )
            {
              pcomp_val[PCOMP1_VAL] = t2_q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp1;
              pcomp_val[PCOMP2_VAL] = t2_q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp2;
              pcomp_val[PCOMP3_VAL] = t2_q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp3;
              pcomp_val[PCOMP4_VAL] = t2_q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp4;
              pcomp_val[PCOMP5_VAL] = t2_q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp5;
            }
            else
            {
              MSG_GERAN_HIGH_0("GSN ""Invalid PCI comp entity in T2 queue");
              ent_num = NO_COMP_ENT; /* Do not create entity */
            }
          }
          else
          {
            if( gsn_create_pcomp_val
                  (pcomp_val, T2_ALG_RFC2507, nsapi_ptr->sapi_ptr) != RFC2507_PCOMP_MAX )
              ent_num = NO_COMP_ENT; /* Do not create enity */
          }
        }
        else if( nsapi_ptr->sn_xid_req.t2_alg == T2_ALG_RFC3095 )
        {
          /* Check if the PCOMP values for the algorithm are
          ** already assigned on the SAPI.
          */
          if( q_cnt(&nsapi_ptr->sapi_ptr->t2_alg_q[T2_ALG_RFC3095]) )
          {
            t2_q_item = q_check(&nsapi_ptr->sapi_ptr->t2_alg_q[T2_ALG_RFC3095]);
            if( t2_q_item )
            {
              pcomp_val[PCOMP1_VAL] = t2_q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp1;
              pcomp_val[PCOMP2_VAL] = t2_q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp2;
            }
            else
            {
              MSG_GERAN_HIGH_0("GSN ""Invalid PCI comp entity in T2 queue");
              ent_num = NO_COMP_ENT; /* Do not create entity */
            }
          }
          else
          {
            if( gsn_create_pcomp_val(pcomp_val,
                                     T2_ALG_RFC3095,
                                     nsapi_ptr->sapi_ptr) != RFC3095_PCOMP_MAX)
              ent_num = NO_COMP_ENT; /* Do not create entity */
          }
        }
        else
        {
          MSG_GERAN_HIGH_1("GSN ""LL %d: Inval t2 algorithm",
                    gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);

          ent_num = NO_COMP_ENT; /* Do not create entity */
        }

        /* Create compression entity after PCOMP values have been assigned */
        if( ent_num != NO_COMP_ENT )
        {
          gsn_xid_t2_t xid_t2;

          xid_t2.hdr.ent_num  = ent_num;

          /* Get T2 algorithm specified by PDP ( or preferred T2
          ** algorithm in case of SNDCP initiated XID negotiation).
          */
          xid_t2.hdr.alg_type = nsapi_ptr->sn_xid_req.t2_alg;

          if( xid_t2.hdr.alg_type == T2_ALG_RFC1144 )
          {
            xid_t2.alg.xid_rfc1144.nsapis = APP_NSAPIS_MASK &
                                (0x01 << GSN_EXTERN_NSAPI(nsapi_ptr->nsapi));

            if( nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP1_VAL] ] )
            {
              xid_t2.alg.xid_rfc1144.pcomp1 = pcomp_val[PCOMP1_VAL];
              nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP1_VAL] ]->state = SELECTED;
            }

            if( nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP2_VAL] ] )
            {
              xid_t2.alg.xid_rfc1144.pcomp2 = pcomp_val[PCOMP2_VAL];
              nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP2_VAL] ]->state = SELECTED;
            }

            xid_t2.alg.xid_rfc1144.s0_1 = nsapi_ptr->sn_xid_req.s0_1;
          }
          if( xid_t2.hdr.alg_type == T2_ALG_RFC2507 )
          {
            xid_t2.alg.xid_rfc2507.nsapis = APP_NSAPIS_MASK &
                                (0x01 << GSN_EXTERN_NSAPI(nsapi_ptr->nsapi));

            if( nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP1_VAL] ] )
            {
              xid_t2.alg.xid_rfc2507.pcomp1   = pcomp_val[PCOMP1_VAL];
              nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP1_VAL] ]->state = SELECTED;
            }

            if( nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP2_VAL] ] )
            {
              xid_t2.alg.xid_rfc2507.pcomp2   = pcomp_val[PCOMP2_VAL];
              nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP2_VAL] ]->state = SELECTED;
            }

            if( nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP3_VAL] ] )
            {
              xid_t2.alg.xid_rfc2507.pcomp3   = pcomp_val[PCOMP3_VAL];
              nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP3_VAL] ]->state = SELECTED;
            }

            if( nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP4_VAL] ] )
            {
              xid_t2.alg.xid_rfc2507.pcomp4   = pcomp_val[PCOMP4_VAL];
              nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP4_VAL] ]->state = SELECTED;
            }

            if( nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP5_VAL] ] )
            {
              xid_t2.alg.xid_rfc2507.pcomp5   = pcomp_val[PCOMP5_VAL];
              nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP5_VAL] ]->state = SELECTED;
            }

            xid_t2.alg.xid_rfc2507.pcomp6   = 0;
            xid_t2.alg.xid_rfc2507.max_hdr  = nsapi_ptr->sn_xid_req.max_hdr;
            xid_t2.alg.xid_rfc2507.max_per  = nsapi_ptr->sn_xid_req.max_per;
            xid_t2.alg.xid_rfc2507.max_time = nsapi_ptr->sn_xid_req.max_time;
            xid_t2.alg.xid_rfc2507.ntcp_sp  = nsapi_ptr->sn_xid_req.ntcp_sp;
            xid_t2.alg.xid_rfc2507.tcp_sp   = nsapi_ptr->sn_xid_req.tcp_sp;
          }

          if( xid_t2.hdr.alg_type == T2_ALG_RFC3095 )
          {
            xid_t2.alg.xid_rfc3095.nsapis = APP_NSAPIS_MASK &
                                (0x01 << GSN_EXTERN_NSAPI(nsapi_ptr->nsapi));

            if( nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP1_VAL] ] )
            {
              xid_t2.alg.xid_rfc3095.pcomp1   = pcomp_val[PCOMP1_VAL];
              nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP1_VAL] ]->state = SELECTED;
            }

            if( nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP2_VAL] ] )
            {
              xid_t2.alg.xid_rfc3095.pcomp2   = pcomp_val[PCOMP2_VAL];
              nsapi_ptr->sapi_ptr->pcomp_pool[ pcomp_val[PCOMP2_VAL] ]->state = SELECTED;
            }

            xid_t2.alg.xid_rfc3095.max_cid  = nsapi_ptr->sn_xid_req.max_cid;
            xid_t2.alg.xid_rfc3095.max_hdr  = nsapi_ptr->sn_xid_req.max_header;
            xid_t2.alg.xid_rfc3095.profile1 = nsapi_ptr->sn_xid_req.profile1;
            xid_t2.alg.xid_rfc3095.profile2 = nsapi_ptr->sn_xid_req.profile2;
            xid_t2.alg.xid_rfc3095.profile3 = nsapi_ptr->sn_xid_req.profile3;
            xid_t2.alg.xid_rfc3095.profile4 = nsapi_ptr->sn_xid_req.profile4;
          }



          /* Create entity */
          pcomp_e_ptr = gsn_create_pcomp_e( &xid_t2, nsapi_ptr->sapi_ptr );
          if( pcomp_e_ptr != NULL )
          {
            /* Set entity's state and operation mode */
            pcomp_e_ptr->state = SELECTED;
            pcomp_e_ptr->op_mode = nsapi_ptr->op_mode;

            /* Build XID T2 block */
            pxid_src.type   = XID_TYPE_2;
            pxid_src.length = 0;

            if( xid_t2.hdr.alg_type == T2_ALG_RFC1144 )
            {
              pxid_src.length = XID_FIELD_HDR_P1_LEN + RFC1144_LEN_P1_MAX;
              pxid_src.type_field.xid_t2.hdr.length = RFC1144_LEN_P1_MAX;
            }
            else if( xid_t2.hdr.alg_type == T2_ALG_RFC2507 )
            {
              pxid_src.length = XID_FIELD_HDR_P1_LEN + RFC2507_LEN_P1_MAX;
              pxid_src.type_field.xid_t2.hdr.length = RFC2507_LEN_P1_MAX;
            }
            else if( xid_t2.hdr.alg_type == T2_ALG_RFC3095 )
            {
              pxid_src.length = XID_FIELD_HDR_P1_LEN + RFC3095_LEN_P1_MAX;
              pxid_src.type_field.xid_t2.hdr.length = RFC3095_LEN_P1_MAX;
            }
            else
            {
              MSG_GERAN_HIGH_0("Invalid pcomp alg");
            }

            pxid_src.type_field.xid_t2.hdr.p       = 1;
            pxid_src.type_field.xid_t2.hdr.x1      = 0;
            pxid_src.type_field.xid_t2.hdr.ent_num = pcomp_e_ptr->ent_num;
            pxid_src.type_field.xid_t2.hdr.x2      = 0;
            pxid_src.type_field.xid_t2.hdr.alg_type  = pcomp_e_ptr->t2_alg;
            pxid_src.type_field.xid_t2.alg = xid_t2.alg;

            /* Encode XID request */
            t2_len = gsn_encode_xid( &pxid_src, t2_xid_ex );

            /* check length of T2 XID */
            if( t2_len != (pxid_src.length + XID_TYPE_HDR_LEN) )
              MSG_GERAN_HIGH_0("GSN ""XID Error encoding T2 XID params");

            /* Allocate dsm item to carry the XID block */
            pxid_req_ptr = gsn_pdu_construct( t2_xid_ex, (uint16)t2_len );
          }
          else
          {
            /* If any PCOMP values were selected for the compression entity,
            ** delete PCOMP values and free alocated memory if not used by
            ** another entity.
            */
            if( !q_cnt(&nsapi_ptr->sapi_ptr->t2_alg_q[xid_t2.hdr.alg_type]) )
            {
              if( xid_t2.hdr.alg_type == T2_ALG_RFC2507 )
              {
                GSN_MEM_FREE( nsapi_ptr->sapi_ptr->
                      pcomp_pool[ pcomp_val[PCOMP1_VAL] ] );

                GSN_MEM_FREE( nsapi_ptr->sapi_ptr->
                      pcomp_pool[ pcomp_val[PCOMP2_VAL] ] );

                GSN_MEM_FREE( nsapi_ptr->sapi_ptr->
                      pcomp_pool[ pcomp_val[PCOMP3_VAL] ] );

                GSN_MEM_FREE( nsapi_ptr->sapi_ptr->
                      pcomp_pool[ pcomp_val[PCOMP4_VAL] ] );

                GSN_MEM_FREE( nsapi_ptr->sapi_ptr->
                      pcomp_pool[ pcomp_val[PCOMP5_VAL] ] );
              }
              else if( xid_t2.hdr.alg_type == T2_ALG_RFC1144 )
              {
                GSN_MEM_FREE( nsapi_ptr->sapi_ptr->
                      pcomp_pool[ pcomp_val[PCOMP1_VAL] ] );

                GSN_MEM_FREE( nsapi_ptr->sapi_ptr->
                      pcomp_pool[ pcomp_val[PCOMP2_VAL] ] );
              }
              else if( xid_t2.hdr.alg_type == T2_ALG_RFC3095)
              {
                GSN_MEM_FREE( nsapi_ptr->sapi_ptr->
                      pcomp_pool[ pcomp_val[PCOMP1_VAL] ] );

                GSN_MEM_FREE( nsapi_ptr->sapi_ptr->
                      pcomp_pool[ pcomp_val[PCOMP2_VAL] ] );
              }
              else
              {
                MSG_GERAN_HIGH_0("Invalid pcomp alg");
              }
            }
          }
        }
        else
        {
          MSG_GERAN_HIGH_0("GSN ""PCI comp entity has not been created");
          pxid_req_ptr = NULL;
        }
      }
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP

  /* Check if Data compression has been requested on this NSAPI.*/
  if( nsapi_ptr->data_comp == TRUE )
  {
    gsn_xid_block_t dxid_src;
    uint8           t1_xid_ex[XID_T1_LEN_MAX + XID_TYPE_HDR_LEN];
    uint8           t1_len          = 0;
    uint8           ent_num         = 0;
    t1_alg_q_t      *t1_q_item      = NULL;
    boolean         op_mode_match   = FALSE;
    dcomp_e_t       *dcomp_e_ptr    = NULL;
    uint8           dcomp_val[DCOMP_VAL_MAX];
    /* Change to uint8 when more data compression algorithms added and
    ** T1_ALG_MAX != 0. int8 is used to avoid unassigned comparison with 0 warning.
    */
    int8            alg;

    memset(dcomp_val, 0, DCOMP_VAL_MAX);

    /* Check if NSAPI already uses a data comp entity */
    if( (nsapi_ptr->dcomp_e_num[T1_ALG_V42BIS] <= ENT_NUM_MAX ) )
    {
      MSG_GERAN_LOW_1( "GSN NS %d: No need for Data comp XID req",
        GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ));
    }
    else
    {
      /* Protocol compression is enabled. If V.42 bis
      ** has been negotiated successfully on the SAPI and used by an NSAPI then
      ** the same compression entity can be used by this NSAPI (if NSAPIs operation
      ** modes are the same). if no data comp algorithm has been negotiated, send XID
      ** to request a new compression entity that uses the V.42 bis algorithm.
      */
      for( alg = 0; alg <= T1_ALG_MAX; alg++ )
      {
        if( q_cnt(&nsapi_ptr->sapi_ptr->t1_alg_q[alg]) )
        {
          /* There  are assigned or selected data comp entities on the SAPI.
          ** Check if the NSAPI can share any of these entities.
          */
          t1_q_item = q_check( &nsapi_ptr->sapi_ptr->t1_alg_q[alg] );

          while( t1_q_item != NULL )
          {
            if( nsapi_ptr->op_mode == t1_q_item->dcomp_e_ptr->op_mode )
            {
              /* The entity is used by NSAPIs of the same op mode as the current NSAPI */
              op_mode_match = TRUE;
              break;
            }

            /* Get next queue item */
            t1_q_item = q_next( &nsapi_ptr->sapi_ptr->t1_alg_q[alg], &t1_q_item->link );
          }
        }
      }
      if( op_mode_match && t1_q_item )
      {
        if( t1_q_item->dcomp_e_ptr->state != UNASSIGNED )
        {
          /* The comp entity is used by NSAPIs of the same op mode as the current
          ** NSAPI. Add NSAPI to the Applicable NSAPIs field and send XID request
          ** to inform the peer that a new NSAPI will use the same data comp entity.
          */
          dxid_src.type   = XID_TYPE_1;
          dxid_src.length = XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;
          dxid_src.type_field.xid_t1.hdr.p        = 0;
          dxid_src.type_field.xid_t1.hdr.x1       = 0;
          dxid_src.type_field.xid_t1.hdr.ent_num  =
                      t1_q_item->dcomp_e_ptr->ent_num;

          /* Do not include parameters that their value has not been changed.
          ** Only update Applicable NSAPIs field.
          */
          dxid_src.type_field.xid_t1.hdr.length = APP_NSAPIS_LEN;

          dxid_src.type_field.xid_t1.alg.xid_v42bis.nsapis =
               ( t1_q_item->dcomp_e_ptr->alg.xid_v42bis.nsapis ||
                  ( 0x01 << GSN_EXTERN_NSAPI( nsapi_ptr->nsapi ) ) );

          /* Encode XID request */
          t1_len = gsn_encode_xid( &dxid_src, t1_xid_ex );

          /* check length of T1 XID */
          if( t1_len != (dxid_src.length + XID_TYPE_HDR_LEN) )
          {
            MSG_GERAN_LOW_0("GSN ""XID encode. Error encoding T1 XID params");
          }

          /* Allocate dsm item to carry the XID block */
          dxid_req_ptr = gsn_pdu_construct( t1_xid_ex, (uint16)t1_len );

        }
        else
        {
          MSG_GERAN_ERROR_1("GSN ""LL %d: Inval V42 bis state",
                      gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);
        }
      }
      else
      {
        /* Select a new comp entity to propose to the peer SNDCP.
        */
        ent_num = gsn_next_unassigned_dcomp_e( nsapi_ptr->sapi_ptr );

        if( ent_num == NO_COMP_ENT )
        {
          MSG_GERAN_HIGH_0("GSN ""No Data comp entities left");
          dxid_req_ptr = NULL;
        }
        else
        {
          /* Before the new compression entity is created,
          ** make sure that there are enough DCOMP values available
          ** to be assigned to the algorithm to be used.
          */
          if( nsapi_ptr->sn_xid_req.t1_alg == T1_ALG_V42BIS )
          {
            /* Check if the DCOMP values of the algorithm are
            ** already assigned on the SAPI.
            */
            if( q_cnt(&nsapi_ptr->sapi_ptr->t1_alg_q[T1_ALG_V42BIS]) )
            {
              t1_q_item = q_check(&nsapi_ptr->sapi_ptr->t1_alg_q[T1_ALG_V42BIS]);
              if( t1_q_item )
              {
                dcomp_val[DCOMP1_VAL] = t1_q_item->dcomp_e_ptr->alg.xid_v42bis.dcomp1;
              }
              else
              {
                MSG_GERAN_HIGH_0("GSN ""Invalid Data comp entity in T1 queue");
                ent_num = NO_COMP_ENT; /* Do not create the enity */
              }
            }
            else
            {
              if( gsn_create_dcomp_val
                    (dcomp_val, T1_ALG_V42BIS, nsapi_ptr->sapi_ptr) != V42BIS_DCOMP_MAX )
                ent_num = NO_COMP_ENT; /* Do not create the enity */
            }
          }
          else
          {
            MSG_GERAN_HIGH_1("GSN ""LL %d: Inval T1 algorithm",
                      gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);

            ent_num = NO_COMP_ENT; /* Do not create the enity */
          }

          /* Create compression entity after DCOMP values have been assigned */
          if( ent_num != NO_COMP_ENT )
          {
            gsn_xid_t1_t xid_t1;

            xid_t1.hdr.ent_num  = ent_num;

            /* Get T1 algorithm specified by PDP ( or preferred T1
            ** algorithm in case of SNDCP initiated XID negotiation).
            */
            xid_t1.hdr.alg_type = nsapi_ptr->sn_xid_req.t1_alg;

            if( xid_t1.hdr.alg_type == T1_ALG_V42BIS )
            {
              xid_t1.alg.xid_v42bis.nsapis = APP_NSAPIS_MASK &
                                  (0x01 << GSN_EXTERN_NSAPI(nsapi_ptr->nsapi));

              if( nsapi_ptr->sapi_ptr->dcomp_pool[ dcomp_val[DCOMP1_VAL] ] )
              {
                xid_t1.alg.xid_v42bis.dcomp1 = dcomp_val[DCOMP1_VAL];
                xid_t1.alg.xid_v42bis.dcomp2 = 0;
                nsapi_ptr->sapi_ptr->dcomp_pool[ dcomp_val[DCOMP1_VAL] ]->state = SELECTED;
              }

              xid_t1.alg.xid_v42bis.p0 = nsapi_ptr->sn_xid_req.p0;
              xid_t1.alg.xid_v42bis.p1 = nsapi_ptr->sn_xid_req.p1;
              xid_t1.alg.xid_v42bis.p2 = nsapi_ptr->sn_xid_req.p2;

            }

            /* Create entity */
            dcomp_e_ptr = gsn_create_dcomp_e( &xid_t1, nsapi_ptr->sapi_ptr );
            if( dcomp_e_ptr != NULL )
            {
              /* Set entity's state and operation mode */
              dcomp_e_ptr->state = SELECTED;
              dcomp_e_ptr->op_mode = nsapi_ptr->op_mode;

              /* Build XID T2 block */
              dxid_src.type   = XID_TYPE_1;
              if( xid_t1.hdr.alg_type == T1_ALG_V42BIS )
              {
                dxid_src.length = XID_FIELD_HDR_P1_LEN + V42BIS_LEN_P1_MAX;
                dxid_src.type_field.xid_t1.hdr.length = V42BIS_LEN_P1_MAX;
              }

              dxid_src.type_field.xid_t1.hdr.p       = 1;
              dxid_src.type_field.xid_t1.hdr.x1      = 0;
              dxid_src.type_field.xid_t1.hdr.ent_num = dcomp_e_ptr->ent_num;
              dxid_src.type_field.xid_t1.hdr.x2      = 0;
              dxid_src.type_field.xid_t1.hdr.alg_type  = dcomp_e_ptr->t1_alg;
              dxid_src.type_field.xid_t1.alg = xid_t1.alg;

              /* Encode XID request */
              t1_len = gsn_encode_xid( &dxid_src, t1_xid_ex );

              /* check length of T2 XID */
              if( t1_len != (dxid_src.length + XID_TYPE_HDR_LEN) )
                MSG_GERAN_HIGH_0("GSN ""XID Error encoding T1 XID params");

              /* Allocate dsm item to carry the XID block */
              dxid_req_ptr = gsn_pdu_construct( t1_xid_ex, (uint16)t1_len );
            }
            else
            {
              /* If any DCOMP values were selected for the compression entity,
              ** delete DCOMP values and free alocated memory if not used by
              ** another entity
              */
              if( !q_cnt(&nsapi_ptr->sapi_ptr->t1_alg_q[xid_t1.hdr.alg_type]) )
              {
                if( xid_t1.hdr.alg_type == T1_ALG_V42BIS )
                {
                  GSN_MEM_FREE( nsapi_ptr->sapi_ptr->
                          dcomp_pool[ dcomp_val[DCOMP1_VAL] ] );
                }
                else
                  MSG_GERAN_HIGH_0("GSN ""XID Invalid T1 algorithm");
              }
            }
          }
          else
          {
            MSG_GERAN_HIGH_0("GSN ""Data comp entity has not been created");
            dxid_req_ptr = NULL;
          }
        }
      }
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

  /* Form the final XID block
  */

  /* Boolean within 'if' always evaluates to True if compression is disabled*/
  /*lint -save -e774*/
  if( !pxid_req_ptr && !dxid_req_ptr )
  {
    /* Boolean within 'if' always evaluates to FALSE if compression is disabled*/
    xid_req_ptr = NULL;
    if( ver_xid_req_ptr )
      GPRS_PDU_FREE( &ver_xid_req_ptr );

    MSG_GERAN_MED_1("GSN ""LL %d: No PCOMP or DCOMP xid req",
                      gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);
  } /*lint -restore*/
  else
  {
    /* Concatenate the XID parameters and form the XID
    ** block to be send to the peer.
    */
    if( pxid_req_ptr && dxid_req_ptr )
    {
      xid_req_ptr = ver_xid_req_ptr;
      gprs_pdu_augment_tail( &xid_req_ptr, &dxid_req_ptr );
      gprs_pdu_augment_tail( &xid_req_ptr, &pxid_req_ptr );

      MSG_GERAN_MED_1("GSN ""LL %d: PCOMP & DCOMP xid req",
                        gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);
    }
    else if( pxid_req_ptr )
    {
      xid_req_ptr = ver_xid_req_ptr;
      gprs_pdu_augment_tail( &xid_req_ptr, &pxid_req_ptr );

      MSG_GERAN_MED_1("GSN ""LL %d: Only PCOMP xid req",
                        gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);
    }
    else if( dxid_req_ptr )
    {
      xid_req_ptr = ver_xid_req_ptr;
      gprs_pdu_augment_tail( &xid_req_ptr, &dxid_req_ptr );

      MSG_GERAN_MED_1("GSN ""LL %d: Only DCOMP xid req",
                        gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);
    }
    else
      xid_req_ptr = NULL;

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

    /* Duplicate XID block, might need to be retransmitted */
    if( xid_req_ptr )
    {
      if( nsapi_ptr->sapi_ptr->xid_req_dup_ptr == NULL )
      {
        nsapi_ptr->sapi_ptr->xid_req_dup_ptr =
          gprs_pdu_duplicate( xid_req_ptr, 0, GPRS_PDU_LENGTH(xid_req_ptr) );

        if( nsapi_ptr->sapi_ptr->xid_req_dup_ptr == NULL )
        {
          MSG_GERAN_ERROR_0("GSN ""gprs_pdu_duplicate() failed. No dsm resources");
        }
      }
      else
        MSG_GERAN_HIGH_1("GSN ""LL %d: Inval xid_req_dup_ptr",
                        gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);
    }
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */
  }

  return( xid_req_ptr );
}

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )

/*===========================================================================
===
===  FUNCTION      GSN_DEL_ALL_SELECTED_COMP_E()
===
===  DESCRIPTION
===
===   Deletes all SELECTED compression entities on the SAPI.
===   This function is called after the XID negotiation has failed and
===   the SELECTED entities have not been ASSIGNED.
===
===  DEPENDENCIES
===
===   None
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
void gsn_del_all_selected_comp_e( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr )  /* SAPI in use */
{

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
  {

    t2_alg_q_t        *q_item;
    t2_alg_q_t        *q_next_item;
    uint8             alg;

    for( alg = 0; alg <= T2_ALG_MAX; alg++ )
    {
      if( q_cnt( &sapi_ptr->t2_alg_q[alg] ) )
      {
        /* One or more comp entities use the comp algorithm.
        ** Remove SELECTED entities, if any, from the list.
        */
        q_item = q_check( &sapi_ptr->t2_alg_q[alg] );
        while( q_item != NULL )
        {
          q_next_item = q_next( &sapi_ptr->t2_alg_q[alg], &q_item->link );

          if( q_item->pcomp_e_ptr->state == SELECTED )
          {
            gsn_del_pcomp_e(  gas_id, q_item->pcomp_e_ptr->ent_num, sapi_ptr );
          }

          /* Get next queue item */
          q_item = q_next_item;

        }
      }
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
  {

    t1_alg_q_t        *q_item;
    t1_alg_q_t        *q_next_item;
    /* Change to uint8 when more data compression algorithms added and
    ** T1_ALG_MAX != 0. int8 is used to avoid unassigned comparison with 0 warning.
    */
    int8              alg;

    for( alg = 0; alg <= T1_ALG_MAX; alg++ )
    {
      if( q_cnt( &sapi_ptr->t1_alg_q[alg] ) )
      {
        /* One or more comp entities use the comp algorithm.
        ** Remove SELECTED entities, if any, from the list.
        */
        q_item = q_check( &sapi_ptr->t1_alg_q[alg] );
        while( q_item != NULL )
        {
          q_next_item = q_next( &sapi_ptr->t1_alg_q[alg], &q_item->link );

          if( q_item->dcomp_e_ptr->state == SELECTED )
          {
            gsn_del_dcomp_e( gas_id, q_item->dcomp_e_ptr->ent_num , sapi_ptr );
          }

          /* Get next queue item */
          q_item = q_next_item;

        }
      }
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

}

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */


#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
/*===========================================================================
===
===  FUNCTION      GSN_DEL_DCOMP_E()
===
===  DESCRIPTION
===
===   Deletes the Data compession entity
===
===  DEPENDENCIES
===
===   None
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
void gsn_del_dcomp_e( gas_id_t gas_id, uint8 ent_num, gsn_sapi_e_t *sapi_ptr )
{
  t1_alg_q_t *q_item;
  t1_alg_q_t *q_next_item;
  uint8      nsapi;
  uint8      nsapis_on_sapi[GSN_NSAPI_MAX], i;
  /* Change to uint8 when more data compression algorithms added and
  ** T1_ALG_MAX != 0. int8 is used to avoid unassigned comparison with 0 warning.
  */
  int8       alg;

  if( sapi_ptr->dcomp_e_pool[ent_num] == NULL )
  {
    MSG_GERAN_ERROR_1("GSN ""Invalid Data compression entity %d", ent_num);
    return;
  }

  /* Get algorithm used by compression entity */
  /*lint -e713 Loss of precision (unsigned char to signed char). See comment above */
  alg = sapi_ptr->dcomp_e_pool[ent_num]->t1_alg;


  if( alg <= T1_ALG_MAX )
  {
    if( q_cnt( &sapi_ptr->t1_alg_q[alg] ) )
    {
      /* One or more comp entities use the comp algorithm.
      */
      q_item = q_check( &sapi_ptr->t1_alg_q[alg] );
      while( q_item != NULL )
      {
        q_next_item = q_next( &sapi_ptr->t1_alg_q[alg], &q_item->link );

        if( q_item->dcomp_e_ptr->ent_num == ent_num )
        {
          /* Remove entity from the list */
          q_delete
            (
                &sapi_ptr->t1_alg_q[alg],
                &q_item->link
             );

          /* Get the NSAPIs mapped on the SAPI */
          gsn_get_nsapis_on_sapi( gas_id, sapi_ptr, nsapis_on_sapi );

          for( i = 0; i < (uint8)GSN_NSAPI_MAX; i++ )
          {
            /* Get the first NSAPI mapped on the SAPI */
            nsapi = nsapis_on_sapi[i];

            /* Exit the for loop if end of valid NSAPIs */
            if( nsapi == GSN_INVAL_NSAPI )
              break;

            if( alg == T1_ALG_V42BIS )
            {
              if( q_item->dcomp_e_ptr->alg.xid_v42bis.nsapis &
                  (0x01 << GSN_EXTERN_NSAPI(nsapi)) )
              {
                /* Delink NSAPI from the comp entity */
                actv_nsapis[ gas_id ][nsapi]->dcomp_e_num[T1_ALG_V42BIS] = NO_COMP_ENT;
              }
            }
          }

          if( !q_cnt( &sapi_ptr->t1_alg_q[alg] ) )
          {
            /* No comp entities use the algorithm. Unassign DCOMP values */
            if( alg == T1_ALG_V42BIS )
            {
              if ( q_item->dcomp_e_ptr->alg.xid_v42bis.dcomp1  &&
                   sapi_ptr->dcomp_pool[q_item->dcomp_e_ptr->alg.xid_v42bis.dcomp1] )
              {
                GSN_MEM_FREE( sapi_ptr->dcomp_pool
                 [q_item->dcomp_e_ptr->alg.xid_v42bis.dcomp1] );
              }
            }
          }

          if( alg == T1_ALG_V42BIS )
          {
            /* Free memmory allocated for V42bis tables */
            gsn_v42_dealloc_tables
              ( gas_id, &q_item->dcomp_e_ptr->comp_e, q_item->dcomp_e_ptr->alg.xid_v42bis.p0 );
          }

          MSG_GERAN_MED_2("GSN ""XID %d: Rmv DCOMP entity %d",
            gsn_extern_sapi[sapi_ptr->sapi],q_item->dcomp_e_ptr->ent_num);

          /* Put entity back to the pool of unassigned entities */
          GSN_MEM_FREE( sapi_ptr->dcomp_e_pool[q_item->dcomp_e_ptr->ent_num] );

          /* free memory of queue item */
          GSN_MEM_FREE( q_item );

          break;
        }

        /* Get next queue item */
        q_item = q_next_item;
      }
    }
    else
    {
      /* At least one item should be in the queue */
      MSG_GERAN_ERROR_1("GSN ""XID %d: Resp inval T1 alg queue",
                  gsn_extern_sapi[sapi_ptr->sapi]);
    }
  }
  else
    MSG_GERAN_ERROR_1("GSN ""XID %d: Inval T1 alg",
                  gsn_extern_sapi[sapi_ptr->sapi]);

}

#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
/*===========================================================================
===
===  FUNCTION      GSN_DEL_PCOMP_E()
===
===  DESCRIPTION
===
===   Deletes the Protocol Control Information (PCI) compression entity
===
===  DEPENDENCIES
===
===   None
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
void gsn_del_pcomp_e( gas_id_t gas_id, uint8 ent_num, gsn_sapi_e_t *sapi_ptr )
{

  t2_alg_q_t  *q_item = NULL;
  t2_alg_q_t  *q_next_item;
  uint8       nsapi, alg;
  uint8       nsapis_on_sapi[GSN_NSAPI_MAX], i;


  if( sapi_ptr->pcomp_e_pool[ ent_num ] == NULL )
  {
    MSG_GERAN_ERROR_1("GSN "" Invalid PCI compression entity %d", ent_num);
    return;
  }

  /* Get algorithm used by compression entity */
  alg = sapi_ptr->pcomp_e_pool[ent_num]->t2_alg;

  if( alg <= T2_ALG_MAX )
  {
    if( q_cnt( &sapi_ptr->t2_alg_q[alg] ) )
    {
      /* One or more comp entities use the comp algorithm.
      ** Remove entity from the list
      */
      q_item = q_check( &sapi_ptr->t2_alg_q[alg] );
      while( q_item != NULL )
      {
        q_next_item = q_next
          ( &sapi_ptr->t2_alg_q[alg], &q_item->link );

        if( q_item->pcomp_e_ptr->ent_num == ent_num )
        {
          /* Remove entity from the list */
          q_delete
            (
                &sapi_ptr->t2_alg_q[alg],
                &q_item->link
             );

          /* Get the NSAPIs mapped on the SAPI */
          gsn_get_nsapis_on_sapi( gas_id, sapi_ptr, nsapis_on_sapi );

          for( i = 0; i < (uint8) GSN_NSAPI_MAX; i++ )
          {
            /* Get the first NSAPI mapped on the SAPI */
            nsapi = nsapis_on_sapi[i];

            /* Exit the for loop if end of valid NSAPIs */
            if( nsapi == GSN_INVAL_NSAPI )
              break;

            if( q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC1144 )
            {
              if( q_item->pcomp_e_ptr->alg.xid_rfc1144.nsapis &
                  (0x01 << GSN_EXTERN_NSAPI(nsapi)) )
              {
                /* Delink NSAPI from the comp entity */
                actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC1144] = NO_COMP_ENT;
              }
            }
            else if( q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC2507 )
            {
              if( q_item->pcomp_e_ptr->alg.xid_rfc2507.nsapis &
                  (0x01 << GSN_EXTERN_NSAPI(nsapi)) )
              {
                /* Delink NSAPI from the comp entity */
                actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC2507] = NO_COMP_ENT;
              }
            }
            else if( q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC3095 )
            {
              if( q_item->pcomp_e_ptr->alg.xid_rfc3095.nsapis &
                  (0x01 << GSN_EXTERN_NSAPI(nsapi)) )
              {
                /* Delink NSAPI from the comp entity */
                actv_nsapis[ gas_id ][nsapi]->pcomp_e_num[T2_ALG_RFC3095] = NO_COMP_ENT;
              }
            }
            else
            {
              MSG_GERAN_ERROR_2("GSN ""XID %d: Inval Algorithm type",
                gsn_extern_sapi[sapi_ptr->sapi], q_item->pcomp_e_ptr->t2_alg);
            }
          }

          if( q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC1144 )
          {
            if( !q_cnt( &sapi_ptr->t2_alg_q[T2_ALG_RFC1144] ) )
            {
              /* No comp entities use the algorithm. Unassign PCOMP values */
              if ( q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp1  &&
                   sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp1] )
              {
                GSN_MEM_FREE(
                sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp1] );
              }
              if ( q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp2  &&
                   sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp2] )
              {
                GSN_MEM_FREE(
                sapi_ptr->pcomp_pool[q_item->pcomp_e_ptr->alg.xid_rfc1144.pcomp2] );
              }

              /* Free memory allocated for the instance of pcomp entity */
              if(sapi_ptr->pcomp_e_pool[q_item->pcomp_e_ptr->ent_num]->comp_e
                 != NULL)
              {
                gsn_slhc_free_mem(
                 gas_id, sapi_ptr->pcomp_e_pool[q_item->pcomp_e_ptr->ent_num]->comp_e);
              }

            }
          }
          else if( q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC2507 )
          {
            if( !q_cnt( &sapi_ptr->t2_alg_q[T2_ALG_RFC2507] ) )
            {
              /* No comp entities use the algorithm. Unassign PCOMP values */
              GSN_MEM_FREE( sapi_ptr->pcomp_pool
                [q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp1] );
              GSN_MEM_FREE( sapi_ptr->pcomp_pool
                [q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp2] );
              GSN_MEM_FREE( sapi_ptr->pcomp_pool
                [q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp3] );
              GSN_MEM_FREE( sapi_ptr->pcomp_pool
                [q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp4] );
              GSN_MEM_FREE( sapi_ptr->pcomp_pool
                [q_item->pcomp_e_ptr->alg.xid_rfc2507.pcomp5] );

              /* Free memory allocated for the instance of pcomp entity */

              if(sapi_ptr->pcomp_e_pool[q_item->pcomp_e_ptr->ent_num]->comp_e
                 != NULL)
              {
                gsn_iphc_free(
                 gas_id, sapi_ptr->pcomp_e_pool[q_item->pcomp_e_ptr->ent_num]->comp_e);
              }
            }
          }
          else if( q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC3095 )
          {
            if( !q_cnt( &sapi_ptr->t2_alg_q[T2_ALG_RFC3095] ) )
            {
              /* No comp entities use the algorithm. Unassign PCOMP values */
              GSN_MEM_FREE( sapi_ptr->pcomp_pool
                [q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp1] );
              GSN_MEM_FREE( sapi_ptr->pcomp_pool
                [q_item->pcomp_e_ptr->alg.xid_rfc3095.pcomp2] );

              /* Free memory allocated for the instance of pcomp entity */
              if(sapi_ptr->pcomp_e_pool[q_item->pcomp_e_ptr->ent_num]->comp_e
                 != NULL)
              {
                gsn_rohc_free(
                 gas_id, sapi_ptr->pcomp_e_pool[q_item->pcomp_e_ptr->ent_num]->comp_e);
              }

            }
          }
          else
          {
            MSG_GERAN_ERROR_0("GSN ""Inval Algorithm type");
          }


          /* Put entity back to the pool of unassigned entities */
          GSN_MEM_FREE( sapi_ptr->pcomp_e_pool[q_item->pcomp_e_ptr->ent_num] );

          /* free memory of queued item */
          GSN_MEM_FREE( q_item );

          break;
        }

        /* Get next queue item */
        q_item = q_next_item;
      }
    }
    else
    {
      /* At least one item should be in the queue */
      MSG_GERAN_ERROR_1("GSN ""XID %d: Resp inval T2 alg queue",
                  gsn_extern_sapi[sapi_ptr->sapi]);
    }
  }
  else
   MSG_GERAN_ERROR_1("GSN "" Inval T2 alg",
                gsn_extern_sapi[sapi_ptr->sapi]);
}
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#if( (defined(FEATURE_GSM_GPRS_SNDCP_PCOMP)) || (defined(FEATURE_GSM_GPRS_SNDCP_DCOMP)) )
/*===========================================================================
===
===  FUNCTION      GSN_RESET_COMP_ENT()
===
===  DESCRIPTION
===
===   Resets all assigned compression entities on the SAPI.
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
void gsn_reset_comp_ent( gas_id_t gas_id, gsn_sapi_e_t *sapi_ptr )
{
#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
{
  t2_alg_q_t        *q_item;
  uint8              alg;

  for( alg = 0; alg <= T2_ALG_MAX; alg++ )
  {
    q_item = q_check( &sapi_ptr->t2_alg_q[alg] );
    while( q_item != NULL )
    {
      if( (q_item->pcomp_e_ptr->op_mode == ACK) &&
          (q_item->pcomp_e_ptr->t2_alg == T2_ALG_RFC1144) )
      {
        gsn_reset_pcomp_ent( gas_id, q_item->pcomp_e_ptr );
      }

      /* Get next queue item */
      q_item = q_next( &sapi_ptr->t2_alg_q[alg], &q_item->link );
    }
  }
}
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */

#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
{
  t1_alg_q_t        *q_item;
  int8              alg;

  for( alg = 0; alg <= T1_ALG_MAX; alg++ )
  {
    q_item = q_check( &sapi_ptr->t1_alg_q[alg] );
    while( q_item != NULL )
    {
      if( (q_item->dcomp_e_ptr->op_mode == ACK) &&
          (q_item->dcomp_e_ptr->t1_alg == T1_ALG_V42BIS) )
      {
        gsn_reset_dcomp_ent( gas_id, q_item->dcomp_e_ptr );
      }
      /* Get next queue item */
      q_item = q_next( &sapi_ptr->t1_alg_q[alg], &q_item->link );
    }
  }
}
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */
}
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || defined(FEATURE_GSM_GPRS_SNDCP_DCOMP */

/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_RMV_NSAPI_XID()
===
===  DESCRIPTION
===
===   If a compression entity is used by the NSAPI, encode an XID block
===   to indicate to the SGSN SNDCP that the NSAPI shall be removed from
===   the "Applicable NSAPIs" of the compression entity.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   Pointer to the xid bock
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
dsm_item_type *gsn_encode_rmv_nsapi_xid( gas_id_t gas_id, const gsn_nsapi_e_t *nsapi_ptr )
{
  dsm_item_type  *xid_req_ptr     = NULL; /* xid block to be send to SGSN SNDCP */

#if ((defined FEATURE_GSM_GPRS_SNDCP_PCOMP) || (defined FEATURE_GSM_GPRS_SNDCP_DCOMP))

  dsm_item_type  *pxid_req_ptr    = NULL; /* xid parameters related to PCI compression */
  dsm_item_type  *dxid_req_ptr    = NULL; /* xid parameters related to Data compression */
  dsm_item_type  *ver_xid_req_ptr = NULL; /* xid parameters related to version number */

#ifdef FEATURE_GSM_GPRS_SNDCP_PCOMP
  {
    uint8           alg;
    gsn_xid_block_t xid_i;
    uint8           xid_ex[XID_T2_LEN_MAX];
    uint8           t2_len          = 0;
    t2_alg_q_t      *t2_q_item      = NULL;

    for( alg = 0; alg <= T2_ALG_MAX; alg++ )
    {
      if( nsapi_ptr->pcomp_e_num[alg] != NO_COMP_ENT )
      {
        t2_q_item = q_check( &nsapi_ptr->sapi_ptr->t2_alg_q[alg] );

        while( t2_q_item )
        {
          if( alg == T2_ALG_RFC1144 )
          {
            if( t2_q_item->pcomp_e_ptr->alg.xid_rfc1144.nsapis &
                (uint16)(0x01 << GSN_EXTERN_NSAPI(nsapi_ptr->nsapi)) )
            {
              xid_i.type   = XID_TYPE_2;
              xid_i.length = XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

              xid_i.type_field.xid_t2.hdr.p       = 0;
              xid_i.type_field.xid_t2.hdr.x1      = 0;
              xid_i.type_field.xid_t2.hdr.ent_num = t2_q_item->pcomp_e_ptr->ent_num;

              xid_i.type_field.xid_t2.hdr.x2       = 0;
              xid_i.type_field.xid_t2.hdr.alg_type = t2_q_item->pcomp_e_ptr->t2_alg;

              xid_i.type_field.xid_t2.hdr.length  = APP_NSAPIS_LEN;

              /* Clear bit in App NSAPIs */
              /*lint -e{502} expected unsigned type for bit operation */
              xid_i.type_field.xid_t2.alg.xid_rfc1144.nsapis &=
                ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( (nsapi_ptr->nsapi) ) );

              /* Encode T2 XID block */
              t2_len = gsn_encode_xid( &xid_i, xid_ex );

              /* check length of T2 XID */
              if( t2_len != (xid_i.length + XID_TYPE_HDR_LEN) )
                MSG_GERAN_ERROR_0("GSN ""XID encode. Error encoding T2 XID params");

              /* Create a pdu item to carry the XID block */
              pxid_req_ptr = gsn_pdu_construct( xid_ex, t2_len );
            }
          }
          else if( alg == T2_ALG_RFC2507 )
          {
            if( t2_q_item->pcomp_e_ptr->alg.xid_rfc2507.nsapis &
                (0x01 << GSN_EXTERN_NSAPI(nsapi_ptr->nsapi)) )
            {
              xid_i.type   = XID_TYPE_2;
              xid_i.length = XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

              xid_i.type_field.xid_t2.hdr.p       = 0;
              xid_i.type_field.xid_t2.hdr.x1      = 0;
              xid_i.type_field.xid_t2.hdr.ent_num = t2_q_item->pcomp_e_ptr->ent_num;

              xid_i.type_field.xid_t2.hdr.x2       = 0;
              xid_i.type_field.xid_t2.hdr.alg_type = t2_q_item->pcomp_e_ptr->t2_alg;

              xid_i.type_field.xid_t2.hdr.length  = APP_NSAPIS_LEN;

              /* Clear bit in App NSAPIs */
              /*lint -e{502} expected unsigned type for bit operation */
              xid_i.type_field.xid_t2.alg.xid_rfc2507.nsapis &=
                ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( (nsapi_ptr->nsapi) ) );

              /* Encode T2 XID block */
              t2_len = gsn_encode_xid( &xid_i, xid_ex );

              /* check length of T2 XID */
              if( t2_len != (xid_i.length + XID_TYPE_HDR_LEN) )
                MSG_GERAN_ERROR_0("GSN ""XID encode. Error encoding T2 XID params");

              /* Create a pdu item to carry the XID block */
              pxid_req_ptr = gsn_pdu_construct( xid_ex, t2_len );
            }
          }
          else if( alg == T2_ALG_RFC3095 )
          {
            if( t2_q_item->pcomp_e_ptr->alg.xid_rfc3095.nsapis &
                (0x01 << GSN_EXTERN_NSAPI(nsapi_ptr->nsapi)) )
            {
              xid_i.type   = XID_TYPE_2;
              xid_i.length = XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

              xid_i.type_field.xid_t2.hdr.p       = 0;
              xid_i.type_field.xid_t2.hdr.x1      = 0;
              xid_i.type_field.xid_t2.hdr.ent_num = t2_q_item->pcomp_e_ptr->ent_num;

              xid_i.type_field.xid_t2.hdr.x2       = 0;
              xid_i.type_field.xid_t2.hdr.alg_type = t2_q_item->pcomp_e_ptr->t2_alg;

              xid_i.type_field.xid_t2.hdr.length  = APP_NSAPIS_LEN;

              /* Clear bit in App NSAPIs */
              /*lint -e{502} expected unsigned type for bit operation */
              xid_i.type_field.xid_t2.alg.xid_rfc3095.nsapis &=
                ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( (nsapi_ptr->nsapi) ) );

              /* Encode T2 XID block */
              t2_len = gsn_encode_xid( &xid_i, xid_ex );

              /* check length of T2 XID */
              if( t2_len != (xid_i.length + XID_TYPE_HDR_LEN) )
                MSG_GERAN_ERROR_0("GSN ""XID encode. Error encoding T2 XID params");

              /* Create a pdu item to carry the XID block */
              pxid_req_ptr = gsn_pdu_construct( xid_ex, t2_len );
            }
          }

          /* Get next entity from the queue */
          t2_q_item = q_next( &nsapi_ptr->sapi_ptr->t2_alg_q[alg], &t2_q_item->link );
        }
      }
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP */


#ifdef FEATURE_GSM_GPRS_SNDCP_DCOMP
  {
    gsn_xid_block_t xid_i;
    uint8           xid_ex[XID_T1_LEN_MAX];
    uint8           t1_len          = 0;
    t1_alg_q_t      *t1_q_item      = NULL;
    /* Change to uint8 when more data compression algorithms added and
    ** T1_ALG_MAX != 0. int8 is used to avoid unassigned comparison with 0 warning.
    */
    int8            alg;


    for( alg = 0; alg <= T1_ALG_MAX; alg++ )
    {
      if( nsapi_ptr->dcomp_e_num[alg] != NO_COMP_ENT )
      {
        t1_q_item = q_check( &nsapi_ptr->sapi_ptr->t1_alg_q[alg] );

        while( t1_q_item )
        {
          if( alg == T1_ALG_V42BIS )
          {
            if( t1_q_item->dcomp_e_ptr->alg.xid_v42bis.nsapis &
                (0x01 << GSN_EXTERN_NSAPI(nsapi_ptr->nsapi)) )
            {
              xid_i.type   = XID_TYPE_1;
              xid_i.length = XID_FIELD_HDR_P0_LEN + APP_NSAPIS_LEN;

              xid_i.type_field.xid_t1.hdr.p       = 0;
              xid_i.type_field.xid_t1.hdr.x1      = 0;
              xid_i.type_field.xid_t1.hdr.ent_num = t1_q_item->dcomp_e_ptr->ent_num;

              xid_i.type_field.xid_t1.hdr.x1       = 0;
              xid_i.type_field.xid_t1.hdr.alg_type = t1_q_item->dcomp_e_ptr->t1_alg;

              xid_i.type_field.xid_t1.hdr.length  = APP_NSAPIS_LEN;

              /* Clear bit in App NSAPIs */
              /*lint -e{502} expected unsigned type for bit operation */
              xid_i.type_field.xid_t1.alg.xid_v42bis.nsapis &=
                ~(uint16)( 0x1 << GSN_EXTERN_NSAPI( (nsapi_ptr->nsapi) ));

              /* Encode T2 XID block */
              t1_len = gsn_encode_xid( &xid_i, xid_ex );

              /* check length of T2 XID */
              if( t1_len != (xid_i.length + XID_TYPE_HDR_LEN) )
                MSG_GERAN_ERROR_0("GSN ""XID encode. Error encoding T1 XID params");

              /* Create a pdu item to carry the XID block */
              dxid_req_ptr = gsn_pdu_construct( xid_ex, t1_len );
            }
          }

          /* Get next entity from the queue */
          t1_q_item = q_next( &nsapi_ptr->sapi_ptr->t1_alg_q[alg], &t1_q_item->link );
        }
      }
    }
  }
#endif /* FEATURE_GSM_GPRS_SNDCP_DCOMP */

  if( pxid_req_ptr || dxid_req_ptr )
  {
    uint8          t0_xid_par[XID_T0_LEN_MAX];

    /* Encode T0 xid parameter - SNDCP version number.
    ** This parameter will not be send to the peer SNDCP
    ** if no T1 or T2 xid parameters need to be proposed to the peer.
    ** It will only be part of an xid block if the block contains
    ** a PCI (T2) or/and a data (T1) compression entity.
    */
    t0_xid_par[0] = XID_TYPE_0; /* Type 0 XID parameter */
    t0_xid_par[1] = 0x01;       /* Length of Version Number */
    t0_xid_par[2] = nsapi_ptr->sn_xid_req.ver_num;  /* SNDCP Version Number */

    /* Allocate a dsm item to carry the T0 xid parameter */
    ver_xid_req_ptr = gsn_pdu_construct( t0_xid_par, XID_T0_LEN_MAX );

    /* Concatenate the XID parameters and form the XID
    ** block to be send to the peer.
    */
    if( pxid_req_ptr && dxid_req_ptr )
    {
      xid_req_ptr = ver_xid_req_ptr;
      gprs_pdu_augment_tail( &xid_req_ptr, &dxid_req_ptr );
      gprs_pdu_augment_tail( &xid_req_ptr, &pxid_req_ptr );
    }
    else if( pxid_req_ptr )
    {
      xid_req_ptr = ver_xid_req_ptr;
      gprs_pdu_augment_tail( &xid_req_ptr, &pxid_req_ptr );
    }
    else if( dxid_req_ptr )
    {
      xid_req_ptr = ver_xid_req_ptr;
      gprs_pdu_augment_tail( &xid_req_ptr, &dxid_req_ptr );
    }
    else
      xid_req_ptr = NULL;

    /* Duplicate XID block, might need to be retransmitted */
    if( xid_req_ptr )
    {
      if( nsapi_ptr->sapi_ptr->xid_req_dup_ptr == NULL )
      {
        nsapi_ptr->sapi_ptr->xid_req_dup_ptr =
          gprs_pdu_duplicate( xid_req_ptr, 0, GPRS_PDU_LENGTH(xid_req_ptr) );

        if( nsapi_ptr->sapi_ptr->xid_req_dup_ptr == NULL )
        {
          MSG_GERAN_ERROR_0("GSN ""gprs_pdu_duplicate() failed. No dsm resources");
        }
      }
      else
      {
        MSG_GERAN_HIGH_1("GSN ""LL %d: Inval xid_req_dup_ptr",
                        gsn_extern_sapi[nsapi_ptr->sapi_ptr->sapi]);
      }
    }
  }

#endif /* FEATURE_GSM_GPRS_SNDCP_PCOMP || FEATURE_GSM_GPRS_SNDCP_DCOMP */

  return( xid_req_ptr );
}


/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_DEF_XID()
===
===  DESCRIPTION
===
===   Encodes T0, T1 and T2 XID parameters with default values
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   Pointer to the xid bock
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
dsm_item_type *gsn_encode_def_xid ( void )
{
  dsm_item_type       *xid = NULL;

  /* Create a pdu item to carry the XID block */
  xid = gsn_pdu_construct( (void *)xid_all_types_def, XID_DEF_ALL_TYPE_LEN_BLK );

  return( xid );
}

/*===========================================================================
===
===  FUNCTION      GSN_PRINT_XID_PARAMS()
===
===  DESCRIPTION
===
===   Dumps the XID PDU in hex format in a F3 message up to 24 octets
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   Pointer to the xid bock
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_print_xid_params ( gas_id_t gas_id, dsm_item_type  *xid_pdu, boolean ul )
{
  dsm_item_type  *xid_pdu_dup;
  uint8          src[255];
  uint8          *ptr;
  uint8          len;
  uint32         a, b, c;

  if( xid_pdu )
  {
    len = (uint8) GPRS_PDU_LENGTH(xid_pdu);

    xid_pdu_dup = gprs_pdu_duplicate( xid_pdu, 0, len );

    if( !gprs_pdu_strip_head( &xid_pdu_dup, src, len) )
    {
      MSG_GERAN_MED_0("GSN ""DEBUG: XID strip to buff failed");
    }
    else
    {
      ptr = &src[0];

      a = 0L;
      a |=  (uint32)(*ptr++) << 24;
      a |=  (uint32)(*ptr++) << 16;
      a |=  (uint32)(*ptr++) << 8;
      a |=  (uint32)(*ptr++) << 0;

      b = 0L;
      b |=  (uint32)(*ptr++) << 24;
      b |=  (uint32)(*ptr++) << 16;
      b |=  (uint32)(*ptr++) << 8;
      b |=  (uint32)(*ptr++) << 0;

      c = 0L;
      c |=  (uint32)(*ptr++) << 24;
      c |=  (uint32)(*ptr++) << 16;
      c |=  (uint32)(*ptr++) << 8;
      c |=  (uint32)(*ptr++) << 0;

      if( ul == TRUE )
      {
        MSG_GERAN_MED_1("GSN ""UL XID len %d", len);
        MSG_GERAN_MED_3("GSN ""UL XID %08x %08x %08x", a, b, c );
      }
      else
      {
        MSG_GERAN_MED_1("GSN ""DL XID len %d", len);
        MSG_GERAN_MED_3("GSN ""DL XID %08x %08x %08x", a, b, c );
      }

      if( len > 12 )
      {
        a = 0L;
        a |=  (uint32)(*ptr++) << 24;
        a |=  (uint32)(*ptr++) << 16;
        a |=  (uint32)(*ptr++) << 8;
        a |=  (uint32)(*ptr++) << 0;

        b = 0L;
        b |=  (uint32)(*ptr++) << 24;
        b |=  (uint32)(*ptr++) << 16;
        b |=  (uint32)(*ptr++) << 8;
        b |=  (uint32)(*ptr++) << 0;

        c = 0L;
        c |=  (uint32)(*ptr++) << 24;
        c |=  (uint32)(*ptr++) << 16;
        c |=  (uint32)(*ptr++) << 8;
        c |=  (uint32)(*ptr++) << 0;

        if( ul == TRUE )
        {
          MSG_GERAN_MED_3("GSN ""UL XID cont %08x %08x %08x", a, b, c );
        }
        else
        {
          MSG_GERAN_MED_3("GSN ""DL XID cont %08x %08x %08x", a, b, c );
        }
      }
    }
  }
  else
  {
    MSG_GERAN_MED_0("GSN ""DEBUG: Empty XID PDU ");
  }
}



/*** EOF: don't remove! ***/

