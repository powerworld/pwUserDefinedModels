unit pwUDM.MTDCConverter;
// MTDC converter-specific signals.

interface

uses
  pwUDM.Core;

const
  // MTDC Converter Hard-Coded Input Signal Indices
  // Note: These are the SAME for all MTDC converters and are hard-coded values that
  // Simulator ALWAYS passes to the DLL
  // If additional inputs are needed from Simulator, then you MUST define them
  // using the "Algebraics"
  HARDCODE_MTDCCONV_CurrentReference       = 0; // output from initialization, input afterward (Amps)
  HARDCODE_MTDCCONV_CurrentOrderUnassigned = 1; // input for initialization: pole current order not yet given to a converter
  HARDCODE_MTDCCONV_CosControlAngle        = 2; // input for initialization: cos(alpha) at a rectifier, cos(beta) at an inverter
  HARDCODE_MTDCCONV_CurrentDC              = 3; // DC current (Amps)
  HARDCODE_MTDCCONV_VoltageDC              = 4; // DC voltage (kV)
  HARDCODE_MTDCCONV_VoltageAC              = 5; // AC voltage (pu)

  // These functions need to be exported in the DLL (add them to the exports clause of the model unit)
  // The ones marked (optional) are commented out in the exports clause unless the model needs them
  //   MTDCConverterCosControlAngle
  //   (optional) MTDCConverterIdcSense
  //   (optional) MTDCConverterVdcSense
  //   (optional) MTDCConverterVacSense
  //   (optional) MTDCConverterCurrentLimitAndMargin

//------------------------------------------------------------------------------
// The signatures of the MTDC converter-specific routines a model exports. The model unit checks its routines
// against these types at compile time (see the end of its implementation section).
type
  TMTDCConverterCosControlAngle = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
  TMTDCConverterIdcSense = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
  TMTDCConverterVdcSense = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
  TMTDCConverterVacSense = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
  // Returns the converter's current limit and current margin; a limit of zero or less means no limit
  TMTDCConverterCurrentLimitAndMargin = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; IdRefLim : PDouble; fid_Margin : PDouble);

implementation

end.
