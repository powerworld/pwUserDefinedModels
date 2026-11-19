unit pwUDM.MTDCLINE;
// This implements a generic multi-terminal DC model as an example

interface

uses
  System.SysUtils,
  pwUDM.Core;

const
  // This example handles up to MAX_TERMINALS converters, one state and one algebraic for each.
  // Simulator does not tell the DLL how many converters the line has, so the integer parameter
  // NumTerminals must be set to that number (from 1 to MAX_TERMINALS).
  MAX_TERMINALS = 8;

  // For the Model
  N_F_PARAMS   = 2;                // number of float model params
  N_I_PARAMS   = 1;                // number of integer model params
  N_S_PARAMS   = 0;                // number of string model params
  N_STATES     = MAX_TERMINALS;    // number of model states
  N_ALGEBRAICS = MAX_TERMINALS;    // number of model algebraics

  // For the NonWindUpLimits
  N_NONWINDUPLIMITS = 0;  // number of nonwindup limits

  //---------------------------------------------------------------------------
  // These are just to make the code easier to read
  // Parameter Value indices.
  // Note: Simulator numbers the integer parameters first, then the float parameters, so the
  //       parameter number that parameterName receives for a float parameter is N_I_PARAMS + PARAM_xxx
  PARAM_Tord      = 0;
  PARAM_IordScale = 1;

  // Integer Parameter Value indices.
  IPARAM_NumTerminals = 0;

  // State Value Indices.
  // The current order of converter number i (counting from 0) is state STATE_IdRef + i
  STATE_IdRef = 0;

  // Algebraic Value Indices
  // The initial current order of converter number i (counting from 0) is algebraic ALG_IdRefInit + i
  ALG_IdRefInit = 0;

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
function MultiTerminalDCGetIDRef(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Converter : PInteger) : double;

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

        , MultiTerminalDCGetIDRef
        ;

implementation

uses
  pwUDM.MultiTerminalDC;


// The number of converters the line has, limited to what this example can handle
function LOCAL_NumberOfTerminals(ParamsAndStates : PTxMyModelData) : Integer;
begin
  Result := ParamsAndStates.IntParams^[IPARAM_NumTerminals];
  if Result < 1 then Result := 1;
  if Result > MAX_TERMINALS then Result := MAX_TERMINALS;
end;

// The place in the hard-coded signals of a signal of a converter
function LOCAL_HardCodedIndex(Converter, Signal : Integer) : Integer;
begin
  Result := Converter*HARDCODE_MTDC_SignalsPerConverter + Signal;
end;



//******************************************************************************
function DLLVersion:Integer;
begin
  result := 1;
end;

