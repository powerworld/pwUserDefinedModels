unit pwUDM.Governor;
// Governor-specific signals.

interface

uses
  pwUDM.Core;

const
  // Governor Hard-Coded Input Signal Indices
  // Note: These are the SAME for all governors and are hard-coded values that
  // Simulator ALWAYS passes to the DLL
  // If additional inputs are needed from Simulator, then you MUST define them
  // using the "Algebraics"
  HARDCODE_GOV_Pref                = 0;
  HARDCODE_GOV_InitPmech           = 1;
  HARDCODE_GOV_GenSpeedDeviationPU = 2;
  HARDCODE_GOV_GenPElecPU          = 3;
  HARDCODE_GOV_GenMVABase          = 4;
  HARDCODE_GOV_GovResponseLimits   = 5;
  HARDCODE_GOV_StabStatePitch      = 6;

  // These functions need to be exported in the DLL (add them to the exports clause of the model unit)
  //   GovernorPmechOut

//------------------------------------------------------------------------------
// The DLL contract: the signature of each routine a model exports. Each model unit checks
// The signatures of the Governor-specific routines a model exports. The model unit checks its routines
// against these types at compile time (see the end of its implementation section).
type
  TGovernorPmechOut = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double; stdcall;

implementation

end.
