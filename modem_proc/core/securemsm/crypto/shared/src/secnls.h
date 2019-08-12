/* $Id: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secnls.h#1 $ */
/* nls: NLS stream cipher and Mundja MAC header files */

/*
THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND AGAINST
INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* The following modifications are made to facilitate NLS-Based 
CSPRNG design:
	1. Remove all interfaces related to MAC
	2. Remove MAC-related data structure from nls_ctx 
	3. Add interfaces for nls_initstate(), nls_loadkey(), 
	   and nls_genkonst()
	4. Define NLS_LONG_OUTPUT as 0. When CSPRNG is seeded 
	   frequently, it is not necessary to consider this case.

	On 06/05/2005 by lxiao Qualcomm Technologies, Inc
*/

#ifndef _NLS_DEFINED
#define _NLS_DEFINED 1
/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/crypto/shared/src/secnls.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/10   nk      Shared Library Framework 
 
===========================================================================*/


#include "comdef.h"

/*
 * NLS maintains a counter that perturbs the state of the register
 * at long intervals to ensure that short cycles are impossibile.
 * This counter makes no difference to the output of the cipher for
 * the first F16 words (approximately 1/4 MegaByte).
 * If NLS is only going to be used for packets shorter than this,
 * setting the following define to 0  will generate faster and simpler code.
 */
#ifndef NLS_LONG_OUTPUT
#define NLS_LONG_OUTPUT 0
#endif

#define N 17
#define NMAC 8
#define WORDSIZE 32
#define WORD unsigned long
#define UCHAR unsigned char
#if NLS_LONG_OUTPUT
#define F16 0x10001ul
#endif /*NLS_LONG_OUTPUT*/
#define MACKONST 8

#define ROTL(w,x) (((w) << (x))|((w) >> (32 - (x))))
#define ROTR(w,x) (((w) >> (x))|((w) << (32 - (x))))

typedef struct {
    WORD	R[N];		/* Working storage for the shift register */
    WORD	konst;		/* key dependent constant */
    WORD	sbuf;		/* partial word encryption buffer */
    int		nbuf;		/* number of part-word stream bits buffered */
#if NLS_LONG_OUTPUT
    WORD	CtrModF16;	/* Multiprecision counter, modulo F16 */
    WORD	CtrMod232;	/* Multiprecision counter, LSW */
#endif /*NLS_LONG_OUTPUT*/
} nls_ctx;

/* interface definitions */

void nls_stream(nls_ctx *c, uint8 *buf, int nbytes);	/* stream cipher */
void nls_initstate(nls_ctx *c);		/* state initialization */
void nls_loadkey(nls_ctx *c, uint8 key[], int keylen);	/* key loading */
void nls_genkonst(nls_ctx *c);	/* generate key-dependent konst */

#endif /* _NLS_DEFINED */
