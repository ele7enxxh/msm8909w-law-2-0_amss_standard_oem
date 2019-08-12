/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R

                 P H Y S I C A L  L A Y E R  P R O T O C O L

       
GENERAL DESCRIPTION
  This module provides supporting API for SCM module to handle AT-init Session
    
EXTERNALIZED FUNCTIONS
    
  hdrscmphy_is_attrib_simple 

  hdrscmphy_get_attrib_offset_and_size

  hdrscmphy_get_attrib_verno_index

  hdrscmphy_prop_max_transmit_power
   
INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  Copyright (c) 2007 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/13   ukl     Reduce Code Duplication HDR-CP
02/17/13   arm     Supported NV enable/disable for network dtx attributes.
12/10/08   smd     Replaced assert.h with amssassert.h
09/03/08   etv     Added signaling support for DTX feature.
09/15/07   etv     Fixed compiler warnings.
08/15/07   etv     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/

#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"
#include "err.h"
#include "amssassert.h"

#include "hdrscmphy.h"

#include "hdrdebug.h"
#include "hdrdsm.h"
#include "hdrscp.h"
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrbit.h"
#include "hdrutil.h"
#include "hdrhmp.h"

#include "hdrscm.h"
#include "hdrscmdb.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
/* Proposed value for Maximum Transmit Power */
#define HDRSCMPHY_DESIRED_MAX_TRANSMIT_POWER                    0x08
#define HDRSCMPHY_DESIRED_DTX_SUPPORT                           1

/* Size of the simple attribute value in bytes */
#define HDRSCMPHY_SIMPLE_ATTRIB_SIZE_BYTES                      1

/* Size of PHY-3 attribute ID */
#define HDRSCMPHY_ATTRIB_ID_SIZE                                8

/* Byte length (Length field in AR excluding itself) includes 
   Attribute ID size (1 byte) and Value size (1 byte) */
#define HDRSCMPHY_SIMPLE_AR_BYTE_LENGTH                         2

/* Default MaxRLTxPwrDiff */
#define HDRSCMPHY_DEFAULT_MAX_RL_TX_PWR_DIFF                    0x6
#define HDRSCMPHY_DEFAULT_DTX_SUPPORT                           0x0

/*---------------------------------------------------------------------------
  Sizes of the complex attributes and its fields in bits 
----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------
 Section: 12.1.7.2.1: AdditionalCarriersInitTxPower (ACITXPWR) Attribute
 ----------------------------------------------------------------------*/
/* AdditionalCarriersAssigned */
#define HDRSCMPHY_ACITXPWR_CARR_ASSIGNED_SIZE                       1
/* ReferenceCDMAChannel */
#define HDRSCMPHY_ACITXPWR_REFERENCE_CDMA_CHAN_SIZE                 24
/* NumAdditionalRLCDMAChannelsAssigned */
#define HDRSCMPHY_ACITXPWR_NUM_ADDL_RL_CDMA_CHAN_ASSIGNED_SIZE      4
/* ReverseCDMAChannel */
#define HDRSCMPHY_ACITXPWR_REVERSE_CDMA_CHANNEL_SIZE                24
/* NumSectors */
#define HDRSCMPHY_ACITXPWR_NUM_SECTORS_SIZE                         3
/* PilotPNOffset*/
#define HDRSCMPHY_ACITXPWR_PILOT_PN_OFFSET_SIZE                     9
/* TxInitAdjustRelativeToReferenceCDMAChannel */
#define HDRSCMPHY_ACITXPWR_REL_TX_INIT_ADJUST_SIZE                  5

/* Minimum size of AdditionalCarriersInitTxPower Attribute
   If the AdditionalCarriersAssigned field is set to zero, AT shall ignore
   the rest of the fields in this attribute  */
#define HDRSCMPHY_ACITXPWR_MIN_SIZE() (HDRSCMPHY_ACITXPWR_CARR_ASSIGNED_SIZE)

/*----------------------------------------------------------------
  Section: 12.1.7.2.2: ReversePilotTransmitSlots (RPTXS) Attribute
-----------------------------------------------------------------*/
/* ReversePilotTransmitSlotsEnabled */
#define HDRSCMPHY_RPTXS_REV_PILOT_TX_SLOTS_ENABLED_SIZE             1
/* ReversePilotTransmitSlotsDuration */
#define HDRSCMPHY_RPTXS_REV_PILOT_TX_SLOTS_DURATION_SIZE            3
/* ReversePilotTransmitSlotsPeriod */
#define HDRSCMPHY_RPTXS_REV_PILOT_TX_SLOTS_PERIOD_SIZE              4

/* Minimum size of ReversePilotTransmitSlots attribute 
   If the ReversePilotTransmitSlotsEnabled attribute is set to zero, AT
   shall ignore rest of the fields in this attribute */
#define HDRSCMPHY_RPTXS_MIN_SIZE()                                  \
          HDRSCMPHY_RPTXS_REV_PILOT_TX_SLOTS_ENABLED_SIZE

/* Maximum Value for ReversePilotTransmitSlotsDuration */
#define HDRSCMPHY_RPTXS_MAX_DURATION                                3

/* Maximum Value for ReversePilotTransmitSlotsPeriod */
#define HDRSCMPHY_RPTXS_MAX_PERIOD                                  4

typedef struct
{
  uint8 min;    /* Minimum allowed value for the simple attribute */
  uint8 max;    /* Maximum allowed value for the simple attribute */

} hdrscmphy_simple_attrib_valid_range_type;

/*------------------------------------------------------------------------
                                MACROS
 -----------------------------------------------------------------------*/

/* Find out if the attribute identified by "attrib_id" is simple or complex */
#define HDRSCMPHY_ATTRIB_IS_SIMPLE(attrib_id) (((attrib_id) & 0xF0) == 0xF0)

/* Maps the simple attribute ID to an index to be used in 
   hdrscmphy_simple_attrib_valid_range table */
#define HDRSCMPHY_SIMPLE_ATTRIB_ID_TO_IND(attrib_id)                   \
                            ((attrib_id) - HDRSCMPHY_FIRST_SIMPLE_ATTRIB)

