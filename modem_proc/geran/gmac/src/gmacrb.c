/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC Radio Block handler
***
***
*** DESCRIPTION
***
***  For uplink control blocks MAC performs no segmentation. The sole function for
***  MAC in the uplink direction is to append the MAC header to the logical channel
***  message being sent to the network.
***
***  In the DL direction the control block handles has to administrate T3200,
***  Re-assembly segmented DL control messages, Removal and decoding of the bit
***  fields of the MAC headers and Organize up to 8 partially received DL control
***  messages (depending on the mode MAC is in).
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
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacrb.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 07/05/01   tlx     Created
***
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#include "grlcx.h"
#include "gmacdef.h"
#include "gmacrb.h"
#include "gmacutil.h"
#include "gmaclog.h"
#include "gmacproc.h"
#include "gmacds.h"
#include "stringl/stringl.h"
/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/
#define NUM_BITS_TO_BYTES(x) ((x / 8) + (( x % 8 > 0) ? 1 : 0))

#ifdef FEATURE_GSM_EDTM
#define GMAC_EXTENDED_PCA_NOT_VALID  0xffff
#endif /*FEATURE_GSM_EDTM*/

/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/

  /* used to manage link list of segmented DL control messages*/
typedef enum
{
  GMAC_DL_MSG_LIST_EMPTY,
  GMAC_DL_MSG_LIST_NOT_FULL,
  GMAC_DL_MSG_LIST_FULL
} dl_msg_list_t;

typedef enum
{
  GMAC_CTRL_ACK_VALUE_0,
  GMAC_CTRL_ACK_VALUE_1,
  GMAC_CTRL_ACK_VALUE_2,
  GMAC_CTRL_ACK_VALUE_3,
  GMAC_CTRL_ACK_NO_CLASH
} ctrl_ack_value_t;

/* 1st optional MAC header octet definition */
typedef struct
{
  
  /* Reduced block sequence number */
  uint8 rbsn;
  #define mac_hdr_1st_opt_t_rbsn_len  1
  
  /* Radio transaction indentifier */
  uint8 rti;
  #define mac_hdr_1st_opt_t_rti_len   5
  
  /* Final segment */
  uint8 fs;
  #define mac_hdr_1st_opt_t_fs_len    1
  
  /* Address control */
  uint8 ac;
  #define mac_hdr_1st_opt_t_ac_len    1
  
} mac_hdr_1st_opt_t;


/* 2nd optional MAC header octet definition */
typedef struct
{
  
  /* Power reduction */
  uint8 pr;
  #define mac_hdr_2nd_opt_t_pr_len    2
  
  /* temporary field indentity */
  uint8 tfi;
  #define mac_hdr_2nd_opt_t_tfi_len   5
  
  /* Direction */
  uint8 dir;
  #define mac_hdr_2nd_opt_t_dir_len   1
  
} mac_hdr_2nd_opt_t;


#ifdef FEATURE_GSM_EDTM
/* extended segmented optional MAC header octet definition */
typedef struct
{
  
  /* Reduced block sequence number extended */
  uint8 rbsne;
  #define mac_hdr_ext_seg_opt_t_rbsne_len    3
  
  /* final segment eextended */
  uint8 fse;
  #define mac_hdr_ext_seg_opt_t_fse_len   1
    
} mac_hdr_ext_seg_opt_t;
#endif /*FEATURE_GSM_EDTM*/

/****************************************************************************
*  structure used to  holds segment DL control msg data                     *
*  note the "msg_segment" array contains the MAC header + the data of the   *
*  RLC/MAC dl control block. when the 2 sections of a segmented Dl control  *
*  msg are to be joined the header  part has to be removed first before the *
*  sections can be joined                                                   *
*****************************************************************************/
#ifdef FEATURE_GSM_EDTM

#define MAX_SEGMENTS_PER_MSG 9

struct dl_msg_buffer
{
  uint8   msg_buffer_rti;
  uint8   timer_id;
  uint8   ts_received_on;
  uint8   msg_ext_segment[MAX_SEGMENTS_PER_MSG][SIZE_OF_CONTROL_RADIO_BLOCK];
  
  uint8   num_header_octs[MAX_SEGMENTS_PER_MSG]; /*header octets for each segment*/                
  uint16  ext_seg_rcvd_bit_map;                  /*only bits 0 to 8 used 0 = 1st seg. 8 = 9th seg*/  
  uint8   final_segment_rbsn;                    /*stores the block sequence number of the Final segment*/
  
  struct  dl_msg_buffer *ptr_to_next_buff;
};

#else

struct dl_msg_buffer
{
  uint8   msg_buffer_rti;
  uint8   no_header_octets ;
  boolean first_msg_segment;
  uint8   timer_id;
  uint8   ts_received_on;
  uint8   msg_segment[SIZE_OF_CONTROL_RADIO_BLOCK];
  struct  dl_msg_buffer *ptr_to_next_buff;
};

#endif /*FEATURE_GSM_EDTM*/

typedef struct dl_msg_buffer dl_msg_buffer_t;

typedef struct
{
  uint32            absolute_rrbp_fn;
  uint8             time_slot;
  ctrl_ack_value_t  control_ack_value;
} last_segmented_rrbp_info_t ;

typedef struct
{
  dl_msg_list_t list_full;

  dl_msg_buffer_t  first_in_list;
  dl_msg_buffer_t *ptr_to_current_in_list;
  dl_msg_buffer_t *ptr_to_previous_in_list;
  dl_msg_buffer_t *ptr_last_in_list;

  /****************************************************************************
  * array indicates the status of the currently  active                       *
  * T3200 associated with segmented DL control Msg                            *
  *****************************************************************************/
  uint8 t3200_active[8];

  /****************************************************************************
  * Holds 1'st optional octet of DL RLC/MAC control block                     *
  ****************************************************************************/
  mac_hdr_1st_opt_t mac_1st_opt_oct;

  /****************************************************************************
  * holds extracted mac header bit feilds                                     *
  ****************************************************************************/
  mac_hdr_t mac_header;
  mac_hdr_t *dl_mac_header_prt;

  /*****************************************************************************
  * stores rrbp_fn,ts and control ack value for the last 3 segmented rrbp      *
  *****************************************************************************/
  last_segmented_rrbp_info_t last_segmented_rrbp_info[3];

  /*var made global to facility logging functionality (particularly logging of PUAN)*/
  /*mainly used for logging of over the air msg rcvd by MAC always initialise before use*/
#ifdef FEATURE_GSM_EDTM
  /*store the rcvd extended segmented msg bit, used in PCA if the segmented msg was polled*/
  uint16 gmac_pca_for_ext_seg_msg;
#endif /*FEATURE_GSM_EDTM*/

  uint8 rrbp_index;
} gmac_rb_data_t;

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/
gmac_rb_data_t gmac_rb_data[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/

static ctrl_ack_value_t gmac_check_for_rrbp_clash(uint32 absolute_rrbp_fn, uint8 ts, gas_id_t gas_id);
static void xlate_mac_hdr_1st_optional_octet(uint8 *src, mac_hdr_1st_opt_t *mac_1st_opt_oct_ptr, gas_id_t gas_id);
static uint8 retrieve_dl_logical_channel_msg(l1_mac_sig_t *msg_ptr, gas_id_t gas_id);
static uint8 set_t3200(gas_id_t gas_id);
static void clear_t3200(uint8 idle_timer_id, gas_id_t gas_id);

#ifdef FEATURE_GSM_EDTM
static void xlate_mac_hdr_ext_seg_optional_octet(uint8 *src, mac_hdr_ext_seg_opt_t *mac_ext_seg_opt_oct_ptr, gas_id_t gas_id);
static boolean gmac_segmented_msg_complete(uint16 current_rcvd_seg_bit_map, uint8  rbsne_of_last_seg, gas_id_t gas_id);
static uint8 gmac_retrieve_completed_segmented_msg(uint8 *ptr_completed_dl_msg,  dl_msg_buffer_t *ptr_current_seg_msg, gas_id_t gas_id);
static void gmac_update_segmented_vars(mac_ph_data_ind_t *ptr_mac_ph_data, dl_msg_buffer_t *ptr_current_seg_msg, gas_id_t gas_id);
static void gmac_initialise_segmented_vars(dl_msg_buffer_t *ptr_current_seg_msg,uint8 ts,uint8 rti, gas_id_t gas_id);
#else
static void xlate_mac_hdr_2nd_optional_octet(uint8 *src, mac_hdr_2nd_opt_t *mac_2nd_opt_oct_ptr, gas_id_t gas_id);
#endif /*FEATURE_GSM_EDTM*/
static gmac_rb_data_t* gmac_get_rb_data_ptr(gas_id_t gas_id);

/*****************************************************************************
***
***     Public Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

/* logging-on-demand stuff */
uint8 dl_channel_msg_to_channel_type(gas_id_t gas_id);
void gmac_log_dl_channel_msg(uint8 msg_type, uint16 msg_length,gas_id_t gas_id);


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    gmac_get_rb_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to null data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_rb_data_t*
=== 
===  SIDE EFFECTS
===
===
===========================================================================*/
static gmac_rb_data_t* gmac_get_rb_data_ptr(gas_id_t gas_id)
{
  gmac_rb_data_t *rb_data_ptr = &gmac_rb_data[0];

  /* validate the GAS ID */
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    rb_data_ptr = &gmac_rb_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)];
  }
  else
  {    
    MSG_GERAN_ERROR_1_G("RB data bad gas_id:%d",gas_id);
  }

  return rb_data_ptr;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_rb_data()
===
===  DESCRIPTION
===  Initialises rb data.  Used for Lower layer resets.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_rb_data( gas_id_t gas_id )
{
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);

  gmac_init_rb_data_non_zero(gas_id);

  memset( &rb_data_ptr->first_in_list, 0, sizeof(rb_data_ptr->first_in_list) );
  memset( rb_data_ptr->last_segmented_rrbp_info, 0, sizeof(rb_data_ptr->last_segmented_rrbp_info) );
}

/*===========================================================================
===
===  FUNCTION    gmac_init_rb_data_non_zero()
===
===  DESCRIPTION
===  Initialises non zero (including all enums) rb data.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_rb_data_non_zero( gas_id_t gas_id )
{
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);

  rb_data_ptr->list_full = GMAC_DL_MSG_LIST_EMPTY;
  rb_data_ptr->dl_mac_header_prt = &rb_data_ptr->mac_header;

#ifdef FEATURE_GSM_EDTM
  rb_data_ptr->gmac_pca_for_ext_seg_msg = GMAC_EXTENDED_PCA_NOT_VALID;
#endif /*FEATURE_GSM_EDTM*/
}

