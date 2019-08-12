/*==============================================================================

                  DAL CONFIG PROPERTY Module

GENERAL DESCRIPTION
    This file provides an implementation of Dal Property lookup.

EXTERNALIZED FUNCTIONS
    DALSYS_GetDALPropertyHandle()
    DALSYS_GetPropertyValue()
 
  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.

$Header: //components/rel/core.mpss/3.5.c12.3/dal/config/src/mba/DALProps.c#1 $

when       who     description
--------   ---     ----------------------------------------------------------
03/02/2012 aa      Initial release for ADSP2.0 MPQuRT USERPD.
==============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSys.h"
#include "DALPropDef.h"
#include "DALSysCmn.h"
#include <string.h>
#include "DALSysInt.h"
#include "stringl/stringl.h"

/*=============================================================================
                    DEFINITIONS AND DECLARATIONS
=============================================================================*/
extern DALProps DAL_Mod_Info;
extern DALProps DALPROP_PropsInfo;

DALProps *
DALSYS_GetDAL_Mod(void){
   return (DALProps *)&DAL_Mod_Info;
}

static void 
_GetPropDir(DALPropsDir ** pDALPropsDir)
{
    *pDALPropsDir = (DALPropsDir *) DALPROP_PropsInfo.pDALPROP_PropBin;
}

static const void **
_GetPropStructPtrArray(void)
{
   return DALPROP_PropsInfo.pDALPROP_StructPtrs;
}

DALResult
DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps)
{
   //go through the DAL Property Directory and find out if we have this 
   //device information
   uint32 dwPropsIdx = 0;
   DALPropsDir *pDALPropsDir = NULL;

   //Get the Property Dir Info Ptr
   _GetPropDir(&pDALPropsDir);

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
   DALSYS_LOG_ERROR_EVENT("Failed- DeviceId:0x%x", DeviceId );
   return DAL_ERROR;
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
DAL_StringDeviceObjectLookup(const char *pszDevName, 
      const StringDevice **ppStringDeviceObj, DALProps *pDALPtr)
{
   uint32 dwDriverHash;
   int dwIndex = 0;
   int dwSize;
   uint32 *pCollisionsIndexes = NULL;

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
   DALSYS_LOG_ERROR_EVENT("Failed for pszDevName:%s", pszDevName);
   return DAL_ERROR;
}
 
DALResult
DALSYS_GetDALPropertyHandleStr(const char *pszDevName, DALSYSPropertyHandle hDALProps)
{
   const StringDevice *pStringDeviceObj = NULL;
   DALPropsDir *pDALPropsDir = NULL;
   
   if(DAL_SUCCESS == DAL_StringDeviceObjectLookup(pszDevName, &pStringDeviceObj, &DALPROP_PropsInfo)){
      //Get the Property Dir Info Ptr
      _GetPropDir(&pDALPropsDir);
      hDALProps[0] = (uint32) pDALPropsDir;
      hDALProps[1] = pStringDeviceObj->dwOffset;
      return DAL_SUCCESS;
   }
   hDALProps[0] = NULL;
   hDALProps[1] = NULL;
   DALSYS_LOG_ERROR_EVENT("Failed for pszDevName:%s", pszDevName);   
   return DAL_ERROR;
}

static 
uint32 GetNameOffsetOrIdVal(uint32 dwAttr, byte *pbCurPropsPos,uint32 *pdwId) 
{
   //read 4 bytes to retrieve nameoffset(string id) or id(numeric id)
      memscpy(pdwId,sizeof(uint32),pbCurPropsPos,sizeof(uint32));
      return sizeof(uint32);
   }

static 
void _PopulateStringProperty(DALSYSPropertyVar *pPropVar,
                             DALPropsDir *pPropsDir,
                             byte *pbPropsBuf)
{
   uint32 dwOffset=0;
   byte *pPropBase = (byte *)pPropsDir;
   _DALSYS_memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
   pPropVar->Val.pszVal = (char *)( pPropBase+pPropsDir->dwPropsStringSectionOffset
                                    +dwOffset) ;
   pPropVar->dwLen = strlen(pPropVar->Val.pszVal);
   pPropVar->dwType = DALSYS_PROP_TYPE_STR_PTR;
}

static
void _PopulateUint32Property(DALSYSPropertyVar *pPropVar,
                             byte *pbPropsBuf)
{
   _DALSYS_memscpy(&(pPropVar->Val.dwVal),sizeof(uint32),pbPropsBuf,sizeof(uint32));
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
   _DALSYS_memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
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
   _DALSYS_memscpy(&dwOffset,sizeof(uint32),pbPropsBuf,sizeof(uint32));
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
   const void **pStruct = _GetPropStructPtrArray();
   uint32 dwIdx = 0;

   if(pStruct)
   {
      _DALSYS_memscpy(&dwIdx,sizeof(uint32),pbPropsBuf,sizeof(uint32));
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
      bLen = *pbPropsBuf[0]+1+1;
      *pbPropsBuf += bLen;
      if (bLen%4)
         *pbPropsBuf+= (4-(bLen%4));
      return DAL_SUCCESS;

   default:
      DALSYS_LOG_ERROR_EVENT("Unknown prop value- pPropVar:0x%x", pPropVar);
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
      DALSYS_LOG_ERROR_EVENT("Unknown type- pPropVar:0x%x", pPropVar);
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
DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps, const char *pszName,
                         uint32 dwId,
                         DALSYSPropertyVar *pDALPropVar)
{
   //go through all the DAL Properties for this device and determine if we
   //have such a property
   uint32 dwName = 0;
   uint32 bPropFound  = 0;
   uint32 bCheckedGlobal = 0;
   uint32 dwOffset = 0;
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
      _DALSYS_memscpy(&pDALPropVar->dwType,sizeof(uint32),pbDevicePropsPos,sizeof(uint32));
      pbDevicePropsPos += sizeof(uint32);

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
         } else
         {
            //Property not found
            return DAL_ERROR;
         }
      }

      dwOffset = GetNameOffsetOrIdVal(pDALPropVar->dwType,pbDevicePropsPos,&dwName);
      pbDevicePropsPos += dwOffset;
      bPropFound = _MatchProp(pDALPropsDir,pszName, dwId, dwName,dwOffset);
      if (0 == bPropFound)
      {
         if (DAL_ERROR == _SkipProp(&pbDevicePropsPos,pDALPropVar))
         {
            return DAL_ERROR;
         }
      } else if (1 == bPropFound)
      {
         return _PopulateProp(pDALPropVar, pDALPropsDir,pbDevicePropsPos);
      }
   }
}

void DALSYS_DoPropsSymLookUp(DEVCFG_TARGET_INFO *pTargetInfoTableIter)
{
   // unimplemented
}

void DevCfg_Init(void)
{
   // does nothing
}
