unit pwUDM.Exciter;
// Exciter-specific signals.

interface

uses
  pwUDM.Core;

const
  // Exciter Hard-Coded Input Signal Indices
  // Note: These are the SAME for all exciters and are hard-coded values that
  // Simulator ALWAYS passes to the DLL
  // If additional inputs are needed from Simulator, then you MUST define them
  // using the "Algebraics"
  HARDCODE_EXCITER_Vref                = 0;
  HARDCODE_EXCITER_InitFieldVoltage    = 1;
  HARDCODE_EXCITER_FieldCurrent        = 2;
  HARDCODE_EXCITER_GenVcomp            = 3;
  HARDCODE_EXCITER_GenSpeedDeviationPU = 4;
  HARDCODE_EXCITER_BusVoltMagPU        = 5;
  HARDCODE_EXCITER_StabilizerSignal    = 6;
  HARDCODE_EXCITER_OELActive           = 7;
  HARDCODE_EXCITER_OELSignal           = 8;
  HARDCODE_EXCITER_UELActive           = 9;
  HARDCODE_EXCITER_UELSignal           = 10;

  // These functions need to be exported in the DLL (add them to the exports clause of the model unit)
  //   ExciterEfieldOut

//------------------------------------------------------------------------------
// The DLL contract: the signature of each routine a model exports. Each model unit checks
// The signatures of the Exciter-specific routines a model exports. The model unit checks its routines
// against these types at compile time (see the end of its implementation section).
type
  TExciterEfieldOut = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double; stdcall;

implementation

end.
