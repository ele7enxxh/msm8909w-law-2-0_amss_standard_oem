#ifndef _DSRLPTXQ_H
#define _DSRLPTXQ_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      R A D I O   L I N K   P R O T O C O L  T X Q   P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains generic txq-list processing functions. 
  The Transmit Queue is a queue of all the frames the mobile RLP
  has sent. The Transmit Queue is used to buffer sent frames, that might
  be used if the other side wants a retransmission. 
  
  The transmit queue is not  specific to any one RLP type, but is a general
  purpose data structure.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 1999 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlptxq.h_v   1.3   02 Dec 2002 22:03:34   sarithay  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/inc/dsrlptxq.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
12/02/02   sy         Changed the footer field from byte to unit16 in
                      dsrlptxq_fr_param_type structure.
05/24/01   hcg        Made changes to support HDR (1xEV).
11/14/00   igt        Changed the implementation of the data structure to a 
                      queue instead of an array of pointers to dsm items.
                      Remove not needed function signatures. 
08/05/99   na/hcg/ak  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"

#ifdef FEATURE_DS

#include "dsrlp_v.h"
#include "dsrlpi.h"


/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#define  DSRLPTXQ_NO_TXQ         0xFFFFFFFF

/*---------------------------------------------------------------------------
  Type that contains all the information for a frame that needs to be 
  inserted into the transmit queue.
---------------------------------------------------------------------------*/
typedef struct 
{
  uint32                  seq_num;           /* Inernal seq num of frame   */
  dsm_item_type          *fr_ptr;            /* Ptr to frame to put in txq */
  byte                    header;            /* # of header bytes in frame */
  uint16                  footer;            /* # of tailer(type bits in fr*/
} dsrlptxq_fr_param_type;




/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                       MACRO DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                      FUNCTION DECLARATIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION      DSRLPTXQ_INIT

DESCRIPTION   This function is called up from dsrlp_init() during the RLP 
              session initialization.  It initializes the rlp TX Queue.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlptxq_init(dsrlpi_sess_ctl_blk_type *rscb_ptr);


/*===========================================================================

FUNCTION      DSRLPTXQ_RESET
             
DESCRIPTION   This goes ahead and clears out a transmit q of all of its
              items.  Called from dsrlpi_reset() if RLP is reset or gotten
              rid of.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlptxq_reset
(
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr 
);


/*===========================================================================

FUNCTION      DSRLPTXQ_ADD_FRAME

DESCRIPTION   This function adds a new frame into the transmit queue. This
              function is called whenever RLP sends a new frame. It runs in 
              the context of the task that does RLP TX.
              
              If the txq is full, then the oldest frame is removed from the
              list to make room for the newest frame.

DEPENDENCIES  Called from Tx task.

RETURN VALUE  None

SIDE EFFECTS  The input RLP frame is stored in a potential chain of dsm 
              items.  The first dsm item's q_link field will be used to
              hold the sequence number of the frame.  
===========================================================================*/
void dsrlptxq_add_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* current RLP ctl blk        */
  dsrlptxq_fr_param_type       *fr_param_ptr /* Info. of fr. to be inserted*/
);

/*===========================================================================

FUNCTION      DSRLPTXQ_REMOVE_ACKED

DESCRIPTION   Given the peer's L_V(N), this function removes from the array
              any frames whose sequence number is less than the input L_V(N).
              
              Normally, this is called when a NAK frame is received, as that
              includes the peer's L_V(N).
              
DEPENDENCIES  Called from Rx task.

RETURN VALUE  None

SIDE EFFECTS  NONE
===========================================================================*/
void dsrlptxq_remove_acked
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to RLP ctl block        */
  uint32                        l_vn        /* peer's L_V(N), formatted to */
                                            /* internal 32-bit format      */
);

/*===========================================================================

FUNCTION      DSRLPTXQ_ACCESS_FRAME

DESCRIPTION   Given a sequence number, this returns a pointer to the
              requested RLP frame.  The user can than access the internal
              contents of the RLP frame.  It is assumed that the following
              remains unchanged:
              
                RLP frame's sequence number
                The dsm item's seq_num field is unchanged
                
              The function is passed three important params:
              
                rscb_ptr    - pointer to the session control block

                fr_info_ptr - the seq_num field needs to contain the
                              requested sequence number.  All other fields
                              are ignored, and are filled in when the
                              appropriate RLP frame is found.
                              
                next_seq_num - filled in if a frame is found.  no input
                               value required.
                
              
DEPENDENCIES  Called from Tx task.  Also see conditions stated above.

RETURN VALUE  If the packet was found, then returns a non-zero value, 
              which indicates the length of the payload in the returned
              dsm packet chain (which contains the relevant RLP frame). 
              
              The passed fr_info_ptr is completely filled out with the info
              from the found dsm packet chain. The fr_info_ptr struct 
              contains a pointer to the head of the dsm pkt chain.
              
              next_seq_num is loaded with the seq_num of the next element
              in the txq.  If there is no next element, then next_seq_num
              is set equal to the requested sequence number.
              
              If the packet was not found, the returns zero.  The input
              fr_info_ptr is unchanged.  next_seq_num is loaded with the 
              seq num of the first (i.e., oldest) element in the array.
              
              If the array is in fact empty, then next_seq_num is unchanged.

SIDE EFFECTS  NONE
===========================================================================*/
word dsrlptxq_access_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,    /* Ptr to RLP ctl block       */
  dsrlptxq_fr_param_type       *fr_info_ptr, /* points to req'd frame info */
  uint32                       *next_seq_num /* filled with next seq num in*/
                                             /* list                       */
);


#endif /* FEATURE_DS */
#endif /* _DSRLPTXQ_H */

