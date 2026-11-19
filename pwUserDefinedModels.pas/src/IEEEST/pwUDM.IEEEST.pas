unit pwUDM.IEEEST;
// This implements the existing IEEEST model as an example

interface

uses
  System.SysUtils,
  pwUDM.Core;

const
  // For the Model
  N_F_PARAMS     = 18;   // number of float model params
  N_I_PARAMS     =  1;   // number of integer model params
  N_S_PARAMS     =  0;   // number of string model params
  N_STATES       =  7;   // number of model states
  N_ALGEBRAICS   =  0;   // number of model algebraics

  // For the NonWindUpLimits
  N_NONWINDUPLIMITS = 0;  // number of nonwindup limits

  //---------------------------------------------------------------------------
  // These are just to make the code easier to read
  // Parameter Value indices.
  PARAM_A1     =  0;
  PARAM_A2     =  1;
  PARAM_A3     =  2;
  PARAM_A4     =  3;
  PARAM_A5     =  4;
  PARAM_A6     =  5;
  PARAM_T1     =  6;
  PARAM_T2     =  7;
  PARAM_T3     =  8;
  PARAM_T4     =  9;
  PARAM_T5     = 10;
  PARAM_T6     = 11;
  PARAM_Ks     = 12;
  PARAM_Lsmax  = 13;
  PARAM_Lsmin  = 14;
  PARAM_Vcu    = 15;
  PARAM_Vcl    = 16;
  PARAM_Tdelay = 17;

  // Integer Parameter Value indices.
  IPARAM_Ics   =  0;

  // State Value Indices.
  STATE_Filter1         = 0;
  STATE_Filter2         = 1;
  STATE_Filter3         = 2;
  STATE_FilterOut       = 3;
  STATE_LL1             = 4;
  STATE_LL2             = 5;
  STATE_UnlimitedSignal = 6;

  // Algebraic Value Indices
  // None ALG_TSXXX             = 0;

