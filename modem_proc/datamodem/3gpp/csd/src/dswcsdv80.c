/*===========================================================================

               W C D M A   C I R C U I T - S W I T C H E D   
                           D A T A  V 8 0


DESCRIPTION
  This file contains data and function definitions for the Synchronous
  Access Mode, Transparent sub-Mode of ITU-T V.80.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003 - 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:$
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dswcsdv80.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/11   TTV     Fixed an issue of leaving critical section correctly.
04/05/11   SS      Q6 free floating support.
09/29/09   SA      Fixed KW errors.
23/02/07   DA      Fixed high lint errors
02/09/06   ar      Lint corrections.
07/26/04   ar      Accommodate use of large DSM items from SIO.
05/30/04   ar      Lint corrections.
07/23/03   dwp     Remove dependency on DSM_DS_SMALL_ITEM_SIZ. Add some 
                   checks of dswcsdv80_data_fragment_size in data path. Fix
                   case where item becomes empty in UL. Cleanup comments.
04/15/03   dgy     Added support for UMTS CS Data handlers. Moved 
                   dswcsd_ota_speed_lookup() here.
02/12/03   dwp     Initial version.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)

#ifdef FEATURE_DATA_WCDMA_CS

#ifdef FEATURE_DATA_V80

#include "amssassert.h"
#include "msg.h"
#include "dsm.h"
#include "err.h"
#include "ds3gsiolib.h"
#include "ds3gsiolib_ex.h"
#include "dswcsdv80.h"
#include "memory.h"
#include "dswcsdi.h"
#include "dswcsdul.h"
#include "dswcsdmshif.h"
#include "data_msg.h"
#include "modem_mem.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Mode variable to know if R interface is in V.80 mode.
  i.e. +ES=6,,8 was issued.

  This variable will be set based on values read from ATCOP
  out of dsat_es_export_val[].
---------------------------------------------------------------------------*/

static boolean dswcsdv80_ul_r_iface_mode_is_v80  = FALSE;
static boolean dswcsdv80_dl_r_iface_mode_is_v80  = FALSE;

/*---------------------------------------------------------------------------
  Octet for recording the SYN1 flag, if unsued. Default it to 0xFF.

  This variable will be set based on values read from ATCOP
  out of dsat_esa_export_val[].
---------------------------------------------------------------------------*/

static uint8   dswcsdv80_syn1 = 0xFF;

/*---------------------------------------------------------------------------
  Pick an arbitrary size for sampling the bit stream. 

  In the DL, this needs to be at least 50% more then the DSM item size 
  passed by RLC.

  In the UL, this needs to be at least one octet more then the DSM item
  size passed by SIO.
---------------------------------------------------------------------------*/
#define DSWCSDV80_DATA_FRAGMENT_SIZE 1024


/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  V.80 Synchronous Access Mode In-Band Commands. Reference Table 9/V.80.

  General V.80 SAM commands.
---------------------------------------------------------------------------*/
/*
#define DSWCSD_V80_MARK_CMD      0xB0
#define DSWCSD_V80_FLAG_CMD      0xB1
#define DSWCSD_V80_ERR_CMD       0xB2
#define DSWCSD_V80_HUNT_CMD      0xB3
#define DSWCSD_V80_UNDER_CMD     0xB4
#define DSWCSD_V80_TOVER_CMD     0xB5
#define DSWCSD_V80_ROVER_CMD     0xB6
#define DSWCSD_V80_RESUME_CMD    0xB7
#define DSWCSD_V80_BNUM_CMD      0xB8
#define DSWCSD_V80_UNUM_CMD      0xB9
*/
/*---------------------------------------------------------------------------
  V.80 SAM Duplex Carrier Control commands
---------------------------------------------------------------------------*/
/*
#define DSWCSD_V80_EOT_CMD       0xBA
#define DSWCSD_V80_ECS_CMD       0xBB
#define DSWCSD_V80_RRN_CMD       0xBC
#define DSWCSD_V80_RTN_CMD       0xBD
#define DSWCSD_V80_RATE_CMD      0xBE
*/
/*---------------------------------------------------------------------------
  V.80 SAM Half Duplex Carrier Control commands
---------------------------------------------------------------------------*/
/*
#define DSWCSD_V80_PRI_CMD       0xBC
#define DSWCSD_V80_CTL_CMD       0xBF
#define DSWCSD_V80_RTNH_CMD      0xBD
#define DSWCSD_V80_RTNC_CMD      0xC0
#define DSWCSD_V80_RATEH_CMD     0xBE
#define DSWCSD_V80_EOTH_CMD      0xBA
*/
/*---------------------------------------------------------------------------
  V.80 over the air rates returned to the DTE after CONNECT
---------------------------------------------------------------------------*/
#define V80_RATE_640   0x31
/*
#define V80_RATE_560   0x30
#define V80_RATE_320   0x2F
#define V80_RATE_288   0x2C
#define V80_RATE_144   0x26
*/

/*---------------------------------------------------------------------------
  Flag for keeping track of the case where the last octet in a DSM item
  from the DTE is an <EM>. This is for proper processing of the next DSM
  item from the DTE that would contain the rest of the in-band command.
---------------------------------------------------------------------------*/
static boolean dswcsdv80_last_octet = FALSE;

/*---------------------------------------------------------------------------
  Look up table for mapping TCs to an index for use in another lookup.

  For single occuring TCs, the index is used in dswcsdv80_dl_escape_single[]

  For multiple occuring TCs, the indicies are used in
  dswcsdv80_dl_escape_doulbe[][].
---------------------------------------------------------------------------*/
static uint8 dswcsdv80_dl_escape_table[256] = {0};

/*---------------------------------------------------------------------------
  Look up table for single occuring TCs in the DL stream. It is indexed as
  shown below.

  dswcsdv80_dl_escape_single[EM7|EM8|DC1|DC3] = { 0x5C, 0x76, 0xA0, 0xA1 };

  Reference ITU-T V.80/Table 9 for the table contents.
---------------------------------------------------------------------------*/
static uint8 dswcsdv80_dl_escape_single[4] = { 0x5C, 0x76, 0xA0, 0xA1 };

