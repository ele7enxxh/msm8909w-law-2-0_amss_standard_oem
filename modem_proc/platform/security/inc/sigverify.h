/*=============================================================================

FILE:       sigverify.h

SERVICES:   API for authenticating dynamic modules using RSA signatures and
            an X509 certificate chain embedded in the dynmod.

==============================================================================
        Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
            QUALCOMM Proprietary and Confidential
==============================================================================*/
#ifndef __SIGVERIFY_H__
#define __SIGVERIFY_H__

#ifdef __cplusplus
extern "C" {
#endif

int SigVerify_start(const char* so_name, const unsigned char* pHdrs, int cbHdrs,
                    const unsigned char* pHash, int cbHash, void** ppv);
int SigVerify_verifyseg(void* pv, int segnum,
                       const unsigned char* pSeg, int cbSeg);
int SigVerify_stop(void* pv);
int SigVerify_CheckForTestEnable(void);

#ifdef __cplusplus
}
#endif

#endif   // __SIGVERIFY_H__
