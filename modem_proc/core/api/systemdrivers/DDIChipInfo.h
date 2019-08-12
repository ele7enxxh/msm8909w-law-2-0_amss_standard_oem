#ifndef DDICHIPINFO_H
#define DDICHIPINFO_H
/**
* @file DDIChipInfo.h
* @brief ChipInfo Driver Interface Header File.
* This file contains the definitions of the constants, data
* structures, and interfaces that comprise the DAL Layer of the
* ChipInfo driver.
*
* Only the interfaces declared shall be used by the client for
* accessing the ChipInfo DAL driver.
*/

/*
===========================================================================

FILE:         DDIChipInfo.h

DESCRIPTION:  
  This is the DAL interface specification for the chip driver/service.

===========================================================================
             Copyright (c) 2015 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
============================================================================
                             Edit History
  $Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/DDIChipInfo.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/20/15   shm     Added API to query foundry.
07/11/11   pbi     Fixed warnings.

===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "DALSys.h"
#include "DALDeviceId.h"

/** 
  @addtogroup macros
  @{ 
*/ 

/**
 * Macro that defines the interface version.
 */
#define DALCHIPINFO_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/**
 * Maximum length of a chip id string.  Can be used by clients when
 * declaring space for DalChip_GetChipIdString.
 */
#define DALCHIPINFO_MAX_ID_LENGTH 16


/**
 * Macro to generate the hardware version number from a given major
 * and minor number.  Used when comparing against return value of
 * hw_version().  For example to check if this is at least version 1.2:
 *   DalChipInfo_GetVersion(h, &version);
 *   if (version >= DALCHIPINFO_VERSION(1,2)) ...
 */
#define DALCHIPINFO_VERSION(major, minor)  (((major) << 16) | (minor))

/**
 * Masks used to indicate which modem 
 * hardware is supported on the platform. 
 */
#define DALCHIPINFO_MODEM_1X         0x0001
#define DALCHIPINFO_MODEM_EVDO       0x0002
#define DALCHIPINFO_MODEM_GSM        0x0004
#define DALCHIPINFO_MODEM_UMTS       0x0008
#define DALCHIPINFO_MODEM_CGPS       0x0010
#define DALCHIPINFO_MODEM_EVDO_REVA  0x0020
#define DALCHIPINFO_MODEM_EVDO_REVB  0x0040
#define DALCHIPINFO_MODEM_LTE        0x0080
#define DALCHIPINFO_MODEM_GNSS       0x0100
#define DALCHIPINFO_MODEM_EHRPD      0x0200

/** 
  @}
*/ /* end_group macros */

/* ============================================================================
**             Public Data Structures used by the ChipInfo Dal
** ========================================================================== */
/** 
  @addtogroup data_structures
  @{ 
*/

/**
 * Chip version type with major number in the upper 16 bits and minor
 * number in the lower 16 bits.  For example:
 *   1.0 -> 0x00010000
 *   2.3 -> 0x00020003
 * The DALCHIP_VERSION macro should be used to generate the appropriate
 * comparison value as the format is not guaranteed to remain unchanged.
 */
typedef uint32 DalChipInfoVersionType;


/**
 * DalChipInfoModemType. Stores the info about the modem 
 * hardware supported on the platform. 
 */
typedef uint32 DalChipInfoModemType;