/*===========================================================================
===
===  FUNCTION      gmac_check_for_rrbp_clash()
===
===  DESCRIPTION
===
===    if a clash is found then the control ack value used for the segmented
===    dl msg is returned. This will be the value to be sent in PCA.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
static ctrl_ack_value_t gmac_check_for_rrbp_clash(uint32 absolute_rrbp_fn, uint8 ts, gas_id_t gas_id)
{
  uint8 i;
  ctrl_ack_value_t ctrl_ack_value = GMAC_CTRL_ACK_NO_CLASH;
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);

  for (i=0;i<2;i++)
  {
    if((rb_data_ptr->last_segmented_rrbp_info[i].absolute_rrbp_fn == absolute_rrbp_fn) &&
       (rb_data_ptr->last_segmented_rrbp_info[i].time_slot == ts))
    {
      ctrl_ack_value = rb_data_ptr->last_segmented_rrbp_info[i].control_ack_value;
    }
  }
  return(ctrl_ack_value);
}


#ifdef FEATURE_GSM_EDTM
/*===========================================================================
===
===  FUNCTION      xlate_mac_hdr_ext_seg_optional_octet
===
===  DESCRIPTION
===  Translate the extended segmentation optional octet mac header from external
===  format into internal format.
===
===  Note the function gunpackb() unpacks an octet starts with the MSB .
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void xlate_mac_hdr_ext_seg_optional_octet
(
uint8 *src,                                     /* source message in 'over-the-air' format */
mac_hdr_ext_seg_opt_t *mac_ext_seg_opt_oct_ptr,  /* destination internal format */
gas_id_t gas_id
)
{
  /* offset from the beginning of the message to the current item */
  uint16 msg_pos = 0;

  /* extract bit fields of extended segment optional  octet */
  mac_ext_seg_opt_oct_ptr->rbsne = gunpackb(src, &msg_pos, GSIZE(mac_hdr_ext_seg_opt_t, rbsne));
  mac_ext_seg_opt_oct_ptr->fse = gunpackb(src, &msg_pos, GSIZE(mac_hdr_ext_seg_opt_t, fse));
}

#else /*FEATURE_GSM_EDTM*/
/*===========================================================================
===
===  FUNCTION      xlate_mac_hdr_2nd_optional_octet
===
===  DESCRIPTION
===  Translate the 2'nd' optional octet mac header from external
===  format into internal format.
===
===  Note the function gunpackb() unpacks an octet starts with the MSB .
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void xlate_mac_hdr_2nd_optional_octet
(
 uint8               *src,                  /* source message in 'over-the-air' format */
 mac_hdr_2nd_opt_t   *mac_2nd_opt_oct_ptr,   /* destination internal format */
 gas_id_t             gas_id
)
{
  /* offset from the beginning of the message to the current item */
  uint16 msg_pos = 0;

  /* extract bit fileds of 2'nd optional  octet */
  mac_2nd_opt_oct_ptr->pr = gunpackb(src, &msg_pos, GSIZE(mac_hdr_2nd_opt_t, pr));
  mac_2nd_opt_oct_ptr->tfi = gunpackb(src, &msg_pos, GSIZE(mac_hdr_2nd_opt_t, tfi));
  mac_2nd_opt_oct_ptr->dir = gunpackb(src, &msg_pos, GSIZE(mac_hdr_2nd_opt_t, dir));
}
#endif /*FEATURE_GSM_EDTM*/

/*===========================================================================
===
===  FUNCTION      xlate_mac_hdr_1st_optional_octet
===
===  DESCRIPTION
===  Translate the 1'st optional octet mac header from external
===  format into internal format
===
===  Note the function gunpackb() unpacks an octet starts with the MSB .
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static void xlate_mac_hdr_1st_optional_octet
(
  uint8              *src,                /* source message in 'over-the-air' format */
  mac_hdr_1st_opt_t  *mac_1st_opt_oct_ptr, /* destination internal format */
  gas_id_t           gas_id
)
{
  /* offset from the beginning of the message to the current item */
  uint16 msg_pos = 0;

  /* extract bit fileds of 1'st optional  octet */
  mac_1st_opt_oct_ptr->rbsn = gunpackb(src, &msg_pos, GSIZE(mac_hdr_1st_opt_t, rbsn));
  mac_1st_opt_oct_ptr->rti = gunpackb(src, &msg_pos, GSIZE(mac_hdr_1st_opt_t, rti));
  mac_1st_opt_oct_ptr->fs = gunpackb(src, &msg_pos, GSIZE(mac_hdr_1st_opt_t, fs));
  mac_1st_opt_oct_ptr->ac = gunpackb(src, &msg_pos, GSIZE(mac_hdr_1st_opt_t, ac));
}


#ifdef FEATURE_GSM_EDTM

