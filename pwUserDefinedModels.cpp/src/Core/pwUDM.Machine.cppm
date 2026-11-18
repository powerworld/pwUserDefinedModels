export module pwUDM.Machine;
// Machine-specific signals.

import pwUDM.Core;

export namespace pwUDM {

// Machine Hard-Coded Signal Indices
// Note: These are the SAME for all machines and are hard-coded values that
// Simulator ALWAYS passes to the DLL
// If additional inputs are needed from Simulator, then you MUST define them
// using the "Algebraics"
inline constexpr int HARDCODE_MACHINE_TSGenFieldV = 0;
inline constexpr int HARDCODE_MACHINE_TSPmech     = 1;
inline constexpr int HARDCODE_MACHINE_InitVreal   = 2;
inline constexpr int HARDCODE_MACHINE_InitVimag   = 3;
inline constexpr int HARDCODE_MACHINE_InitIreal   = 4;
inline constexpr int HARDCODE_MACHINE_InitIimag   = 5;
inline constexpr int HARDCODE_MACHINE_TSstateId   = 6;
inline constexpr int HARDCODE_MACHINE_TSstateIq   = 7;

// These functions need to be exported in the DLL
extern "C" __declspec(dllexport) double MachineSpeedDeviationOut(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) void MachineTheveninImpedance(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* theR, double* theX);
extern "C" __declspec(dllexport) void MachineTheveninVoltage(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* Delta, double* Vd, double* Vq);
extern "C" __declspec(dllexport) double MachineFieldCurrent(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) double MachineElectricalTorque(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
// The following four functions are optional: export them from a model only if it needs them
extern "C" __declspec(dllexport) double MachineNortonCurrent(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* IReal, double* IImag);
extern "C" __declspec(dllexport) double MachineHighVReactiveCurrentLim(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) void MachineLowVActiveCurrentPoints(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* LVPnt1, double* LVPnt0);
extern "C" __declspec(dllexport) void MachineCompensatingImpedance(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* RComp, double* XComp);

}
