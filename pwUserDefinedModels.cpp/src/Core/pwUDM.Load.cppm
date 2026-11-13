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
extern "C" __declspec(dllexport) double __stdcall LoadNortonAdmittance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* theG, double* theB);
extern "C" __declspec(dllexport) double __stdcall LoadNortonCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* IReal, double* IImag);
extern "C" __declspec(dllexport) double __stdcall LoadNortonCurrentAlgebraicDerivative(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* dIr_de, double* dIr_df, double* dIi_de, double* dIi_df);
extern "C" __declspec(dllexport) bool __stdcall LoadInitializeAlgebraic(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* thePUTol, double* P, double* Q, double* V, double* UsedP, double* UsedQ);

}
