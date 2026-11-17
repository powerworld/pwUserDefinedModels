unit pwUDM.TGOV1;
// This implements the existing TGOV1 model as an example

interface

uses
  System.SysUtils,
  pwUDM.Core;

const
  // For the Model
  N_F_PARAMS   = 8;   // number of float model params
  N_I_PARAMS   = 0;   // number of integer model params
  N_S_PARAMS   = 0;   // number of string model params
  N_STATES     = 2;   // number of model states
  N_ALGEBRAICS = 0;   // number of model algebraics

  // For the NonWindUpLimits
  N_NONWINDUPLIMITS = 1;  // number of nonwindup limits

  //---------------------------------------------------------------------------
  // These are just to make the code easier to read
  // Parameter Value indices.
  PARAM_R     = 0;
  PARAM_T1    = 1;
  PARAM_Vmax  = 2;
  PARAM_Vmin  = 3;
  PARAM_T2    = 4;
  PARAM_T3    = 5;
  PARAM_Dt    = 6;
  PARAM_Trate = 7;

  // State Value Indices.
  STATE_TurbPower = 0;
  STATE_ValvePos  = 1;

  NONWINDUPINDEX_ValvePos = 0; // index for list of non-windup limited states

  // Algebraic Value Indices
  // None ALG_TSXXX             = 0;