/**
Chip identification type.  Any new ids must be added to the end.
*/
typedef enum
{
  DALCHIPINFO_ID_UNKNOWN     = 0,
  DALCHIPINFO_ID_MDM1000     = 1,
  DALCHIPINFO_ID_ESM6235     = 2,
  DALCHIPINFO_ID_QSC6240     = 3,
  DALCHIPINFO_ID_MSM6245     = 4,
  DALCHIPINFO_ID_MSM6255     = 5,
  DALCHIPINFO_ID_MSM6255A    = 6,
  DALCHIPINFO_ID_MSM6260     = 7,
  DALCHIPINFO_ID_MSM6246     = 8,
  DALCHIPINFO_ID_QSC6270     = 9,
  DALCHIPINFO_ID_MSM6280     = 10,
  DALCHIPINFO_ID_MSM6290     = 11,
  DALCHIPINFO_ID_MSM7200     = 12,
  DALCHIPINFO_ID_MSM7201     = 13,
  DALCHIPINFO_ID_ESM7205     = 14,
  DALCHIPINFO_ID_ESM7206     = 15,
  DALCHIPINFO_ID_MSM7200A    = 16,
  DALCHIPINFO_ID_MSM7201A    = 17,
  DALCHIPINFO_ID_ESM7205A    = 18,
  DALCHIPINFO_ID_ESM7206A    = 19,
  DALCHIPINFO_ID_ESM7225     = 20,
  DALCHIPINFO_ID_MSM7225     = 21,
  DALCHIPINFO_ID_MSM7500     = 22,
  DALCHIPINFO_ID_MSM7500A    = 23,
  DALCHIPINFO_ID_MSM7525     = 24,
  DALCHIPINFO_ID_MSM7600     = 25,
  DALCHIPINFO_ID_MSM7601     = 26,
  DALCHIPINFO_ID_MSM7625     = 27,
  DALCHIPINFO_ID_MSM7800     = 28,
  DALCHIPINFO_ID_MDM8200     = 29,
  DALCHIPINFO_ID_QSD8650     = 30,
  DALCHIPINFO_ID_MDM8900     = 31,
  DALCHIPINFO_ID_QST1000     = 32,
  DALCHIPINFO_ID_QST1005     = 33,
  DALCHIPINFO_ID_QST1100     = 34,
  DALCHIPINFO_ID_QST1105     = 35,
  DALCHIPINFO_ID_QST1500     = 40,
  DALCHIPINFO_ID_QST1600     = 41,
  DALCHIPINFO_ID_QST1700     = 42,
  DALCHIPINFO_ID_QSD8250     = 36,
  DALCHIPINFO_ID_QSD8550     = 37,
  DALCHIPINFO_ID_QSD8850     = 38,
  DALCHIPINFO_ID_MDM2000     = 39,
  DALCHIPINFO_ID_MSM7227     = 43,
  DALCHIPINFO_ID_MSM7627     = 44,
  DALCHIPINFO_ID_QSC6165     = 45,
  DALCHIPINFO_ID_QSC6175     = 46,
  DALCHIPINFO_ID_QSC6185     = 47,
  DALCHIPINFO_ID_QSC6195     = 48,
  DALCHIPINFO_ID_QSC6285     = 49,
  DALCHIPINFO_ID_QSC6295     = 50,
  DALCHIPINFO_ID_QSC6695     = 51,
  DALCHIPINFO_ID_ESM6246     = 52,
  DALCHIPINFO_ID_ESM6290     = 53,
  DALCHIPINFO_ID_ESC6270     = 54,
  DALCHIPINFO_ID_ESC6240     = 55,
  DALCHIPINFO_ID_MDM8220     = 56,
  DALCHIPINFO_ID_MDM9200     = 57,
  DALCHIPINFO_ID_MDM9600     = 58,
  DALCHIPINFO_ID_MSM7630     = 59,
  DALCHIPINFO_ID_MSM7230     = 60,
  DALCHIPINFO_ID_ESM7227     = 61,
  DALCHIPINFO_ID_MSM7625D1   = 62,
  DALCHIPINFO_ID_MSM7225D1   = 63,
  DALCHIPINFO_ID_QSD8250A    = 64,
  DALCHIPINFO_ID_QSD8650A    = 65,
  DALCHIPINFO_ID_MSM7625D2   = 66,
  DALCHIPINFO_ID_MSM7227D1   = 67,
  DALCHIPINFO_ID_MSM7627D1   = 68,
  DALCHIPINFO_ID_MSM7627D2   = 69,
  DALCHIPINFO_ID_MSM8260     = 70,
  DALCHIPINFO_ID_MSM8660     = 71,
  DALCHIPINFO_ID_MDM8200A    = 72,
  DALCHIPINFO_ID_QSC6155     = 73,
  DALCHIPINFO_ID_MSM8255     = 74,
  DALCHIPINFO_ID_MSM8655     = 75,
  DALCHIPINFO_ID_ESC6295     = 76,
  DALCHIPINFO_ID_MDM3000     = 77,
  DALCHIPINFO_ID_MDM6200     = 78,
  DALCHIPINFO_ID_MDM6600     = 79,
  DALCHIPINFO_ID_MDM6210     = 80,
  DALCHIPINFO_ID_MDM6610     = 81,
  DALCHIPINFO_ID_QSD8672     = 82,
  DALCHIPINFO_ID_MDM6215     = 83,
  DALCHIPINFO_ID_MDM6615     = 84,
  DALCHIPINFO_ID_APQ8055     = 85,
  DALCHIPINFO_ID_APQ8060     = 86,
  DALCHIPINFO_ID_MSM8960     = 87,
  DALCHIPINFO_ID_MSM7225A    = 88,
  DALCHIPINFO_ID_MSM7625A    = 89,
  DALCHIPINFO_ID_MSM7227A    = 90,
  DALCHIPINFO_ID_MSM7627A    = 91,
  DALCHIPINFO_ID_ESM7227A    = 92,
  DALCHIPINFO_ID_QSC6195D2   = 93,
  DALCHIPINFO_ID_FSM9200     = 94,
  DALCHIPINFO_ID_FSM9800     = 95,
  DALCHIPINFO_ID_MSM7225AD1  = 96,
  DALCHIPINFO_ID_MSM7227AD1  = 97,
  DALCHIPINFO_ID_MSM7225AA   = 98,
  DALCHIPINFO_ID_MSM7225AAD1 = 99,
  DALCHIPINFO_ID_MSM7625AA   = 100,
  DALCHIPINFO_ID_MSM7227AA   = 101,
  DALCHIPINFO_ID_MSM7227AAD1 = 102,
  DALCHIPINFO_ID_MSM7627AA   = 103,
  DALCHIPINFO_ID_MDM9615     = 104,
  DALCHIPINFO_ID_MDM9615M    = DALCHIPINFO_ID_MDM9615,
  DALCHIPINFO_ID_MDM8215     = 106,
  DALCHIPINFO_ID_MDM9215     = 107,  
  DALCHIPINFO_ID_MDM9215M    = DALCHIPINFO_ID_MDM9215,
  DALCHIPINFO_ID_APQ8064     = 109,
  DALCHIPINFO_ID_QSC6270D1   = 110,
  DALCHIPINFO_ID_QSC6240D1   = 111,
  DALCHIPINFO_ID_ESC6270D1   = 112,
  DALCHIPINFO_ID_ESC6240D1   = 113,
  DALCHIPINFO_ID_MDM6270     = 114,
  DALCHIPINFO_ID_MDM6270D1   = 115,
  DALCHIPINFO_ID_MSM8930     = 116,
  DALCHIPINFO_ID_MSM8630     = 117,  
  DALCHIPINFO_ID_MSM8230     = 118,
  DALCHIPINFO_ID_APQ8030     = 119, 
  DALCHIPINFO_ID_MSM8627     = 120, 
  DALCHIPINFO_ID_MSM8227     = 121, 
  DALCHIPINFO_ID_MSM8660A    = 122, 
  DALCHIPINFO_ID_MSM8260A    = 123,
  DALCHIPINFO_ID_APQ8060A    = 124,
  DALCHIPINFO_ID_MPQ8062     = 125,
  DALCHIPINFO_ID_MSM8974     = 126,
  DALCHIPINFO_ID_MSM8225     = 127,
  DALCHIPINFO_ID_MSM8225D1   = 128,
  DALCHIPINFO_ID_MSM8625     = 129,
  DALCHIPINFO_ID_MPQ8064     = 130,
  DALCHIPINFO_ID_MSM7225AB   = 131,
  DALCHIPINFO_ID_MSM7225ABD1 = 132,
  DALCHIPINFO_ID_MSM7625AB   = 133,
  DALCHIPINFO_ID_MDM9625     = 134,
  DALCHIPINFO_ID_MSM7125A    = 135,
  DALCHIPINFO_ID_MSM7127A    = 136,
  DALCHIPINFO_ID_MSM8125AB   = 137,
  DALCHIPINFO_ID_MSM8960AB   = 138,
  DALCHIPINFO_ID_APQ8060AB   = 139,
  DALCHIPINFO_ID_MSM8260AB   = 140,
  DALCHIPINFO_ID_MSM8660AB   = 141,
  DALCHIPINFO_ID_MSM8930AA   = 142,
  DALCHIPINFO_ID_MSM8630AA   = 143,
  DALCHIPINFO_ID_MSM8230AA   = 144,
  DALCHIPINFO_ID_MSM8626     = 145,
  DALCHIPINFO_ID_MPQ8092     = 146,
  DALCHIPINFO_ID_MSM8610     = 147,
  DALCHIPINFO_ID_MDM8225     = 148,
  DALCHIPINFO_ID_MDM9225     = 149,
  DALCHIPINFO_ID_MDM9225M    = 150,
  DALCHIPINFO_ID_MDM8225M    = 151,
  DALCHIPINFO_ID_MDM9625M    = 152,
  DALCHIPINFO_ID_APQ8064_V2PRIME = 153,
  DALCHIPINFO_ID_MSM8930AB   = 154,
  DALCHIPINFO_ID_MSM8630AB   = 155,
  DALCHIPINFO_ID_MSM8230AB   = 156,
  DALCHIPINFO_ID_APQ8030AB   = 157,
  DALCHIPINFO_ID_MSM8226     = 158,
  DALCHIPINFO_ID_MSM8526     = 159,
  DALCHIPINFO_ID_APQ8030AA   = 160,
  DALCHIPINFO_ID_MSM8110     = 161,  
  DALCHIPINFO_ID_MSM8210     = 162,  
  DALCHIPINFO_ID_MSM8810     = 163,  
  DALCHIPINFO_ID_MSM8212     = 164,  
  DALCHIPINFO_ID_MSM8612     = 165,  
  DALCHIPINFO_ID_MSM8112     = 166,
  DALCHIPINFO_ID_MSM8125     = 167,
  DALCHIPINFO_ID_MSM8225Q    = 168,
  DALCHIPINFO_ID_MSM8625Q    = 169,
  DALCHIPINFO_ID_MSM8125Q    = 170,
  DALCHIPINFO_ID_MDM9310     = 171,
  DALCHIPINFO_ID_APQ8064_SLOW_PRIME = 172,
  DALCHIPINFO_ID_MDM8110M    = 173,
  DALCHIPINFO_ID_MDM8615M    = 174,
  DALCHIPINFO_ID_MDM9320     = 175,  
  DALCHIPINFO_ID_MDM9225_1   = 176,  
  DALCHIPINFO_ID_MDM9225M_1  = 177,
  DALCHIPINFO_ID_APQ8084     = 178,
  DALCHIPINFO_ID_MSM8130     = 179,
  DALCHIPINFO_ID_MSM8130AA   = 180,
  DALCHIPINFO_ID_MSM8130AB   = 181,
  DALCHIPINFO_ID_MSM8627AA   = 182,
  DALCHIPINFO_ID_MSM8227AA   = 183,
  DALCHIPINFO_ID_APQ8074     = 184,
  DALCHIPINFO_ID_MSM8274     = 185,
  DALCHIPINFO_ID_MSM8674     = 186,
  DALCHIPINFO_ID_MDM9635     = 187,
  DALCHIPINFO_ID_FSM9900     = 188,
  DALCHIPINFO_ID_FSM9965     = 189,
  DALCHIPINFO_ID_FSM9955     = 190,
  DALCHIPINFO_ID_FSM9950     = 191,
  DALCHIPINFO_ID_FSM9915     = 192,
  DALCHIPINFO_ID_FSM9910     = 193,
  DALCHIPINFO_ID_MSM8974_PRO = 194,
  DALCHIPINFO_ID_MSM8962     = 195,
  DALCHIPINFO_ID_MSM8262     = 196,
  DALCHIPINFO_ID_APQ8062     = 197,
  DALCHIPINFO_ID_MSM8126     = 198,
  DALCHIPINFO_ID_APQ8026     = 199,
  DALCHIPINFO_ID_MSM8926     = 200,
  DALCHIPINFO_ID_MSM8326     = 205,
  DALCHIPINFO_ID_MSM8916     = 206,
  DALCHIPINFO_ID_MSM8994     = 207,
  DALCHIPINFO_ID_APQ8074_AA  = 208,
  DALCHIPINFO_ID_APQ8074_AB  = 209,
  DALCHIPINFO_ID_APQ8074_PRO = 210,
  DALCHIPINFO_ID_MSM8274_AA  = 211,
  DALCHIPINFO_ID_MSM8274_AB  = 212,
  DALCHIPINFO_ID_MSM8274_PRO = 213,
  DALCHIPINFO_ID_MSM8674_AA  = 214,
  DALCHIPINFO_ID_MSM8674_AB  = 215,
  DALCHIPINFO_ID_MSM8674_PRO = 216,
  DALCHIPINFO_ID_MSM8974_AA  = 217,
  DALCHIPINFO_ID_MSM8974_AB  = 218,
  DALCHIPINFO_ID_APQ8028     = 219,
  DALCHIPINFO_ID_MSM8128     = 220,
  DALCHIPINFO_ID_MSM8228     = 221,
  DALCHIPINFO_ID_MSM8528     = 222,
  DALCHIPINFO_ID_MSM8628     = 223,
  DALCHIPINFO_ID_MSM8928     = 224,
  DALCHIPINFO_ID_MSM8510     = 225,
  DALCHIPINFO_ID_MSM8512     = 226,
  DALCHIPINFO_ID_MDM9630     = 227,
  DALCHIPINFO_ID_MDM9635M    = DALCHIPINFO_ID_MDM9635,
  DALCHIPINFO_ID_MDM9230     = 228,
  DALCHIPINFO_ID_MDM9235M    = 229,
  DALCHIPINFO_ID_MDM8630     = 230,
  DALCHIPINFO_ID_MDM9330     = 231,
  DALCHIPINFO_ID_MPQ8091     = 232,
  DALCHIPINFO_ID_MSM8936     = 233,
  DALCHIPINFO_ID_MDM9240     = 234,
  DALCHIPINFO_ID_MDM9340     = 235,
  DALCHIPINFO_ID_MDM9640     = 236,
  DALCHIPINFO_ID_MDM9245M    = 237,
  DALCHIPINFO_ID_MDM9645M    = 238,
  DALCHIPINFO_ID_MSM8939     = 239,
  DALCHIPINFO_ID_APQ8036     = 240,
  DALCHIPINFO_ID_APQ8039     = 241,
  DALCHIPINFO_ID_MSM8236     = 242,
  DALCHIPINFO_ID_MSM8636     = 243,
  DALCHIPINFO_ID_APQ8064_AU  = 244,
  DALCHIPINFO_ID_MSM8909     = 245,
  DALCHIPINFO_ID_MSM8996     = 246,
  DALCHIPINFO_ID_APQ8016     = 247,
  DALCHIPINFO_ID_MSM8216     = 248,
  DALCHIPINFO_ID_MSM8116     = 249,
  DALCHIPINFO_ID_MSM8616     = 250,
  DALCHIPINFO_ID_MSM8992     = 251,
  DALCHIPINFO_ID_APQ8092     = 252,
  DALCHIPINFO_ID_APQ8094     = 253,
  DALCHIPINFO_ID_FSM9008     = 254,
  DALCHIPINFO_ID_FSM9010     = 255,
  DALCHIPINFO_ID_FSM9016     = 256,
  DALCHIPINFO_ID_FSM9055     = 257,
  DALCHIPINFO_ID_MSM8209     = 258,
  DALCHIPINFO_ID_MSM8208     = 259,
  DALCHIPINFO_ID_MDM9209     = 260,
  DALCHIPINFO_ID_MDM9309     = 261,
  DALCHIPINFO_ID_MDM9609     = 262,
  DALCHIPINFO_ID_MSM8239     = 263,
  DALCHIPINFO_ID_MSM8952     = 264,
  DALCHIPINFO_ID_APQ8009     = 265,
  DALCHIPINFO_ID_MSM8956     = 266,
  DALCHIPINFO_ID_QDF2432     = 267,
  DALCHIPINFO_ID_MSM8929     = 268,
  DALCHIPINFO_ID_MSM8629     = 269,
  DALCHIPINFO_ID_MSM8229     = 270,
  DALCHIPINFO_ID_APQ8029     = 271,
  DALCHIPINFO_ID_QCA9618     = 272,
  DALCHIPINFO_ID_QCA9619     = 273,
  DALCHIPINFO_ID_APQ8056     = 274,
  DALCHIPINFO_ID_MSM8609     = 275,
  DALCHIPINFO_ID_FSM9916     = 276,
  DALCHIPINFO_ID_APQ8076     = 277,
  DALCHIPINFO_ID_MSM8976     = 278,
  DALCHIPINFO_ID_MDM9650     = 279,
  DALCHIPINFO_ID_IPQ8065     = 280,
  DALCHIPINFO_ID_IPQ8069     = 281,
  DALCHIPINFO_ID_MSM8939_BC  = 282,
  DALCHIPINFO_ID_MDM9250     = 283,
  DALCHIPINFO_ID_MDM9255     = 284,
  DALCHIPINFO_ID_MDM9350     = 285,
  DALCHIPINFO_ID_MDM9655     = 286,
  DALCHIPINFO_ID_IPQ4028     = 287,
  DALCHIPINFO_ID_IPQ4029     = 288,
  DALCHIPINFO_ID_APQ8052     = 289,
  DALCHIPINFO_ID_MDM9607     = 290,
  DALCHIPINFO_ID_APQ8096     = 291,
  DALCHIPINFO_ID_MSM8998     = 292,
  DALCHIPINFO_ID_MSM8953     = 293,
  DALCHIPINFO_ID_MSM8937     = 294,
  DALCHIPINFO_ID_APQ8037     = 295,
  DALCHIPINFO_ID_MDM8207     = 296,
  DALCHIPINFO_ID_MDM9207     = 297,
  DALCHIPINFO_ID_MDM9307     = 298,
  DALCHIPINFO_ID_MDM9628     = 299,
  DALCHIPINFO_ID_MSM8909W    = 300,
  DALCHIPINFO_ID_APQ8009W    = 301,
  DALCHIPINFO_ID_MSM8996L    = 302,
  DALCHIPINFO_ID_MSM8917     = 303,
  DALCHIPINFO_ID_APQ8053     = 304,
  DALCHIPINFO_ID_MSM8996_PRO = 305,
  DALCHIPINFO_ID_MSM8997     = 306,
  DALCHIPINFO_ID_APQ8017     = 307,
  DALCHIPINFO_ID_MSM8217     = 308,
  DALCHIPINFO_ID_MSM8617     = 309,

  DALCHIPINFO_NUM_IDS        = 310,
  DALCHIPINFO_ID_32BITS      = 0x7FFFFFF
} DalChipInfoIdType;

