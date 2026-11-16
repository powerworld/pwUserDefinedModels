// Load-specific signals and exports.
export module pwUDM.Load;

import std;
import pwUDM;

export namespace pwUDM {

// Load Hard-Coded Input Signal Indices
// Note: These are the SAME for all loads and are hard-coded values that Simulator ALWAYS passes to the DLL.
// If additional inputs are needed from Simulator, then you MUST define them using the "Algebraics"
// signals and include them at the BEGINNING of the ALG vector
inline constexpr int HARDCODE_LOAD_DeviceVPU = 0;
inline constexpr int HARDCODE_LOAD_DeviceAngleRad = 1;
inline constexpr int HARDCODE_LOAD_DeltaFreqPU = 2;
inline constexpr int HARDCODE_LOAD_DeviceStatus = 3;
inline constexpr int HARDCODE_LOAD_LoadScalar = 4;

// These functions need to be exported in the DLL
// The signatures follow the Delphi reference implementation (pwUserDefinedModels.pas): the three Norton
// functions return nothing, and LoadInitializeAlgebraic takes its first four values by value.
extern "C" __declspec(dllexport) void __stdcall LoadNortonAdmittance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* theG, double* theB);
extern "C" __declspec(dllexport) void __stdcall LoadNortonCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* IReal, double* IImag);
extern "C" __declspec(dllexport) void __stdcall LoadNortonCurrentAlgebraicDerivative(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* IReal_dVreal, double* IReal_dVimag, double* IImag_dVreal, double* IImag_dVimag);
extern "C" __declspec(dllexport) bool __stdcall LoadInitializeAlgebraic(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double INPUT_PUTol, double SteadyStateP, double SteadyStateQ, double SteadyStateV, double* InitLoadP, double* InitLoadQ);

}
