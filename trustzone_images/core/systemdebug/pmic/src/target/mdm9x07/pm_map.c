#include "sysdbg_pmic_private.h"

// Bit mask for the valid registers
const PeripheralRegValid_type SMBBPCHGRValid_Mask =
{
.valid1 = 0x0f7f3d30, 
.valid2 = 0x00000000, 
.valid3 = 0x78353fff, 
.valid4 = 0x000001ff, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x3e010000, 
.valid8 = 0x0000f7cf, 
}; 

const PeripheralRegValid_type PBSCLIENTValid_Mask =
{
.valid1 = 0x0f7f033e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000045, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000003, 
}; 

const PeripheralRegValid_type SLPCLKValid_Mask =
{
.valid1 = 0x0f7f0132, 
.valid2 = 0x00000000, 
.valid3 = 0x1f800370, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x3f010000, 
.valid8 = 0x00000007, 
}; 

const PeripheralRegValid_type MPPValid_Mask =
{
.valid1 = 0x0f7f013e, 
.valid2 = 0x00000000, 
.valid3 = 0x0000154f, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000000, 
}; 

const PeripheralRegValid_type LPGLUTValid_Mask =
{
.valid1 = 0x00000032, 
.valid2 = 0x00000000, 
.valid3 = 0x7fffffff, 
.valid4 = 0x7fffffff, 
.valid5 = 0x7fffffff, 
.valid6 = 0x7fffffff, 
.valid7 = 0x0e010100, 
.valid8 = 0x00000000, 
}; 

const PeripheralRegValid_type SMBBPBOOSTValid_Mask =
{
.valid1 = 0x0f7f0130, 
.valid2 = 0x00000000, 
.valid3 = 0x00003072, 
.valid4 = 0x03fd0a00, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x0000003f, 
}; 

const PeripheralRegValid_type BCLKGENCLKValid_Mask =
{
.valid1 = 0x00000032, 
.valid2 = 0x00000000, 
.valid3 = 0x00030040, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010003, 
.valid8 = 0x00000004, 
}; 

const PeripheralRegValid_type VREFLPDDRValid_Mask =
{
.valid1 = 0x0000013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000050, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00030000, 
}; 

const PeripheralRegValid_type LPG3DValid_Mask =
{
.valid1 = 0x00000032, 
.valid2 = 0x00000000, 
.valid3 = 0x000000e0, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x00000000, 
}; 

const PeripheralRegValid_type HFBUCK2CTRLValid_Mask =
{
.valid1 = 0x0f7f013e, 
.valid2 = 0x00000000, 
.valid3 = 0x7f6e0173, 
.valid4 = 0x04000b0f, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x001f0007, 
}; 

const PeripheralRegValid_type COINCELLValid_Mask =
{
.valid1 = 0x0000013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000070, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x00030001, 
}; 

const PeripheralRegValid_type SPMIPDIGOPTIONSValid_Mask =
{
.valid1 = 0x00000000, 
.valid2 = 0x00000000, 
.valid3 = 0x000000f0, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x00000000, 
.valid8 = 0x00000000, 
}; 

const PeripheralRegValid_type VADCADJValid_Mask =
{
.valid1 = 0x0f7f033e, 
.valid2 = 0x00000000, 
.valid3 = 0x7eb70141, 
.valid4 = 0x00000003, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000083, 
}; 

const PeripheralRegValid_type WLEDValid_Mask =
{
.valid1 = 0x0f7f013e, 
.valid2 = 0x00000000, 
.valid3 = 0x03ffffff, 
.valid4 = 0x004f004f, 
.valid5 = 0x0000004f, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00ff003f, 
}; 

const PeripheralRegValid_type HFBUCK2PSValid_Mask =
{
.valid1 = 0x0000003e, 
.valid2 = 0x00000000, 
.valid3 = 0x00001c00, 
.valid4 = 0x023f0000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x00070018, 
}; 

const PeripheralRegValid_type BBCLKValid_Mask =
{
.valid1 = 0x0000013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000150, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000000, 
}; 

const PeripheralRegValid_type RFCLKValid_Mask =
{
.valid1 = 0x0000013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000150, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000000, 
}; 

const PeripheralRegValid_type LDOXODIGValid_Mask =
{
.valid1 = 0x0000013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000063, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000004, 
}; 

const PeripheralRegValid_type TIMERValid_Mask =
{
.valid1 = 0x0f7f0132, 
.valid2 = 0x00000000, 
.valid3 = 0x00000365, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000003, 
}; 

const PeripheralRegValid_type SPMIPDIGValid_Mask =
{
.valid1 = 0x0f7f793e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000001, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x00000007, 
}; 

