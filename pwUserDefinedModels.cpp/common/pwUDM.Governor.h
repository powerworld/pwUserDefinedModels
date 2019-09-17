#ifndef PW_UDM_GOVERNOR
#define PW_UDM_GOVERNOR

#include "pwUDM.h"

// Governor Hard-Coded Input Signal Indices
// Note: These are the SAME for all governors and are hard-cded values that Simulator ALWAYS passes to the DLL.
// If additional inputs are needed from Simulator, then you MUST define them using the "Algebraics"
const	int	HARDCODE_GOV_Pref = 0;
const	int	HARDCODE_GOV_InitPmech = 1;
const	int	HARDCODE_GOV_GenSpeedDeviationPU = 2;
const	int	HARDCODE_GOV_GenPElecPU = 3;
const	int	HARDCODE_GOV_GenMVABase = 4;
const	int	HARDCODE_GOV_GovResponseLimits = 5;
const	int	HARDCODE_GOV_StabStatePitch = 6;

// These functions need to be exported in the DLL
PW_UDM_DllExport(double) GovernorPmechOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);

#endif // PW_UDM_GOVERNOR