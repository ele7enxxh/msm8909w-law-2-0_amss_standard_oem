/********************************************************************************
* Copyright (c) 2014, "ams AG"
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of "ams AG" nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
/******************************************************************************
* Copyright (c) 2014, "ams AG"
* All rights reserved.
* THIS SOFTWARE IS PROVIDED FOR USE ONLY IN CONJUNCTION WITH AMS PRODUCTS.
* USE OF THE SOFTWARE IN CONJUNCTION WITH NON-AMS-PRODUCTS IS EXPLICITLY
* EXCLUDED.
*******************************************************************************/

#include "ams_tmg399x.h"
#include "sns_dd_ams_tmg399x_priv.h"


// Address of registers for TMG399x devices
char const regAddr[] =
    {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,  // 00-0f
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,  // 10-1f
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,  // 20-2f
        0xC7, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xFA, 0xFC, 0xFD, 0xFE, 0xFF                                 // 30-3A
    };


AMS_FIELD_DEFS const tmg399x[MAX_NUM_OF_FIELDS] =
    {
        {7, 1, AMS_REG_ENABLE},     // PBEN
        {6, 1, AMS_REG_ENABLE},     // GEN
        {5, 1, AMS_REG_ENABLE},     // PIEN
        {4, 1, AMS_REG_ENABLE},     // AIEN
        {3, 1, AMS_REG_ENABLE},     // WEN
        {2, 1, AMS_REG_ENABLE},     // PEN
        {1, 1, AMS_REG_ENABLE},     // AEN
        {0, 1, AMS_REG_ENABLE},     // PON

        {0, 8, AMS_REG_ATIME},      // ATIME

        {0, 8, AMS_REG_WTIME},      // WTIME

        {0, 8, AMS_REG_AILTHL},     // AILTHL
        {0, 8, AMS_REG_AIHTHL},     // AIHTHL
        {0, 8, AMS_REG_AILTHH},     // AILTHH
        {0, 8, AMS_REG_AIHTHH},     // AIHTHH

        {0, 8, AMS_REG_PITHL},      // PITHL
        {0, 8, AMS_REG_PITHH},      // PITHH

        {4, 4, AMS_REG_PERS},       // PPERS
        {0, 4, AMS_REG_PERS},       // APERS

        {1, 1, AMS_REG_CONFIG1},    // WLONG

        {6, 2, AMS_REG_PPULSE},     // PPLEN
        {0, 6, AMS_REG_PPULSE},     // PPULSE

        {6, 2, AMS_REG_CONTROL},    // LDRIVE
        {5, 1, AMS_REG_CONTROL},    // RPINS
        {4, 1, AMS_REG_CONTROL},    // RINTPOL
        {2, 2, AMS_REG_CONTROL},    // PGAIN
        {0, 2, AMS_REG_CONTROL},    // AGAIN

        {7, 1, AMS_REG_CONFIG2},    // PSIEN
        {6, 1, AMS_REG_CONFIG2},    // CPSIEN
        {4, 2, AMS_REG_CONFIG2},    // LEDBOOST

        {0, 3, AMS_REG_REVID},      // REVID

        {2, 6, AMS_REG_ID},         // ID
        {0, 2, AMS_REG_ID},         // VID

        {7, 1, AMS_REG_STATUS},     // CPGSAT,
        {6, 1, AMS_REG_STATUS},     // PGSAT,
        {5, 1, AMS_REG_STATUS},     // PINT,
        {4, 1, AMS_REG_STATUS},     // AINT,
        {3, 1, AMS_REG_STATUS},     // PBINT,
        {2, 1, AMS_REG_STATUS},     // GINT,
        {1, 1, AMS_REG_STATUS},     // PVALID,
        {0, 1, AMS_REG_STATUS},     // AVALID,

        {0, 8, AMS_REG_CDATAL},     // CDATAL,
        {0, 8, AMS_REG_CDATAH},     // CDATAH,
        {0, 8, AMS_REG_RDATAL},     // RDATAL,
        {0, 8, AMS_REG_RDATAH},     // RDATAH,
        {0, 8, AMS_REG_GDATAL},     // GDATAL,
        {0, 8, AMS_REG_GDATAH},     // GDATAH,
        {0, 8, AMS_REG_BDATAL},     // BDATAL,
        {0, 8, AMS_REG_BDATAH},     // BDATAH,

        {0, 8, AMS_REG_PDATA},      // PDATA,

        {0, 8, AMS_REG_POFFSET_NE}, // POFFSET_NE,

        {0, 8, AMS_REG_POFFSET_SW}, // POFFSET_SW,

        {5, 1, AMS_REG_CONFIG3},    // PCMP,
        {4, 1, AMS_REG_CONFIG3},    // SAI,
        {3, 1, AMS_REG_CONFIG3},    // PMSK_N,
        {2, 1, AMS_REG_CONFIG3},    // PMSK_S,
        {1, 1, AMS_REG_CONFIG3},    // PMSK_W,
        {0, 1, AMS_REG_CONFIG3},    // PMSK_E,

        {5, 1, AMS_REG_CONFIG_A0},  // PBIEN,
        {4, 1, AMS_REG_CONFIG_A0},  // SLEW,
        {3, 1, AMS_REG_CONFIG_A0},  // ISQZL,
        {0, 3, AMS_REG_CONFIG_A0},  // ISQZT,

        {0, 8, AMS_REG_CONFIG_A1},  // CONFIG_A1,

        {0, 8, AMS_REG_CONFIG_A2},  // ISNL,

        {0, 8, AMS_REG_CONFIG_A3},  // ISOFF,

        {0, 8, AMS_REG_CONFIG_A4},  // IPNL,

        {0, 8, AMS_REG_CONFIG_A5},  // IPOFF,

        {0, 6, AMS_REG_CONFIG_A6},  // IBT,

        {0, 8, AMS_REG_CONFIG_A7},  // ISLEN,

        {1, 1, AMS_REG_CONFIG_A8},  // PBINT2,
        {0, 1, AMS_REG_CONFIG_A8},  // PBUSY,


        {0, 8, AMS_REG_CONFIG_A0},  // GPENTH,

        {0, 8, AMS_REG_CONFIG_A1},  // GEXTH,

        {6, 2, AMS_REG_CONFIG_A2},  // GFIFOTH,
        {2, 4, AMS_REG_CONFIG_A2},  // GEXMSK,
        {0, 2, AMS_REG_CONFIG_A2},  // GEXPERS,

        {5, 2, AMS_REG_CONFIG_A3},  // GGAIN,
        {3, 2, AMS_REG_CONFIG_A3},  // GLDRIVE,
        {0, 3, AMS_REG_CONFIG_A3},  // GWTIME,

        {0, 8, AMS_REG_CONFIG_A4},  // GOFFSET_N,

        {0, 8, AMS_REG_CONFIG_A5},  // GOFFSET_S,

        {6, 2, AMS_REG_CONFIG_A6},  // GPLEN,
        {0, 6, AMS_REG_CONFIG_A6},  // GPULSE,

        {0, 8, AMS_REG_CONFIG_A7},  // GOFFSET_W,

        {0, 8, AMS_REG_CONFIG_A9},  // GOFFSET_E,

        {0, 2, AMS_REG_CONFIG_AA},  // GDIMS,

        {1, 1, AMS_REG_CONFIG_AB},  // GIEN,
        {0, 1, AMS_REG_CONFIG_AB},  // GMODE,

        {0, 8, AMS_REG_CONFIG_AE},  // GFLVL,

        {1, 1, AMS_REG_CONFIG_AF},  // GFOV,
        {0, 1, AMS_REG_CONFIG_AF},  // GVALID,

        // 16 bit fields:
        {0, 16, AMS_REG_AILTHL},    // AILTHH+AILTHL
        {0, 16, AMS_REG_AIHTHL},    // AIHTHH+AIHTHL

        {0, 16, AMS_REG_CDATAL},    // CDATA = CDATAH + CDATAL
        {0, 16, AMS_REG_RDATAL},    // RDATA = RDATAH + RDATAL
        {0, 16, AMS_REG_GDATAL},    // GDATA = GDATAH + GDATAL
        {0, 16, AMS_REG_BDATAL},    // BDATA = BDATAH + BDATAL
    };


