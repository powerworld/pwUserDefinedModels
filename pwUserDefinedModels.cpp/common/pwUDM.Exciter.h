#ifndef PW_UDM_EXCITER
#define PW_UDM_EXCITER

#include "pwUDM.h"

// Exciter Hard-Coded Input Signal Indices
// Note: These are the SAME for all exciters and are hard-coded values that Simulator ALWAYS passes to the DLL.  
// If additional inputs are needed from Simulator, then you must define them in "Algebraics"
const int HARDCODE_EXCITER_Vref					= 0;
const int HARDCODE_EXCITER_InitFieldVoltage		= 1;
const int HARDCODE_EXCITER_InitFieldCurrent		= 2;
const int HARDCODE_EXCITER_GenVComp				= 3;
const int HARDCODE_EXCITER_GenSpeedDeviationPU	= 4;
const int HARDCODE_EXCITER_BusVoltMagPU			= 5;
const int HARDCODE_EXCITER_StabilizerSignal		= 6;
const int HARDCODE_EXCITER_OELActive			= 7;
const int HARDCODE_EXCITER_OELSignal			= 8;
const int HARDCODE_EXCITER_UELActive			= 9;
const int HARDCODE_EXCITER_UELSignal			= 10;

// These functions need to be exported in the DLL
PW_UDM_DllExport(double)	ExciterEfieldOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);

#endif // PW_UDM_EXCITER