/*============================================================================

FILE:      DalChipInfoLocal.c

DESCRIPTION:
  This file implements the portion of the ChipInfo DAL that reads information
  from the hardware.

PUBLIC CLASSES:
  ChipInfo_DriverInit
  ChipInfo_DriverDeInit

============================================================================
        Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/chipinfo/src/DalChipInfoLocal.c#2 $
  $DateTime: 2015/09/16 01:43:16 $
  $Author: pwbldsvc $
 
when       who     what, where, why
--------   ---     --------------------------------------------------------
07/11/11   pbi     Fixed warnings.
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALDeviceId.h"
#include "DalChipInfo.h"
#include "DDIHWIO.h"
#include <stringl/stringl.h>


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * DalChipInfoDataLUTType
 *
 * Static per-chip data definitions.
 *
 * szChipId:  String representing the MSM name.
 * nFamily:   Chip Family enum.
 * nModem:    Bitmask of supported modems on this MSM.
 */
typedef struct
{
  const char          *szChipId;
  DalChipInfoFamilyType nFamily;
  DalChipInfoModemType nModem;
} DalChipInfoDataLUTType;


/*
 * Define some modem combination shortcuts.
 */
#define DALCHIPINFO_MODEM_1XEVDO       (DALCHIPINFO_MODEM_1X | DALCHIPINFO_MODEM_EVDO)
#define DALCHIPINFO_MODEM_1XEVDO_REVA  (DALCHIPINFO_MODEM_1XEVDO | DALCHIPINFO_MODEM_EVDO_REVA)
#define DALCHIPINFO_MODEM_1XEVDO_REVB  (DALCHIPINFO_MODEM_1XEVDO_REVA | DALCHIPINFO_MODEM_EVDO_REVB)
#define DALCHIPINFO_MODEM_WWAN         (DALCHIPINFO_MODEM_1XEVDO_REVB | DALCHIPINFO_MODEM_GSM | DALCHIPINFO_MODEM_UMTS)


/*=========================================================================
      Data Definitions
==========================================================================*/

/*
 * DALCHIPINFO_MFGNUM_*
 *
 * Manufacturer IDs
 */
#define DALCHIPINFO_MFGNUM_QUALCOMM 0x170

/*
 * DALCHIPINFO_PARTNUM_*
 *
 * Definitions of part number fields.
 */

#define DALCHIPINFO_PARTNUM_MSM8916        0x705
#define DALCHIPINFO_PARTNUM_APQ8016        0x706
#define DALCHIPINFO_PARTNUM_MSM8216        0x707
#define DALCHIPINFO_PARTNUM_MSM8116        0x708
#define DALCHIPINFO_PARTNUM_MSM8616        0x709

#define DALCHIPINFO_PARTNUM_MSM8936        0x90A
#define DALCHIPINFO_PARTNUM_MSM8939        0x90B
#define DALCHIPINFO_PARTNUM_APQ8036        0x90C
#define DALCHIPINFO_PARTNUM_APQ8039        0x90D
#define DALCHIPINFO_PARTNUM_MSM8236        0x90E
#define DALCHIPINFO_PARTNUM_MSM8636        0x90F

#define DALCHIPINFO_PARTNUM_MSM8909        0x960
#define DALCHIPINFO_PARTNUM_MSM8209        0x961
#define DALCHIPINFO_PARTNUM_MSM8208        0x962
#define DALCHIPINFO_PARTNUM_MDM9209        0x965
#define DALCHIPINFO_PARTNUM_MDM9309        0x966
#define DALCHIPINFO_PARTNUM_MDM9609        0x967
#define DALCHIPINFO_PARTNUM_APQ8009        0x968
#define DALCHIPINFO_PARTNUM_MSM8609        0x984

#define DALCHIPINFO_PARTNUM_MSM8909W       0x051
#define DALCHIPINFO_PARTNUM_APQ8009W       0x052

#define DALCHIPINFO_PARTNUM_MDM9240        0x951	
#define DALCHIPINFO_PARTNUM_MDM9340        0x952	
#define DALCHIPINFO_PARTNUM_MDM9640        0x950	
#define DALCHIPINFO_PARTNUM_MDM9245        0x955	
#define DALCHIPINFO_PARTNUM_MDM9645        0x954	

