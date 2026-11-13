// Exciter-specific signals and exports.
export module pwUDM.Exciter;

import std;
import pwUDM;

export namespace pwUDM {

// Exciter Hard-Coded Input Signal Indices
// Note: These are the SAME for all exciters and are hard-coded values that Simulator ALWAYS passes to the DLL.
// If additional inputs are needed from Simulator, then you must define them in "Algebraics"
inline constexpr int HARDCODE_EXCITER_Vref = 0;
inline constexpr int HARDCODE_EXCITER_InitFieldVoltage = 1;
inline constexpr int HARDCODE_EXCITER_InitFieldCurrent = 2;
inline constexpr int HARDCODE_EXCITER_GenVComp = 3;
inline constexpr int HARDCODE_EXCITER_GenSpeedDeviationPU = 4;
inline constexpr int HARDCODE_EXCITER_BusVoltMagPU = 5;
inline constexpr int HARDCODE_EXCITER_StabilizerSignal = 6;
inline constexpr int HARDCODE_EXCITER_OELActive = 7;
inline constexpr int HARDCODE_EXCITER_OELSignal = 8;
inline constexpr int HARDCODE_EXCITER_UELActive = 9;
inline constexpr int HARDCODE_EXCITER_UELSignal = 10;

// These functions need to be exported in the DLL
extern "C" __declspec(dllexport) double __stdcall ExciterEfieldOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);

}
