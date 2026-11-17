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
using StringParamsArray = char*[MaxInt / 480];

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
const wchar_t* pwModelClassString(ModelClass modelClass);

// Copies SourceString (plus terminator) into StrBuf when it is non-empty and fits in StrSize^.
// Always returns the length of SourceString so the caller can retry with a larger buffer.
int pwStringCopy(wchar_t* StrBuf, int* StrSize, const wchar_t* SourceString);

// These types should be the same for all models and model types
struct TxMyModelData
{
    DoubleArray* FloatParams;  // Array of float parameters
    IntegerArray* IntParams;   // Array of integer parameters
    StringParamsArray* StrParams;  // Array of string parameters

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

// These functions need to be exported in the DLL. Declaring them here, with C linkage and
// __stdcall, makes the compiler check every model's definition against this signature.
extern "C" __declspec(dllexport) int __stdcall DLLVersion();
extern "C" __declspec(dllexport) int __stdcall modelClassName(int* StrSize, wchar_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) void __stdcall allParamCounts(TxParamCounts* numbersOfEverything, double* TimeStepSeconds);
extern "C" __declspec(dllexport) int __stdcall parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) int __stdcall stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) int __stdcall OtherObjectClass(int* Num, int* StrSize, wchar_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) int __stdcall OtherObjectDescription(int* Num, int* StrSize, wchar_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) void __stdcall getDefaultParameterValue(TxMyModelData* ParamsAndStates);
extern "C" __declspec(dllexport) int __stdcall SubIntervalPower2Exponent(TxMyModelData* ParamsAndStates, double* TimeStepSeconds);
extern "C" __declspec(dllexport) void __stdcall initializeYourself(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) void __stdcall calculateFofX(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits, DoubleArray* dotX);
extern "C" __declspec(dllexport) void __stdcall PropagateIgnoredStateAndInput(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) int __stdcall getNonWindUpLimits(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits);
extern "C" __declspec(dllexport) bool __stdcall TimeStepEnd(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, int* index, int* MaxPossibleEventIndex, double* EventTime, int* ExtraObjectIndex);
extern "C" __declspec(dllexport) int __stdcall TimeStepEndAction(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, int* index, int* StrSize, wchar_t* StrBuf, int dummy);

}
