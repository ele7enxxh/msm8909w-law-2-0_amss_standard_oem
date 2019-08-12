/*==============================================================================
  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/dal/framework/sys/src/tz/DALProps.c#1 $

when       who     description
--------   ---     ----------------------------------------------------------
09/25/12   aa      Modified the prop lookup logic for type and nameoffset
                   as they are now stored in one word.
04/25/12   sc      Ported DevCfg

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
_DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps)
{
   //go through the DAL Property Directory and find out if we have this 
   //device information
   uint32 dwPropsIdx = 0;
   DALPropsDir *pDALPropsDir = NULL;

   //Get the Property Dir Info Ptr
   DALSYS_GetPropDir(&pDALPropsDir);

   if (pDALPropsDir)
   {
      for (dwPropsIdx=0;dwPropsIdx<pDALPropsDir->dwNumDevices;dwPropsIdx++)
      {
         if (DeviceId == pDALPropsDir->dwPropsDeviceOffset[dwPropsIdx][0])
         {
            //Device found, intialize offset
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
DAL_StringDeviceObjectLookup(const char *pszDevName, const StringDevice **ppStringDeviceObj, DALProps *pDALPtr )
{
   uint32 dwDriverHash;
   int dwIndex = 0;
   int dwSize;
   const uint32 *pCollisionsIndexes = NULL;

   if(pDALPtr && pDALPtr->pDevices && pszDevName 
     && dwIndex< pDALPtr->dwDeviceSize && ppStringDeviceObj)
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
_DALSYS_GetDALPropertyHandleStr(const char *pszDevName, DALSYSPropertyHandle hDALProps)
{
   const StringDevice *pStringDeviceObj = NULL;
   DALPropsDir *pDALPropsDir = NULL;
   
   if(DAL_SUCCESS == DAL_StringDeviceObjectLookup(pszDevName, &pStringDeviceObj,DALSYS_GetDAL_Prop())){
      //Get the Property Dir Info Ptr
      DALSYS_GetPropDir(&pDALPropsDir);
      hDALProps[0] = (uint32) pDALPropsDir;
      hDALProps[1] = pStringDeviceObj->dwOffset;
      return DAL_SUCCESS;
   }
   hDALProps[0] = NULL;
   hDALProps[1] = NULL;
   return DAL_ERROR;
}

static 
uint32 GetNameOffsetOrIdVal(uint32 dwTypeNameOffset, uint32 *pdwId) 
{  
   //Get the name offset or id : extract the least signficant 24 bits
   *pdwId = (dwTypeNameOffset & 0x00FFFFFF);
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
   pPropVar->Val.pszVal = (char *)( pPropBase+pPropsDir->dwPropsStringSectionOffset
                                    +dwOffset) ;
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
                          +dwOffset+1);
   pPropVar->dwLen = pPropVar->Val.pbVal[-1]+1;
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
                                      +pPropsDir->dwPropsUint32SectionOffset
                                      +dwOffset+sizeof(uint32));
   pPropVar->dwLen = pPropVar->Val.pdwVal[-1]+1;
   pPropVar->dwType = DALSYS_PROP_TYPE_UINT32_PTR;
}

static 
void _PopulateStructPtrProperty(DALSYSPropertyVar *pPropVar,
                                   DALPropsDir *pPropsDir,
                                   byte *pbPropsBuf)
{
   //const void **pStruct = DALSYS_GetPropStructPtrArray();
   uint32 dwIdx = 0;

   memscpy(&dwIdx,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   //pPropVar->Val.pStruct = pStruct[dwIdx];
   pPropVar->Val.pStruct = (void *)dwIdx;
   pPropVar->dwLen = sizeof(void *);
   pPropVar->dwType = DALPROP_ATTR_TYPE_STRUCT_PTR;
   
}

static 
void _PopulateStructProperty(DALSYSPropertyVar *pPropVar,
                             DALPropsDir *pPropsDir,
                             byte *pbPropsBuf) //pos
{
   uint32 dwOffset=0;
   byte *pPropBase = (byte *)pPropsDir;
   memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->Val.pStruct = (pPropBase+pPropsDir->dwPropsStructSectionOffset
                          +dwOffset);
   pPropVar->dwType = DALPROP_ATTR_TYPE_STRUCT;
   pPropVar->dwLen = 0;
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
   case DALPROP_ATTR_TYPE_STRUCT:
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
   
   case DALPROP_ATTR_TYPE_STRUCT:
      _PopulateStructProperty(pPropVar,pPropsDir,pbPropsBuf);
      return DAL_SUCCESS;
   default:
      return DAL_ERROR;
   }
}

static 
uint32 _MatchProp( DALPropsDir *pPropsDir, 
                   const char *pszName, uint32 dwId, uint32 dwName,
                   uint32 dwOffset)
{
   uint32 bPropFound = 0;
   byte *pbPropsBaseAddr = (byte *)pPropsDir;
   
   //we have a Id name & Currently search for a id property 
   if ((NULL == pszName) && (dwName == dwId))
   {
      bPropFound = 1; 
   }
   else
   {
      //we have a string name, NameId
      if ((NULL != pszName) && 
          (0 == strcmp((const char *)pbPropsBaseAddr+pPropsDir->dwPropsNameSectionOffset+dwName,
                       pszName)))
      {
         //this is the property of interest
         bPropFound = 1;
      }
   }
   return bPropFound;
}

DALResult
_DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps, const char *pszName,
                         uint32 dwId,
                         DALSYSPropertyVar *pDALPropVar)
{
   //go through all the DAL Properties for this device and determine if we
   //have such a property
   uint32 dwName = 0;
   uint32 bPropFound  = 0;
   uint32 bCheckedGlobal = 0;
   uint32 dwOffset = 0;
   uint32 dwTypeNameOffset = 0;
   DALSYS_PROPERTY_HANDLE_DECLARE(hDALGlobalProps);
   //Points to device props to begin with 
   DALPropsDir *pDALPropsDir = (DALPropsDir *)hDALProps[0];
   byte  *pbDevicePropsPos   = ((byte *)hDALProps[0])+hDALProps[1];

   if((NULL == pDALPropsDir) || (0 == pbDevicePropsPos))
   {
      return DAL_ERROR;
   }

   //Get the Global Props handle
   if (DAL_SUCCESS != _DALSYS_GetDALPropertyHandle(0x00,hDALGlobalProps))
   {
      bCheckedGlobal = 1; //Flag it as though we checked global
   }

   for(;;)
   {
      //Get the Type & NameOffset word
      memscpy(&dwTypeNameOffset,sizeof(uint32),pbDevicePropsPos,sizeof(uint32));
     
      // Get the type : 8 most significant bits
      pDALPropVar->dwType = (dwTypeNameOffset & 0xFF000000) >> 24;
      
      //check for end of props
      if (DALPROP_ATTR_PROP_END == pDALPropVar->dwType)
      {
         //Exhausted our search for local props, try global now (only once)
         if (0 == bCheckedGlobal)
         {
            bCheckedGlobal = 1;
            pbDevicePropsPos = ((byte *)hDALProps[0])
                               +pDALPropsDir->dwPropsDeviceOffset[0][1];
            continue;
         }
         else
         {
            //Property not found
            return DAL_ERROR;
         }
      }

      dwOffset = GetNameOffsetOrIdVal(dwTypeNameOffset,&dwName);
      pbDevicePropsPos += dwOffset;
      bPropFound = _MatchProp(pDALPropsDir,pszName, dwId, dwName,dwOffset);
      if (0 == bPropFound)
      {
         if (DAL_ERROR == _SkipProp(&pbDevicePropsPos,pDALPropVar))
         {
            return DAL_ERROR;
         }
      }
      else if (1 == bPropFound)
      {
         return _PopulateProp(pDALPropVar, pDALPropsDir,pbDevicePropsPos);
      }
   }
}