/*------------------------------------------------------------------------
                              LOOKUP TABLES
 -----------------------------------------------------------------------*/

/* This table lists the valid ranges of simple attributes ordered based 
   on index got from HDRSCMPHY_SIMPLE_ATTRIB_ID_TO_IND macro. In order
   to add a new simple attribute, follow these steps.
   1. Update the attrib_id_type enum
   2. Place the range in LUT below at the location that maps to index
      given by the HDRSCMPHY_SIMPLE_ATTRIB_ID_TO_IND macro. (Most 
      likely in the beginning shifting all other attributes down!) */
hdrscmphy_simple_attrib_valid_range_type 
  hdrscmphy_simple_attrib_valid_range [HDRSCMPHY_MAX_NUM_SIMPLE_ATTRIBS] =
{
  {0x00, 0x01},       /* 0xF9 DTXSupport */
  {0x00, 0x00},       /* 0xFA MACIndexBits */
  {0x00, 0x00},       /* 0xFB DTXRRIMode */
#ifdef FEATURE_HDR_REVB_DTX_MODE
  {0x00, 0x01},       /* 0xFC DTXMode */
#else
  {0x00, 0x00},       /* 0xFC DTXMode */
#endif /* FEATURE_HDR_REVB_DTX_MODE */
  {0x00, 0x00},       /* 0xFD MaxTransmitPower */
  {0x06, 0x06},       /* 0xFE MaxRLTxPwrDiff */
  {0x00, 0x00}        /* 0xFF MultiCarrierCapability */
};

/* This table lists the GAUPable attributes */
hdrscmphy_attrib_id_enum_type 
  hdrscmphy_gaupable_attribs[HDRSCMPHY_MAX_NUM_GAUPABLE_ATTRIBS] = 
{
  HDRSCMPHY_MAX_RL_TX_POWER_DIFF,
  HDRSCMPHY_DTX_MODE,
  HDRSCMPHY_DTX_RRI_MODE,
  HDRSCMPHY_ADDITIONAL_CARRIERS_INIT_TX_POWER
};


 /* EJECT */
/*===========================================================================
 
                  DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/

static struct
{
  uint8 trans_id;
    /* Transaction ID for AT-init Sesssion Negotiation */

  hdrscmphy_additional_carriers_init_tx_pwr_type init_tx_pwr;

  hdrscmphy_reverse_pilot_transmit_slots_type rev_pilot_tx_slots;

  uint8 max_transmit_power;

  uint8 dtx_support;

  boolean is_net_dtx_enabled;

} hdrscmphy;


/* EJECT */  
/*------------------------------------------------------------------------
                              LOOKUP TABLES
 -----------------------------------------------------------------------*/


/*===========================================================================

                         LOCAL FUNCTION DEFINITIONS
                    
===========================================================================*/

/*===========================================================================

FUNCTION HDRSCMPHY_DUMP_ADDL_CARR_INIT_TX_PWR                   LOCAL

DESCRIPTION
  Dump the F3 message for AdditionalCarriersInitTxPower Attribute

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmphy_dump_addl_carr_init_tx_pwr(void)
{
  hdrscmphy_additional_rl_cdma_channels_type *rl_chan_ptr = NULL;
    /* Convenience Pointer */

  uint8 chan_idx;
    /* Channel Index */

  uint8 sec_idx;
    /* Sector Index */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                 "AdditionalCarriersAssigned: %d",
                 hdrscmphy.init_tx_pwr.additional_carriers_assigned);

  /* If additional Carriers assigned, */
  if (hdrscmphy.init_tx_pwr.additional_carriers_assigned == 1)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
                   "Ref. CDMA Chan: 0x%x, NumAddlRLChans: %d",
                   hdrscmphy.init_tx_pwr.reference_cdma_channel,
                   hdrscmphy.init_tx_pwr.num_additional_rl_channels);

    /* For each additional RL CDMA Channel assigned */
    for ( chan_idx = 0; 
          chan_idx < hdrscmphy.init_tx_pwr.num_additional_rl_channels; 
          ++chan_idx )
    {
      /* Initialize the convenience pointer */
      rl_chan_ptr = &hdrscmphy.init_tx_pwr.additional_rl_chans[chan_idx];

      HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
                     "RLCDMAChan: 0x%x, NumSectors: %d",
                     rl_chan_ptr->reverse_cdma_channel,
                     rl_chan_ptr->num_sectors);

      /* For each associated sector, */
      for (sec_idx = 0; sec_idx < rl_chan_ptr->num_sectors; ++sec_idx)
      {
        HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
                       "PilotPNOffset: 0x%x, RelTxInitAdjust: 0x%x",
                       rl_chan_ptr->sectors[sec_idx].pilot_pn_offset,
                       rl_chan_ptr->sectors[sec_idx].relative_tx_init_adjust);

      } /* for each sector... */

    } /* for each additional RL CDMA Channel... */

  } /* if assigned */

} /* hdrscmphy_dump_addl_carr_init_tx_pwr */

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMFMAC_ADDL_CARR_INIT_TX_PWR_IS_VALID                LOCAL

DESCRIPTION
  Check if the AdditionalCarriersInitTxPwr Attribute is valid 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmphy_addl_carr_init_tx_pwr_is_valid(void)
{

  /* Accept only default value */
  return ((hdrscmphy.init_tx_pwr.additional_carriers_assigned == 0)? 
          E_SUCCESS: E_OUT_OF_RANGE);

} /* hdrscmphy_addl_carr_init_tx_pwr_is_valid */

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_PARSE_ADDL_CARR_INIT_TX_POWER                LOCAL

