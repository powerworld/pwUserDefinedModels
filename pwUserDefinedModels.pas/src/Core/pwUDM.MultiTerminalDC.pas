unit pwUDM.MultiTerminalDC;
// Multi-terminal DC-specific signals.

interface

uses
  pwUDM.Core;

const
  // Multi-Terminal DC Hard-Coded Input Signal Indices
  // Note: These are the SAME for all multi-terminal DC models and are hard-coded values that
  // Simulator ALWAYS passes to the DLL
  // There are HARDCODE_MTDC_SignalsPerConverter signals for each converter, one converter after the other,
  // so signal number "Signal" of converter number "Converter" (counting from 0) is at
  //   Converter*HARDCODE_MTDC_SignalsPerConverter + Signal
  // The DLL is not told how many converters there are, so the model needs a parameter for that
  // If additional inputs are needed from Simulator, then you MUST define them
  // using the "Algebraics"
  HARDCODE_MTDC_SignalsPerConverter = 7;

  HARDCODE_MTDC_CurrentReference = 0;  // the current order of the converter (Amps)
  HARDCODE_MTDC_CurrentDC        = 1;  // DC current of the converter (Amps)
  HARDCODE_MTDC_VoltageDC        = 2;  // DC voltage of the converter (kV)
  HARDCODE_MTDC_VoltageAC        = 3;  // AC voltage of the converter (pu)
  HARDCODE_MTDC_SensedCurrentDC  = 4;  // sensed DC current of the converter (Amps)
  HARDCODE_MTDC_SensedVoltageDC  = 5;  // sensed DC voltage of the converter (kV)
  HARDCODE_MTDC_SensedVoltageAC  = 6;  // sensed AC voltage of the converter (pu)

  // These functions need to be exported in the DLL (add them to the exports clause of the model unit)
  //   MultiTerminalDCGetIDRef

//------------------------------------------------------------------------------
// The signatures of the multi-terminal DC-specific routines a model exports. The model unit checks its routines
// against these types at compile time (see the end of its implementation section).
type
  // Returns the current order of converter number Converter^ (counting from 0)
  TMultiTerminalDCGetIDRef = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Converter : PInteger) : double;

implementation

end.
