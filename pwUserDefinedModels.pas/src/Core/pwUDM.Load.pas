unit pwUDM.Load;
// Load-specific signals.

interface

uses
  pwUDM.Core;

const
  // Load Hard-Coded Input Signal Indices
  // Note: These are the SAME for all loads and are hard-coded values that
  // Simulator ALWAYS passes to the DLL
  // If additional inputs are needed from Simulator, then you MUST define them
  // using the "Algebraics" signals and include them at the BEGINNING of the ALG vector
  HARDCODE_LOAD_DeviceVPU      =  0;
  HARDCODE_LOAD_DeviceAngleRad =  1;
  HARDCODE_LOAD_DeltaFreqPU    =  2;
  HARDCODE_LOAD_DeviceStatus   =  3;
  HARDCODE_LOAD_LoadScalar     =  4;

  // These functions need to be exported in the DLL (add them to the exports clause of the model unit)
  //   LoadNortonAdmittance
  //   LoadNortonCurrent
  //   LoadNortonCurrentAlgebraicDerivative
  //   LoadInitializeAlgebraic

//------------------------------------------------------------------------------
// The DLL contract: the signature of each routine a model exports. Each model unit checks
// The signatures of the Load-specific routines a model exports. The model unit checks its routines
// against these types at compile time (see the end of its implementation section).
type
  TLoadNortonAdmittance = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; theG : PDouble; theB : PDouble); stdcall;
  TLoadNortonCurrent = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; IReal : PDouble; IImag : PDouble); stdcall;
  TLoadNortonCurrentAlgebraicDerivative = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; IReal_dVreal : PDouble; IReal_dVimag : PDouble; IImag_dVreal : PDouble; IImag_dVimag : PDouble); stdcall;
  // InitLoadP and InitLoadQ will most frequently
  // be equal to the SteadyStateP and SteadyStateQ
  // Any difference will be automatically made-up for within
  // Simulator by embedding an impedance with the load that makes the
  // NetP and NetQ match SteadyStateP and SteadyStateQ initially
  TLoadInitializeAlgebraic = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; INPUT_PUTol, SteadyStateP, SteadyStateQ, SteadyStateV : Double; InitLoadP, InitLoadQ : PDouble) : boolean; stdcall;

implementation

end.