/**
Chip Family type.  The family type of the chip we are running on. 
Each family may include multiple chip ids. 
*/
typedef enum
{
  DALCHIPINFO_FAMILY_UNKNOWN     = 0,
  DALCHIPINFO_FAMILY_MSM6246     = 1,
  DALCHIPINFO_FAMILY_MSM6260     = 2,
  DALCHIPINFO_FAMILY_QSC6270     = 3,
  DALCHIPINFO_FAMILY_MSM6280     = 4,
  DALCHIPINFO_FAMILY_MSM6290     = 5,
  DALCHIPINFO_FAMILY_MSM7200     = 6,
  DALCHIPINFO_FAMILY_MSM7500     = 7,
  DALCHIPINFO_FAMILY_MSM7600     = 8,
  DALCHIPINFO_FAMILY_MSM7625     = 9,
  DALCHIPINFO_FAMILY_MSM7X30     = 10,
  DALCHIPINFO_FAMILY_MSM7800     = 11,
  DALCHIPINFO_FAMILY_MDM8200     = 12,
  DALCHIPINFO_FAMILY_QSD8650     = 13,
  DALCHIPINFO_FAMILY_MSM7627     = 14,
  DALCHIPINFO_FAMILY_QSC6695     = 15,
  DALCHIPINFO_FAMILY_MDM9X00     = 16,
  DALCHIPINFO_FAMILY_QSD8650A    = 17,
  DALCHIPINFO_FAMILY_MSM8X60     = 18,
  DALCHIPINFO_FAMILY_MDM8200A    = 19,
  DALCHIPINFO_FAMILY_QSD8672     = 20,
  DALCHIPINFO_FAMILY_MDM6615     = 21,
  DALCHIPINFO_FAMILY_MSM8660     = DALCHIPINFO_FAMILY_MSM8X60,
  DALCHIPINFO_FAMILY_MSM8960     = 22,
  DALCHIPINFO_FAMILY_MSM7625A    = 23,
  DALCHIPINFO_FAMILY_MSM7627A    = 24,
  DALCHIPINFO_FAMILY_MDM9X15     = 25,
  DALCHIPINFO_FAMILY_MSM8930     = 26,
  DALCHIPINFO_FAMILY_MSM8630     = DALCHIPINFO_FAMILY_MSM8930,
  DALCHIPINFO_FAMILY_MSM8230     = DALCHIPINFO_FAMILY_MSM8930,
  DALCHIPINFO_FAMILY_APQ8030     = DALCHIPINFO_FAMILY_MSM8930,
  DALCHIPINFO_FAMILY_MSM8627     = 30,
  DALCHIPINFO_FAMILY_MSM8227     = DALCHIPINFO_FAMILY_MSM8627,
  DALCHIPINFO_FAMILY_MSM8974     = 32,
  DALCHIPINFO_FAMILY_MSM8625     = 33,
  DALCHIPINFO_FAMILY_MSM8225     = DALCHIPINFO_FAMILY_MSM8625,
  DALCHIPINFO_FAMILY_APQ8064     = 34,
  DALCHIPINFO_FAMILY_MDM9x25     = 35,
  DALCHIPINFO_FAMILY_MSM8960AB   = 36,
  DALCHIPINFO_FAMILY_MSM8930AB   = 37,
  DALCHIPINFO_FAMILY_MSM8x10     = 38,
  DALCHIPINFO_FAMILY_MPQ8092     = 39,
  DALCHIPINFO_FAMILY_MSM8x26     = 40,
  DALCHIPINFO_FAMILY_MSM8225Q    = 41,
  DALCHIPINFO_FAMILY_MSM8625Q    = 42,
  DALCHIPINFO_FAMILY_APQ8x94     = 43,
  DALCHIPINFO_FAMILY_APQ8084     = DALCHIPINFO_FAMILY_APQ8x94,
  DALCHIPINFO_FAMILY_MSM8x32     = 44,
  DALCHIPINFO_FAMILY_MDM9x35     = 45,
  DALCHIPINFO_FAMILY_MSM8974_PRO = 46,
  DALCHIPINFO_FAMILY_FSM9900     = 47,
  DALCHIPINFO_FAMILY_MSM8x62     = 48,
  DALCHIPINFO_FAMILY_MSM8926     = 49,
  DALCHIPINFO_FAMILY_MSM8994     = 50,
  DALCHIPINFO_FAMILY_IPQ8064     = 51,
  DALCHIPINFO_FAMILY_MSM8916     = 52,
  DALCHIPINFO_FAMILY_MSM8936     = 53,
  DALCHIPINFO_FAMILY_MDM9x45     = 54,
  DALCHIPINFO_FAMILY_APQ8096     = 55,
  DALCHIPINFO_FAMILY_MSM8996     = 56,
  DALCHIPINFO_FAMILY_MSM8992     = 57,
  DALCHIPINFO_FAMILY_MSM8909     = 58,
  DALCHIPINFO_FAMILY_FSM90xx     = 59,
  DALCHIPINFO_FAMILY_MSM8952     = 60,
  DALCHIPINFO_FAMILY_QDF2432     = 61,
  DALCHIPINFO_FAMILY_MSM8929     = 62,
  DALCHIPINFO_FAMILY_MSM8956     = 63,
  DALCHIPINFO_FAMILY_MSM8976     = DALCHIPINFO_FAMILY_MSM8956,
  DALCHIPINFO_FAMILY_QCA961x     = 64,
  DALCHIPINFO_FAMILY_MDM9x55     = 65,
  DALCHIPINFO_FAMILY_MDM9x07     = 66,
  DALCHIPINFO_FAMILY_MSM8998     = 67,
  DALCHIPINFO_FAMILY_MSM8953     = 68,
  DALCHIPINFO_FAMILY_MSM8993     = 69,
  DALCHIPINFO_FAMILY_MSM8937     = 70,
  DALCHIPINFO_FAMILY_MSM8917     = 71,
  DALCHIPINFO_FAMILY_MSM8996_PRO = 72,
  DALCHIPINFO_FAMILY_MSM8997     = 73,

  DALCHIPINFO_NUM_FAMILIES       = 74,
  DALCHIPINFO_FAMILY_32BITS      = 0x7FFFFFF
} DalChipInfoFamilyType;

