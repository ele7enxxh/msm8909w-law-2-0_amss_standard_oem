/*==============================================================================
@file DALProps.c

$Header: //components/rel/rpm.bf/2.1.1/core/dal/framework/sys/src/rpm/DALProps.c#2 $

Copyright (c) 2010, 2011
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSys.h"
#include "DALPropDef.h"
#include "DALSysCmn.h"
#include <string.h>
#include "stringl/stringl.h"

extern DALProps DALPROP_PropsInfo;

DALResult
DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps)
{
   //go through the DAL Property Directory and find out if we have this 
   //device information
   uint32 dwPropsIdx = 0;
   DALPropsDir *pDALPropsDir = (DALPropsDir *)DALPROP_PropsInfo.pDALPROP_PropBin;

   if (pDALPropsDir)
   {
      for (dwPropsIdx=0;dwPropsIdx<pDALPropsDir->dwNumDevices;dwPropsIdx++)
      {
         if (DeviceId == pDALPropsDir->dwPropsDeviceOffset[dwPropsIdx][0])
         {
            //Device found, initialize offset
            //Initialize handle with BASE addr.
            hDALProps[0] = (uint32) pDALPropsDir;
            hDALProps[1] = pDALPropsDir->dwPropsDeviceOffset[dwPropsIdx][1];
            return DAL_SUCCESS;
         }
      }
   }
   hDALProps[0] = NULL;
   hDALProps[1] = NULL;
   return DAL_ERROR;
}

static DALProps *
DALSYS_GetDAL_Prop(void)
{
   return (DALProps *)&DALPROP_PropsInfo;
}

static uint32
DJB2_hash(const char *pszDevName)
{
   uint32 dwHash = 5381;
   while (*pszDevName)
      dwHash = ((dwHash << 5) + dwHash) + (int)*pszDevName++; /* dwHash * 33 + c */
   return dwHash;
}
    
DALResult
DAL_StringDeviceObjectLookup(const char *pszDevName, const StringDevice **ppStringDeviceObj, DALProps *pDALPtr)
{
   uint32 dwDriverHash;
   int dwIndex = 0;
   int dwSize;
   const uint32 *pCollisionsIndexes = NULL;

   if(pDALPtr!= NULL && pDALPtr->pDevices!=NULL && pszDevName!= NULL 
     && dwIndex< pDALPtr->dwDeviceSize && ppStringDeviceObj!=NULL)
   {
      dwDriverHash = DJB2_hash(pszDevName); 
      for(dwIndex=0;dwIndex< pDALPtr->dwDeviceSize ; dwIndex++)
      {
         if(dwDriverHash ==  pDALPtr->pDevices[dwIndex].dwHash)
         {
            break;
         } 
      }
      // Finish the search with right device index. 
      if(dwIndex != pDALPtr->dwDeviceSize)
      {
         // if there is no Collisions.
         if(pDALPtr->pDevices[dwIndex].dwNumCollision == 0)
         {
            *ppStringDeviceObj = &(pDALPtr->pDevices[dwIndex]);
            return DAL_SUCCESS;
         }
         // If there is a Collision, scan the list for right device object.
         pCollisionsIndexes = pDALPtr->pDevices[dwIndex].pdwCollisions;
         dwSize = pDALPtr->pDevices[dwIndex].dwNumCollision;
         for(dwIndex=0;dwIndex<dwSize;dwIndex++)
         {
            // Get the dwIndex, where the str might be
            if(strlen(pszDevName) == strlen(pDALPtr->pDevices[pCollisionsIndexes[dwIndex]].pszName))
            {
               if(strncmp(pszDevName, pDALPtr->pDevices[pCollisionsIndexes[dwIndex]].pszName, strlen(pszDevName))==0)
               {
                  *ppStringDeviceObj = &(pDALPtr->pDevices[pCollisionsIndexes[dwIndex]]);
                  return DAL_SUCCESS;
               }
            }
         }
      }   
   }
   return DAL_ERROR;
}
 
DALResult
DALSYS_GetDALPropertyHandleStr(const char *pszDevName, DALSYSPropertyHandle hDALProps)
{
   const StringDevice *pStringDeviceObj = NULL;
   //DALPropsDir *pDALPropsDir = NULL;
   
   if(DAL_SUCCESS == DAL_StringDeviceObjectLookup(pszDevName, &pStringDeviceObj,    DALSYS_GetDAL_Prop())){
      //Get the Property Dir Info Ptr
      DALPropsDir *pDALPropsDir = (DALPropsDir *)DALPROP_PropsInfo.pDALPROP_PropBin;
      hDALProps[0] = (uint32) pDALPropsDir;
      hDALProps[1] = pStringDeviceObj->dwOffset;
      return DAL_SUCCESS;
   }
   hDALProps[0] = NULL;
   hDALProps[1] = NULL;
   return DAL_ERROR;
}

