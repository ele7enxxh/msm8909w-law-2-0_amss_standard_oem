

/*!
  @file
  rfnv_tdscdma_b40_b.c

  @brief

  @details
  This module defines nv tables and functions to deal with NV items for TDSCDMA B40_B band
  unit.

*/
/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================

                           INCLUDE FILES

===========================================================================*/
#if 0
#include <stringl/stringl.h>

#include "rfcommon_nv.h"
#include "rfnv_tdscdma.h"
#include "rfnv_tdscdma_b40_b.h"
#include "msg.h"
#include "modem_mem.h"
#include "rfc_card.h"
#include "rfcommon_core_txlin.h"
#include "rf_tdscdma_core_util.h"

//#include "rflte_ext_mc.h"
//#include "rfcommon_nv.h"
//#include "msg.h"
//#include "rfc_card.h"
//#include "modem_mem.h"
//#include "rfcommon_core_txlin.h"
//#include "rfnv_tdscdma.h"
//#include "rfnv_tdscdma_b40_b.h"


/*===========================================================================

                              CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

                                DATA DEFINITIONS

===========================================================================*/
/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF NV item identifier array for LTE B40_B band.
  This array defines RF NV items that are supported and will be read.

*/

const rfcommon_rfnv_items_list_type rfnv_tdscdma_b40_b_tx_items_list[ ] =
{
    /* Expected HDET versus AGC */
    {RFNV_TDSCDMA_B40_B_EXP_HDET_VS_AGC_I, NV_ITEM_ATTRIB_CRTCL_BMSK},
    /* limit tx power versus freq */
    {RFNV_TDSCDMA_B40_B_TX_LIM_VS_FREQ_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
    /* Multi-Lin structure */
    {RFNV_TDSCDMA_B40_B_TX_MULTI_LIN_DATA_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
};


/*----------------------------------------------------------------------------*/
/*!
  @brief
  TDSCDMA B40_B RF NV tbl with default values.
  If the non-calibrated items are not read from NV, the default values are used.

*/

//rfnv_tdscdma_tx_tbl_type rfnv_tdscdma_b40_b_tbl;


/*----------------------------------------------------------------------------*/
/*!
  @brief 
  TDSCDMA B40 RF NV tbl with default values.
  If the non-calibrated items are not read from NV, the default values are used.

*/
rfnv_tdscdma_tx_tbl_type rfnv_tdscdma_b40_b_tx_tbl = 
{
  /* tx calibration channels list */
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  /* UE maximum TX power */
  0,  
  /* tx delays and update agc time */
  {0,0},
  {0,0},
  0,
  /* pa range update time */
  {0,0},
  /* tx rotator angles */
  {0,0,0,0},
  /* PA Compensation UP/Down for PA switch threshold from gain states, max num -1  */
  {
    {0,0},
    {0,0},
    {0,0}
  },
   /* Tx linearizer versus temperature */
  {  
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
  },
   /* PA Range Map */
   {0,0,0,0},
   /* timer hysterisis for swithcing from high to low gain state*/
   0,
   
   /* rise/fall value for PA switch threshold from Linearizer gain states, max num -1 */
  {
    {0,0},
    {0,0},
    {0,0}
  },
   /* offset Tx power limit versus temperature */
   {0,0,0,0,0,0,0,0},

    /* pa switch point shift */   
   {0,0,0,0,0,0,0},   
   /*  MPR power backoff */
   {0,5,10,15,20,25,15},  
   /*  MPR power backoff V2*/
   {0,5,10,15,20,25,5,10,15,20,25,5,10,15,20,25,15},     
   /* MPR analog power backoff */
   {0,0,0,0,0,0,0},
   /* MPR digital power backoff */
   {0,0,0,0,0,0,0},
  {
   {
    /* linearizer table of 64 entry for pa range 0 */
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* linearizer table of 64 entry for pa range 1*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* linearizer table of 64 entry for pa range 2*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* linearizer table of 64 entry for pa range 3*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   },
   {
     /* tx gain table of 64 entry for linearizer of pa range 0*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* tx gain table of 64 entry for linearizer of pa range 1*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* tx gain table of 64 entry for linearizer of pa range 2*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* tx gain table of 64 entry for linearizer of pa range 3*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   }
  },
   /* tx power freq compensation per pa range */
   {   
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   },   
   /* Expected HDET versus AGC */
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},   
    /* tx limiting freq compensation for high PA range */
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

    /* antenna tuner tx */
  {  {0,0,0,0},
     {0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  },
};


/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function copies one RF NV data item, as specified by "item_code/
  identifier" into an element of the RF NV item tbl for TDSCDMA B40_B band.
  This function must be called during initialization for each and every RF
  NV item of TDSCDMA B40_B band

  @param item_code Item to put
  @param *rfnv_item_ptr Pointer where to get the item

*/
void rfnv_tdscdma_b40_b_copy_tx_item
(
rfnv_item_id_enum_type  item_code,          /* Item to put */
rfnv_item_type          *rfnv_item_ptr      /* Pointer where to get the item */
)
{
//    tx_band_cal_type * band_cal_type_buf = NULL;
//    boolean nv_load_status = TRUE;
    //rfc_smps_nv_type smps_item;
    //smps_item.mode = RFCOM_TDSCDMA_MODE;
    //smps_item.band = RF_TDSCDMA_B40_B_BAND;

    /* read RF NV item based on its identifier/item code */
    switch ( item_code )
    {
    case RFNV_TDSCDMA_B40_EXP_HDET_VS_AGC_I:
       memscpy((void*)&rfnv_tdscdma_b40_b_tx_tbl.exp_hdet_vs_agc[0],sizeof(rfnv_tdscdma_b40_b_tx_tbl.exp_hdet_vs_agc),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
       break;

	case RFNV_TDSCDMA_B40_TX_LIM_VS_FREQ_I:
	   memscpy((void*)&rfnv_tdscdma_b40_b_tx_tbl.tx_limit_vs_freq[0],sizeof(rfnv_tdscdma_b40_b_tx_tbl.tx_limit_vs_freq),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
	   break;

    case RFNV_TDSCDMA_B40_B_TX_MULTI_LIN_DATA_I:
       // tdscdma multi_lin is handled separately in rf_tdscdma_core_txlin_init()
        break;

    default:
        MSG_ERROR( "Unsupported RF NV item identifier: %d", item_code, 0,0 );

    } /* switch ( item_code )  */

} /* end of rfnv_tdscdma_b40_b_copy_item */


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the number of items in the RF TDSCDMA B40_B NV list.

  @return uint16 Number of item in the TDSCDMA B40_B NV list.

*/
uint16 rfnv_tdscdma_b40_b_tx_list_size_get( void )
{
    return( ARR_SIZE( rfnv_tdscdma_b40_b_tx_items_list ) );
}

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the pointer to RF TDSCDMA B40_B NV table

  @return Pointer of rfnv_tdscdma_nv_tbl_type

*/
void *rfnv_tdscdma_b40_b_tx_item_table_get( void )
{
    return( (void *) &rfnv_tdscdma_b40_b_tx_tbl );
}


/*--------------------------------------------------------------------------*/
#endif