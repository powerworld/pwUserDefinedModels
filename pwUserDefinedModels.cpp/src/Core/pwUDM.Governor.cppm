// Governor-specific signals and exports.
export module pwUDM.Governor;

import std;
import pwUDM;

export namespace pwUDM {

// Governor Hard-Coded Input Signal Indices
// Note: These are the SAME for all governors and are hard-coded values that Simulator ALWAYS passes to the DLL.
// If additional inputs are needed from Simulator, then you MUST define them using the "Algebraics"
inline constexpr int HARDCODE_GOV_Pref = 0;
inline constexpr int HARDCODE_GOV_InitPmech = 1;
inline constexpr int HARDCODE_GOV_GenSpeedDeviationPU = 2;
inline constexpr int HARDCODE_GOV_GenPElecPU = 3;
inline constexpr int HARDCODE_GOV_GenMVABase = 4;
inline constexpr int HARDCODE_GOV_GovResponseLimits = 5;
inline constexpr int HARDCODE_GOV_StabStatePitch = 6;

// These functions need to be exported in the DLL
extern "C" __declspec(dllexport) double __stdcall GovernorPmechOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);

}