// The routines below are the DLL contract that Simulator calls: each one is exported by the
// exports clause that follows them (the optional ones are commented out unless the model needs them)
function DLLVersion:Integer; stdcall;
function modelClassName(StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer; stdcall;
procedure allParamCounts(numbersOfEverything : PTxParamCounts; TimeStepSeconds : PDouble); stdcall;
function parameterName(ParamNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
function stateName(StateNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
// function OtherObjectClass(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
// function OtherObjectDescription(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
procedure getDefaultParameterValue(ParamsAndStates : PTxMyModelData); stdcall;
// function getStringParamDefaultValue(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
// function signalSelection(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer; stdcall;
function SubIntervalPower2Exponent(ParamsAndStates : PTxMyModelData; TimeStepSeconds : PDouble) : integer; stdcall;
procedure initializeYourself(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions); stdcall;
procedure calculateFofX(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits; dotX : PDoubleArray); stdcall;
procedure PropagateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions); stdcall;
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits): Integer; stdcall;
function GovernorPmechOut(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double; stdcall;

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

        , GovernorPmechOut
        ;

implementation

uses
  pwUDM.Governor;



//******************************************************************************
function DLLVersion:Integer;
begin
  result := 1;
end;

//******************************************************************************
function modelClassName(StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
begin
  result := pwStringCopy(StrBuf, StrSize, pwModelClassString(mcGovernor));
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
    PARAM_R     : ts := 'R';
    PARAM_T1    : ts := 'T1';
    PARAM_Vmax  : ts := 'Vmax';
    PARAM_Vmin  : ts := 'Vmin';
    PARAM_T2    : ts := 'T2';
    PARAM_T3    : ts := 'T3';
    PARAM_Dt    : ts := 'Dt';
    PARAM_Trate : ts := 'Trate';
    else          ts := '';
  end;
  result := pwStringCopy(StrBuf, StrSize, ts);
end;

//******************************************************************************
function stateName(StateNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case StateNum^ of
    STATE_TurbPower : ts := 'Turbine Power';
    STATE_ValvePos  : ts := 'Valve Position';
   else ts := '';
  end;
  result := pwStringCopy(StrBuf, StrSize, ts);
end;

//******************************************************************************
{ non specified for this model
function OtherObjectClass(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'Bus';
   else ts := '';
  end;
  result := pwStringCopy(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
{ non specified for this model
function OtherObjectDescription(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'Signal Bus';
   else ts := '';
  end;
  result := pwStringCopy(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
procedure getDefaultParameterValue(ParamsAndStates : PTxMyModelData);
var pDoubArr: PDoubleArray;
begin
  // Example TGOV1 parameters
  pDoubArr := ParamsAndStates^.FloatParams;
  pDoubArr^[PARAM_R    ]  := 0.05;
  pDoubArr^[PARAM_T1   ]  := 0.50;
  pDoubArr^[PARAM_Vmax ]  := 1.0;
  pDoubArr^[PARAM_Vmin ]  := 0.0;
  pDoubArr^[PARAM_T2   ]  := 2.5;
  pDoubArr^[PARAM_T3   ]  := 7.5;
  pDoubArr^[PARAM_Dt   ]  := 0.0;
  pDoubArr^[PARAM_Trate]  := 0.0;
end;

{ non specified for this model
function getStringParamDefaultValue(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'DefaultString0';
   else ts := '';
  end;
  result := pwStringCopy(StrBuf, StrSize, ts);
end;
}

{ non specified for this model
//******************************************************************************
function signalSelection(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer; stdcall;
var ts : PChar;
begin
  ts := ''; // there are none!
  case Num^ of
    ALG_XXX         : ts := 'TSXXX';
   else ts := '';
  end;
  result := pwStringCopy(StrBuf, StrSize, ts);
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
var // Parameters,
    fR, fTrate : Double;
    local_PMech, PMechObjectInitInternal, GenObjectMVABase : Double;
    GovResponseLimits : Byte;

    pHardCodedArr : PDoubleArray;
    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  fR     := pFloatParamArr^[PARAM_R];
  fTrate := pFloatParamArr^[PARAM_Trate];

  PMechObjectInitInternal := pHardCodedArr^[HARDCODE_GOV_InitPmech];
  GenObjectMVABase        := pHardCodedArr^[HARDCODE_GOV_GenMVABase];
  GovResponseLimits       := Round(pHardCodedArr^[HARDCODE_GOV_GovResponseLimits]);

  If fTrate = 0 Then fTrate := GenObjectMVABase;

  //--------------------------------------------------------------------
  // Back solve to determine the initial states.
  //--------------------------------------------------------------------
  local_PMech := PMechObjectInitInternal;
  local_PMech := local_PMech*GenObjectMVABase/fTrate; // jamie 2/9/09

  // Handle the Governor Response Limits Options
  // A value of 1 means it's "Down Only", 2 means it's "Fixed"
  // This automatically change the Vmax and Vmin parameters as necessary.
  case GovResponseLimits of
    1 : begin // "Down Only"
          pFloatParamArr^[PARAM_Vmax] := local_PMech;
          // Make sure Min is less than Max still also...
          if pFloatParamArr^[PARAM_Vmin] > local_PMech then pFloatParamArr^[PARAM_Vmin] := local_PMech;
        end;
    2 : begin // "Fixed"
          pFloatParamArr^[PARAM_Vmax] := local_PMech;
          pFloatParamArr^[PARAM_Vmin] := local_PMech;
        end;
   else begin end; // do nothing otherwise
  end;

  // No Ignored States

  // Must Set the initial PRef value
  // The following parameters should actually get SET, not read in!
  pHardCodedArr^[HARDCODE_GOV_Pref] := local_PMech*fR;

  // Write out initial state vector
  pStatesArr^[STATE_TurbPower] := local_PMech;
  pStatesArr^[STATE_ValvePos]  := local_PMech;
end;

//******************************************************************************
procedure calculateFofX(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits; dotX : PDoubleArray);
var // Parameters
    fR, fT1, fT2, fT3 : Double;
    fInvR, fInvT1, fInvT3 : Double;

    // State variables
    stateTurbinePower, stateValve : Double;

    // FofX or xdot,
    dotStateTurbinePower, dotStateValve : Double;
    Pref_In, deltaWPU_In : Double;

    fActiveLimitHLState2 : Byte;

    pFloatParamArr: PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pStatesArr : PDoubleArray;
    pByteActiveLimitArr : PByteArray;
    pDotXArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pByteActiveLimitArr  := nonWindUpLimits.activeLimits;
  pDotXArr := dotX;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  stateTurbinePower := pStatesArr^[STATE_TurbPower];
  stateValve        := pStatesArr^[STATE_ValvePos];

  fR  := pFloatParamArr^[PARAM_R];
  fT1 := pFloatParamArr^[PARAM_T1];
  fT2 := pFloatParamArr^[PARAM_T2];
  fT3 := pFloatParamArr^[PARAM_T3];

  Pref_In     := pHardCodedArr^[HARDCODE_GOV_Pref];
  deltaWPU_In := pHardCodedArr^[HARDCODE_GOV_GenSpeedDeviationPU];

  // Calculate Inverse of appropriate values
  if fR  = 0 then fInvR  := 0 else fInvR  := 1/fR;
  if fT1 = 0 then fInvT1 := 0 else fInvT1 := 1/fT1;
  if fT3 = 0 then fInvT3 := 0 else fInvT3 := 1/fT3;

  //--------------------------------------------------------------------
  // fActiveLimitHL = 0 means not active, 1 means high limit, 2 means low limit
  fActiveLimitHLState2 := pByteActiveLimitArr^[NONWINDUPINDEX_ValvePos]; // Only one nonwindup limit, so index is 0

  // dot state 2
  dotStateValve := fInvT1*((Pref_In - deltaWPU_In)*fInvR - stateValve);
  // Note: state 2 has a limit, and when that limit is active, we want dotStateValve to be zero.
  if (fActiveLimitHLState2 > 0) then begin
    // 1 indicates it's stuck at the HIGH limit
    // 2 indicates it's stuck at the LOW limit
    if      (fActiveLimitHLState2 = 1) and (dotStateValve > 0) then dotStateValve := 0
    else if (fActiveLimitHLState2 = 2) and (dotStateValve < 0) then dotStateValve := 0;
  end;

  // dot state 1
  dotStateTurbinePower := fInvT3*(stateValve + fT2*dotStateValve - stateTurbinePower);

  //--------------------------------------------------------------------
  // Push the resulting derivatives to the appropriate memory
  //--------------------------------------------------------------------
  pDotXArr^[STATE_TurbPower] := dotStateTurbinePower;
  pDotXArr^[STATE_ValvePos]  := dotStateValve;
end;

//******************************************************************************
procedure PropagateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
Begin
  // no ignored states or handling of algebraics here
end;

//******************************************************************************
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits): Integer;
var pFloatParamArr : PDoubleArray;
begin
  Result := 1;   // only one state has a non windup limit

  pFloatParamArr := ParamsAndStates.FloatParams;
  nonWindUpLimits.limitStates^[NONWINDUPINDEX_ValvePos] := STATE_ValvePos;
  nonWindUpLimits.minLimits^[NONWINDUPINDEX_ValvePos] := pFloatParamArr^[PARAM_Vmin];
  nonWindUpLimits.maxLimits^[NONWINDUPINDEX_ValvePos] := pFloatParamArr^[PARAM_Vmax];
end;

function GovernorPmechOut(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var fDt, fTrate : Double;
    stateTurbinePower : Double;
    GenObjectMVABase, deltaWPU_In : Double;
    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
Begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  stateTurbinePower := pStatesArr^[STATE_TurbPower];

  fDt    := pFloatParamArr^[PARAM_Dt];
  fTrate := pFloatParamArr^[PARAM_Trate];

  GenObjectMVABase := pHardCodedArr^[HARDCODE_GOV_GenMVABase];
  deltaWPU_In      := pHardCodedArr^[HARDCODE_GOV_GenSpeedDeviationPU];

  If fTrate = 0 Then fTrate := GenObjectMVABase;

  //--------------------------------------------------------------------
  // Calculate Pmech output
  //--------------------------------------------------------------------
  Result := stateTurbinePower - fDt*deltaWPU_In;
  Result := Result/GenObjectMVABase*fTrate;
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
  Check_GovernorPmechOut : TGovernorPmechOut = GovernorPmechOut;

end.