#define DALCHIPINFO_PARTNUM_MSM8952        0x972	

#define DALCHIPINFO_PARTNUM_MSM8956        0x970
#define DALCHIPINFO_PARTNUM_APQ8056        0x971
#define DALCHIPINFO_PARTNUM_APQ8076        0x983
#define DALCHIPINFO_PARTNUM_MSM8976        0x982

/*
 * amPartMap
 *
 * Map of part numbers to logical parts.
 */
DalChipInfoPartMapType amPartMap[] =
{
  { DALCHIPINFO_PARTNUM_MSM8916,    DALCHIPINFO_ID_MSM8916   },
  { DALCHIPINFO_PARTNUM_MSM8936,    DALCHIPINFO_ID_MSM8936   },
  { DALCHIPINFO_PARTNUM_MSM8939,    DALCHIPINFO_ID_MSM8939   },
  { DALCHIPINFO_PARTNUM_APQ8036,    DALCHIPINFO_ID_APQ8036   },
  { DALCHIPINFO_PARTNUM_APQ8039,    DALCHIPINFO_ID_APQ8039   },
  { DALCHIPINFO_PARTNUM_MSM8236,    DALCHIPINFO_ID_MSM8236   },
  { DALCHIPINFO_PARTNUM_MSM8636,    DALCHIPINFO_ID_MSM8636   },  
  { DALCHIPINFO_PARTNUM_MDM9240,    DALCHIPINFO_ID_MDM9240   },
  { DALCHIPINFO_PARTNUM_MDM9340,    DALCHIPINFO_ID_MDM9340   },
  { DALCHIPINFO_PARTNUM_MDM9640,    DALCHIPINFO_ID_MDM9640   },
  { DALCHIPINFO_PARTNUM_MDM9245,    DALCHIPINFO_ID_MDM9245   },
  { DALCHIPINFO_PARTNUM_MDM9645,    DALCHIPINFO_ID_MDM9645   },
  { DALCHIPINFO_PARTNUM_APQ8016,    DALCHIPINFO_ID_APQ8016   },
  { DALCHIPINFO_PARTNUM_MSM8216,    DALCHIPINFO_ID_MSM8216   },
  { DALCHIPINFO_PARTNUM_MSM8116,    DALCHIPINFO_ID_MSM8116   },
  { DALCHIPINFO_PARTNUM_MSM8616,    DALCHIPINFO_ID_MSM8616   },
  { DALCHIPINFO_PARTNUM_MSM8909,    DALCHIPINFO_ID_MSM8909   },
  { DALCHIPINFO_PARTNUM_MSM8209,    DALCHIPINFO_ID_MSM8209   },
  { DALCHIPINFO_PARTNUM_MSM8208,    DALCHIPINFO_ID_MSM8208   },
  { DALCHIPINFO_PARTNUM_MDM9209,    DALCHIPINFO_ID_MDM9209   },
  { DALCHIPINFO_PARTNUM_MDM9309,    DALCHIPINFO_ID_MDM9309   },
  { DALCHIPINFO_PARTNUM_MDM9609,    DALCHIPINFO_ID_MDM9609   },
  { DALCHIPINFO_PARTNUM_APQ8009,    DALCHIPINFO_ID_APQ8009   },
  { DALCHIPINFO_PARTNUM_MSM8609,    DALCHIPINFO_ID_MSM8609   },
  { DALCHIPINFO_PARTNUM_MSM8952,    DALCHIPINFO_ID_MSM8952   },
  { DALCHIPINFO_PARTNUM_MSM8956,    DALCHIPINFO_ID_MSM8956   },  
  { DALCHIPINFO_PARTNUM_APQ8056,    DALCHIPINFO_ID_APQ8056   },  
  { DALCHIPINFO_PARTNUM_APQ8076,    DALCHIPINFO_ID_APQ8076   },  
  { DALCHIPINFO_PARTNUM_MSM8976,    DALCHIPINFO_ID_MSM8976   },
  { DALCHIPINFO_PARTNUM_MSM8909W,   DALCHIPINFO_ID_MSM8909W  },
  { DALCHIPINFO_PARTNUM_APQ8009W,   DALCHIPINFO_ID_APQ8009W  },
};