const PeripheralRegValid_type LDODIGValid_Mask =
{
.valid1 = 0x0f7f073e, 
.valid2 = 0x00000000, 
.valid3 = 0x00041563, 
.valid4 = 0x00000f00, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x0003000f, 
}; 

const PeripheralRegValid_type BMSValid_Mask =
{
.valid1 = 0x0f7f0132, 
.valid2 = 0x00000000, 
.valid3 = 0x3c0b1f5d, 
.valid4 = 0x7eaab43f, 
.valid5 = 0x43337c3f, 
.valid6 = 0x7fff1f1f, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000007, 
}; 

const PeripheralRegValid_type CLKDISTValid_Mask =
{
.valid1 = 0x0f7f013e, 
.valid2 = 0x00000000, 
.valid3 = 0x000e056f, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x00070017, 
}; 

const PeripheralRegValid_type INTBUSARBDIGValid_Mask =
{
.valid1 = 0x00000132, 
.valid2 = 0x00000000, 
.valid3 = 0x00000010, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x00000004, 
}; 

const PeripheralRegValid_type BCLKGENMAINValid_Mask =
{
.valid1 = 0x0f7f013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00030040, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x0000003f, 
}; 

const PeripheralRegValid_type REVIDPM8026Valid_Mask =
{
.valid1 = 0x0000013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000000, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x00010000, 
.valid8 = 0x00000001, 
}; 

const PeripheralRegValid_type SWITCHDIGValid_Mask =
{
.valid1 = 0x0f7f013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00001560, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00030007, 
}; 

const PeripheralRegValid_type BUAValid_Mask =
{
.valid1 = 0x0f7f0332, 
.valid2 = 0x00000000, 
.valid3 = 0x00000041, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000007, 
}; 

const PeripheralRegValid_type TEMPALARMValid_Mask =
{
.valid1 = 0x0f7f013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000045, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x0003000f, 
}; 

const PeripheralRegValid_type SMBBPUSBValid_Mask =
{
.valid1 = 0x0f7f7730, 
.valid2 = 0x00000000, 
.valid3 = 0x001fc19d, 
.valid4 = 0x00000007, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x3e010000, 
.valid8 = 0x00073fff, 
}; 

const PeripheralRegValid_type MISCPM8026Valid_Mask =
{
.valid1 = 0x0f7f013e, 
.valid2 = 0x00000000, 
.valid3 = 0x0000007d, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x00030001, 
}; 

const PeripheralRegValid_type MBGDIGValid_Mask =
{
.valid1 = 0x0000013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000050, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x01ff000c, 
}; 

const PeripheralRegValid_type FTS2PSValid_Mask =
{
.valid1 = 0x0000003e, 
.valid2 = 0x00000000, 
.valid3 = 0x001f0003, 
.valid4 = 0x7fff001f, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010003, 
.valid8 = 0x000701bc, 
}; 

const PeripheralRegValid_type INTRDIGValid_Mask =
{
.valid1 = 0x00000332, 
.valid2 = 0x00000000, 
.valid3 = 0x00000041, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000001, 
}; 

const PeripheralRegValid_type DIFFCLKValid_Mask =
{
.valid1 = 0x0000013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000054, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000004, 
}; 

const PeripheralRegValid_type IADCADJValid_Mask =
{
.valid1 = 0x0f7f033e, 
.valid2 = 0x00000000, 
.valid3 = 0x7eb50141, 
.valid4 = 0x00000003, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x003c0043, 
}; 

const PeripheralRegValid_type PBSCOREValid_Mask =
{
.valid1 = 0x0000ff3e, 
.valid2 = 0x00000000, 
.valid3 = 0x0000ff0f, 
.valid4 = 0x00000000, 
.valid5 = 0x00ff0000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x0000001f, 
}; 

const PeripheralRegValid_type SMBBPBUCKValid_Mask =
{
.valid1 = 0x0f7f0130, 
.valid2 = 0x00000000, 
.valid3 = 0x00000040, 
.valid4 = 0x01de5d3f, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x07ffffff, 
}; 

const PeripheralRegValid_type FLSHDRVRValid_Mask =
{
.valid1 = 0x0f7f073e, 
.valid2 = 0x00000000, 
.valid3 = 0x07ffffdf, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x001f00ff, 
}; 

const PeripheralRegValid_type SMBBPMISCValid_Mask =
{
.valid1 = 0x0f7f003e, 
.valid2 = 0x00000000, 
.valid3 = 0x000001ff, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x7e816000, 
.valid8 = 0x0000001f, 
}; 

const PeripheralRegValid_type IADCValid_Mask =
{
.valid1 = 0x0f7f033e, 
.valid2 = 0x00000000, 
.valid3 = 0x7eb50141, 
.valid4 = 0x00000003, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x003f01ff, 
}; 

