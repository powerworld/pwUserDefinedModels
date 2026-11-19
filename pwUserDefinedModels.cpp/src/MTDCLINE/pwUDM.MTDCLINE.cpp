module pwUDM.MTDCLINE;

import std;
import pwUDM.Core;
import pwUDM.MultiTerminalDC;

namespace pwUDM::MTDCLINE {

namespace {

// The number of converters the line has, limited to what this example can handle
int LOCAL_NumberOfTerminals(TxMyModelData* ParamsAndStates)
{
    int result = (*ParamsAndStates->IntParams)[IPARAM_NumTerminals];
    if (result < 1) { result = 1; }
    if (result > MAX_TERMINALS) { result = MAX_TERMINALS; }
    return(result);
}

// The place in the hard-coded signals of a signal of a converter
int LOCAL_HardCodedIndex(int Converter, int Signal)
{
    return(Converter*HARDCODE_MTDC_SignalsPerConverter + Signal);
}

}

//******************************************************************************
extern "C" __declspec(dllexport) int DLLVersion()
{
    return(1);
}

//******************************************************************************
extern "C" __declspec(dllexport) int modelClassName(int* StrSize, char16_t* StrBuf, int dummy)
{
    int result = pwProvideString(StrBuf, StrSize, pwModelClassString(ModelClass::MTDCLine));
    return result;
}

//******************************************************************************
extern "C" __declspec(dllexport) void allParamCounts(TxParamCounts* numbersOfEverything, double* TimeStepSeconds)
{
    // -------The model-specific parameters-----
    numbersOfEverything->nFloatParams = N_F_PARAMS;
    numbersOfEverything->nIntParams = N_I_PARAMS;
    numbersOfEverything->nStrParams = N_S_PARAMS;
    numbersOfEverything->nStates = N_STATES;
    numbersOfEverything->nAlgebraics = N_ALGEBRAICS;

    // ----For nonwindup limits of the model---
    numbersOfEverything->nNonWindUpLimits = N_NONWINDUPLIMITS;
}

//******************************************************************************
extern "C" __declspec(dllexport) int parameterName(int* ParamNum, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    // The integer parameters come first, then the float parameters
    switch (*ParamNum)
    {
        case IPARAM_NumTerminals:          ts = u"NumTerminals"; break;
        case N_I_PARAMS + PARAM_Tord:      ts = u"Tord"; break;
        case N_I_PARAMS + PARAM_IordScale: ts = u"IordScale"; break;
        default:                           ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}

//******************************************************************************
extern "C" __declspec(dllexport) int stateName(int* StateNum, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    switch (*StateNum)
    {
        case STATE_IdRef + 0: ts = u"Terminal 1 Id Ref"; break;
        case STATE_IdRef + 1: ts = u"Terminal 2 Id Ref"; break;
        case STATE_IdRef + 2: ts = u"Terminal 3 Id Ref"; break;
        case STATE_IdRef + 3: ts = u"Terminal 4 Id Ref"; break;
        case STATE_IdRef + 4: ts = u"Terminal 5 Id Ref"; break;
        case STATE_IdRef + 5: ts = u"Terminal 6 Id Ref"; break;
        case STATE_IdRef + 6: ts = u"Terminal 7 Id Ref"; break;
        case STATE_IdRef + 7: ts = u"Terminal 8 Id Ref"; break;
        default:              ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}

//******************************************************************************
/* non specified for this model
extern "C" __declspec(dllexport) int OtherObjectClass(int* Num, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    switch (*Num)
    {
        case 0:   ts = u"Bus"; break;
        default:  ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
/* non specified for this model
extern "C" __declspec(dllexport) int OtherObjectDescription(int* Num, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    switch (*Num)
    {
        case 0:   ts = u"Signal Bus"; break;
        default:  ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
extern "C" __declspec(dllexport) void getDefaultParameterValue(TxMyModelData* ParamsAndStates)
{
    DoubleArray* pDoubArr;
    IntegerArray* pIntArr;

    // Example MTDCLINE parameters
    pDoubArr = ParamsAndStates->FloatParams;
    (*pDoubArr)[PARAM_Tord] = 0.5;
    (*pDoubArr)[PARAM_IordScale] = 1.0;

    pIntArr = ParamsAndStates->IntParams;
    (*pIntArr)[IPARAM_NumTerminals] = 3;
}

/* non specified for this model
extern "C" __declspec(dllexport) int getStringParamDefaultValue(int* Num, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    switch (*Num)
    {
        case 0:   ts = u"DefaultString0"; break;
        default:  ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
/* non specified for this model
extern "C" __declspec(dllexport) int signalSelection(int* Num, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    ts = u""; // there are none!
    switch (*Num)
    {
        case ALG_XXX: ts = u"TSXXX"; break;
        default:  ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
extern "C" __declspec(dllexport) int SubIntervalPower2Exponent(TxMyModelData* ParamsAndStates, double* TimeStepSeconds)
{
    // Return an integer value between 0 and 7
    // This function returns the EXPONENT of 2 for the number subintervals.
    // Thus, if you want 8 subintervals, then this should return 3 because 2^3 = 8
    // The maximum number of subintervals allowed is 128, so (2^7) = 128
    return(0);
}

//******************************************************************************
extern "C" __declspec(dllexport) void initializeYourself(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    // Parameters,
    double fTord;
    int nTerminals, i;
    double local_IdRef;

    DoubleArray* pHardCodedArr;
    DoubleArray* pFloatParamArr;
    DoubleArray* pStatesArr;
    DoubleArray* pAlgebraicsArr;
    BooleanArray* pIgnoreStatesArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pFloatParamArr = ParamsAndStates->FloatParams;
    pStatesArr = ParamsAndStates->States;
    pAlgebraicsArr = ParamsAndStates->Algebraics;
    pIgnoreStatesArr = ParamsAndStates->IgnoreStates;

    //--------------------------------------------------------------------
    // Grab States, Params, Algebraics Needed for this method
    //--------------------------------------------------------------------
    fTord = (*pFloatParamArr)[PARAM_Tord];
    nTerminals = LOCAL_NumberOfTerminals(ParamsAndStates);

    for (i = 0; i <= MAX_TERMINALS - 1; ++i)
    {
        if (i < nTerminals)
        {
            // The converters are initialized before the line, so each already has its initial current order.
            // Each converter keeps its initial share of the pole current order.
            local_IdRef = (*pHardCodedArr)[LOCAL_HardCodedIndex(i, HARDCODE_MTDC_CurrentReference)];

            // Ignored States
            // A time constant of zero means the current order follows its target directly
            if (fTord == 0) { (*pIgnoreStatesArr)[STATE_IdRef + i] = 1; }
        }
        else
        {
            // This terminal does not exist
            local_IdRef = 0;
            (*pIgnoreStatesArr)[STATE_IdRef + i] = 1;
        }

        (*pAlgebraicsArr)[ALG_IdRefInit + i] = local_IdRef;

        // Write out initial state vector
        (*pStatesArr)[STATE_IdRef + i] = local_IdRef;
    }
}

//******************************************************************************
extern "C" __declspec(dllexport) void calculateFofX(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits, DoubleArray* dotX)
{
    // Parameters
    double fTord, fIordScale;
    double fInvTord;
    int nTerminals, i;

    // State variables
    double stateIdRef;

    // FofX or xdot,
    double dotIdRef;

    DoubleArray* pFloatParamArr;
    DoubleArray* pStatesArr;
    DoubleArray* pAlgebraicsArr;
    DoubleArray* pDotXArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pFloatParamArr = ParamsAndStates->FloatParams;
    pStatesArr = ParamsAndStates->States;
    pAlgebraicsArr = ParamsAndStates->Algebraics;
    pDotXArr = dotX;

    //--------------------------------------------------------------------
    // Grab States, Params, Algebraics Needed for this method
    //--------------------------------------------------------------------
    fTord = (*pFloatParamArr)[PARAM_Tord];
    fIordScale = (*pFloatParamArr)[PARAM_IordScale];
    nTerminals = LOCAL_NumberOfTerminals(ParamsAndStates);

    // Calculate Inverse of appropriate values
    if (fTord == 0) { fInvTord = 0; } else { fInvTord = 1/fTord; }

    for (i = 0; i <= MAX_TERMINALS - 1; ++i)
    {
        // The current order of each terminal follows its initial share of the scaled pole current order
        if (i < nTerminals)
        {
            stateIdRef = (*pStatesArr)[STATE_IdRef + i];
            dotIdRef = fInvTord*(fIordScale*(*pAlgebraicsArr)[ALG_IdRefInit + i] - stateIdRef);
        }
        else { dotIdRef = 0; }

        //--------------------------------------------------------------------
        // Push the resulting derivatives to the appropriate memory
        //--------------------------------------------------------------------
        (*pDotXArr)[STATE_IdRef + i] = dotIdRef;
    }
}

//******************************************************************************
extern "C" __declspec(dllexport) void PropagateIgnoredStateAndInput(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    double fIordScale;
    int nTerminals, i;

    DoubleArray* pFloatParamArr;
    DoubleArray* pStatesArr;
    DoubleArray* pAlgebraicsArr;
    BooleanArray* pIgnoreStatesArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pFloatParamArr = ParamsAndStates->FloatParams;
    pStatesArr = ParamsAndStates->States;
    pAlgebraicsArr = ParamsAndStates->Algebraics;
    pIgnoreStatesArr = ParamsAndStates->IgnoreStates;

    fIordScale = (*pFloatParamArr)[PARAM_IordScale];
    nTerminals = LOCAL_NumberOfTerminals(ParamsAndStates);

    for (i = 0; i <= MAX_TERMINALS - 1; ++i)
    {
        // An ignored current order is just its target
        if ((*pIgnoreStatesArr)[STATE_IdRef + i])
        {
            if (i < nTerminals) { (*pStatesArr)[STATE_IdRef + i] = fIordScale*(*pAlgebraicsArr)[ALG_IdRefInit + i]; }
            else { (*pStatesArr)[STATE_IdRef + i] = 0; }
        }
    }
}

//******************************************************************************
extern "C" __declspec(dllexport) int getNonWindUpLimits(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits)
{
    return(0);   // no state has a non windup limit
}

//******************************************************************************
extern "C" __declspec(dllexport) double MultiTerminalDCGetIDRef(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, int* Converter)
{
    double fTord, fIordScale;
    int nTerminals;
    DoubleArray* pFloatParamArr;
    DoubleArray* pStatesArr;
    DoubleArray* pAlgebraicsArr;

    pFloatParamArr = ParamsAndStates->FloatParams;
    pStatesArr = ParamsAndStates->States;
    pAlgebraicsArr = ParamsAndStates->Algebraics;

    fTord = (*pFloatParamArr)[PARAM_Tord];
    fIordScale = (*pFloatParamArr)[PARAM_IordScale];
    nTerminals = LOCAL_NumberOfTerminals(ParamsAndStates);

    //--------------------------------------------------------------------
    // Calculate the current order of the converter. *Converter counts from 0.
    //--------------------------------------------------------------------
    if ((*Converter < 0) || (*Converter >= nTerminals)) { return(0); }
    else if (fTord == 0) { return(fIordScale*(*pAlgebraicsArr)[ALG_IdRefInit + *Converter]); }
    else { return((*pStatesArr)[STATE_IdRef + *Converter]); }
}

}