static 
uint32 GetNameOffsetOrIdVal(uint32 dwAttr, byte *pbCurPropsPos,uint32 *pdwId) 
{
   //read 4 bytes to retrieve name-offset(string id) or id(numeric id)
   memscpy(pdwId,sizeof(uint32),pbCurPropsPos,sizeof(uint32));
   if(*pdwId & 0x80000000 ) // 31st bit
   {
      // clear 31st bit as it used for identifying an id or name offset.
      // if its set its a offset
      *pdwId =  *pdwId & 0x7FFFFFFF ;
   }      
   return sizeof(uint32);
}

static 
void _PopulateStringProperty(DALSYSPropertyVar *pPropVar,
                             DALPropsDir *pPropsDir,
                             byte *pbPropsBuf)
{
   uint32 dwOffset=0;
   byte *pPropBase = (byte *)pPropsDir;
   memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->Val.pszVal = (char *)(pPropBase+pPropsDir->dwPropsStringSectionOffset
                                   + dwOffset) ;
   pPropVar->dwLen = strlen(pPropVar->Val.pszVal);
   pPropVar->dwType = DALSYS_PROP_TYPE_STR_PTR;
}

static
void _PopulateUint32Property(DALSYSPropertyVar *pPropVar,
                             byte *pbPropsBuf)
{
   memscpy(&(pPropVar->Val.dwVal),sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->dwLen = sizeof(uint32);
   pPropVar->dwType = DALSYS_PROP_TYPE_UINT32;
}

static 
void _PopulateByteSeqProperty(DALSYSPropertyVar *pPropVar,
                              byte *pbPropsBuf)
{
   byte bLen = pbPropsBuf[0];
   pPropVar->Val.pbVal = pbPropsBuf+1;
   pPropVar->dwLen = bLen+1;
   pPropVar->dwType = DALSYS_PROP_TYPE_BYTE_PTR;
}

static 
void _PopulateByteSeqPtrProperty(DALSYSPropertyVar *pPropVar,
                                 DALPropsDir *pPropsDir,
                                 byte *pbPropsBuf)
{
   uint32 dwOffset=0;
   byte *pPropBase = (byte *)pPropsDir;
   //Get the byte offset
   memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->Val.pbVal = (pPropBase+pPropsDir->dwPropsByteSectionOffset
                          + dwOffset+1);
   pPropVar->dwLen = pPropVar->Val.pbVal[-1] + 1;
   pPropVar->dwType = DALSYS_PROP_TYPE_BYTE_PTR;
}

static 
void _PopulateUint32SeqPtrProperty(DALSYSPropertyVar *pPropVar,
                                   DALPropsDir *pPropsDir,
                                   byte *pbPropsBuf)
{
   uint32 dwOffset=0;
   byte *pPropBase = (byte *)pPropsDir;

   //Get the byte offset
   memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->Val.pdwVal = (uint32 *) (pPropBase
                                      + pPropsDir->dwPropsUint32SectionOffset
                                      + dwOffset+sizeof(uint32));
   pPropVar->dwLen = pPropVar->Val.pdwVal[-1]+1;
   pPropVar->dwType = DALSYS_PROP_TYPE_UINT32_PTR;
}

static 
void _PopulateStructPtrProperty(DALSYSPropertyVar *pPropVar,
                                   DALPropsDir *pPropsDir,
                                   byte *pbPropsBuf)
{
   const void **pStruct = DALPROP_PropsInfo.pDALPROP_StructPtrs;
   uint32 dwIdx = 0;

   if(pStruct)
   {
      memscpy(&dwIdx,sizeof(uint32),pbPropsBuf,sizeof(uint32));
      pPropVar->Val.pStruct = pStruct[dwIdx];
      pPropVar->dwLen = sizeof(void *);
      pPropVar->dwType = DALPROP_ATTR_TYPE_STRUCT_PTR;
   }
}

static 
DALResult _SkipProp(byte **pbPropsBuf, DALSYSPropertyVar *pPropVar)
{
   byte bLen = 0;
   switch (pPropVar->dwType&DALPROP_ATTR_TYPE_ID_ATTR_MASK)
   {
   case DALPROP_ATTR_TYPE_STRING_PTR:
   case DALPROP_ATTR_TYPE_UINT32:
   case DALPROP_ATTR_TYPE_BYTE_SEQ_PTR:
   case DALPROP_ATTR_TYPE_UINT32_SEQ_PTR:
   case DALPROP_ATTR_TYPE_STRUCT_PTR:
      *pbPropsBuf += sizeof(uint32);      
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_BYTE_SEQ:
      bLen = *pbPropsBuf[0]+1+1; // The first one is refering to the len and the secound one is zero based index
      *pbPropsBuf += bLen;
      if (bLen%4)
         *pbPropsBuf+= (4-(bLen%4));
      return DAL_SUCCESS;

   default:
      return DAL_ERROR;
   }
}

static 
DALResult _PopulateProp(DALSYSPropertyVar *pPropVar, DALPropsDir *pPropsDir,
                        byte *pbPropsBuf)
{
   switch (pPropVar->dwType&DALPROP_ATTR_TYPE_ID_ATTR_MASK)
   {
   case DALPROP_ATTR_TYPE_STRING_PTR:
      _PopulateStringProperty(pPropVar,pPropsDir,pbPropsBuf);
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_UINT32:
      _PopulateUint32Property(pPropVar,pbPropsBuf);
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_BYTE_SEQ:
      _PopulateByteSeqProperty(pPropVar,pbPropsBuf);
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_BYTE_SEQ_PTR:
      _PopulateByteSeqPtrProperty(pPropVar,pPropsDir,pbPropsBuf);
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_UINT32_SEQ_PTR:
      _PopulateUint32SeqPtrProperty(pPropVar,pPropsDir,pbPropsBuf);
      return DAL_SUCCESS;

   case DALPROP_ATTR_TYPE_STRUCT_PTR:
      _PopulateStructPtrProperty(pPropVar,pPropsDir,pbPropsBuf);
      return DAL_SUCCESS;
   
   default:
      return DAL_ERROR;
   }
}

static 
DALResult _MatchProp( DALPropsDir *pPropsDir, 
                   const char *pszName, uint32 dwId, uint32 dwName,
                   uint32 dwOffset)
{
   DALResult bPropFound = DAL_ERROR_PROP_NOT_FOUND;
   byte *pbPropsBaseAddr = (byte *)pPropsDir;
   
   //we have a Id name & Currently search for a id property 
   if ((NULL == pszName) && (dwName == dwId))
   {
         bPropFound = DAL_SUCCESS;
   }
   else
   {
      //we have a string name, NameId
      if ((NULL != pszName) && 
          (0 == strcmp((const char *)pbPropsBaseAddr+pPropsDir->dwPropsNameSectionOffset+dwName,
                       pszName)))
      {
         //this is the property of interest
         bPropFound = DAL_SUCCESS;
      }
   }
   return bPropFound;
}

DALResult
DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps, const char *pszName,
                        uint32 dwId, DALSYSPropertyVar *pDALPropVar)
{
   //go through all the DAL Properties for this device and determine if we
   //have such a property
   uint32 dwName = 0;
   uint32 bPropFound  = 0;
   uint32 bCheckedGlobal = 0;
   uint32 dwOffset = 0;
   DALResult ret;
   DALSYS_PROPERTY_HANDLE_DECLARE(hDALGlobalProps);
   //Points to device props to begin with 
   DALPropsDir *pDALPropsDir = (DALPropsDir *)hDALProps[0];
   byte  *pbDevicePropsPos   = ((byte *)hDALProps[0])+hDALProps[1];

   if((NULL == pDALPropsDir) || (0 == pbDevicePropsPos))
   {
      return DAL_ERROR;
   }

   //Get the Global Props handle
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(0x00,hDALGlobalProps))
   {
      bCheckedGlobal = 1; //Flag it as though we checked global
   }

   for(;;)
   {
      //Get the Type
      memscpy(&pDALPropVar->dwType,sizeof(uint32),pbDevicePropsPos,
            sizeof(uint32));
      pbDevicePropsPos += sizeof(uint32);

      //check for end of props
      if (DALPROP_ATTR_PROP_END == pDALPropVar->dwType)
      {
         //Exhausted our search for local props, try global now (only once)
         if (0 == bCheckedGlobal)
         {
            bCheckedGlobal = 1;
            pbDevicePropsPos = ((byte *)hDALProps[0])
                               + pDALPropsDir->dwPropsDeviceOffset[0][1];
            continue;
         } 
         else
         {
         //Property not found
            return DAL_ERROR_PROP_NOT_FOUND;
         }
      }

      dwOffset = GetNameOffsetOrIdVal((pDALPropVar->dwType & DALPROP_ATTR_TYPE_ID_ATTR_MASK),pbDevicePropsPos,&dwName);
      pbDevicePropsPos += dwOffset;
      bPropFound = _MatchProp(pDALPropsDir,pszName, dwId, dwName,dwOffset);
      if (DAL_SUCCESS != bPropFound)
      {
         ret = _SkipProp(&pbDevicePropsPos,pDALPropVar);
         if (DAL_SUCCESS != ret ) return ret;
      } 
      else if (DAL_SUCCESS == bPropFound)
      {
		 if(pDALPropVar->dwType & DALPROP_ATTR_TYPE_ID_VISIBILITY_MASK)
			return DAL_ERROR_PROP_ACCESS_DENIED;
         return _PopulateProp(pDALPropVar, pDALPropsDir,pbDevicePropsPos);
      }
   }
}
