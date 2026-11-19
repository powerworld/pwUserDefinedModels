unit pwUDM.IEEET1;
// This implements the existing IEEET1 model as an example

interface

uses
  System.SysUtils,
  pwUDM.Core;

const
  //-----------------------------------------------------------------------------
  // For the Model
  N_F_PARAMS  = 15;     // number of float model params
  N_I_PARAMS   = 0;     // number of integer model params
  N_S_PARAMS   = 0;     // number of string model params
  N_STATES     = 4;     // number of model states
  N_ALGEBRAICS = 0;     // number of model algebraics

  // For the NonWindUpLimits
  N_NONWINDUPLIMITS = 1;  // number of nonwindup
  //-----------------------------------------------------------------------------
  // These are just to make the code easier to read
  // Parameter Value indices.
  PARAM_Tr     =  0;
  PARAM_Ka     =  1;
  PARAM_Ta     =  2;
  PARAM_Vrmax  =  3;
  PARAM_Vrmin  =  4;
  PARAM_Ke     =  5;
  PARAM_Te     =  6;
  PARAM_Kf     =  7;
  PARAM_Tf     =  8;
  PARAM_Switch =  9;
  PARAM_E1     = 10;
  PARAM_SE1    = 11;
  PARAM_E2     = 12;
  PARAM_SE2    = 13;
  PARAM_Spdmlt = 14;

  // State Value Indices.
  STATE_EField   = 0;
  STATE_SensedVt = 1;
  STATE_VR       = 2;
  STATE_VF       = 3;

  NONWINDUPINDEX_VR = 0; // index for list of non-windup limited states

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
function ExciterEfieldOut(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;

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

        , ExciterEfieldOut
        ;

implementation

uses
  pwUDM.Exciter;


// Special functions for handling saturation
Procedure LOCAL_PWSolveQuadraticEq (a,b,c : double; Var x1,x2 : double; Var Imag : Boolean);
Var  t : Double;
Begin
  t := Sqr(b) - 4*a*c;
  If t > 0 Then Begin
    imag := False;
    t := Sqrt(t);
    x1 := (-b+t)/(2*a);
    x2 := (-b-t)/(2*a);
   End
  Else Begin
    imag := True;
    x1 := -b / (2*a);
    x2 := Sqrt(-t)/(2*a);
  End;
End;

Procedure LOCAL_SaturationCalculateValues(tempE1,tempS1,tempE2,tempS2 : Double; var fSatA, fSatB : Double);
Var tempA,tempB,tempC,result1,result2 : Double;  // quadratic equation coefficients and results
    isImag : Boolean;  // True if quadratic equation results are imaginary
Begin
  If tempS1 <= 0 Then Begin // Should only occur for zero
    fSatA := tempE1;
    If tempE2=tempE1 Then fSatB := 0  // usual situation is all zeros
    Else fSatB := tempS2/Sqr(tempE2-fSatA);
   End
  Else Begin
    If tempS2 = tempS1 Then Begin
      tempS2 := tempS1*1.001;
    End;
    tempA := tempS2 - tempS1;
    tempB := 2*(-tempS2*tempE1 + tempS1*tempE2);
    tempC := tempS2*Sqr(tempE1) - tempS1*Sqr(tempE2);
    LOCAL_PWSolveQuadraticEq (tempA,tempB,tempC,result1,result2,isImag);
    fSatA := result2;
    fSatB := (tempS1)/Sqr(tempE1-fSatA);
  End;
End;

Function LOCAL_SaturationValue(fSatA, fSatB, input : Double) : Double;
Begin
  If input <= fSatA Then result := 0
  Else Result := fSatB*Sqr(input - fSatA);
End;

Procedure LOCAL_AutoSetVrminVrmax(local_E2,local_Ke, fSatA, fSatB : Double; out local_Vrmin,local_Vrmax : Double);  // TJO 5/28/08 // kate 1/12/12 also pass in fSatA and fSatB
Begin
  If local_Ke <= 0 Then begin
    If local_E2 <= fSatA Then local_Vrmax := 0
    Else local_Vrmax := fSatB*local_E2*Sqr(local_E2-fSatA);
  end
  Else local_Vrmax := (LOCAL_SaturationValue(fSatA, fSatB, local_E2) + local_Ke)*local_E2;
  local_Vrmin := -local_Vrmax;
End;

Function LOCAL_AutoSetKeNew(local_VrMax,local_VrMin,local_Efd, fSatA, fSatB : Double) : Double;
Begin
  If local_VrMin <= 0 Then Result := -LOCAL_saturationValue(fSatA, fSatB, local_Efd)
  Else Result := local_VrMin - LOCAL_saturationValue(fSatA, fSatB, local_Efd);
End;



//******************************************************************************
function DLLVersion:Integer;
begin
  result := 1;
end;

//******************************************************************************
function modelClassName(StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
begin
  result := pwProvideString(StrBuf, StrSize, pwModelClassString(mcExciter));
end;

//******************************************************************************
procedure allParamCounts(numbersOfEverything : PTxParamCounts; TimeStepSeconds : PDouble);
begin
  with numbersOfEverything^ do begin
  // -------The model-specific parameters-----
    nFloatParams := N_F_PARAMS;
    nIntParams   := N_I_PARAMS;
    nStrParams   := N_S_PARAMS;
    nStates      := N_STATES;
    nAlgebraics  := N_ALGEBRAICS;
  // ----For nonwindup limits of the model---
    nNonWindUpLimits := N_NONWINDUPLIMITS;
  end;
end;

//******************************************************************************
function parameterName(ParamNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case ParamNum^ of
    PARAM_Tr     : ts := 'Tr';
    PARAM_Ka     : ts := 'Ka';
    PARAM_Ta     : ts := 'Ta';
    PARAM_Vrmax  : ts := 'Vrmax';
    PARAM_Vrmin  : ts := 'Vrmin';
    PARAM_Ke     : ts := 'Ke';
    PARAM_Te     : ts := 'Te';
    PARAM_Kf     : ts := 'Kf';
    PARAM_Tf     : ts := 'Tf';
    PARAM_Switch : ts := 'Switch';
    PARAM_E1     : ts := 'E1';
    PARAM_SE1    : ts := 'SE(E1)';
    PARAM_E2     : ts := 'E2';
    PARAM_SE2    : ts := 'SE(E2)';
    PARAM_Spdmlt : ts := 'Spdmlt';
   else  ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;

//******************************************************************************
function stateName(StateNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case StateNum^ of
    STATE_EField   : ts := 'EField';
    STATE_SensedVt : ts := 'Sensed Vt';
    STATE_VR       : ts := 'VR';
    STATE_VF       : ts := 'VF';
   else ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;

//******************************************************************************
{ none for this object
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
{ none for this object
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
  pFloatParamArr^[PARAM_Tr    ]  :=  0.0;
  pFloatParamArr^[PARAM_Ka    ]  := 50.0;
  pFloatParamArr^[PARAM_Ta    ]  :=  0.04;
  pFloatParamArr^[PARAM_Vrmax ]  :=  1.0;
  pFloatParamArr^[PARAM_Vrmin ]  := -1.0;
  pFloatParamArr^[PARAM_Ke    ]  := -0.06;
  pFloatParamArr^[PARAM_Te    ]  :=  0.6;
  pFloatParamArr^[PARAM_Kf    ]  :=  0.09;
  pFloatParamArr^[PARAM_Tf    ]  :=  1.46;
  pFloatParamArr^[PARAM_Switch]  :=  0.0;
  pFloatParamArr^[PARAM_E1    ]  :=  2.80;
  pFloatParamArr^[PARAM_SE1   ]  :=  0.04;
  pFloatParamArr^[PARAM_E2    ]  :=  3.73;
  pFloatParamArr^[PARAM_SE2   ]  :=  0.33;
  pFloatParamArr^[PARAM_Spdmlt]  :=  0.0;
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
var // Right now this example is IEEET1 exciter
    // Parameters
    fTr, fKa, fTa, fVrmax, fVrmin, fKe, fKf, fTf, fE1, fSEE1, fE2, fSEE2 : Double;

    fSatA,fSatB : Double;

    local_Vrmax : Double;
    fVRef : Double;

    local_EField : Double;  // Intial field value from generator
    local_VR     : Double;  // VR initial value
    local_VTerminalSensed : Double;  // Sensed terminal voltage
    local_VF : Double;
    fInvTF : Double;

    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  fTr            := pFloatParamArr^[PARAM_Tr    ];
  fKa            := pFloatParamArr^[PARAM_Ka    ];
  fTa            := pFloatParamArr^[PARAM_Ta    ];
  fVrmax         := pFloatParamArr^[PARAM_Vrmax ];
  fVrmin         := pFloatParamArr^[PARAM_Vrmin ];
  fKe            := pFloatParamArr^[PARAM_Ke    ];
  fKf            := pFloatParamArr^[PARAM_Kf    ];
  fTf            := pFloatParamArr^[PARAM_Tf    ];
  fE1            := pFloatParamArr^[PARAM_E1    ];
  fSEE1          := pFloatParamArr^[PARAM_SE1   ];
  fE2            := pFloatParamArr^[PARAM_E2    ];
  fSEE2          := pFloatParamArr^[PARAM_SE2   ];
  if fTf = 0 then fKf := 0;

  local_EField           := pHardCodedArr^[HARDCODE_EXCITER_InitFieldVoltage];
  local_VTerminalSensed  := pHardCodedArr^[HARDCODE_EXCITER_GenVcomp];

  // This function returns fSatA and fSatB constants for a quadratic model.
  LOCAL_SaturationCalculateValues(fE1,fSEE1,fE2,fSEE2, fSatA, fSatB);
  If fVrmax = 0 Then LOCAL_AutoSetVrminVrmax(fE2,fKe,fSatA, fSatB, fVrmin,fVrmax);
  If fKe = 0 Then fKe := LOCAL_AutoSetKeNew(fVrmax,fVrmin,local_Efield, fSatA, fSatB);

  // local_VR     := local_EField*fKE + SaturationValueTimesInput(local_EField);
  // kate 1/12/12 this is effectively doing what SaturationValueTimesInput did.
  If local_EField <= fSatA Then local_Vrmax := 0 // no saturation
  Else local_Vrmax := fSatB*local_EField*Sqr(local_EField-fSatA);
  local_VR  := local_EField*fKE + local_Vrmax;

  fVRef := local_VTerminalSensed + local_VR/fKA;

  // Ignored States
  If fTr = 0 Then pIgnoreStatesArr^[STATE_SensedVt] := True;
  If fTa = 0 Then pIgnoreStatesArr^[STATE_VR] := True;

  // The following parameters should actually get SET, not read in!
  pHardCodedArr^[HARDCODE_EXCITER_Vref] := fVRef;

  //--------------------------------------------------------
  // Model of a Derivative "Washout" block
  //         Ks                     /  -K/T        \
  // OUT = ------ * IN  ==>>  OUT = | ------ + K/T | * IN
  //       [1+Ts]                   \ [1+Ts]       /
  // OUT = (StateInter + K/T) * IN
  //--------------------------------------------------------
  if fTf = 0 then fInvTF := 0 else fInvTF := 1/fTf;
  local_VF := -fKf*fInvTF*local_Efield;
//  local_VF := (fVRef - local_VTerminalSensed) - local_VR/fKa; // kate 3/15/13

//  // Write out initial state vector
  pStatesArr^[STATE_EField  ] := local_EField;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
  pStatesArr^[STATE_SensedVt] := local_VTerminalSensed;
  pStatesArr^[STATE_VR      ] := local_VR;
  pStatesArr^[STATE_VF      ] := local_VF;
end;

//******************************************************************************
procedure calculateFofX(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits; dotX : PDoubleArray);
var // First there are the parameters,
    fTr, fKa, fTa, fVrmax, fVrmin, fKe, fTe, fKf, fTf, fSwitch, fE1, fSEE1, fE2, fSEE2, fSpdmlt: Double;
    fInvTE, fInvTR, fInvTA, fInvTF : Double;

    // The state variables, state at the beginning of the time is passed in
    stateEField, stateSensedVt, stateVR, stateVF : Double;

    // The FofX or xdot,
    dotStateEField, dotStateSensedVt, dotStateVR, dotStateVF : Double;

    fSatA,fSatB : Double;

    tempSatValTimesInput, vInputState : Double;

    GenObjectVTerminalMayBeCompensated_In, VRef_In, StateVsWith_OEL_UEL_In : Double;

    fPIOFeedbackoutput : Double;
    fActiveLimitHLStateVr : Byte;

    pFloatParamArr: PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pStatesArr : PDoubleArray;
    pByteActiveLimitArr : PByteArray;
    pDotXArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr    := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pByteActiveLimitArr := nonWindUpLimits.activeLimits;
  pDotXArr := dotX;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  stateEField   := pStatesArr^[STATE_EField  ];
  stateSensedVt := pStatesArr^[STATE_SensedVt];
  stateVR       := pStatesArr^[STATE_VR      ];
  stateVF       := pStatesArr^[STATE_VF      ];

  fTr            := pFloatParamArr^[PARAM_Tr    ];
  fKa            := pFloatParamArr^[PARAM_Ka    ];
  fTa            := pFloatParamArr^[PARAM_Ta    ];
  fVrmax         := pFloatParamArr^[PARAM_Vrmax ];
  fVrmin         := pFloatParamArr^[PARAM_Vrmin ];
  fKe            := pFloatParamArr^[PARAM_Ke    ];
  fTe            := pFloatParamArr^[PARAM_Te    ];
  fKf            := pFloatParamArr^[PARAM_Kf    ];
  fTf            := pFloatParamArr^[PARAM_Tf    ];
  fSwitch        := pFloatParamArr^[PARAM_Switch];
  fE1            := pFloatParamArr^[PARAM_E1    ];
  fSEE1          := pFloatParamArr^[PARAM_SE1   ];
  fE2            := pFloatParamArr^[PARAM_E2    ];
  fSEE2          := pFloatParamArr^[PARAM_SE2   ];
  fSpdmlt        := pFloatParamArr^[PARAM_Spdmlt];

  VRef_In := pHardCodedArr^[HARDCODE_EXCITER_Vref];

  GenObjectVTerminalMayBeCompensated_In := pHardCodedArr^[HARDCODE_EXCITER_GenVcomp];
  StateVsWith_OEL_UEL_In := pHardCodedArr^[HARDCODE_EXCITER_StabilizerSignal];
  if pHardCodedArr^[HARDCODE_EXCITER_OELActive] > 0 then begin
    StateVsWith_OEL_UEL_In := StateVsWith_OEL_UEL_In + pHardCodedArr^[HARDCODE_EXCITER_OELSignal];
  end;
  if pHardCodedArr^[HARDCODE_EXCITER_UELActive] > 0 then begin
    StateVsWith_OEL_UEL_In := StateVsWith_OEL_UEL_In + pHardCodedArr^[HARDCODE_EXCITER_UELSignal];
  end;

  // Calculate Inverse of appropriate values
  if fTr = 0 then fInvTR := 0 else fInvTR := 1/fTr;
  if fTe = 0 then fInvTE := 0 else fInvTE := 1/fTe;
  if fTa = 0 then fInvTA := 0 else fInvTA := 1/fTa;
  if fTf = 0 then fInvTF := 0 else fInvTF := 1/fTf;

  //--------------------------------------------------------------------
  // fActiveLimitHL = 0 means not active, 1 means high limit, 2 means low limit
  fActiveLimitHLStateVr     := pByteActiveLimitArr^[NONWINDUPINDEX_VR]; // kate 8/8/12 only one nonwindup limit, so index is 1 (indexing of this array starts at 1)

  LOCAL_SaturationCalculateValues(fE1,fSEE1,fE2,fSEE2, fSatA, fSatB);

  If stateEField <= fSatA Then tempSatValTimesInput := 0 // no saturation
  Else tempSatValTimesInput := fSatB*stateEField*Sqr(stateEField-fSatA);
  dotStateEField := fInvTE*(stateVR - stateEField*fKe - tempSatValTimesInput);

  // Calculate initial value of the second state.
  dotStateSensedVt := fInvTR * (GenObjectVTerminalMayBeCompensated_In - stateSensedVt);

  //--------------------------------------------------------
  // Model of a Derivative "Washout" block
  //         Ks                      -K/T
  // OUT = ------ * IN  ==>>  OUT = ------ * IN   +   K/T * IN
  //       [1+Ts]                   [1+Ts]
  // OUT = StateInter + K/T * IN
  //--------------------------------------------------------
  fPIOFeedbackoutput := stateVf + fKf*fInvTF*stateEField;  // this value should be zero until something happens

  // Calculate 3rd state init val
  vInputState := VRef_In - stateSensedVt + StateVsWith_OEL_UEL_In;

  dotStateVR := fInvTA*(fKA*(vInputState - fPIOFeedbackoutput) - stateVR);
//  dotStateVR := fInvTA*(fKA*(vInputState - stateVf) - stateVR); // kate 3/15/13 test

  // kate 8/8/12 add limit checking, not sure this is necessary since it already seemed to match the built in model
  if (fActiveLimitHLStateVr > 0) then begin
    if      (fActiveLimitHLStateVr = 1) and (dotStateVR > 0) then dotStateVR := 0
    else if (fActiveLimitHLStateVr = 2) and (dotStateVR < 0) then dotStateVR := 0;
  end;

  //--------------------------------------------------------
  //            -K/T
  // StateVf = ------ * StateEField
  //           [1+Ts]
  //
  // DOTStateVf = 1/T * (-K/T*StateEField - StateVf)
  //--------------------------------------------------------
  dotStateVF := - fInvTF*(fPIOFeedbackoutput); // kate 3/7/12
//  dotStateVF := fInvTF*( fKf*dotStateEField - stateVf ); // kate 3/15/13 test

  // Push the resulting derivatives to the appropriate memory
  pDotXArr^[STATE_EField  ] := dotStateEField; // dotEFieldBeforeWindupLimit
  pDotXArr^[STATE_SensedVt] := dotStateSensedVt;                // dotVTerminalSensed
  pDotXArr^[STATE_VR      ] := dotStateVR;                      // dotVR
  pDotXArr^[STATE_VF      ] := dotStateVF;                      // fPIOFeedback.xdot(1)
end;

//******************************************************************************
procedure PropagateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
var GenObjectVTerminalMayBeCompensated_In : double;
    VRef_In : double;
    StateVsWith_OEL_UEL_In : double;
    stateSensedVt, stateEField, stateVf : double;
    fKa, fVrmax, fVrmin, fKf, fTf, fInvTF : double;
    fPIOFeedbackoutput : double;
    newValue : double;

    pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
    pFloatParamArr: PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;

  if pIgnoreStatesArr^[STATE_SensedVt] then begin
    GenObjectVTerminalMayBeCompensated_In := pHardCodedArr^[HARDCODE_EXCITER_GenVcomp];
    pStatesArr^[STATE_SensedVt] := GenObjectVTerminalMayBeCompensated_In;
  end;

  if pIgnoreStatesArr^[STATE_VR      ] then begin
    stateSensedVt := pStatesArr^[STATE_SensedVt];
    stateEField   := pStatesArr^[STATE_EField  ];
    stateVf       := pStatesArr^[STATE_VF      ];
    fKa           := pFloatParamArr^[PARAM_Ka    ];
    fVrmax        := pFloatParamArr^[PARAM_Vrmax ];
    fVrmin        := pFloatParamArr^[PARAM_Vrmin ];
    fKf           := pFloatParamArr^[PARAM_Kf    ];
    fTf           := pFloatParamArr^[PARAM_Tf    ];

    VRef_In       := pHardCodedArr^[HARDCODE_EXCITER_Vref];

    StateVsWith_OEL_UEL_In := pHardCodedArr^[HARDCODE_EXCITER_StabilizerSignal];
    if pHardCodedArr^[HARDCODE_EXCITER_OELActive] > 0 then begin
      StateVsWith_OEL_UEL_In := StateVsWith_OEL_UEL_In + pHardCodedArr^[HARDCODE_EXCITER_OELSignal];
    end;
    if pHardCodedArr^[HARDCODE_EXCITER_UELActive] > 0 then begin
      StateVsWith_OEL_UEL_In := StateVsWith_OEL_UEL_In + pHardCodedArr^[HARDCODE_EXCITER_UELSignal];
    end;
    if fTf = 0 then fInvTF := 0 else fInvTF := 1/fTf;
    fPIOFeedbackoutput := stateVf + fKf*fInvTF*stateEField;
    newValue := (VRef_In - stateSensedVt + StateVsWith_OEL_UEL_In - fPIOFeedbackoutput) * fKa;
    if      newValue > fVrmax then newValue := fVrmax
    else if newValue < fVrmin then newValue := fVrmin;
    pStatesArr^[STATE_VR      ] := newValue;
  end;
end;

//******************************************************************************
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits): Integer;
var pFloatParamArr : PDoubleArray;
begin
  Result := N_NONWINDUPLIMITS;   // only one state has a non windup limit
  pFloatParamArr := ParamsAndStates.FloatParams;
  nonWindUpLimits.LimitStates^[NONWINDUPINDEX_VR] := STATE_VR;
  nonWindUpLimits.minLimits^[NONWINDUPINDEX_VR] := pFloatParamArr^[PARAM_Vrmin];
  nonWindUpLimits.maxLimits^[NONWINDUPINDEX_VR] := pFloatParamArr^[PARAM_Vrmax];
end;

function ExciterEfieldOut(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
begin
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;

  result := pStatesArr^[STATE_EField] *
            (1 + pHardCodedArr^[HARDCODE_EXCITER_GenSpeedDeviationPU]);
end;

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
  Check_ExciterEfieldOut : TExciterEfieldOut = ExciterEfieldOut;

end.
