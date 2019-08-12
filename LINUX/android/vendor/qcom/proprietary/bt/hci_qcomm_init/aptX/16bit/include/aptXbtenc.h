/******************************************************************************
 *
 *  Copyright (c) 2016 Qualcomm Technologies International, Ltd.
 *  All Rights Reserved.
 *  Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 *
 ******************************************************************************/

/*-----------------------------------------------------------------------------
 *
 *  aptXbtenc.h
 *
 *  This file exposes a public interface to allow clients to invoke btaptX
 *  16 encoding on 4 new PCM samples, generating 2 new codeword (one for the
 *  left channel and one for the right channel).
 *
 *  $Revision: #1 $
 *  $Date: 2016/02/15 $
 *
 *----------------------------------------------------------------------------*/

#ifndef APTX_ENC_H
#define APTX_ENC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _DLLEXPORT
#define APTXBTENCEXPORT __declspec(dllexport)
#else
#define APTXBTENCEXPORT
#endif

/* StereoEncode will take 8 audio samples (16-bit per sample)
 * and generate one 32-bit codeword with autosync inserted.
 * The bitstream is compatible with be BC05 implementation.
 */

int APTXBTENCEXPORT aptxbtenc_encodestereo(
   void* _state, void* _pcmL,void* _pcmR,void* _buffer
);

/* aptxbtenc_version can be used to extract the version number
 * of the aptX encoder
 */

const char* APTXBTENCEXPORT aptxbtenc_version(void);

/* aptxbtenc_build can be used to extract the build number
 * of the aptX encoder
 */

const char* APTXBTENCEXPORT aptxbtenc_build(void);

/* NewAptxEnc will create the required structure to run the
 * aptX encoder. The function will return a pointer to that structure.
 * endian: type boolean : 0 = output is little endian,
 *                        1 = output is big endian
 */
void APTXBTENCEXPORT * NewAptxEnc(short endian);

/*  aptxbtenc_init is used to initialise the encoder structure.
 * _state should be a pointer to the encoder structure (stereo).
 * endian represent the endianness of the output data
 * (0=little endian. Big endian otherwise)
 * The function returns 1 if an error occured during the initialisation.
 * The function returns 0 if no error occured during the initialisation.
 */

int APTXBTENCEXPORT aptxbtenc_init(void* _state, short endian);

/* SizeofAptxbtenc returns the size (in byte) of the memory
 * allocation required to store the state of the encoder
 */
int APTXBTENCEXPORT SizeofAptxbtenc (void);


typedef void* aptX_creator(short);
typedef void aptX_disposer(void*);
typedef int  aptX_StereoEncode(void*,void*,void*,void*);
#ifdef __cplusplus
} //  /extern "C"
#endif

#endif  // APTX_ENC_H