/*===========================================================================
===
===  FUNCTION      retrieve_dl_logical_channel_msg
===
===  DESCRIPTION
===  In idle MS has to store 8 segmented messages and 2 when in transfer.
===
===  This function adds, removes and administrates the sections of
===  a segmented DL MAC control messages.
===
===  The flushing of the list when MS changes mode.
===
===  Starting T3200 and clearing T3200. The retrieving of the logical
===  channel message.
===
===  Determining the CRTL_ACK value to be used in the PCA.
===
===  see 4.60, chapters 9.1.11a, 9.1.11b, 9.2, 10.3
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===  The DL channel msg type. (if a complete DL msg is to retrieved then NO_MSG is returned).
===
===  SIDE EFFECTS
===
===
===========================================================================*/
static uint8 retrieve_dl_logical_channel_msg(l1_mac_sig_t *lm_sig_ptr, gas_id_t gas_id)
{
  /*dl segmented list declirations */
  uint8   i;
  uint8   msg_buffer_wanted;

  /*dl mac control header declirations*/
  uint8   msg = SEGMENTED_MSG;

  /***********************************************************/
  /*          dl mac control header bit field extraction     */
  /***********************************************************/
  uint8             *ptr_opt_oct_1;
  mac_hdr_1st_opt_t *ptr_mac_1st_opt_oct;

  uint8 *ptr_opt_oct_ext_seg;
  mac_hdr_ext_seg_opt_t *ptr_mac_ext_seg_opt_oct,mac_ext_seg_opt_oct;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_rb_data_t     *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);

  boolean msg_complete = FALSE;

  memset(&mac_ext_seg_opt_oct,0,sizeof(mac_ext_seg_opt_oct));

  /* 1st optional octet present*/
  ptr_opt_oct_1 = &lm_sig_ptr->msg.data_ind.dl_ctrl_block[1] ;
  ptr_mac_1st_opt_oct = &rb_data_ptr->mac_1st_opt_oct;

  xlate_mac_hdr_1st_optional_octet(ptr_opt_oct_1,  ptr_mac_1st_opt_oct, gas_id);

  /* rti received in msg, used to check if 2nd segment is in list*/
  msg_buffer_wanted = rb_data_ptr->mac_1st_opt_oct.rti;

  /*check for ext seg protocol errors*/
  if(rb_data_ptr->mac_1st_opt_oct.fs == 0 && rb_data_ptr->mac_1st_opt_oct.rbsn == 1)
  {

    if(rb_data_ptr->mac_1st_opt_oct.ac == 0)
    {
       /*1st optional oct + extended segmentation oct*/
       /*mac header + 1 optional octet + extended segmantation oct*/
       ptr_opt_oct_ext_seg = &lm_sig_ptr->msg.data_ind.dl_ctrl_block[2] ;
       MSG_GERAN_HIGH_1_G("MAC info ac bit = 0, ext seg octet =  x%x",lm_sig_ptr->msg.data_ind.dl_ctrl_block[2]);
    }
    else
    {
      /*1st and 2nd optional oct + extended segmentation oct*/
      /*mac header + 2 optional octet + extended segmantation oct*/
      ptr_opt_oct_ext_seg = &lm_sig_ptr->msg.data_ind.dl_ctrl_block[3] ; 
      MSG_GERAN_HIGH_1_G("MAC info ac bit = 1, ext seg octet = x%x",lm_sig_ptr->msg.data_ind.dl_ctrl_block[3]);
    }    


    ptr_mac_ext_seg_opt_oct= &mac_ext_seg_opt_oct;    
    xlate_mac_hdr_ext_seg_optional_octet(ptr_opt_oct_ext_seg,  ptr_mac_ext_seg_opt_oct, gas_id);

    if(mac_ext_seg_opt_oct.fse == 1 && mac_ext_seg_opt_oct.rbsne == 0)
    {
      /*exit protocol error fse = and rbsne 0 is a invalid cobbination*/
      MSG_GERAN_HIGH_2_G("GMAC info protocol error is a invalid combination fse =%d, rbsne =%d", 
                         mac_ext_seg_opt_oct.fse,mac_ext_seg_opt_oct.rbsne);
                
      return(msg);
    }
    else if(mac_ext_seg_opt_oct.fse == 0 && mac_ext_seg_opt_oct.rbsne == 7)
    {
      /*exit protocol error fse = and rbsne 0 is a invalid cobbination*/
      MSG_GERAN_HIGH_2_G("GMAC info protocol error is a invalid combination fse =%d, rbsne =%d", 
                         mac_ext_seg_opt_oct.fse,mac_ext_seg_opt_oct.rbsne);
      return(msg);       
    }
    else
    {
      /*acceptable fse/rbsne combination continue*/
      MSG_GERAN_HIGH_2_G("GMAC info ext seg header rcvd fse =%d, rbsne =%d", mac_ext_seg_opt_oct.fse,mac_ext_seg_opt_oct.rbsne);         
    }

  }
  else
  {    
    if(rb_data_ptr->mac_1st_opt_oct.fs == 1 && rb_data_ptr->mac_1st_opt_oct.rbsn == 0)
    {
     /*fs =1 rbsn =0, not segmented.... but power reduction*/
     MSG_GERAN_HIGH_0_G("GMAC info Not a segmented msg should never get here");
     return(msg);
    }       
  }

  /**************************************************************************************************
  *          dl mac control  segmented msg list maintenance
  *                 and msg re-assembly
  ***************************************************************************************************/

  /*create sentinel ie initialise list*/
  if (rb_data_ptr->list_full == GMAC_DL_MSG_LIST_EMPTY)
  {
    rb_data_ptr->first_in_list.ptr_to_next_buff = NULL;
    rb_data_ptr->ptr_to_current_in_list= &rb_data_ptr->first_in_list;
    rb_data_ptr->list_full = GMAC_DL_MSG_LIST_NOT_FULL;
  }

  /*check the current list of segmented dl control messages */
  if (rb_data_ptr->list_full == GMAC_DL_MSG_LIST_NOT_FULL)
  {
    /*list not full*/
    if(rb_data_ptr->first_in_list.ptr_to_next_buff != NULL)
    {
      rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->first_in_list.ptr_to_next_buff;
      rb_data_ptr->ptr_to_previous_in_list = &rb_data_ptr->first_in_list;
    }
    else
    {
      /*first time round loop*/
      rb_data_ptr->ptr_to_current_in_list= &rb_data_ptr->first_in_list;
    }

    for(i=1;i<8;i++)
    {
      if(rb_data_ptr->first_in_list.ptr_to_next_buff != NULL)
      {
        /***********************************************************************************
        *  location in list already occupied. if RTI's match second 1/2 of DL message
        *  has been received otherwise append the DL message segment to the end of the list
        ************************************************************************************/

        if((rb_data_ptr->ptr_to_current_in_list->msg_buffer_rti == msg_buffer_wanted)
          && (rb_data_ptr->ptr_to_current_in_list->ts_received_on == lm_sig_ptr->msg.data_ind.timeslot))
        {
          /* rti found in list*/
          MSG_GERAN_HIGH_0_G("GMAC info list NOT full, segment belongs to segmented msg in list,add this segment to it");
  
          gmac_update_segmented_vars(&(lm_sig_ptr->msg.data_ind),rb_data_ptr->ptr_to_current_in_list,gas_id);

          /*final segment has been rcvd, have we got a complete msg*/
          msg_complete =  gmac_segmented_msg_complete( rb_data_ptr->ptr_to_current_in_list->ext_seg_rcvd_bit_map,
                                                       rb_data_ptr->ptr_to_current_in_list->final_segment_rbsn,
                                                       gas_id);  

          if(msg_complete == TRUE)
          {
            /*Clear timer T3200 for segment retrieved from  list*/
            clear_t3200(rb_data_ptr->ptr_to_current_in_list->timer_id,gas_id);
    
            /*retrive completed segment msg and decode and decoded it */   
            msg = gmac_retrieve_completed_segmented_msg(global_data_ptr->dl_msg,rb_data_ptr->ptr_to_current_in_list,gas_id);

            MSG_GERAN_HIGH_3_G("GMAC info complete DL msg. RTI=%d TS=%d list pos =%d", msg_buffer_wanted, lm_sig_ptr->msg.data_ind.timeslot, i );

            /* remove from list and free memory*/
            rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;

            /*free up memory*/
            GPRS_MEM_FREE(rb_data_ptr->ptr_to_current_in_list);

            /*indicate list is not full*/
            rb_data_ptr->list_full = GMAC_DL_MSG_LIST_NOT_FULL;

          }

          /*exit for loop as msg segment belong to a segmented msg in the process of being rcvd */   
          break;
                  
        }
        else
        {
          /* rti not found ...add new entry if location not occupied*/
          if(rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff != NULL)
          {
            /*location in list already occupied*/
            rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
            rb_data_ptr->ptr_to_previous_in_list= rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff;
          }
          else
          {
            MSG_GERAN_MED_3_G("GMAC info next DL seg RTI=%d TS=%d list No.=%d", msg_buffer_wanted, lm_sig_ptr->msg.data_ind.timeslot, i+1);

            /*create new entry*/
            rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff = (dl_msg_buffer_t *)GPRS_MEM_MALLOC(sizeof(dl_msg_buffer_t) / sizeof(uint8));

            if ( rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff == NULL )
            {
              ERR_GERAN_FATAL_0_G("GMAC IDLE  Info : Heap Exhaustion.");
            }
            else
            {
              rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;

              /*initialises segmented msg data into list*/
              gmac_initialise_segmented_vars(rb_data_ptr->ptr_to_current_in_list,lm_sig_ptr->msg.data_ind.timeslot,rb_data_ptr->mac_1st_opt_oct.rti,gas_id);                          

              MSG_GERAN_HIGH_0_G("GMAC info list NOT full, segment does NOT belongs to segmented msg in list, New segmented msg to add to list");

              gmac_update_segmented_vars(&(lm_sig_ptr->msg.data_ind),rb_data_ptr->ptr_to_current_in_list,gas_id);

              rb_data_ptr->ptr_last_in_list = rb_data_ptr->ptr_to_current_in_list;

              if(i == 7)
              {
                /*list now completly filled*/
                rb_data_ptr->list_full = GMAC_DL_MSG_LIST_FULL;
              }
              break;
            }
          }
        }
      }
      else
      {
        /*****************************************************
        *         List is empty,create first entry          *
        ******************************************************/
        MSG_GERAN_MED_2_G("GMAC info 1 st DL seg RTI=%d TS=%d list No. = 1",msg_buffer_wanted, lm_sig_ptr->msg.data_ind.timeslot);

        rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff = (dl_msg_buffer_t *)GPRS_MEM_MALLOC(sizeof(dl_msg_buffer_t) / sizeof(uint8));

        if ( rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff == NULL )
        {
          ERR_GERAN_FATAL_0_G("GMAC IDLE  Info : Heap Exhaustion.");
        }
        else
        {
          rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;

          /*initialises segmented msg data into list*/
          gmac_initialise_segmented_vars(rb_data_ptr->ptr_to_current_in_list,lm_sig_ptr->msg.data_ind.timeslot,rb_data_ptr->mac_1st_opt_oct.rti,
                                         gas_id);

          /*initialsize*/
          MSG_GERAN_HIGH_0_G("GMAC info list EMPTY, New segmented msg added to list");

          gmac_update_segmented_vars(&(lm_sig_ptr->msg.data_ind),rb_data_ptr->ptr_to_current_in_list,
                                       gas_id);

          rb_data_ptr->ptr_last_in_list = rb_data_ptr->ptr_to_current_in_list;

          break;
        }
      }
    }
  }
  else
  {
  /****************************************************************************
  * list full
    ****************************************************************************/
    rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->first_in_list.ptr_to_next_buff;
    rb_data_ptr->ptr_to_previous_in_list = &rb_data_ptr->first_in_list;

    for(i=1;i<9;i++)
    {
      if(rb_data_ptr->ptr_to_current_in_list->msg_buffer_rti == msg_buffer_wanted)
      {
      /*****************************************************************************
      *  rti found in list,get other segment and combine the 2 segments in dl_msg
        ******************************************************************************/

        /*Clear timer T3200 for segment retrieved from  list*/
        clear_t3200(rb_data_ptr->ptr_to_current_in_list->timer_id, gas_id);

        MSG_GERAN_HIGH_0_G("GMAC info list FULL, segment belongs to segmented msg in list, add this segment to it");
  
        gmac_update_segmented_vars(&(lm_sig_ptr->msg.data_ind),rb_data_ptr->ptr_to_current_in_list, gas_id);

        /*final segment has been rcvd, have we got a complete msg*/
        msg_complete =  gmac_segmented_msg_complete( rb_data_ptr->ptr_to_current_in_list->ext_seg_rcvd_bit_map,
                                                     rb_data_ptr->ptr_to_current_in_list->final_segment_rbsn,
                                                     gas_id);  

        if(msg_complete == TRUE)
        {
          /*Clear timer T3200 for segment retrieved from  list*/
          clear_t3200(rb_data_ptr->ptr_to_current_in_list->timer_id,gas_id);
    
          /*retrive completed segment msg and decode and decoded it */   
          msg = gmac_retrieve_completed_segmented_msg(global_data_ptr->dl_msg,rb_data_ptr->ptr_to_current_in_list,gas_id);

          MSG_GERAN_HIGH_3_G("GMAC info complete DL msg. RTI=%d TS=%d list pos =%d", 
                             msg_buffer_wanted, lm_sig_ptr->msg.data_ind.timeslot, i );

          /* remove from list and free memory*/
          rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;

          /*free up memory*/
          GPRS_MEM_FREE(rb_data_ptr->ptr_to_current_in_list);

          /*indicate list is not full*/
          rb_data_ptr->list_full = GMAC_DL_MSG_LIST_NOT_FULL;
        }
        else
        {
          MSG_GERAN_HIGH_3_G( "GMAC info incomplete DL msg RTI=%d TS=%d list pos =%d", 
                              msg_buffer_wanted, lm_sig_ptr->msg.data_ind.timeslot, i );
        }

        /*exit for loop as msg segment belong to a segmented msg in the process of being rcvd */   
        break;
      }
      else
      {

        /*no match found move to next inlist*/
        rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
        rb_data_ptr->ptr_to_previous_in_list= rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff;

        if(i > 7)
        {
          /*********************************************
          * list full and no match found
          *********************************************/

          /*remove oldest from list*/
          rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->first_in_list.ptr_to_next_buff;
          rb_data_ptr->ptr_to_previous_in_list = &rb_data_ptr->first_in_list;

          rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff =  rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;

          clear_t3200(rb_data_ptr->ptr_to_current_in_list->timer_id,gas_id);

          /*free up memory*/
          GPRS_MEM_FREE(rb_data_ptr->ptr_to_current_in_list);

          /*create a new entry */
          rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_last_in_list;
          rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff = (dl_msg_buffer_t *)GPRS_MEM_MALLOC(sizeof(dl_msg_buffer_t) / sizeof(uint8));

          if ( rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff == NULL )
          {
            ERR_GERAN_FATAL_0_G("GMAC IDLE  Info : Heap Exhaustion.");
          }
          else
          {
            /*append to end of list*/
            rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;

            /*initialises segmented msg data into list*/
            gmac_initialise_segmented_vars(rb_data_ptr->ptr_to_current_in_list,lm_sig_ptr->msg.data_ind.timeslot,rb_data_ptr->mac_1st_opt_oct.rti, gas_id);

            /*initialsize*/
            MSG_GERAN_HIGH_0_G("GMAC info list FULL, no match found,remove oldest from list and add new segmented msg to list");

            gmac_update_segmented_vars(&(lm_sig_ptr->msg.data_ind),rb_data_ptr->ptr_to_current_in_list,gas_id);

            rb_data_ptr->ptr_last_in_list = rb_data_ptr->ptr_to_current_in_list;

          }
        }
      }
    }
  }
  return (msg);
}
#else /*FEATURE_GSM_EDTM*/
uint8 retrieve_dl_logical_channel_msg(l1_mac_sig_t *lm_sig_ptr, gas_id_t gas_id)
{
  uint8   i;
  uint8   msg_buffer_wanted;
  boolean first_segment = TRUE;
  uint8   number_header_octets = 0;
  uint8   msg = SEGMENTED_MSG;
  uint8             *ptr_opt_oct_1;
  mac_hdr_1st_opt_t *ptr_mac_1st_opt_oct;
  uint8             *ptr_opt_oct_2;
  mac_hdr_2nd_opt_t *ptr_mac_2nd_opt_oct, mac_2nd_opt_oct;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_rb_data_t     *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);
  global_data_ptr->csn_msg_bit_length = 0;
  ptr_opt_oct_1 = &lm_sig_ptr->msg.data_ind.dl_ctrl_block[1] ;
  ptr_mac_1st_opt_oct = &rb_data_ptr->mac_1st_opt_oct;
  xlate_mac_hdr_1st_optional_octet(ptr_opt_oct_1,  ptr_mac_1st_opt_oct, gas_id);
  if( rb_data_ptr->mac_1st_opt_oct.ac != 1)
  {
    number_header_octets = 2;  /*mac header + 1 optional octet*/
  }
  else
  {
    number_header_octets = 3;  /*mac header + 2 optional octet*/
    ptr_opt_oct_2 = &lm_sig_ptr->msg.data_ind.dl_ctrl_block[2] ;
    ptr_mac_2nd_opt_oct = &mac_2nd_opt_oct;
    xlate_mac_hdr_2nd_optional_octet(ptr_opt_oct_2,  ptr_mac_2nd_opt_oct, gas_id);
  }
  msg_buffer_wanted = rb_data_ptr->mac_1st_opt_oct.rti;
  if (rb_data_ptr->mac_1st_opt_oct.fs == 0 && rb_data_ptr->mac_1st_opt_oct.rbsn == 0)
  {
    first_segment = TRUE;
  }
  else if (rb_data_ptr->mac_1st_opt_oct.fs == 1 && rb_data_ptr->mac_1st_opt_oct.rbsn == 1)
  {
    first_segment = FALSE;
  }
  else if(rb_data_ptr->mac_1st_opt_oct.fs == 1 && rb_data_ptr->mac_1st_opt_oct.rbsn == 0)
  {
  }
  else
  {
  }
  if (rb_data_ptr->list_full == GMAC_DL_MSG_LIST_EMPTY)
  {
    rb_data_ptr->first_in_list.ptr_to_next_buff = NULL;
    rb_data_ptr->ptr_to_current_in_list= &rb_data_ptr->first_in_list;
    rb_data_ptr->list_full = GMAC_DL_MSG_LIST_NOT_FULL;
  }
  if (rb_data_ptr->list_full == GMAC_DL_MSG_LIST_NOT_FULL)
  {
    if(rb_data_ptr->first_in_list.ptr_to_next_buff != NULL)
    {
      rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->first_in_list.ptr_to_next_buff;
      rb_data_ptr->ptr_to_previous_in_list = &rb_data_ptr->first_in_list;
    }
    else
    {
      rb_data_ptr->ptr_to_current_in_list= &rb_data_ptr->first_in_list;
    }
    for(i=1;i<8;i++)
    {
      if(rb_data_ptr->first_in_list.ptr_to_next_buff != NULL)
      {
        if((rb_data_ptr->ptr_to_current_in_list->msg_buffer_rti == msg_buffer_wanted)
          && (rb_data_ptr->ptr_to_current_in_list->ts_received_on == lm_sig_ptr->msg.data_ind.timeslot))
        {
          clear_t3200(rb_data_ptr->ptr_to_current_in_list->timer_id, gas_id);
          if (rb_data_ptr->ptr_to_current_in_list->no_header_octets > SIZE_OF_CONTROL_RADIO_BLOCK)
          {
            MSG_GERAN_ERROR_1_G("Bad no_header_octets:%d, reject DL message",rb_data_ptr->ptr_to_current_in_list->no_header_octets);
            memset(global_data_ptr->dl_msg,0,SIZE_OF_CONTROL_RADIO_BLOCK);
            global_data_ptr->dl_msg[0] = PACKET_UNUSED_ID << 2;
          }
          else
          {
            if(first_segment == TRUE)
            {
              memscpy(global_data_ptr->dl_msg,sizeof(global_data_ptr->dl_msg),
                      &lm_sig_ptr->msg.data_ind.dl_ctrl_block[number_header_octets ],
                      SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets );
              memscpy(&(global_data_ptr->dl_msg [SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets]),
                      (sizeof(global_data_ptr->dl_msg) - (SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets)),
                      &rb_data_ptr->ptr_to_current_in_list->msg_segment[rb_data_ptr->ptr_to_current_in_list->no_header_octets],
                      SIZE_OF_CONTROL_RADIO_BLOCK - rb_data_ptr->ptr_to_current_in_list->no_header_octets);
            }
            else
            {
              memscpy(global_data_ptr->dl_msg,sizeof(global_data_ptr->dl_msg),
                      &rb_data_ptr->ptr_to_current_in_list->msg_segment[rb_data_ptr->ptr_to_current_in_list->no_header_octets],
                      SIZE_OF_CONTROL_RADIO_BLOCK - rb_data_ptr->ptr_to_current_in_list->no_header_octets);
              memscpy(&(global_data_ptr->dl_msg [SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets]),
                      (sizeof(global_data_ptr->dl_msg) - (SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets)),
                      &(lm_sig_ptr->msg.data_ind.dl_ctrl_block[number_header_octets ]),
                      SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets );
            }
          }
          size_of_rcvd_dl_control( (uint8)((SIZE_OF_CONTROL_RADIO_BLOCK * 2) - (number_header_octets + rb_data_ptr->ptr_to_current_in_list->no_header_octets)));
          global_data_ptr->csn_decoder_result = gcsn1_decoder(
            global_data_ptr->dl_msg,
            &global_data_ptr->csn_msg_bit_length,
            SIZE_OF_DL_MSG,
            gas_id
          );
          global_data_ptr->csn_dec_result_ptr = &global_data_ptr->csn_decoder_result;
          global_data_ptr->csn_dec_data_ptr = global_data_ptr->csn_decoder_result.data_ptr;
          global_data_ptr->csn_dec_hdr_ptr = global_data_ptr->csn_decoder_result.header_ptr;
          msg = global_data_ptr->csn_dec_hdr_ptr->message_type;
          gmac_log_dl_channel_msg(msg, global_data_ptr->csn_msg_bit_length,gas_id);
          MSG_GERAN_LOW_3_G( "complete DL msg. RTI=%d TS=%d list pos =%d", msg_buffer_wanted, lm_sig_ptr->msg.data_ind.timeslot, i );
          rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
          GPRS_MEM_FREE(rb_data_ptr->ptr_to_current_in_list);
          rb_data_ptr->list_full = GMAC_DL_MSG_LIST_NOT_FULL;
          break;
        }
        else
        {
          if(rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff != NULL)
          {
            rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
            rb_data_ptr->ptr_to_previous_in_list=rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff;
          }
          else
          {
            MSG_GERAN_LOW_3_G("next DL seg RTI=%d TS=%d list No.=%d", msg_buffer_wanted, lm_sig_ptr->msg.data_ind.timeslot, i+1);
            rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff = (dl_msg_buffer_t *)GPRS_MEM_MALLOC(sizeof(dl_msg_buffer_t) / sizeof(uint8));
            if ( rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff == NULL )
            {
              ERR_GERAN_FATAL_0_G("GMAC IDLE  Info : Heap Exhaustion.");
            }
            else
            {
              rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
              rb_data_ptr->ptr_to_current_in_list->no_header_octets = number_header_octets;
              rb_data_ptr->ptr_to_current_in_list->first_msg_segment = first_segment;
              rb_data_ptr->ptr_to_current_in_list->msg_buffer_rti = rb_data_ptr->mac_1st_opt_oct.rti;
              rb_data_ptr->ptr_to_current_in_list->ts_received_on = lm_sig_ptr->msg.data_ind.timeslot;
              memscpy(rb_data_ptr->ptr_to_current_in_list->msg_segment,sizeof(rb_data_ptr->ptr_to_current_in_list->msg_segment),                      
                      lm_sig_ptr->msg.data_ind.dl_ctrl_block,
                      SIZE_OF_CONTROL_RADIO_BLOCK);
              rb_data_ptr->ptr_to_current_in_list->timer_id = set_t3200(gas_id);
              rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff = NULL;
              rb_data_ptr->ptr_last_in_list = rb_data_ptr->ptr_to_current_in_list;
              if(i == 7)
              {
                rb_data_ptr->list_full = GMAC_DL_MSG_LIST_FULL;
              }
              break;
            }
          }
        }
      }
      else
      {
        MSG_GERAN_LOW_3_G( "1 st DL seg RTI=%d TS=%d list No. =%d", msg_buffer_wanted, lm_sig_ptr->msg.data_ind.timeslot, i );
        rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff = (dl_msg_buffer_t *)GPRS_MEM_MALLOC(sizeof(dl_msg_buffer_t) / sizeof(uint8));
        if ( rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff == NULL )
        {
          ERR_GERAN_FATAL_0_G("GMAC IDLE  Info : Heap Exhaustion.");
        }
        else
        {
          rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
          rb_data_ptr->ptr_to_current_in_list->no_header_octets = number_header_octets;
          rb_data_ptr->ptr_to_current_in_list->first_msg_segment = first_segment;
          rb_data_ptr->ptr_to_current_in_list->msg_buffer_rti = rb_data_ptr->mac_1st_opt_oct.rti;
          rb_data_ptr->ptr_to_current_in_list->ts_received_on = lm_sig_ptr->msg.data_ind.timeslot;
          memscpy(rb_data_ptr->ptr_to_current_in_list->msg_segment,sizeof(rb_data_ptr->ptr_to_current_in_list->msg_segment),
                  lm_sig_ptr->msg.data_ind.dl_ctrl_block,
                  SIZE_OF_CONTROL_RADIO_BLOCK);
          rb_data_ptr->ptr_to_current_in_list->timer_id = set_t3200(gas_id);
          rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff = NULL;
          rb_data_ptr->ptr_last_in_list = rb_data_ptr->ptr_to_current_in_list;
          break;
        }
      }
    }
  }
  else
  {
    rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->first_in_list.ptr_to_next_buff;
    rb_data_ptr->ptr_to_previous_in_list = &rb_data_ptr->first_in_list;
    for(i=1;i<9;i++)
    {
      if(rb_data_ptr->ptr_to_current_in_list->msg_buffer_rti == msg_buffer_wanted)
      {
        MSG_GERAN_LOW_3_G( "list full, match, complete DL msg RTI=%d TS=%d list pos =%d", 
                           msg_buffer_wanted, lm_sig_ptr->msg.data_ind.timeslot, i );
        clear_t3200(rb_data_ptr->ptr_to_current_in_list->timer_id, gas_id);
        if (rb_data_ptr->ptr_to_current_in_list->no_header_octets > SIZE_OF_CONTROL_RADIO_BLOCK)
        { 
          MSG_GERAN_ERROR_1_G("Bad no_header_octets:%d, reject DL message",rb_data_ptr->ptr_to_current_in_list->no_header_octets);
          memset(global_data_ptr->dl_msg,0,SIZE_OF_CONTROL_RADIO_BLOCK);
          global_data_ptr->dl_msg[0] = PACKET_UNUSED_ID << 2;
        }
        else
        {
          if(first_segment == TRUE)
          {
            memscpy(global_data_ptr->dl_msg,sizeof(global_data_ptr->dl_msg),
                    &lm_sig_ptr->msg.data_ind.dl_ctrl_block[number_header_octets ],
                    SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets );
            memscpy(&(global_data_ptr->dl_msg [SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets]),
                    (sizeof(global_data_ptr->dl_msg) - (SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets)), 
                    &rb_data_ptr->ptr_to_current_in_list->msg_segment[rb_data_ptr->ptr_to_current_in_list->no_header_octets],
                    SIZE_OF_CONTROL_RADIO_BLOCK - rb_data_ptr->ptr_to_current_in_list->no_header_octets);
          }
          else
          {
            memscpy(global_data_ptr->dl_msg,sizeof(global_data_ptr->dl_msg), 
                    &rb_data_ptr->ptr_to_current_in_list->msg_segment[rb_data_ptr->ptr_to_current_in_list->no_header_octets],
                    SIZE_OF_CONTROL_RADIO_BLOCK - rb_data_ptr->ptr_to_current_in_list->no_header_octets);
            memscpy(&(global_data_ptr->dl_msg [SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets]),
                    (sizeof(global_data_ptr->dl_msg) - (SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets)), 
                    &lm_sig_ptr->msg.data_ind.dl_ctrl_block[number_header_octets ],
                    SIZE_OF_CONTROL_RADIO_BLOCK - number_header_octets );
          }
        }
        size_of_rcvd_dl_control((uint8)((SIZE_OF_CONTROL_RADIO_BLOCK * 2) - (number_header_octets + rb_data_ptr->ptr_to_current_in_list->no_header_octets)));
        global_data_ptr->csn_decoder_result = gcsn1_decoder(
          global_data_ptr->dl_msg,
          &global_data_ptr->csn_msg_bit_length,
          SIZE_OF_DL_MSG,
          gas_id
        ); 
        global_data_ptr->csn_dec_result_ptr = &global_data_ptr->csn_decoder_result;
        global_data_ptr->csn_dec_data_ptr = global_data_ptr->csn_decoder_result.data_ptr;
        global_data_ptr->csn_dec_hdr_ptr = global_data_ptr->csn_decoder_result.header_ptr;
        msg = global_data_ptr->csn_dec_hdr_ptr->message_type;
        gmac_log_dl_channel_msg(msg, global_data_ptr->csn_msg_bit_length, gas_id);
        rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
        GPRS_MEM_FREE(rb_data_ptr->ptr_to_current_in_list);
        rb_data_ptr->list_full = GMAC_DL_MSG_LIST_NOT_FULL;
        break;
      }
      else
      {
        rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
        rb_data_ptr->ptr_to_previous_in_list=rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff;
        if(i > 7)
        {
          MSG_GERAN_LOW_3_G( "list full no match RTI=%d TS=%d list pos =%d", msg_buffer_wanted, lm_sig_ptr->msg.data_ind.timeslot, i );
          rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->first_in_list.ptr_to_next_buff;
          rb_data_ptr->ptr_to_previous_in_list = &rb_data_ptr->first_in_list;
          rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff =  rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
          clear_t3200(rb_data_ptr->ptr_to_current_in_list->timer_id, gas_id);
          GPRS_MEM_FREE(rb_data_ptr->ptr_to_current_in_list);
          rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_last_in_list;
          rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff = (dl_msg_buffer_t *)GPRS_MEM_MALLOC(sizeof(dl_msg_buffer_t) / sizeof(uint8));
          if ( rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff == NULL )
          {
            ERR_GERAN_FATAL_0_G("GMAC IDLE  Info : Heap Exhaustion.");
          }
          else
          {
            rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
            rb_data_ptr->ptr_to_current_in_list->no_header_octets = number_header_octets;
            rb_data_ptr->ptr_to_current_in_list->first_msg_segment = first_segment;
            rb_data_ptr->ptr_to_current_in_list->msg_buffer_rti = rb_data_ptr->mac_1st_opt_oct.rti;
            rb_data_ptr->ptr_to_current_in_list->ts_received_on = lm_sig_ptr->msg.data_ind.timeslot;
            memscpy(rb_data_ptr->ptr_to_current_in_list->msg_segment,sizeof(rb_data_ptr->ptr_to_current_in_list->msg_segment),
                    lm_sig_ptr->msg.data_ind.dl_ctrl_block,SIZE_OF_CONTROL_RADIO_BLOCK);
            rb_data_ptr->ptr_to_current_in_list->timer_id = set_t3200(gas_id);
            rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff = NULL;
            rb_data_ptr->ptr_last_in_list = rb_data_ptr->ptr_to_current_in_list;
          }
        }
      }
    }
  }
  return (msg);
}
#endif /*FEATURE_GSM_EDTM*/
static uint8 set_t3200(gas_id_t gas_id)
{
  uint8  idle_timer_id;
  uint8 bs_cv_max;      /* value of zero translates to 1 when calculation */
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);
  boolean non_drx_mode = TRUE;
  rr_read_public_store(RRPS_CELL_OPTIONS_BS_CV_MAX,&bs_cv_max,gas_id);
  for(idle_timer_id= (uint8)MAC_TIMER_T3200_1;idle_timer_id<= (uint8)MAC_TIMER_T3200_8;idle_timer_id++)
  {
    if (rb_data_ptr->t3200_active[idle_timer_id - (uint8)MAC_TIMER_T3200_1] == FALSE)
    {
      if(non_drx_mode)
      {
        if(bs_cv_max == 0 )
        {
          (void)rex_set_timer(&mac_timers_ptr->gmac_t3200_array[idle_timer_id - (uint8)MAC_TIMER_T3200_1].timer_t3200, T3200_DURATION(1) );
        }
        else
        {
          (void)rex_set_timer(&mac_timers_ptr->gmac_t3200_array[idle_timer_id - (uint8)MAC_TIMER_T3200_1].timer_t3200, T3200_DURATION(bs_cv_max) );
        }
      }
      else
      {
      }
      rb_data_ptr->t3200_active[idle_timer_id - (uint8)MAC_TIMER_T3200_1 ] = TRUE;
      break;
    }
  }
  return ((uint8)idle_timer_id);
}
static void clear_t3200(uint8 idle_timer_id, gas_id_t gas_id)
{
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);
  (void)rex_clr_timer(&mac_timers_ptr->gmac_t3200_array[idle_timer_id - (uint8)MAC_TIMER_T3200_1].timer_t3200);
  rb_data_ptr->t3200_active[idle_timer_id - (uint8)MAC_TIMER_T3200_1] = FALSE;
}
boolean rb_block_handler(gas_id_t gas_id)
{
  boolean segmented_msg = FALSE;
  uint8 rbsn_bit  = 0;
  uint8 fs_bit = 0;
  uint8 ac_bit = 0;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);
  global_data_ptr->csn_msg_bit_length = 0;
