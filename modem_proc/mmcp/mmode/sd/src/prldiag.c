/*==========================================================================
          Diagnostic System Packet Definitions for PRL

  Description: Packet definitions to support the interface between the
  DMSS software and the external device for Preffered Roaming list-related
  packets.

Copyright (c) 2007 - 2009 Qualcomm Technologies Incorporated. All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/



/*===========================================================================

                            Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/prldiag.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/21/11   fj      Fix klocwork errors.
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/10/11   am      Adding qtf support
09/22/10   gm      KW fix replaced nv_max_size_of_roaming_list() with
                   NV_ROAMING_LIST_MAX_SIZE
08/06/10   aj      Replaced SD_ERR with SD_MSG_ERROR
02/09/10   mj      16K PRL read error from QPST
09/19/09   sv      KW Fixes
03/03/09   fj      Move this file to MMODE SD VU.
                   Suppress lint error 641.
10/31/05   as      Fixed lint errors.
05/04/05   as      Featurized nv_stat_enum_type to ensure uint16 for
                   external API.
03/01/05   jqi     Leave PRL_ENALBED flag intact if PRL is invalide.
02/23/05   as      Included diagnv.h to prevent compiler warnings
11/20/03   as      Created file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "comdef.h"
#include "prl.h"
#include "diag.h"
#include "msg.h"
#include "prldiag.h"
#include "diagcmd.h"
#include "diagnv.h"
#include "sddbg.h"    /* SD debug macros */
#include "sdi.h"
#include "prl_v.h"
#include "bit.h"
#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

/* For memscpy */
#include <stringl/stringl.h>

