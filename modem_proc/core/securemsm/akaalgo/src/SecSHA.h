#ifndef SECSHA_H
#define SECSHA_H

/*===========================================================================

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/akaalgo/src/SecSHA.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

===========================================================================*/

/****************************************************************************
*   
*  THIS SOFTWARE IS DELIVERED AND MADE PUBLICLY AVAILABLE "AS IS."  QUALCOMM
*  MAKES NO WARRANTY OF ANY NATURE, EITHER EXPRESS OR IMPLIED, WITH RESPECT TO
*  THE SOFTWARE, INCLUDING BUT NOT LIMITED TO ANY WARRANTY (i) OF ITS
*  ACCURACY, FUNCTIONALITY OR EFFECTIVENESS; (ii) FOR THE CONSEQUENCES OF ITS
*  USE; (iii) OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE; OR (iv)
*  THAT IT WILL BE FREE FROM INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
*  OR OTHER RIGHTS.
*  
*  QUALCOMM SHALL NOT BE LIABLE TO ANY INDIVIDUAL,  COMPANY OR ENTITY FOR ANY
*  DETRIMENT, LOSS OR DAMAGE IN ANY WAY INCURRED, DIRECTLY OR INDIRECTLY, BY
*  REASON OF THE USE OF THE SOFTWARE, INCLUDING BUT NOT LIMITED TO DIRECT OR
*  INDIRECT DAMAGES, INCIDENTAL OR CONSEQUENTIAL DAMAGES, OR ANY OTHER LOSS OR
*  DAMAGE ARISING OUT OF THE USE OF THE SOFTWARE, WHETHER IN AN ACTION FOR OR
*  ARISING OUT OF BREACH OF CONTRACT, TORT OR ANY OTHER CAUSE OF ACTION.
*   
*****************************************************************************
*
*   File:           mccsha.h
*
*   Contact:        John Noerenberg
*                   Qualcomm Technologies, Inc
*                   
*                   jwn2@qualcomm.com
*
*   Description:    Header for SHA-1 functions implemented for ESA
*
*****************************************************************************/

/* header for SHA and related procedures */
#include "comdef.h"

#define DIGEST_LENGTH 20

typedef struct {
	uint8 digest[DIGEST_LENGTH]; /* Message digest */
	uint32 count[2];              /* count of bits */
	uint32 data[16];              /* data buffer */
}SHA_INFO;
	
void shaInitial(SHA_INFO* sha_info);

void shaUpdate( SHA_INFO*         sha_info, 
				uint8*            buffer,
				uint32            offset,
				uint32            count);

void shaFinal(SHA_INFO*  sha_info);

void sha_vector(uint8   num_elem,
                uint8*  addr[],
                uint32* len,
	            uint8*  mac);

void hmac_sha_vector(uint8*  key,
                     uint32  key_len,
                     uint8   num_elem,
		             uint8*  addr[],
                     uint32* len,
                     uint8*  mac);

void hmac_sha(uint8* key,
              uint32 key_len,
              uint8* data,
              uint32 data_len,
	          uint8* mac);



#endif /* SECSHA_H */