/*---------------------------------------------------------------------------
  Look up table for pairs of TCs. There are 16 possible combos. The lookup
  is done dswcsdv80_dl_escape_double[next_octet][current_octet], and shown
  below.

  e.g. A <EM8><DC3> in the DL stream would be A9

   \ current
    \
next \  EM7  EM8  DC1  DC3
      \____________________
  EM7 | 5D | A7 | AA | AD | 
  EM8 | A4 | 77 | AB | AE | 
  DC1 | A5 | A8 | A2 | AF |
  DC3 | A6 | A9 | AC | A3 |
      ---------------------

  Reference ITU-T V.80/Table 9 for the table contents.
---------------------------------------------------------------------------*/
static uint8 dswcsdv80_dl_escape_double[4][4] = 
  { { 0x5D, 0xA7, 0xAA, 0xAD },
    { 0xA4, 0x77, 0xAB, 0xAE },
    { 0xA5, 0xA8, 0xA2, 0xAF },
    { 0xA6, 0xA9, 0xAC, 0xA3 } };

/*---------------------------------------------------------------------------
  V.80 rate table element
---------------------------------------------------------------------------*/
typedef struct
{
   ds_ucsd_data_rate_T  fnur;           /* Fixed network user rate         */
   uint8                v80_rate;       /* Wanted air interface user rate  */
} dswcsdv80_rate_element_T;

/*---------------------------------------------------------------------------
  Table used to convert the Fixed Network User Rate to V.80 Tx/Rx rate.
---------------------------------------------------------------------------*/
LOCAL const 
dswcsdv80_rate_element_T dswcsdv80_rate_table[] =
{
  /*
   FNUR                  V.80 Tx/Rx Rate
   ----                  ---------------
  */                                                                                                          
  {DS_UCSD_RATE_64000,   V80_RATE_640}
};

/*---------------------------------------------------------------------------
  Number of entries in the V.80 rate table
---------------------------------------------------------------------------*/
LOCAL const int dswcsdv80_rate_table_size = sizeof (dswcsdv80_rate_table) /
                                            sizeof (dswcsdv80_rate_element_T); 

/*---------------------------------------------------------------------------
  Scratch table for the uplink. Move to global to support USB. With no info
  on max size of a DSM, and current implementation, table must be larger
  then a large DSM item, plus one octet.
---------------------------------------------------------------------------*/
LOCAL uint8 dswcsdv80_from_dte_scratch[ DSWCSDV80_DATA_FRAGMENT_SIZE ];

/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSWCSD_OTA_SPEED_LOOKUP

DESCRIPTION
  V.80 needs to know the OTA rate so it can pass the information to the
  DTE. This is done after returning CONNECT, and should be done if there
  is an OTA rate re-negotiation.

DEPENDENCIES
  None

RETURN VALUE
  uint8; The Tx/Rx OTA rate as an octet. The octet values are defined
  in Admendment 1 of ITU-T V.80.

SIDE EFFECTS
  None

===========================================================================*/
uint8 dswcsd_ota_speed_lookup
(
  ds_ucsd_data_rate_T fnur                      /* Fixed Network User Rate */
)
{
  uint8 i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Find and return the V.80 OTA rate from the Rate Table.
  -------------------------------------------------------------------------*/
  for ( i = 0; i < dswcsdv80_rate_table_size; i++ )
  {
     if ( dswcsdv80_rate_table[ i ].fnur == fnur )
     {
        DATA_MSG1_HIGH( "Found V.80 OTA rate, 0x%x",
                  dswcsdv80_rate_table[ i ].v80_rate);

        return ( dswcsdv80_rate_table[ i ].v80_rate );
     }
  }

  DATA_MSG1_ERROR( "Cannot find V.80 OTA rate: FNUR=%d", fnur);

  return ( V80_RATE_640 );

} /* dswcsd_ota_speed_lookup() */


/*===========================================================================

FUNCTION DSWCSD_V80_SEND_CMD_TO_DTE

DESCRIPTION
  This function will build a command to send to the DTE. It will allocate
  the required DSM item, and send it to SIO.

  No command sent towards the DTE is longer then four octets.

  For now, this will only be used to send the over the air rate to
  the DTE after the CONNECT is returned.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  The V.80 command is inserted into the data stream towards the DTE.

===========================================================================*/
void dswcsdv80_send_cmd_to_dte
(
  dswcsdv80_cmd_enum_type cmd /* the command to be sent to the DTE */
)
{

  uint16                    i = 0;
  uint8                     rate;
  dsm_item_type             *new_item;
  ds3g_siolib_status_e_type sio_status;
  dswcsd_call_info_T dswcsd_call_info;
  ds3g_siolib_port_e_type  port_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memscpy(&dswcsd_call_info, sizeof(dswcsd_call_info_T),
           dswcsd_get_call_info_ptr(), sizeof(dswcsd_call_info_T));

  if ( (new_item = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)) == NULL)
  {
    DATA_MSG1_ERROR("no memory for V.80 CMD: small pool=%d",
        DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL)); /*lint !e10 !e26 */
    return;
  }

  /*-------------------------------------------------------------------------
    All commands will always be escaped/preceded w/<EM>.
  -------------------------------------------------------------------------*/
  new_item->data_ptr[i++] = DSWCSD_V80_EM7;

  /*-------------------------------------------------------------------------
    Build the appropriate command.
  -------------------------------------------------------------------------*/
  switch(cmd)
  {
    case DSWCSDV80_RATEH_CMD:

      new_item->data_ptr[i++] = (uint8)DSWCSDV80_RATEH_CMD;

      rate = dswcsd_ota_speed_lookup( dswcsd_call_info.fnur );
      new_item->data_ptr[i++] = rate;
      new_item->data_ptr[i++] = rate;

      DATA_MSG2_HIGH( "Send Tx=0x%x Rx=0x%x rate info to DTE", rate, rate);
      break;

    default:
      DATA_MSG0_ERROR("Bad V.80 cmd towards DTE");
      (void)dsm_free_buffer(new_item);
      return;

  } /* end switch */

  new_item->used = i;   

  port_id = ds3g_siolib_ex_get_csd_port();
  if ( ( sio_status = ds3g_siolib_ex_setup_tx( new_item, FALSE, port_id ) ) !=
    DS3G_SIOLIB_OK )
  {
    DATA_MSG1_ERROR( "SIO is not open: status=%d", sio_status);
  }

} /* dswcsd_v80_send_cmd_to_dte() */