#if defined( DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
  #error code not present
#else
  xlate_ext_mac_hdr(&global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] , rb_data_ptr->dl_mac_header_prt);
#endif /* DEBUG_GSM_GPRS_RLC_TEST */
  if(rb_data_ptr->mac_header.payload == 2)
  {
    rbsn_bit = (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] >> 7);
    fs_bit =  ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] >> 1) & 0x01);
    if((rbsn_bit == 0)  && (fs_bit == 1))
    {
      ac_bit = (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] & 0x01 );
      if(ac_bit == 1)
      {
        memscpy(global_data_ptr->dl_msg,sizeof(global_data_ptr->dl_msg),
                &global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[3],
                (MAX_SIZE_OF_CONTROL_BLOCK_DATA - 2));
        size_of_rcvd_dl_control(MAX_SIZE_OF_CONTROL_BLOCK_DATA - 2);
      }
      else
      {
        memscpy(global_data_ptr->dl_msg,sizeof(global_data_ptr->dl_msg),
                &global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2],
                (MAX_SIZE_OF_CONTROL_BLOCK_DATA - 1));
        size_of_rcvd_dl_control(MAX_SIZE_OF_CONTROL_BLOCK_DATA - 1);
      }
      global_data_ptr->csn_decoder_result = gcsn1_decoder(
        global_data_ptr->dl_msg,
        &global_data_ptr->csn_msg_bit_length,
        SIZE_OF_DL_MSG,
        gas_id
      );
      global_data_ptr->csn_dec_result_ptr = &global_data_ptr->csn_decoder_result;
      global_data_ptr->csn_dec_data_ptr = global_data_ptr->csn_decoder_result.data_ptr;
      global_data_ptr->csn_dec_hdr_ptr = global_data_ptr->csn_decoder_result.header_ptr;
      global_data_ptr->channel_msg = global_data_ptr->csn_dec_hdr_ptr->message_type;
      gmac_log_dl_channel_msg(global_data_ptr->channel_msg, global_data_ptr->csn_msg_bit_length, gas_id);
    }
    else
    {
      global_data_ptr->channel_msg = retrieve_dl_logical_channel_msg(&(global_data_ptr->msg_ptr->l1_mac_sig),gas_id);
      if(global_data_ptr->channel_msg == SEGMENTED_MSG)
      {
        segmented_msg = TRUE;
      }
    }
  }
  else
  {
    memscpy(global_data_ptr->dl_msg,sizeof(global_data_ptr->dl_msg),
            &global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1 ],
            (MAX_SIZE_OF_CONTROL_BLOCK_DATA));
    size_of_rcvd_dl_control(MAX_SIZE_OF_CONTROL_BLOCK_DATA);
    global_data_ptr->csn_decoder_result = gcsn1_decoder(
      global_data_ptr->dl_msg,
      &global_data_ptr->csn_msg_bit_length,
      SIZE_OF_DL_MSG,
      gas_id
    );
    global_data_ptr->csn_dec_result_ptr = &global_data_ptr->csn_decoder_result;
    global_data_ptr->csn_dec_data_ptr = global_data_ptr->csn_decoder_result.data_ptr;
    global_data_ptr->csn_dec_hdr_ptr = global_data_ptr->csn_decoder_result.header_ptr;
    global_data_ptr->channel_msg = global_data_ptr->csn_dec_hdr_ptr->message_type;
    gmac_log_dl_channel_msg(global_data_ptr->channel_msg, global_data_ptr->csn_msg_bit_length,gas_id);
  }
  if (rb_data_ptr->mac_header.type.dl_ctrl.sp == 1)
  {
    if(global_data_ptr->channel_msg == SEGMENTED_MSG)
    {
      MSG_GERAN_MED_0_G("MAC info Rcvd segmented Control msg ");
      rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);
    }
    global_data_ptr->gmac_local_rrbp_valid = RM_SP_RRBP_VALID;
  }
  else
  {
    global_data_ptr->gmac_local_rrbp_valid = RM_SP_RRBP_NOT_VALID;
  }
  return(segmented_msg);
}
void t3200_reset_handler( gas_id_t gas_id )
{
  uint8 i;
  mac_mac_sig_id_t reset_timer;
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);
  for ( reset_timer = MAC_TIMER_T3200_1; reset_timer <= MAC_TIMER_T3200_8; reset_timer++ )
  {
    if(rb_data_ptr->first_in_list.ptr_to_next_buff != NULL)
    {
      rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->first_in_list.ptr_to_next_buff;
      rb_data_ptr->ptr_to_previous_in_list = &rb_data_ptr->first_in_list;
      for(i= (uint8)MAC_TIMER_T3200_1; i<= (uint8)MAC_TIMER_T3200_8; i++)
      {
        if(rb_data_ptr->ptr_to_current_in_list->timer_id == (uint8)reset_timer)
        {
          rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
          GPRS_MEM_FREE(rb_data_ptr->ptr_to_current_in_list);
          rb_data_ptr->list_full = GMAC_DL_MSG_LIST_NOT_FULL;
          rb_data_ptr->t3200_active[(uint8)reset_timer - (uint8)MAC_TIMER_T3200_1] = FALSE;
          (void)rex_clr_timer(&mac_timers_ptr->gmac_t3200_array[(uint8)reset_timer - (uint8)MAC_TIMER_T3200_1].timer_t3200);
          break;
        }
        else
        {
          if (rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff !=NULL)
          {
            rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
            rb_data_ptr->ptr_to_previous_in_list = rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff;
          }
          else
          {
            break;
          }
        }
      }
    }
  }
}
void t3200_expiry_handler(mac_mac_sig_id_t expired_timer, gas_id_t gas_id)
{
  uint8 i;
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);
  if ( (expired_timer != MAC_TIMER_T3200_1) &&
       (expired_timer != MAC_TIMER_T3200_2) &&
       (expired_timer != MAC_TIMER_T3200_3) &&
       (expired_timer != MAC_TIMER_T3200_4) &&
       (expired_timer != MAC_TIMER_T3200_5) &&
       (expired_timer != MAC_TIMER_T3200_6) &&
       (expired_timer != MAC_TIMER_T3200_7) &&
       (expired_timer != MAC_TIMER_T3200_8) )
  {
    MSG_GERAN_ERROR_1_G("GMAC expired timer out of range: 0x%x", expired_timer);
    return;
  }
  if(rb_data_ptr->first_in_list.ptr_to_next_buff != NULL)
  {
    rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->first_in_list.ptr_to_next_buff;
    rb_data_ptr->ptr_to_previous_in_list = &rb_data_ptr->first_in_list;
    for(i= (uint8)MAC_TIMER_T3200_1; i<= (uint8)MAC_TIMER_T3200_8; i++)
    {
      if(rb_data_ptr->ptr_to_current_in_list->timer_id == (uint8)expired_timer)
      {
        rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
        GPRS_MEM_FREE(rb_data_ptr->ptr_to_current_in_list);
        rb_data_ptr->list_full = GMAC_DL_MSG_LIST_NOT_FULL;
        rb_data_ptr->t3200_active[(uint8)expired_timer - (uint8)MAC_TIMER_T3200_1] = FALSE;
        break;
      }
      else
      {
        if (rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff !=NULL)
        {
          rb_data_ptr->ptr_to_current_in_list = rb_data_ptr->ptr_to_current_in_list->ptr_to_next_buff;
          rb_data_ptr->ptr_to_previous_in_list = rb_data_ptr->ptr_to_previous_in_list->ptr_to_next_buff;
        }
        else
        {
          break;
        }
      }
    }
  }
}
uint32 gmac_rrbp_absolute_fn(uint32 dl_frame_number, uint8 rrbp_value, gas_id_t gas_id)
{
  uint32 absolute_frame_number = 0;
  uint32 hyperframe_constant = 2715648;
  switch (rrbp_value)
  {
    case 0:
      absolute_frame_number = (dl_frame_number + 13) %  hyperframe_constant;
      break ;
    case 1:
      absolute_frame_number = (dl_frame_number + 17) %  hyperframe_constant;
      break;
    case 2:
      absolute_frame_number = (dl_frame_number + 21) %  hyperframe_constant;
      break ;
    case 3:
      absolute_frame_number = (dl_frame_number + 26) %  hyperframe_constant;
      break ;
    default:
      break;
  }
  return(absolute_frame_number);
}
void rrbp_control_block_handler_2(rrbp_msg_t pca_direction, gas_id_t gas_id)
{
  control_ack_type_t  ctrl_ack_type;
  access_burst_t access_burst_type;
  ctrl_ack_value_t  ctrl_ack_value = GMAC_CTRL_ACK_NO_CLASH;
  boolean error_in_sending_pca = FALSE;
  uint32  rrbp_value;
  packet_uplink_ack_nack_t     *ul_ack_nack_ptr;
  packet_downlink_assignment_t *dl_assign_ptr;
  uint32 local_rrps_tlli = 0;
  uint32 old_local_rrps_tlli = 0;
  uint8  rbsn_bit  = 0;
  uint8  fs_bit    = 0;
  mac_l1_sig_t                mac_l1_sig;
  mac_l1_pacch_rrbp_t *ml_pacch_rrbp_sig_ptr = &(mac_l1_sig.mac_l1_pacch_rrbp);
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);
  ul_ack_nack_ptr = (packet_uplink_ack_nack_t *)global_data_ptr->csn_dec_data_ptr;
  dl_assign_ptr = (packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr;
  if( ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x08) >> 3) == 1)
  {
    rr_read_public_store(RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE,&access_burst_type, gas_id);
    rr_read_public_store(RRPS_CELL_OPTIONS_CONTROL_ACK_TYPE,&ctrl_ack_type, gas_id);
    rrbp_value = gmac_rrbp_absolute_fn(global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no,
                                       rb_data_ptr->mac_header.type.dl_ctrl.rrbp, gas_id);
    MSG_GERAN_MED_3_G("payload %d channel_msg %d ctrl_ack_value %d",
                      rb_data_ptr->mac_header.payload,global_data_ptr->channel_msg,ctrl_ack_value);
    MSG_GERAN_MED_2_G("access_burst_type %d ctrl_ack_type %d ",access_burst_type,ctrl_ack_type);
    rbsn_bit = (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] >> 7);
    fs_bit =  ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] >> 1) & 0x01);
   
    if((rb_data_ptr->mac_header.payload == 2) && (rbsn_bit == 0) && (fs_bit == 1))
    {
      MSG_GERAN_MED_2_G("MAC info optional ctrl headers used for PWR rbsn_bit %d fs_bit %d",rbsn_bit,fs_bit);
      MSG_GERAN_MED_3_G("MAC info optional ctrl headers 0x%x 0x%x 0x%x",
                        global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0],
                        global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1],
                        global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2]);
      if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] & 0x01) == 1)
      {
        if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1]  >> 7) == 1 )
        {
          ctrl_ack_value = GMAC_CTRL_ACK_VALUE_1;
        }
        else
        {
          ctrl_ack_value = GMAC_CTRL_ACK_VALUE_2;
        }
      }
      else
      {
        MSG_GERAN_MED_0_G( "ERROR: MAC control header 2nd optional octet not present");
        error_in_sending_pca = TRUE;
      }      
    }           
    else if(global_data_ptr->channel_msg == SEGMENTED_MSG)
    {
      if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1] & 0x01) == 1)
      {
        MSG_GERAN_MED_3_G("MAC info optional ctrl headers 0x%x 0x%x 0x%x",
                          global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0],
                          global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1],
                          global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2]);
        if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2] & 0x01) == 0)
        {
          if (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE)
          {
            if(((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2] & 0x3e) >> 1) == global_data_ptr->gmac_local_tfi_store.ul_tfi )
            {
              if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) & global_data_ptr->gmac_local_ul_ts_alloc ) == 0 )
              {
                 error_in_sending_pca = TRUE;
                 MSG_GERAN_HIGH_3_G("GMAC UL TFI rxd on TS NOT in UL TBF, rxd TS:%d, UL TFI:%d, UL TS curr bmap:0x%x", 
                                    global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot, 
                                    global_data_ptr->gmac_local_tfi_store.ul_tfi, 
                                    global_data_ptr->gmac_local_ul_ts_alloc);
              }
              else
              {
                if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1]  >> 7) == 1 )
                {
                  ctrl_ack_value = GMAC_CTRL_ACK_VALUE_1;
                }
                else
                {
                  ctrl_ack_value = GMAC_CTRL_ACK_VALUE_2;
                }              
              }              
            }
            else
            {
              MSG_GERAN_MED_2_G("ERROR: MAC ctrl header ul tfi missmatch. ctrl header tfi =%d, current ul tfi =%d",
                                ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2] & 0x3e) >> 1),
                                global_data_ptr->gmac_local_tfi_store.ul_tfi);
              error_in_sending_pca = TRUE;
            } 
          }
          else
          {
            MSG_GERAN_MED_0_G("MAC info ERROR SEG msg using ul tfi when there is no valid UL TFI");
            error_in_sending_pca = TRUE;
          }
        }
        else
        {        
          if (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid == TRUE)
          {            
            if(((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2] & 0x3e) >> 1) == global_data_ptr->gmac_local_tfi_store.dl_tfi )
            {            
              if ( ( ((uint8)(0x80 >> global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot)) & global_data_ptr->gmac_local_dl_ts_alloc ) == 0 )
              {
                 error_in_sending_pca = TRUE;
                 MSG_GERAN_HIGH_3_G("GMAC DL TFI rxd on TS NOT in DL TBF, rxd TS:%d, DL TFI:%d, DL TS curr bmap:0x%x", 
                                    global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot, 
                                    global_data_ptr->gmac_local_tfi_store.dl_tfi, 
                                    global_data_ptr->gmac_local_dl_ts_alloc);
              }
              else
              {                        
                if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[1]  >> 7) == 1 )
                {
                  ctrl_ack_value = GMAC_CTRL_ACK_VALUE_1;
                }
                else
                {
                  ctrl_ack_value = GMAC_CTRL_ACK_VALUE_2;
                }              
              }              
            }
            else
            {
              MSG_GERAN_MED_2_G( "ERROR: MAC ctrl header dl tfi missmatch. ctrl header tfi =%d, current dl tfi =%d",
                                 ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2] & 0x3e) >> 1),
                                 global_data_ptr->gmac_local_tfi_store.dl_tfi);
              error_in_sending_pca = TRUE;
            }
          }
          else
          {
            MSG_GERAN_MED_0_G("MAC info ERROR SEG msg using dl tfi when there is no valid DL TFI");
            error_in_sending_pca = TRUE;
          }          
        }
      }
      else
      {
        error_in_sending_pca = TRUE;
      }
      if(error_in_sending_pca == FALSE)
      {
        rb_data_ptr->last_segmented_rrbp_info[rb_data_ptr->rrbp_index].control_ack_value = ctrl_ack_value;
        rb_data_ptr->last_segmented_rrbp_info[rb_data_ptr->rrbp_index].absolute_rrbp_fn = gmac_rrbp_absolute_fn(global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no,
                                                                                                                rb_data_ptr->mac_header.type.dl_ctrl.rrbp,
                                                                                                                gas_id);
        rb_data_ptr->last_segmented_rrbp_info[rb_data_ptr->rrbp_index].time_slot = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot;
        rb_data_ptr->rrbp_index = (rb_data_ptr->rrbp_index+1) % 3;
      }
    }
    else if(global_data_ptr->channel_msg == PACKET_UPLINK_ACK_NACK)
    {
      if (ul_ack_nack_ptr->r99_additions_flag == valid)
      {
        if ((ul_ack_nack_ptr->tbf_est == 1) ||
            (ul_ack_nack_ptr->final_alloc == 1) ||
            (ctrl_ack_type == ACCESS_BURST))
        {
          ctrl_ack_value = GMAC_CTRL_ACK_VALUE_0;
        }
        else
        {
          ctrl_ack_value = GMAC_CTRL_ACK_VALUE_3;
        }
      }
      else
      {
        ctrl_ack_value = gmac_check_for_rrbp_clash(rrbp_value,global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot, gas_id);
        if(ctrl_ack_value == GMAC_CTRL_ACK_NO_CLASH)
        {
          ctrl_ack_value = GMAC_CTRL_ACK_VALUE_3;
        }
      }
    }
    else if(global_data_ptr->channel_msg == PACKET_DOWNLINK_ASSIGNMENT)
    {
      if ((dl_assign_ptr->pkt_timing_adv.valid_options == no_timing_params_valid) && (global_data_ptr->mac_mode != TRANSFER_MODE) )
      {
        if((((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0xc0) >> 6) == 1) &&
           (ctrl_ack_type == ACCESS_BURST))
        {
          ctrl_ack_value = GMAC_CTRL_ACK_VALUE_3;
        }
        else
        {
          error_in_sending_pca = TRUE;
        }
      }
      else
      {
        ctrl_ack_value = gmac_check_for_rrbp_clash(rrbp_value,global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot, gas_id);
        if(ctrl_ack_value == GMAC_CTRL_ACK_NO_CLASH)
        {
          ctrl_ack_value = GMAC_CTRL_ACK_VALUE_3;
        }
      }
    }
    else
    {
      ctrl_ack_value = gmac_check_for_rrbp_clash(rrbp_value,global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot, gas_id);
      if(ctrl_ack_value == GMAC_CTRL_ACK_NO_CLASH)
      {
        ctrl_ack_value = GMAC_CTRL_ACK_VALUE_3;
      }
    }
    if(ctrl_ack_type == ACCESS_BURST)
    {
#ifdef FEATURE_GSM_EDTM
        if(rb_data_ptr->gmac_pca_for_ext_seg_msg != GMAC_EXTENDED_PCA_NOT_VALID)
        {
           MSG_GERAN_HIGH_0_G("MAC info burst type is AB, invalid for polled ext segment msg");
           error_in_sending_pca = TRUE;
        }
        else
#endif /*FEATURE_GSM_EDTM*/
        {
          if( access_burst_type == EIGHT_BIT)
          {
            ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = PCA_8_BIT_AB_FORMAT + (uint8)ctrl_ack_value;
            ml_pacch_rrbp_sig_ptr->msg_burst_type = AB_8_BIT;
          }
          else
          {
            ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = PCA_11_BIT_AB_FORMAT_BITS_9_TO_2;
            ml_pacch_rrbp_sig_ptr->pacch_ab_data[1] = PCA_11_BIT_AB_FORMAT_BITS_1 + (uint8)ctrl_ack_value;
            ml_pacch_rrbp_sig_ptr->msg_burst_type = AB_11_BIT;
          }
        }                       
    }
    else
    {
      rr_read_public_store(RRPS_TLLI,&local_rrps_tlli, gas_id);
      if(local_rrps_tlli == (uint32)0xFFFFFFFF)
      {
        rr_read_public_store(RRPS_TLLI_OLD,&old_local_rrps_tlli, gas_id);
        global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.tlli = old_local_rrps_tlli;
      }
      else
      {
        global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.tlli = local_rrps_tlli;
      }
      global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.up_ctrl_ack =  (uint8)ctrl_ack_value;
#ifdef FEATURE_GSM_EDTM
      if(rb_data_ptr->gmac_pca_for_ext_seg_msg != GMAC_EXTENDED_PCA_NOT_VALID)
      {
       MSG_GERAN_HIGH_1_G("GMAC info rcvd segments bit map (rb_data_ptr->gmac_pca_for_ext_seg_msg) = 0x%x", 
                          rb_data_ptr->gmac_pca_for_ext_seg_msg);
       global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.rel5_additions_flag = valid;
       global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.tn_rrbp_flag = not_valid;
       global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.grnti_ext_flag = not_valid;
       global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.rel6_additions_flag = valid;
       global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.ctrl_ack_ext_flag = valid;
       global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.ctrl_ack_extension = rb_data_ptr->gmac_pca_for_ext_seg_msg;
      }
#endif /*FEATURE_GSM_EDTM*/
      if (gcsn1_encoder(PACKET_CONTROL_ACKNOWLEDGE,&(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]),&global_data_ptr->csn_encoder_ul_msg) == PASS)
      {
        if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
        {
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
        }
        else
        {
          ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
        }
        ml_pacch_rrbp_sig_ptr->msg_burst_type = NB;
        (void)mac_log_signalling_message(PACCH_RRBP_CHANNEL,
                                         PACKET_CONTROL_ACKNOWLEDGE,
                                         SIZE_OF_CONTROL_RADIO_BLOCK,
                                         &(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]),
                                         gas_id);
      }
      else
      {
        MSG_GERAN_LOW_0_G( "ERROR: CSN1 ul msg encoder error");
        error_in_sending_pca = TRUE;
      }
    }
    if(error_in_sending_pca == FALSE)
    {
      ml_pacch_rrbp_sig_ptr->absolute_fn = gmac_rrbp_absolute_fn(global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no,
                                                                 rb_data_ptr->mac_header.type.dl_ctrl.rrbp,gas_id);
      ml_pacch_rrbp_sig_ptr->ts_of_dl_msg = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.timeslot;
      ml_pacch_rrbp_sig_ptr->rrbp_msg = pca_direction;
#ifdef FEATURE_GSM_EDTM
      ml_pacch_rrbp_sig_ptr->message_type = PACKET_CONTROL_ACKNOWLEDGE;
#endif /*FEATURE_GSM_EDTM*/
      MSG_GERAN_MED_0_G("MAC sent MAC_L1_PACCH_RRBP");
      mac_l1_sig.mac_l1_pacch_rrbp.message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
      mac_l1_sig.mac_l1_pacch_rrbp.message_header.message_set = MS_MAC_L1;
      gprs_l1_mac_acc_func(&mac_l1_sig,gas_id);
    }
  }
}
void gmac_rrbp_send_pca(uint32 fn,uint8 ts,boolean rrbp_present,uint8 rrbp_value,rrbp_msg_t direction,
                        gas_id_t gas_id)
{
  access_burst_t access_burst_type;
  control_ack_type_t ctrl_ack_type;
  uint8    ctrl_ack_value = 3;   /* ref 4.60 chapter 11.2.2 */
  uint32   local_rrps_tlli;
  mac_l1_sig_t             local_mac_l1_sig;
  mac_l1_pacch_rrbp_t      *ml_pacch_rrbp_sig_ptr = &(local_mac_l1_sig.mac_l1_pacch_rrbp);
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  rr_read_public_store(RRPS_CELL_OPTIONS_ACCESS_BURST_TYPE,&access_burst_type,gas_id);
  rr_read_public_store(RRPS_CELL_OPTIONS_CONTROL_ACK_TYPE,&ctrl_ack_type,gas_id);
  if(ctrl_ack_type == ACCESS_BURST)
  {
    memset(ml_pacch_rrbp_sig_ptr, 0, sizeof(mac_l1_pacch_rrbp_t));
    if(access_burst_type == EIGHT_BIT)
    {
      ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = PCA_8_BIT_AB_FORMAT + ctrl_ack_value;
      ml_pacch_rrbp_sig_ptr->msg_burst_type = AB_8_BIT;
    }
    else
    {
      ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = PCA_11_BIT_AB_FORMAT_BITS_9_TO_2;
      ml_pacch_rrbp_sig_ptr->pacch_ab_data[1] = PCA_11_BIT_AB_FORMAT_BITS_1 + ctrl_ack_value;
      ml_pacch_rrbp_sig_ptr->msg_burst_type = AB_11_BIT;
    }
  }
  else
  {
    rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);
    global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.up_ctrl_ack = ctrl_ack_value;
    global_data_ptr->csn_encoder_ul_msg.type.pkt_ctrl_ack.tlli = local_rrps_tlli;
    if (gcsn1_encoder(PACKET_CONTROL_ACKNOWLEDGE,&(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]),&global_data_ptr->csn_encoder_ul_msg) == PASS)
    {
      if (global_data_ptr->c_flags.mac_ctrl_header_r_bit == SET)
      {
        ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_SET;
      }
      else
      {
        ml_pacch_rrbp_sig_ptr->pacch_ab_data[0] = UL_CNTL_BLOCK_HEADER_R_BIT_NOT_SET;
      }
      ml_pacch_rrbp_sig_ptr->msg_burst_type = NB;
      (void)mac_log_signalling_message(PACCH_RRBP_CHANNEL,
                                       PACKET_CONTROL_ACKNOWLEDGE,
                                       MAX_SIZE_OF_CONTROL_BLOCK_DATA,
                                       &(ml_pacch_rrbp_sig_ptr->pacch_ab_data[1]),
                                       gas_id);
    }
    else
    {
      if(geran_get_nv_recovery_restart_enabled(gas_id))
      {
        MSG_GERAN_ERROR_0_G("MAC error CSN.1 encoder failure");
        MSG_GERAN_ERROR_0_G("GMAC initiated Recovery Restart");
        geran_initiate_recovery_restart(GERAN_CLIENT_GMAC, gas_id);
      } 
      else
      {
        ERR_GERAN_FATAL_0_G("MAC error CSN.1 encoder failure");
      }
    }
  }
  if (rrbp_present)
  {
    ml_pacch_rrbp_sig_ptr->absolute_fn =  gmac_rrbp_absolute_fn(fn,rrbp_value,gas_id);
  }
  else
  {
    ml_pacch_rrbp_sig_ptr->absolute_fn =  fn;
  }
  ml_pacch_rrbp_sig_ptr->ts_of_dl_msg = ts;
  ml_pacch_rrbp_sig_ptr->rrbp_msg = direction;
