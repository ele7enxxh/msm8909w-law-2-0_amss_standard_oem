#ifndef _RTXCONTEXT_HH_
#define _RTXCONTEXT_HH_

#include "rtxsrc/rtxBuffer.h"

#define OSCTXTINIT    0x1aa2a34a

#ifdef __cplusplus
extern "C" {
#endif
int rtxCheckLicense (OSCTXT* pctxt, int bits);
int rtxInitAndCheckLicense (OSCTXT* pctxt, int bits);

EXTERNRT void rtxCtxtSetMemHeap (OSCTXT* pctxt, OSCTXT* pSrcCtxt);
EXTERNRT void rtxCtxtSetMsgMemHeap (OSCTXT* pctxt, OSCTXT* pSrcCtxt);

#ifdef LICCHECK

#ifdef ASN1RT

#include "rtsrc/checklic.hhh"

#define LCHECKX(pctxt) \
   { int ret = rtCheckLicense0 (pctxt, 0); if (0 != ret) return ret; }

#define LCHECKX2(pctxt) \
   { if (0 != rtCheckLicense0 (pctxt, 0)) return NULL; }

#else
#define rtxlc0    x12
#define rtxlc     rtxlc0

int rtxlc (OSCTXT* pctxt);

#define LCHECKX(pctxt) \
{ int ret = rtxlc (pctxt); if (0 != ret) return ret; }

#define LCHECKX2(pctxt) \
{ if (0 != rtxlc (pctxt)) return NULL; }

#endif /* ASN1RT */

#else
#define LCHECKX(pctxt)
#define LCHECKX2(pctxt)
#endif

#ifdef __cplusplus
}
#endif

#endif