uint16_t const bitFieldMask[] =
    {0x0001,   //!  1 bit
     0x0003,   //!  2 bits
     0x0007,   //!  3 bits
     0x000F,   //!  4 bits
     0x001F,   //!  5 bits
     0x003F,   //!  6 bits
     0x007F,   //!  7 bits
     0x00FF,   //!  8 bits
     0x01FF,   //!  9 bits
     0x03FF,   //! 10 bits
     0x07FF,   //! 11 bits
     0x0FFF,   //! 12 bits
     0x1FFF,   //! 13 bits
     0x3FFF,   //! 14 bits
     0x7FFF,   //! 15 bits
     0xFFFF,   //! 16 bits
    };


uint8_t const ams_tmg399x_alsGainVal[] = {
     1,
     4,
    16,
    64,
};


uint16_t const ams_tmg399x_min_integ_time_us[] = {
    2780,                                  // TMG3992
    2780,                                  // TMG3993
};


lux_segment const ams_tmg399x_lux_coef[] = {
    {458, 440, 1000, -10, 3057, 785},    // TMG3992
    {458, 440, 1000, -10, 3057, 785},    //fv TMG3993 from sample code 2.0.6
    /* {245, 100, 1000, -200, 3057, 785},    // TMG3993 */
};

uint8_t const ams_power_on_reset_values[] = {
    // 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
    0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x40, 0x00,  // 00 80
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 10 90
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 20 A0
};

#define NUM_OF_IDs  2

typedef enum {
    ID_TMG3992 = 0x27,
    ID_TMG3993 = 0x2A,
} DEVICE_IDs;

uint8_t const deviceIds[] = { ID_TMG3992, ID_TMG3993 };

bool ams_validDeviceId(uint8_t deviceId)
{
    uint16_t i;

    for(i = 0; i < NUM_OF_IDs; ++i)
    {
        if(deviceId == deviceIds[i])
        {
            return true;
        }
    }

    return false;
}
