export module pwUDM.Core;
// Core types, helpers and the DLL contract shared by every PowerWorld user-defined model.

import std;

export namespace pwUDM {

//------------------------------------------------------------------------------
// The following declaration are made just to make the code easier to read
// Allows us to access variables using an "array syntax"
// ** notes on pointer operations are from http://rvelthuis.de/articles/articles-pointers.html **
// Use this which tells the compiler it's an array and that it can access it like an array.
// It makes it easier to read and look at, but does require defining the types
// of DoubleArray as well as using a pointer to it, DoubleArray*.
// for TxMyModelData
inline constexpr int MaxInt = std::numeric_limits<int>::max();

using DoubleArray = double[MaxInt / 64];
using IntegerArray = int[MaxInt / 32];
using ByteArray = unsigned char[MaxInt / 8];
using BooleanArray = bool[MaxInt / 8];
using WideStringArray = char16_t*[MaxInt / 64];

// The kinds of user defined model; the DLL reports which one it is through modelClassName.
enum class ModelClass
{
    Exciter,
    Governor,
    Load,
    Machine,
    Stabilizer,
    MTDCConvertor,
    MTDCLine
};

// Returns the class name string that Simulator expects for a model class.
const char16_t* pwModelClassString(ModelClass modelClass);

// Provides SourceString to Simulator for the name functions that return a string.
// Simulator owns StrBuf: write into it only during this call, and never keep, free or resize it.
// Copies SourceString (plus terminator) into StrBuf when it is non-empty and fits in StrSize^.
// Always returns the length of SourceString, so Simulator can ask first with StrSize^ = 0
// (and no buffer) and then supply a buffer of that length.
int pwProvideString(char16_t* StrBuf, int* StrSize, const char16_t* SourceString);

// The records below are shared with Simulator, so they are packed to 8 bytes, like Delphi's {$A8}.
#pragma pack(push, 8)

// These types should be the same for all models and model types
struct TxMyModelData
{
    DoubleArray* FloatParams;  // Array of float parameters
    IntegerArray* IntParams;   // Array of integer parameters
    WideStringArray* StrParams;  // Array of string parameters

    DoubleArray* HardCodedSignals;  // Array of Input signals

    DoubleArray* States;  // Array of states as doubles
    BooleanArray* IgnoreStates;  // Array of booleans signifying if particular state is ignored
    DoubleArray* Algebraics;  // Array of doubles represent the algebraic values
};

struct TxSystemOptions
{
    bool IgnoreLimitChecking;
    double TimeStepSeconds;
    double SimulationTimeSeconds;
    double WBase;
    double SBase;
    double PUSolutionTolerance;
    double MinVoltSLoad;
    double MinVoltILoad;
};

struct TxNonWindUpLimits
{
    IntegerArray* LimitStates;  // Specify states by number which have non-windup limits
    DoubleArray* minLimits;  // Specify the value of the Min Limit
    DoubleArray* maxLimits;  // Specify the value of the Max Limit
    ByteArray* activeLimits;  // points to the start of a Byte Array
                              // containing information on which limits are active.
};

struct TxParamCounts
{
    // The Model
    int nFloatParams;   // N_F_PARAMS
    int nIntParams;     // N_I_PARAMS
    int nStrParams;     // N_S_PARAMS
    int nStates;        // N_STATES
    int nAlgebraics;    // N_ALGEBRAICS

    // Nonwindup Limits
    int nNonWindUpLimits; // N_NONWINDUPLIMITS
};

#pragma pack(pop)

// The layout of these records is part of the DLL contract with Simulator (64-bit).
static_assert(sizeof(TxMyModelData) == 56);
static_assert(sizeof(TxSystemOptions) == 64);
static_assert(sizeof(TxNonWindUpLimits) == 32);
static_assert(sizeof(TxParamCounts) == 24);
static_assert(alignof(TxMyModelData) == 8 && alignof(TxSystemOptions) == 8 && alignof(TxNonWindUpLimits) == 8);
static_assert(alignof(TxParamCounts) == 4);
// IgnoreLimitChecking is padded up to the 8-byte boundary of the first double, as Delphi's {$A8} does.
static_assert(__builtin_offsetof(TxSystemOptions, TimeStepSeconds) == 8);

// These functions need to be exported in the DLL. Declaring them here, with C linkage,
// makes the compiler check every model's definition against this signature.
extern "C" __declspec(dllexport) int DLLVersion();
extern "C" __declspec(dllexport) int modelClassName(int* StrSize, char16_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) void allParamCounts(TxParamCounts* numbersOfEverything, double* TimeStepSeconds);
extern "C" __declspec(dllexport) int parameterName(int* ParamNum, int* StrSize, char16_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) int stateName(int* StateNum, int* StrSize, char16_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) int OtherObjectClass(int* Num, int* StrSize, char16_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) int OtherObjectDescription(int* Num, int* StrSize, char16_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) void getDefaultParameterValue(TxMyModelData* ParamsAndStates);
extern "C" __declspec(dllexport) int SubIntervalPower2Exponent(TxMyModelData* ParamsAndStates, double* TimeStepSeconds);
extern "C" __declspec(dllexport) void initializeYourself(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) void calculateFofX(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits, DoubleArray* dotX);
extern "C" __declspec(dllexport) void PropagateIgnoredStateAndInput(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) int getNonWindUpLimits(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits);
extern "C" __declspec(dllexport) bool TimeStepEnd(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, int* index, int* MaxPossibleEventIndex, double* EventTime, int* ExtraObjectIndex);
extern "C" __declspec(dllexport) int TimeStepEndAction(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, int* index, int* StrSize, char16_t* StrBuf, int dummy);

}