/*
 * chipIdMap
 *
 * Map of logical parts to DalChipInfoDataLUT indexes.
 */
DalChipInfoIdMapType chipIdMap[] =
{
  { DALCHIPINFO_ID_UNKNOWN,  0   },
  { DALCHIPINFO_ID_MSM8916,  1  },
  { DALCHIPINFO_ID_MSM8936,  2  },
  { DALCHIPINFO_ID_MSM8939,  3  },
  { DALCHIPINFO_ID_APQ8036,  4  },
  { DALCHIPINFO_ID_APQ8039,  5  },
  { DALCHIPINFO_ID_MSM8236,  6  },
  { DALCHIPINFO_ID_MSM8636,  7  },
  { DALCHIPINFO_ID_MDM9240,  8  },
  { DALCHIPINFO_ID_MDM9340,  9  },
  { DALCHIPINFO_ID_MDM9640,  10 },
  { DALCHIPINFO_ID_MDM9245,  11 },
  { DALCHIPINFO_ID_MDM9645,  12 },  
  { DALCHIPINFO_ID_APQ8016,  13 },  
  { DALCHIPINFO_ID_MSM8216,  14 },  
  { DALCHIPINFO_ID_MSM8116,  15 },  
  { DALCHIPINFO_ID_MSM8616,  16 },  
  { DALCHIPINFO_ID_MSM8909,  17 },
  { DALCHIPINFO_ID_MSM8209,  18 },
  { DALCHIPINFO_ID_MSM8208,  19 },  
  { DALCHIPINFO_ID_MDM9209,  20 },  
  { DALCHIPINFO_ID_MDM9309,  21 },  
  { DALCHIPINFO_ID_MDM9609,  22 },
  { DALCHIPINFO_ID_APQ8009,  23 },
  { DALCHIPINFO_ID_MSM8952,  24 },
  { DALCHIPINFO_ID_MSM8609,  25 },
  { DALCHIPINFO_ID_MSM8956,  26  },  
  { DALCHIPINFO_ID_APQ8056,  27  },  
  { DALCHIPINFO_ID_APQ8076,  28  },  
  { DALCHIPINFO_ID_MSM8976,  29  },
  { DALCHIPINFO_ID_MSM8909W, 30 },
  { DALCHIPINFO_ID_APQ8009W, 31 },
};

#define DALCHIPINFO_ID_MAP_SIZE (sizeof(amPartMap) / sizeof(amPartMap[0]))
#define DALCHIPINFO_CHIPID_MAP_SIZE (sizeof(chipIdMap) / sizeof(chipIdMap[0]))
/*
 * DalChipInfoDataLUT
 *
 * Array of chip id strings, families and modem support indexed by MSM ID.
 */
