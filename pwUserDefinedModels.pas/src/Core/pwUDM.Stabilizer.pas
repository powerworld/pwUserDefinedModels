unit pwUDM.Stabilizer;
// Stabilizer-specific signals.

interface

uses
  pwUDM.Core;

const
  // Stabilizer Hard-Coded Input Signal Indices
  // Note: These are the SAME for all stabilizers and are hard-coded values that
  // Simulator ALWAYS passes to the DLL
  // If additional inputs are needed from Simulator, then you MUST define them
  // using the "Algebraics"
  HARDCODE_STAB_GenSpeedDeviationPU = 0;
  HARDCODE_STAB_BusFreqDeviationPU  = 1;
  HARDCODE_STAB_GenPElecPU          = 2;
  HARDCODE_STAB_GenPAccelPU         = 3;
  HARDCODE_STAB_BusVoltMagPU        = 4;
  HARDCODE_STAB_GenVcomp            = 5;

  // These functions need to be exported in the DLL (add them to the exports clause of the model unit)
  // The ones marked (optional) are commented out in the exports clause unless the model needs them
  //   StabilizerVsOut
  //   (optional) StabilizerPitchOut

//------------------------------------------------------------------------------
// The signatures of the Stabilizer-specific routines a model exports. The model unit checks its routines
// against these types at compile time (see the end of its implementation section).
type
  TStabilizerVsOut = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
  TStabilizerPitchOut = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;

implementation

end.