#ifdef __cplusplus
  extern "C" {
#endif
/*--------------------------------------------------------------------------
  Local variables used in reading and writing the preferred roaming list,
  for phones which support Preferred Roaming, only.  This includes an access
  variable, and a buffer to hold the list being read or written.
--------------------------------------------------------------------------*/
extern nv_roaming_list_type nv_pr_list;  /* buffer to hold preferred
                                            roaming list table being
                                            sent to or read from NV
                                          */
#define DIAG_PRL_ACT_NONE  0
#define DIAG_PRL_ACT_READ  1
#define DIAG_PRL_ACT_WRITE 2
LOCAL byte pr_list_activity = DIAG_PRL_ACT_NONE;

#ifndef FEATURE_HWTC
/*===========================================================================

FUNCTION PRLDIAG_PR_LIST_WR

DESCRIPTION
  This procedure processes a request to write a preferred roaming list.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
PACKED void * prldiag_pr_list_wr (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{
  static word    cur_seq_num = 0;     /* Current sequence number         */
  static dword   cur_prl_index = 0;   /* Current index into nv_pr_list */
  static boolean got_first = FALSE;   /* Did we get the first one yet?   */
  static dword   bit_count = 0;       /* Count of valid pr_list bits     */
  dword          byte_count = 0;      /* working variable for range checking */
  word           prl_size=0;
  nv_items_enum_type item_id = (nv_items_enum_type)0;
word offset = 0;

  diag_pr_list_wr_req_type *req_ptr = (diag_pr_list_wr_req_type *) req_pkt_ptr;
  diag_pr_list_wr_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_pr_list_wr_rsp_type);

  nv_item_type  nv_item;  /* NV Item buffer for NV interface */
  prl_sspr_p_rev_e_type sspr_p_rev; /* PRL protocol revision */

  boolean        flag = TRUE;

  /* First make sure the SPC has been entered properly or that
  ** security is unlocked.  If it hasn't, we will return an error packet.
  */
  if (diag_get_sp_state () == DIAG_SPC_LOCKED &&
      (diag_get_security_state() == DIAG_SEC_LOCKED  ||
      diag_check_password (NULL) == TRUE))
  {
    return ( diagpkt_err_rsp ( DIAG_BAD_SPC_MODE_F, req_pkt_ptr, pkt_len ) );
  }

  /* Check for bad parameters.*/
  else if (req_ptr->num_bits > DIAG_PR_LIST_BLOCK_SIZE * 8) {
    return ( diagpkt_err_rsp ( DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len ) );
  }

  rsp_ptr = (diag_pr_list_wr_rsp_type *)diagpkt_alloc(DIAG_PR_LIST_WR_F, rsp_len);

  if(rsp_ptr == NULL) {
    SD_ERR_0( "diag rsp_ptr is NULL");
    return rsp_ptr;
  }

  /* Check for first packet.*/
  do
  {
    flag = FALSE;

    if (req_ptr->seq_num == 0) {
      got_first = TRUE;

      /* Reset the indicies and counts
      */
      cur_seq_num = 0;
      cur_prl_index = 0;
      bit_count = 0;

      pr_list_activity = DIAG_PRL_ACT_WRITE;
    }

    else {

      /* Check to make sure no-one has tried to read the pr_list in between
      ** write packets!  Treat this like an out of sequence packet.
      */
      if (pr_list_activity != DIAG_PRL_ACT_WRITE) {
        rsp_ptr->rl_status = DIAG_RL_WR_OUT_OF_SEQ;
        break;
      }

      /* Check to see if the sequence number is correct.*/
      if ((req_ptr->seq_num == (cur_seq_num + 1)) && got_first) {
        cur_seq_num++;
      }

      /* Check for duplicate packets.*/
      else if ((req_ptr->seq_num == cur_seq_num) && got_first) {
        /* Just acknowledge it
        */
        rsp_ptr->rl_status = DIAG_RL_WR_OK;
        break;
      }

      /* Bad sequence number.*/
      else {
        rsp_ptr->rl_status = DIAG_RL_WR_OUT_OF_SEQ;
        break;
      }
    }

    /* Check see if the current packet will fit in the buffer.
    ** Current packet size is in bits, so we must convert to the next
    ** largest number of bytes.
    */
    byte_count = (req_ptr->num_bits + 7) / 8;
    if (byte_count > (NV_ROAMING_LIST_MAX_SIZE - cur_prl_index)) {
      SD_MSG_HIGH_0("returning DIAG_RL_WR_OVERFLOW byte_count > (NV_ROAMING_LIST_MAX_SIZE - cur_prl_index)");	
      rsp_ptr->rl_status = DIAG_RL_WR_OVERFLOW;
      break;
    }


    if(cur_prl_index == 0)
    {
      prl_size =   SD_B_UNPACKW( req_ptr->pr_list_data , offset , PR_LIST_SIZE_LENGTH);
      SD_MSG_LOW_2("byte0 = %d byte1= %d ", req_ptr->pr_list_data[0], req_ptr->pr_list_data[1]);
       /*
      ** checking the length of prl . If length in header(starting 2 bytes of PRL) 
      ** is more than buffer size then status will be buffer overflow    
       */
      if(prl_size > NV_ROAMING_LIST_MAX_SIZE)
      {
        SD_MSG_HIGH_1("returning DIAG_RL_WR_OVERFLOW prl_size %d > NV_ROAMING_LIST_MAX_SIZE", prl_size);
        rsp_ptr->rl_status = DIAG_RL_WR_OVERFLOW ;
        break ;
      }
    }

    /* Didn't find any problems. Guess it's ok.*/
    memscpy( (void *) &(nv_pr_list.roaming_list[cur_prl_index]),
             ((ARR_SIZE(nv_pr_list.roaming_list) - cur_prl_index) * sizeof(nv_pr_list.roaming_list[0]) ),
             (void *) req_ptr->pr_list_data, byte_count );

    cur_prl_index += byte_count;
    bit_count += req_ptr->num_bits;

    if (req_ptr->more)
    {
      rsp_ptr->rl_status = DIAG_RL_WR_OK;
    }
    else
    {
      word prl_version = PRL_DEFAULT_VER;

      got_first = FALSE;
      rsp_ptr->rl_status = DIAG_RL_WR_OK_DONE;

      /* Set up the NAM and size for this roaming list */
      nv_pr_list.nam = req_ptr->nam;
      nv_pr_list.size = bit_count;

      /*lint -save -e641*/
      /* Get PRL protocol revision from NV. Validate PRL revision from nv first
      ** then with all other available protocol revisions next.
      ** Update revision in nv if returned revision does not match that from nv.
      */
      if ( diag_nv_read ( NV_SSPR_P_REV_I, &nv_item ) != NV_DONE_S ) {
         SD_MSG_HIGH_0("Failed to read PRL protocol revision");
         nv_item.sspr_p_rev = PRL_SSPR_P_REV_INVALID;
         sspr_p_rev = PRL_SSPR_P_REV_1;
      }
      else{
         sspr_p_rev = (prl_sspr_p_rev_e_type)nv_item.sspr_p_rev;
      }

      nv_pr_list.valid = FALSE;
      if( prl_validate((byte*) nv_pr_list.roaming_list, &prl_version, &sspr_p_rev, TRUE) == PRL_VALID)
      {
        if ( sspr_p_rev != nv_item.sspr_p_rev) {
          nv_item.sspr_p_rev = sspr_p_rev;
          (void) diag_nv_write( NV_SSPR_P_REV_I, &nv_item);
        }
        nv_pr_list.valid = TRUE;
      }
      /*lint -restore */

      /* Set the PRL version */
      nv_pr_list.prl_version = prl_version;

      #ifdef FEATURE_IS683A_PRL
      item_id = NV_ROAMING_LIST_683_I;
      #else
      item_id = NV_ROAMING_LIST_I;
      #endif

      #ifdef FEATURE_NV_ENUMS_ARE_WORDS
      #error code not present
#else
      rsp_ptr->nv_stat = (uint16)diag_nv_write (item_id, &nv_pr_list);
      #endif
    }
  } while(flag);

  return (rsp_ptr);

}  /* prldiag_pr_list_wr */