static const DalChipInfoDataLUTType DalChipInfoDataLUT[DALCHIPINFO_CHIPID_MAP_SIZE] =
{
  { "UNKNOWN",     DALCHIPINFO_FAMILY_UNKNOWN,  0 },
  { "MSM8916",     DALCHIPINFO_FAMILY_MSM8916,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8936",     DALCHIPINFO_FAMILY_MSM8936,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8939",     DALCHIPINFO_FAMILY_MSM8936,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "APQ8036",     DALCHIPINFO_FAMILY_MSM8936,  0 },
  { "APQ8039",     DALCHIPINFO_FAMILY_MSM8936,  0 },
  { "MSM8236",     DALCHIPINFO_FAMILY_MSM8936,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8636",     DALCHIPINFO_FAMILY_MSM8936,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MDM9240",     DALCHIPINFO_FAMILY_MDM9x45,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MDM9340",     DALCHIPINFO_FAMILY_MDM9x45,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MDM9640",     DALCHIPINFO_FAMILY_MDM9x45,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MDM9245",     DALCHIPINFO_FAMILY_MDM9x45,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MDM9645",     DALCHIPINFO_FAMILY_MDM9x45,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "APQ8016",     DALCHIPINFO_FAMILY_MSM8916,  0 },
  { "MSM8216",     DALCHIPINFO_FAMILY_MSM8916,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8116",     DALCHIPINFO_FAMILY_MSM8916,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8616",     DALCHIPINFO_FAMILY_MSM8916,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8909",     DALCHIPINFO_FAMILY_MSM8909,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8209",     DALCHIPINFO_FAMILY_MSM8909,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8208",     DALCHIPINFO_FAMILY_MSM8909,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MDM9209",     DALCHIPINFO_FAMILY_MSM8909,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MDM9309",     DALCHIPINFO_FAMILY_MSM8909,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MDM9609",     DALCHIPINFO_FAMILY_MSM8909,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "APQ8009",     DALCHIPINFO_FAMILY_MSM8909,  0 },
  { "MSM8952",     DALCHIPINFO_FAMILY_MSM8952,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8609",     DALCHIPINFO_FAMILY_MSM8909,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8956",     DALCHIPINFO_FAMILY_MSM8976,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "APQ8056",     DALCHIPINFO_FAMILY_MSM8976,  0 },
  { "APQ8076",     DALCHIPINFO_FAMILY_MSM8976,  0 },
  { "MSM8976",     DALCHIPINFO_FAMILY_MSM8976,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "MSM8909W",    DALCHIPINFO_FAMILY_MSM8909,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
  { "APQ8009W",    DALCHIPINFO_FAMILY_MSM8909,  0 },
};

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**  ChipInfo_GetFamily
**
** ======================================================================== */

void ChipInfo_GetFamily
(
  ChipInfoDrvCtxt       *pCtxt,
  DalChipInfoFamilyType *peFamily,
  uint32                nHWRevNum
)
{
  DalChipInfoIdType ePart;
  uint32             nPart=0,n;

  /*
   * Read part number from hardware.
   */
  ChipInfo_GetPart(pCtxt, &ePart, nHWRevNum);

  /*
   * Look up family.
   */
  if (ePart < DALCHIPINFO_NUM_IDS)
  {
    /*
     * Look up using the table.
     */
    for (n = 0; n < DALCHIPINFO_CHIPID_MAP_SIZE; n++)
    {
      if (ePart == chipIdMap[n].ePart)
      {
        nPart = chipIdMap[n].nPart;
        break;
      }
    }
  
    if (n == DALCHIPINFO_CHIPID_MAP_SIZE)
    {
      nPart = 0;
    }
    *peFamily = DalChipInfoDataLUT[nPart].nFamily;
  }
  else
  {
    *peFamily = DALCHIPINFO_FAMILY_UNKNOWN;
  }

} /* END ChipInfo_GetFamily */


/* ===========================================================================
**  ChipInfo_GetFamilyByPart
**
** ======================================================================== */

void ChipInfo_GetFamilyByPart
(
  DalChipInfoIdType    ePart,
  DalChipInfoFamilyType *peFamily
)
{
  uint32             nPart=0,n;

  /*
   * Look up family.
   */
  if (ePart < DALCHIPINFO_NUM_IDS)
  {
    /*
     * Look up using the table.
     */
    for (n = 0; n < DALCHIPINFO_CHIPID_MAP_SIZE; n++)
    {
      if (ePart == chipIdMap[n].ePart)
      {
        nPart = chipIdMap[n].nPart;
        break;
      }
    }
  
    if (n == DALCHIPINFO_CHIPID_MAP_SIZE)
    {
      nPart = 0;
    }
    *peFamily = DalChipInfoDataLUT[nPart].nFamily;
  }
  else
  {
    *peFamily = DALCHIPINFO_FAMILY_UNKNOWN;
  }

} /* END ChipInfo_GetFamilyByPart */


/* ===========================================================================
**  ChipInfo_GetPart
**
** ======================================================================== */

void ChipInfo_GetPart
(
  ChipInfoDrvCtxt     *pCtxt,
  DalChipInfoIdType   *pePart,
  uint32              nHWRevNum
)
{
  uint32 nPart, n;

  /*
   * Read the part number.
   */
  ChipInfo_GetRawPart(pCtxt, &nPart, nHWRevNum);

  /*
   * Look up using the table.
   */
  for (n = 0; n < DALCHIPINFO_ID_MAP_SIZE; n++)
  {
    if (nPart == amPartMap[n].nPart)
    {
      *pePart = amPartMap[n].ePart;
      break;
    }
  }

  if (n == DALCHIPINFO_ID_MAP_SIZE)
  {
    *pePart = DALCHIPINFO_ID_UNKNOWN;
  }

} /* END ChipInfo_GetPart */


/* ===========================================================================
**  ChipInfo_GetVersion
**
** ======================================================================== */

void ChipInfo_GetVersion
(
  ChipInfoDrvCtxt        *pCtxt,
  DalChipInfoVersionType *pnVersion,
  uint32                 nSOCHWVersion
)
{
  uint32 majNumBmsk, majNumShft, minNumBmsk, minNumShft, majorNum;
  DALSYSPropertyVar PropVar;

 
  DALSYS_GetPropertyValue(pCtxt->hProps,"MAJOR_VERSION_BMSK",0,&PropVar);
  majNumBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"MAJOR_VERSION_SHFT",0,&PropVar);
  majNumShft = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"MINOR_VERSION_BMSK",0,&PropVar);
  minNumBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"MINOR_VERSION_SHFT",0,&PropVar);
  minNumShft = PropVar.Val.dwVal;

  /* If MAJOR VERSION is 0x0 we treat it as 0x1 to provide correct versioninfo on Dino*/
  majorNum = (nSOCHWVersion & majNumBmsk) >> majNumShft;
  if( majorNum == 0x0)
  {
    majorNum = 0x1;
  }
  *pnVersion = DALCHIPINFO_VERSION(
     (majorNum),
      ((nSOCHWVersion & minNumBmsk) >> minNumShft));

} /* END ChipInfo_GetVersion */