const PeripheralRegValid_type FTS2CTRLValid_Mask =
{
.valid1 = 0x0f7f0f3e, 
.valid2 = 0x00000000, 
.valid3 = 0x003f0177, 
.valid4 = 0x0000ffff, 
.valid5 = 0x00000000, 
.valid6 = 0x00ff0000, 
.valid7 = 0x0f010003, 
.valid8 = 0x007f7fbf, 
}; 

const PeripheralRegValid_type TRIMValid_Mask =
{
.valid1 = 0x00000f3e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000000, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x0000007f, 
}; 

const PeripheralRegValid_type XOCOREValid_Mask =
{
.valid1 = 0x0000013e, 
.valid2 = 0x00000000, 
.valid3 = 0x5c003051, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x00000014, 
}; 

const PeripheralRegValid_type LPGSLICEValid_Mask =
{
.valid1 = 0x0f7f0032, 
.valid2 = 0x00000000, 
.valid3 = 0x00ff007f, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x0000003f, 
}; 

const PeripheralRegValid_type DIVCLKValid_Mask =
{
.valid1 = 0x00000132, 
.valid2 = 0x00000000, 
.valid3 = 0x00000048, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000000, 
}; 

const PeripheralRegValid_type RTCALARMValid_Mask =
{
.valid1 = 0x0f7f0132, 
.valid2 = 0x00000000, 
.valid3 = 0x0000014f, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000003, 
}; 

const PeripheralRegValid_type RTCRWValid_Mask =
{
.valid1 = 0x0f7f0132, 
.valid2 = 0x00000000, 
.valid3 = 0x00000f5f, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000007, 
}; 

const PeripheralRegValid_type LDOSTEPPERDIGValid_Mask =
{
.valid1 = 0x0f7f073e, 
.valid2 = 0x00000000, 
.valid3 = 0x00041563, 
.valid4 = 0x00000f02, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x0003000f, 
}; 

const PeripheralRegValid_type VADCBTMValid_Mask =
{
.valid1 = 0x0f7f0f3e, 
.valid2 = 0x00000000, 
.valid3 = 0x7fb7014f, 
.valid4 = 0x3f3f3f03, 
.valid5 = 0x3f3f3f3f, 
.valid6 = 0x00003fff, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000007, 
}; 

const PeripheralRegValid_type GPIOValid_Mask =
{
.valid1 = 0x0f7f013e, 
.valid2 = 0x00000000, 
.valid3 = 0x0000006f, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000000, 
}; 

const PeripheralRegValid_type PONValid_Mask =
{
.valid1 = 0x0f7ffdbe, 
.valid2 = 0x00000000, 
.valid3 = 0x0dffffff, 
.valid4 = 0x00330cdc, 
.valid5 = 0x0033f50d, 
.valid6 = 0x00000000, 
.valid7 = 0x0e010000, 
.valid8 = 0x000700df, 
}; 

const PeripheralRegValid_type SMBBPBATIFValid_Mask =
{
.valid1 = 0x0f7f1f30, 
.valid2 = 0x00000000, 
.valid3 = 0x0000c780, 
.valid4 = 0x00000000, 
.valid5 = 0x00770000, 
.valid6 = 0x00000000, 
.valid7 = 0x3e010000, 
.valid8 = 0x000300ff, 
}; 

const PeripheralRegValid_type VADCValid_Mask =
{
.valid1 = 0x0f7f033e, 
.valid2 = 0x00000000, 
.valid3 = 0x7eb70141, 
.valid4 = 0x00000003, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x003f00ff, 
}; 

const PeripheralRegValid_type VIBDRVDIGValid_Mask =
{
.valid1 = 0x0000013e, 
.valid2 = 0x00000000, 
.valid3 = 0x00000042, 
.valid4 = 0x00000000, 
.valid5 = 0x00000000, 
.valid6 = 0x00000000, 
.valid7 = 0x0f010000, 
.valid8 = 0x00000004, 
}; 

// Dummy mask where I assume I know what I am doing
const PeripheralRegValid_type DummyValid_Mask = 
{
.valid1 = 0xFFFFFFFF,
.valid2 = 0xFFFFFFFF,
.valid3 = 0xFFFFFFFF,
.valid4 = 0xFFFFFFFF,
.valid5 = 0xFFFFFFFF,
.valid6 = 0xFFFFFFFF,
.valid7 = 0xFFFFFFFF,
.valid8 = 0xFFFFFFFF,
};


