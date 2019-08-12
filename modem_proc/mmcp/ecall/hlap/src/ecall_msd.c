/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Ecall MSD File

GENERAL DESCRIPTION 
This file contains the APIs to construct and print the Ecall MSD structure
 
Ref spec: 
            CEN/TC 278
            Date: 2009-11
            prEN 15722:2009
            CEN/TC 278
            Secretariat: NEN 
 
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2008 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/hlap/src/ecall_msd.c#1 $ 
  $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/11/12   DL      Change the definition of type_of_call in MSD structure to 
                   meet the MSD spec EN15722 
09/20/09   PA      Initial version.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_ECALL_APP

#include "msg.h"
#include "err.h"
#include "string.h"
#include "stringl.h"

#include "ecall_defines.h"
#include "ecall_msd.h"
#include "ecall_efs.h"
#include "bit.h"

/*===========================================================================/
FUNCTION ECALL_CONSTRUCT_MSD

DESCRIPTION
  This function add the GPS fix position values, if any
  and hard codes the rest of the Ecall MSD Structure

DEPENDENCIES
   None

 PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS

==============================================================================*/

void ecall_construct_msd_struct(ecall_msd_structure_s_type *ecall_msd)
{
  ecall_msd->ecall_formatversion = 1;     // 1B. MSD format version set to 1 to discriminate from later MSD formats
  ecall_msd->ecall_messageidentifier = 0; //1B. Message identifier

  ecall_msd->ecall_control_info.activation = ECALL_MANUAL_INITIATED;
  ecall_msd->ecall_control_info.callType   = (ecall_type_of_call) 1;  /* MSD spec EN15722: Test eCall(1) */
  ecall_msd->ecall_control_info.positionconfidence = FALSE;
  ecall_msd->ecall_control_info.ecall_vehicletype  = passengerVehicleClassM1; //passengerVehicleClassM1 

  (void)strlcpy(ecall_msd->ecall_vin, "WMIVDSVDSYA123456", sizeof(ecall_vehicle_id_number)+1);

  ecall_msd->ecall_vehicle_propulsionstorage.gasolineTankPresent   = TRUE;
  ecall_msd->ecall_vehicle_propulsionstorage.dieselTankPresent     = FALSE;
  ecall_msd->ecall_vehicle_propulsionstorage.compressedNaturalGas  = FALSE;
  ecall_msd->ecall_vehicle_propulsionstorage.liquidPropaneGas      = FALSE;
  ecall_msd->ecall_vehicle_propulsionstorage.electricEnergyStorage = FALSE;
  ecall_msd->ecall_vehicle_propulsionstorage.hydrogenStorage       = FALSE;

  ecall_msd->ecall_timestamp                          = 0;
  ecall_msd->ecall_vehicle_location.positionlatitude  = ECALL_INVALID_GPS ;
  ecall_msd->ecall_vehicle_location.positionlongitude = ECALL_INVALID_GPS ;

  ecall_msd->ecall_vehicledirection = 0xFF;

  ecall_msd->ecall_numberOfPassengers = 0xFF; 

} //Construct_MSD


/*===========================================================================/
FUNCTION ECALL_MSD_CONVERT_VIN_TO_PRINTABLE_STRING

DESCRIPTION
  This function converts the VIN characters to printable string format.
  ASN.1 encodes all the permitted VIN characters starting from '0'.
 
DEPENDENCIES
   None

 PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS

==============================================================================*/
byte ecall_msd_convert_vin_to_pritable_string
(
  char  msd_char
)
{
  byte ret = 0xFF;

  /* The acceptable characters for VIN are:
     "A"  .. "H" |"J"  .. "N" |"P" |"R"  .. "Z" |"0"  .. "9"
     ASN.1 will map the above characters to:
     0x0a .. 0x11|0x12 .. 0x16|0x17|0x18 .. 0x20|0x00 .. 0x09 */

  if ( (msd_char >= 'A') && (msd_char <= 'H') )
  {
    ret = msd_char - 55;
  }
  else if ( (msd_char >= 'J') && (msd_char <= 'N') )
  {
    ret = msd_char - 56;
  }
  else if ( msd_char == 'P' )
  {
    ret = 0x17;
  }
  else if ( (msd_char >= 'R') && (msd_char <= 'Z') )
  {
    ret = msd_char - 58;
  }
  else if ( (msd_char >= '0') && (msd_char <= '9') )
  {
    ret = msd_char - '0';
  }

  /* Unsupported Characters */
  if ( 0xFF == ret )
  {
    ECALL_MSG_1( ECALL_MED, "Unsupported VIN Character %c; change to 0 automatically",
                             msd_char );
    ret = 0x00;
  }

  return ret;

} /* ECALL_MSD_CONVERT_VIN_TO_PRINTABLE_STRING */

