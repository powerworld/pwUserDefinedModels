export module pwUDM.MultiTerminalDC;
// Multi-terminal DC-specific signals.

import pwUDM.Core;

export namespace pwUDM {

// Multi-Terminal DC Hard-Coded Input Signal Indices
// Note: These are the SAME for all multi-terminal DC models and are hard-coded values that
// Simulator ALWAYS passes to the DLL
// There are HARDCODE_MTDC_SignalsPerConverter signals for each converter, one converter after the other,
// so signal number "Signal" of converter number "Converter" (counting from 0) is at
//   Converter*HARDCODE_MTDC_SignalsPerConverter + Signal
// The DLL is not told how many converters there are, so the model needs a parameter for that
// If additional inputs are needed from Simulator, then you MUST define them
// using the "Algebraics"
inline constexpr int HARDCODE_MTDC_SignalsPerConverter = 7;

inline constexpr int HARDCODE_MTDC_CurrentReference    = 0;  // the current order of the converter (Amps)
inline constexpr int HARDCODE_MTDC_CurrentDC           = 1;  // DC current of the converter (Amps)
inline constexpr int HARDCODE_MTDC_VoltageDC           = 2;  // DC voltage of the converter (kV)
inline constexpr int HARDCODE_MTDC_VoltageAC           = 3;  // AC voltage of the converter (pu)
inline constexpr int HARDCODE_MTDC_SensedCurrentDC     = 4;  // sensed DC current of the converter (Amps)
inline constexpr int HARDCODE_MTDC_SensedVoltageDC     = 5;  // sensed DC voltage of the converter (kV)
inline constexpr int HARDCODE_MTDC_SensedVoltageAC     = 6;  // sensed AC voltage of the converter (pu)

// These functions need to be exported in the DLL
// Returns the current order of converter number Converter^ (counting from 0)
extern "C" __declspec(dllexport) double MultiTerminalDCGetIDRef(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, int* Converter);

}
