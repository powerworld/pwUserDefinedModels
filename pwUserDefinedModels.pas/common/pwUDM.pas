Unit pwUDM;

interface

//------------------------------------------------------------------------------
// These types should be the same for all models and model types
type
  TTxMyModelData = record
    FloatParams  : PDouble;  // Array of float parameters
    IntParams    : PInteger; // Array of integer parameters
    StrParams    : PPChar;   // Array of string parameters

    HardCodedSignals : PDouble;  // Array of Input signals

    States       : PDouble;  // Array of states as doubles
    IgnoreStates : PBoolean; // Array of booleans signifying if particular state is ignored
    Algebraics   : PDouble;  // Array of doubles represent the algebraic values
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
    LimitStates  : PInteger; // Specify states by number which have non-windup limits
    minLimits    : PDouble;  // Specify the value of the Min Limit
    maxLimits    : PDouble;  // Specify the value of the Max Limit
    activeLimits : PByte;    // points to the start of a Byte Array
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

  //----------------------------------------------------------------------------
  // The following declaration are made just to make the code easier to read
  // Allows us to access variablues using an "array syntax"
  // ** notes on pointer operations are from http://rvelthuis.de/articles/articles-pointers.html **
  // Use this which tells Delphi it's an array and that it can access it like an array.
  // It makes it easier to read and look at, but does require defining the types
  // of TFloatParamsArray as well as a type for its pointer PFloatParamsArray.
  // for TTxMyModelData
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

implementation

end.
