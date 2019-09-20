library pwUDM.GENCLS.pas;
// This implements the existing GENCLS model as an example

uses
  pwUDM in '..\common\pwUDM.pas',
  SysUtils;

const
  // For the Model
  N_F_PARAMS   =  6;   // number of float model params
  N_I_PARAMS   =  0;   // number of integer model params
  N_S_PARAMS   =  0;   // number of string model params
  N_STATES     =  2;   // number of model states
  N_ALGEBRAICS =  0;   // number of model algebraics

  // For the NonWindUpLimits
  N_NONWINDUPLIMITS = 0;  // number of nonwindup

  // ---------------------------------------------------------------------------
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

  // Stabilizer Hard-Coded Signal Indices
  // Note: These are the SAME for all machines and are hard-cded values that
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

{$R *.res}

//******************************************************************************
// This functions handles the exchange of string length and copying the string
function LOCAL_StringProcess(StrBuf: PChar; StrSize: PInteger; SourceString: PChar): Integer;
begin
  result := length(SourceString);
  if (result > 0) and (StrSize^ >= Result) then strcopy(StrBuf, SourceString);
end;

//******************************************************************************
function DLLVersion:Integer; stdcall;
begin
  result := 1;
end;

//******************************************************************************
function modelClassName(StrSize: PInteger;
                        StrBuf: PChar;
                        dummy: Integer): Integer; stdcall;
var ts : PChar;
begin
  // Choose one of the following
	ts := 'UserDefinedMachineModel';
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;

//******************************************************************************
procedure allParamCounts(numbersOfEverything : PTxParamCounts; TimeStepSeconds : PDouble); stdcall;
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
function parameterName(ParamNum: PInteger;
                       StrSize: PInteger;
                       StrBuf: PChar;
                       dummy : Integer): Integer; stdcall;
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
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;