/**
Chip Foundry type.  
*/
typedef enum
{
  DALCHIPINFO_FOUNDRYID_UNKNOWN  = 0,
  DALCHIPINFO_FOUNDRYID_TSMC     = 1,
  DALCHIPINFO_FOUNDRYID_GF       = 2,
  DALCHIPINFO_FOUNDRYID_SS       = 3,
  DALCHIPINFO_FOUNDRYID_IBM      = 4,
  DALCHIPINFO_FOUNDRYID_UMC      = 5,
  DALCHIPINFO_FOUNDRYID_SMIC     = 6,

  DALCHIPINFO_NUM_FOUNDRYIDS     = 7,
  DALCHIPINFO_FOUNDRYID_32BITS   = 0x7FFFFFF
} DalChipInfoFoundryIdType;

/** 
  @}
*/ /* end_group Data Structures */


/** @cond */

typedef struct DalChipInfo DalChipInfo;
struct DalChipInfo
{
  DalDevice DalChipInfoDevice;
  DALResult (*GetChipVersion)(DalDeviceHandle * _h,  uint32  nNotUsed,  DalChipInfoVersionType * pnVersion);
  DALResult (*GetRawChipVersion)(DalDeviceHandle * _h,  uint32  nNotUsed,  uint32 * pnVersion);
  DALResult (*GetChipId)(DalDeviceHandle * _h,  uint32  nNotUsed,  DalChipInfoIdType * peId);
  DALResult (*GetRawChipId)(DalDeviceHandle * _h,  uint32  nNotUsed,  uint32 * pnId);
  DALResult (*GetChipIdString)(DalDeviceHandle * _h,  char * szIdString,  uint32  nMaxLength);
  DALResult (*GetChipFamily)(DalDeviceHandle * _h,  uint32  nNotUsed,  DalChipInfoFamilyType * peFamily);
  DALResult (*GetModemSupport)(DalDeviceHandle * _h,  uint32  nNotUsed,  DalChipInfoModemType * pnModem);
  DALResult (*GetFoundryId)(DalDeviceHandle * _h,  uint32  nNotUsed, DalChipInfoFoundryIdType * peFoundryId);
};

