/*===========================================================================

                          E C A L L _ P S A P . C

DESCRIPTION
  eCall modem implementation

REFERENCES
            CEN/TC 278
            Date: 2009-11
            prEN 15722:2009
            CEN/TC 278
            Secretariat: NEN 

EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright(c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/src/ecall_psap.c#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ 
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/14/09    bd     Ported module for AMSS from QSE source

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/


#include "comdef.h"

#ifdef FEATURE_ECALL_PSAP
#include "crc.h"
#include "err.h"
#include "msg.h"

#include "ecall_msd_defines.h"
#include "ecall_modem_psap.h"
#include "ecall_efs.h"
#include "bit.h"


void ecall_deserialise_msd(const uint8  *msd_byte_array_in,int16   len);

ecall_msd_structure_s_type ecall_psap_msd_struct;

/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================
FUNCTION      ECALL_MSD_CONVERT_ASN1_VIN_TO_CHARACTER

DESCRIPTION  
        This API converts the MSD VIN from ASN1 format to characters.
 
PARAMETERS   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
char ecall_msd_convert_asn1_vin_to_character
(
  byte asn1_byte
)
{
  char ret = 0xFF;
    /* The acceptable characters for VIN are:
     "A"  .. "H" |"J"  .. "N" |"P" |"R"  .. "Z" |"0"  .. "9"
     ASN.1 will map the above characters to:
     0x0a .. 0x11|0x12 .. 0x16|0x17|0x18 .. 0x20|0x00 .. 0x09 */

  if ( asn1_byte <= 0x09 )
  {
    ret = asn1_byte + '0';
  }
  else if ( (asn1_byte >= 0x0a) && (asn1_byte <= 0x11) )
  {
    ret = asn1_byte + 55;
  }
  else if ( (asn1_byte >= 0x12) && (asn1_byte <= 0x16) )
  {
    ret = asn1_byte + 56;
  }
  else if ( (asn1_byte == 0x17) )
  {
    ret = 'P';
  }
  else if ( (asn1_byte >= 0x18) && (asn1_byte <= 0x20) )
  {
    ret = asn1_byte + 58;
  }

  if ( 0xFF == ret )
  {
    ECALL_MSG_1( ECALL_MED, "Unsupported VIN ASN.1 byte value %d; change to '0' automatically\n",
                             asn1_byte );
    ret = '0';
  }

  return ret;

}



