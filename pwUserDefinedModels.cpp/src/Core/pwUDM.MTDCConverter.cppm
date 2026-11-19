export module pwUDM.MTDCConverter;
// MTDC converter-specific signals.

import pwUDM.Core;

export namespace pwUDM {

// MTDC Converter Hard-Coded Input Signal Indices
// Note: These are the SAME for all MTDC converters and are hard-coded values that
// Simulator ALWAYS passes to the DLL
// If additional inputs are needed from Simulator, then you MUST define them
// using the "Algebraics"
inline constexpr int HARDCODE_MTDCCONV_CurrentReference       = 0;  // output from initialization, input afterward (Amps)
inline constexpr int HARDCODE_MTDCCONV_CurrentOrderUnassigned = 1;  // input for initialization: pole current order not yet given to a converter
inline constexpr int HARDCODE_MTDCCONV_CosControlAngle        = 2;  // input for initialization: cos(alpha) at a rectifier, cos(beta) at an inverter
inline constexpr int HARDCODE_MTDCCONV_CurrentDC              = 3;  // DC current (Amps)
inline constexpr int HARDCODE_MTDCCONV_VoltageDC              = 4;  // DC voltage (kV)
inline constexpr int HARDCODE_MTDCCONV_VoltageAC              = 5;  // AC voltage (pu)

// These functions need to be exported in the DLL
extern "C" __declspec(dllexport) double MTDCConverterCosControlAngle(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
// The following four functions are optional: export them from a model only if it needs them
extern "C" __declspec(dllexport) double MTDCConverterIdcSense(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) double MTDCConverterVdcSense(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) double MTDCConverterVacSense(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
// Returns the converter's current limit and current margin; a limit of zero or less means no limit
extern "C" __declspec(dllexport) void MTDCConverterCurrentLimitAndMargin(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* IdRefLim, double* fid_Margin);

}
