unit pwUDM.Core;
// Core types, helpers and the DLL contract shared by every PowerWorld user-defined model.

interface

//------------------------------------------------------------------------------
// The following declaration are made just to make the code easier to read
// Allows us to access variablues using an "array syntax"
// ** notes on pointer operations are from http://rvelthuis.de/articles/articles-pointers.html **
// Use this which tells Delphi it's an array and that it can access it like an array.
// It makes it easier to read and look at, but does require defining the types
// of TFloatParamsArray as well as a type for its pointer PFloatParamsArray.
// for TTxMyModelData
type
  TDoubleArray       = array[0..MaxInt div 64] of Double;
  PDoubleArray       = ^TDoubleArray;
  TIntegerArray      = array[0..MaxInt div 32] of Integer;
  PIntegerArray      = ^TIntegerArray;
  TByteArray         = array[0..MaxInt div 8] of Byte;
  PByteArray         = ^TByteArray;
  TBooleanArray      = array[0..MaxInt div 8] of Boolean;
  PBooleanArray      = ^TBooleanArray;
  TStringParamsArray = array[0..MaxInt div 480] of string;
  PStringParamsArray = ^TStringParamsArray;
  //----------------------------------------------------------------------------

type
  // The kinds of user defined model; the DLL reports which one it is through modelClassName.
  TModelClass = (mcExciter, mcGovernor, mcLoad, mcMachine, mcStabilizer, mcMTDCConvertor, mcMTDCLine);

// Returns the class name string that Simulator expects for a model class.
function pwModelClassString(modelClass : TModelClass) : PChar;

// Copies SourceString (plus terminator) into StrBuf when it is non-empty and fits in StrSize^.
// Always returns the length of SourceString so the caller can retry with a larger buffer.
function pwStringCopy(StrBuf : PChar; StrSize : PInteger; SourceString : PChar) : Integer;

//------------------------------------------------------------------------------
// These types should be the same for all models and model types
type
  TTxMyModelData = record
    FloatParams  : PDoubleArray;  // Array of float parameters
    IntParams    : PIntegerArray; // Array of integer parameters
    StrParams    : PStringParamsArray; // Array of string parameters

    HardCodedSignals : PDoubleArray;  // Array of Input signals

    States       : PDoubleArray;  // Array of states as doubles
    IgnoreStates : PBooleanArray; // Array of booleans signifying if particular state is ignored
    Algebraics   : PDoubleArray;  // Array of doubles represent the algebraic values
  end;
  PTxMyModelData = ^TTxMyModelData;

  TTxSystemOptions = record
    IgnoreLimitChecking   : boolean;
    TimeStepSeconds       : double;
    SimulationTimeSeconds : double;
    WBase                 : double;
    SBase                 : double;
    PUSolutionTolerance   : double;
    MinVoltSLoad          : double;
    MinVoltILoad          : double;
  end;
  PTxSystemOptions = ^TTxSystemOptions;

  TTxNonWindUpLimits = record
    LimitStates  : PIntegerArray; // Specify states by number which have non-windup limits
    minLimits    : PDoubleArray;  // Specify the value of the Min Limit
    maxLimits    : PDoubleArray;  // Specify the value of the Max Limit
    activeLimits : PByteArray;    // points to the start of a Byte Array
                                  // containing information on which limits are active.
  end;
  PTxNonWindUpLimits = ^TTxNonWindupLimits;

  TTxParamCounts = record
    // The Model
    nFloatParams  : Integer; // N_F_PARAMS
    nIntParams    : Integer; // N_I_PARAMS
    nStrParams    : Integer; // N_S_PARAMS
    nStates       : Integer; // N_STATES
    nAlgebraics   : Integer; // N_ALGEBRAICS

    // Nonwindup Limits
    nNonWindUpLimits : Integer; // N_NONWINDUPLIMITS
  end;
  PTxParamCounts = ^TTxParamCounts;


//------------------------------------------------------------------------------
// The DLL contract: the signature of each routine a model exports. Each model unit checks
// its routines against these types at compile time (see the end of its implementation section).
type
  TDLLVersion = function: Integer; stdcall;
  TModelClassName = function(StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer; stdcall;
  TAllParamCounts = procedure(numbersOfEverything : PTxParamCounts; TimeStepSeconds : PDouble); stdcall;
  TParameterName = function(ParamNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
  TStateName = function(StateNum: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
  TOtherObjectClass = function(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
  TOtherObjectDescription = function(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
  TGetDefaultParameterValue = procedure(ParamsAndStates : PTxMyModelData); stdcall;
  TGetStringParamDefaultValue = function(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;
  TSignalSelection = function(Num: PInteger; StrSize: PInteger; StrBuf: PChar; dummy: Integer): Integer; stdcall;
  TSubIntervalPower2Exponent = function(ParamsAndStates : PTxMyModelData; TimeStepSeconds : PDouble) : integer; stdcall;
  TInitializeYourself = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions); stdcall;
  TCalculateFofX = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits; dotX : PDoubleArray); stdcall;
  TPropagateIgnoredStateAndInput = procedure(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions); stdcall;
  TGetNonWindUpLimits = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; nonWindUpLimits : PTxNonWindUpLimits): Integer; stdcall;
  TTimeStepEnd = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; index : PInteger; MaxPossibleEventIndex : PInteger; EventTime : PDouble; ExtraObjectIndex : PInteger) : boolean; stdcall;
  TTimeStepEndAction = function(ParamsAndStates : PTxMyModelData; SystemOptions : PTxSystemOptions; index : PInteger; StrSize: PInteger; StrBuf: PChar; dummy : Integer): Integer; stdcall;

implementation

uses
  System.SysUtils;

function pwModelClassString(modelClass : TModelClass) : PChar;
begin
  case modelClass of
    mcExciter       : Result := 'UserDefinedExciter';
    mcGovernor      : Result := 'UserDefinedGovernor';
    mcLoad          : Result := 'UserDefinedLoadModel';
    mcMachine       : Result := 'UserDefinedMachineModel';
    mcStabilizer    : Result := 'UserDefinedStabilizer';
    mcMTDCConvertor : Result := 'UserDefinedMTDCConverter';
    mcMTDCLine      : Result := 'UserDefinedMultiTerminalDC';
    // Add here in the future
  else Result := '';
  end;
end;

function pwStringCopy(StrBuf : PChar; StrSize : PInteger; SourceString : PChar) : Integer;
begin
  result := length(SourceString);
  if (result > 0) and (StrSize^ >= Result) then strcopy(StrBuf, SourceString);
end;

end.
