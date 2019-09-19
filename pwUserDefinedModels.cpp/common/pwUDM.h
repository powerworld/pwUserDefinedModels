#ifndef PW_UDM
#define PW_UDM

#include <limits>

#define PW_UDM_DllExport(returnValue) extern "C" __declspec(dllexport) returnValue __stdcall

// These types should be the same for all models and model types

typedef double TDoubleArray[INT_MAX / 64];
typedef int TIntegerArray[INT_MAX / 32];
typedef unsigned __int8 TByteArray [INT_MAX / 8];
typedef bool TBooleanArray[INT_MAX / 8];
typedef char* TStringParamsArray[INT_MAX / 480];

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

struct TTxParamCounts {
    // The Model
    int nFloatParams; // N_F_PARAMS
    int nIntParams; // N_I_PARAMS
    int nStrParams; // N_S_PARAMS
    int nStates; // N_STATES
    int nAlgebraics; // N_ALGEBRAICS
    // Nonwindup Limits
    int    nNonWindUpLimits; // N_NONWINDUPLIMITS
};

// These functions need to be exported in the DLL
PW_UDM_DllExport(int) DLLVersion();
PW_UDM_DllExport(int) modelClassName(int* StrSize, wchar_t* StrBuf, int dummy);
PW_UDM_DllExport(void) allParamCounts(TTxParamCounts* numbersOfEverything, double* timeStepSeconds);
PW_UDM_DllExport(int) parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy);
PW_UDM_DllExport(int) stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy);
PW_UDM_DllExport(int) OtherObjectClass(int* Num, int* StrSize, wchar_t* StrBuf, int dummy);
PW_UDM_DllExport(int) OtherObjectDescription(int* Num, int* StrSize, wchar_t* StrBuf, int dummy);
PW_UDM_DllExport(void) getDefaultParameterValue(TTxMyModelData* ParamsAndStates);
PW_UDM_DllExport(int) SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates, double* timeStepSeconds);
PW_UDM_DllExport(void) initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
PW_UDM_DllExport(void) calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX);
PW_UDM_DllExport(void) PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
PW_UDM_DllExport(int) getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits);
PW_UDM_DllExport(bool) TimeStepEnd(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, int* index, int* MaxPossibleEventIndex, double* EventTime, int* ExtraObjectIndex);
PW_UDM_DllExport(int) TimeStepEndAction(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, int* index, int* StrSize, wchar_t* StrBuf, int dummy);

#endif // PW_UDM