DESCRIPTION
  This is a generic interface to process the AdditionalCarriersInitTxPower 
  Attribute.

  Reference: Section: 12.1.7.2.1

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE
  E_SUCCESS - If the attribute is parsed successfully
  E_NO_DATA - If there is a parsing error
  E_OUT_OF_RANGE - If the parsed value is out of valid range.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmphy_parse_addl_carr_init_tx_power
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{
  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM chain */

  uint16 value_id;
    /* Value ID of the complex attribute AdditionalCarriersInitTxPower */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Error Code */

  uint8 rl_cdma_chan_idx = 0;
    /* Loop index for each additional RL CDMA channel */

  uint8 sector_idx = 0;
    /* Loop index for each sector */

  hdrscmphy_additional_rl_cdma_channels_type *rl_cdma_chan_ptr = NULL;
    /* Convenience pointer */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to first Value Id */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
   * set of AdditionalCarriersInitTxPower complex Attribute Value */
  while ( ( attrib_offset + p_aip->value_id_size + 
            HDRSCMPHY_ACITXPWR_MIN_SIZE() ) <= p_aip->attrib_len )
  {
    /* Initialize the error code to success since we know that we have 
       at least one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint16 */
    err_no = HDRBIT_UNPACK16( item_ptr, 
                              &(value_id), 
                              (p_aip->attrib_offset + attrib_offset), 
                              p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of AdditionalCarriersInitTxPwr Attrib", 
          0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to
     * start of  AdditionalCarriersAssigned field */ 
    attrib_offset += p_aip->value_id_size;

    /* Unpack AdditionalCarriersAssigned */
    err_no = HDRBIT_UNPACK8(
              item_ptr, 
              &(hdrscmphy.init_tx_pwr.additional_carriers_assigned),
              (p_aip->attrib_offset + attrib_offset),
              HDRSCMPHY_ACITXPWR_CARR_ASSIGNED_SIZE);

    /* If error unpacking AdditionalCarriersAssigned */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking AdditionalCarriersAssigned field of ACInitTxPwr",
          0, 0, 0);
      break;
    }

    /* Since AdditionalCarriersAssigned was parsed successfully, update the 
       attribute offset to start of ReferenceCDMAChannelSize */
    attrib_offset += HDRSCMPHY_ACITXPWR_CARR_ASSIGNED_SIZE;

    /* If the AdditionalCarriersAssigned field is set to zero, the access
       terminal shall ignore the rest of the fields in this attribute */
    if (hdrscmphy.init_tx_pwr.additional_carriers_assigned == 1)
    {
      /* Unpack ReferenceCDMAChannel */
      err_no = HDRBIT_UNPACK32(item_ptr, 
                               &(hdrscmphy.init_tx_pwr.reference_cdma_channel),
                               (p_aip->attrib_offset + attrib_offset),
                               HDRSCMPHY_ACITXPWR_REFERENCE_CDMA_CHAN_SIZE);

      /* If error unpacking ReferenceCDMAChannel, */
      if (err_no != E_SUCCESS)
      {
        ERR("Error unpacking ReferenceCDMAChannel field of ACInitTxPwr",
            0, 0, 0);
        break;
      }

      /* Since ReferenceCDMAChannel was parsed successfully, update the 
         attribute offset to start of NumAdditionalRLCDMAChannelsAssigned */
      attrib_offset += HDRSCMPHY_ACITXPWR_REFERENCE_CDMA_CHAN_SIZE;

      /* Unpack NumAdditionalRLCDMAChannelsAssigned */
      err_no = HDRBIT_UNPACK8(
                item_ptr, 
                &(hdrscmphy.init_tx_pwr.num_additional_rl_channels), 
                (p_aip->attrib_offset + attrib_offset), 
                HDRSCMPHY_ACITXPWR_NUM_ADDL_RL_CDMA_CHAN_ASSIGNED_SIZE);

      /* If error unpacking NumAdditionalRLCDMAChannelsAssigned */
      if (err_no != E_SUCCESS)
      {
        ERR("Error unpacking NumAdditionalRLCDMAChannels of ACInitTxPwr",
            0, 0, 0);
        break;
      }

      /* Since NumAdditionalRLCDMAChannelAssigned was parsed successfully,
         update the offset to start of ReverseCDMAChannel field */
      attrib_offset += HDRSCMPHY_ACITXPWR_NUM_ADDL_RL_CDMA_CHAN_ASSIGNED_SIZE;

      /* For each additional RL CDMA Channel assigned... */
      for ( rl_cdma_chan_idx = 0; 
            rl_cdma_chan_idx < hdrscmphy.init_tx_pwr.num_additional_rl_channels;
            ++rl_cdma_chan_idx )
      {
        /* Initialize the convenience pointer */
        rl_cdma_chan_ptr = 
          &(hdrscmphy.init_tx_pwr.additional_rl_chans[rl_cdma_chan_idx]);

        /* Unpack ReverseCDMAChannel[rl_cdma_chan_idx] field */
        err_no = HDRBIT_UNPACK32(
                  item_ptr,
                  &(rl_cdma_chan_ptr->reverse_cdma_channel),
                  (p_aip->attrib_offset + attrib_offset),
                  HDRSCMPHY_ACITXPWR_REVERSE_CDMA_CHANNEL_SIZE);

        if (err_no != E_SUCCESS)
        {
          ERR("Error unpacking ReverseCDMAChannel of ACInitTxPwr", 0, 0, 0);
          break;
        }

        /* Since ReverseCDMAChannel was parsed successfully, update the offset
           to start of NumSectors field */
        attrib_offset += HDRSCMPHY_ACITXPWR_REVERSE_CDMA_CHANNEL_SIZE;

        /* Unpack NumSectors */
        err_no = HDRBIT_UNPACK8(item_ptr,
                                &(rl_cdma_chan_ptr->num_sectors),
                                (p_aip->attrib_offset + attrib_offset),
                                HDRSCMPHY_ACITXPWR_NUM_SECTORS_SIZE);

        /* If error unpacking, */
        if (err_no != E_SUCCESS)
        {
          ERR("Error unpacking NumSectors of ACInitTxPwr", 0, 0, 0);
          break;
        }

        /* Since the NumSectors was parsed successfully, update the offset 
           to start of PilotPNOffset of first sector */
        attrib_offset += HDRSCMPHY_ACITXPWR_NUM_SECTORS_SIZE;

        /* For each sector... */
        for (sector_idx = 0; sector_idx < rl_cdma_chan_ptr->num_sectors;
              ++sector_idx)
        {
          /* Unpack PilotPNOffset */
          err_no = HDRBIT_UNPACK16(
                    item_ptr, 
                    &(rl_cdma_chan_ptr->sectors[sector_idx].pilot_pn_offset),
                    (p_aip->attrib_offset + attrib_offset),
                    HDRSCMPHY_ACITXPWR_PILOT_PN_OFFSET_SIZE);

          /* If error unpacking */
          if (err_no != E_SUCCESS)
          {
            ERR("Error unpacking PilotPNOffset of ACInitTxPwr", 0, 0, 0);
            break;
          }

          /* Shift offset to start of 
             TxInitAdjustRelativeToReferenceCDMAChannel */
          attrib_offset += HDRSCMPHY_ACITXPWR_PILOT_PN_OFFSET_SIZE;

          /* Unpack TxInitAdjustRelativeToReferenceCDMAChannel */
          err_no = HDRBIT_UNPACK8( 
            item_ptr, 
            &(rl_cdma_chan_ptr->sectors[sector_idx].relative_tx_init_adjust), 
            (p_aip->attrib_offset + attrib_offset), 
            HDRSCMPHY_ACITXPWR_REL_TX_INIT_ADJUST_SIZE);

          if (err_no != E_SUCCESS)
          {
            ERR("Error unpacking RelTxInitAdjust of ACInitTxPwr", 0, 0, 0);
            break;
          }

          attrib_offset += HDRSCMPHY_ACITXPWR_REL_TX_INIT_ADJUST_SIZE;

        } /* For each sector */

        /* If there was an error in the inner for-loop above break out of 
           outer for loop */
        if (err_no != E_SUCCESS)
        {
          break;
        }
                                 
      } /* For each additional RL CDMA Channel assigned */

      /* If there was an error in the for-loop above break out of while */
      if (err_no != E_SUCCESS)
      {
        break;
      }

      /* No error occured so far, proceed to check if the parsed attribute
         if valid */


      /* If the AdditionalCarriersInitTxPower attribute, just parsed is valid, 
         break out of loop. Otherwise continue looking for a valid one in 
         the message */ 
      if ( (err_no = hdrscmphy_addl_carr_init_tx_pwr_is_valid()) != 
            E_OUT_OF_RANGE )
      {
        /* F3 message */
        hdrscmphy_dump_addl_carr_init_tx_pwr();

        /* Update the Accepted attribute information */
        a_aip->is_accepted = TRUE;
        a_aip->value_id = value_id;
        a_aip->complex_val_ptr = (void*) &(hdrscmphy.init_tx_pwr);
        a_aip->complex_val_size = 
          sizeof(hdrscmphy_additional_carriers_init_tx_pwr_type);

        break;

      } /* If valid */

    } /* if additional_carriers_assigned == 1 */

    /* Adjust the attrib_offset for reserved bits if any */
    HDRSCM_ALIGN_TO_NEXT_OCTET(attrib_offset);

  } /* while */

  return err_no;

} /* hdrscmphy_parse_addl_carr_init_tx_power */

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_DUMP_REV_PILOT_TX_SLOTS                   LOCAL