/*===========================================================================

FUNCTION DSWCSDV80_DL_INBAND_ESCAPE

DESCRIPTION
  This function determines if we need to do any V.80 processing on the
  bitstream prior to giving the data to the DTE. If V.80 processing is 
  needed, it is done on the DSM item passed in. 
  
DEPENDENCIES
  None

PARAMETERS
  pkt_ptr - double pointer to DSM pkt chain

RETURN VALUE
  None

SIDE EFFECTS
  The pkt chain passed in may get larger. As individiaul DSM items are
  processed, they can become a pkt chain. These are inserted in place.

===========================================================================*/
void dswcsdv80_dl_inband_escape
(
  dsm_item_type **pkt_ptr
)
{
  uint8         c;
  int32         i=0;
  uint16        item_used = 0;
  uint16        s, size=0;
  boolean       process = FALSE, chained = FALSE;
  dsm_item_type *item_ptr, *rest_of_chain = NULL;
  uint8         curr_index, next_index = 0xFF;
  uint8         *dswcsdv80_to_dte_scratch = NULL;
#ifdef V80_DEBUG
  int32         pkt_cnt = 0;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dswcsdv80_to_dte_scratch = modem_mem_alloc((DSWCSDV80_DATA_FRAGMENT_SIZE * sizeof(uint8)), 
                                                  MODEM_MEM_CLIENT_DATA_CRIT);
  if(dswcsdv80_to_dte_scratch == NULL)
  {
    DATA_MSG0_ERROR("memory allocation failed");
    return;
  }

  memset((void *)dswcsdv80_to_dte_scratch, 0, 
               (DSWCSDV80_DATA_FRAGMENT_SIZE * sizeof(uint8)));

  if(pkt_ptr == NULL)
  {
    DATA_MSG0_ERROR("Ptr to DSM pkt chain is NULL" );
    return;
  }
  item_ptr = *pkt_ptr;

  /*-------------------------------------------------------------------------
    Walk the pkt chain checking each item for any V.80 Transparency 
    Characters.
  -------------------------------------------------------------------------*/

  while (item_ptr != NULL)
  {
  
    /*-----------------------------------------------------------------------
      There are four Transparency characters (TC) of interest:

      <EM> (7 bit) = 0x19 = DSWCSD_V80_EM7
      <EM> (8 bit) = 0x99 = DSWCSD_V80_EM8
      <DC1>        = 0x11 = DSWCSD_V80_DC1
      <DC3>        = 0x13 = DSWCSD_V80_DC3

      Scan the item looking for any of the four TCs.

      Single occuring, and any combo of two are to be handled.

      Only when one of these four occurs w/o being next to another of the
      four does the size of the item increase. So single occuring TCs
      generate two chars.
  
      However, the combos of two don't change the size of the item.
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      For now, don't do anything special if the last octet in the DSM item
      is a TC. Treat it as a single occuring TC. If the next octet in the
      next DSM item was a TC, we will have added an extra octet in each of
      the two DSM items. A small price to pay for the added complexity...
  
      Make a quick scan of the item for any TCs. Note i will be pointing at
      the first TC in the DSM item (should one be found).
    -----------------------------------------------------------------------*/

#ifdef V80_DEBUG
    pkt_cnt++;
#endif
    item_used = item_ptr->used;

    for (i=0; i<item_used; i++)
    {
      c = item_ptr->data_ptr[i];
  
      if ( c == DSWCSD_V80_EM7 || c == DSWCSD_V80_EM8 ||
           c == DSWCSD_V80_DC1 || c == DSWCSD_V80_DC3 )
      {
        #ifdef V80_DEBUG
          DATA_MSG1_ERROR("Found first TC 0x%x",c);
        #endif
        process = TRUE;
        break;
      }
    }

    /*-----------------------------------------------------------------------
      If process is TRUE, that means there is at least one TC. Otherwise,
      the DSM item is *clean*. If clean, get the next item in the chain.
      Otherwise, V.80 process this item. Insert the V.80 processed item
      into the passed pkt chain, and continue checking the rest of the
      pkt chain.
    -----------------------------------------------------------------------*/
    if(process == FALSE)
    {
      item_ptr = item_ptr->pkt_ptr; /* walk to next item in chain */
    }
    else
    {
      /*---------------------------------------------------------------------
        Make sure DSM item size is within expected limit. 
      ---------------------------------------------------------------------*/
      ASSERT(item_ptr->size < DSWCSDV80_DATA_FRAGMENT_SIZE);

      #ifdef V80_DEBUG
        DATA_MSG2_ERROR("pkt chain has TC in pkt_cnt=%d, byte=%d",pkt_cnt,i);
      #endif

      process = FALSE;

      /*---------------------------------------------------------------------
        First, copy the all the data up to where we saw the first TC in
        this item. Var i is pointing at the first TC in this item.
      ---------------------------------------------------------------------*/
      for(s=0;s<i;s++)
      {
        dswcsdv80_to_dte_scratch[s] = item_ptr->data_ptr[s];
      }

      /*---------------------------------------------------------------------
        Process this single item w/in the pkt chain. We will temporarily 
        remove any items chained to it before we process it. When done,
        we will re-chain the previous items. Care must be taken when 
        re-chaining previous items since V.80 processing may have caused
        this single item to become a chain.

        Then item_ptr will be adjusted to the next item that has not been
        processed yet so the existing loop can finish the pkt chain.
      ---------------------------------------------------------------------*/

      /*---------------------------------------------------------------------
        Are there more items after this one? If so, copy pointer to the rest 
        of the chain, and separate this item from them.
      ---------------------------------------------------------------------*/
      if(item_ptr->pkt_ptr != NULL)
      {
        rest_of_chain = item_ptr->pkt_ptr;
        item_ptr->pkt_ptr = NULL;
        #ifdef V80_DEBUG
          DATA_MSG0_ERROR("Not last item in chain");
        #endif
      }

      do
      {

        curr_index = dswcsdv80_dl_escape_table[(item_ptr->data_ptr[i])];

        if(curr_index != 0xFF)
        {

          #ifdef V80_DEBUG
            DATA_MSG1_ERROR("Found current TC=0x%x",item_ptr->data_ptr[i]);
          #endif

          /*-----------------------------------------------------------------
            If this TC is the last octet in the item, it 
            will be treated as a single TC.
          -----------------------------------------------------------------*/
          if(i+1 < item_ptr->used)
          {
            next_index = 
              dswcsdv80_dl_escape_table[(item_ptr->data_ptr[(i+1)])];
          }

          else
          {
            /*---------------------------------------------------------------
              The last octet in this DSM item is a TC. Set next_octet to
              0xFF so this TC is processed as a single occuring TC.
            ---------------------------------------------------------------*/
            next_index = 0xFF;
            #ifdef V80_DEBUG
              DATA_MSG2_ERROR("last octet is a TC, data_ptr[%d]=0x%x", i,
                         item_ptr->data_ptr[i+1]);
            #endif /* V80_DEBUG */
          }

          /*-----------------------------------------------------------------
            Copy in the <EM> flag first.
          -----------------------------------------------------------------*/
          dswcsdv80_to_dte_scratch[s++] = DSWCSD_V80_EM7;

          /*---------------------------------------------------------------------
            Make sure s is within expected limit. 
          ---------------------------------------------------------------------*/
          ASSERT(s < DSWCSDV80_DATA_FRAGMENT_SIZE);

          if((next_index != 0xFF) && (next_index < 4))
          {
            #ifdef V80_DEBUG
              DATA_MSG1_ERROR("Found double TC next=0x%x",item_ptr->data_ptr[i+1]);
            #endif

            dswcsdv80_to_dte_scratch[s++] = 
              dswcsdv80_dl_escape_double[next_index][curr_index];
    
            i += 2;
          }
          else 
          {
            dswcsdv80_to_dte_scratch[s++] = 
              dswcsdv80_dl_escape_single[curr_index];

            i++;
          }
        }
        else
        {
          /*-----------------------------------------------------------------
            Not a TC, copy the octet into dswcsdv80_to_dte_scratch[].
          -----------------------------------------------------------------*/
          dswcsdv80_to_dte_scratch[s++] = item_ptr->data_ptr[i++];
        }

      } while(i < item_ptr->used);

      /*---------------------------------------------------------------------
        s now has the total number of octets that have to be sent to the DTE.
    
        Reuse the item passed in. If needed, another item will be chained
        to it.
      ---------------------------------------------------------------------*/
      if(s <= item_ptr->size)
      {
        #ifdef V80_DEBUG
          DATA_MSG2_ERROR("DL: old item big enough s=%d, size=%d",s,
                                                   item_ptr->size);
        #endif
        /*-------------------------------------------------------------------
          The small item has enough space. Copy the scratch table into
          the passed in small item.  Reset the used field of the item,
          and clear the scratch table.
        -------------------------------------------------------------------*/
        memscpy(item_ptr->data_ptr, (uint32)s, dswcsdv80_to_dte_scratch, (uint32)s);
        item_ptr->used = s;
        chained = FALSE;
      }
      else
      {
        #ifdef V80_DEBUG
          DATA_MSG1_ERROR("Calling pushdown_tail w/size=%d",s);
        #endif
        /*-------------------------------------------------------------------
          The small item is no longer big enough. Reuse this item and have
          dsm_pushdown_tail append as needed (won't be more then one more).
          Set the used field of this item back to 0 so dsm_pushdown_tail will
          stomp over the previous data.
        -------------------------------------------------------------------*/
        item_ptr->used = 0;

        if( (size = dsm_pushdown_tail(&item_ptr,
                                      dswcsdv80_to_dte_scratch, s,
                                      DSM_DS_SMALL_ITEM_POOL) ) != s)
        {
          /*-----------------------------------------------------------------
            Failed to allocate needed DSM items. Release the DSM
            item being allocated.
          -----------------------------------------------------------------*/
          dsm_free_packet(pkt_ptr);
    
          pkt_ptr = NULL;

          DATA_MSG2_ERROR("Not enough memory for V80 to DTE: %d != %d", s, size);
        }
       
        chained = TRUE;

      } /* !(s <= item_ptr->size) */

      /*---------------------------------------------------------------------
        Clean out scratch table
      -------------------------------------------------------------*/
      memset(dswcsdv80_to_dte_scratch, 0, sizeof(uint8));

      /*---------------------------------------------------------------------
        If there was a TC in the middle of the pkt chain, re-attach
        the rest of the chain so it can be processed now. Note, the
        item processed could have become a pkt chain. It will never
        become more then two items though. Boolean chained is used
        for this.
      ---------------------------------------------------------------------*/
      if(rest_of_chain != NULL)
      {
        dsm_append(&item_ptr, &rest_of_chain);

        if(chained == TRUE)
        {
          chained = FALSE;
          item_ptr = item_ptr->pkt_ptr->pkt_ptr;
        }
        else
        {
          item_ptr = item_ptr->pkt_ptr;
        }

        rest_of_chain = NULL;
      }
      else
      {
        ASSERT(NULL != item_ptr)
        item_ptr = item_ptr->pkt_ptr;
      }
  
    } /* else process == TRUE */
  
  } /* end while (item_ptr != NULL) */

  modem_mem_free(dswcsdv80_to_dte_scratch, MODEM_MEM_CLIENT_DATA_CRIT);

} /* dswcsdv80_dl_inband_escape() */


