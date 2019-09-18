#ifndef PW_UDM_STABILIZER
#define PW_UDM_STABILIZER

#include "pwUDM.h"

// Stabilizer Hard-Coded Input Signal Indices
// Note: These are the SAME for all stabilizers and are hard-cded values that Simulator ALWAYS passes to the DLL
// If additional inputs are needed from Simulator, then you MUST define them in "Algebraics"
const	int HARDCODE_STAB_GenSpeedDeviationPU = 0;
const	int HARDCODE_STAB_BusFreqDeviationPU = 1;
const	int HARDCODE_STAB_GenPElecPU = 2;
const	int HARDCODE_STAB_GenPAccelPU = 3;
const	int HARDCODE_STAB_BusVoltMagPU = 4;
const	int HARDCODE_STAB_GenVcomp = 5;

// These functions need to be exported in the DLL
PW_UDM_DllExport(double) StabilizerVsOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
PW_UDM_DllExport(double) StabilizerPitchOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);

#endif // PW_UDM_STABILIZER