DESCRIPTION
  Dump the F3 message for ReversePilotTransmitSlots Attribute

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrscmphy_dump_rev_pilot_tx_slots(void)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "ReversePilotTransmitSlotsEnabled: %d",
                 hdrscmphy.rev_pilot_tx_slots.enabled);

  if (hdrscmphy.rev_pilot_tx_slots.enabled == 1)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
                   "ReversePilotTransmitSlots Duration: %d, Period: %d",
                   hdrscmphy.rev_pilot_tx_slots.duration,
                   hdrscmphy.rev_pilot_tx_slots.period);
  }

} /* hdrscmphy_dump_rev_pilot_tx_slots */

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_REV_PILOT_TX_SLOTS_IS_VALID                LOCAL

DESCRIPTION
  Check if the ReversePilotTxSlots Attribute is valid 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  E_SUCCESS - If the Attribute is valid.
  E_OUT_OF_RANGE - otherwise. 

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmphy_rev_pilot_tx_slots_is_valid(void)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Only default values supported */
  return ((hdrscmphy.rev_pilot_tx_slots.enabled == 0)? 
          E_SUCCESS: E_OUT_OF_RANGE);

} /* hdrscmphy_rev_pilot_tx_slots_is_valid */

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_PARSE_REVERSE_PILOT_TRANSMIT_SLOTS    LOCAL