/*===========================================================================/
FUNCTION ECALL_SERIALISE_MSD

DESCRIPTION
  This function serializes the MSD structure into a byte array
 
DEPENDENCIES
   None

 PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS

==============================================================================*/
void ecall_serialise_msd(const ecall_msd_structure_s_type *msd_struct, ecall_msd_array_type msd_byte_array)
{

  byte   msd_ptr_temp[ECALL_MSD_MAX_LENGTH];
  byte   num_bits_to_write = 0;
  byte   byte_pos = 0;
  byte   start_pos_in_byte = 0;
  uint16 p = 0;

  memset(msd_ptr_temp,0,ECALL_MSD_MAX_LENGTH);

  /* Byte 0 */
  num_bits_to_write = 8;
  b_packb( msd_struct->ecall_formatversion, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 1 */
  /* Extension Marker for the Sequence: MSDMessage */
  /* Value of 0 to represent no extension additions */
  num_bits_to_write = 1;
  start_pos_in_byte = 0;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* OPTIONAL optionalAdditionalData.Present: Absence */
  start_pos_in_byte = 1;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* Extension Marker for the Sequence: MSDStructure */
  /* Value of 0 to represent no extension additions */
  start_pos_in_byte = 2;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* OPTIONAL recentVehicleLocationN1.Present: Absence */
  start_pos_in_byte = 3;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* OPTIONAL recentVehicleLocationN2.Present: Absence */
  start_pos_in_byte = 4;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* OPTIONAL numberOfPassengers.Present: Presence */
  start_pos_in_byte = 5;
  b_packb( (byte)1, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* numberOfPassengers */
  num_bits_to_write = 8;
  start_pos_in_byte = 6;
  b_packb( msd_struct->ecall_messageidentifier, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 2 */
  num_bits_to_write = 1;
  start_pos_in_byte = 6;
  b_packb( msd_struct->ecall_control_info.activation,  &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write); 

  start_pos_in_byte = 7;
  b_packb( msd_struct->ecall_control_info.callType,  &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 3 */
  start_pos_in_byte = 0;
  b_packb( msd_struct->ecall_control_info.positionconfidence,  &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  num_bits_to_write = 5;
  start_pos_in_byte = 1;
  /* ASN.1 maps the least permitted value 1 as 0, so we need to substract 1 for vehicleType */
  b_packb( (msd_struct->ecall_control_info.ecall_vehicletype)-1,  &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* isowmi */
  num_bits_to_write = 6;
  start_pos_in_byte = 6;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[0]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 4 */
  start_pos_in_byte = 4;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[1]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 5 */
  start_pos_in_byte = 2;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[2]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 6 */
  start_pos_in_byte = 0;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[3]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  start_pos_in_byte = 6;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[4]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 7 */
  start_pos_in_byte = 4;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[5]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 8 */
  start_pos_in_byte = 2;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[6]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 9 */
  start_pos_in_byte = 0;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[7]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  start_pos_in_byte = 6;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[8]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 10 */
  start_pos_in_byte = 4;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[9]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 11 */
  start_pos_in_byte = 2;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[10]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 12 */
  start_pos_in_byte = 0;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[11]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  start_pos_in_byte = 6;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[12]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 13 */
  start_pos_in_byte = 4;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[13]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 14 */
  start_pos_in_byte = 2;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[14]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 15 */
  start_pos_in_byte = 0;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[15]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  start_pos_in_byte = 6;
  b_packb( ecall_msd_convert_vin_to_pritable_string(msd_struct->ecall_vin[16]),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1; /*increment 1 byte*/

  /* Byte 16 */
  /* Extension Marker for the Sequence: VehiclePropulsionStorageType */
  /* Value of 0 to represent no extension additions */
  num_bits_to_write = 1;
  start_pos_in_byte = 4;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* gasolinTankPresent.NotDefault == TRUE */
  start_pos_in_byte = 5;
  b_packb( (byte)1, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* dieselTankPresent.NotDefault == FALSE */
  start_pos_in_byte = 6;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* compressedNaturalGas.NotDefault == FALSE */
  start_pos_in_byte = 7;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1;

  /* Byte 17 */
  /* liquidPropaneGas.NotDefault == FALSE */
  start_pos_in_byte = 0;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* electricEnergyStorage.NotDefault == FALSE */
  start_pos_in_byte = 1;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* hydrogenStorage.NotDefault == FALSE */
  start_pos_in_byte = 2;
  b_packb( (byte)0, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  /* gasolinTankPresent.Value == 1 */
  start_pos_in_byte = 3;
  b_packb( (byte)1, &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);

  num_bits_to_write = ECALL_SIZEOF_TIMESTAMP * 8;
  start_pos_in_byte = 4;
  b_packd( msd_struct->ecall_timestamp,  &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += ECALL_SIZEOF_TIMESTAMP;            /*increment 4 byte*/

  /* Byte 21 */
  num_bits_to_write = ECALL_SIZEOF_POS_MILLIARCSEC * 8;
  b_packd( (uint32)(msd_struct->ecall_vehicle_location.positionlatitude + 2147483648LL),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write );
  byte_pos += ECALL_SIZEOF_POS_MILLIARCSEC;      /*increment 4 byte*/

  /* Byte 25 */
  num_bits_to_write = ECALL_SIZEOF_POS_MILLIARCSEC * 8;
  b_packd( (uint32)(msd_struct->ecall_vehicle_location.positionlongitude + 2147483648LL),
           &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write );
  byte_pos += ECALL_SIZEOF_POS_MILLIARCSEC;      /*increment 4 byte*/

  /* Byte 29 */
  num_bits_to_write = 8;
  start_pos_in_byte = 4;
  b_packd( msd_struct->ecall_vehicledirection,  &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1;      /*increment 1 byte*/

  /* Byte 30 */
  b_packd( msd_struct->ecall_numberOfPassengers,  &msd_ptr_temp[byte_pos], start_pos_in_byte, num_bits_to_write);
  byte_pos += 1;      /*increment 1 byte*/

  ECALL_MSG_3( ECALL_HIGH, "padding from (Byte %d, Bit %d), sizeof(msd_byte_array) = %d",
                            byte_pos,
                            (start_pos_in_byte+1)%8,
                            ECALL_MSD_MAX_LENGTH );
  ecall_memscpy ( msd_byte_array, ECALL_MSD_MAX_LENGTH,
                  msd_ptr_temp, sizeof(msd_ptr_temp) );

  /* Print MSD */
  ECALL_MSG_0( ECALL_ERROR, "============================================================" );
  ECALL_MSG_1( ECALL_HIGH, "ecall_print_serialised_msd msd_len = %d",
                            ECALL_MSD_MAX_LENGTH );
  ECALL_MSG_0( ECALL_ERROR, "------------------------------------------------------------" );
  for (p = 0; p < ECALL_MSD_MAX_LENGTH; p += 8)
  {
    if ((p + 8) < ECALL_MSD_MAX_LENGTH)
    {
      MSG_8(MSG_SSID_ECALL, MSG_LEGACY_HIGH,
            "  %02x %02x %02x %02x   %02x %02x %02x %02x",
            msd_byte_array[p],   msd_byte_array[p+1], msd_byte_array[p+2], msd_byte_array[p+3], 
            msd_byte_array[p+4], msd_byte_array[p+5], msd_byte_array[p+6], msd_byte_array[p+7]);
    }
    else
    {
      /* MSD length is always 140, so we will have 4 bytes left over */
      MSG_4(MSG_SSID_ECALL, MSG_LEGACY_HIGH,
            "  %02x %02x %02x %02x",
            msd_byte_array[p], msd_byte_array[p+1], msd_byte_array[p+2], msd_byte_array[p+3]);
    }
  }
  ECALL_MSG_0( ECALL_ERROR, "============================================================" );

}
#else

void ecall_msd_dummy_func(void);

#endif //End Featurisation

