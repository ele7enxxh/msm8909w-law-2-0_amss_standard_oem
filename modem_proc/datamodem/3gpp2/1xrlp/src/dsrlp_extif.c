/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  R A D I O   L I N K   P R O T O C O L  E X T E R N A L  I N T E R F A C E 

GENERAL DESCRIPTION
  This file contains externalized RLP interface functions to be used by 
  other modules. These modules could be other modules within data services
  or outside data services.  

EXTERNALIZED FUNCTIONS

DESCRIPTION
  

 Copyright (c) 2003 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlp_extif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
01/18/11   op         Migrated to MSG 2.0 Macros
07/05/04   gr         Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "data_msg.h"

#if (defined( FEATURE_DS_RLP3) || defined (FEATURE_HDR))

#include "dsrlpi.h"
#include "dsrlp_extif.h"

/*===========================================================================

            EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    PUBLIC FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       DSRLP_EXTIF_GET_NAK_CNT

DESCRIPTION    This function checks if RLP is in Established state and 
               populates the nak_count_ptr. It returns TRUE if the NAK
               Q is not empty. Otherwise it returns FALSE. 
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE if NAK Q is non-empty

SIDE EFFECTS   
===========================================================================*/
boolean dsrlp_extif_get_nak_cnt
(
  uint32 *nak_count_ptr
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr=NULL;  /* rlp-specific control block  */
  boolean status = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (nak_count_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Input ptr NULL");
    return status;
  }

  /*-------------------------------------------------------------------------
    Retrieve the NAK count for the RLP instance DSRLP_FIXED_SR_ID_INDEX. 
  -------------------------------------------------------------------------*/
  *nak_count_ptr = 0;
  rscb_ptr      = &(dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX]);

  if ( rscb_ptr->rlp_list[0].state.state == DSRLPI_ESTABLISHED_STATE)
  {
    *nak_count_ptr =  (uint32)q_cnt(&(rscb_ptr->rlp_list[0].state.nak_q));
    if( (*nak_count_ptr) > 0) 
    {
      DATA_RLP_MSG0(MSG_LEGACY_MED, "NAK queue is not null - NACK pending");
    }
    status = TRUE;
  }
  return status; 
}

/*===========================================================================

FUNCTION       DSRLP_EXTIF_GET_IDLE_TX_FRAME_CNT

DESCRIPTION    This function checks if RLP is in Established state and 
               populates the idle_tx_fr_cnt_ptr. It returns TRUE if RLP is
               the Established state. Otherwise it returns FALSE. 
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE if RLP is in the ESTABLISHED state

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlp_extif_get_idle_tx_frame_cnt
(
  uint32 *idle_tx_fr_cnt_ptr /* ptr to tx idle frame count to be populated */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr=NULL;  /* rlp-specific control block  */
  boolean status = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (idle_tx_fr_cnt_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Input ptr NULL");
    return status;
  }

  *idle_tx_fr_cnt_ptr = 0; 
  /*-------------------------------------------------------------------------
    Retrieve the contiguous tx idle frame count for the RLP 
    instance DSRLP_FIXED_SR_ID_INDEX. 
  -------------------------------------------------------------------------*/
  rscb_ptr      = &(dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX]);

  if ( rscb_ptr->rlp_list[0].state.state == DSRLPI_ESTABLISHED_STATE)
  {
    *idle_tx_fr_cnt_ptr =  rscb_ptr->rlp_list[0].stats.tx_contig_idle_fr_cnt;
    status = TRUE;
  }

  return status;

}

/*===========================================================================

FUNCTION       DSRLP_EXTIF_GET_RX_IDLE_FRAME_CNT

DESCRIPTION    This function checks if RLP is in Established state and 
               populates the idle_rx_fr_cnt_ptr. It returns TRUE if RLP is
               the Established state. Otherwise it returns FALSE. 
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE if RLP is in the ESTABLISHED state

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlp_extif_get_idle_rx_frame_cnt
(
  uint32 *idle_rx_fr_cnt_ptr /* ptr to rx idle frame count to be populated */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr=NULL;  /* rlp-specific control block  */
  boolean status = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (idle_rx_fr_cnt_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Input ptr NULL");
    return status;
  }

  *idle_rx_fr_cnt_ptr = 0; 

  /*-------------------------------------------------------------------------
    Retrieve the contiguous rx idle frame count for the RLP 
    instance DSRLP_FIXED_SR_ID_INDEX. 
  -------------------------------------------------------------------------*/
  rscb_ptr      = &(dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX]);

  if ( rscb_ptr->rlp_list[0].state.state == DSRLPI_ESTABLISHED_STATE)
  {
    *idle_rx_fr_cnt_ptr =  rscb_ptr->rlp_list[0].stats.rx_contig_idle_fr_cnt;
    status = TRUE;
  }

  return status;

}

#endif /* any of the RLPs defined */
