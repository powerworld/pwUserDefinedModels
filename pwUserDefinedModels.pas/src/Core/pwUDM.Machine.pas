unit pwUDM.Machine;
// Machine-specific signals.

interface

uses
  pwUDM.Core;

const
  // Machine Hard-Coded Signal Indices
  // Note: These are the SAME for all machines and are hard-coded values that
  // Simulator ALWAYS passes to the DLL
  // If additional inputs are needed from Simulator, then you MUST define them
  // using the "Algebraics"
  HARDCODE_MACHINE_TSGenFieldV = 0;
  HARDCODE_MACHINE_TSPmech     = 1;
  HARDCODE_MACHINE_InitVreal   = 2;
  HARDCODE_MACHINE_InitVimag   = 3;
  HARDCODE_MACHINE_InitIreal   = 4;
  HARDCODE_MACHINE_InitIimag   = 5;
  HARDCODE_MACHINE_TSstateId   = 6;
  HARDCODE_MACHINE_TSstateIq   = 7;

  // These functions need to be exported in the DLL (add them to the exports clause of the model unit)
  // The ones marked (optional) are commented out in the exports clause unless the model needs them
  //   MachineSpeedDeviationOut
  //   MachineTheveninImpedance
  //   MachineTheveninVoltage
  //   MachineFieldCurrent
  //   MachineElectricalTorque
  //   (optional) MachineNortonCurrent
  //   (optional) MachineHighVReactiveCurrentLim
  //   (optional) MachineLowVActiveCurrentPoints
  //   (optional) MachineCompensatingImpedance

//------------------------------------------------------------------------------
// The DLL contract: the signature of each routine a model exports. Each model unit checks
// The signatures of the Machine-specific routines a model exports. The model unit checks its routines
// against these types at compile time (see the end of its implementation section).
type
  TMachineHighVReactiveCurrentLim = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double; stdcall;
  TMachineLowVActiveCurrentPoints = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Lvpnt1 : PDouble; Lvpnt0 : PDouble); stdcall;
  TMachineSpeedDeviationOut = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double; stdcall;
  TMachineTheveninImpedance = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; theR : PDouble; theX : PDouble); stdcall;
  TMachineTheveninVoltage = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Delta : PDouble; Vd    : PDouble; Vq    : PDouble); stdcall;
  TMachineNortonCurrent = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; IReal : PDouble; IImag : PDouble); stdcall;
  TMachineFieldCurrent = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double; stdcall;
  TMachineElectricalTorque = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double; stdcall;
  TMachineCompensatingImpedance = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Rcomp : PDouble; Xcomp : PDouble); stdcall;

implementation

end.
