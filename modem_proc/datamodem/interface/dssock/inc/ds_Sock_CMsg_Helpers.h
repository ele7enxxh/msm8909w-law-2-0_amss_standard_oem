#ifndef DS_SOCK_CMSG_HELPERS_H
#define DS_SOCK_CMSG_HELPERS_H
/*
=======================================================================
                    Copyright (c) 2010 Qualcomm Technologies Incorporated.
                            All Rights Reserved.
                     QUALCOMM Confidential and Proprietary
=======================================================================
*/

/*---------------------------------------------------------------------------
  Ancillary data looks like this:

   typedef struct ds_Sock_AncMsg
   {
      int   nMsgLen;                   * data byte count, including header *
      int   nMsgLevel;                              * originating protocol *
      int   nMsgType;                             * protocol-specific type *
      char  MsgData[nMsgLen-(sizeof(int)*3)];  * chunks of data, each 4-byte 
                                                 aligned                   *
   } ds_Sock_AncMsg;

   Each of the structs in the buffer are word-aligned from 
   the beginning of the buffer.  The nLen, nLevel, and nType fields are in
   little-endian format.  The data is in a type-specific format.

*/

/*!
  @brief      
  Parse next ancillary data.

  @details
  Given a pointer to a buffer containing ancillary data, return a pointer
  to the next ancillary data entry.  Outputs updated buffer pointer and
  length.

  @param[in]   pvBuf -        pointer to buffer that contains ancillary 
                              elements (see ds_Sock_AncMsg above).
  @param[in]   nBufLen      - length of pvBuf in bytes.
  @param[out]  pnMsgLevel   - msg level of the ancillary element pointed 
                              by pvBuf.
  @param[out]  pnMsgType    - msg type of the ancillary element pointed 
                              by pvBuf.
  @param[out]  ppvMsgData   - pointer to the data portion of the ancillary
                              element pointed by pvBuf.
  @param[in]   pnMsgDataLen - length of data pointed by ppvMsgData in bytes
  @param[out]  ppvBuf       - pointer to buffer which contains the next 
                              ancillary elements after the one pointed by 
                              pvBuf. This is used in subsequent calls 
                              to ds_Sock_AncData_ParseNext.
  @param[in]   pnBufLen     - length of ppvBuf in bytes.
  @return      0 on success.
  @return      EFAILED (1) on failure.

  Pseudo code as usage example
  ============================
  
  err = pIDSSocket->RecvMsg(msg_name, msg_buf, msg_bufLen, &msg_bufLen,
                            msg_control, msg_controlLen, &msg_controlLenReq,
                            &msg_flags, flags);
  if not err....

  if (  RecvFlags::MSG_ERRQUEUE_FLAG & msg_flags) {
      int nMsgLevel, nMsgType, nMsgLen;
      byte* pAncDataMsg;
      ::byte* pBuffLoc = msg_control;
      int nLenRemained = msg_controlLen;
      while (nLenRemained > 0)
      {
      nErr = ds_Sock_AncData_ParseNext(pBuffLoc, nLenRemained,
                                       &nMsgLevel, &nMsgType, 
                                       (const void**)&pAncDataMsg,
                                       &nMsgLen, (const void**)&pBuffLoc,
                                       &nLenRemained);

      if (0 != nErr) {
      return nError;
      }
      ....parse specific pAncDataMsg based on nMsgLevel and nMsgType
   }
*/
static __inline int ds_Sock_AncData_ParseNext(const void*  pvBuf,
                                              int          nBufLen,
                                              int*         pnMsgLevel,
                                              int*         pnMsgType,
                                              const void** ppvMsgData,
                                              int*         pnMsgDataLen,
                                              const void** ppvBuf,
                                              int*         pnBufLen)
{
   const unsigned char* pBuf = (const unsigned char*)pvBuf;
   struct {
      int   nLen;
      int   nLevel;
      int   nType;
   } hdr = {0};
   
   if (nBufLen < (int)sizeof(hdr)) {
      return 1; /* EFAILED */
   }

#define __INT_FROM_LE_BYTES(b) \
      (b)[0] |                 \
      (b)[1]<<8 |              \
      (b)[2]<<16 |             \
      (b)[3]<<24
   
   /* parse header */
   hdr.nLen   = __INT_FROM_LE_BYTES(pBuf);
   hdr.nLevel = __INT_FROM_LE_BYTES(pBuf+4);
   hdr.nType  = __INT_FROM_LE_BYTES(pBuf+8);

   /* sanity */
   if (hdr.nLen < 0 || 
       hdr.nLen < (int)sizeof(hdr) ||
       hdr.nLen > nBufLen) {
      return 1; /* EFAILED */
   }

   if ((int*)0 != pnMsgLevel) {
      *pnMsgLevel = hdr.nLevel;
   }
   if ((int*)0 != pnMsgType) {
      *pnMsgType = hdr.nType;
   }
   /* msg data info */
   if ((const void**)0 != ppvMsgData) {
      *ppvMsgData = pBuf + (int)sizeof(hdr);
   }
   if ((int*)0 != pnMsgDataLen) {
      *pnMsgDataLen = hdr.nLen - (int)sizeof(hdr);
   }

   /* next item is 4-byte aligned */
   hdr.nLen += 3;
   hdr.nLen &= ~3;

   /* advance read pointer */
   if ((int*)0 != pnBufLen) {
      *pnBufLen = nBufLen - hdr.nLen;
      if ((void*)0 != *ppvBuf) {
        if (0 < (*pnBufLen)) {
          /*more ancillary elements*/
          *ppvBuf = pBuf + hdr.nLen;
        }
        else 
        {/*no more ancillary elements*/
          *ppvBuf = 0;
        }
      }

   }
   
   return 0; /* SUCCESS */
}
#endif /* #ifndef DS_SOCK_CMSG_HELPERS_H */

