unit pwUDM.MTDCCONV;
// This implements a generic MTDC converter model as an example

interface

uses
  System.SysUtils,
  pwUDM.Core;

const
  // For the Model
  N_F_PARAMS   = 9;   // number of float model params
  N_I_PARAMS   = 0;   // number of integer model params
  N_S_PARAMS   = 0;   // number of string model params
  N_STATES     = 4;   // number of model states
  N_ALGEBRAICS = 0;   // number of model algebraics

  // For the NonWindUpLimits
  N_NONWINDUPLIMITS = 1;  // number of nonwindup limits

  //---------------------------------------------------------------------------
  // These are just to make the code easier to read
  // Parameter Value indices.
  PARAM_Tf_Idc    = 0;
  PARAM_Tf_Vdc    = 1;
  PARAM_Tf_Vac    = 2;
  PARAM_Ki        = 3;
  PARAM_Id_base   = 4;
  PARAM_CosMin    = 5;
  PARAM_CosMax    = 6;
  PARAM_IdRefLim  = 7;
  PARAM_Id_margin = 8;

  // State Value Indices.
  STATE_IdcSense = 0;
  STATE_VdcSense = 1;
  STATE_VacSense = 2;
  STATE_CosAngle = 3;

  NONWINDUPINDEX_CosAngle = 0; // index for list of non-windup limited states

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
function MTDCConverterCosControlAngle(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
function MTDCConverterIdcSense(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
function MTDCConverterVdcSense(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
function MTDCConverterVacSense(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
procedure MTDCConverterCurrentLimitAndMargin(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; IdRefLim : PDouble; fid_Margin : PDouble);

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

        , MTDCConverterCosControlAngle
        , MTDCConverterIdcSense
        , MTDCConverterVdcSense
        , MTDCConverterVacSense
        , MTDCConverterCurrentLimitAndMargin
        ;

implementation

uses
  pwUDM.MTDCConverter;



//******************************************************************************
function DLLVersion:Integer;
begin
  result := 1;
end;

//******************************************************************************
function modelClassName(StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
begin
  result := pwProvideString(StrBuf, StrSize, pwModelClassString(mcMTDCConvertor));
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
    PARAM_Tf_Idc    : ts := 'Tf_Idc';
    PARAM_Tf_Vdc    : ts := 'Tf_Vdc';
    PARAM_Tf_Vac    : ts := 'Tf_Vac';
    PARAM_Ki        : ts := 'Ki';
    PARAM_Id_base   : ts := 'Id_base';
    PARAM_CosMin    : ts := 'CosMin';
    PARAM_CosMax    : ts := 'CosMax';
    PARAM_IdRefLim  : ts := 'IdRefLim';
    PARAM_Id_margin : ts := 'Id_margin';
    else              ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;

//******************************************************************************
function stateName(StateNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case StateNum^ of
    STATE_IdcSense : ts := 'Idc Sense';
    STATE_VdcSense : ts := 'Vdc Sense';
    STATE_VacSense : ts := 'Vac Sense';
    STATE_CosAngle : ts := 'Cos Angle';
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
  // Example MTDCCONV parameters
  pFloatParamArr := ParamsAndStates^.FloatParams;
  pFloatParamArr^[PARAM_Tf_Idc   ]  :=    0.02;
  pFloatParamArr^[PARAM_Tf_Vdc   ]  :=    0.02;
  pFloatParamArr^[PARAM_Tf_Vac   ]  :=    0.05;
  pFloatParamArr^[PARAM_Ki       ]  :=    5.0;
  pFloatParamArr^[PARAM_Id_base  ]  := 1000.0;
  pFloatParamArr^[PARAM_CosMin   ]  :=   -1.0;
  pFloatParamArr^[PARAM_CosMax   ]  :=    1.0;
  pFloatParamArr^[PARAM_IdRefLim ]  :=    0.0;
  pFloatParamArr^[PARAM_Id_margin]  :=    0.0;
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
    fTf_Idc, fTf_Vdc, fTf_Vac, fId_margin : Double;
    // Signals from Simulator at the start of the simulation
    local_Idc, local_Vdc, local_Vac, local_CosAngle : Double;

    pHardCodedArr : PDoubleArray;
    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  fTf_Idc    := pFloatParamArr^[PARAM_Tf_Idc   ];
  fTf_Vdc    := pFloatParamArr^[PARAM_Tf_Vdc   ];
  fTf_Vac    := pFloatParamArr^[PARAM_Tf_Vac   ];
  fId_margin := pFloatParamArr^[PARAM_Id_margin];

  local_Idc      := pHardCodedArr^[HARDCODE_MTDCCONV_CurrentDC     ];
  local_Vdc      := pHardCodedArr^[HARDCODE_MTDCCONV_VoltageDC     ];
  local_Vac      := pHardCodedArr^[HARDCODE_MTDCCONV_VoltageAC     ];
  local_CosAngle := pHardCodedArr^[HARDCODE_MTDCCONV_CosControlAngle];

  // Ignored States
  // A time constant of zero means the sensed value follows its input directly
  If fTf_Idc = 0 Then pIgnoreStatesArr^[STATE_IdcSense] := True;
  If fTf_Vdc = 0 Then pIgnoreStatesArr^[STATE_VdcSense] := True;
  If fTf_Vac = 0 Then pIgnoreStatesArr^[STATE_VacSense] := True;

  // The following parameters should actually get SET, not read in!
  // The regulator starts at a steady state: it sees no error when the current order
  // is the present current plus the margin.
  pHardCodedArr^[HARDCODE_MTDCCONV_CurrentReference] := local_Idc + fId_margin;

  // Write out initial state vector
  pStatesArr^[STATE_IdcSense] := local_Idc;
  pStatesArr^[STATE_VdcSense] := local_Vdc;
  pStatesArr^[STATE_VacSense] := local_Vac;
  pStatesArr^[STATE_CosAngle] := local_CosAngle;
end;

//******************************************************************************
procedure calculateFofX(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits; dotX : PDoubleArray);
var // Parameters
    fTf_Idc, fTf_Vdc, fTf_Vac, fKi, fId_base, fId_margin : Double;
    fInvTf_Idc, fInvTf_Vdc, fInvTf_Vac : Double;

    // State variables
    stateIdcSense, stateVdcSense, stateVacSense : Double;

    // FofX or xdot,
    dotStateIdcSense, dotStateVdcSense, dotStateVacSense, dotStateCosAngle : Double;
    Id_Ref_In, Idc_In, Vdc_In, Vac_In : Double;
    fCurrentError : Double;

    fActiveLimitHLStateCos : Byte;

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
  stateIdcSense := pStatesArr^[STATE_IdcSense];
  stateVdcSense := pStatesArr^[STATE_VdcSense];
  stateVacSense := pStatesArr^[STATE_VacSense];

  fTf_Idc    := pFloatParamArr^[PARAM_Tf_Idc   ];
  fTf_Vdc    := pFloatParamArr^[PARAM_Tf_Vdc   ];
  fTf_Vac    := pFloatParamArr^[PARAM_Tf_Vac   ];
  fKi        := pFloatParamArr^[PARAM_Ki       ];
  fId_base   := pFloatParamArr^[PARAM_Id_base  ];
  fId_margin := pFloatParamArr^[PARAM_Id_margin];

  Id_Ref_In := pHardCodedArr^[HARDCODE_MTDCCONV_CurrentReference];
  Idc_In    := pHardCodedArr^[HARDCODE_MTDCCONV_CurrentDC       ];
  Vdc_In    := pHardCodedArr^[HARDCODE_MTDCCONV_VoltageDC       ];
  Vac_In    := pHardCodedArr^[HARDCODE_MTDCCONV_VoltageAC       ];

  // Calculate Inverse of appropriate values
  if fTf_Idc = 0 then fInvTf_Idc := 0 else fInvTf_Idc := 1/fTf_Idc;
  if fTf_Vdc = 0 then fInvTf_Vdc := 0 else fInvTf_Vdc := 1/fTf_Vdc;
  if fTf_Vac = 0 then fInvTf_Vac := 0 else fInvTf_Vac := 1/fTf_Vac;
  if fId_base = 0 then fId_base := 1;

  //--------------------------------------------------------------------
  // fActiveLimitHL = 0 means not active, 1 means high limit, 2 means low limit
  fActiveLimitHLStateCos := pByteActiveLimitArr^[NONWINDUPINDEX_CosAngle]; // Only one nonwindup limit, so index is 0

  // dot sensed values: first order lags of the DC current, the DC voltage and the AC voltage
  dotStateIdcSense := fInvTf_Idc*(Idc_In - stateIdcSense);
  dotStateVdcSense := fInvTf_Vdc*(Vdc_In - stateVdcSense);
  dotStateVacSense := fInvTf_Vac*(Vac_In - stateVacSense);

  // dot cos angle: an integral current regulator. The error is in pu of Id_base.
  fCurrentError := (Id_Ref_In - fId_margin - stateIdcSense)/fId_base;
  dotStateCosAngle := fKi*fCurrentError;
  // Note: the cos angle has a limit, and when that limit is active, we want dotStateCosAngle to be zero.
  if (fActiveLimitHLStateCos > 0) then begin
    // 1 indicates it's stuck at the HIGH limit
    // 2 indicates it's stuck at the LOW limit
    if      (fActiveLimitHLStateCos = 1) and (dotStateCosAngle > 0) then dotStateCosAngle := 0
    else if (fActiveLimitHLStateCos = 2) and (dotStateCosAngle < 0) then dotStateCosAngle := 0;
  end;

  //--------------------------------------------------------------------
  // Push the resulting derivatives to the appropriate memory
  //--------------------------------------------------------------------
  pDotXArr^[STATE_IdcSense] := dotStateIdcSense;
  pDotXArr^[STATE_VdcSense] := dotStateVdcSense;
  pDotXArr^[STATE_VacSense] := dotStateVacSense;
  pDotXArr^[STATE_CosAngle] := dotStateCosAngle;
end;

//******************************************************************************
procedure PropagateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
var fIdRefLim : double;

    pHardCodedArr : PDoubleArray;
    pFloatParamArr: PDoubleArray;
    pStatesArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;

  // An ignored sensed value is just its input
  if pIgnoreStatesArr^[STATE_IdcSense] then pStatesArr^[STATE_IdcSense] := pHardCodedArr^[HARDCODE_MTDCCONV_CurrentDC];
  if pIgnoreStatesArr^[STATE_VdcSense] then pStatesArr^[STATE_VdcSense] := pHardCodedArr^[HARDCODE_MTDCCONV_VoltageDC];
  if pIgnoreStatesArr^[STATE_VacSense] then pStatesArr^[STATE_VacSense] := pHardCodedArr^[HARDCODE_MTDCCONV_VoltageAC];

  // The current limit of the converter also limits the current order that Simulator passes in.
  // A limit of zero or less means no limit.
  fIdRefLim := pFloatParamArr^[PARAM_IdRefLim];
  if (fIdRefLim > 0) and (pHardCodedArr^[HARDCODE_MTDCCONV_CurrentReference] > fIdRefLim) then begin
    pHardCodedArr^[HARDCODE_MTDCCONV_CurrentReference] := fIdRefLim;
  end;
end;

//******************************************************************************
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits): Integer;
var pFloatParamArr : PDoubleArray;
begin
  Result := 1;   // only one state has a non windup limit

  pFloatParamArr := ParamsAndStates.FloatParams;
  nonWindUpLimits.LimitStates^[NONWINDUPINDEX_CosAngle] := STATE_CosAngle;
  nonWindUpLimits.minLimits^[NONWINDUPINDEX_CosAngle] := pFloatParamArr^[PARAM_CosMin];
  nonWindUpLimits.maxLimits^[NONWINDUPINDEX_CosAngle] := pFloatParamArr^[PARAM_CosMax];
end;

//******************************************************************************
function MTDCConverterCosControlAngle(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var pStatesArr : PDoubleArray;
begin
  pStatesArr := ParamsAndStates.States;

  // The cosine of the control angle is the output of the current regulator
  Result := pStatesArr^[STATE_CosAngle];
end;

//******************************************************************************
function MTDCConverterIdcSense(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
begin
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;

  // An ignored sensed value is just its input
  if pIgnoreStatesArr^[STATE_IdcSense]
  then Result := pHardCodedArr^[HARDCODE_MTDCCONV_CurrentDC]
  else Result := pStatesArr^[STATE_IdcSense];
end;

//******************************************************************************
function MTDCConverterVdcSense(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
begin
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;

  // An ignored sensed value is just its input
  if pIgnoreStatesArr^[STATE_VdcSense]
  then Result := pHardCodedArr^[HARDCODE_MTDCCONV_VoltageDC]
  else Result := pStatesArr^[STATE_VdcSense];
end;

//******************************************************************************
function MTDCConverterVacSense(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
begin
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;

  // An ignored sensed value is just its input
  if pIgnoreStatesArr^[STATE_VacSense]
  then Result := pHardCodedArr^[HARDCODE_MTDCCONV_VoltageAC]
  else Result := pStatesArr^[STATE_VacSense];
end;

//******************************************************************************
procedure MTDCConverterCurrentLimitAndMargin(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; IdRefLim : PDouble; fid_Margin : PDouble);
var pFloatParamArr: PDoubleArray;
begin
  pFloatParamArr := ParamsAndStates.FloatParams;

  // A limit of zero or less means no limit
  IdRefLim^   := pFloatParamArr^[PARAM_IdRefLim ];
  fid_Margin^ := pFloatParamArr^[PARAM_Id_margin];
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
  Check_MTDCConverterCosControlAngle : TMTDCConverterCosControlAngle = MTDCConverterCosControlAngle;
  Check_MTDCConverterIdcSense : TMTDCConverterIdcSense = MTDCConverterIdcSense;
  Check_MTDCConverterVdcSense : TMTDCConverterVdcSense = MTDCConverterVdcSense;
  Check_MTDCConverterVacSense : TMTDCConverterVacSense = MTDCConverterVacSense;
  Check_MTDCConverterCurrentLimitAndMargin : TMTDCConverterCurrentLimitAndMargin = MTDCConverterCurrentLimitAndMargin;

end.