typedef struct DalChipInfoHandle DalChipInfoHandle; 
struct DalChipInfoHandle 
{
  uint32             dwDalHandleId;
  const DalChipInfo *pVtbl;
  void              *pClientCtxt;
};

#define DAL_ChipInfoDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALCHIPINFO_INTERFACE_VERSION,hDalDevice)

/** @endcond */


/** 
  @addtogroup functions
  @{
*/


/* ============================================================================
**  Function : DalChipInfo_GetChipVersion
** ============================================================================
*/
/**
  Returns the version of the chip.
    
  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  pnVersion  Pointer to a 
                             DalChipInfoVersionType type
                             structure passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.
  
  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetChipVersion(DalDeviceHandle * _h,  DalChipInfoVersionType * pnVersion)
{
  if(DALISREMOTEHANDLE(_h))
  {
    DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
    return hRemote->pVtbl->FCN_2(DALVTBLIDX(((DalChipInfoHandle *)_h)->pVtbl, GetChipVersion ), _h, (uint32 )0, (uint32 *)pnVersion);
  }
  return ((DalChipInfoHandle *)_h)->pVtbl->GetChipVersion( _h, 0, pnVersion);
}


/* ============================================================================
**  Function : DalChipInfo_GetRawChipVersion
** ============================================================================
*/
/**
  Returns the version of the chip as read from the hardware register.
    
  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  pnVersion  Pointer to a uint32 passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.
  
  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetRawChipVersion(DalDeviceHandle * _h,  uint32 * pnVersion)
{
  if(DALISREMOTEHANDLE(_h))
  {
    DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
    return hRemote->pVtbl->FCN_2(DALVTBLIDX(((DalChipInfoHandle *)_h)->pVtbl, GetRawChipVersion ), _h, (uint32 )0, (uint32 *)pnVersion);
  }
  return ((DalChipInfoHandle *)_h)->pVtbl->GetRawChipVersion( _h, 0, pnVersion);
}


/* ============================================================================
**  Function : DalChipInfo_GetChipId
** ============================================================================
*/
/**
  Returns the ChipId.
    
  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  peId       Pointer to a 
                             DalChipInfoIdType type
                             structure passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.
  
  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetChipId(DalDeviceHandle * _h,  DalChipInfoIdType * peId)
{
  if(DALISREMOTEHANDLE(_h))
  {
    DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
    return hRemote->pVtbl->FCN_2(DALVTBLIDX(((DalChipInfoHandle *)_h)->pVtbl, GetChipId ), _h, (uint32 )0, (uint32 *)peId);
  }
  return ((DalChipInfoHandle *)_h)->pVtbl->GetChipId( _h, 0, peId);
}


/* ============================================================================
**  Function : DalChipInfo_GetRawChipId
** ============================================================================
*/
/**
  Returns the ChipId as read from the hardware register.
    
  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  pnId       Pointer to a uint32 passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.
  
  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetRawChipId(DalDeviceHandle * _h,  uint32 * pnId)
{
  if(DALISREMOTEHANDLE(_h))
  {
    DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
    return hRemote->pVtbl->FCN_2(DALVTBLIDX(((DalChipInfoHandle *)_h)->pVtbl, GetRawChipId ), _h, (uint32 )0, (uint32 *)pnId);
  }
  return ((DalChipInfoHandle *)_h)->pVtbl->GetRawChipId( _h, 0, pnId);
}


/* ============================================================================
**  Function : DalChipInfo_GetChipIdString
** ============================================================================
*/
/**
  Returns the string representing the chip name.
    
  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  szIdString Pointer passed by the caller that
                             will be populated by the driver.
  @param[in]      nMaxLength Length of the string  to be passed by
                             the caller. DALCHIPINFO_MAX_ID_LENGTH
                             is the maximum length supported.

  @return
  DAL_SUCCESS always.
  
  @dependencies
  None.
 
  @sa
  DALCHIPINFO_MAX_ID_LENGTH
*/
static __inline DALResult
DalChipInfo_GetChipIdString(DalDeviceHandle * _h,  char * szIdString,  uint32  nMaxLength)
{
  if(DALISREMOTEHANDLE(_h))
  {
    DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
    return hRemote->pVtbl->FCN_9(DALVTBLIDX(((DalChipInfoHandle *)_h)->pVtbl, GetChipIdString ), _h, (char * )szIdString, nMaxLength);
  }
  return ((DalChipInfoHandle *)_h)->pVtbl->GetChipIdString( _h, szIdString, nMaxLength);
}