// The routines below are the DLL contract that Simulator calls: each one is exported by the
// exports clause that follows them (the optional ones are commented out unless the model needs them)
function DLLVersion:Integer;
function modelClassName(StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
procedure allParamCounts(numbersOfEverything : PTxParamCounts; TimeStepSeconds : PDouble);
function parameterName(ParamNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
function stateName(StateNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
function OtherObjectClass(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
function OtherObjectDescription(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
procedure getDefaultParameterValue(ParamsAndStates : PTxMyModelData);
// function getStringParamDefaultValue(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
// function signalSelection(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
function SubIntervalPower2Exponent(ParamsAndStates : PTxMyModelData; TimeStepSeconds : PDouble) : integer;
procedure initializeYourself(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
procedure calculateFofX(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits; dotX : PDoubleArray);
procedure PropagateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits): Integer;
function StabilizerVsOut(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
// function StabilizerPitchOut(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;

exports DLLVersion
        , modelClassName
        , allParamCounts
        , parameterName
        , stateName
        , OtherObjectClass
        , OtherObjectDescription
        , getDefaultParameterValue
     // , getStringParamDefaultValue
     // , signalSelection

        , SubIntervalPower2Exponent
        , initializeYourself
        , calculateFofX
        , PropagateIgnoredStateAndInput
        , getNonWindUpLimits

        , StabilizerVsOut
        // , StabilizerPitchOut
        ;

implementation

uses
  pwUDM.Stabilizer;



//******************************************************************************
function DLLVersion:Integer;
begin
  result := 1;
end;

//******************************************************************************
function modelClassName(StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer;
begin
  result := pwProvideString(StrBuf, StrSize, pwModelClassString(mcStabilizer));
end;

//******************************************************************************
procedure allParamCounts(numbersOfEverything : PTxParamCounts; TimeStepSeconds : PDouble);
begin
  with numbersOfEverything^ do begin
    // -------The model-specific parameters-----
    nFloatParams  := N_F_PARAMS;
    nIntParams    := N_I_PARAMS;
    nStrParams    := N_S_PARAMS;
    nStates       := N_STATES;
    nAlgebraics   := N_ALGEBRAICS;
    // ----For nonwindup limits of the model---
    nNonWindUpLimits := N_NONWINDUPLIMITS;
  end;
end;

//******************************************************************************
function parameterName(ParamNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  // Integer parameters
  case ParamNum^ of
    IPARAM_Ics    : ts := 'Ics';
   else ts := '';
  end;

  // Double parameters
  if ts = '' then begin
    case (ParamNum^ - N_I_PARAMS) of
      PARAM_A1     : ts := 'A1';
      PARAM_A2     : ts := 'A2';
      PARAM_A3     : ts := 'A3';
      PARAM_A4     : ts := 'A4';
      PARAM_A5     : ts := 'A5';
      PARAM_A6     : ts := 'A6';
      PARAM_T1     : ts := 'T1';
      PARAM_T2     : ts := 'T2';
      PARAM_T3     : ts := 'T3';
      PARAM_T4     : ts := 'T4';
      PARAM_T5     : ts := 'T5';
      PARAM_T6     : ts := 'T6';
      PARAM_Ks     : ts := 'Ks';
      PARAM_Lsmax  : ts := 'Lsmax';
      PARAM_Lsmin  : ts := 'Lsmin';
      PARAM_Vcu    : ts := 'Vcu';
      PARAM_Vcl    : ts := 'Vcl';
      PARAM_Tdelay : ts := 'Tdelay';
     else            ts := '';
    end;
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;

//******************************************************************************
function stateName(StateNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case StateNum^ of
    STATE_Filter1         : ts := 'Filter 1';
    STATE_Filter2         : ts := 'Filter 2';
    STATE_Filter3         : ts := 'Filter 3';
    STATE_FilterOut       : ts := 'Filter Out';
    STATE_LL1             : ts := 'LL1';
    STATE_LL2             : ts := 'LL2';
    STATE_UnlimitedSignal : ts := 'Unlimited Signal';
   else ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;

//******************************************************************************
function OtherObjectClass(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'Bus';
   else ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;

//******************************************************************************
function OtherObjectDescription(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'Signal Bus';
   else ts := '';
  end;
  result := pwProvideString(StrBuf, StrSize, ts);
end;

//******************************************************************************
procedure getDefaultParameterValue(ParamsAndStates : PTxMyModelData);
var pFloatParamArr: PDoubleArray;
    pIntParamArr : PIntegerArray;
begin
  // Integers
  pIntParamArr := ParamsAndStates^.IntParams;
  pIntParamArr^[IPARAM_Ics   ] := 1;

  // Doubles
  pFloatParamArr := ParamsAndStates^.FloatParams;
  pFloatParamArr^[PARAM_A1    ] :=  1.013;
  pFloatParamArr^[PARAM_A2    ] :=  0.013;
  pFloatParamArr^[PARAM_A3    ] :=  0.0;
  pFloatParamArr^[PARAM_A4    ] :=  0.0;
  pFloatParamArr^[PARAM_A5    ] :=  1.013;
  pFloatParamArr^[PARAM_A6    ] :=  0.113;
  pFloatParamArr^[PARAM_T1    ] :=  0.0;
  pFloatParamArr^[PARAM_T2    ] :=  0.02;
  pFloatParamArr^[PARAM_T3    ] :=  0.0;
  pFloatParamArr^[PARAM_T4    ] :=  0.0;
  pFloatParamArr^[PARAM_T5    ] :=  1.65;
  pFloatParamArr^[PARAM_T6    ] :=  1.65;
  pFloatParamArr^[PARAM_Ks    ] :=  3.00;
  pFloatParamArr^[PARAM_Lsmax ] :=  0.1;
  pFloatParamArr^[PARAM_Lsmin ] := -0.1;
  pFloatParamArr^[PARAM_Vcu   ] :=  0.0;
  pFloatParamArr^[PARAM_Vcl   ] :=  0.0;
  pFloatParamArr^[PARAM_Tdelay] :=  0.0;
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
var fA1, fA2, fA3, fA4, timeStep: Double;
    pFloatParamArr: PDoubleArray;
begin
  // Return an integer value between 0 and 7
  // This function returns the EXPONENT of 2 for the number subintervals.
  // Thus, if you want 8 subintervals, then this should return 3 because 2^3 = 8
  // The maximum number of subintervals allowed is 128, so (2^7) = 128
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := ParamsAndStates.FloatParams;
  fA1     := pFloatParamArr^[PARAM_A1    ];
  fA2     := pFloatParamArr^[PARAM_A2    ];
  fA3     := pFloatParamArr^[PARAM_A3    ];
  fA4     := pFloatParamArr^[PARAM_A4    ];
  timeStep := TimeStepSeconds^;
  if ( (fA1 <> 0) and (fA1 < 2*timeStep) and (Abs(fA2) < 2*timeStep)) or
     ( (fA3 <> 0) and (fA3 < 2*timeStep) and (Abs(fA4) < 2*timeStep))
  then result := 6
  else if ( (fA1 <> 0) and (fA1 < 4*timeStep) and (Abs(fA2) < 4*timeStep)) or
          ( (fA3 <> 0) and (fA3 < 4*timeStep) and (Abs(fA4) < 4*timeStep))
  then result := 4
  else result := 0;
end;

//******************************************************************************
procedure initializeYourself(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
var local_Signal : Double;
    coef_a0, coef_a1, coef_a2, coef_a3, coef_a4 : Double;
    fIcs : integer;
    fA1, fA2, fA3, fA4, fT2, fT4 : Double;

    pIntParamArr: PIntegerArray;
    pFloatParamArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pStatesArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pIntParamArr := ParamsAndStates.IntParams;
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  fIcs    := pIntParamArr^[IPARAM_Ics];

  fA1     := pFloatParamArr^[PARAM_A1    ];
  fA2     := pFloatParamArr^[PARAM_A2    ];
  fA3     := pFloatParamArr^[PARAM_A3    ];
  fA4     := pFloatParamArr^[PARAM_A4    ];
  fT2     := pFloatParamArr^[PARAM_T2    ];
  fT4     := pFloatParamArr^[PARAM_T4    ];

  if (fIcs > 5) or (fIcs < 0) then local_Signal := 0 // error check for bad input
  else local_Signal := pHardCodedArr^[fIcs];

  // IEEEST example
  // Write out initial state vector
  pStatesArr^[STATE_Filter1        ] := local_Signal;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
  pStatesArr^[STATE_Filter2        ] := local_Signal;
  pStatesArr^[STATE_Filter3        ] := local_Signal;
  pStatesArr^[STATE_FilterOut      ] := local_Signal;
  pStatesArr^[STATE_LL1            ] := local_Signal;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
  pStatesArr^[STATE_LL2            ] := local_Signal;
  pStatesArr^[STATE_UnlimitedSignal] := 0;

  //--------------------------------------------------------------------
  // Determine which States are ignored
  //--------------------------------------------------------------------
  coef_a0 := 1;
  coef_a1 := fA1+fA3;
  coef_a2 := fA2 + fA1*fA3 + fA4;
  coef_a3 := fA3*fA2 + fA1*fA4;
  coef_a4 := fA2*fA4;

  if (coef_a4 = 0) and (coef_a3 <> 0) then begin
    pIgnoreStatesArr^[STATE_FilterOut] := true;
  end
  else if (coef_a4 = 0) and (coef_a3 = 0) and (coef_a2 <> 0) then begin
    pIgnoreStatesArr^[STATE_FilterOut] := true;
    pIgnoreStatesArr^[STATE_Filter3] := true;
  end
  else if (coef_a4 = 0) and (coef_a3 = 0) and (coef_a2 = 0) and (coef_a1 <> 0) then begin
    pIgnoreStatesArr^[STATE_FilterOut] := true;
    pIgnoreStatesArr^[STATE_Filter3] := true;
    pIgnoreStatesArr^[STATE_Filter2] := true;
  end
  else if (coef_a4 = 0) and (coef_a3 = 0) and (coef_a2 = 0) and (coef_a1 = 0) and (coef_a0 <> 0) then begin
    pIgnoreStatesArr^[STATE_FilterOut] := true;
    pIgnoreStatesArr^[STATE_Filter3] := true;
    pIgnoreStatesArr^[STATE_Filter2] := true;
    pIgnoreStatesArr^[STATE_Filter1] := true;
  end;

  if fT2 = 0 then pIgnoreStatesArr^[STATE_LL1] := true;
  if fT4 = 0 then pIgnoreStatesArr^[STATE_LL2] := true;
end;

//******************************************************************************
procedure calculateFofX(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits; dotX : PDoubleArray);
var // parameters
    fIcs : integer;
    fA1, fA2, fA3, fA4, fA5, fA6, fT1, fT2, fT3, fT4, fT5, fT6, fKs, fLsmax, fLsmin, fVcu, fVcl, fTdelay : Double;
    fInvT2, fInvT4, fInvT6 : Double;

    // The state variables,
    i : integer;
    StateFilter : array[0..3] of double;
    dotStateFilter : array[0..3] of double;
    coefA : array[0..4] of double;
    coefB : array[0..4] of double;
    divideBy : double;
    maxOrder : integer;
    tempIN : double;
    tempOUT : double;
    tempPrevState : double;

    stateLL1, stateLL2, stateUnlimitedSignal : Double;
    dotStateLL1, dotStateLL2, dotStateUnlimitedSignal : Double;
    local_Signal : Double;

    pIntParamArr: PIntegerArray;
    pFloatParamArr: PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pStatesArr : PDoubleArray;
    pDotXArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pIntParamArr := ParamsAndStates.IntParams;
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;
  pDotXArr := dotX;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //--------------------------------------------------------------------
  stateFilter[0]       := pStatesArr^[STATE_Filter1        ];
  stateFilter[1]       := pStatesArr^[STATE_Filter2        ];
  stateFilter[2]       := pStatesArr^[STATE_Filter3        ];
  stateFilter[3]       := pStatesArr^[STATE_FilterOut      ];
  stateLL1             := pStatesArr^[STATE_LL1            ];
  stateLL2             := pStatesArr^[STATE_LL2            ];
  stateUnlimitedSignal := pStatesArr^[STATE_UnlimitedSignal];

  fIcs := pIntParamArr^[IPARAM_Ics];

  fA1     := pFloatParamArr^[PARAM_A1    ];
  fA2     := pFloatParamArr^[PARAM_A2    ];
  fA3     := pFloatParamArr^[PARAM_A3    ];
  fA4     := pFloatParamArr^[PARAM_A4    ];
  fA5     := pFloatParamArr^[PARAM_A5    ];
  fA6     := pFloatParamArr^[PARAM_A6    ];
  fT1     := pFloatParamArr^[PARAM_T1    ];
  fT2     := pFloatParamArr^[PARAM_T2    ];
  fT3     := pFloatParamArr^[PARAM_T3    ];
  fT4     := pFloatParamArr^[PARAM_T4    ];
  fT5     := pFloatParamArr^[PARAM_T5    ];
  fT6     := pFloatParamArr^[PARAM_T6    ];
  fKs     := pFloatParamArr^[PARAM_Ks    ];
  fLsmax  := pFloatParamArr^[PARAM_Lsmax ];
  fLsmin  := pFloatParamArr^[PARAM_Lsmin ];
  fVcu    := pFloatParamArr^[PARAM_Vcu   ];
  fVcl    := pFloatParamArr^[PARAM_Vcl   ];
  fTdelay := pFloatParamArr^[PARAM_Tdelay];

  dotStateFilter[0] := 0;
  dotStateFilter[1] := 0;
  dotStateFilter[2] := 0;
  dotStateFilter[3] := 0;

  //  kate 1/5/12 Reading in parameters we need for the network:  inNetworkParams
  if (fIcs > 5) or (fIcs < 0) then local_Signal := 0 // error check for bad input
  else local_Signal := pHardCodedArr^[fIcs];

  //----------------------------------------------------------------------------
  // General Filter Block
  //----------------------------------------------------------------------------
  // seems that this is the way the big filter block is handled in Simulator
  // b's = numerator, a's = denominator
  //
  //           | b0 + b1*s + b2*s^2 + b3*s^3 + b4*s^4 |
  // INPUT --> |--------------------------------------| --> OUTPUT
  //           | a0 + a1*s + a2*s^2 + a3*s^3 + 1*s^4  |
  //
  // Writing these in "observable cononical form" (look in a control system theory book)

  // INPUT = localsignal
  // OUTPUT = x3; (last state is output)
  //   dotx0 =    - a0*OUTPUT + (b0 - a0*b4)*INPUT
  //   dotx1 = x0 - a1*OUTPUT + (b1 - a1*b4)*INPUT
  //   dotx2 = x1 - a2*OUTPUT + (b2 - a2*b4)*INPUT
  //   dotx3 = x2 - a3*OUTPUT + (b3 - a3*b4)*INPUT
  // Written generically for entry "k" assuming highest order is "N"
  //   dotx(k) = X(k-1) - a(k)*OUTPUT + [b(k) - a(k)*b(N)]*INPUT
  //----------------------------------------------------------------------------

  // For our example, Numerator is   ( 1 + A5*s + A6*s^2 )
  coefB[0] := 1;
  coefB[1] := fA5;
  coefB[2] := fA6;
  coefB[3] := 0;
  coefB[4] := 0;

  // Denominator is ( 1 + A1s + A2s^2) * ( 1 + A3s + A4s^2) =
  // This gives   =   1 + (A1+A3)s + (A2 + A1*A3 + A5)s^2 + (A3*A2 + A1*A4)s^3 + (A2+A4)s^4
  coefA[0] := 1;
  coefA[1] := fA1+fA3;
  coefA[2] := fA2 + fA1*fA3 + fA4;
  coefA[3] := fA3*fA2 + fA1*fA4;
  coefA[4] := fA2*fA4;

  // Must also convert coefficients so that the "a4 = 1", so write as follows
  // Must also convert so coefficent is "1" on the highest order term in denominator
  divideBy := 1;
  maxOrder := -1;
  for i := 4 downto 0 do begin
    if (coefA[i] <> 0) then begin
      divideBy := coefA[i];
      maxOrder := i;
      break; // jump OUT of the for i loop
    end;
  end;

  // Divide all coefficients so that the highest order non-zero A termi is 1.
  for i := 0 to 4 do begin
    coefA[i] := coefA[i]/divideBy;
    coefB[i] := coefB[i]/divideBy;
  end;

  // INPUT = localsignal
  // OUTPUT = x3; (last state is output)
  // dotx0 =    - a0*OUTPUT + (b0 - a0*b4)*INPUT
  // dotx1 = x0 - a1*OUTPUT + (b1 - a1*b4)*INPUT
  // dotx2 = x1 - a2*OUTPUT + (b2 - a2*b4)*INPUT

  if maxOrder > 0 then begin
    tempOUT := StateFilter[maxOrder-1];
    tempIN := local_Signal;
    for i := 0 to maxOrder-1 do begin
      if i > 0 then tempPrevState := StateFilter[i-1] else tempPrevState := 0;
      // we can ignore the terms which multiply by b4 because it's ALWAYS zero in this example
      //  dotx(k) = X(k-1) - a(k)*OUTPUT + [b(k) - a(k)*b(N)]*INPUT
      dotStateFilter[i] := tempPrevState - coefA[i]*tempOUT + coefB[i]*tempIN;
    end;
    // Make all ignored states have the same derivative
    // needed because I'm treating the OUTPUT variable as the state of this polyintegration object!
    for i := maxOrder to 3 do begin
      dotStateFilter[i] := dotStateFilter[i-1];
    end;
  end;

  //----------------------------------------------------------------------------
  // Lead-lag blocks
  //----------------------------------------------------------------------------
  //         | b0+b1s |                 | b1     b0-b1/a1 |
  //   u --> |------- | --> y           |---- + ----------|
  //         | a0+a1s |                 | a1      1+a1*s  |
  //
  // dotOUT = 1/a1*(-OUT*a0 + b0*IN + b1*dotIN);
  // Treat the STATE as the OUTPUT of the lead/lag block
  //
  // Block for State 5 --
  // a0 = 1
  // b0 = 1
  // a1 = T2
  // b1 = T1
  //
  // For State 6 --
  // a0 = 1
  // b0 = 1
  // a1 = T4
  // b1 = T3
  //
  // For State 7 --
  // a0 = 0
  // b0 = 0
  // a1 = T6
  // b1 = Ks*T5
  //----------------------------------------------------------------------------
  If fT2 = 0 then begin // treat as though fT1 is also 0.0    // State 5
    dotStateLL1 := dotStateFilter[3]; // ignored state.
  end
  else begin
    fInvT2 := 1/fT2;
    dotStateLL1 := fInvT2*(-stateLL1 + stateFilter[3] + fT1*dotStateFilter[3]);   // kate 3/18/13
  end;

  If fT4 = 0 then begin // treat as though fT3 is also 0.0     // State 6
    dotStateLL2 := dotStateLL1; // ignored state.
  end
  else begin
    fInvT4 := 1/fT4;
    dotStateLL2 := fInvT4*(-stateLL2 + stateLL1 + fT3*dotStateLL1);  // kate 3/18/13
  end;

  //----------------------------------------------------------------------------
  // Derivative block is similar math to the lead/lag block
  //----------------------------------------------------------------------------
  if fT6 = 0 then begin                                      // State 7
    dotStateUnlimitedSignal := dotStateLL2; // ignore state
  end
  else begin
    fInvT6 := 1/fT6;
    dotStateUnlimitedSignal := fInvT6*(-stateUnlimitedSignal + fKs*fT5*dotStateLL2); // kate 3/18/13

  end;

  //--------------------------------------------------------------------
  // Push the resulting derivatives to the appropriate memory
  //--------------------------------------------------------------------
  pDotXArr^[STATE_Filter1        ] := dotStateFilter[0];
  pDotXArr^[STATE_Filter2        ] := dotStateFilter[1];
  pDotXArr^[STATE_Filter3        ] := dotStateFilter[2];
  pDotXArr^[STATE_FilterOut      ] := dotStateFilter[3];
  pDotXArr^[STATE_LL1            ] := dotStateLL1;
  pDotXArr^[STATE_LL2            ] := dotStateLL2;
  pDotXArr^[STATE_UnlimitedSignal] := dotStateUnlimitedSignal;
end;

//******************************************************************************
procedure PropagateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions);
Var fIcs : integer;
    local_Signal : double;

    pIntParamArr: PIntegerArray;
    pHardCodedArr : PDoubleArray;
    pStatesArr : PDoubleArray;
    pIgnoreStatesArr : PBooleanArray;
Begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pStatesArr := ParamsAndStates.States;
  pIgnoreStatesArr := ParamsAndStates.IgnoreStates;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;

  pIntParamArr := ParamsAndStates.IntParams;
  fIcs := pIntParamArr^[IPARAM_Ics];

  if (fIcs > 5) or (fIcs < 0) then local_Signal := 0 // error check for bad input
  else local_Signal := pHardCodedArr^[fIcs];

  //--------------------------------------------------------------------
  // Propagate Ignored States
  //--------------------------------------------------------------------
  if pIgnoreStatesArr^[STATE_Filter1  ] then pStatesArr^[STATE_Filter1  ] := local_Signal;
  if pIgnoreStatesArr^[STATE_Filter2  ] then pStatesArr^[STATE_Filter2  ] := pStatesArr^[STATE_Filter1  ];
  if pIgnoreStatesArr^[STATE_Filter3  ] then pStatesArr^[STATE_Filter3  ] := pStatesArr^[STATE_Filter2  ];
  if pIgnoreStatesArr^[STATE_FilterOut] then pStatesArr^[STATE_FilterOut] := pStatesArr^[STATE_Filter3  ];
  if pIgnoreStatesArr^[STATE_LL1      ] then pStatesArr^[STATE_LL1      ] := pStatesArr^[STATE_FilterOut];
  if pIgnoreStatesArr^[STATE_LL2      ] then pStatesArr^[STATE_LL2      ] := pStatesArr^[STATE_LL1      ];
end;

//******************************************************************************
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits): Integer;
begin
  Result := 0;   // None
end;

//******************************************************************************
// Stabilizer-Specific Functions
//******************************************************************************
// At least one of StabilizerVsOut and StabilizerPitchOut is required (declared in pwUDM.Stabilizer).
// This model implements StabilizerVsOut only.
function StabilizerVsOut(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
var fLsmax, fLsmin, fVcu, fVcl : Double;
    stateUnlimitedSignal : Double;
    GenVcomp_In : Double;

    pFloatParamArr: PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pStatesArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := ParamsAndStates.FloatParams;
  pStatesArr := ParamsAndStates.States;
  pHardCodedArr := ParamsAndStates.HardCodedSignals;

  //--------------------------------------------------------------------
  // Grab States, Params, Algebraics Needed for this method
  //-------------------------------+-------------------------------------
  stateUnlimitedSignal := pStatesArr^[STATE_UnlimitedSignal];
  fLsmax   := pFloatParamArr^[PARAM_Lsmax ];
  fLsmin   := pFloatParamArr^[PARAM_Lsmin ];

  GenVcomp_In := pHardCodedArr^[HARDCODE_STAB_GenVcomp];
  fVcu     := pFloatParamArr^[PARAM_Vcu   ];
  fVcl     := pFloatParamArr^[PARAM_Vcl   ];

  //--------------------------------------------------------------------
  // Perform Calculations
  //--------------------------------------------------------------------
  Result := stateUnlimitedSignal;
  if not SystemOptions^.IgnoreLimitChecking then begin
    If result < fLsmin Then Result := fLsmin
    Else If result > fLsmax Then Result := fLSmax;
  end;

  // Check for output limiter signals.
  If      (fVCu <> 0) and (fVCu <  999) and (GenVcomp_In > fVCu) Then result := 0
  Else If (fVCl <> 0) and (fVcl > -999) and (GenVcomp_In < fVcl) Then result := 0;
end;

{ NOT defined for this object
function StabilizerPitchOut(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions) : double;
}

//******************************************************************************
// Compile-time check: each exported routine must have the signature of the DLL contract
// (pwUDM.Core and the model class unit), otherwise this unit does not compile
const
  Check_DLLVersion : TDLLVersion = DLLVersion;
  Check_modelClassName : TModelClassName = modelClassName;
  Check_allParamCounts : TAllParamCounts = allParamCounts;
  Check_parameterName : TParameterName = parameterName;
  Check_stateName : TStateName = stateName;
  Check_OtherObjectClass : TOtherObjectClass = OtherObjectClass;
  Check_OtherObjectDescription : TOtherObjectDescription = OtherObjectDescription;
  Check_getDefaultParameterValue : TGetDefaultParameterValue = getDefaultParameterValue;
//  Check_getStringParamDefaultValue : TGetStringParamDefaultValue = getStringParamDefaultValue;
//  Check_signalSelection : TSignalSelection = signalSelection;
  Check_SubIntervalPower2Exponent : TSubIntervalPower2Exponent = SubIntervalPower2Exponent;
  Check_initializeYourself : TInitializeYourself = initializeYourself;
  Check_calculateFofX : TCalculateFofX = calculateFofX;
  Check_PropagateIgnoredStateAndInput : TPropagateIgnoredStateAndInput = PropagateIgnoredStateAndInput;
  Check_getNonWindUpLimits : TGetNonWindUpLimits = getNonWindUpLimits;
  Check_StabilizerVsOut : TStabilizerVsOut = StabilizerVsOut;
//  Check_StabilizerPitchOut : TStabilizerPitchOut = StabilizerPitchOut;

end.