/* ===========================================================================
**  ChipInfo_GetManufacturer
**
** ======================================================================== */

void ChipInfo_GetManufacturer
(
  ChipInfoDrvCtxt             *pCtxt,
  DalChipInfoManufacturerType *peManufacturer,
  uint32                      nHWRevNum
)
{
  uint32 pnRawManufacturer;

  ChipInfo_GetRawManufacturer(pCtxt, &pnRawManufacturer, nHWRevNum);
  switch(pnRawManufacturer)
    {
    case DALCHIPINFO_MFGNUM_QUALCOMM:
    {
      *peManufacturer = DALCHIPINFO_MFG_QUALCOMM;
      break;
    }

    default:
    {
      *peManufacturer = DALCHIPINFO_MFG_UNKNOWN;
      break;
    }
  }

} /* END ChipInfo_GetManufacturer */


/* ===========================================================================
**  ChipInfo_GetRawManufacturer
**
** ======================================================================== */

void ChipInfo_GetRawManufacturer
(
  ChipInfoDrvCtxt     *pCtxt,
  uint32 *pnRawManufacturer,
  uint32 nHWRevNum
)
{

  uint32 mfgIdBmsk, mfgIdShft;
  DALSYSPropertyVar PropVar;


  DALSYS_GetPropertyValue(pCtxt->hProps,"QUALCOMM_MFG_ID_BMSK",0,&PropVar);
  mfgIdBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"QUALCOMM_MFG_ID_SHFT",0,&PropVar);
  mfgIdShft = PropVar.Val.dwVal;

  *pnRawManufacturer = (nHWRevNum & mfgIdBmsk) >> mfgIdShft;

} /* END ChipInfo_GetRawManufacturer */


/* ===========================================================================
**  ChipInfo_GetRawPart
**
** ======================================================================== */

void ChipInfo_GetRawPart
(
  ChipInfoDrvCtxt     *pCtxt,
  uint32 *pnRawPart,
  uint32 nHWRevNum
)
{

  uint32 partNumBmsk, partNumShft;
  DALSYSPropertyVar PropVar;

  DALSYS_GetPropertyValue(pCtxt->hProps,"PARTNUM_BMSK",0,&PropVar);
  partNumBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"PARTNUM_SHFT",0,&PropVar);
  partNumShft = PropVar.Val.dwVal;

  *pnRawPart = (nHWRevNum & partNumBmsk) >> partNumShft;

} /* END ChipInfo_GetRawPart */


/* ===========================================================================
**  ChipInfo_GetRawVersion
**
** ======================================================================== */