DESCRIPTION
  This is a generic interface to process the ReversePilotTransmitSlots 
  Attribute.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmphy_parse_reverse_pilot_transmit_slots
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip
)
{

  uint16 attrib_offset;
    /* Offset to the current byte being processed from the start of DSM 
       chain */

  uint16 value_id;
    /* Value ID of the complex attribute ReversePilotTransmitSlots */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Error Code */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attribute offset to first Value Id */
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;

  /* While there are enough bits left in the Attribute Record for one more
   * set of ReversePilotTransmitSlots Complex Attribute Value */
  while ( ( attrib_offset + p_aip->value_id_size + 
            HDRSCMPHY_RPTXS_MIN_SIZE() ) <= p_aip->attrib_len )
  {
    /* Initialize the error code to success since we know that we have 
       at least one attrib value in the Attrib Record */
    err_no = E_SUCCESS;

    /* Unpack the Value ID into a uint16 */
    err_no = HDRBIT_UNPACK16( item_ptr, 
                              &(value_id), 
                              (p_aip->attrib_offset + attrib_offset), 
                              p_aip->value_id_size );

    /* If error unpacking value ID, break */
    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Value ID of ReversePilotTransmitSlots Attrib", 
          0,0,0);
      break;
    }

    /* Since Value Id was parsed sucessfully, Update the attribute offset to
     * start of ReversePilotTransmitSlotsEnabled field */ 
    attrib_offset += p_aip->value_id_size;

    err_no = HDRBIT_UNPACK8(item_ptr,
                            &(hdrscmphy.rev_pilot_tx_slots.enabled),
                            (p_aip->attrib_offset + attrib_offset),
                            HDRSCMPHY_RPTXS_REV_PILOT_TX_SLOTS_ENABLED_SIZE);

    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking ReversePilotTransmitSlotsEnabled Field", 0, 0, 0);
      break;
    }

    /* Shift offset to start of ReversePilotTransmitSlotsPeriod */
    attrib_offset += HDRSCMPHY_RPTXS_REV_PILOT_TX_SLOTS_ENABLED_SIZE;

    if (hdrscmphy.rev_pilot_tx_slots.enabled == 1)
    {

      err_no = HDRBIT_UNPACK8(item_ptr,
                              &(hdrscmphy.rev_pilot_tx_slots.duration),
                              (p_aip->attrib_offset + attrib_offset),
                              HDRSCMPHY_RPTXS_REV_PILOT_TX_SLOTS_DURATION_SIZE);

      if (err_no != E_SUCCESS)
      {
        ERR("Error unpacking ReversePilotTransmitSlotsDuration Field", 0, 0, 0);
        break;
      }

      /* Shift offset to start of ReversePilotTransmitSlotsPeriod */
      attrib_offset += HDRSCMPHY_RPTXS_REV_PILOT_TX_SLOTS_DURATION_SIZE;

      err_no = HDRBIT_UNPACK8(item_ptr,
                              &(hdrscmphy.rev_pilot_tx_slots.period),
                              (p_aip->attrib_offset + attrib_offset),
                              HDRSCMPHY_RPTXS_REV_PILOT_TX_SLOTS_PERIOD_SIZE);

      if (err_no != E_SUCCESS)
      {
        ERR("Error unpacking ReversePilotTransmitSlotsPeriod Field", 0, 0, 0);
        break;
      }

      attrib_offset += HDRSCMPHY_RPTXS_REV_PILOT_TX_SLOTS_PERIOD_SIZE;

      /* No error occured so far, proceed to check if the parsed attribute
         if valid */

      /* If the ReversePilotTransmitSlots attribute, just parsed is valid, 
         break out of loop. Otherwise continue looking for a valid one in 
         the message */ 
      if ( (err_no = hdrscmphy_rev_pilot_tx_slots_is_valid()) != 
            E_OUT_OF_RANGE )
      {
        /* F3 message */
        hdrscmphy_dump_rev_pilot_tx_slots();

        /* Update the Accepted attribute information */
        a_aip->is_accepted = TRUE;
        a_aip->value_id = value_id;
        a_aip->complex_val_ptr = (void*) &(hdrscmphy.rev_pilot_tx_slots);
        a_aip->complex_val_size = 
          sizeof(hdrscmphy_reverse_pilot_transmit_slots_type);

        break;

      } /* If valid */

    } /* if enabled */

    HDRSCM_ALIGN_TO_NEXT_OCTET(attrib_offset); 

  } /* while */

  return err_no;

} /* hdrscmphy_parse_reverse_pilot_transmit_slots */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_PROPOSE

DESCRIPTION
  This routine is used to by PHY to propose Maximum Transmit Power and 
  DTX Support during initial AT-init Session Configuration.

DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  E_SUCCESS - If a ConfigRequest Update was sent successfully.
  E_NOT_AVAILABLE - If we didn't have a free Rsp timer to send the req msg.

SIDE EFFECTS
  None.

===========================================================================*/
static hdrerrno_enum_type hdrscmphy_propose( void )
{

  uint8 timer_id;
      /* Reserved timer Id */

  dsm_item_type * send_msg_ptr = NULL;
    /* Pointer to Config req. message to send */

  uint8 attrib_offset = 0;
    /* Attribute field offset from start of DSM item */

  hdrerrno_enum_type err_no = E_NOT_AVAILABLE;
    /* Error code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reserve a Response timer for this Config Request message  */
  if ( (err_no = hdrscm_reserve_rsp_timer(HDRHAI_PHYSICAL_LAYER_PROTOCOL, 
                   &timer_id, hdrscmphy.trans_id, FALSE)) == E_SUCCESS)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
                 "AT-init Proposal for PHY-3");

    hdrscmphy.max_transmit_power = HDRSCMPHY_DESIRED_MAX_TRANSMIT_POWER;
    hdrscmphy.dtx_support = HDRSCMPHY_DESIRED_DTX_SUPPORT;

    /* Get a new DSM buffer. hdrutil is guaranteed to return a buffer.
       Failure is handled by an ERR_FATAL */
    send_msg_ptr = hdrutil_new_msg_buffer();

    /* Pack ConfigurationRequest message's Message ID */
    HDRBIT_PACK8(send_msg_ptr, HDRSCM_CONFIG_REQ_MSG,
                 attrib_offset, HDRSCM_MESSAGE_ID_SIZE );

    /* Shift the offset to start of Transaction ID */
    attrib_offset += HDRSCM_MESSAGE_ID_SIZE;

    /* Pack the message's Transaction ID */
    HDRBIT_PACK8(send_msg_ptr, hdrscmphy.trans_id, attrib_offset, 
                 HDRSCM_TRANS_ID_SIZE);

    /* Shift the offset to start of Attribute Record's Length */
    attrib_offset += HDRSCM_TRANS_ID_SIZE;

    /*-----------------------------------------------------------------------
               Pack MaxTransmitPower Attribute record if necessary
    -----------------------------------------------------------------------*/

    /* Pack Length field */
    HDRBIT_PACK8(send_msg_ptr, HDRSCMPHY_SIMPLE_AR_BYTE_LENGTH,
                 attrib_offset, HDRSCM_LENGTH_SIZE);

    /* Shift offset to start of Attribute ID field */
    attrib_offset += HDRSCM_LENGTH_SIZE;

    /* Pack Attribute ID */
    HDRBIT_PACK8(send_msg_ptr, HDRSCMPHY_MAX_TRANSMIT_POWER, attrib_offset, 
                 HDRSCMPHY_ATTRIB_ID_SIZE);

    /* Shift offset to start of attribute value field */
    attrib_offset += HDRSCMPHY_ATTRIB_ID_SIZE;

    /* Pack MaxTransmitPower attribute value */
    HDRBIT_PACK8(send_msg_ptr, hdrscmphy.max_transmit_power, attrib_offset,
                 (HDRSCMPHY_SIMPLE_ATTRIB_SIZE_BYTES * 8));

    attrib_offset += (HDRSCMPHY_SIMPLE_ATTRIB_SIZE_BYTES * 8);