//******************************************************************************
function stateName(StateNum: PInteger;
                   StrSize: PInteger;
                   StrBuf: PChar;
                   dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  case StateNum^ of
    STATE_Angle : ts := 'Angle';
    STATE_Speed : ts := 'Speed';
   else ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;

//******************************************************************************
{ non specified for this model
function OtherObjectClass(Num: PInteger;
                          StrSize: PInteger;
                          StrBuf: PChar;
                          dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'Bus';
   else ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
{ non specified for this model
function OtherObjectDescription(Num: PInteger;
                                StrSize: PInteger;
                                StrBuf: PChar;
                                dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'Signal Bus';
   else ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
procedure getDefaultParameterValue(ParamsAndStates : PTxMyModelData); stdcall;
var pDoubArr: PDoubleArray;
begin
  // Example TGOV1 parameters
  pDoubArr := PDoubleArray(ParamsAndStates^.FloatParams);
  pDoubArr^[PARAM_H    ] := 3.0;
  pDoubArr^[PARAM_D    ] := 0.0;
  pDoubArr^[PARAM_Ra   ] := 0.0;
  pDoubArr^[PARAM_Xdp  ] := 0.2;
  pDoubArr^[PARAM_Rcomp] := 0.0;
  pDoubArr^[PARAM_Xcomp] := 0.0;
end;

{ non specified for this model
function getStringParamDefaultValue(Num: PInteger;
                                    StrSize: PInteger;
                                    StrBuf: PChar;
                                    dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'DefaultString0';
   else ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;
}

{ non specified for this model
//******************************************************************************
function signalSelection(Num: PInteger;
                         StrSize: PInteger;
                         StrBuf: PChar;
                         dummy: Integer): Integer; stdcall;
var ts : PChar;
begin
  ts := ''; // there are none!
  case Num^ of
    ALG_XXX         : ts := 'TSXXX';
   else ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
function SubIntervalPower2Exponent(ParamsAndStates : PTxMyModelData; TimeStepSeconds : PDouble) : integer; stdcall;
begin
  // Return an integer value between 0 and 7
  // This function returns the EXPONENT of 2 for the number subintervals.
  // Thus, if you want 8 subintervals, then this should return 3 because 2^3 = 8
  // The maximum number of subintervals allowed is 128, so (2^7) = 128
  result := 0;
end;

//******************************************************************************
procedure initializeYourself(ParamsAndStates : PTxMyModelData;
                             SystemOptions : PTxSystemOptions); stdcall;
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
    CosDelta, SinDelta : double;

    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := PDoubleArray(ParamsAndStates.FloatParams);
  pStatesArr := PDoubleArray(ParamsAndStates.States);
  pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);

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
  CosDelta := cos(local_Delta);
  SinDelta := sin(local_Delta);
  fStateId := -CosDelta*local_Iti + SinDelta*local_Itr;
  fStateIq :=  CosDelta*local_Itr + SinDelta*local_Iti;
  local_Vd := -CosDelta*local_Vti + SinDelta*local_Vtr;
  local_Vq :=  CosDelta*local_Vtr + SinDelta*local_Vti;

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
procedure calculateFofX(ParamsAndStates : PTxMyModelData;
                        SystemOptions : PTxSystemOptions;
                        nonWindUpLimits : PTxNonWindUpLimits;
                        dotX : PDouble
                        ); stdcall;
var // First there are the parameters,
    H , fInv2H, fD : Double;
    speedState : Double; // state at the beginning of the time is passed in

    // dq stator currents always referred to gen side of any off-nominal step-up turns ratio
    fStateId, fStateIq   : Double;

    StateEInternalD, StateEInternalQ, StateTElec : Double;
    SystemOmegaBase, ActualPMech, fInputFromExciter : Double;

    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pDotXArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := PDoubleArray(ParamsAndStates.FloatParams);
  pStatesArr := PDoubleArray(ParamsAndStates.States);
  pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);
  pDotXArr := PDoubleArray(dotX);

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  speedState := pStatesArr^[STATE_Speed];

  H  := pFloatParamArr^[PARAM_H];
  fD := pFloatParamArr^[PARAM_D];
  fInv2H := 1/(2*H);

  SystemOmegaBase   := SystemOptions^.WBase;
  ActualPMech       := pHardCodedArr^[HARDCODE_MACHINE_TSPmech    ];
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
  pDotXArr^[STATE_Angle] := speedState * SystemOmegaBase;
  pDotXArr^[STATE_Speed] := fInv2H*((ActualPMech - fD*speedState)/(1+speedState)  - StateTElec);
end;

//******************************************************************************
//******************************************************************************
procedure PropagateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData;
                                        SystemOptions : PTxSystemOptions); stdcall;
Begin
  // no ignored states or handling of algebraics here
end;

//******************************************************************************
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData;
                            SystemOptions : PTxSystemOptions;
                            nonWindUpLimits : PTxNonWindUpLimits): Integer; stdcall;
begin
  Result := 0;
end;

{ non specified for this model
//******************************************************************************
function MachineHighVReactiveCurrentLim(ParamsAndStates : PTxMyModelData;
                                        SystemOptions : PTxSystemOptions) : double; stdcall;
var pFloatParamArr: PDoubleArray;
Begin
  pFloatParamArr := PDoubleArray(ParamsAndStates.FloatParams);
  result := pFloatParamArr^[PARAM_VHighLimit];
end;
}

{ non specified for this model
//******************************************************************************
procedure MachineLowVActiveCurrentPoints(ParamsAndStates : PTxMyModelData;
                                         SystemOptions : PTxSystemOptions;
                                         Lvpnt1 : PDouble;
                                         Lvpnt0 : PDouble
                                        ); stdcall;
var pFloatParamArr: PDoubleArray;
Begin
  pFloatParamArr := PDoubleArray(ParamsAndStates.FloatParams);
  Lvpnt1^ := pFloatParamArr^[PARAM_Lvpnt1];
  Lvpnt0^ := pFloatParamArr^[PARAM_Lvpnt0];
end;
}

//******************************************************************************
function MachineSpeedDeviationOut(ParamsAndStates : PTxMyModelData;
                                  SystemOptions : PTxSystemOptions) : double; stdcall;
var pStatesArr : PDoubleArray;
begin
  pStatesArr := PDoubleArray(ParamsAndStates.States);
  result := pStatesArr^[STATE_Speed];
end;

//******************************************************************************
procedure MachineTheveninImpedance(ParamsAndStates : PTxMyModelData;
                                   SystemOptions : PTxSystemOptions;
                                   theR : PDouble;
                                   theX : PDouble
                                   ); stdcall;
var pFloatParamArr: PDoubleArray;
Begin
  pFloatParamArr := PDoubleArray(ParamsAndStates.FloatParams);
  theR^ := pFloatParamArr^[PARAM_Ra ];
  theX^ := pFloatParamArr^[PARAM_Xdp];
end;

//******************************************************************************
procedure MachineTheveninVoltage(ParamsAndStates : PTxMyModelData;
                                 SystemOptions : PTxSystemOptions;
                                 Delta : PDouble;
                                 Vd    : PDouble;
                                 Vq    : PDouble
                                 ); stdcall;
var pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
Begin
  pStatesArr := PDoubleArray(ParamsAndStates.States);
  pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);
  Delta^ := pStatesArr^[STATE_Angle];
  Vd^ := 0; // ALWAYS zero for a Classical Machine
  Vq^ := pHardCodedArr^[HARDCODE_MACHINE_TSGenFieldV];
end;

{ non specified for this model
procedure MachineNortonCurrent(ParamsAndStates : PTxMyModelData;
                               SystemOptions : PTxSystemOptions;
                               IReal : PDouble;
                               IImag : PDouble
                               ); stdcall;
begin
end;
}


function MachineFieldCurrent(ParamsAndStates : PTxMyModelData;
                             SystemOptions : PTxSystemOptions) : double; stdcall;
var pHardCodedArr : PDoubleArray;
begin
  // Field Current and Voltage are the same for this model
  pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);
  result := pHardCodedArr^[HARDCODE_MACHINE_TSGenFieldV];
end;

function MachineElectricalTorque(ParamsAndStates : PTxMyModelData;
                                 SystemOptions : PTxSystemOptions) : double; stdcall;
var pHardCodedArr : PDoubleArray;
    fVd, fVq, fStateId, fStateIq : double;
begin
  pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);
  fVd      := 0; // ALWAYS zero for a Classical Machine
  fVq      := pHardCodedArr^[HARDCODE_MACHINE_TSGenFieldV];
  fStateId := pHardCodedArr^[HARDCODE_MACHINE_TSstateId  ];
  fStateIq := pHardCodedArr^[HARDCODE_MACHINE_TSstateIq  ];
  Result := fVd*fStateId + fVq * fStateIq;
end;

{ non specified for this model
procedure MachineCompensatingImpedance(ParamsAndStates : PTxMyModelData;
                                       SystemOptions : PTxSystemOptions;
                                       Rcomp : PDouble;
                                       Xcomp : PDouble
                                       ); stdcall;
var pFloatParamArr: PDoubleArray;
Begin
  pFloatParamArr := PDoubleArray(ParamsAndStates.FloatParams);
  RComp^ := pFloatParamArr^[PARAM_RComp];
  XComp^ := pFloatParamArr^[PARAM_XComp];
end;
}

//******************************************************************************
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

begin
//
end.
