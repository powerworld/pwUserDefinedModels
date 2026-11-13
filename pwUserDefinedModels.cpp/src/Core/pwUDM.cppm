// Core types, helpers and the DLL contract shared by every PowerWorld user-defined model.
export module pwUDM;

import std;

// These types should be the same for all models and model types
export namespace pwUDM {

inline constexpr int IntMax = std::numeric_limits<int>::max();

using TDoubleArray = double[IntMax / 64];
using TIntegerArray = int[IntMax / 32];
using TByteArray = unsigned char[IntMax / 8];
using TBooleanArray = bool[IntMax / 8];
using TStringParamsArray = char*[IntMax / 480];

enum class ModelClass
{
    Machine,
    Exciter,
    Governor,
    Stabilizer,
    Load,
    MTDCConvertor,
    MTDCLine
};

// Copies string (plus terminator) into buffer when it is non-empty and fits in *bufferSize.
// Always returns the length of string so the caller can retry with a larger buffer.
int pwStringCopy(wchar_t* buffer, const int* bufferSize, const wchar_t* string);

// Returns the class name string that Simulator expects for a model class.
const wchar_t* pwModelClassString(ModelClass udmClass);

struct TTxMyModelData
{
    TDoubleArray* FloatParams;
    TIntegerArray* IntParams;
    TStringParamsArray* StrParams;
    TDoubleArray* HardCodedSignals;
    TDoubleArray* States;
    TBooleanArray* IgnoreStates;
    TDoubleArray* Algebraics;
};

struct TTxSystemOptions
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

struct TTxNonWindUpLimits
{
    TIntegerArray* limitStates;
    TDoubleArray* minLimits;
    TDoubleArray* maxLimits;
    TByteArray* activeLimits;
};

struct TTxParamCounts
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
extern "C" __declspec(dllexport) void __stdcall allParamCounts(TTxParamCounts* numbersOfEverything, double* timeStepSeconds);
extern "C" __declspec(dllexport) int __stdcall parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) int __stdcall stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) int __stdcall OtherObjectClass(int* Num, int* StrSize, wchar_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) int __stdcall OtherObjectDescription(int* Num, int* StrSize, wchar_t* StrBuf, int dummy);
extern "C" __declspec(dllexport) void __stdcall getDefaultParameterValue(TTxMyModelData* ParamsAndStates);
extern "C" __declspec(dllexport) int __stdcall SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates, double* timeStepSeconds);
extern "C" __declspec(dllexport) void __stdcall initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) void __stdcall calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX);
extern "C" __declspec(dllexport) void __stdcall PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
extern "C" __declspec(dllexport) int __stdcall getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits);
extern "C" __declspec(dllexport) bool __stdcall TimeStepEnd(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, int* index, int* MaxPossibleEventIndex, double* EventTime, int* ExtraObjectIndex);
extern "C" __declspec(dllexport) int __stdcall TimeStepEndAction(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, int* index, int* StrSize, wchar_t* StrBuf, int dummy);

}