#ifdef FEATURE_GSM_EDTM
  ml_pacch_rrbp_sig_ptr->message_type = PACKET_CONTROL_ACKNOWLEDGE;
#endif /*FEATURE_GSM_EDTM*/
  MSG_GERAN_MED_0_G("MAC Send MAC_L1_PACCH_RRBP");
  ml_pacch_rrbp_sig_ptr->message_header.message_id = (uint8)MAC_L1_PACCH_RRBP;
  ml_pacch_rrbp_sig_ptr->message_header.message_set = MS_MAC_L1;
  gprs_l1_mac_acc_func(&local_mac_l1_sig,gas_id);
}
#ifdef FEATURE_GSM_EDTM
static boolean gmac_segmented_msg_complete
(
 uint16 current_rcvd_seg_bit_map, 
 uint8  rbsne_of_last_seg,
 gas_id_t gas_id
)
{
 uint8   i;
 uint8   mask =0x1;
 boolean all_segments_rcvd = FALSE;
  if(rbsne_of_last_seg == 0xff)
  {
    all_segments_rcvd = FALSE;
  }
  else
  {
    for(i=0;i<=rbsne_of_last_seg;i++)
    {
      if((current_rcvd_seg_bit_map & (mask << i)) == 0   )
      {
         break;
      }
    }
    if(i == rbsne_of_last_seg)
    {
      all_segments_rcvd = TRUE;
    }
  }
  return(all_segments_rcvd);
}
static uint8 gmac_retrieve_completed_segmented_msg
(
uint8 *ptr_completed_dl_msg,
dl_msg_buffer_t *ptr_current_seg_msg,
gas_id_t gas_id
)
{
  uint8  current_pos_in_dl_msg = 0;
  uint8  i;
  uint8  dl_ctrl_msg_type;
  uint16 csn_msg_length_in_bits;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);
  memset( ptr_completed_dl_msg,0,SIZE_OF_DL_MSG);
  for(i=0;i<rb_data_ptr->ptr_to_current_in_list->final_segment_rbsn;i++)
  {
    if (ptr_current_seg_msg->num_header_octs[i] > SIZE_OF_CONTROL_RADIO_BLOCK)
    { 
      MSG_GERAN_ERROR_1_G("Bad no_header_octets:%d, reject DL message",ptr_current_seg_msg->num_header_octs[i]);
      memset(global_data_ptr->dl_msg,0,SIZE_OF_CONTROL_RADIO_BLOCK);
      global_data_ptr->dl_msg[0] = PACKET_UNUSED_ID << 2;
      break;
    }
    else
    {
      memscpy( &(ptr_completed_dl_msg[current_pos_in_dl_msg]),
               SIZE_OF_DL_MSG - current_pos_in_dl_msg,
               &ptr_current_seg_msg->msg_ext_segment[i][ptr_current_seg_msg->num_header_octs[i]],
               SIZE_OF_CONTROL_RADIO_BLOCK - ptr_current_seg_msg->num_header_octs[i] );
    }
    current_pos_in_dl_msg = current_pos_in_dl_msg + (SIZE_OF_CONTROL_RADIO_BLOCK - ptr_current_seg_msg->num_header_octs[i]);
  }
  size_of_rcvd_dl_control(current_pos_in_dl_msg);
  global_data_ptr->csn_decoder_result = gcsn1_decoder(
    ptr_completed_dl_msg,
    &csn_msg_length_in_bits,
    SIZE_OF_DL_MSG,
    gas_id
  );
  global_data_ptr->csn_dec_result_ptr = &global_data_ptr->csn_decoder_result;
  global_data_ptr->csn_dec_data_ptr = global_data_ptr->csn_decoder_result.data_ptr;
  global_data_ptr->csn_dec_hdr_ptr = global_data_ptr->csn_decoder_result.header_ptr;
  dl_ctrl_msg_type = global_data_ptr->csn_dec_hdr_ptr->message_type;
  gmac_log_dl_channel_msg(dl_ctrl_msg_type, csn_msg_length_in_bits,gas_id);
  return(dl_ctrl_msg_type);
}
static void gmac_update_segmented_vars
(
  mac_ph_data_ind_t *ptr_mac_ph_data, 
  dl_msg_buffer_t   *ptr_current_seg_msg,
  gas_id_t           gas_id
)
{  
  uint8 sp_bit    =0;
  uint8 ac_bit    = 0;
  uint8 fs_bit    = 0;
  uint8 rbsn      = 0xff;
  uint8 rbsne     = 0xff;
  uint8 fse_bit   = 0x0ff;
  uint8 i;
  boolean part_of_extended_seg_msg = FALSE;
  uint8 offset_for_tfi_octet = 0;
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);
  ac_bit = ptr_mac_ph_data->dl_ctrl_block[1] & 0x01;
  fs_bit = (ptr_mac_ph_data->dl_ctrl_block[1] >> 1) & 0x01;
  rbsn = (ptr_mac_ph_data->dl_ctrl_block[1] >> 7);
  if(ac_bit == 1)
  {
    offset_for_tfi_octet = 1;
  }
  else
  {
    offset_for_tfi_octet = 0;
  }
  if (fs_bit == 0 && rbsn == 1)
  {       
     part_of_extended_seg_msg = TRUE;       
     rbsne = (ptr_mac_ph_data->dl_ctrl_block[2 + offset_for_tfi_octet] >> 5);
     ptr_current_seg_msg->num_header_octs[rbsne + 1] = 3 + offset_for_tfi_octet; 
     fse_bit = (ptr_mac_ph_data->dl_ctrl_block[2 + offset_for_tfi_octet] >> 4) & 0x01;
     if(fse_bit == 1)
     {
       ptr_current_seg_msg->final_segment_rbsn = rbsne + 2 ; 
     }
     ptr_current_seg_msg->ext_seg_rcvd_bit_map = (uint16)
         (ptr_current_seg_msg->ext_seg_rcvd_bit_map | ( 0x1 << (rbsne + 1)) );
     memscpy(ptr_current_seg_msg->msg_ext_segment[rbsne + 1],
             sizeof(ptr_current_seg_msg->msg_ext_segment[rbsne + 1]),
             ptr_mac_ph_data->dl_ctrl_block,
             SIZE_OF_CONTROL_RADIO_BLOCK); 
  }
  else
  {
     ptr_current_seg_msg->num_header_octs[rbsn] = 2 + offset_for_tfi_octet;  
     if(fs_bit == 0 && rbsn == 0)
     {
        ptr_current_seg_msg->ext_seg_rcvd_bit_map = 
                   (ptr_current_seg_msg->ext_seg_rcvd_bit_map | 1);
        memscpy(ptr_current_seg_msg->msg_ext_segment[rbsn],
                sizeof(ptr_current_seg_msg->msg_ext_segment[rbsn]),
                ptr_mac_ph_data->dl_ctrl_block,
                SIZE_OF_CONTROL_RADIO_BLOCK); 
     }
     if(fs_bit == 1 && rbsn == 1)
     {
        ptr_current_seg_msg->final_segment_rbsn = rbsn + 1; 
        ptr_current_seg_msg->ext_seg_rcvd_bit_map = 
                (ptr_current_seg_msg->ext_seg_rcvd_bit_map | 2);
        memscpy(ptr_current_seg_msg->msg_ext_segment[rbsn],
                sizeof(ptr_current_seg_msg->msg_ext_segment[rbsn]),
                ptr_mac_ph_data->dl_ctrl_block,
                SIZE_OF_CONTROL_RADIO_BLOCK);                                                  
     }           
  }     
  sp_bit = ((ptr_mac_ph_data->dl_ctrl_block[0] & 0x08) >> 3);
  if((sp_bit ==  1) && (part_of_extended_seg_msg == TRUE))
  {   
    rb_data_ptr->gmac_pca_for_ext_seg_msg = 0;
    for (i=0; i<= 8; i++)
    {
      if (ptr_current_seg_msg->ext_seg_rcvd_bit_map & (0x1 << i))
      {
        rb_data_ptr->gmac_pca_for_ext_seg_msg = (uint16)(rb_data_ptr->gmac_pca_for_ext_seg_msg | (0x1 << (8 - i)));
      }
    }   
  }
  else
  {
    rb_data_ptr->gmac_pca_for_ext_seg_msg = GMAC_EXTENDED_PCA_NOT_VALID;
  }
  MSG_GERAN_HIGH_1_G("GMAC ext_seg_rcvd_bit_map = 0x%x",ptr_current_seg_msg->ext_seg_rcvd_bit_map);
  MSG_GERAN_HIGH_2_G("GMAC info sp_bit = %d rcvd segments bit map (rb_data_ptr->gmac_pca_for_ext_seg_msg) = 0x%x", 
                     sp_bit,rb_data_ptr->gmac_pca_for_ext_seg_msg);
}
static void gmac_initialise_segmented_vars
(
dl_msg_buffer_t *ptr_current_seg_msg,
uint8 ts,
uint8 rti,
gas_id_t gas_id
)
{
  gmac_rb_data_t *rb_data_ptr = gmac_get_rb_data_ptr(gas_id);
  ptr_current_seg_msg->ext_seg_rcvd_bit_map = 0;
  ptr_current_seg_msg->msg_buffer_rti = rti;
  ptr_current_seg_msg->ts_received_on = ts;
  ptr_current_seg_msg->ext_seg_rcvd_bit_map = 0;
  ptr_current_seg_msg->final_segment_rbsn = 0xff;
  ptr_current_seg_msg->ptr_to_next_buff = NULL;
  rb_data_ptr->ptr_to_current_in_list->timer_id = set_t3200(gas_id);
}
#endif /*FEATURE_GSM_EDTM*/
uint8  dl_channel_msg_to_channel_type(gas_id_t gas_id)
{
  uint8 ret_value = DL_CHANNEL_UNDEFINED;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  switch(global_data_ptr->channel_msg)
  {
    case PACKET_PAGING_REQUEST:
    case PACKET_ACCESS_REJECT:
    case PACKET_CELL_CHANGE_ORDER:
    case PACKET_DOWNLINK_ASSIGNMENT:
    case PACKET_MEASUREMENT_ORDER:
    case PACKET_POLLING_REQUEST:
    case PACKET_TIMESLOT_RECONFIGURE:
    case PACKET_UPLINK_ASSIGNMENT:
    case PACKET_DOWNLINK_DUMMY_CNTRL_BLK:
      if(global_data_ptr->mac_mode == TRANSFER_MODE)
      {
        ret_value = DL_PACCH_CHANNEL;
      }
      else
      {
        ret_value = PCCCH_CHANNEL;
      }
      break;
    case PACKET_PRACH_PARAMETERS:
    case PACKET_QUEUEING_NOTIFIACTION:
      ret_value = PCCCH_CHANNEL;
      break;
    case PACKET_UPLINK_ACK_NACK:
    case PACKET_PWR_CTRL_TMNG_ADV:
    case PACKET_TBF_RELEASE:
    case PACKET_PDCH_RELEASE:
    case PACKET_PS_HANDOVER:
    case PACKET_PHYSICAL_INFORMATION:
    case PACKET_CS_COMMAND:
    case PACKET_CS_RELEASE: 
    case PACKET_SERVING_CELL_DATA:
    case PACKET_SERVING_CELL_SI:
    case PACKET_CELL_CHANGE_CONTINUE:
    case PACKET_NEIGHBOUR_CELL_DATA:
      ret_value = DL_PACCH_CHANNEL;
      break;
    case PACKET_PSI1:
    case PACKET_PSI2:
    case PACKET_PSI3:
    case PACKET_PSI3_BIS:
    case PACKET_PSI4:
    case PACKET_PSI5:
    case PACKET_PSI13:
      if(global_data_ptr->mac_mode == TRANSFER_MODE)
      {
        ret_value = DL_PACCH_CHANNEL;
      }
      else
      {
        ret_value = PBCCH_CHANNEL;
      }
      break;
    default:
      MSG_GERAN_HIGH_1_G("MAC INfo LOG ON DEMAND: OTA not part of logged msg_type %d",global_data_ptr->channel_msg);
      ret_value = DL_CHANNEL_UNDEFINED;
      break;
  }
  return (ret_value);
}
void gmac_log_dl_channel_msg(uint8 msg_type, uint16 msg_length, gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  switch(global_data_ptr->channel_msg)
  {
    case PACKET_DOWNLINK_DUMMY_CNTRL_BLK:
      break;
    default:
      (void)mac_log_signalling_message(dl_channel_msg_to_channel_type(gas_id),
                                       msg_type,
                                       (byte)NUM_BITS_TO_BYTES(msg_length),
                                       global_data_ptr->dl_msg,
                                       gas_id);
      break;
  }
}