/* ============================================================================
**  Function : DalChipInfo_GetChipFamily
** ============================================================================
*/
/**
  Returns the family of the chip.
    
  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  peFamily   Pointer to a DalChipInfoFamilyType type
                             structure passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.
  
  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetChipFamily(DalDeviceHandle * _h,  DalChipInfoFamilyType * peFamily)
{
  if(DALISREMOTEHANDLE(_h))
  {
    DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
    return hRemote->pVtbl->FCN_2(DALVTBLIDX(((DalChipInfoHandle *)_h)->pVtbl, GetChipFamily ), _h, (uint32 )0, (uint32 *)peFamily);
  }
  return ((DalChipInfoHandle *)_h)->pVtbl->GetChipFamily( _h, 0, peFamily);
}


/* ============================================================================
**  Function : DalChipInfo_GetModemSupport
** ============================================================================
*/
/**
  Returns the Modem support information of the chip.
    
  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  pnModem    Pointer to a DalChipInfoModemType type
                             structure passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.
  
  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetModemSupport(DalDeviceHandle * _h,  DalChipInfoModemType * pnModem)
{
  if(DALISREMOTEHANDLE(_h))
  {
    DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
    return hRemote->pVtbl->FCN_2(DALVTBLIDX(((DalChipInfoHandle *)_h)->pVtbl, GetModemSupport ), _h, (uint32 )0, (uint32 *)pnModem);
  }
  return ((DalChipInfoHandle *)_h)->pVtbl->GetModemSupport( _h, 0, pnModem);
}


/* ============================================================================
**  Function : DalChipInfo_GetFoundryId
** ============================================================================
*/
/**
  Returns the FoundryId.
    
  @param[in,out]  _h         Dal Device Handle given by the caller.
  @param[in,out]  peId       Pointer to a DalChipInfoFoundryIdType type
                             structure passed by the caller that
                             will be populated by the driver.

  @return
  DAL_SUCCESS always.
  
  @dependencies
  None.

*/
static __inline DALResult
DalChipInfo_GetFoundryId(DalDeviceHandle * _h,  DalChipInfoFoundryIdType * peId)
{
  if(DALISREMOTEHANDLE(_h))
  {
    DalRemoteHandle *hRemote = (DalRemoteHandle *)DALPREPREMOTEHANDLE(_h);
    return hRemote->pVtbl->FCN_2(DALVTBLIDX(((DalChipInfoHandle *)_h)->pVtbl, GetFoundryId ), _h, (uint32 )0, (uint32 *)peId);
  }
  return ((DalChipInfoHandle *)_h)->pVtbl->GetFoundryId( _h, 0, peId);
}


