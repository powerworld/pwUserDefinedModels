export module pwUDM.Stabilizer;
// Stabilizer-specific signals.

import pwUDM.Core;

export namespace pwUDM {

// Stabilizer Hard-Coded Input Signal Indices
// Note: These are the SAME for all stabilizers and are hard-coded values that
// Simulator ALWAYS passes to the DLL
// If additional inputs are needed from Simulator, then you MUST define them
// using the "Algebraics"
inline constexpr int HARDCODE_STAB_GenSpeedDeviationPU = 0;
inline constexpr int HARDCODE_STAB_BusFreqDeviationPU  = 1;
inline constexpr int HARDCODE_STAB_GenPElecPU          = 2;
inline constexpr int HARDCODE_STAB_GenPAccelPU         = 3;
inline constexpr int HARDCODE_STAB_BusVoltMagPU        = 4;
inline constexpr int HARDCODE_STAB_GenVcomp            = 5;

// These functions need to be exported in the DLL
extern "C" __declspec(dllexport) double StabilizerVsOut(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
// The following function is optional: export it from a model only if it needs it
extern "C" __declspec(dllexport) double StabilizerPitchOut(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);

}