void ChipInfo_GetRawVersion
(
  ChipInfoDrvCtxt     *pCtxt,
  uint32 *pnRawVersion,
  uint32 nHWRevNum
)
{
  uint32 versionIdBmsk, versionIdShft;
  DALSYSPropertyVar PropVar;

  DALSYS_GetPropertyValue(pCtxt->hProps,"VERSION_ID_BMSK",0,&PropVar);
  versionIdBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"VERSION_ID_SHFT",0,&PropVar);
  versionIdShft = PropVar.Val.dwVal;

  *pnRawVersion = (nHWRevNum & versionIdBmsk) >> versionIdShft;

} /* END ChipInfo_GetRawVersion */


/* ===========================================================================
**  ChipInfo_GetFoundry
**
** ======================================================================== */

void ChipInfo_GetFoundry
(
  ChipInfoDrvCtxt        *pCtxt,
  DalChipInfoFoundryIdType *pnFoundry,
  uint32                 nFoundryNum
)
{
  uint32 nFoundryBmsk, nFoundryShft;
  DALSYSPropertyVar PropVar;
  uint32 nFoundryRaw;

 
  DALSYS_GetPropertyValue(pCtxt->hProps,"FOUNDRY_BMSK",0,&PropVar);
  nFoundryBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"FOUNDRY_SHFT",0,&PropVar);
  nFoundryShft = PropVar.Val.dwVal;
  nFoundryRaw = (nFoundryNum & nFoundryBmsk) >> nFoundryShft;

  if(nFoundryRaw >= (DALCHIPINFO_NUM_FOUNDRYIDS - 1 ))
  {
	  *pnFoundry = DALCHIPINFO_FOUNDRYID_UNKNOWN;
  }
  else
  {
	  *pnFoundry = (DalChipInfoFoundryIdType)(nFoundryRaw + 1);
  }

} /* END ChipInfo_GetFoundry */



/*=========================================================================
      Implementation
==========================================================================*/


