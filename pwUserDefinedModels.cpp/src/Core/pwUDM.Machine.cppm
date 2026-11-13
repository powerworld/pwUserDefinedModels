// Machine-specific signals and exports.
export module pwUDM.Machine;

import std;
import pwUDM;

export namespace pwUDM {

// Machine Hard-Coded Signal Indices
// Note: These are the SAME for all machines and are hard-coded values that Simulator ALWAYS passes to the DLL
// If additional inputs are needed from Simulator, then you MUST define them in "Algebraics"
inline constexpr int HARDCODE_MACHINE_TSGenFieldV = 0;
inline constexpr int HARDCODE_MACHINE_TSPmech     = 1;
inline constexpr int HARDCODE_MACHINE_InitVreal   = 2;
inline constexpr int HARDCODE_MACHINE_InitVimag   = 3;
inline constexpr int HARDCODE_MACHINE_InitIreal   = 4;
inline constexpr int HARDCODE_MACHINE_InitIimag   = 5;
inline constexpr int HARDCODE_MACHINE_TSstateId   = 6;
inline constexpr int HARDCODE_MACHINE_TSstateIq   = 7;

// These functions need to be exported in the DLL
extern "C" __declspec(dllexport) double __stdcall MachineSpeedDeviationOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) void __stdcall MachineTheveninImpedance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* theR, double* theX);
extern "C" __declspec(dllexport) void __stdcall MachineTheveninVoltage(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* Delta, double* Vd, double* Vq);
extern "C" __declspec(dllexport) double __stdcall MachineFieldCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) double __stdcall MachineElectricalTorque(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) double __stdcall MachineNortonCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* IReal, double* IImag);
extern "C" __declspec(dllexport) double __stdcall MachineHighVReactiveCurrentLim(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) void __stdcall MachineLowVActiveCurrentPoints(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* LVPnt1, double* LVPnt0);
extern "C" __declspec(dllexport) void __stdcall MachineCompensatingImpedance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* RComp, double* XComp);

}
