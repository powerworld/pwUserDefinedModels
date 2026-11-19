unit pwUDM.GENCLS;
// This implements the existing GENCLS model as an example

interface

uses
  System.SysUtils,
  pwUDM.Core;

const
  // For the Model
  N_F_PARAMS   = 6;  // number of float model params
  N_I_PARAMS   = 0;  // number of integer model params
  N_S_PARAMS   = 0;  // number of string model params
  N_STATES     = 2;  // number of model states
  N_ALGEBRAICS = 0;  // number of model algebraics

  // For the NonWindUpLimits
  N_NONWINDUPLIMITS = 0;  // number of nonwindup

  //---------------------------------------------------------------------------
  // These are just to make the code easier to read
  // Parameter Value indices.
  PARAM_H     = 0;
  PARAM_D     = 1;
  PARAM_Ra    = 2;
  PARAM_Xdp   = 3;
  PARAM_Rcomp = 4;
  PARAM_Xcomp = 5;

  // State Value Indices.
  STATE_Angle = 0;
  STATE_Speed = 1;

  // Algebraic Value Indices
  // None ALG_TSXXX             = 0;

// The routines below are the DLL contract that Simulator calls: each one is exported by the
// exports clause that follows them (the optional ones are commented out unless the model needs them)
function DLLVersion:Integer;
function modelClassName(StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
procedure allParamCounts(numbersOfEverything : PTxParamCounts; TimeStepSeconds : PDouble);
function parameterName(ParamNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
function stateName(StateNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
// function OtherObjectClass(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
// function OtherObjectDescription(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
procedure getDefaultParameterValue(ParamsAndStates : PTxMyModelData);
// function getStringParamDefaultValue(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
// function signalSelection(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
function SubIntervalPower2Exponent(ParamsAndStates : PTxMyModelData; TimeStepSeconds : PDouble) : integer;
procedure initializeYourself(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
procedure calculateFofX(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits; dotX : PDoubleArray);
procedure PropagateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits): Integer;
// function MachineHighVReactiveCurrentLim(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
// procedure MachineLowVActiveCurrentPoints(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Lvpnt1 : PDouble; Lvpnt0 : PDouble);
function MachineSpeedDeviationOut(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
procedure MachineTheveninImpedance(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; theR : PDouble; theX : PDouble);
procedure MachineTheveninVoltage(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Delta : PDouble; Vd    : PDouble; Vq    : PDouble);
// procedure MachineNortonCurrent(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; IReal : PDouble; IImag : PDouble);
function MachineFieldCurrent(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
function MachineElectricalTorque(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
// procedure MachineCompensatingImpedance(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Rcomp : PDouble; Xcomp : PDouble);

exports DLLVersion
        , modelClassName
        , allParamCounts
        , parameterName
        , stateName
     // , OtherObjectClass
     // , OtherObjectDescription
        , getDefaultParameterValue
     // , getStringParamDefaultValue
     // , signalSelection

        , SubIntervalPower2Exponent
        , initializeYourself
        , calculateFofX
        , PropagateIgnoredStateAndInput
        , getNonWindUpLimits

     // , MachineHighVReactiveCurrentLim
     // , MachineLowVActiveCurrentPoints
        , MachineSpeedDeviationOut
        , MachineTheveninImpedance
        , MachineTheveninVoltage
     // , MachineNortonCurrent
        , MachineFieldCurrent
        , MachineElectricalTorque
     // , MachineCompensatingImpedance
        ;

implementation

uses
  pwUDM.Machine;



//******************************************************************************
function DLLVersion:Integer;
begin
  result := 1;
end;

//******************************************************************************
function modelClassName(StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
begin
  result := pwProvideString(StrBuf, StrSize, pwModelClassString(mcMachine));
end;

//******************************************************************************
procedure allParamCounts(numbersOfEverything : PTxParamCounts; TimeStepSeconds : PDouble);
begin
  with numbersOfEverything^ do begin
    // The model-specific parameters
    nFloatParams := N_F_PARAMS;
    nIntParams   := N_I_PARAMS;
    nStrParams   := N_S_PARAMS;
    nStates      := N_STATES;
    nAlgebraics  := N_ALGEBRAICS;
    // For nonwindup limits of the model
    nNonWindUpLimits := N_NONWINDUPLIMITS;
  end;
end;

//******************************************************************************
function parameterName(ParamNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case ParamNum^ of
    PARAM_H     : ts := 'H';
    PARAM_D     : ts := 'D';
    PARAM_Ra    : ts := 'Ra';
    PARAM_Xdp   : ts := 'Xdp';
    PARAM_Rcomp : ts := 'Rcomp';
    PARAM_Xcomp : ts := 'Xcomp';
    else          ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;

//******************************************************************************
function stateName(StateNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case StateNum^ of
    STATE_Angle : ts := 'Angle';
    STATE_Speed : ts := 'Speed';
   else ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;

//******************************************************************************
{ non specified for this model
function OtherObjectClass(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'Bus';
   else ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
{ non specified for this model
function OtherObjectDescription(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'Signal Bus';
   else ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
procedure getDefaultParameterValue(ParamsAndStates : PTxMyModelData);
var pFloatParamArr: PDoubleArray;
begin
  // Example TGOV1 parameters
  pFloatParamArr := ParamsAndStates^.FloatParams;
  pFloatParamArr^[PARAM_H    ] := 3.0;
  pFloatParamArr^[PARAM_D    ] := 0.0;
  pFloatParamArr^[PARAM_Ra   ] := 0.0;
  pFloatParamArr^[PARAM_Xdp  ] := 0.2;
  pFloatParamArr^[PARAM_Rcomp] := 0.0;
  pFloatParamArr^[PARAM_Xcomp] := 0.0;
end;

{ non specified for this model
function getStringParamDefaultValue(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'DefaultString0';
   else ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;
}

{ non specified for this model
//******************************************************************************
function signalSelection(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
var ts : PChar;
begin
  ts := ''; // there are none!
  case Num^ of
    ALG_XXX         : ts := 'TSXXX';
   else ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
function SubIntervalPower2Exponent(ParamsAndStates : PTxMyModelData; TimeStepSeconds : PDouble) : integer;
begin
  // Return an integer value between 0 and 7
  // This function returns the EXPONENT of 2 for the number subintervals.
  // Thus, if you want 8 subintervals, then this should return 3 because 2^3 = 8
  // The maximum number of subintervals allowed is 128, so (2^7) = 128
  result := 0;
end;

//******************************************************************************
procedure initializeYourself(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
  {****************************************************************************************************************}
  { Four quadrant arctangent function.  Returns values between -Pi and Pi }
  // from MathUnit
  Function LOCAL_ArcTn2(y, x : double ) : double;
  Begin
    If x <> 0.0 then Result := arctan (y/x)
    else
      if y < 0.0 then Result := -pi / 2.0
      else Result := pi/2.0;
    If (x < 0) and (Result <= 0) Then Result := Result + pi       // KJP 2/5/01 - Added equality as well, since if x < 0 and y = 0, you always get -pi,
    Else If (x < 0) and (Result > 0) Then Result := Result - pi;  // which conflicts with other results when y <> 0
  End;       { Function ArcTn2 }

var local_Vd,local_Vq : Double;  // Vdq voltage used locally
    local_PsiD,local_PsiQ : Double;  // Initial psiD and psiQ values
    Pmech_In, Efd_In,
    fRa, fXdp,

    // dq stator currents
    fStateId,fStateIq,

    local_Delta,
    local_Itr, local_Iti,
    local_Vtr, local_Vti,
    local_Vintr, local_Vinti : double;
    cosDelta, sinDelta : double;

    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  fRa   := pFloatParamArr^[PARAM_Ra ];
  fXdp  := pFloatParamArr^[PARAM_Xdp];

  local_Vtr := pHardCodedArr^[HARDCODE_MACHINE_InitVreal];
  local_Vti := pHardCodedArr^[HARDCODE_MACHINE_InitVimag];
  local_Itr := pHardCodedArr^[HARDCODE_MACHINE_InitIreal];
  local_Iti := pHardCodedArr^[HARDCODE_MACHINE_InitIimag];

  //--------------------------------------------------------------------
  // Calculate the Internal Voltage
  //--------------------------------------------------------------------
  //                       = Itr + jIti
  //                    Iterm-->
  //   /------Ra+jXdp-----------             Vint = Vterm + Iterm*(Ra+jXdp)
  //   |+                 +                       = Vtr+jVti + (Itr+jIti)*(Ra+jXdp)
  //  Vint              Vterm = Vtr+jVti          = (Vtr + Itr*Ra - Iti*Xdp) + j(Vti + Iti*Ra + Itr*Xdp)
  //   |-                 -                       = Vintr + jVinti
  //  Ground
  //
  local_Vintr := local_Vtr + local_Itr*fRA - local_Iti*fXdp;
  local_Vinti := local_Vti + local_Iti*fRA + local_Itr*fXdp;
  // Get Angle of Internal Voltage
  local_Delta := LOCAL_ArcTn2(local_Vinti,local_Vintr);

  //--------------------------------------------------------------------
  // Convert terminal Current and Voltage to the "DQ reference frame"
  // Simular the Figure 3.4 from Sauer/Pai book
  //--------------------------------------------------------------------
  cosDelta := cos(local_Delta);
  sinDelta := sin(local_Delta);
  fStateId := -cosDelta*local_Iti + sinDelta*local_Itr;
  fStateIq :=  cosDelta*local_Itr + sinDelta*local_Iti;
  local_Vd := -cosDelta*local_Vti + sinDelta*local_Vtr;
  local_Vq :=  cosDelta*local_Vtr + sinDelta*local_Vti;

  //--------------------------------------------------------------------
  // Perform calculations
  //--------------------------------------------------------------------
  // fpsiD and fpsiQ are found using 3.215 and 3.216 in Sauer/Pai book
  local_PsiD :=  (local_Vq + fRa*fStateIq);
  local_PsiQ := -(local_Vd + fRa*fStateId);

  // These initial values of the inputs should actually be SET here
  Efd_In := local_PsiD + fStateId*fXdp;
  Pmech_In :=(local_PsiD * fStateIq - local_PsiQ * fStateId);

  //--------------------------------------------------------------------
  // Write out initial state vector
  //--------------------------------------------------------------------
  pStatesArr^[STATE_Angle] := local_delta;
  pStatesArr^[STATE_Speed] := 0;

  //--------------------------------------------------------------------
  // Pass back initial inputs
  //      exciter input Efd
  //      governor input Pmech
  //--------------------------------------------------------------------
  pHardCodedArr^[HARDCODE_MACHINE_TSGenFieldV] := Efd_In;
  pHardCodedArr^[HARDCODE_MACHINE_TSPmech] := Pmech_In;

  pHardCodedArr^[HARDCODE_MACHINE_TSstateId] := fStateId;
  pHardCodedArr^[HARDCODE_MACHINE_TSstateIq] := fStateIq;
end;

//******************************************************************************
procedure calculateFofX(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits; dotX : PDoubleArray);
var // First there are the parameters,
    fH , fInv2H, fD : Double;
    stateSpeed : Double; // state at the beginning of the time is passed in

    // dq stator currents always referred to gen side of any off-nominal step-up turns ratio
    fStateId, fStateIq   : Double;

    StateEInternalD, StateEInternalQ, StateTElec : Double;
    SystemOmegaBase_In, ActualPMech_In, fInputFromExciter : Double;

    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pDotXArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pDotXArr := dotX;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  stateSpeed := pStatesArr^[STATE_Speed];

  fH  := pFloatParamArr^[PARAM_H];
  fD := pFloatParamArr^[PARAM_D];
  fInv2H := 1/(2*fH);

  SystemOmegaBase_In   := SystemOptions^.WBase;
  ActualPMech_In       := pHardCodedArr^[HARDCODE_MACHINE_TSPmech    ];
  fInputFromExciter := pHardCodedArr^[HARDCODE_MACHINE_TSGenFieldV];
  fStateId          := pHardCodedArr^[HARDCODE_MACHINE_TSstateId  ];
  fStateIq          := pHardCodedArr^[HARDCODE_MACHINE_TSstateIq  ];

  //--------------------------------------------------------------------
  // Perform Calculations necessary
  //--------------------------------------------------------------------
  // StateEInternalD is always zero for gencls
  StateEInternalD := 0;
  StateEInternalQ := fInputFromExciter;
  StateTElec := StateEInternalD * fStateId + StateEInternalQ * fStateIq;

  // ??? kate 1/5/12 figure out support for subintervals after basic stuff works!
  // If SubInterval2PowerSpecified > 0 Then UpdateStateCurrent(GenObject.DeviceBusVoltage);  // Sets the state current values for the generators

  //--------------------------------------------------------------------
  // Push the resulting derivatives to the appropriate memory
  //--------------------------------------------------------------------
  pDotXArr^[STATE_Angle] := stateSpeed * SystemOmegaBase_In;
  pDotXArr^[STATE_Speed] := fInv2H*((ActualPMech_In - fD*stateSpeed)/(1+stateSpeed)  - StateTElec);
end;

//******************************************************************************
//******************************************************************************
procedure PropagateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
Begin
  // no ignored states or handling of algebraics here
end;

//******************************************************************************
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits): Integer;
begin
  Result := 0;
end;

{ non specified for this model
//******************************************************************************
function MachineHighVReactiveCurrentLim(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var pFloatParamArr: PDoubleArray;
Begin
  pFloatParamArr := ParamsAndStates.FloatParams;
  result := pFloatParamArr^[PARAM_VHighLimit];
end;
}

{ non specified for this model
//******************************************************************************
procedure MachineLowVActiveCurrentPoints(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Lvpnt1 : PDouble; Lvpnt0 : PDouble);
var pFloatParamArr: PDoubleArray;
Begin
  pFloatParamArr := ParamsAndStates.FloatParams;
  Lvpnt1^ := pFloatParamArr^[PARAM_Lvpnt1];
  Lvpnt0^ := pFloatParamArr^[PARAM_Lvpnt0];
end;
}

//******************************************************************************
function MachineSpeedDeviationOut(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var pStatesArr : PDoubleArray;
begin
  pStatesArr := ParamsAndStates.States;
  result := pStatesArr^[STATE_Speed];
end;

//******************************************************************************
procedure MachineTheveninImpedance(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; theR : PDouble; theX : PDouble);
var pFloatParamArr: PDoubleArray;
Begin
  pFloatParamArr := ParamsAndStates.FloatParams;
  theR^ := pFloatParamArr^[PARAM_Ra ];
  theX^ := pFloatParamArr^[PARAM_Xdp];
end;

// Either MachineNortonCurrent is required,
// or MachineTheveninVoltage and MachineTheveninImpedance are required.
// This model uses the Thevenin pair. To use MachineNortonCurrent instead, define it
// (declared in pwUDM.Machine) and remove the two Thevenin functions below.

//******************************************************************************
procedure MachineTheveninVoltage(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Delta : PDouble; Vd    : PDouble; Vq    : PDouble);
var pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
Begin
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  Delta^ := pStatesArr^[STATE_Angle];
  Vd^ := 0; // ALWAYS zero for a Classical Machine
  Vq^ := pHardCodedArr^[HARDCODE_MACHINE_TSGenFieldV];
end;

{ non specified for this model
procedure MachineNortonCurrent(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; IReal : PDouble; IImag : PDouble);
begin
end;
}


function MachineFieldCurrent(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var pHardCodedArr : PDoubleArray;
begin
  // Field Current and Voltage are the same for this model
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  result := pHardCodedArr^[HARDCODE_MACHINE_TSGenFieldV];
end;

function MachineElectricalTorque(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var pHardCodedArr : PDoubleArray;
    fVd, fVq, fStateId, fStateIq : double;
begin
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  fVd      := 0; // ALWAYS zero for a Classical Machine
  fVq      := pHardCodedArr^[HARDCODE_MACHINE_TSGenFieldV];
  fStateId := pHardCodedArr^[HARDCODE_MACHINE_TSstateId  ];
  fStateIq := pHardCodedArr^[HARDCODE_MACHINE_TSstateIq  ];
  Result := fVd*fStateId + fVq * fStateIq;
end;

{ non specified for this model
procedure MachineCompensatingImpedance(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Rcomp : PDouble; Xcomp : PDouble);
var pFloatParamArr: PDoubleArray;
Begin
  pFloatParamArr := ParamsAndStates.FloatParams;
  RComp^ := pFloatParamArr^[PARAM_RComp];
  XComp^ := pFloatParamArr^[PARAM_XComp];
end;
}

// Optional exports not implemented by this model (declared in pwUDM.Machine):
// MachineHighVReactiveCurrentLim, MachineLowVActiveCurrentPoints, MachineCompensatingImpedance.
// Define any of them here to provide that behavior.

//******************************************************************************
// Compile-time check: each exported routine must have the signature of the DLL contract
// (pwUDM.Core and the model class unit), otherwise this unit does not compile
const
  Check_DLLVersion : TDLLVersion = DLLVersion;
  Check_modelClassName : TModelClassName = modelClassName;
  Check_allParamCounts : TAllParamCounts = allParamCounts;
  Check_parameterName : TParameterName = parameterName;
  Check_stateName : TStateName = stateName;
//  Check_OtherObjectClass : TOtherObjectClass = OtherObjectClass;
//  Check_OtherObjectDescription : TOtherObjectDescription = OtherObjectDescription;
  Check_getDefaultParameterValue : TGetDefaultParameterValue = getDefaultParameterValue;
//  Check_getStringParamDefaultValue : TGetStringParamDefaultValue = getStringParamDefaultValue;
//  Check_signalSelection : TSignalSelection = signalSelection;
  Check_SubIntervalPower2Exponent : TSubIntervalPower2Exponent = SubIntervalPower2Exponent;
  Check_initializeYourself : TInitializeYourself = initializeYourself;
  Check_calculateFofX : TCalculateFofX = calculateFofX;
  Check_PropagateIgnoredStateAndInput : TPropagateIgnoredStateAndInput = PropagateIgnoredStateAndInput;
  Check_getNonWindUpLimits : TGetNonWindUpLimits = getNonWindUpLimits;
//  Check_MachineHighVReactiveCurrentLim : TMachineHighVReactiveCurrentLim = MachineHighVReactiveCurrentLim;
//  Check_MachineLowVActiveCurrentPoints : TMachineLowVActiveCurrentPoints = MachineLowVActiveCurrentPoints;
  Check_MachineSpeedDeviationOut : TMachineSpeedDeviationOut = MachineSpeedDeviationOut;
  Check_MachineTheveninImpedance : TMachineTheveninImpedance = MachineTheveninImpedance;
  Check_MachineTheveninVoltage : TMachineTheveninVoltage = MachineTheveninVoltage;
//  Check_MachineNortonCurrent : TMachineNortonCurrent = MachineNortonCurrent;
  Check_MachineFieldCurrent : TMachineFieldCurrent = MachineFieldCurrent;
  Check_MachineElectricalTorque : TMachineElectricalTorque = MachineElectricalTorque;
//  Check_MachineCompensatingImpedance : TMachineCompensatingImpedance = MachineCompensatingImpedance;

end.