PMMapType PM_map_data = 
{
	.pmap = 
	{
		// {BBA_CLK1       , SPMI_SLAVE_0, &BBCLKValid_Mask, 0x5100},
		// {BBA_CLK2       , SPMI_SLAVE_0, &BBCLKValid_Mask, 0x5200},
		// {BCLKA_GEN_MAIN  , SPMI_SLAVE_1, &BCLKGENMAINValid_Mask, 0x1000},
		// {BMS1A          , SPMI_SLAVE_0, &BMSValid_Mask, 0x4000},
		// {BUAA           , SPMI_SLAVE_0, &BUAValid_Mask, 0x1C00},
		// {BUSA           , SPMI_SLAVE_0, &INTBUSARBDIGValid_Mask, 0x0400},
		// {CLKA_DIST      , SPMI_SLAVE_0, &CLKDISTValid_Mask, 0x5900},
		// {COINA          , SPMI_SLAVE_0, &COINCELLValid_Mask, 0x2800},
		// {DIFFA_CLK1     , SPMI_SLAVE_0, &DIFFCLKValid_Mask, 0x5700},
		// {DIVA_CLK1      , SPMI_SLAVE_0, &DIVCLKValid_Mask, 0x5B00},
		// {DIVA_CLK2      , SPMI_SLAVE_0, &DIVCLKValid_Mask, 0x5C00},
		// {DIVA_CLK3      , SPMI_SLAVE_0, &DIVCLKValid_Mask, 0x5D00},
		// {FLASH1A        , SPMI_SLAVE_1, &FLSHDRVRValid_Mask, 0xD300},
		// {GPIO1A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC000},
		// {GPIO2A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC100},
		// {GPIO3A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC200},
		// {GPIO4A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC300},
		// {GPIO5A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC400},
		// {GPIO6A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC500},
		// {GPIO7A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC600},
		// {GPIO8A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC700},
		// {IADC1A_USR     , SPMI_SLAVE_0, &IADCValid_Mask, 0x3600},
		// {IADC2A_BMS     , SPMI_SLAVE_0, &IADCADJValid_Mask, 0x3800},
		// {IADC2A_MDM     , SPMI_SLAVE_0, &IADCADJValid_Mask, 0x3700},
		// {INTA           , SPMI_SLAVE_0, &INTRDIGValid_Mask, 0x0500},
		{LDO1A          , SPMI_SLAVE_1, &DummyValid_Mask, 0x4000},
		// {LDO10A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4900},
		// {LDO11A         , SPMI_SLAVE_1, &LDOXODIGValid_Mask, 0x4A00},
		{LDO12A         , SPMI_SLAVE_1, &DummyValid_Mask, 0x4B00},
		// {LDO13A         , SPMI_SLAVE_1, &LDOXODIGValid_Mask, 0x4C00},
		// {LDO14A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4D00},
		// {LDO15A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4E00},
		// {LDO16A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4F00},
		// {LDO17A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5000},
		// {LDO18A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5100},
		// {LDO19A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5200},
		// {LDO2A          , SPMI_SLAVE_1, &LDOSTEPPERDIGValid_Mask, 0x4100},
		// {LDO20A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5300},
		// {LDO21A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5400},
		// {LDO22A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5500},
		// {LDO23A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5600},
		// {LDO24A         , SPMI_SLAVE_1, &LDOSTEPPERDIGValid_Mask, 0x5700},
		// {LDO25A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5800},
		// {LDO26A         , SPMI_SLAVE_1, &LDOSTEPPERDIGValid_Mask, 0x5900},
		// {LDO27A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5A00},
		// {LDO28A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5B00},
		// {LDO3A          , SPMI_SLAVE_1, &LDOSTEPPERDIGValid_Mask, 0x4200},
		// {LDO4A          , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4300},
		// {LDO5A          , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4400},
		// {LDO6A          , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4500},
		{LDO7A          , SPMI_SLAVE_1, &DummyValid_Mask, 0x4600},
		// {LDO8A          , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4700},
		// {LDO9A          , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4800},
		// {LPGA_CHAN1     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB100},
		// {LPGA_CHAN2     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB200},
		// {LPGA_CHAN3     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB300},
		// {LPGA_CHAN4     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB400},
		// {LPGA_CHAN5     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB500},
		// {LPGA_CHAN6     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB600},
		// {LPGA_LUT       , SPMI_SLAVE_1, &LPGLUTValid_Mask, 0xB000},
		// {LVS1A          , SPMI_SLAVE_1, &SWITCHDIGValid_Mask, 0x8000},
		// {MBG1A          , SPMI_SLAVE_0, &MBGDIGValid_Mask, 0x2C00},
		// {MISCA          , SPMI_SLAVE_0, &MISCPM8026Valid_Mask, 0x0900},
		// {MPP1A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA000},
		// {MPP2A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA100},
		// {MPP3A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA200},
		// {MPP4A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA300},
		// {MPP5A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA400},
		// {MPP6A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA500},
		// {MPP7A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA600},
		// {MPP8A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA700},
		// {PBSA_CLIENT0   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7100},
		// {PBSA_CLIENT1   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7200},
		// {PBSA_CLIENT10  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7B00},
		// {PBSA_CLIENT11  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7C00},
		// {PBSA_CLIENT12  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7D00},
		// {PBSA_CLIENT13  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7E00},
		// {PBSA_CLIENT14  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7F00},
		// {PBSA_CLIENT15  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x8000},
		// {PBSA_CLIENT2   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7300},
		// {PBSA_CLIENT3   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7400},
		// {PBSA_CLIENT4   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7500},
		// {PBSA_CLIENT5   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7600},
		// {PBSA_CLIENT6   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7700},
		// {PBSA_CLIENT7   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7800},
		// {PBSA_CLIENT8   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7900},
		// {PBSA_CLIENT9   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7A00},
		// {PBSA_CORE      , SPMI_SLAVE_0, &PBSCOREValid_Mask, 0x7000},
		{PONA           , SPMI_SLAVE_0, &DummyValid_Mask, 0x0800}, //&PONValid_Mask
		// {PWMA_3D        , SPMI_SLAVE_1, &LPG3DValid_Mask, 0xBC00},
		// {REVIDA         , SPMI_SLAVE_0, &REVIDPM8026Valid_Mask, 0x0100},
		// {RFA_CLK1       , SPMI_SLAVE_0, &RFCLKValid_Mask, 0x5400},
		// {RFA_CLK2       , SPMI_SLAVE_0, &RFCLKValid_Mask, 0x5500},
		// {RFA_CLK3       , SPMI_SLAVE_0, &RFCLKValid_Mask, 0x5600},
		// {RTCA_ALARM     , SPMI_SLAVE_0, &RTCALARMValid_Mask, 0x6100},
		// {RTCA_TIMER     , SPMI_SLAVE_0, &TIMERValid_Mask, 0x6200},
		{RTCA_WR        , SPMI_SLAVE_0, &DummyValid_Mask, 0x6000}, //&RTCRWValid_Mask
		{S1A_CTRL       , SPMI_SLAVE_1, &DummyValid_Mask, 0x1400}, //&FTS2CTRLValid_Mask
		{S1A_FREQ       , SPMI_SLAVE_1, &DummyValid_Mask, 0x1600}, //&BCLKGENCLKValid_Mask
		{S1A_PS         , SPMI_SLAVE_1, &DummyValid_Mask, 0x1500}, //&FTS2PSValid_Mask
		// {S2A_CTRL       , SPMI_SLAVE_1, &FTS2CTRLValid_Mask, 0x1700},
		// {S2A_FREQ       , SPMI_SLAVE_1, &BCLKGENCLKValid_Mask, 0x1900},
		// {S2A_PS         , SPMI_SLAVE_1, &FTS2PSValid_Mask, 0x1800},
		 {S3A_CTRL       , SPMI_SLAVE_1, &DummyValid_Mask, 0x1A00},
		// {S3A_FREQ       , SPMI_SLAVE_1, &BCLKGENCLKValid_Mask, 0x1C00},
		// {S3A_PS         , SPMI_SLAVE_1, &HFBUCK2PSValid_Mask, 0x1B00},
		{S4A_CTRL       , SPMI_SLAVE_1, &DummyValid_Mask, 0x1D00},//&HFBUCK2CTRLValid_Mask
		{S4A_FREQ       , SPMI_SLAVE_1, &DummyValid_Mask, 0x1F00}, //BCLKGENCLKValid_Mask
		{S4A_PS         , SPMI_SLAVE_1, &DummyValid_Mask, 0x1E00}, //HFBUCK2PSValid_Mask
		// {S5A_CTRL       , SPMI_SLAVE_1, &HFBUCK2CTRLValid_Mask, 0x2000},
		// {S5A_FREQ       , SPMI_SLAVE_1, &BCLKGENCLKValid_Mask, 0x2200},
		// {S5A_PS         , SPMI_SLAVE_1, &HFBUCK2PSValid_Mask, 0x2100},
		// {SLEEPA_CLK1    , SPMI_SLAVE_0, &SLPCLKValid_Mask, 0x5A00},
		// {SMBBPA_BAT_IF  , SPMI_SLAVE_0, &SMBBPBATIFValid_Mask, 0x1200},
		// {SMBBPA_BOOST   , SPMI_SLAVE_0, &SMBBPBOOSTValid_Mask, 0x1500},
		// {SMBBPA_BUCK    , SPMI_SLAVE_0, &SMBBPBUCKValid_Mask, 0x1100},
		// {SMBBPA_CHGR    , SPMI_SLAVE_0, &SMBBPCHGRValid_Mask, 0x1000},
		// {SMBBPA_FREQ    , SPMI_SLAVE_0, &BCLKGENCLKValid_Mask, 0x1700},
		// {SMBBPA_MISC    , SPMI_SLAVE_0, &SMBBPMISCValid_Mask, 0x1600},
		// {SMBBPA_USB_CHGPTH  , SPMI_SLAVE_0, &SMBBPUSBValid_Mask, 0x1300},
		// {SPMIA          , SPMI_SLAVE_0, &SPMIPDIGValid_Mask, 0x0600},
		// {SPMIA_OPTIONS  , SPMI_SLAVE_2, &SPMIPDIGOPTIONSValid_Mask, 0x0600},
		// {TEMPA_ALARM    , SPMI_SLAVE_0, &TEMPALARMValid_Mask, 0x2400},
		// {TRIMA          , SPMI_SLAVE_0, &TRIMValid_Mask, 0xFE00},
		// {VADC1A_USR     , SPMI_SLAVE_0, &VADCValid_Mask, 0x3100},
		// {VADC2A_BTM     , SPMI_SLAVE_0, &VADCBTMValid_Mask, 0x3400},
		// {VADC3A_BMS     , SPMI_SLAVE_0, &VADCADJValid_Mask, 0x3300},
		// {VADC3A_MDM     , SPMI_SLAVE_0, &VADCADJValid_Mask, 0x3200},
		// {VIB1A          , SPMI_SLAVE_1, &VIBDRVDIGValid_Mask, 0xC000},
		// {VREFLPDDRA     , SPMI_SLAVE_0, &VREFLPDDRValid_Mask, 0x0A00},
		// {WLED1A         , SPMI_SLAVE_1, &WLEDValid_Mask, 0xD800},
		// {XOA            , SPMI_SLAVE_0, &XOCOREValid_Mask, 0x5000},
		// {BBA_CLK1       , SPMI_SLAVE_0, &BBCLKValid_Mask, 0x5100},
		// {BBA_CLK2       , SPMI_SLAVE_0, &BBCLKValid_Mask, 0x5200},
		// {BCLKA_GEN_MAIN  , SPMI_SLAVE_1, &BCLKGENMAINValid_Mask, 0x1000},
		// {BMS1A          , SPMI_SLAVE_0, &BMSValid_Mask, 0x4000},
		// {BUAA           , SPMI_SLAVE_0, &BUAValid_Mask, 0x1C00},
		// {BUSA           , SPMI_SLAVE_0, &INTBUSARBDIGValid_Mask, 0x0400},
		// {CLKA_DIST      , SPMI_SLAVE_0, &CLKDISTValid_Mask, 0x5900},
		// {COINA          , SPMI_SLAVE_0, &COINCELLValid_Mask, 0x2800},
		// {DIFFA_CLK1     , SPMI_SLAVE_0, &DIFFCLKValid_Mask, 0x5700},
		// {DIVA_CLK1      , SPMI_SLAVE_0, &DIVCLKValid_Mask, 0x5B00},
		// {DIVA_CLK2      , SPMI_SLAVE_0, &DIVCLKValid_Mask, 0x5C00},
		// {DIVA_CLK3      , SPMI_SLAVE_0, &DIVCLKValid_Mask, 0x5D00},
		// {FLASH1A        , SPMI_SLAVE_1, &FLSHDRVRValid_Mask, 0xD300},
		// {GPIO1A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC000},
		// {GPIO2A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC100},
		// {GPIO3A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC200},
		// {GPIO4A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC300},
		// {GPIO5A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC400},
		// {GPIO6A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC500},
		// {GPIO7A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC600},
		// {GPIO8A         , SPMI_SLAVE_0, &GPIOValid_Mask, 0xC700},
		// {IADC1A_USR     , SPMI_SLAVE_0, &IADCValid_Mask, 0x3600},
		// {IADC2A_BMS     , SPMI_SLAVE_0, &IADCADJValid_Mask, 0x3800},
		// {IADC2A_MDM     , SPMI_SLAVE_0, &IADCADJValid_Mask, 0x3700},
		// {INTA           , SPMI_SLAVE_0, &INTRDIGValid_Mask, 0x0500},
		// {LDO1A          , SPMI_SLAVE_1, &LDOSTEPPERDIGValid_Mask, 0x4000},
		{LDO10A         , SPMI_SLAVE_1, &DummyValid_Mask, 0x4900}, //&LDODIGValid_Mask
		// {LDO11A         , SPMI_SLAVE_1, &LDOXODIGValid_Mask, 0x4A00},
		// {LDO12A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4B00},
		// {LDO13A         , SPMI_SLAVE_1, &LDOXODIGValid_Mask, 0x4C00},
		// {LDO14A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4D00},
		// {LDO15A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4E00},
		// {LDO16A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4F00},
		// {LDO17A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5000},
		// {LDO18A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5100},
		// {LDO19A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5200},
		{LDO2A          , SPMI_SLAVE_1, &DummyValid_Mask, 0x4100}, //&LDOSTEPPERDIGValid_Mask
		// {LDO20A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5300},
		// {LDO21A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5400},
		// {LDO22A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5500},
		// {LDO23A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5600},
		// {LDO24A         , SPMI_SLAVE_1, &LDOSTEPPERDIGValid_Mask, 0x5700},
		// {LDO25A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5800},
		// {LDO26A         , SPMI_SLAVE_1, &LDOSTEPPERDIGValid_Mask, 0x5900},
		// {LDO27A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5A00},
		// {LDO28A         , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x5B00},
		{LDO3A          , SPMI_SLAVE_1, &DummyValid_Mask, 0x4200}, //&LDOSTEPPERDIGValid_Mask
		// {LDO4A          , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4300},
		// {LDO5A          , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4400},
		{LDO6A          , SPMI_SLAVE_1, &DummyValid_Mask, 0x4500}, //&LDODIGValid_Mask
		// {LDO7A          , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4600},
		// {LDO8A          , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4700},
		// {LDO9A          , SPMI_SLAVE_1, &LDODIGValid_Mask, 0x4800},
		// {LPGA_CHAN1     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB100},
		// {LPGA_CHAN2     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB200},
		// {LPGA_CHAN3     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB300},
		// {LPGA_CHAN4     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB400},
		// {LPGA_CHAN5     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB500},
		// {LPGA_CHAN6     , SPMI_SLAVE_1, &LPGSLICEValid_Mask, 0xB600},
		// {LPGA_LUT       , SPMI_SLAVE_1, &LPGLUTValid_Mask, 0xB000},
		// {LVS1A          , SPMI_SLAVE_1, &SWITCHDIGValid_Mask, 0x8000},
		// {MBG1A          , SPMI_SLAVE_0, &MBGDIGValid_Mask, 0x2C00},
		// {MISCA          , SPMI_SLAVE_0, &MISCPM8026Valid_Mask, 0x0900},
		// {MPP1A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA000},
		// {MPP2A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA100},
		// {MPP3A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA200},
		// {MPP4A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA300},
		// {MPP5A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA400},
		// {MPP6A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA500},
		// {MPP7A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA600},
		// {MPP8A          , SPMI_SLAVE_0, &MPPValid_Mask, 0xA700},
		// {PBSA_CLIENT0   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7100},
		// {PBSA_CLIENT1   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7200},
		// {PBSA_CLIENT10  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7B00},
		// {PBSA_CLIENT11  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7C00},
		// {PBSA_CLIENT12  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7D00},
		// {PBSA_CLIENT13  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7E00},
		// {PBSA_CLIENT14  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7F00},
		// {PBSA_CLIENT15  , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x8000},
		// {PBSA_CLIENT2   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7300},
		// {PBSA_CLIENT3   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7400},
		// {PBSA_CLIENT4   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7500},
		// {PBSA_CLIENT5   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7600},
		// {PBSA_CLIENT6   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7700},
		// {PBSA_CLIENT7   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7800},
		// {PBSA_CLIENT8   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7900},
		// {PBSA_CLIENT9   , SPMI_SLAVE_0, &PBSCLIENTValid_Mask, 0x7A00},
		// {PBSA_CORE      , SPMI_SLAVE_0, &PBSCOREValid_Mask, 0x7000},
		// {PONA           , SPMI_SLAVE_0, &PONValid_Mask, 0x0800},
		// {PWMA_3D        , SPMI_SLAVE_1, &LPG3DValid_Mask, 0xBC00},
		// {REVIDA         , SPMI_SLAVE_0, &REVIDPM8026Valid_Mask, 0x0100},
		// {RFA_CLK1       , SPMI_SLAVE_0, &RFCLKValid_Mask, 0x5400},
		// {RFA_CLK2       , SPMI_SLAVE_0, &RFCLKValid_Mask, 0x5500},
		// {RFA_CLK3       , SPMI_SLAVE_0, &RFCLKValid_Mask, 0x5600},
		// {RTCA_ALARM     , SPMI_SLAVE_0, &RTCALARMValid_Mask, 0x6100},
		// {RTCA_TIMER     , SPMI_SLAVE_0, &TIMERValid_Mask, 0x6200},
		// {RTCA_WR        , SPMI_SLAVE_0, &RTCRWValid_Mask, 0x6000},
		// {S1A_CTRL       , SPMI_SLAVE_1, &FTS2CTRLValid_Mask, 0x1400},
		// {S1A_FREQ       , SPMI_SLAVE_1, &BCLKGENCLKValid_Mask, 0x1600},
		// {S1A_PS         , SPMI_SLAVE_1, &FTS2PSValid_Mask, 0x1500},
		// {S2A_CTRL       , SPMI_SLAVE_1, &FTS2CTRLValid_Mask, 0x1700},
		// {S2A_FREQ       , SPMI_SLAVE_1, &BCLKGENCLKValid_Mask, 0x1900},
		// {S2A_PS         , SPMI_SLAVE_1, &FTS2PSValid_Mask, 0x1800},
		// {S3A_CTRL       , SPMI_SLAVE_1, &HFBUCK2CTRLValid_Mask, 0x1A00},
		// {S3A_FREQ       , SPMI_SLAVE_1, &BCLKGENCLKValid_Mask, 0x1C00},
		// {S3A_PS         , SPMI_SLAVE_1, &HFBUCK2PSValid_Mask, 0x1B00},
		// {S4A_CTRL       , SPMI_SLAVE_1, &HFBUCK2CTRLValid_Mask, 0x1D00},
		// {S4A_FREQ       , SPMI_SLAVE_1, &BCLKGENCLKValid_Mask, 0x1F00},
		// {S4A_PS         , SPMI_SLAVE_1, &HFBUCK2PSValid_Mask, 0x1E00},
		// {S5A_CTRL       , SPMI_SLAVE_1, &HFBUCK2CTRLValid_Mask, 0x2000},
		// {S5A_FREQ       , SPMI_SLAVE_1, &BCLKGENCLKValid_Mask, 0x2200},
		// {S5A_PS         , SPMI_SLAVE_1, &HFBUCK2PSValid_Mask, 0x2100},
		// {SLEEPA_CLK1    , SPMI_SLAVE_0, &SLPCLKValid_Mask, 0x5A00},
		// {SMBBPA_BAT_IF  , SPMI_SLAVE_0, &SMBBPBATIFValid_Mask, 0x1200},
		// {SMBBPA_BOOST   , SPMI_SLAVE_0, &SMBBPBOOSTValid_Mask, 0x1500},
		// {SMBBPA_BUCK    , SPMI_SLAVE_0, &SMBBPBUCKValid_Mask, 0x1100},
		// {SMBBPA_CHGR    , SPMI_SLAVE_0, &SMBBPCHGRValid_Mask, 0x1000},
		// {SMBBPA_FREQ    , SPMI_SLAVE_0, &BCLKGENCLKValid_Mask, 0x1700},
		// {SMBBPA_MISC    , SPMI_SLAVE_0, &SMBBPMISCValid_Mask, 0x1600},
		// {SMBBPA_USB_CHGPTH  , SPMI_SLAVE_0, &SMBBPUSBValid_Mask, 0x1300},
		// {SPMIA          , SPMI_SLAVE_0, &SPMIPDIGValid_Mask, 0x0600},
		// {SPMIA_OPTIONS  , SPMI_SLAVE_2, &SPMIPDIGOPTIONSValid_Mask, 0x0600},
		// {TEMPA_ALARM    , SPMI_SLAVE_0, &TEMPALARMValid_Mask, 0x2400},
		// {TRIMA          , SPMI_SLAVE_0, &TRIMValid_Mask, 0xFE00},
		// {VADC1A_USR     , SPMI_SLAVE_0, &VADCValid_Mask, 0x3100},
		// {VADC2A_BTM     , SPMI_SLAVE_0, &VADCBTMValid_Mask, 0x3400},
		// {VADC3A_BMS     , SPMI_SLAVE_0, &VADCADJValid_Mask, 0x3300},
		// {VADC3A_MDM     , SPMI_SLAVE_0, &VADCADJValid_Mask, 0x3200},
		// {VIB1A          , SPMI_SLAVE_1, &VIBDRVDIGValid_Mask, 0xC000},
		// {VREFLPDDRA     , SPMI_SLAVE_0, &VREFLPDDRValid_Mask, 0x0A00},
		// {WLED1A         , SPMI_SLAVE_1, &WLEDValid_Mask, 0xD800},
		// {XOA            , SPMI_SLAVE_0, &XOCOREValid_Mask, 0x5000},
		
	}
};