/*===========================================================================
FUNCTION      ECALL_DESERIALISE_MSD

DESCRIPTION  
        CEN/TC 278
        Date: 2008-10
        prEN 15722
        Draft EN 081018
 
PARAMETERS   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
/*lint -esym(715,len) */
void ecall_deserialise_msd
(
  const uint8  *msd_byte_array_in,
  int16   len
)
{ 
  byte  num_bits_to_read = 0;
  byte  byte_pos = 0;
  byte  start_pos_in_byte = 0;
  byte  msd_byte_array[ECALL_MSD_MAX_LENGTH];
 
  memset(msd_byte_array,0,ECALL_MSD_MAX_LENGTH);
  ecall_memscpy ( msd_byte_array, sizeof(msd_byte_array),
                  msd_byte_array_in, len );

  /* Byte 0 */
  num_bits_to_read = 8;
  ecall_psap_msd_struct.ecall_formatversion = b_unpackb(&msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);
  byte_pos += 1;

  /* Byte 1 */
  start_pos_in_byte = 6;
  ecall_psap_msd_struct.ecall_messageidentifier = (uint8) b_unpackb(&msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);
  byte_pos += 1;

  /* Byte 2 */
  num_bits_to_read = 1;
  start_pos_in_byte = 6;
  ecall_psap_msd_struct.ecall_control_info.activation = (ecall_activation_type) b_unpackb(&msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);

  start_pos_in_byte = 7;
  ecall_psap_msd_struct.ecall_control_info.callType = (ecall_type_of_call) b_unpackb(&msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);
  byte_pos += 1;

  /* Byte 3 */
  start_pos_in_byte = 0;
  ecall_psap_msd_struct.ecall_control_info.positionconfidence = b_unpackb(&msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);

  num_bits_to_read = 5;
  start_pos_in_byte = 1;
  ecall_psap_msd_struct.ecall_control_info.ecall_vehicletype = (ecall_vehicle_e_type)( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) + 1 );

  /* isowmi */
  num_bits_to_read = 6;
  start_pos_in_byte = 6;
  ecall_psap_msd_struct.ecall_vin[0] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 4 */
  start_pos_in_byte = 4;
  ecall_psap_msd_struct.ecall_vin[1] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 5 */
  start_pos_in_byte = 2;
  ecall_psap_msd_struct.ecall_vin[2] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 6 */
  start_pos_in_byte = 0;
  ecall_psap_msd_struct.ecall_vin[3] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;

  start_pos_in_byte = 6;
  ecall_psap_msd_struct.ecall_vin[4] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 7 */
  start_pos_in_byte = 4;
  ecall_psap_msd_struct.ecall_vin[5] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 8 */
  start_pos_in_byte = 2;
  ecall_psap_msd_struct.ecall_vin[6] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 9 */
  start_pos_in_byte = 0;
  ecall_psap_msd_struct.ecall_vin[7] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;

  start_pos_in_byte = 6;
  ecall_psap_msd_struct.ecall_vin[8] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 10 */
  start_pos_in_byte = 4;
  ecall_psap_msd_struct.ecall_vin[9] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 11 */
  start_pos_in_byte = 2;
  ecall_psap_msd_struct.ecall_vin[10] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 12 */
  start_pos_in_byte = 0;
  ecall_psap_msd_struct.ecall_vin[11] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;

  start_pos_in_byte = 6;
  ecall_psap_msd_struct.ecall_vin[12] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 13 */
  start_pos_in_byte = 4;
  ecall_psap_msd_struct.ecall_vin[13] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 14 */
  start_pos_in_byte = 2;
  ecall_psap_msd_struct.ecall_vin[14] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;

  /* Byte 15 */
  start_pos_in_byte = 0;
  ecall_psap_msd_struct.ecall_vin[15] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;

  start_pos_in_byte = 6;
  ecall_psap_msd_struct.ecall_vin[16] = ecall_msd_convert_asn1_vin_to_character( b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read ) ) ;
  byte_pos += 1;


  /* Byte 16 */
  num_bits_to_read = 1;
  start_pos_in_byte = 5;
  ecall_psap_msd_struct.ecall_vehicle_propulsionstorage.gasolineTankPresent = b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);
  
  start_pos_in_byte = 6;
  ecall_psap_msd_struct.ecall_vehicle_propulsionstorage.dieselTankPresent = b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);

  start_pos_in_byte = 7;
  ecall_psap_msd_struct.ecall_vehicle_propulsionstorage.compressedNaturalGas = b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);
  byte_pos += 1;

  /* Byte 17 */
  start_pos_in_byte = 0;
  ecall_psap_msd_struct.ecall_vehicle_propulsionstorage.liquidPropaneGas = b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);

  start_pos_in_byte = 1;
  ecall_psap_msd_struct.ecall_vehicle_propulsionstorage.electricEnergyStorage = b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);

  start_pos_in_byte = 2;
  ecall_psap_msd_struct.ecall_vehicle_propulsionstorage.hydrogenStorage = b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);

  num_bits_to_read  = ECALL_SIZEOF_TIMESTAMP * 8;
  start_pos_in_byte = 4;
  ecall_psap_msd_struct.ecall_timestamp = b_unpackd( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);
  byte_pos += ECALL_SIZEOF_TIMESTAMP;

  /* Byte 21 */
  num_bits_to_read = ECALL_SIZEOF_POS_MILLIARCSEC * 8;
  ecall_psap_msd_struct.ecall_vehicle_location.positionlatitude = 
                        (ecall_pos_type)(b_unpackd( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read) - 2147483648LL);
  byte_pos += ECALL_SIZEOF_POS_MILLIARCSEC;

  /* Byte 25 */
  num_bits_to_read = ECALL_SIZEOF_POS_MILLIARCSEC * 8;
  ecall_psap_msd_struct.ecall_vehicle_location.positionlongitude = 
                        (ecall_pos_type)(b_unpackd( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read) - 2147483648LL);
  byte_pos += ECALL_SIZEOF_POS_MILLIARCSEC;

  /* Byte 29 */
  num_bits_to_read = 8;
  ecall_psap_msd_struct.ecall_vehicledirection = b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);
  byte_pos += 1;

  /* Byte 30 */
  ecall_psap_msd_struct.ecall_numberOfPassengers = b_unpackb( &msd_byte_array[byte_pos], start_pos_in_byte, num_bits_to_read);
  byte_pos += 1;


  ECALL_MSG_3( ECALL_HIGH, "padding from (Byte %d, Bit %d), sizeof(msd_byte_array) = %d",
                            byte_pos,
                            (start_pos_in_byte+1)%8,
                            sizeof(msd_byte_array));
  
  /*------------------------------------------------------------------------------------------------*/
  /*                                   Print to the QXDM                                            */
  /*------------------------------------------------------------------------------------------------*/
   ECALL_MSG_2( ECALL_HIGH, "Populating InMillArcSecond longitude, latitude  %d, %d",
                             ecall_psap_msd_struct.ecall_vehicle_location.positionlongitude,
                             ecall_psap_msd_struct.ecall_vehicle_location.positionlatitude );

  ECALL_MSG_1( ECALL_HIGH, "Populating vehicledirection %d",
                            ecall_psap_msd_struct.ecall_vehicledirection );

  /*------------------------------------------------------------------------------------------------*/
  /*                                   Save to the EFS                                              */
  /*------------------------------------------------------------------------------------------------*/
  
  (void)ecall_save_msd(&ecall_psap_msd_struct);

}