/*=========================================================================
FUNCTION  DalChipInfo_ChipVersion
===========================================================================*/
/**
  Returns the version of the chip. 

  This function is a wrapper around DalChipInfo_GetChipVersion. It provides
  the chip version without the code overhead of attaching to the DAL.

  @return
  DalChipInfoVersionType version of the chip.

  NULL  -- DAL device attach failed.

  @sa
  DalChipInfo_GetChipVersion
*/
static __inline DalChipInfoVersionType
DalChipInfo_ChipVersion(void)
{
  static DalDeviceHandle       *phChipInfo = NULL;
  DALResult                     eResult;
  DalChipInfoVersionType        nVersion;

  if (phChipInfo == NULL)
  {
    eResult = 
      DAL_ChipInfoDeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);

    if (eResult != DAL_SUCCESS)
    {
      return (DalChipInfoVersionType)NULL;
    }
  }

  DalChipInfo_GetChipVersion(phChipInfo, &nVersion);

  return nVersion;
}


/*=========================================================================
FUNCTION  DalChipInfo_ChipId
===========================================================================*/
/**
  Returns the ChipId. 

  This function is a wrapper around DalChipInfo_GetChipId. It provides
  the chip ID without the code overhead of attaching to the DAL.

  @return
  DalChipInfoIdType   neId

  DALCHIPINFO_ID_UNKNOWN  -- DAL device attach failed.

  @sa
  DalChipInfo_GetChipId
*/
static __inline DalChipInfoIdType
DalChipInfo_ChipId(void)
{
  static DalDeviceHandle       *phChipInfo = NULL;
  DALResult                     eResult;
  DalChipInfoIdType             neId;

  if (phChipInfo == NULL)
  {
    eResult = 
      DAL_ChipInfoDeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);

    if (eResult != DAL_SUCCESS)
    {
      return DALCHIPINFO_ID_UNKNOWN;
    }
  }

  DalChipInfo_GetChipId(phChipInfo, &neId);

  return neId;
}