/*===========================================================================

FUNCTION DSWCSD_V80_FROM_DTE

DESCRIPTION
  This routine looks for any <EM> flags in the item passed in. If none
  are found, nothing is done. If an <EM> is found, the item is processed
  completely.

DEPENDENCIES
  None
  
RETURN VALUE
  Boolean
    TRUE - if the single DSM item passed in became a pkt chain.
    FALSE - if the single DSM item passed in did not become a pkt chain.

SIDE EFFECTS
  Original data is modified if V.80 processing is required. Also, the single
  item passed in may become a pkt chain.

===========================================================================*/
boolean dswcsd_v80_from_dte
(
  dsm_item_type **item_ptr_passed
)
{
  uint16  i      = 0,      /* index for passed in DSM item */
          backup = 0,      /* var to account for prev item ending w/ <EM>  */
          s      = 0;      /* index into post V.80 process table */

  uint16  size = 0;

  boolean process = FALSE; /* does this item need V.80 processing */
  
  uint8   next_octet;      /* copy of octet after an <EM> */

  dsm_item_type *item_ptr;

  boolean item_became_chain = FALSE; /* did item passed in become a chain */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(item_ptr_passed == NULL)
  {
    DATA_MSG0_ERROR("NULL ptr into dswcsd_v80_from_dte()");
    return (item_became_chain);
  }
  ASSERT (NULL != *item_ptr_passed);
  
  item_ptr = *item_ptr_passed;

  if(item_ptr == NULL)
  {
    DATA_MSG0_HIGH("dswcsd_v80_from_dte() got NULL item");
    return (item_became_chain);
  }

  /*-------------------------------------------------------------------------
    If the last octet in the last DSM item processed was an <EM>,
    dswcsdv80_last_octet will be true. This allows proper processing
    of the next item.
  -------------------------------------------------------------------------*/
  if(dswcsdv80_last_octet == TRUE)
  {
    process = TRUE;
    backup = 1;
    #ifdef V80_DEBUG
      DATA_MSG0_ERROR("Last octet was <EM>");
    #endif
  }
  else
  {
    /*-----------------------------------------------------------------------
      Scan through this item to see if there is an <EM> flag. If there is,
      break out of the loop, and set process = TRUE. Loop counter i will
      be pointing at the <EM>.

      If there is no <EM> in this item, the item is untouched, and we 
      return out.
    -----------------------------------------------------------------------*/
    for (i=0; i<item_ptr->used; i++)
    {
      if(item_ptr->data_ptr[i] == DSWCSD_V80_EM7)
      {
        #ifdef V80_DEBUG
          DATA_MSG1_ERROR("Found first <EM> 0x%x",item_ptr->data_ptr[i]);
        #endif
        process = TRUE;
        break;
      }
    }
  }
  
  if(process == TRUE)
  {
    /*-----------------------------------------------------------------------
      Make sure DSM item size is within expected limit. 
    -----------------------------------------------------------------------*/
    ASSERT(item_ptr->size <= DSWCSDV80_DATA_FRAGMENT_SIZE);

    /*-----------------------------------------------------------------------
      There is at least one <EM> in this item, or the previous item ended
      with an <EM>. Take care of it now.

      First, copy all the data up to where we saw the first <EM>.
      i is pointning at the first <EM> found.

      If the previous item ended w/an <EM>, then no copy happens yet.
    -----------------------------------------------------------------------*/
    for(s=0;s<i;s++)
    {
      dswcsdv80_from_dte_scratch[s] = item_ptr->data_ptr[s];
    }

    /*-----------------------------------------------------------------------
      Now, process the rest of the item. Take care of any more <EM>s after
      the first one that caused us to come here.
    -----------------------------------------------------------------------*/
    do
    {

      if( (item_ptr->data_ptr[i] == DSWCSD_V80_EM7) || 
          (dswcsdv80_last_octet == TRUE) )
      {
        /*-------------------------------------------------------------------
          dswcsdv80_last_octet will only be true if the last octet 
          in the last DSM item processed was an <EM>. This sets up
          next_octet correctly.
        -------------------------------------------------------------------*/
        if(dswcsdv80_last_octet == TRUE)
        {
          next_octet = item_ptr->data_ptr[0];
          dswcsdv80_last_octet = FALSE;
        }
        else
        {
          if(i+1 < item_ptr->used)
          {
            next_octet = item_ptr->data_ptr[i+1];
          }
          else
          {
            /*---------------------------------------------------------------
              This is the case where the last octet in this DSM item
              is an <EM>. Set dswcsdv80_last_octet to TRUE so we can 
              account for this in the next DSM item. Break out of while
              loop since we're now done with this item.
            ---------------------------------------------------------------*/
            dswcsdv80_last_octet = TRUE;
            #ifdef V80_DEBUG
              DATA_MSG2_ERROR("last octet is EM used:%d == i:%d",
                                  item_ptr->used,i+1);
            #endif
            break;
          }
        }

        if( (next_octet >= 0xB0) && (next_octet <= 0xC0) )
        {
          /*-----------------------------------------------------------------
            In-band command. Strip it from the bit stream. i.e. Don't
            copy into dswcsdv80_from_dte_scratch. None are currently
            supported, so no action is taken.
          -----------------------------------------------------------*/

          /*-----------------------------------------------------------------
            The rate command is greater then two octets. Look for this
            special case.
          -----------------------------------------------------------------*/
          if( next_octet == (uint8)DSWCSDV80_RATEH_CMD )
          {
            i += 4-backup;

            #ifdef V80_DEBUG
              DATA_MSG2_ERROR("Found inband cmd 0x%x, rm %d octets",
                         next_octet,(4-backup));
            #endif
          }
          else
          {
            i += 2-backup;

            #ifdef V80_DEBUG
              DATA_MSG2_ERROR("Found inband cmd 0x%x, rm %d octets",
                         next_octet,(2-backup));
            #endif
          }

        }
        else
        {
          /*-----------------------------------------------------------------
            Transparency Character (TC) command. Perform action as
            requested per Table 9/V.80.
          -----------------------------------------------------------------*/
          ASSERT( DSWCSDV80_DATA_FRAGMENT_SIZE > (s +1));
          switch(next_octet)
          {
            case 0x5C:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM7;
              break;
            case 0x76:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM8;
              break;
            case 0xA0:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC1;
              break;
            case 0xA1:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC3;
              break;
            case 0x5D:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM7;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM7;
              break;
            case 0x77:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM8;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM8;
              break;
            case 0xA2:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC1;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC1;
              break;
            case 0xA3:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC3;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC3;
              break;
            case 0xA4:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM7;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM8;
              break;
            case 0xA5:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM7;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC1;
              break;
            case 0xA6:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM7;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC3;
              break;
            case 0xA7:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM8;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM7;
              break;
            case 0xA8:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM8;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC1;
              break;
            case 0xA9:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM8;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC3;
              break;
            case 0xAA:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC1;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM7;
              break;
            case 0xAB:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC1;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM8;
              break;
            case 0xAC:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC1;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC3;
              break;
            case 0xAD:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC3;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM7;
              break;
            case 0xAE:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC3;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_EM8;
              break;
            case 0xAF:
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC3;
              dswcsdv80_from_dte_scratch[s++] = DSWCSD_V80_DC1;
              break;
            default:
             /*--------------------------------------------------------------
               This would be the case where we get a solo <EM>. There
               should not be any in the UL stream, but ...

               Our policy will be to remove the <EM> and the next 
               octet from the stream. So don't copy into 
               dswcsdv80_from_dte_scratch, and let i iterate as usual.
             -------------------------------------------------------*/
              DATA_MSG0_ERROR("Single <EM>, remove it and next octet");
              break;
           
          } /* end switch(next_octet) */

          i += 2-backup;

          #ifdef V80_DEBUG
            if(next_octet == 0x5C || next_octet == 0x76 ||
               next_octet == 0xA0 || next_octet == 0xA1)
            {
              DATA_MSG2_ERROR("Found TC 0x%x, insert 0x%x",
                      next_octet, dswcsdv80_from_dte_scratch[(s-1)]);
            }
            else
            {
              MSG_ERROR("Found TC 0x%x, insert 0x%x 0x%x",
                        next_octet, 
                        dswcsdv80_from_dte_scratch[(s-2)], 
                        dswcsdv80_from_dte_scratch[(s-1)]);
            }
          #endif

        }
        /*-------------------------------------------------------------------
          The variable backup is at most used once per DSM item.
          Only when the last octet, in the last DSM item, was <EM>.

          It gets used for the next DSM item where the completion 
          of the preceding <EM> occurs.

          However, once the fragmented <EM><?> is processed, backup 
          must be set back to 0. If not, any other <EM><?> in this 
          item would not be processed correctly.
        -------------------------------------------------------------------*/
        backup = 0;

      } /* if(item_ptr->data_ptr[i] == DSWCSD_V80_EM7 || last_octet == TRUE*/
      else
      {
        /*-------------------------------------------------------------------
          Not <EM>, just copy over.
        -----------------------------------------------------------*/
        ASSERT( DSWCSDV80_DATA_FRAGMENT_SIZE > s );
        dswcsdv80_from_dte_scratch[s++] = item_ptr->data_ptr[i++];
      }

    } while (i < item_ptr->used);

    /*-----------------------------------------------------------------------
      In general, the V.80 UL data stream can only get smaller or stay 
      the same size. 

      However, on a per DSM item basis, a single item can get larger. The
      one case when this can happens is:
  
      1.) A V.80 insert Transparency Character command gets split across 
          two DSM items, and this cmd is for inserting two octets.
      2.) The second DSM item containing the insert Transparency Character
          command is full of data that will remain after V.80 processing.
          (Could also be more insert double TC cmds...)
  
      Only when the above two items are true, will dswcsdv80_from_dte_scratch[] 
      be larger then the DSM item (by one octet).

      Split V.80 commands that are not for inserting a Transparency Char
      will not cause dswcsdv80_from_dte_scratch[] to get larger then the 
      item size.

      Write the V.80 processed data back into the item. If the item became
      larger, use dsm_pushdown_tail to chain on the extra item. Otherwise
      we stomp in the V.80 processed data into the current item.

      Update the used field as appropriate for each case.
    -----------------------------------------------------------------------*/
    if(s>item_ptr->size)
    {

      #ifdef V80_DEBUG
        DATA_MSG2_ERROR("UL data to big for DSM item %d>%d",s,item_ptr->size);
      #endif

      item_ptr->used = 0;

      if( (size = dsm_pushdown_tail(&item_ptr,
                                    dswcsdv80_from_dte_scratch, s,
                                    DSM_DS_SMALL_ITEM_POOL) ) != s)
      {
        /*-------------------------------------------------------------------
          Failed to allocate needed DSM items. Release the DSM
          item being allocated.
        -------------------------------------------------------------------*/
        dsm_free_packet(item_ptr_passed);

        item_ptr_passed = NULL;

        DATA_MSG2_ERROR("Not enough memory for V80 from DTE: %d != %d", s, size);
      }
      else
      {
        *item_ptr_passed = item_ptr;
        item_became_chain = TRUE;
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Either the data still fits into the DSM item or there is nothing
        left to send.
      ---------------------------------------------------------------------*/
      if(s == 0)
      {
        /*-------------------------------------------------------------------
          In the UL, it's possible a DSM item becomes empty after V.80
          processing. In this case, free the item, set the pointer to the
          item to NULL, so the calling environment knows the item became
          empty and was free'd.
        -------------------------------------------------------------------*/
        #ifdef V80_DEBUG
          DATA_MSG0_ERROR("Item became empty dswcsd_v80_from_dte()"); 
        #endif
        (void)dsm_free_buffer(item_ptr);
        *item_ptr_passed = NULL;
        item_ptr_passed = NULL;
      }
      else
      {
        /*-------------------------------------------------------------------
          There still is data to send, and it fits with in the original 
          DSM item. Copy the V.80 processed data back into the original
          item and update the used field.
        -------------------------------------------------------------------*/
        memscpy(item_ptr->data_ptr, s, dswcsdv80_from_dte_scratch, s);
        item_ptr->used = s;
        *item_ptr_passed = item_ptr;
      }
    }

  } /* if(process == TRUE) */

  return (item_became_chain);

} /* dswcsd_v80_from_dte() */


/*===========================================================================

FUNCTION DSWCSDV80_BUILD_UL_SDU_WITH_FLAG

DESCRIPTION
  When there is not enough data from the DTE, this function will 
  generate the idle frames to send to the remote end. All data from the DTE
  is used first, then the rest of the SDUs/Frames will be padded with the
  SYN1 flag.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to an SDU.

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *dswcsdv80_build_ul_sdu_with_flag(void)
{
  uint16 sdu_byte_cnt = 0;    /* current # bytes built into current SDU */
  uint16 alloc_size;            /* Number of bytes pushed into an DSM item */
  uint16 octets_left_for_sdu;      /* Number of SIO data bytes to retrieve */
  uint8  *sio_data_buf = NULL;   /* Buff for SIO data */
  dsm_item_type        *sdu_ptr = NULL;                     /* SDU pointer */
  boolean item_became_chain = FALSE; /* V.80 processed item became a chain */
  dsm_item_type *dswcsd_sio_dsm_item_ptr = NULL;
  dsm_item_type *temp_item_ptr = NULL;
  uint16        dswcsd_dsm_payload_size = 0;
  uint16        dswcsd_from_sio_q_cnt   = 0;
  dswcsd_call_info_T dswcsd_call_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memscpy(&dswcsd_call_info, sizeof(dswcsd_call_info_T),
           dswcsd_get_call_info_ptr(), sizeof(dswcsd_call_info_T));

  dswcsd_sio_dsm_item_ptr = dswcsd_get_sio_dsm_item_ptr();
  dswcsd_dsm_payload_size = dswcsd_get_dsm_payload_size();
  dswcsd_from_sio_q_cnt   = dswcsd_get_from_sio_q_cnt();

  sio_data_buf =  modem_mem_alloc((DSWCSDV80_DATA_FRAGMENT_SIZE * sizeof(uint8)), 
                                     MODEM_MEM_CLIENT_DATA_CRIT);
  if(sio_data_buf == NULL)
  {
    DATA_MSG0_ERROR("memory allocation failed");
    return NULL;
  }

   memset((void *)sio_data_buf, 0, 
           (DSWCSDV80_DATA_FRAGMENT_SIZE * sizeof(char)));

  /*-------------------------------------------------------------------------
    Build each SDU
  -------------------------------------------------------------------------*/
  while ( sdu_byte_cnt < dswcsd_call_info.bytes_per_sdu )
  {
    /*-----------------------------------------------------------------------
      Take account of unfinished DSM item from the previous SDU.
    -----------------------------------------------------------------------*/

    if ( dswcsd_sio_dsm_item_ptr == NULL )
    {
      /*---------------------------------------------------------------------
        There is no unfinished DSM item, retrieve queued SIO data.
      ---------------------------------------------------------------------*/
      dswcsd_sio_dsm_item_ptr = (dsm_item_type *) 
                                 q_get( dswcsd_get_from_sio_q_ptr() );
      dswcsd_set_sio_dsm_item_ptr((dsm_item_type *) 
                                  q_get( dswcsd_get_from_sio_q_ptr() ));

      /*---------------------------------------------------------------------
        See if there was some data for the SDU. If so, use it,
        otherwise, all will be SYN1.
      ---------------------------------------------------------------------*/
      if(dswcsd_sio_dsm_item_ptr != NULL)
      {
        #ifdef V80_DEBUG
          DATA_MSG0_ERROR("Data from SIO for SDU");
        #endif

        /*-------------------------------------------------------------------
          Decrease the count for queued SIO data.
        -------------------------------------------------------------------*/
        dswcsd_from_sio_q_cnt -= dswcsd_sio_dsm_item_ptr->used;
        dswcsd_update_from_sio_q_cnt(dswcsd_from_sio_q_cnt);

        /*-------------------------------------------------------------------
          There is some data. Do V.80 preprocessing. Note if the DSM 
          item became a chain or not.
        -------------------------------------------------------------------*/
        item_became_chain = dswcsd_v80_from_dte(&dswcsd_sio_dsm_item_ptr);

        if(dswcsd_sio_dsm_item_ptr == NULL)
        {
          /*-----------------------------------------------------------------
            In the UL, it's possible a DSM item becomes empty after V.80
            processing. In this case, dswcsd_v80_from_dte() will free the
            item and dswcsd_sio_dsm_item_ptr will be NULL. When this 
            happens, go back to the top of the loop to get a new DSM item 
            to process.
          -----------------------------------------------------------------*/
          #ifdef V80_DEBUG
            DATA_MSG0_ERROR("Item became empty build w/flag");
          #endif
          continue;
        }

      }
      else
      {
        /*-------------------------------------------------------------------
          There wasn't enough data for a TTI. Fill rest/entire SDU 
          with SYN1.
        -------------------------------------------------------------------*/

        if ( (temp_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL)) == NULL)
        {
          DATA_MSG2_ERROR("out of memory: small=%d large=%d",
              DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL),
              DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL)); /*lint !e10 !e26*/
          dsm_free_packet(&sdu_ptr);
          modem_mem_free(sio_data_buf, MODEM_MEM_CLIENT_DATA_CRIT);
          return NULL;
        }
        else
        {
          dswcsd_sio_dsm_item_ptr = temp_item_ptr;
          dswcsd_set_sio_dsm_item_ptr(temp_item_ptr);
        }

        /*-------------------------------------------------------------------
          Fill the item w/SYN1
        -------------------------------------------------------------------*/
        memset ( dswcsd_sio_dsm_item_ptr->data_ptr, 
                 dswcsdv80_syn1,
                 dswcsd_sio_dsm_item_ptr->size );

        dswcsd_sio_dsm_item_ptr->used = dswcsd_sio_dsm_item_ptr->size;

      } /* else no data in dswcsd_from_sio_q */

      /*---------------------------------------------------------------------
        Get the size of the DSM item payload.
      ---------------------------------------------------------------------*/
      dswcsd_dsm_payload_size = dswcsd_sio_dsm_item_ptr->used;
      dswcsd_set_dsm_payload_size(dswcsd_dsm_payload_size);

    } /* if ( dswcsd_sio_dsm_item_ptr == NULL ) */

    /*-----------------------------------------------------------------------
      Assemble UL SDU: compute how many bytes to pull out of 
      the DSM item, pull the bytes out of the DSM item, and 
      insert the bytes into the SDU.
    -----------------------------------------------------------------------*/

    if ( ( sdu_byte_cnt + dswcsd_dsm_payload_size ) >
         dswcsd_call_info.bytes_per_sdu )
    {
      octets_left_for_sdu = dswcsd_call_info.bytes_per_sdu - sdu_byte_cnt;
    }
    else
    {
      octets_left_for_sdu = dswcsd_dsm_payload_size;
    }

    (void)dsm_pullup( &dswcsd_sio_dsm_item_ptr, sio_data_buf, octets_left_for_sdu );

    dswcsd_dsm_payload_size -= octets_left_for_sdu;
    dswcsd_set_dsm_payload_size(dswcsd_dsm_payload_size);

    if ( ( alloc_size = dsm_pushdown_tail( &sdu_ptr,
                                           sio_data_buf,
                                           octets_left_for_sdu,
                                           DSM_DS_SMALL_ITEM_POOL ) ) !=
         octets_left_for_sdu)
    {
      /*---------------------------------------------------------------------
        Failed to allocate needed DSM items. Release the DSM
        item being allocated and set the SDU pointer to NULL.
      ---------------------------------------------------------------------*/

      dsm_free_packet(&sdu_ptr);

      sdu_ptr = NULL;

      DATA_MSG0_ERROR("Failed to obtain enough memory");
    }
    else
    {
      /*---------------------------------------------------------------------
        Successfully inserted data into the SDU. Update the SDU 
        byte count and the app_field.
      ---------------------------------------------------------------------*/

      sdu_byte_cnt += octets_left_for_sdu;
      ASSERT (NULL != sdu_ptr );
      sdu_ptr->app_field += ( alloc_size * NUM_BITS_PER_BYTE );
    }

    /*-----------------------------------------------------------------------
      When we have finished copying this DSM item into an SDU,
      see if this item was chained. This will only be if there was
      some data from SIO. V.80 can make a single item become at most 
      two. If it had been chained, dswcsd_sio_dsm_item_ptr is 
      pointing at the second item. Now make dswcsd_dsm_payload_size 
      correspond to the same item.

      Otherwise, the item is empty, so set dswcsd_sio_dsm_item_ptr
      back to NULL.
    -----------------------------------------------------------------------*/
    if ( dswcsd_dsm_payload_size == 0 )
    {
      if(item_became_chain == TRUE)
      {
        item_became_chain = FALSE;
        dswcsd_dsm_payload_size = dswcsd_sio_dsm_item_ptr->used;
        dswcsd_set_dsm_payload_size(dswcsd_dsm_payload_size);
        #ifdef V80_DEBUG
          DATA_MSG1_ERROR("V.80 chained the item in UL, pkt_ptr->size=%d",
                     dswcsd_dsm_payload_size);
        #endif
      }
      else
      {
        dswcsd_set_sio_dsm_item_ptr(NULL);
      }
    }

  } /* end while loop */
  modem_mem_free(sio_data_buf, MODEM_MEM_CLIENT_DATA_CRIT);
  return (sdu_ptr);

} /* dswcsdv80_build_ul_sdu_with_flag() */


