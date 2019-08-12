#ifndef DALSoftwareSwitches_H
#define DALSoftwareSwitches_H
/*==============================================================================

FILE:      DalSoftwareSwitches.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALFramework.h"
#include "DDISoftwareSwitches.h"
#include "DALGlbCtxt.h"
/*------------------------------------------------------------------------------
Declaring a "SoftwareSwitches" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct SoftwareSwitchesDrvCtxt SoftwareSwitchesDrvCtxt;
typedef struct SoftwareSwitchesDevCtxt SoftwareSwitchesDevCtxt;
typedef struct SoftwareSwitchesClientCtxt SoftwareSwitchesClientCtxt;
typedef struct DALSoftwareSwitches DALSoftwareSwitches;

struct DALSoftwareSwitches
{
  DALGLB_HEADER hdr;                //DALGLB_HEADER FIELDS
  uint32 state;
  /* This is a flag to indicate whether the switches module has been initializes*/
  uint32 switches_initialized;
};
/*------------------------------------------------------------------------------
Declaring a private "SoftwareSwitches" Vtable
------------------------------------------------------------------------------*/
typedef struct SoftwareSwitchesDALVtbl SoftwareSwitchesDALVtbl;
struct SoftwareSwitchesDALVtbl
{
	int (*SoftwareSwitches_DriverInit)(SoftwareSwitchesDrvCtxt *);
	int (*SoftwareSwitches_DriverDeInit)(SoftwareSwitchesDrvCtxt *);
};

struct SoftwareSwitchesDevCtxt
{
	//Base Members
	uint32   dwRefs;                                    
    DALDEVICEID DevId;                                  
    uint32   dwDevCtxtRefIdx; 
    SoftwareSwitchesDrvCtxt  *pSoftwareSwitchesDrvCtxt;                             
	DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    uint32 Reserved[16];
    //SoftwareSwitches Dev state can be added by developers here
    /* This a synchronization object for the switches module*/
    DALSYS_SYNC_OBJECT(SwSwitchesSyncObject);
    DALSYSSyncHandle   SoftwareSwitchesSync;
    /* This is a pointer to the switches state structure */
    DALSoftwareSwitches *switches_ptr;
};

struct SoftwareSwitchesDrvCtxt
{
	//Base Members
	SoftwareSwitchesDALVtbl SoftwareSwitchesDALVtbl;
	uint32  dwNumDev;
	uint32  dwSizeDevCtxt;
	uint32  bInit;
    uint32  dwRefs;
	SoftwareSwitchesDevCtxt SoftwareSwitchesDevCtxt[1];
    //SoftwareSwitches Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "SoftwareSwitches" Client Context
------------------------------------------------------------------------------*/
struct SoftwareSwitchesClientCtxt
{
	//Base Members
	uint32  dwRefs;                     
    uint32  dwAccessMode;  
    void *pPortCtxt;
    SoftwareSwitchesDevCtxt *pSoftwareSwitchesDevCtxt;            
    DalSoftwareSwitchesHandle DalSoftwareSwitchesHandle; 
	//SoftwareSwitches Client state can be added by developers here

};

DALResult SoftwareSwitches_DriverInit(SoftwareSwitchesDrvCtxt *);
DALResult SoftwareSwitches_DriverDeInit(SoftwareSwitchesDrvCtxt *);
DALResult SoftwareSwitches_DeviceInit(SoftwareSwitchesClientCtxt *);
DALResult SoftwareSwitches_DeviceDeInit(SoftwareSwitchesClientCtxt *);
DALResult SoftwareSwitches_Reset(SoftwareSwitchesClientCtxt *);
DALResult SoftwareSwitches_PowerEvent(SoftwareSwitchesClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult SoftwareSwitches_Open(SoftwareSwitchesClientCtxt *, uint32);
DALResult SoftwareSwitches_Close(SoftwareSwitchesClientCtxt *);
DALResult SoftwareSwitches_Info(SoftwareSwitchesClientCtxt *,DalDeviceInfo *, uint32);
DALResult SoftwareSwitches_InheritObjects(SoftwareSwitchesClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);



/*------------------------------------------------------------------------------
Functions specific to DalSoftwareSwitches interface
------------------------------------------------------------------------------*/
DALResult SoftwareSwitches_SetSwitch( SoftwareSwitchesClientCtxt *, uint32 );
DALResult SoftwareSwitches_ClearSwitch( SoftwareSwitchesClientCtxt *, uint32 );
DALResult SoftwareSwitches_SetAll( SoftwareSwitchesClientCtxt *, uint32 );
DALResult SoftwareSwitches_GetMask( SoftwareSwitchesClientCtxt *, uint32 ,  uint32 *);

#endif /*DALSoftwareSwitches_H*/
/*===========================================================================

  FUNCTION      SoftwareSwitches_DeviceInit

  DESCRIPTION   This api initializes the software switches module on 
                initialization. 

  PARAMETERS    SoftwareSwitchesClientCtxt * pCtxt : The client context of 
                the switches module.

  DEPENDENCIES   None

  RETURN VALUE   DALResult
                
  SIDE EFFECTS  None.

===========================================================================

  FUNCTION      SoftwareSwitches_Set

  DESCRIPTION   This api sets the static switch state data maintained by the 
                bsp for the switches module.

  PARAMETERS    SoftwareSwitchesClientCtxt * pCtxt : The client context of the 
                switches module.
                uint32 switch : the switch provided by the
                api caller.             
                uint32 enable : uint32 flag indication whether the switch 
                should be enabled or disabled.

  DEPENDENCIES   None

  RETURN VALUE   DALResult
                
  SIDE EFFECTS  None.




===========================================================================

  FUNCTION      SoftwareSwitches_Get

  DESCRIPTION   This api gets the static data maintained by the module for the
                switches module.

  PARAMETERS    SoftwareSwitchesClientCtxt * pCtxt : The client context of the 
                switches module.
                uint32 software_switch : the switches provided by the
                api caller.        
                uint32 * state : the pointer to the uint32 variable that the 
                switches api populates with the switch state.

  DEPENDENCIES   None

  RETURN VALUE   DALResult
                
  SIDE EFFECTS  None.


===========================================================================*/


