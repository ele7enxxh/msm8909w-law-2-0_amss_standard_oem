#ifndef DALIPCInt_H
#define DALIPCInt_H
/*==============================================================================

FILE:      DalIPCInt.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALFramework.h"
#include "DDIIPCInt.h"

/*------------------------------------------------------------------------------
Declaring a "IPCInt" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct IPCIntDrvCtxt IPCIntDrvCtxt;
typedef struct IPCIntDevCtxt IPCIntDevCtxt;
typedef struct IPCIntClientCtxt IPCIntClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "IPCInt" Vtable
------------------------------------------------------------------------------*/
typedef struct IPCIntDALVtbl IPCIntDALVtbl;
struct IPCIntDALVtbl
{
	int (*IPCInt_DriverInit)(IPCIntDrvCtxt *);
	int (*IPCInt_DriverDeInit)(IPCIntDrvCtxt *);
};

typedef struct
{
  DalIPCIntProcessorType eProcessor;
  uint32   nIPCIntBase;
  uint32*  nIPCIntBasePhys;
  uint32*  nIPCIntBaseVirt;
  uint32   nIPCIntOffset;
}IPCIntCntrlType;

struct IPCIntDevCtxt
{
	//Base Members
	uint32   dwRefs;                                    
    DALDEVICEID DevId;                                  
    uint32   dwDevCtxtRefIdx; 
    IPCIntDrvCtxt  *pIPCIntDrvCtxt;                             
    DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
    uint32 Reserved[16];
    //IPCInt Dev state can be added by developers here
    IPCIntCntrlType ipcint_cntrl;    
    DALSYS_SYNC_OBJECT(IPCIntSyncObject);
    DALSYSSyncHandle  IPCInt_synchronization;
};

struct IPCIntDrvCtxt
{
	//Base Members
	IPCIntDALVtbl IPCIntDALVtbl;
	uint32  dwNumDev;
	uint32  dwSizeDevCtxt;
	uint32  bInit;
    uint32  dwRefs;
	IPCIntDevCtxt IPCIntDevCtxt[1];
    //IPCInt Drv state can be added by developers here
    DalIPCIntProcessorType eSourceProcessor; 
};

/*------------------------------------------------------------------------------
Declaring a "IPCInt" Client Context
------------------------------------------------------------------------------*/
struct IPCIntClientCtxt
{
	//Base Members
	uint32  dwRefs;                     
    uint32  dwAccessMode;  
    void *pPortCtxt;
    IPCIntDevCtxt *pIPCIntDevCtxt;           
    DalIPCIntHandle DalIPCIntHandle; 
	//IPCInt Client state can be added by developers here

};

// Old IPCInt bit-shift constants
#define MSS_SW_INTR_3      15
#define MSS_SW_INTR_2      14
#define MSS_SW_INTR_1      13
#define MSS_SW_INTR_0      12

#define LPASS_3_IRQ        11
#define LPASS_2_IRQ        10
#define LPASS_1_IRQ        9 
#define LPASS_0_IRQ        8   

#define APSS_3_IRQ         7
#define APSS_2_IRQ         6
#define APSS_1_IRQ         5
#define APSS_0_IRQ         4

#define RPM_GP_3_IRQ       3
#define RPM_GP_2_IRQ       2
#define RPM_GP_1_IRQ       1 
#define RPM_GP_0_IRQ       0

// New IPCInt bit-shift constants this is according to VPM 1.1.0 . This needs to change with the next interrupt map.

#define APCS_TZ_IPC_GP_3_IRQ      27     //APCS_TZ_IPC  BIT[24:27] ?
#define APCS_TZ_IPC_GP_2_IRQ      26 
#define APCS_TZ_IPC_GP_1_IRQ      25
#define APCS_TZ_IPC_GP_0_IRQ      24

#define WCN_IPC_GP_3_IRQ            19     //WCN_IPC        BIT[16:19]
#define WCN_IPC_GP_2_IRQ            18
#define WCN_IPC_GP_1_IRQ            17
#define WCN_IPC_GP_0_IRQ            16

#define MPSS_IPC_GP_3_IRQ           15     //MPSS_IPC   On the Modem this is arbitrary.
#define MPSS_IPC_GP_2_IRQ           14
#define MPSS_IPC_GP_1_IRQ           13
#define MPSS_IPC_GP_0_IRQ           12

#define ADSP_IPC_GP_3_IRQ           11    //ADSP_IPC       BIT[8:11] same as LPASS
#define ADSP_IPC_GP_2_IRQ           10
#define ADSP_IPC_GP_1_IRQ           9
#define ADSP_IPC_GP_0_IRQ           8

#define APCS_HLOS_IPC_GP_3_IRQ      7     //APCS_HLOS_IPC    BIT[7:4]
#define APCS_HLOS_IPC_GP_2_IRQ      6
#define APCS_HLOS_IPC_GP_1_IRQ      5
#define APCS_HLOS_IPC_GP_0_IRQ      4

#define RPM_IPC_GP_3_IRQ            3    // bit 3 is actually reserved so this is a placeholder.
#define RPM_IPC_GP_2_IRQ            2    //RPM_IPC        BIT[0:2]  these are valid
#define RPM_IPC_GP_1_IRQ            1
#define RPM_IPC_GP_0_IRQ            0


DALResult IPCInt_DriverInit(IPCIntDrvCtxt *);
DALResult IPCInt_DriverDeInit(IPCIntDrvCtxt *);
DALResult IPCInt_DeviceInit(IPCIntClientCtxt *);
DALResult IPCInt_DeviceDeInit(IPCIntClientCtxt *);
DALResult IPCInt_Reset(IPCIntClientCtxt *);
DALResult IPCInt_PowerEvent(IPCIntClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult IPCInt_Open(IPCIntClientCtxt *, uint32);
DALResult IPCInt_Close(IPCIntClientCtxt *);
DALResult IPCInt_Info(IPCIntClientCtxt *,DalDeviceInfo *, uint32);
DALResult IPCInt_InheritObjects(IPCIntClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);



/*------------------------------------------------------------------------------
Functions specific to DalIPCInt interface
------------------------------------------------------------------------------*/
DALResult IPCInt_Trigger( IPCIntClientCtxt *,  DalIPCIntProcessorType ,  DalIPCIntInterruptType );
DALResult IPCInt_TriggerBySource( IPCIntClientCtxt *,  DalIPCIntProcessorType ,  DalIPCIntProcessorType ,  DalIPCIntInterruptType );
DALResult IPCInt_IsSupported( IPCIntClientCtxt *,  DalIPCIntProcessorType ,  DalIPCIntInterruptType );
DALResult IPCInt_IsSupportedBySource( IPCIntClientCtxt *,  DalIPCIntProcessorType ,  DalIPCIntProcessorType ,  DalIPCIntInterruptType );

#endif /*DALIPCInt_H*/

