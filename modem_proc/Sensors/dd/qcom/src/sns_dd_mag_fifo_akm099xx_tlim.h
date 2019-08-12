/*==============================================================================
 Copyright (c) 2014, AKM
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
     3. Neither the name of the AKM nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
==============================================================================*/

/*==============================================================================

                       EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
--------     ---     -----------------------------------------------------------
08/25/15     AKM     Add AK09913/AK09916 support.
03/31/15     AKM     Updated copyright message
09/12/14     AKM     Add AK09914/AK09915
==============================================================================*/

#ifndef _SNSD_MAG_AKM099XX_TLIM_H
#define _SNSD_MAG_AKM099XX_TLIM_H

// Limit of factory shipment test
#define TLIMIT_NO_READ_ID         0x001
#define TLIMIT_NO_INVALID_ID      0x002
#define TLIMIT_NO_RESET           0x003
#define TLIMIT_NO_READ_ASA        0x004
#define TLIMIT_NO_SET_SELFTEST    0x005
#define TLIMIT_NO_READ_ST1        0x006
#define TLIMIT_NO_READ_DATA       0x007
#define TLIMIT_NO_ASAX            0x101
#define TLIMIT_LO_ASAX            1
#define TLIMIT_HI_ASAX            254

#define TLIMIT_NO_ASAY            0x102
#define TLIMIT_LO_ASAY            1
#define TLIMIT_HI_ASAY            254

#define TLIMIT_NO_ASAZ            0x103
#define TLIMIT_LO_ASAZ            1
#define TLIMIT_HI_ASAZ            254

#define TLIMIT_NO_SLF_RVHX        0x201
#define TLIMIT_NO_SLF_RVHY        0x202
#define TLIMIT_NO_SLF_RVHZ        0x203
#define TLIMIT_NO_SLF_ST2         0x204
#define TLIMIT_LO_SLF_ST2         0
#define TLIMIT_HI_SLF_ST2         0
#define TLIMIT_ST2_MASK           (0x0F)

/*******************************
* AK09916 dependent value
*/
#define TLIMIT_LO_SLF_RVHX_09916  -200  /* TBD */
#define TLIMIT_HI_SLF_RVHX_09916  200   /* TBD */
#define TLIMIT_LO_SLF_RVHY_09916  -200  /* TBD */
#define TLIMIT_HI_SLF_RVHY_09916  200   /* TBD */
#define TLIMIT_LO_SLF_RVHZ_09916  -1000 /* TBD */
#define TLIMIT_HI_SLF_RVHZ_09916  -200  /* TBD */

/*******************************
* AK09915 dependent value
*/
#define TLIMIT_LO_SLF_RVHX_09915  -200
#define TLIMIT_HI_SLF_RVHX_09915  200
#define TLIMIT_LO_SLF_RVHY_09915  -200
#define TLIMIT_HI_SLF_RVHY_09915  200
#define TLIMIT_LO_SLF_RVHZ_09915  -800
#define TLIMIT_HI_SLF_RVHZ_09915  -200

/*******************************
* AK09914 dependent value
*/
/* AK09914 has no test-limit because it is no longer to go to mass production. */
#define TLIMIT_LO_SLF_RVHX_09914  -2000
#define TLIMIT_HI_SLF_RVHX_09914  2000
#define TLIMIT_LO_SLF_RVHY_09914  -2000
#define TLIMIT_HI_SLF_RVHY_09914  2000
#define TLIMIT_LO_SLF_RVHZ_09914  -16000
#define TLIMIT_HI_SLF_RVHZ_09914  0

/*******************************
 * AK09913 dependent value
 */
#define TLIMIT_LO_SLF_RVHX_09913  -200
#define TLIMIT_HI_SLF_RVHX_09913  200
#define TLIMIT_LO_SLF_RVHY_09913  -200
#define TLIMIT_HI_SLF_RVHY_09913  200
#define TLIMIT_LO_SLF_RVHZ_09913  -1000
#define TLIMIT_HI_SLF_RVHZ_09913  -200

/*******************************
 * AK09912 dependent value
 */
#define TLIMIT_LO_SLF_RVHX_09912  -200
#define TLIMIT_HI_SLF_RVHX_09912  200
#define TLIMIT_LO_SLF_RVHY_09912  -200
#define TLIMIT_HI_SLF_RVHY_09912  200
#define TLIMIT_LO_SLF_RVHZ_09912  -1600
#define TLIMIT_HI_SLF_RVHZ_09912  -400

/*******************************
 * AK09911 dependent value
 */
#define TLIMIT_LO_SLF_RVHX_09911  -30
#define TLIMIT_HI_SLF_RVHX_09911  30
#define TLIMIT_LO_SLF_RVHY_09911  -30
#define TLIMIT_HI_SLF_RVHY_09911  30
#define TLIMIT_LO_SLF_RVHZ_09911  -400
#define TLIMIT_HI_SLF_RVHZ_09911  -50
#endif /* End include guard  _SNSD_MAG_AKM099XX_TLIM_H */