#ifdef FEATURE_HDR_REVB_DTX_MODE
  /*-----------------------------------------------------------------------
                  Pack DTXSupport Attribute record if necessary
  -----------------------------------------------------------------------*/

    /* If network DTX is enabled via NV */
    if ( hdrscmphy.is_net_dtx_enabled )
    {
      /* Pack the Length field */
      HDRBIT_PACK8(send_msg_ptr,
                   HDRSCMPHY_SIMPLE_AR_BYTE_LENGTH,
                   attrib_offset,
                   HDRSCM_LENGTH_SIZE);

      /* Shift the offset to start of Attribute ID field */
      attrib_offset += HDRSCM_LENGTH_SIZE;

      HDRBIT_PACK8(send_msg_ptr,
                   HDRSCMPHY_DTX_SUPPORT,
                   attrib_offset,
                   HDRSCMPHY_ATTRIB_ID_SIZE);

      /* Shift the offset to start of Attribute Value field */
      attrib_offset += HDRSCMPHY_ATTRIB_ID_SIZE;

      /* Pack the value of attribute DTXSupport Attrib */
      HDRBIT_PACK8(send_msg_ptr, 
                   hdrscmphy.dtx_support,
                   attrib_offset,
                   (HDRSCMPHY_SIMPLE_ATTRIB_SIZE_BYTES * 8));

      attrib_offset += (HDRSCMPHY_SIMPLE_ATTRIB_SIZE_BYTES * 8);
        HDR_MSG_PROT_1(MSG_LEGACY_HIGH,
                   "nDTX: AT Proposed. DTXSupport %d",
                   hdrscmphy.dtx_support);

    }
#endif /* FEATURE_HDR_REVB_DTX_MODE */


    /* Send ConfigRequest msg on RTC. All config messages are reliable. */
    hdrhmp_send_msg(HDRHAI_PHYSICAL_LAYER_PROTOCOL,
                    HDRHAI_RTC,
                    TRUE,
                    HDRSCM_CONFIG_REQ_MSG_PRI,
                    HDRHAI_IN_CONFIG,
                    send_msg_ptr,
                    hdrscm_tx_outcome_cb,
                    (void*) ((dword) timer_id) );

    HDR_MSG_PROT_2(MSG_LEGACY_HIGH,
                   "AT Proposed. MaxTransmitPower %d, DTXSupport %d",
                   hdrscmphy.max_transmit_power,
                   hdrscmphy.dtx_support);

    /* Increment the transaction ID for next transaction */ 
    ++hdrscmphy.trans_id;

  } /* If a Response timer is available */

  return err_no;

} /* hdrscmphy_propose */

 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION HDRSCMPHY_ATTRIB_IS_SUPPORTED

DESCRIPTION
  Is this attribute supported?

DEPENDENCIES
  None

PARAMETERS 
  attrib_id: Attribute ID in question.
  subtype:   Subtype of the protocol that received this attribute.

RETURN VALUE
  TRUE - If the attribute is supported.
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrscmphy_attrib_is_supported
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  boolean attrib_is_supported = FALSE;
    /* Is the attribute supported? */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Only ST-3 Physical layer has configurable attributes and we 
     support negotiation of all these attributes. */
  if (subtype == HDRSCP_SUB3_PHYS_LAYER)
  {
    /* Sub-3 Physical layer attribute. Checking if the attrib_id is valid
       for this subtype... */
    attrib_is_supported = (((attrib_id >= HDRSCMPHY_FIRST_SIMPLE_ATTRIB) &&
                            (attrib_id <= HDRSCMPHY_LAST_SIMPLE_ATTRIB)) || 
                           ((attrib_id >= HDRSCMPHY_FIRST_COMPLEX_ATTRIB) &&
                            (attrib_id <= HDRSCMPHY_LAST_COMPLEX_ATTRIB)));
  }

  return attrib_is_supported;

} /* hdrscmphy_attrib_is_supported */

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_PROCESS_COMPLEX_ATTRIB

DESCRIPTION
  This is a generic interface to process all complex attributes given below.

    0x01 - AdditionalCarriersInitTxPower
    0x02 - ReversePilotTransmitSlots 

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - pointer to the DSM item holding the message to be parsed.
  p_aip    - proposed attribute information pointer.
  a_aip    - accepted attribute information pointer.
  subtype  - Subtype negotiated Physical layer protocol.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmphy_process_complex_attrib
(
  dsm_item_type                               *item_ptr,
  hdrscm_proposed_attrib_type   const *const   p_aip,
  hdrscm_accepted_attrib_type         *const   a_aip,
  uint16                        const          subtype
)
{

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error code; Lets be optimistic! */

  hdrscmphy_attrib_id_enum_type attrib_id = 
    (hdrscmphy_attrib_id_enum_type) p_aip->attrib_id;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 


  switch ( attrib_id )
  {
    case HDRSCMPHY_ADDITIONAL_CARRIERS_INIT_TX_POWER:
      err_no = hdrscmphy_parse_addl_carr_init_tx_power( item_ptr, 
                                                        p_aip, 
                                                        a_aip);
      break;

    case HDRSCMPHY_REVERSE_PILOT_TRANSMIT_SLOTS:
      err_no = hdrscmphy_parse_reverse_pilot_transmit_slots( item_ptr,
                                                             p_aip,
                                                             a_aip);
      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
        "Invalid complex attrib: %d", p_aip->attrib_id );
      err_no = E_NOT_SUPPORTED;

  } /* switch */

  return err_no;

} /* hdrscmphy_process_complex_attrib */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_ATTRIB_IS_SIMPLE

DESCRIPTION
  To find if the attribute given by the attrib_id is simple. 
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID 

ASSUMPTIONS
  Assumes that the subtype is 0x0003
  Assumes that the attribute ID is valid for the given subtype.

RETURN VALUE
  TRUE - if the attribute given by 'attrib_id' is simple.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscmphy_attrib_is_simple
(
  uint16 const attrib_id,
  uint16 const subtype
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Function parameter "subtype" not used. */
  SYS_ARG_NOT_USED(subtype);

  return HDRSCMPHY_ATTRIB_IS_SIMPLE(attrib_id);

} /* hdrscmphy_attrib_is_simple */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_SIMPLE_ATTRIB_IS_VALID