/*===========================================================================

FUNCTION DSWCSDV80_INIT

DESCRIPTION
  Populates the lookup table used in the donw link for V.80
  Transparency Character (TC) escaping. There are four octets that
  have to be escaped. Every octet must be checked for these four.

  This table is used instead of doing if's against the four possible 
  octets. The four octet values contain the index into two other tables.

  These tables are:
  dswcsdv80_dl_escape_single[] is used for single occuring TCs.
  dswcsdv80_dl_escape_double[][] is used for pairs of TCs.

  More details are given at the table definitions.

DEPENDENCIES
  None

RETURN VALUE
  None to an SDU.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsdv80_init()
{

  /*-------------------------------------------------------------------------
    Fill the table /w 0xFF
  -------------------------------------------------------------------------*/
  memset ( dswcsdv80_dl_escape_table, 0xFF, 256 );

  /*-------------------------------------------------------------------------
    EM7 is index 0
  -------------------------------------------------------------------------*/
  dswcsdv80_dl_escape_table[DSWCSD_V80_EM7] = 0;

  /*-------------------------------------------------------------------------
    EM8 is index 1
  -------------------------------------------------------------------------*/
  dswcsdv80_dl_escape_table[DSWCSD_V80_EM8] = 1;

  /*-------------------------------------------------------------------------
    DC1 is index 2
  -------------------------------------------------------------------------*/
  dswcsdv80_dl_escape_table[DSWCSD_V80_DC1] = 2;

  /*-------------------------------------------------------------------------
    DC3 is index 3
  -------------------------------------------------------------------------*/
  dswcsdv80_dl_escape_table[DSWCSD_V80_DC3] = 3;

}