/*===========================================================================
FUNCTION      ECALL_PSAP_RECEIVE_MSD

DESCRIPTION  

PARAMETERS   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void ecall_psap_receive_msd
(
  const uint8  *msd, 
  int16   len
)
{
  static uint32 num_msds = 0;
  int16 i;
  uint8 xsd[ECALL_MSD_MAX_LENGTH];
  
  if ( len != ECALL_MSD_MAX_LENGTH )
  {
    ECALL_MSG_2( ECALL_ERROR, "len %d != ECALL_MSD_MAX_LENGTH %d",
                               len,
                               ECALL_MSD_MAX_LENGTH );
    return;
  }
  ecall_memscpy ( xsd, sizeof(xsd),
                  msd, len );

  ECALL_MSG_0( ECALL_ERROR, "============================================================" );
  ECALL_MSG_3( ECALL_HIGH, "Successfully received MSD #%d with len %d and 0-seed CRC32 0x%08x",
                            ++num_msds,
                            len,
                            crc_32_calc(xsd, (uint16)len, 0) );
  ECALL_MSG_0( ECALL_ERROR, "------------------------------------------------------------" );
  for (i = 0; i < len; i += 8)
  {
    if ((i + 8) < len)
    {
      MSG_8(MSG_SSID_ECALL, MSG_LEGACY_HIGH,
            "  %02x %02x %02x %02x   %02x %02x %02x %02x",
             xsd[i],   xsd[i+1], xsd[i+2], xsd[i+3], 
             xsd[i+4], xsd[i+5], xsd[i+6], xsd[i+7]);
    }
    else
    {
      /* MSD length is always 140, so we will have 4 bytes left over */
      MSG_4(MSG_SSID_ECALL, MSG_LEGACY_HIGH,
            "  %02x %02x %02x %02x",
             xsd[i], xsd[i+1], xsd[i+2], xsd[i+3]);
    }
  }
  ECALL_MSG_0( ECALL_ERROR, "============================================================" );

  ecall_deserialise_msd(msd, len);

}

#endif