//******************************************************************************
function modelClassName(StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
begin
  result := pwProvideString(StrBuf, StrSize, pwModelClassString(mcMTDCLine));
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
  // The integer parameters come first, then the float parameters
  case ParamNum^ of
    IPARAM_NumTerminals          : ts := 'NumTerminals';
    N_I_PARAMS + PARAM_Tord      : ts := 'Tord';
    N_I_PARAMS + PARAM_IordScale : ts := 'IordScale';
    else                           ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;

//******************************************************************************
function stateName(StateNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case StateNum^ of
    STATE_IdRef + 0 : ts := 'Terminal 1 Id Ref';
    STATE_IdRef + 1 : ts := 'Terminal 2 Id Ref';
    STATE_IdRef + 2 : ts := 'Terminal 3 Id Ref';
    STATE_IdRef + 3 : ts := 'Terminal 4 Id Ref';
    STATE_IdRef + 4 : ts := 'Terminal 5 Id Ref';
    STATE_IdRef + 5 : ts := 'Terminal 6 Id Ref';
    STATE_IdRef + 6 : ts := 'Terminal 7 Id Ref';
    STATE_IdRef + 7 : ts := 'Terminal 8 Id Ref';
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
    pIntParamArr : PIntegerArray;
begin
  // Example MTDCLINE parameters
  pFloatParamArr := ParamsAndStates^.FloatParams;
  pFloatParamArr^[PARAM_Tord     ]  := 0.5;
  pFloatParamArr^[PARAM_IordScale]  := 1.0;

  pIntParamArr := ParamsAndStates^.IntParams;
  pIntParamArr^[IPARAM_NumTerminals]  := 3;
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
var // Parameters,
    fTord : Double;
    nTerminals, i : Integer;
    local_IdRef : Double;

    pHardCodedArr : PDoubleArray;
    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pAlgebraicsArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pAlgebraicsArr := ParamsAndStates.Algebraics;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  fTord := pFloatParamArr^[PARAM_Tord];
  nTerminals := LOCAL_NumberOfTerminals(ParamsAndStates);

  for i := 0 to MAX_TERMINALS-1 do begin
    if i < nTerminals then begin
      // The converters are initialized before the line, so each already has its initial current order.
      // Each converter keeps its initial share of the pole current order.
      local_IdRef := pHardCodedArr^[LOCAL_HardCodedIndex(i, HARDCODE_MTDC_CurrentReference)];

      // Ignored States
      // A time constant of zero means the current order follows its target directly
      If fTord = 0 Then pIgnoreStatesArr^[STATE_IdRef + i] := True;
    end
    else begin
      // This terminal does not exist
      local_IdRef := 0;
      pIgnoreStatesArr^[STATE_IdRef + i] := True;
    end;

    pAlgebraicsArr^[ALG_IdRefInit + i] := local_IdRef;

    // Write out initial state vector
    pStatesArr^[STATE_IdRef + i] := local_IdRef;
  end;
end;

//******************************************************************************
procedure calculateFofX(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits; dotX : PDoubleArray);
var // Parameters
    fTord, fIordScale : Double;
    fInvTord : Double;
    nTerminals, i : Integer;

    // State variables
    stateIdRef : Double;

    // FofX or xdot,
    dotStateIdRef : Double;

    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pAlgebraicsArr : PDoubleArray;
    pDotXArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pAlgebraicsArr := ParamsAndStates.Algebraics;
  pDotXArr := dotX;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  fTord      := pFloatParamArr^[PARAM_Tord     ];
  fIordScale := pFloatParamArr^[PARAM_IordScale];
  nTerminals := LOCAL_NumberOfTerminals(ParamsAndStates);

  // Calculate Inverse of appropriate values
  if fTord = 0 then fInvTord := 0 else fInvTord := 1/fTord;

  for i := 0 to MAX_TERMINALS-1 do begin
    // The current order of each terminal follows its initial share of the scaled pole current order
    if i < nTerminals then begin
      stateIdRef := pStatesArr^[STATE_IdRef + i];
      dotStateIdRef := fInvTord*(fIordScale*pAlgebraicsArr^[ALG_IdRefInit + i] - stateIdRef);
    end
    else dotStateIdRef := 0;

    //--------------------------------------------------------------------
    // Push the resulting derivatives to the appropriate memory
    //--------------------------------------------------------------------
    pDotXArr^[STATE_IdRef + i] := dotStateIdRef;
  end;
end;

//******************************************************************************
procedure PropagateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
var fIordScale : Double;
    nTerminals, i : Integer;

    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pAlgebraicsArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pAlgebraicsArr := ParamsAndStates.Algebraics;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;

  fIordScale := pFloatParamArr^[PARAM_IordScale];
  nTerminals := LOCAL_NumberOfTerminals(ParamsAndStates);

  for i := 0 to MAX_TERMINALS-1 do begin
    // An ignored current order is just its target
    if pIgnoreStatesArr^[STATE_IdRef + i] then begin
      if i < nTerminals
      then pStatesArr^[STATE_IdRef + i] := fIordScale*pAlgebraicsArr^[ALG_IdRefInit + i]
      else pStatesArr^[STATE_IdRef + i] := 0;
    end;
  end;
end;

//******************************************************************************
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits): Integer;
begin
  Result := 0;   // no state has a non windup limit
end;

//******************************************************************************
function MultiTerminalDCGetIDRef(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; Converter : PInteger) : double;
var fTord, fIordScale : Double;
    nTerminals : Integer;
    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pAlgebraicsArr : PDoubleArray;
begin
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pAlgebraicsArr := ParamsAndStates.Algebraics;

  fTord      := pFloatParamArr^[PARAM_Tord     ];
  fIordScale := pFloatParamArr^[PARAM_IordScale];
  nTerminals := LOCAL_NumberOfTerminals(ParamsAndStates);

  //--------------------------------------------------------------------
  // Calculate the current order of the converter. Converter^ counts from 0.
  //--------------------------------------------------------------------
  if (Converter^ < 0) or (Converter^ >= nTerminals) then Result := 0
  else if fTord = 0 then Result := fIordScale*pAlgebraicsArr^[ALG_IdRefInit + Converter^]
  else Result := pStatesArr^[STATE_IdRef + Converter^];
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
  Check_MultiTerminalDCGetIDRef : TMultiTerminalDCGetIDRef = MultiTerminalDCGetIDRef;

end.