/*==========================================================================

  FUNCTION      ChipInfo_DriverInit

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_DriverInit
(
  ChipInfoDrvCtxt     *pCtxt
)
{
  DALResult eResult = DAL_SUCCESS;
  DALSYSPropertyVar PropVar;
  uint32 nHWIORegAddr = 0, nSOCHWVerRegAddr = 0, nFoundryRegAddr = 0, nSerialNumRegAddr = 0;
  uint32 nHWRevNum, nSOCHWVerNum, nFoundryNum;
  uint32 nHWRevNumPhysAddress, nSOCHWVerPhysAddress, nFoundryPhysAddress;
  uint32 n,nPart=0;
    
  /* Get the property handle */
  DALSYS_GetDALPropertyHandle(DALDEVICEID_CHIPINFO,pCtxt->hProps);

  DALSYS_GetPropertyValue(pCtxt->hProps,"HWREVNUM_PHYS_ADDR",0,&PropVar);
  nHWRevNumPhysAddress = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"SOC_HW_VERSION_PHYS_ADDR",0,&PropVar);
  nSOCHWVerPhysAddress = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"FOUNDRY_PHYS_ADDR",0,&PropVar);
  nFoundryPhysAddress = PropVar.Val.dwVal;

  /*-----------------------------------------------------------------------*/
  /* Attach to the HWIO DAL.                                               */
  /*-----------------------------------------------------------------------*/
  eResult = DAL_DeviceAttach(DALDEVICEID_HWIO, &pCtxt->phDalHWIO);
  if (eResult == DAL_SUCCESS)
  {
     eResult = DalHWIO_MapRegionByAddress(pCtxt->phDalHWIO, (uint8*)nHWRevNumPhysAddress,(uint8 **)&nHWIORegAddr);
     eResult |= DalHWIO_MapRegionByAddress(pCtxt->phDalHWIO, (uint8*)nSOCHWVerPhysAddress,(uint8 **)&nSOCHWVerRegAddr);
     eResult |= DalHWIO_MapRegionByAddress(pCtxt->phDalHWIO, (uint8*)nFoundryPhysAddress,(uint8 **)&nFoundryRegAddr);
  }
  else
  {
    nHWIORegAddr = nHWRevNumPhysAddress;
    nSOCHWVerRegAddr = nSOCHWVerPhysAddress;
    nFoundryRegAddr = nFoundryPhysAddress;
  }
  DALSYS_GetPropertyValue(pCtxt->hProps,"HWREVNUM_OFFSET",0,&PropVar);
  nHWIORegAddr = PropVar.Val.dwVal  + nHWIORegAddr;
  DALSYS_GetPropertyValue(pCtxt->hProps,"SOC_HW_VERSION_OFFSET",0,&PropVar);
  nSOCHWVerRegAddr = PropVar.Val.dwVal  + nSOCHWVerRegAddr;
  DALSYS_GetPropertyValue(pCtxt->hProps,"SERIALNUM_OFFSET",0,&PropVar);
  nSerialNumRegAddr = PropVar.Val.dwVal  + nFoundryRegAddr;
  DALSYS_GetPropertyValue(pCtxt->hProps,"FOUNDRY_OFFSET",0,&PropVar);
  nFoundryRegAddr = PropVar.Val.dwVal  + nFoundryRegAddr;

  nHWRevNum = *(uint32*)nHWIORegAddr;
  nSOCHWVerNum = *(uint32*)nSOCHWVerRegAddr;
  nFoundryNum = *(uint32*)nFoundryRegAddr;

  /*-----------------------------------------------------------------------*/
  /* Read chip information from HW.                                        */
  /*-----------------------------------------------------------------------*/

  ChipInfo_GetVersion(pCtxt,(DalChipInfoVersionType *)&pCtxt->nChipVersion, nSOCHWVerNum);
  ChipInfo_GetPart(pCtxt,(DalChipInfoIdType *)&pCtxt->eChipId, nHWRevNum);
  ChipInfo_GetRawVersion(pCtxt,&pCtxt->nRawChipVersion, nHWRevNum);
  ChipInfo_GetRawPart(pCtxt,&pCtxt->nRawChipId, nHWRevNum);
  ChipInfo_GetFamily(pCtxt,(DalChipInfoFamilyType *)&pCtxt->eChipFamily, nHWRevNum);
  ChipInfo_GetFoundry(pCtxt,(DalChipInfoFoundryIdType *)&pCtxt->eFoundryId, nFoundryNum);
  pCtxt->nSerialNum = *(DalChipInfoSerialNumType*)nSerialNumRegAddr;

  /*-----------------------------------------------------------------------*/
  /* Check for override of the ChipId from the properties.                 */
  /*-----------------------------------------------------------------------*/

  if (pCtxt->eChipId == DALCHIPINFO_ID_UNKNOWN)
  {
    eResult = DALSYS_GetPropertyValue(pCtxt->hProps, "ChipIdOverride", 0, &PropVar);

    if (eResult == DAL_SUCCESS)
    {
      pCtxt->eChipId = (DalChipInfoIdType)PropVar.Val.dwVal;

      ChipInfo_GetFamilyByPart(
        (DalChipInfoIdType)pCtxt->eChipId,
        (DalChipInfoFamilyType *)&pCtxt->eChipFamily);

      pCtxt->nChipVersion = 0x00010000;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Fill in derived information.                                          */
  /*-----------------------------------------------------------------------*/

  /*
   * Look up using the table.
   */
  for (n = 0; n < DALCHIPINFO_CHIPID_MAP_SIZE; n++)
  {
    if (pCtxt->eChipId == chipIdMap[n].ePart)
    {
      nPart = chipIdMap[n].nPart;
      break;
    }
  }

  if (n == DALCHIPINFO_CHIPID_MAP_SIZE)
  {
    nPart = 0;
  }
  pCtxt->nModemSupport = DalChipInfoDataLUT[nPart].nModem;
  strlcpy(
    pCtxt->szChipIdString, DalChipInfoDataLUT[nPart].szChipId,
    DALCHIPINFO_MAX_ID_LENGTH);


  /*-----------------------------------------------------------------------*/
  /* Success.                                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END ChipInfo_DriverInit */


/*==========================================================================

  FUNCTION      ChipInfo_DriverDeInit

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_DriverDeInit
(
  ChipInfoDrvCtxt *pCtxt
)
{
  if (pCtxt->phDalHWIO != NULL)
  {
    DalDevice_Close(pCtxt->phDalHWIO);
  }

  /*-----------------------------------------------------------------------*/
  /* Success.                                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END ChipInfo_DriverDeInit */