/*=========================================================================
FUNCTION  DalChipInfo_ChipFamily
===========================================================================*/
/**
  Returns the family of the chip.

  This function is a wrapper around DalChipInfo_GetChipFamily. It provides
  the chip family without the code overhead of attaching to the DAL.

  @return
  DalChipInfoFamilyType   peFamily

  DALCHIPINFO_FAMILY_UNKNOWN  -- DAL device attach failed.

  @sa
  DalChipInfo_GetChipFamily
*/
static __inline DalChipInfoFamilyType
DalChipInfo_ChipFamily(void)
{
  static DalDeviceHandle       *phChipInfo = NULL;
  DALResult                     eResult;
  DalChipInfoFamilyType         neFamily;

  if (phChipInfo == NULL)
  {
    eResult = 
      DAL_ChipInfoDeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo);

    if (eResult != DAL_SUCCESS)
    {
      return DALCHIPINFO_FAMILY_UNKNOWN;
    }
  }

  DalChipInfo_GetChipFamily(phChipInfo, &neFamily);

  return neFamily;
}


/*=========================================================================
FUNCTION  DalChipInfo_FoundryId
===========================================================================*/
/**
  Returns the FoundryId. 

  This function is a wrapper around DalChipInfo_GetFoundryId. It provides
  the foundry ID without the code overhead of attaching to the DAL.

  @return
  DalChipInfoFoundryIdType   neId

  DALCHIPINFO_FOUNDRYID_UNKNOWN  -- DAL device attach failed.

  @sa
  DalChipInfo_GetFoundryId
*/
static __inline DalChipInfoFoundryIdType
DalChipInfo_FoundryId(void)
{
  static DalDeviceHandle       *phChipInfo = NULL;
  DalDeviceHandle              *phChipInfoTemp;
  DALResult                     eResult;
  DalChipInfoFoundryIdType      neId;

  if (phChipInfo == NULL)
  {
    eResult = 
      DAL_ChipInfoDeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfoTemp);

    if (eResult != DAL_SUCCESS)
    {
      return DALCHIPINFO_FOUNDRYID_UNKNOWN;
    }

    phChipInfo = phChipInfoTemp;
  }

  DalChipInfo_GetFoundryId(phChipInfo, &neId);

  return neId;
}

/** 
  @}
*/ /* end_group Functions */

#endif /* !DDICHIPINFO_H */

