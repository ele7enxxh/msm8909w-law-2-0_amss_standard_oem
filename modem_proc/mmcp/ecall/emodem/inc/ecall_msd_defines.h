#ifndef ECALL_MSD_DEFINES_H_
#define ECALL_MSD_DEFINES_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         E C A L L _ M S D _ D E F I N E S 
                            H E A D E R   F I L E

GENERAL DESCRIPTION 
This file contains the definition of the MSD Structure
 
Reference: 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/inc/ecall_msd_defines.h#1 $ 
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

#if (defined(FEATURE_ECALL_APP) || defined(FEATURE_ECALL_PSAP))

#include "ecall_app.h"

#define ECALL_SIZEOF_VIN  (17)

/* In Milli Arc Seconds */
#define ECALL_MIN_LAT     (0xECB02700) /* -90*60*60.000  */
#define ECALL_MAX_LAT     (0x134FD900) /*  90*60*60.000  */
#define ECALL_MIN_LON     (0xD9604E00) /* -180*60*60.000 */
#define ECALL_MAX_LON     (0x269FB200) /*  180*60*60.000 */
#define ECALL_INVALID_GPS (0x7FFFFFFF)

/* In 100 Milli Arc Seconds*/
#define ECALL_MIN_REC_LAT (-512)
#define ECALL_MAX_REC_LAT  (511)
#define ECALL_MIN_REC_LON (-512)
#define ECALL_MAX_REC_LON  (511)

typedef int32 ecall_pos_type;
typedef dword ecall_timestamp_type;
typedef char  ecall_vehicle_id_number[ECALL_SIZEOF_VIN];


#define ECALL_SIZEOF_TIMESTAMP           (sizeof(ecall_timestamp_type))
#define ECALL_SIZEOF_POS_MILLIARCSEC     (sizeof(ecall_pos_type))
#define ECALL_SIZEOF_POS_100MILLIARCSEC  (sizeof(ecall_rec_pos_type))


typedef enum ecallvehicle
{
  passengerVehicleClassM1 = 1,
  busesAndCoachesClassM2,
  busesAndCoachesClassM3,
  lightCommercialVehiclesClassN1,
  heavyDutyVehiclesClassN2,
  heavyDutyVehiclesClassN3,
  motorcyclesClassL1e,
  motorcyclesClassL2e,
  motorcyclesClassL3e,
  motorcyclesClassL4e,
  motorcyclesClassL5e,
  motorcyclesClassL6e,
  motorcyclesClassL7e
} ecall_vehicle_e_type;

typedef struct
{
  ecall_activation_type activation;  /* manualActivation (0) / automaticActivation (1) */
  ecall_type_of_call callType;       /* EN15722: EmergCall (0) / Test (1) */
  boolean positionconfidence;        /* lowConfidenceInPosition (0) / positionCanBeTrusted (1) */
  ecall_vehicle_e_type ecall_vehicletype;
} ecall_control_info_s_type;

typedef struct
{
  boolean gasolineTankPresent;
  boolean dieselTankPresent ;
  boolean compressedNaturalGas ;
  boolean liquidPropaneGas ;
  boolean electricEnergyStorage ;
  boolean hydrogenStorage; 
} ecall_vehicle_propulsionstorage_s_type;


typedef struct
{
  ecall_pos_type positionlatitude;
  ecall_pos_type positionlongitude;
} ecall_vehicle_location_s_type;

typedef struct ecall_msd_structure
{
                                     /* 1Byte - MSD format version set to 1 to discriminate from later MSD formats*/
  uint8 ecall_formatversion;

                                     /* 1Byte - Message identifier, starting with 1 and to be incremented with 
                                         every MSD retransmission after the incident event */
  uint8 ecall_messageidentifier;

                                     /* 1Byte - 1Bit for automaticActivation; 1Bit for testCall; 1Bit for
                                        positionCanBeTrusted; 5Bits for vehicleType */
  ecall_control_info_s_type ecall_control_info;

                                     /* 12Bytes and 6Bits - 6Bits for each PrintableString.
                                        isowmi PrintableString (Size(3)) (FROM("A".."H"|"J".."N"|"P"|"R".."Z"|"0".."9"))
                                        isovds PrintableString (Size(6)) (FROM("A".."H"|"J".."N"|"P"|"R".."Z"|"0".."9"))
                                        isovisModelyear PrintableString (Size(1)) (FROM("A".."H"|"J".."N"|"P"|"R".."Z"|"0".."9"))
                                        isovisSeqPlant PrintableString (Size(7)) (FROM("A".."H"|"J".."N"|"P"|"R".."Z"|"0".."9")) */
  ecall_vehicle_id_number ecall_vin;

                                     /* 1Byte - identifies the type of vehicle energy storage. Will always set to
                                        a gasoline only car in eCall App */
  ecall_vehicle_propulsionstorage_s_type ecall_vehicle_propulsionstorage;

                                     /* 4Bytes - Timestamp of incident event. As seconds elapsed since midnight January 1st, 1970 UTC. */
  ecall_timestamp_type   ecall_timestamp;

                                     /* 8Bytes - 4 Bytes for Latitude and 4 Bytes for Longitude */
  ecall_vehicle_location_s_type ecall_vehicle_location;

                                     /* 1Byte - Clockwise heading in 2 degree units, 255 represents "unknown" */
  uint8 ecall_vehicledirection;

                                     /* 1Byte- OPTIONAL - Minimum known number of fastened seatbelts, 
                                        to be set to 255 if no information is available */
  uint8 ecall_numberOfPassengers;

} ecall_msd_structure_s_type;


#else

void ecall_msd_defines_dummy_func(void);

#endif /*End Featurisation */


#endif /* MSD_DEFINES_H_ */

