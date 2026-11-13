// Stabilizer-specific signals and exports.
export module pwUDM.Stabilizer;

import std;
import pwUDM;

export namespace pwUDM {

// Stabilizer Hard-Coded Input Signal Indices
// Note: These are the SAME for all stabilizers and are hard-coded values that Simulator ALWAYS passes to the DLL
// If additional inputs are needed from Simulator, then you MUST define them in "Algebraics"
inline constexpr int HARDCODE_STAB_GenSpeedDeviationPU = 0;
inline constexpr int HARDCODE_STAB_BusFreqDeviationPU = 1;
inline constexpr int HARDCODE_STAB_GenPElecPU = 2;
inline constexpr int HARDCODE_STAB_GenPAccelPU = 3;
inline constexpr int HARDCODE_STAB_BusVoltMagPU = 4;
inline constexpr int HARDCODE_STAB_GenVcomp = 5;

// These functions need to be exported in the DLL
extern "C" __declspec(dllexport) double __stdcall StabilizerVsOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) double __stdcall StabilizerPitchOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);

}