DESCRIPTION
  This function is used by SCM to query if the given 'attrib_value' is a 
  valid value for the attribute specified by 'attrib_id'.
  
DEPENDENCIES
  None.

PARAMETERS
  attrib_id    - Attribute ID of the simple attribute.
  attrib_value - Value of the simple attribute. 
  subtype      - Subtype negotiated for FMAC.

ASSUMPTIONS
  1. Subtype is HDRSCP_SUB3_PHYS_LAYER. (Please note that any message for
     other subtypes will be dropped at attrib_is_supported check)
  2. Attribute ID given by "attrib_id" is a valid simple attribute for the 
     given subtype.

RETURN VALUE
  E_SUCCESS          - If the attrib_value is valid.
  E_OUT_OF_RANGE     - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrscmphy_simple_attrib_is_valid
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
)
{

  uint8 index;
    /* LUT Index got from the attribute ID */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SYS_ARG_NOT_USED(subtype);

  /* Get the index from the simple attribute */
  index = HDRSCMPHY_SIMPLE_ATTRIB_ID_TO_IND(attrib_id);

  /* If the attribute value is out of allowed range, */
  if ((attrib_value < hdrscmphy_simple_attrib_valid_range[index].min) ||
      (attrib_value > hdrscmphy_simple_attrib_valid_range[index].max))
  {
    err_no = E_OUT_OF_RANGE;
  }

  return err_no;

} /* hdrscmphy_simple_attrib_is_valid */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_ATTRIB_IS_GAUPABLE

DESCRIPTION
  This function is used by SCM module to find out if an attribute is GAUPable.

DEPENDENCIES
  None.

PARAMETERS
  attrib_id - Attribute ID.
  subtype - Subtype of Physical layer protocol
  
RETURN VALUE
  E_SUCCESS       - If the attribute is GAUPable.
  E_NOT_ALLOWED   - If AN is not allowed to GAUP this attribute.

SIDE EFFECTS
  None.
===========================================================================*/
hdrerrno_enum_type hdrscmphy_attrib_is_gaupable
(
  uint16 const attrib_id,
  uint16 const subtype
)
{
  uint8 gaup_idx = 0;
    /* Loop index */

  hdrerrno_enum_type err_no = E_NOT_ALLOWED;
    /* Error code */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SYS_ARG_NOT_USED(subtype);

  /* For each attribute in LUT, */
  for (gaup_idx = 0; gaup_idx < HDRSCMPHY_MAX_NUM_GAUPABLE_ATTRIBS; ++gaup_idx)
  {
    /* If the attribute ID is same as the GAUPable attribute */
    if (hdrscmphy_gaupable_attribs[gaup_idx] == attrib_id)
    {
      err_no = E_SUCCESS;
      break;
    }
  }

  return err_no;

} /* hdrscmphy_attrib_is_gaupable */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_GET_ATTRIB_OFFSET_AND_SIZE

DESCRIPTION
  To get the attribute's byte offset into the config struct and its size.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  attrib_offset - Address where the Byte Offset from the start of the struct 
                  to the attribute is stored.
  attrib_size   - Address where the size of the attribute is stored.
  
RETURN VALUE
  E_SUCCESS - If the given attrib ID is valid.
  E_DATA_INVALID - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmphy_get_attrib_offset_and_size
(
  uint16 attrib_id, 
  uint16 *attrib_offset,
  uint16 *attrib_size,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_DATA_INVALID;
    /* error code */

  uint8 attrib_index;
    /* Attribute Index */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdrscmphy_attrib_is_supported(attrib_id,subtype))
  {
    err_no = E_SUCCESS;

    switch (attrib_id)
    {
      case HDRSCMPHY_ADDITIONAL_CARRIERS_INIT_TX_POWER:
        *attrib_offset = FPOS(hdrscmphy_config_struct_type, init_tx_pwr);
        *attrib_size = sizeof(hdrscmphy_additional_carriers_init_tx_pwr_type);
        break;

      case HDRSCMPHY_REVERSE_PILOT_TRANSMIT_SLOTS:
        *attrib_offset = FPOS(hdrscmphy_config_struct_type,
                              rl_pilot_transmit_slots);
        *attrib_size = sizeof(hdrscmphy_reverse_pilot_transmit_slots_type);
        break;

      default:
        /* Simple attributes */
        /* Get the attribute Index from the attribute ID */
        attrib_index = HDRSCMPHY_SIMPLE_ATTRIB_ID_TO_IND(attrib_id);

        *attrib_size = HDRSCMPHY_SIMPLE_ATTRIB_SIZE_BYTES;
        *attrib_offset = ( FPOS(hdrscmphy_config_struct_type, mac_index_bits) + 
                           (attrib_index * HDRSCMPHY_SIMPLE_ATTRIB_SIZE_BYTES));
    } /* switch */

  }

  return err_no;

} /* hdrscmphy_get_attrib_offset_and_size */


/* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_GET_ATTRIB_VERNO_INDEX

DESCRIPTION
  Returns the index of the given attribute in the version number array.

DEPENDENCIES
  None

PARAMETERS
  attrib_id     - Attribute Id.
  
RETURN VALUE
  E_SUCCESS - If the given attribute ID is valid.

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdrscmphy_get_attrib_verno_index
(
  uint16 attrib_id,
  uint16 *index,
  uint16 subtype
)
{
  hdrerrno_enum_type err_no = E_DATA_INVALID;
    /* Error code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  if (hdrscmphy_attrib_is_supported(attrib_id, subtype))
  {

    err_no = E_SUCCESS;

    switch (attrib_id)
    {
      case HDRSCMPHY_ADDITIONAL_CARRIERS_INIT_TX_POWER:
        *index = 0;
        break;

      case HDRSCMPHY_REVERSE_PILOT_TRANSMIT_SLOTS:
        *index = 1;
        break;

      default:
        /* Complex attributes precede simple attributes in the version number
           array. */
        *index = (HDRSCMPHY_SIMPLE_ATTRIB_ID_TO_IND(attrib_id) + 
                  HDRSCMPHY_MAX_NUM_COMPLEX_ATTRIBS);
    }

  }

  return err_no;

} /* hdrscmphy_get_attrib_verno_index */

/* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_SET_CONFIG_TO_DEFAULT

DESCRIPTION
  Set the configuration attributes to its default values.

DEPENDENCIES
  None

PARAMETERS
  cfg - Pointer to the config structure.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmphy_set_config_to_default
(
  hdrscmphy_config_struct_type *cfg
)
{

  /* Zero-out the entire config struct and initialize only attributes
     with non-zero default values */
  memset(cfg, 0, sizeof(hdrscmphy_config_struct_type));

  cfg->max_rl_tx_pwr_diff = HDRSCMPHY_DEFAULT_MAX_RL_TX_PWR_DIFF;

} /* hdrscmphy_set_config_to_default */


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCMPHY_CONFIG_IS_NEEDED                                 EXTERNAL

DESCRIPTION
  This function is used to find out if an AT-initiated Configuration is 
  needed by Phy ST-3

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - If AT-init config is required.
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrscmphy_config_is_needed( void )
{
  return ( hdrscm_config_is_needed(HDRHAI_PHYSICAL_LAYER_PROTOCOL) );
} /* hdrscmphy_config_is_needed */

 /* EJECT */
/*===========================================================================
FUNCTION HDRSCMPHY_CONFIGURE                                        EXTERNAL

DESCRIPTION
  This function does the AT-init configuration of the following PHY attributes 
    1. MaxTransmitPower 
    2. DTXSupport

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - If the configuration succeeded.
  FALSE - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrscmphy_configure( void )
{
  boolean config_status = FALSE;
    /* AT-init Config needed? */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  config_status = (hdrscmphy_propose() == E_SUCCESS);

  return config_status;

} /* hdrscmphy_configure */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCMPHY_GET_PROPOSED_ATTRIB_VALUE

DESCRIPTION
  This function is used by SCM to get the attribute value proposed by AT in 
  the recent transaction.

DEPENDENCIES
  None.

PARAMETERS
  trans_id      - Transaction ID.
  a_aip         - Array of attribute information pointers where the info about
                  each of the attributes in the transaction specified by 
                  trans_id are stored.

RETURN VALUE
  E_SUCCESS - If the given transaction ID is valid.
  E_FAILURE - If a valid transaction specified by 'trans_id' is not the recent.

SIDE EFFECTS
  None.

===========================================================================*/
hdrerrno_enum_type hdrscmphy_get_proposed_attrib_value
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *a_aip,
  uint16 const subtype
)
{
  hdrerrno_enum_type err_no = E_FAILURE;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (trans_id == hdrscmphy.trans_id-1)
  {
    switch(a_aip->attrib_id)
    {
      case HDRSCMPHY_MAX_TRANSMIT_POWER:
        a_aip->value_id = hdrscmphy.max_transmit_power;
        a_aip->complex_val_ptr = NULL;
        a_aip->complex_val_size = HDRSCMPHY_SIMPLE_ATTRIB_SIZE_BYTES;
        a_aip->is_accepted = TRUE;
        err_no = E_SUCCESS;
        break;

#ifdef FEATURE_HDR_REVB_DTX_MODE
    case HDRSCMPHY_DTX_SUPPORT:
      a_aip->value_id = hdrscmphy.dtx_support;
      a_aip->complex_val_ptr = NULL;
      a_aip->complex_val_size = HDRSCMPHY_SIMPLE_ATTRIB_SIZE_BYTES;
      a_aip->is_accepted = TRUE;
      err_no = E_SUCCESS;
      break;
#endif /* FEATURE_HDR_REVB_DTX_MODE */

      default:
        /* unsupported attrib_id */
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
            "PHY3: Attrib id %d not supported", a_aip->attrib_id);
        break;
    }
  }
  else
  {
    HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
        "PHY3: trans_id %d mismatched proposed trans_id %d",
         trans_id, (uint8)(hdrscmphy.trans_id - 1));
  }

  return err_no;

} /* hdrscmphy_get_proposed_attrib_value */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSCMPHY_FAIL_CURR_CONFIG_NEG

DESCRIPTION
  This function fails the current config negotiation

DEPENDENCIES
  None

PARAMETERS
  close_reason - Session Close reason

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmphy_fail_curr_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason
)
{
  uint16 subtype;
    /* Protocol subtype */

  hdrind_ind_data_union_type ind_data;
    /* Indication data */

  /* Allocate out of the heap because it is possible for this indication to 
     be queued from a different task context. Otherwise, the payload may be 
     corrupted which could cause a null pointer de-reference. */
  static hdrscp_scp_fail_ind_type session_fail_data;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the In-Config subtype */
  subtype = hdrscp_get_config_subtype(HDRHAI_PHYSICAL_LAYER_PROTOCOL);

  switch(subtype)
  {
    case HDRSCP_SUB3_PHYS_LAYER:
      /* fill the indication data */
      session_fail_data.type = HDRHAI_PHYSICAL_LAYER_PROTOCOL;
      session_fail_data.subtype = HDRSCP_SUB3_PHYS_LAYER;
      session_fail_data.attribute_id_length = 
        HDRSCMPHY_SIMPLE_ATTRIB_SIZE_BYTES;
      session_fail_data.attribute_id = HDRSCMPHY_MAX_TRANSMIT_POWER;
      session_fail_data.reason = close_reason;

      ind_data.scp_fail_ptr = (void *)&session_fail_data;

      hdrind_give_ind(HDRIND_PHY_PROT_NEG_FAILED,(void *)&ind_data);
      break;

    default:
      ERR("PHY Sub: %d does not support AT-init proposal", subtype,0,0);
  }

} /* hdrscmphy_fail_curr_config_neg */

#ifdef FEATURE_HDR_REVB_DTX_MODE
/*===========================================================================

FUNCTION HDRSCMPHY_SET_NET_DTX_MODE

DESCRIPTION
   This function is set network based dtx mode read from NV.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscmphy_set_net_dtx_mode( boolean is_net_dtx_enabled_by_nv )
{
  hdrscmphy.is_net_dtx_enabled = is_net_dtx_enabled_by_nv;

} /* hdrscmphy_set_net_dtx_nv_mode */
#endif /* FEATURE_HDR_REVB_DTX_MODE */

