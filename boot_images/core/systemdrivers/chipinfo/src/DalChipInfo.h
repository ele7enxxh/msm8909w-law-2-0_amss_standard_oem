#ifndef DALChipInfo_H
#define DALChipInfo_H
/*==============================================================================

FILE:      DalChipInfo.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

============================================================================
        Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/chipinfo/src/DalChipInfo.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
==============================================================================*/

#include "DALFramework.h"
#include "DDIChipInfo.h"


/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * DALChipinfoPartMapType
 *
 * Map from PARTNUM field value to logical part number.
 */
typedef struct
{
  uint32             nPart;
  DalChipInfoIdType  ePart;
} DalChipInfoPartMapType;

/*
 * DalChipInfoIdMapType
 *
 * Map from logical part number to sequential number.
 */
typedef struct
{
  DalChipInfoIdType  ePart;
  uint32             nPart;
} DalChipInfoIdMapType;


/*
 * ChipInfo_ManufacturerType
 *
 * List of all possible manufacturers.
 */
typedef enum
{
  DALCHIPINFO_MFG_UNKNOWN     = 0,
  DALCHIPINFO_MFG_QUALCOMM    = 1,

  DALCHIPINFO_NUM_MFGS        = 2,
  DALCHIPINFO_MFG_32BITS      = 0x7FFFFFF
} DalChipInfoManufacturerType;

/*------------------------------------------------------------------------------
Declaring a "ChipInfo" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct ChipInfoDrvCtxt ChipInfoDrvCtxt;
typedef struct ChipInfoDevCtxt ChipInfoDevCtxt;
typedef struct ChipInfoClientCtxt ChipInfoClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "ChipInfo" Vtable
------------------------------------------------------------------------------*/
typedef struct ChipInfoDALVtbl ChipInfoDALVtbl;
struct ChipInfoDALVtbl
{
	int (*ChipInfo_DriverInit)(ChipInfoDrvCtxt *);
	int (*ChipInfo_DriverDeInit)(ChipInfoDrvCtxt *);
};

struct ChipInfoDevCtxt
{
  /*
   * Base Members
   */
  uint32            dwRefs;                                    
  DALDEVICEID       DevId;                                  
  uint32            dwDevCtxtRefIdx; 
  ChipInfoDrvCtxt  *pChipInfoDrvCtxt;                             
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32 Reserved[16];
};

struct ChipInfoDrvCtxt
{
  /*
   * Base Members
   */
  ChipInfoDALVtbl ChipInfoDALVtbl;
  uint32          dwNumDev;
  uint32          dwSizeDevCtxt;
  uint32          bInit;
  uint32          dwRefs;
  ChipInfoDevCtxt ChipInfoDevCtxt[1];

  /*
   * Cached chip version information from either HW or the remote
   * DAL processor.
   */
  DalChipInfoIdType      eChipId;
  DalChipInfoFoundryIdType eFoundryId;
  DalChipInfoFamilyType  eChipFamily;
  DalChipInfoVersionType nChipVersion;
  uint32                 nRawChipVersion;
  uint32                 nRawChipId;
  DalChipInfoSerialNumType  nSerialNum;
  DalChipInfoModemType   nModemSupport;
  char                   szChipIdString[DALCHIPINFO_MAX_ID_LENGTH];

  /*
   * Handle for communicating with the HWIO DAL.
   */
  DalDeviceHandle       *phDalHWIO;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProps);
};

/*------------------------------------------------------------------------------
Declaring a "ChipInfo" Client Context
------------------------------------------------------------------------------*/
struct ChipInfoClientCtxt
{
  /*
   * Base Members
   */
  uint32  dwRefs;                     
  uint32  dwAccessMode;  
  void *pPortCtxt;
  ChipInfoDevCtxt *pChipInfoDevCtxt;            
  DalChipInfoHandle DalChipInfoHandle; 
};




/* -----------------------------------------------------------------------
** Interface
** ----------------------------------------------------------------------- */

void ChipInfo_GetPart            ( ChipInfoDrvCtxt *pCtxt, DalChipInfoIdType *pePart, uint32 pHWRevNum );
void ChipInfo_GetVersion         ( ChipInfoDrvCtxt *pCtxt, DalChipInfoVersionType *pnVersion, uint32 pHWRevNum  );
void ChipInfo_GetManufacturer    ( ChipInfoDrvCtxt *pCtxt, DalChipInfoManufacturerType *peManufacturer, uint32 pHWRevNum  );

void ChipInfo_GetFamily          ( ChipInfoDrvCtxt *pCtxt, DalChipInfoFamilyType *peFamily, uint32 pHWRevNum  );
void ChipInfo_GetFamilyByPart    ( DalChipInfoIdType ePart, DalChipInfoFamilyType *peFamily );

void ChipInfo_GetRawPart         ( ChipInfoDrvCtxt *pCtxt, uint32 *pnRawPart, uint32 pHWRevNum  );
void ChipInfo_GetRawVersion      ( ChipInfoDrvCtxt *pCtxt, uint32 *pnRawVersion, uint32 pHWRevNum  );
void ChipInfo_GetRawManufacturer ( ChipInfoDrvCtxt *pCtxt, uint32 *pnRawManufacturer, uint32 pHWRevNum  );


DALResult ChipInfo_DriverInit(ChipInfoDrvCtxt *);
DALResult ChipInfo_DriverDeInit(ChipInfoDrvCtxt *);
DALResult ChipInfo_DeviceInit(ChipInfoClientCtxt *);
DALResult ChipInfo_DeviceDeInit(ChipInfoClientCtxt *);
DALResult ChipInfo_Reset(ChipInfoClientCtxt *);
DALResult ChipInfo_PowerEvent(ChipInfoClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult ChipInfo_Open(ChipInfoClientCtxt *, uint32);
DALResult ChipInfo_Close(ChipInfoClientCtxt *);
DALResult ChipInfo_Info(ChipInfoClientCtxt *,DalDeviceInfo *, uint32);
DALResult ChipInfo_InheritObjects(ChipInfoClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);



/*------------------------------------------------------------------------------
Functions specific to DalChipInfo interface
------------------------------------------------------------------------------*/
DALResult ChipInfo_GetChipVersion( ChipInfoClientCtxt *,  uint32 ,  DalChipInfoVersionType *);
DALResult ChipInfo_GetRawChipVersion( ChipInfoClientCtxt *,  uint32 ,  uint32 *);
DALResult ChipInfo_GetChipId( ChipInfoClientCtxt *,  uint32 ,  DalChipInfoIdType *);
DALResult ChipInfo_GetRawChipId( ChipInfoClientCtxt *,  uint32 ,  uint32 *);
DALResult ChipInfo_GetChipIdString( ChipInfoClientCtxt *,  char *,  uint32 );
DALResult ChipInfo_GetChipFamily( ChipInfoClientCtxt *,  uint32 ,  DalChipInfoFamilyType *);
DALResult ChipInfo_GetModemSupport( ChipInfoClientCtxt *,  uint32 ,  DalChipInfoModemType *);
DALResult ChipInfo_GetFoundryId( ChipInfoClientCtxt *,uint32 ,  DalChipInfoFoundryIdType *);
DALResult ChipInfo_GetSerialNumber( ChipInfoClientCtxt *,uint32 ,  DalChipInfoSerialNumType *);

#endif /*DALChipInfo_H*/


