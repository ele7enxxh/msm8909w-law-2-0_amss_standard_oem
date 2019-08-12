#include "comdef.h"    /* Definition for basic types and macros */
#include "sm_api.h"


typedef enum {
  GPS_ENTER_SA_RF_VERIF = 0x00,
  GPS_EXIT_SA_RF_VERIF  = 0x01,
  GPS_SA_RF_VERIF_CMD_MAX
} gpsdiag_eSaRfVerifCmdType;

typedef struct
{
  uint16 w_Requests;
  uint32 q_ToA;
  uint8  u_Sv;
  uint8  u_SrchMode;
  uint16 w_PostDetN;
  uint32 q_TaskMask1;
  uint8  u_TaskMask2;
  int32  l_DoppFreq;
  int32  l_DoppFreqRate;
  uint32 q_SubMs;
  uint8 u_DesiredPfa;
  uint16 w_JammerThreshold;
} sm_PdCmd;

typedef PACKED struct PACKED_POST
{
  uint8 u_Requests;
  uint32 u_StartFC;
  uint16 w_TsInfo;
  uint8 u_TsSignalPower;
  uint32 q_TsDoppler;
  uint16 w_NumSamples_1k;
  uint16 w_NumIntegs;
  uint8  u_SmCollect;
  uint8  u_CollectPoint;
  uint8  u_Scale;
  uint16 w_StartDelay;
  int32   l_WbCenterFreq;        /* Wideband FFT Center Frequency ( 24-bit cordic freq word )*/
  int32   l_NbCenterFreq;        /* Narrowband FFT Center Frequency ( 24-bit cordic freq word )*/
//  FLT   f_BcSampRate;            /* BC Sample Rate in Hz */
  uint8   u_NumWbPeaks;          /* Number of Wideband Peaks to Detect */
  uint8   u_NumWbAdjSamp;        /* Number of Samples on Each Side of Peak to Sum */
  uint8   u_NumNbPeaks;          /* Number of Narrowband Peaks to Detect */
  uint8   u_NumNbAdjSamp;        /* Number of Samples on Each Side of Peak to Sum */
}sm_IqTestCmd;

typedef PACKED struct PACKED_POST
{
  uint8  u_Sv;             /* Satellite PRN [0..32]. If zero, operation is track ALL SVs */
  uint8   u_Command;       /* 0 - START_CLEAR_ALL
                            * 1 - START_NO_CLEAR
                            * 2 - STOP_CLEAR_ALL 
                            * 3 - STOP_NO_CLEAR
                            */
}sm_StandaloneTestCmd;



/*typedef enum
{
 
}sm_IqTest_Errors;
*/

typedef struct
{
 uint8 u_EnterExitRfVerifMode; /* Enter/exit SARF Verif mode */
} sm_RfVerifMode;