/*===========================================================================

FUNCTION PRLDIAG_PR_LIST_RD

DESCRIPTION
  This procedure processes a request to read a preferred roaming list.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
PACKED void * prldiag_pr_list_rd (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{
  dword byte_count = 0;           /* working variable for range checking */
  nv_stat_enum_type nv_stat;     /* To catch the status from nv_read */
  byte cur_seq_num;              /* Sequence number from current packet */
  uint16 cur_index;                /* current index into the roaming list */

  static byte verify_nam;        /* Save the NAM of the request packet for
                                    verification next time */
  diag_pr_list_rd_req_type *req_ptr = (diag_pr_list_rd_req_type *) req_pkt_ptr;
  diag_pr_list_rd_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_pr_list_rd_rsp_type);

  /*-----------------------------------------------------------------------
    If the current activity going on in the nv_pr_list is not a READ, or
    if the current sequence number is 0, read the pr_list from NV.
  -----------------------------------------------------------------------*/
  if ((pr_list_activity != DIAG_PRL_ACT_READ) ||
      (req_ptr->seq_num == 0)) {

    /* Read the PR_LIST associated with the given NAM
    */
    nv_pr_list.nam = req_ptr->nam;

    /*lint -save -e826*/
    if ((nv_stat = diag_nv_read(
                                #ifdef FEATURE_IS683A_PRL
                                NV_ROAMING_LIST_683_I,
                                #else
                                NV_ROAMING_LIST_I,
                                #endif
                               (nv_item_type *)&nv_pr_list)) != NV_DONE_S )
    /*lint -restore*/
    {

      rsp_ptr = (diag_pr_list_rd_rsp_type *)diagpkt_alloc(DIAG_PR_LIST_RD_F, rsp_len);

      if(rsp_ptr == NULL)
      {
        SD_ERR_0("Unable to allocate diag pkt");
        return rsp_ptr;
      }

      SD_MSG_HIGH_0("Failed to read PR List (normally)!");

      /* Send the response with NV status */
      #ifdef FEATURE_IS683A_PRL
      rsp_ptr->rl_status = DIAG_RL_RD_NV_683A_ERR;
      #else
      rsp_ptr->rl_status = DIAG_RL_RD_NV_ERR;
      #endif

      #ifdef FEATURE_NV_ENUMS_ARE_WORDS
      #error code not present
#else
      rsp_ptr->nv_stat = (uint16)nv_stat;
      #endif
      rsp_ptr->seq_num = req_ptr->seq_num;
      return (rsp_ptr);
    }
    else {
      verify_nam = req_ptr->nam;
      pr_list_activity = DIAG_PRL_ACT_READ;
    }
  }

  /*-----------------------------------------------------------------------
    Check for bad parameters.
  -----------------------------------------------------------------------*/
  cur_seq_num = req_ptr->seq_num;
  byte_count = (nv_pr_list.size + 7) / 8;  /* num of bytes with data */

  /* If the NAM doesn't match the NAM it should, then it's an error */
  if (req_ptr->nam != verify_nam) {
    SD_MSG_LOW_0 ( "bad parm (nam mismatch)");
    return ( diagpkt_err_rsp ( DIAG_BAD_PARM_F, req_ptr, pkt_len ) );
  }

  /* If the valid number of bytes is 0, and the seq_num is NOT 0, then the
     request is bad, or if the index into the read buffer is past the end
     of valid data, the request is bad.
  */
  else if ( ((byte_count == 0) && (cur_seq_num != 0)) ||
            ((((dword)cur_seq_num * DIAG_PR_LIST_BLOCK_SIZE) >= byte_count)) ) 
  {

    /* Handle NV exists, but there is no data case. Send failure reponse not bad parameter */
    if ( byte_count  == 0 )
    {
      rsp_ptr = (diag_pr_list_rd_rsp_type *)diagpkt_alloc(DIAG_PR_LIST_RD_F, rsp_len);

      if(rsp_ptr == NULL)
      {
        SD_ERR_0("Unable to allocate diag pkt");
        return rsp_ptr;
      }

      SD_MSG_HIGH_0("Failed to read PR List (normally)!");

      /* Send the response with NV status */
#ifdef FEATURE_IS683A_PRL
      rsp_ptr->rl_status = DIAG_RL_RD_NV_683A_ERR;
#else
      rsp_ptr->rl_status = DIAG_RL_RD_NV_ERR;
#endif

#ifdef FEATURE_NV_ENUMS_ARE_WORDS
      #error code not present
#else
      rsp_ptr->nv_stat = (uint16)NV_NOTACTIVE_S;//nv_stat;
#endif
      rsp_ptr->seq_num = req_ptr->seq_num;
      return (rsp_ptr);

    }
    
    return ( diagpkt_err_rsp ( DIAG_BAD_PARM_F, req_ptr, pkt_len ) );
  }

  /*-----------------------------------------------------------------------
    Didn't find any problems. Guess it's ok.
  -----------------------------------------------------------------------*/
  else
  {
    rsp_ptr = (diag_pr_list_rd_rsp_type *)diagpkt_alloc (DIAG_PR_LIST_RD_F, rsp_len);

    /* validate rsp_ptr */
    if( rsp_ptr == NULL )
    {
      SD_ERR_0( "diag rsp_ptr is NULL");
      return rsp_ptr;
    }

    /* Set up fields in response packet that are always the same */
    #ifdef FEATURE_IS683A_PRL
    rsp_ptr->rl_status = DIAG_RL_RD_683A_OK;
    #else
    rsp_ptr->rl_status = DIAG_RL_RD_OK;
    #endif
    /*lint -save -e641*/
    rsp_ptr->nv_stat = NV_DONE_S;
    /*lint -restore */
    rsp_ptr->seq_num = req_ptr->seq_num;

    /* The index that we'll be copying from
    */
    cur_index = cur_seq_num * DIAG_PR_LIST_BLOCK_SIZE;

    /* Byte count of zero is a special case - we don't need to copy
    ** anything - just return a bit count of 0, and more of 0, since
    ** there's no more data.
    */
    if ((byte_count == 0) || (byte_count < cur_index)) {
      rsp_ptr->num_bits = 0;
      rsp_ptr->more = 0;
    }

    /* If the number of bytes left after this index is larger than the
    ** block size, then just copy a block size into the return packet, set
    ** the number of bits to the block size * 8 and more is always 1, since
    ** there's more data after this.
    */
    else if ( (byte_count - cur_index) > DIAG_PR_LIST_BLOCK_SIZE ) {

      SD_MSG_LOW_1 ( "big block %ld (more)", (byte_count - cur_index));
      memscpy( (void *) rsp_ptr->pr_list_data,
               sizeof(rsp_ptr->pr_list_data),
               (void *) &(nv_pr_list.roaming_list[cur_index]),
               DIAG_PR_LIST_BLOCK_SIZE );
      rsp_ptr->num_bits = DIAG_PR_LIST_BLOCK_SIZE * 8;
      rsp_ptr->more = 1;
    }

    /* If the number of bytes left after this index is less than or equal to
    ** the block size, then copy the remaining bytes into the return packet,
    ** set the number of bits to the number of BITS in the whole list minus
    ** the current index * 8, and set more to 0, since there's no more
    ** data after this.
    */
    else {

      if (cur_index >= NV_ROAMING_LIST_MAX_SIZE) 
      {
        SD_ERR_1("cur_index = %d, out of boundary",cur_index);
        diagpkt_free(PACKED(void *)rsp_ptr);
        return (rsp_ptr);
      }

      if (byte_count <= cur_index) 
      {
        SD_ERR_1("byte_count <= cur_index, byte_count = %d",
                     byte_count);
        diagpkt_free(PACKED(void *)rsp_ptr);
        return (rsp_ptr);
      }

      memscpy( (void *) rsp_ptr->pr_list_data,
               sizeof(rsp_ptr->pr_list_data),
               (void *) &(nv_pr_list.roaming_list[cur_index]),
               (byte_count - cur_index) );
      rsp_ptr->num_bits = (word)(nv_pr_list.size - (cur_index*8));
      rsp_ptr->more = 0;
    }

    return (rsp_ptr);
  }

}  /* prldiag_pr_list_rd */
#endif /* !FEATURE_HWTC */

/**************************************************************************
** TMC diag DISPATCH TABLE.
Note:  Any new commands for TMC need to be added here.
****************************************************************************/
#ifndef FEATURE_HWTC
static const diagpkt_user_table_entry_type prldiag_tbl[] =
{
  {DIAG_PR_LIST_WR_F, DIAG_PR_LIST_WR_F, prldiag_pr_list_wr},
  {DIAG_PR_LIST_RD_F, DIAG_PR_LIST_RD_F, prldiag_pr_list_rd}
};
#endif

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, prldiag_tbl);

#else

  void prldiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, prldiag_tbl);
  }

#endif

/*lint +esym(715,pkt_len,req_ptr) */
//#endif /* #ifndef FEATURE_HWTC */

#ifdef __cplusplus
  }
#endif