/*===========================================================================

FUNCTION DSWCSDV80_GET_UL_R_IFACE_MODE_IS_V80

DESCRIPTION
  Returns the value of dswcsdv80_ul_r_iface_mode_is_v80

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean dswcsdv80_get_ul_r_iface_mode_is_v80(void)
{
  return dswcsdv80_ul_r_iface_mode_is_v80;
}

/*===========================================================================

FUNCTION DSWCSDV80_SET_UL_R_IFACE_MODE_IS_V80

DESCRIPTION
  Sets the value of dswcsdv80_ul_r_iface_mode_is_v80

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsdv80_set_ul_r_iface_mode_is_v80(boolean value)
{
  dswcsdv80_ul_r_iface_mode_is_v80 = value;
}

/*===========================================================================

FUNCTION DSWCSDV80_GET_DL_R_IFACE_MODE_IS_V80

DESCRIPTION
  Returns the value of dswcsdv80_dl_r_iface_mode_is_v80

DEPENDENCIES
  None

RETURN VALUE
  boolean.

SIDE EFFECTS
  None

===========================================================================*/
boolean dswcsdv80_get_dl_r_iface_mode_is_v80(void)
{
  return dswcsdv80_dl_r_iface_mode_is_v80;
}

/*===========================================================================

FUNCTION DSWCSDV80_SET_DL_R_IFACE_MODE_IS_V80

DESCRIPTION
  Sets the value of dswcsdv80_dl_r_iface_mode_is_v80

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsdv80_set_dl_r_iface_mode_is_v80(boolean value)
{
  dswcsdv80_dl_r_iface_mode_is_v80 = value;
}

/*===========================================================================

FUNCTION DSWCSDV80_SET_SYN1

DESCRIPTION
  Sets the value of dswcsdv80_syn1

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsdv80_set_syn1(uint8 value)
{
  dswcsdv80_syn1 = value;
}

#endif /* FEATURE_DATA_V80 */

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA) */

